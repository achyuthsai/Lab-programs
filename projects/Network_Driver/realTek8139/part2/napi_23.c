/**
 * napi_23.c : NIC Driver for 2.6.23
 *
 * A RealTek RTL-8139 Fast Ethernet driver for Linux
 * with introducing advanced interrupt handler 
 * " New API (NAPI)"
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/compiler.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/rtnetlink.h>
#include <linux/delay.h>
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/completion.h>
#include <linux/crc32.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/irq.h>

#define DRV_NAME "A RealTek RTL-8139 Fast Ethernet Driver"
#define VERSION  "0.9.28"
#define DRIVER_NAME   DRV_NAME " of Version : " VERSION

/*========================================================================
 *              	Tx/Rxring buffers & msg_enable stuff
 *========================================================================*/

#if defined(CONFIG_SH_DREAMCAST)
#define RX_BUF_IDX      1       /* 16K ring */
#else
#define RX_BUF_IDX      2       /* 32K ring */
#endif
#define RX_BUF_LEN      (8192 << RX_BUF_IDX)

#define RX_BUF_PAD      16
#define RX_BUF_WRAP_PAD 2048 /* spare padding to handle lack of packet wrap */

#if RX_BUF_LEN == 65536
#define RX_BUF_TOT_LEN  RX_BUF_LEN
#else
#define RX_BUF_TOT_LEN  (RX_BUF_LEN + RX_BUF_PAD + RX_BUF_WRAP_PAD)
#endif
/* Number of Tx descriptor registers. */
#define NUM_TX_DESC     4

/* max supported ethernet frame size -- must be at least (dev->mtu+14+4).*/
#define MAX_ETH_FRAME_SIZE      1536

/* Size of the Tx bounce buffers -- must be at least (dev->mtu+14+4). */
#define TX_BUF_SIZE     MAX_ETH_FRAME_SIZE
#define TX_BUF_TOT_LEN  (TX_BUF_SIZE * NUM_TX_DESC)

/* PCI Tuning Parameters
   Threshold is bytes transferred to chip before transmission starts. */
#define TX_FIFO_THRESH 256      /* In bytes, rounded down to 32 byte units. */

/* The following settings are log_2(bytes)-4:  0 == 16 bytes .. 6==1024, 7==end of packet. */
#define RX_FIFO_THRESH  7       /* Rx buffer level before first PCI xfer.  */
#define RX_DMA_BURST    7       /* Maximum PCI burst, '6' is 1024 */
#define TX_DMA_BURST    6       /* Maximum PCI burst, '6' is 1024 */
#define TX_RETRY        8       /* 0-15.  retries = 16 + (TX_RETRY * 16) */


/* Default Message level */
#define RTL8139_DEF_MSG_ENABLE  (NETIF_MSG_DRV          | \
                                 NETIF_MSG_PROBE        | \
                                 NETIF_MSG_LINK         | \
                                 NETIF_MSG_RX_STATUS    | \
                                 NETIF_MSG_TX_QUEUED)
/* bitmapped message enable number */
static int debug = -1;

/*========================================================================
 *              pci device id table stuff
 *========================================================================*/

typedef enum {
      RTL8139 = 0,
} board_t;

static struct pci_device_id nic_pci_idtable[] = {
        {0x10ec, 0x8139, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0,}
};

/*========================================================================
 *              device spec bit registers stuff
 *========================================================================*/

/* symbolic offsets to reg */
enum nic_registers {
        MAC0 		= 0,            /* Ethernet hardware address. */
        TxStatus0 	= 0x10,       	/* Transmit status (Four 32bit registers). */
        TxAddr0 	= 0x20,         /* Tx descriptors (also four 32bit). */
        RxBuf 		= 0x30,
        ChipCmd 	= 0x37,
        RxBufPtr 	= 0x38,
        IMR 		= 0x3C,
        ISR 		= 0x3E,
        TxConfig 	= 0x40,
        RxConfig 	= 0x44,
        RxMissed 	= 0x4C,         /* 24 bits valid, write clears. */
        Cfg9346 	= 0x50,
	Config3         = 0x59,

};

/* config3 bit reg */
enum Config3Bits {
        Cfg3_LinkUp     = (1 << 4), 	/* 1 = wake up on link up */
        Cfg3_Magic      = (1 << 5), 	/* 1 = wake up on Magic Packet (tm) */
};

/* config 9346 bit reg */
enum Cfg9346Bits {
        Cfg9346_Lock 	= 0x00,
        Cfg9346_Unlock 	= 0xC0,
};

/* rx mode bit reg */
enum rx_mode_bits {
        AcceptBroadcast = 0x08,
        AcceptMyPhys 	= 0x02,
};

/* chip cmd bit reg */
enum ChipCmdBits {
        CmdReset 	= 0x10,
        CmdRxEnb 	= 0x08,
        CmdTxEnb 	= 0x04,
        RxBufEmpty 	= 0x01,
};

/* interrupt status bit reg */
enum ISRBits {
        PCIErr 		= 0x8000,
        PCSTimeout 	= 0x4000,
        RxFIFOOver 	= 0x40,
        RxUnderrun 	= 0x20,
        RxOverflow 	= 0x10,
        TxErr 		= 0x08,
        TxOK 		= 0x04,
        RxErr 		= 0x02,
        RxOK 		= 0x01,
        RxAckBits 	= RxFIFOOver | RxOverflow | RxOK,
};

