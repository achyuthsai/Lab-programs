cmd_/home/veda/kernel/hello.ko := ld -r -m elf_i386 -T /usr/src/linux-headers-3.2.0-31-generic-pae/scripts/module-common.lds --build-id  -o /home/veda/kernel/hello.ko /home/veda/kernel/hello.o /home/veda/kernel/hello.mod.o