/* tx status bit reg */
enum TxStatusBits {
        TxUnderrun 	= 0x4000,
        TxStatOK 	= 0x8000,
        TxOutOfWindow 	= 0x20000000,
        TxAborted 	= 0x40000000,
        TxCarrierLost 	= 0x80000000,
};

/* rx status bit reg */
enum RxStatusBits {
        RxStatusOK 	= 0x0001,
};

/* tx config bit reg */
enum TxConfigBits {
        /* Interframe Gap Time. Only TxIFG96 doesn't violate IEEE 802.3 */
        TxIFGShift 	= 24,
        TxIFG96 	= (3 << TxIFGShift),    /* 9.6us / 960ns (10 / 100Mbps) */
        TxClearAbt 	= (1 << 0),  /* Clear abort (WO) */
        TxDMAShift 	= 8,         /* DMA burst value (0-7) is shifted this many bits */
        TxRetryShift 	= 4,       /* TXRR value (0-15) is shifted this many bits */
};

/* rx config bit reg */
enum RxConfigBits {
        /* rx fifo threshold */
        RxCfgFIFOShift 	= 13,
        /* Max DMA burst */
        RxCfgDMAShift 	= 8,
        /* rx ring buffer length */
        RxCfgRcv32K 	= (1 << 12),
        /* Disable packet wrap at end of Rx buffer. (not possible with 64k) */
        RxNoWrap 	= (1 << 7),
};

/* rx config bit reg */
static const unsigned int nic_rx_config =
        RxCfgRcv32K | RxNoWrap |
        (RX_FIFO_THRESH << RxCfgFIFOShift) |
        (RX_DMA_BURST << RxCfgDMAShift);

/* tx config bit reg */
static const unsigned int nic_tx_config =
        TxIFG96 | (TX_DMA_BURST << TxDMAShift) | (TX_RETRY << TxRetryShift);

/* interrupt mask bit reg */
static const u16 nic_intr_mask =
        PCIErr | PCSTimeout | RxUnderrun | RxOverflow | RxFIFOOver |
        TxErr | TxOK | RxErr | RxOK;

/*========================================================================
 *              device spec private struct
 *========================================================================*/

struct nic_private {
	struct pci_dev          	*pdev;			/* pci device */
        void __iomem 			*iobase;		/* pci dev addr */
        struct net_device 		*dev;			/* net device */
        struct net_device_stats 	stats;			/* net status */
        int 				irq;			/* irq num */
	unsigned char 			*rx_ring;		/* rx ring (page+off) */
        unsigned int 			cur_rx;  		/* curr pkt off in rxring */
        unsigned int 			tx_flag;		/* tx flag */
        unsigned long 			cur_tx;			/* curr pkt off in txring */
        unsigned long 			dirty_tx;		/* dirty pkt off in txring */
        unsigned char 			*tx_buf[NUM_TX_DESC];   /* Tx bounce buffers */
        unsigned char 			*tx_bufs; 		/* tx ring (page+off) */
        dma_addr_t 			rx_ring_dma;		/* rx ring (bus+off) */	
        dma_addr_t 			tx_bufs_dma;		/* tx ring (bus+off) */
        spinlock_t 			lock;			/* tx lock */
        spinlock_t 			rx_lock;		/* rx lock */
        u32 				rx_config;		/* rx config bit */
        unsigned int 			regs_len;		/* reg length */
	u32 				msg_enable;		/* msg enabled bit */
        unsigned long 			fifo_copy_timeout;	/* FIFO time out in tx ring */	
};

/* write MMIO register, with flush */
#define NAPI_WRITEL_FLAG(reg, vall) do {                        \
                        writel ((vall), np->iobase + (reg));    \
                        readl (np->iobase + (reg));             \
                        } while (0);
#define NAPI_WRITEW_FLAG(reg, valw) do {                        \
                        writew ((valw), np->iobase + (reg));    \
                        readw (np->iobase + (reg));             \
                        } while (0);
#define NAPI_WRITEB_FLAG(reg, valb) do {                        \
                        writeb ((valb), np->iobase + (reg));    \
                        readb (np->iobase + (reg));             \
                        } while (0);

/*========================================================================
 *		ethtool stuff
 *======================================================================*/

/* func declerations */	
static void nic_get_drvinfo(struct net_device *dev,struct ethtool_drvinfo *info);
static int nic_get_regs_len(struct net_device *dev);
static void nic_get_regs(struct net_device *dev, 
			struct ethtool_regs *regs, void *regbuf);
static void nic_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol);
static void  nic_get_ringparam(struct net_device *dev, 
		struct ethtool_ringparam  *ering);
static int nic_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol);
static u32 nic_get_link(struct net_device *dev);

/* ethtool ops */
static struct ethtool_ops nic_ethtool_ops = {
        .get_drvinfo            = nic_get_drvinfo,
        .get_regs_len           = nic_get_regs_len,
        .get_regs               = nic_get_regs,
        .get_wol                = nic_get_wol,
        .set_wol                = nic_set_wol,
        .get_ringparam          = nic_get_ringparam,
        .get_link               = nic_get_link,
};

/* report driver information*/
static void nic_get_drvinfo(struct net_device *dev,struct ethtool_drvinfo *info)
{
        struct nic_private *np = netdev_priv(dev);

        strcpy(info->driver,  DRV_NAME);
        strcpy(info->version, VERSION);
        strcpy(info->bus_info, pci_name(np->pdev));
        info->regdump_len = np->regs_len;
}

/* get length of the registers */
static int nic_get_regs_len(struct net_device *dev)
{
        struct nic_private *np = netdev_priv(dev);
		return np->regs_len;
}

/* get device registers*/
static void nic_get_regs(struct net_device *dev, 
			struct ethtool_regs *regs, void *regbuf)
{
        struct nic_private *np = netdev_priv(dev);

        memcpy_fromio(regbuf, np->iobase, regs->len);
}

/* report whether Wake-on-Lan is enabled*/
static void nic_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{

        struct nic_private *np = netdev_priv(dev);

        u8 cfg3 = readb(np->iobase+Config3); /*read config reg 3*/

        wol->supported = WAKE_PHY | WAKE_MAGIC
                | WAKE_UCAST | WAKE_MCAST | WAKE_BCAST;

        wol->wolopts = 0;

        if (cfg3 & Cfg3_LinkUp)
                wol->wolopts |= WAKE_PHY;

        if (cfg3 & Cfg3_Magic)
                wol->wolopts |= WAKE_MAGIC;

}

/* report ring sizes */
static void  nic_get_ringparam(struct net_device *dev, 
		struct ethtool_ringparam  *ering)
{
        ering->rx_max_pending = RX_BUF_TOT_LEN - 1;
        ering->rx_mini_max_pending = 0;

}

/* set the ethtool Wake-on-LAN settings. */
static int nic_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
        struct nic_private *np = netdev_priv(dev);
        u8 cfg3;

        cfg3 = readb(np->iobase+Config3); /*read config reg3*/

        if (wol->wolopts & WAKE_PHY)
                cfg3 |= Cfg3_LinkUp;
        if (wol->wolopts & WAKE_MAGIC)
                cfg3 |= Cfg3_Magic;

        writeb(cfg3,np->iobase+Config3);

        return 0;
}

/*get link status*/
static u32 nic_get_link(struct net_device *dev)
{
        return netif_carrier_ok(dev) ? 1 : 0;
}


/*========================================================================
 *              net_device interface stuff
 *========================================================================*/

/* func declerations */
static int nic_up (struct net_device *dev);
static int nic_poll (struct net_device *dev, int *budget);
static int nic_start_xmit (struct sk_buff *skb, struct net_device *dev);
static irqreturn_t nic_interrupt (int irq, void *dev_instance);
static int nic_down (struct net_device *dev);
static struct net_device_stats *nic_get_stats (struct net_device *dev);

/* reset the controller b/f program */
static void nic_reset_controller(struct net_device *dev)
{
        struct nic_private 		*np = netdev_priv(dev);

        /* reset the controller */
        writeb(CmdReset, np->iobase+ChipCmd);

        /* wait till reset is over ;returns if finishes */
        while(readb(np->iobase+ChipCmd ) & CmdReset);
}

/* program the controller */
static void nic_controller_start (struct net_device *dev)
{
        struct nic_private 		*np = netdev_priv(dev);
	int				i;
	
	/* reset the controller b/f program */
	nic_reset_controller (dev);

        /* unlock Config[01234] and BMCR register writes */
	NAPI_WRITEB_FLAG (Cfg9346, Cfg9346_Unlock);
	
        	/* enables the command reg for Tx & Rx is ready */
        	writeb(CmdRxEnb|CmdTxEnb,np->iobase+ChipCmd);

 		np->rx_config = nic_rx_config | AcceptBroadcast | AcceptMyPhys;
        	writel(np->rx_config, np->iobase+RxConfig);
	        writel(nic_tx_config, np->iobase+TxConfig);
        	np->cur_rx = 0;
	
	/* lock Config[01234] and BMCR register writes */
        writeb(Cfg9346_Lock, np->iobase+Cfg9346);
        
		/* assign Txring addr  -->  Txring register of a chip
         	 *        Rxring addr  -->  Rxring register of a chip
         	 */
		NAPI_WRITEL_FLAG (RxBuf, np->rx_ring_dma);
 	        for (i = 0; i < NUM_TX_DESC; i++)
			NAPI_WRITEL_FLAG (TxAddr0 + (i * 4),
                                np->tx_bufs_dma + (np->tx_buf[i] - np->tx_bufs));


  	        /* enables the interrupts */
        	writeb(0xff,np->iobase+ISR);    	      // clear ISR first
        	writew(nic_intr_mask,np->iobase+IMR);    // now enables ISR
}

/* initialize the Rx/Tx rings, along with various 'dev' bits. */
static void nic_init_ring (struct net_device *dev)
{
        struct nic_private 		*np = netdev_priv(dev);
        int 				i;

        np->cur_rx 	= 0;
        np->cur_tx 	= 0;
        np->dirty_tx 	= 0;

        for (i = 0; i < NUM_TX_DESC; i++)
                np->tx_buf[i] = &np->tx_bufs[i * TX_BUF_SIZE];
}

/**
 * nic_up - up a net-interface
 *
 * nic_up get's invoked
 * when the ifconfig is used to enable the net interface
 *
 *        ifconfig eth0 up
 *               |
 *               |
 *               V
 *            nic_up()
 *
 * Resposibilites:
 *      register ISR
 *      allocates Tx/RXrings & init
 *      enables device queue
 *	program the controller
 */
static int nic_up (struct net_device *dev)
{
        struct nic_private 		*np = netdev_priv(dev);
	int 				rc;

	printk(KERN_INFO "\nUp a net-interface.. \n");
	
	/* register ISR */
        rc = request_irq (np->irq, nic_interrupt, 
				IRQF_SHARED, dev->name, (void *)dev);
        if (rc)
                return rc;

	/* allocates Rxring buf for received packets */
        np->rx_ring = pci_alloc_consistent(np->pdev, 
					RX_BUF_TOT_LEN,
                                        &np->rx_ring_dma);
	if (!np->rx_ring) {
                printk(KERN_ERR "Could not allocate DMA memory.\n");
                return -ENOMEM;
        }

	/* allocates Txring buf for transmitted packets */
        np->tx_bufs = pci_alloc_consistent(np->pdev, 
					TX_BUF_TOT_LEN,
                                        &np->tx_bufs_dma);	
	 if(!np->tx_bufs){
                printk(KERN_ERR "Could not allocate DMA memory.\n");
                return -ENOMEM;
        }
        np->tx_flag = (TX_FIFO_THRESH << 11) & 0x003f0000;

	/* initializes the Tx/Rx rings along with verious dev bits  */
	nic_init_ring (dev);
	
	/*============================================================
         * after creating dma buf's
         * tells to dev this is my buffer location
         * so; we can copy these buff addr into relevent register
         * by program the controller
         * and eanbles interrupts
         * ===========================================================*/
	
	/* program the controller */
	nic_controller_start (dev);

	/* enables the device queue */	
        netif_start_queue (dev);
	 
	return 0;
}

/**
 * nic_isr_ack - get the ISR ack for rx pkts
 *
 * calls after poll processing of rx packets..and verifies
 * any rx errors..if found updates that into priv structs
 *
 */
static void nic_isr_ack(struct nic_private *np)
{
        u16 				status;

	/* get the rx pkt status */
        status = readw (np->iobase + ISR) & RxAckBits;

        /* Clear out errors and receive interrupts */
        if (likely(status != 0)) {
                if (unlikely(status & (RxFIFOOver | RxOverflow))) {
                        np->stats.rx_errors++;
                        if (status & RxFIFOOver)
                                np->stats.rx_fifo_errors++;
                }
		NAPI_WRITEW_FLAG (ISR, RxAckBits);
        }
}
/**
 * nic_rx - poll processing
 *
 * take the packets from rxring & processing by copying
 * packets into skbuf..and gives to protocal layer...
 *
 * Responsibilities :
 * 	-  get the rxring ptr & it's offset & size
 * 	-  get the various status details
 * 	-  allocates sk_buf obj
 * 	-  copy packet from rxring into sk_buff   
 * 	-  add data to a sk_buff
 * 	-  set protocol field of sk_buff
 * 	-  set rx-packet status deatils in priv
 * 	-  gives sk_buff into kernel
 * 	-  calls nic_isr_ack
 */
static int nic_rx(struct net_device *dev, struct nic_private *np,
                      int budget)
{
        unsigned int 			rx_size  = 0;
        int 				received = 0;
	
	/* get the pointer to RxRing */
        unsigned char 			*rx_ring = np->rx_ring;

	/* get the current packet offset of RxRing */
        unsigned int 			cur_rx   = np->cur_rx;

	printk(KERN_INFO "\npoll processing..\n");
        
	/*==============================================================
         * Now the driver should run in a loop and take the each packet
         * from the RxRing .
         * The loop should run until three conditions are satisfied .
         * Three conditions are
         *          network interface should be up,
         *          received packets should be less than the 'budget'
         *          the RxRing is not empty.
         *==============================================================*/
        while (netif_running(dev) && received < budget
               && (readb (np->iobase+ChipCmd) & RxBufEmpty) == 0) {

	/*================================================================
         * A received packet is put into a structure struct sk_buff
         * called socket buffer. This structure is the main encapsulation
         * of a packet and contains pointers to point different layer headers
         * in the packet and pointers to input ,output interfaces
         * Driver has to take the packet from the RxRing ,
         * copy that into sk_buff object and give it to the kernel
         *================================================================*/

                u32 			ring_offset = cur_rx % RX_BUF_LEN;
                u32 			rx_status;
                unsigned int 		pkt_size;
                struct sk_buff 		*skb;
                rmb();

                /* get the packet header and find packet size
                 * read size+status of next frame from DMA ring buffer
                 */
                rx_status 	= le32_to_cpu (*(u32 *) (rx_ring + ring_offset));
                rx_size 	= rx_status >> 16;
                pkt_size 	= rx_size - 4;

		/* print the ingress queued pkt details */
                if (netif_msg_rx_status(np))
			       printk(KERN_INFO "%s: napi_rx() ingress queued pkt status %4.4x,"
                                " size %4.4x, cur %4.4x.\n", dev->name, rx_status,
                                rx_size, cur_rx);

		/* packet copy from FIFO still in progress.
                 * Theoretically, this should never happen
                 * since EarlyRx is disabled.
                 */
                if (unlikely(rx_size == 0xfff0)) {
                        if (!np->fifo_copy_timeout)
                                np->fifo_copy_timeout = jiffies + 2;
                        else if (time_after(jiffies, np->fifo_copy_timeout)) {
                                printk ("%s: hung FIFO. Reset.", dev->name);
                                rx_size = 0;
                                goto no_early_rx;
                        }
                        if (netif_msg_intr(np)) {
                                printk(KERN_DEBUG "%s: fifo copy in progress.",
                                       dev->name);
                        }
                        break;
                }
		no_early_rx:
                np->fifo_copy_timeout = 0;


                /* If Rx err or invalid rx_size/rx_status received
                 * (which happens if we get lost in the ring),
                 * Rx process gets reset, so we abort any further
                 * Rx processing.
                 */
                if (unlikely((rx_size > (MAX_ETH_FRAME_SIZE+4)) ||
                             (rx_size < 8) ||
                             (!(rx_status & RxStatusOK)))) {
                        received = -1;
                        goto out;
                }

		/* allocate sk_buff obj  */
                skb = dev_alloc_skb (pkt_size + 2);
                if (likely(skb)) {
                        skb->dev = dev;
                        skb_reserve (skb, 2);   /* 16 byte align the IP fields. */

			/* copy packet from RxRing into data field of sk_buff */
#if RX_BUF_IDX == 3
                        wrap_copy(skb, rx_ring, ring_offset+4, pkt_size);
#else
			skb_copy_to_linear_data (skb, &rx_ring[ring_offset + 4], pkt_size);
#endif
			/* add data to a sk_buff */
                        skb_put (skb, pkt_size);

			/* set protocol field of sk_buff to appropriate packet type.
                         * This is 'type' of ethernet frame.
                         * Call eth_type_trans to get the packet type.
                         * For token ting devices use tr_type_trans function.
                         */
                        skb->protocol = eth_type_trans (skb, dev);

			/* set rx-packet status deatils in priv */
                        dev->last_rx 		= jiffies;
        		np->stats.rx_bytes 	+= pkt_size;
                        np->stats.rx_packets++;

			/* gives sk_buff into kernel */
                        netif_receive_skb (skb);
                }
		else {
                        if (net_ratelimit())
                                printk (KERN_WARNING
                                        "%s: Memory squeeze, dropping packet.\n",
                                        dev->name);
                        np->stats.rx_dropped++;
                }
		received++;

                cur_rx = (cur_rx + rx_size + 4 + 3) & ~3;
	  	writew ((u16) (cur_rx - 16), np->iobase + RxBufPtr );

		/* calls the ISR-ACK */
		nic_isr_ack(np);
		}

		if (unlikely(!received || rx_size == 0xfff0))
 	        	nic_isr_ack(np);
		 
		np->cur_rx = cur_rx;

        /*
         * The receive buffer should be mostly empty.
         * Tell NAPI to reenable the Rx irq.
         */
        if (np->fifo_copy_timeout)
                received = budget;
out:
        return received;
}

/**
 * nic_poll - poll's the rx'ed packets
 *
 * Kernel calls poll method of the driver in the NET_RX_SOFTIRQ
 * softirq bottom half to process the input packet.
 * This softirq was scheduled in interrupt handler routine.
 *
 * poll method receives two parameters,
 * 	one is pointer to struct net_device object and
 *      second is 'budget'. 
 *
 * budget field is the maximum number of packets the
 * kernel can accept at this time or we can say that
 * the 'budget' value is a maximum number of packets that the
 * current CPU can receive from all interfaces.
 *
 * Responsibilities :
 * 	-  check rx interrupt status
 * 	-  if ok does the polling (nic_rx)
 * 	-  turn again polling if number of packets received is less than budget
 *
 */

static int nic_poll(struct net_device *dev, int *budget)
{
        struct nic_private 		*np = netdev_priv(dev);
        int 				orig_budget = min(*budget, dev->quota);
        int 				done = 0;

#if 1
	/* check if it was Rx interrupt
         * calls to nic_rx()
         * it returns number of rx'ed packets
         */
        spin_lock(&np->rx_lock);
	if (likely(readb(np->iobase+ISR) & RxAckBits)) {	
		int work_done=0;
		
		printk(KERN_INFO "\nPoll's the Rx'ed packet..\n");
                work_done += nic_rx(dev, np, orig_budget);
		if (likely(work_done > 0)) {
                        *budget -= work_done;
                        dev->quota -= work_done;
                        done = (work_done < orig_budget);
                }
        }

	/* if number of packets received is less than budget ,
         * reenable receive interrupts
         * by setting the interrupts mask regsiter(IMR) and
         * all __nic_complete function to turn of polling.
         */
        if (done) {
                unsigned long flags;
                local_irq_save(flags);

		NAPI_WRITEW_FLAG (IMR, nic_intr_mask);
                __netif_rx_complete(dev);

                local_irq_restore(flags);
        }
        spin_unlock(&np->rx_lock);

	return !done;

#endif
        return 0;
	
}

/**
 * nic_start_xmit - starts packet tx'mission
 *
 * When the kernel has packets to send out of the interface ,
 * it calls driver's nic_xmit method.
 *
 * func contains two args
 *     - sk_buff of the packet to be transmitted
 *     - net_device object
 *
 * Driver should extract packet from the sk_buff and
 * put that into Txring
 * then driver should write length of packet and threshold in the
 * transmission descriptor status register of the device .
 * then the device takes the packet from the Txring and sends it.
 *
 * Responsibilities :
 * 	-  find the tx desc entry
 * 	-  copy the packet sk_buff into txring
 * 	-  set the len of packet into tx desc status reg
 * 	-  device take pkt from txring & sends...
 * 	-  trigger's an tx interrupt
 */
static int nic_start_xmit (struct sk_buff *skb, struct net_device *dev)
{
	struct nic_private 		*np = netdev_priv(dev);
        unsigned int 			entry;
        unsigned int 			len = skb->len;

	printk(KERN_INFO "\nStarts packet tx'mission..\n");

        /* calculate the next Tx descriptor entry. */
        entry = np->cur_tx % NUM_TX_DESC;


        /* see if frame size is more than our trasmition buffer
         * copy each frame on sk_buf into txring acc to desc
         * free sk_buf for individula txring desc
         */
        if (likely(len < TX_BUF_SIZE)) {
                if (len < ETH_ZLEN)
                        memset(np->tx_buf[entry], 0, ETH_ZLEN);
                skb_copy_and_csum_dev(skb, np->tx_buf[entry]);
                dev_kfree_skb(skb);

	   } else {
                dev_kfree_skb(skb);
                np->stats.tx_dropped++;
                return 0;
        }

        /* writer length of packet and threshold in
         * Transmission descripor status register
         */
	wmb();
        NAPI_WRITEL_FLAG (TxStatus0 + (entry * sizeof (u32)),
                   np->tx_flag | max(len, (unsigned int)ETH_ZLEN));
        dev->trans_start = jiffies;
	np->cur_tx++;
	
	/* stop the egress queue if not enough space */
        if ((np->cur_tx - NUM_TX_DESC) == np->dirty_tx)
                netif_stop_queue (dev);

	/* print the egress queued pkt details */
        if (netif_msg_tx_queued(np))
		printk (KERN_INFO "%s: nic_start_xmit() egress queued pkt size %u"
                                " to slot %d.\n", dev->name, len, entry);

        return 0;
}

/**
 * nic_tx_interrupt - processing tx'ed packets
 *
 * after packet been tx'mision is over ISR will schedule
 * this routine for processing tx'ed pkts
 *  
 * Responsibilities :
 * 	-  get dirty_tx,tx_left details from priv
 * 	-  get the status from tx desc status reg
 * 	-  check the all major error conditions like TxOutOfWindow, TxAborted...
 * 	-  update all error details into priv
 * 	-  wake up the txring queue
 */

static void nic_tx_interrupt (struct net_device *dev, struct nic_private *np)
{
 	unsigned long 			dirty_tx, tx_left;

	/* get the dirty_tx & tx_left from priv */
        dirty_tx 	= np->dirty_tx;
        tx_left 	= np->cur_tx - dirty_tx;

	printk(KERN_INFO "\nProcessing Tx'ed packets (int context)...\n");

	while (tx_left > 0) {
                int 		entry 	= dirty_tx % NUM_TX_DESC;
                int 		txstatus;

		/* read transmission descriptor status register and
                 * see what the status of the packet.
                 */
		txstatus = readl ((np->iobase+TxStatus0) + (entry * sizeof (u32)));

		/* check the txstatus if the packet is Tx'mitted/not */
		if (!(txstatus & (TxStatOK | TxUnderrun | TxAborted)))
                        break;  /* if not break */

		/* increment error statistics if there are
                 * any problems in the transmission
                 */
                if (txstatus & (TxOutOfWindow | TxAborted)) {
                        /* There was an major error, log it. */
                        if (netif_msg_tx_err(np))
                                printk(KERN_DEBUG "%s: Transmit error, Tx status %8.8x.\n",
                                        dev->name, txstatus);
                        np->stats.tx_errors++;
                        if (txstatus & TxAborted) {
                                np->stats.tx_aborted_errors++;
				writel (TxClearAbt, np->iobase+TxConfig);
                                writew (TxErr, np->iobase+ISR);
                                wmb();
                        }
                        if (txstatus & TxCarrierLost)
                                np->stats.tx_carrier_errors++;
                        if (txstatus & TxOutOfWindow)
                                np->stats.tx_window_errors++;
                } else {
                        if (txstatus & TxUnderrun) {
                                /* Add 64 to the Tx FIFO threshold. */
                                if (np->tx_flag < 0x00300000)
                                        np->tx_flag += 0x00020000;
                                np->stats.tx_fifo_errors++;
                        }
                        np->stats.collisions += (txstatus >> 24) & 15;
                        np->stats.tx_bytes += txstatus & 0x7ff;
                        np->stats.tx_packets++;
                }

                dirty_tx++;
                tx_left--;
        }

	/* wakeup the egress queue if enough space */
        if (np->dirty_tx != dirty_tx) {
                np->dirty_tx = dirty_tx;
                mb();
                netif_wake_queue (dev);
        }
}

/**
 * nic_interrupt - ISR handler
 *
 * handler will called for triggered interrupt during 
 * pkt before reception & after transmission
 *
 * Responsibilities :
 * 	-  get the isr
 * 	-  check the irq is shared/not 
 * 	-  get the ACK status
 * 	-  schedule BH 			if isr = rx interrupt
 * 	-  calls nic_tx_interrupt	if isr = tx interrupt
 */

static irqreturn_t nic_interrupt (int irq, void *dev_instance)
{
	/* get the net_device obj from dev_instance */
   	struct net_device 		*dev = (struct net_device *) dev_instance;
        struct nic_private 		*np  = netdev_priv(dev);
        u16 				status, ackstat;
        int 				handled = 0;
	
	printk(KERN_INFO "\nIn Interrupt Context.....");

	/* get the interrupt status register(ISR)
         * of the controller into local var
         */
	spin_lock (&np->lock);
	status = readw(np->iobase+ISR);

	/* check the irq is shared/not? */
        if (unlikely((status & nic_intr_mask) == 0))
	{
		printk("\n No Interrupts.. with Status == %#4.4x.\n", status);
                goto out;
	}
	handled = 1;	
	
	/* get the ACK status */
        ackstat = status & ~(RxAckBits | TxErr);

	if (ackstat) {
                writew (ackstat, np->iobase+ISR);
	}
	
        /*====== check the mode of packet handling( ie Rx or Tx) ========*/

	
        /*=============================================================
         * before Reception.........
         * as we know ingress queued packets are processed by poll function.
         *
         * When a packet arrives into the network interface card ,
         * network controller checks the dest MAC
         * if it matches with its MAC. the packet pumps into receive
         * buffer(Rxring) and kernel signals an interrupt...
         *
         * by executing ISR; if it is Rx interrupt.
         * Driver should scheduling the NET_RX_SOFTIRQ softirq (BH) for execution
         * (ie poll processing) and return from interrupt handler..                                      *=============================================================*/
	/* schedule BH'ed (poll) code */
        if (status & RxAckBits){
		printk("\nRx Interrupt triggered..with status = %#4.4x.\n", status);
                if (netif_rx_schedule_prep(dev)) {
			NAPI_WRITEW_FLAG (IMR, 0x73);
                        __netif_rx_schedule (dev);
			printk(KERN_INFO "\nSchedules Softirq..\n");
                }
        }

	/*====================================================================
         *  after transmission.....
         *  Network interface card will take the packet from the TxBuffer and
         *  puts that in it's FIFO.
         *  Once the FIFO reached threshold value set by the driver it sends the packet.
         *  After sending the packet network interface card will raise an interrupt.
         *
         *  by executing ISR; if it is Tx interrupt.
         *  it updates the tx statistics & does the extra txring cleanup..
         *=====================================================================*/
	if (status & (TxOK | TxErr)) {
		printk("\nTx Interrupt triggered..with status = %#4.4x.\n", status);
		nic_tx_interrupt (dev, np);
              	if (status & TxErr)
			writew (TxErr,np->iobase+ISR);
		}
 out:
        spin_unlock (&np->lock);
        printk ("\n%s: exiting interrupt, intr_status=%#4.4x.\n",
                dev->name, readw (np->iobase+ISR));
        return IRQ_RETVAL(handled);
}

/**
 * nic_get_stats - get the rx/tx packet status 
 *
 * for every packet transmisson/reception; this func
 * will set the status details into device specific priv struct
 *
 * Resposibilities :
 * 	-  verify the interface runnable/not
 * 	-  so fill pkt status into priv
 */

static struct net_device_stats *nic_get_stats (struct net_device *dev)
{

        struct nic_private 		*np = netdev_priv(dev);
        unsigned long 			flags;

	/* get the status if it is runnable interface */
        if (netif_running(dev)) {
                spin_lock_irqsave (&np->lock, flags);
		
		/* fill the pkt status into priv */
                np->stats.rx_missed_errors += readl (np->iobase+RxMissed);
                writel (0, np->iobase+RxMissed);

                spin_unlock_irqrestore (&np->lock, flags);
        }

        return &np->stats;
}

/**
 * nic_down - down a net-interface
 *
 * nic_down get's invoked
 * when the ifconfig is used to disable the net interface
 *
 *        ifconfig eth0 down
 *               |
 *               |
 *               V
 *          nic_down()
 *
 * Resposibilites:
 * 	disables device queue
 *      destroys Tx/Rxrings
 *      unregister ISR
 */

static int nic_down (struct net_device *dev)
{
        struct nic_private 		*np = netdev_priv(dev);

	printk(KERN_INFO "\nClose a net-interface.. \n");

        /* disables device queue */
        netif_stop_queue (dev);

        /* destroys Tx/Rxrings */
        pci_free_consistent(np->pdev, RX_BUF_TOT_LEN,
                            np->rx_ring, np->rx_ring_dma);
        pci_free_consistent(np->pdev, TX_BUF_TOT_LEN,
                            np->tx_bufs, np->tx_bufs_dma);
        np->rx_ring = NULL;
        np->tx_bufs = NULL;

        /* unregister ISR */
        free_irq (dev->irq, dev);

        return 0;
}

/*========================================================================
 *              pci interface stuff
 *=======================================================================*/

/* func declerations */
static int __devinit nic_probe (struct pci_dev *pdev,
                                       const struct pci_device_id *ids);
static void __devexit nic_remove(struct pci_dev *pdev);


/** 
 * nic_probe - probing for device found 
 *
 * HW Initialization: register pci_driver with PCI-BIOS (done in nic_init)
 * 	-  register pci_driver to pci-bios (switch to probr if succeed)
 * 	-  allocates the net_device obj
 * 	-  enable device
 * 	-  access the pci_dev configuration space (done pci-bios by default)
 * 	-  discover the resources ie base addr IO(port space)
 * 	-  request the region
 * 	-  enables the DMA bus mastering if
 * 	-  allocates the resources(ioremap)
 *
 * SW Initialization: register net_device with Common netdevice layer
 * 	-  allocates the net_device obj
 *	-  copy device data from pci_dev into driver specific priv
 *	-  get the MAC Address
 *	-  assign the driver interface routines into net_device struct
 *	-  register net_device with common net device layer
 *	-  pulls the dev config from pci_dev into net_device	
 */
static int __devinit nic_probe (struct pci_dev *pdev,
                                       const struct pci_device_id *ent)
{
 	struct net_device 		*dev = NULL;
        struct nic_private 		*np;
	unsigned long                   mmio_start, mmio_end, mmio_flags, mmio_len;
        void __iomem                    *memio;
	int 				disable_dev_on_err = 0,rc,i;

	printk(KERN_INFO "\nprobeing for device found..\n");

        /* enable pci device */
        rc = pci_enable_device (pdev);
        if (rc)
                goto err_out;
	
	/* discover the resources ie base addr IO(port space) */
        mmio_start      = pci_resource_start (pdev, 1);
        mmio_end        = pci_resource_end (pdev, 1);
        mmio_flags      = pci_resource_flags (pdev, 1);
        mmio_len        = pci_resource_len (pdev, 1);

        printk(KERN_INFO "\nMMIO region size == 0x%02lX\n", mmio_len);

        /* requesting pci-bios to check whether that particular region
         * is used by some other pci_driver at this point/not
         */
        rc = pci_request_regions (pdev, DRV_NAME);
        if (rc)
                goto  err_out;
        disable_dev_on_err = 1;

        /* enable PCI bus-mastering */
        pci_set_master(pdev);

        /* ioremap MMIO region */
         memio = pci_iomap(pdev, 1, 0);
        if (memio == NULL) {

	         dev_err(&pdev->dev, "cannot remap MMIO, aborting\n");
                rc = -EIO;
                goto err_out;
        }

	/* allocates net_device object */
        dev = alloc_etherdev(sizeof(struct nic_private));
        if (!dev) {
                rc = -ENOMEM;
                printk(KERN_ERR "alloc_etherdev failed\n");
                goto err_out;
        }
        SET_NETDEV_DEV(dev, &pdev->dev);

        printk(KERN_INFO "\nallocates etherdev of %d bytes size\n",
                                        sizeof(struct nic_private));

	/* copy device data from dev into driver specific priv */
        dev->base_addr   	= (long) memio;
        np               	= netdev_priv(dev);
        np->pdev         	= pdev;
        np->iobase       	= memio;
        np->regs_len     	= mmio_len;
        np->irq          	= pdev->irq;
	np->msg_enable          =
                        (debug < 0 ? RTL8139_DEF_MSG_ENABLE : ((1 << debug) - 1));

	/* get the MAC Address */
	 for(i = 0; i < 6; i++) {  /* Hardware Address */
               dev->dev_addr[i] = (readb((const volatile void __iomem *)dev->base_addr + i));
               dev->broadcast[i] = 0xff;
        }

	/* assign the driver interface routines into net_device struct */
        dev->hard_header_len 	= 14;
        dev->open 		= nic_up;
        dev->hard_start_xmit 	= nic_start_xmit;
        dev->poll 		= nic_poll;
	dev->weight 		= 64;
        dev->stop 		= nic_down;
        dev->get_stats 		= nic_get_stats;
	dev->ethtool_ops 	= &nic_ethtool_ops;
	dev->irq 		= pdev->irq;

	/* Initialize Tx,Rx locks */
	spin_lock_init (&np->lock);
        spin_lock_init (&np->rx_lock);

	/* regiset net_device with common network layer*/
        rc = register_netdev(dev);
        if (rc) {
                printk("\nCannot register net device, aborting");
                goto err_out;
        }
        printk(KERN_INFO "\nRegistering net_device with common network layer\n");


	/* pulls the dev config from pci_dev into net_device */
        pci_set_drvdata (pdev, dev);

        printk (KERN_INFO "%s: at 0x%lx, "
                "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x, "
                "IRQ %d\n",
                dev->name,
                dev->base_addr,
                dev->dev_addr[0], dev->dev_addr[1],
                dev->dev_addr[2], dev->dev_addr[3],
                dev->dev_addr[4], dev->dev_addr[5],
                dev->irq);

		return 0;
err_out:
	if (disable_dev_on_err)
        	pci_disable_device (pdev);

        return rc;
}

/**
 * nic_remove - Un-probeing for device Not-found
 *
 * SW Uninitialization :
 * 	-  unregister net_device from common net device layer
 * 	-  free the net_device obj which is already been used
 *
 * HW Uninitialization :
 *	-  release the pci region
 *	-  set NULL to pci region
 *	-  disables the pci device 
 */

static void __devexit nic_remove(struct pci_dev *pdev)
{
        struct net_device          	*dev = pci_get_drvdata (pdev);
        struct nic_private 		*np  = netdev_priv(dev);

	printk(KERN_INFO "\nUn-probeing for device Not-found..\n");

        if (dev != NULL){
        /* unregister net_device from common net device layer */
        unregister_netdev (dev);
        
	/* free the net_device obj which is already been used */
        free_netdev(dev);

        /* release the pci region which is already been requested */
        if (np->iobase)
                pci_release_regions (pdev);

        /* set the dev config from dev into NULL */
        pci_set_drvdata (pdev, NULL);

        /* disables the pci device */
        pci_disable_device (pdev);
	}
}

/* pci driver ops */
static struct pci_driver nic_pci_driver  = {
        .name                   = DRV_NAME,
        .id_table               = nic_pci_idtable,
        .probe                  = nic_probe,
        .remove                 = nic_remove,
};

/* driver init */
static int __init nic_init(void)
{
        printk(KERN_INFO "Registering " DRIVER_NAME "\n");
        return pci_register_driver(&nic_pci_driver);
}

/* driver cleanup */
static void __exit nic_cleanup(void)
{
        printk(KERN_INFO "UnRegistering " DRIVER_NAME "\n");
        pci_unregister_driver(&nic_pci_driver);
}

module_init(nic_init);
module_exit(nic_cleanup);

MODULE_AUTHOR ("TEAM VEDA");
MODULE_DESCRIPTION ("RealTek RTL-8139 Fast Ethernet with Driver");
MODULE_LICENSE("GPL");










