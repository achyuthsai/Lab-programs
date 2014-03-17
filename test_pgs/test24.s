	.file	"test24.c"
	.section	.rodata
.LC0:
	.string	"%d %d %d %d %d "
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	andl	$-16, %esp
	subl	$48, %esp
	movl	$5, 44(%esp)
	subl	$1, 44(%esp)
	addl	$1, 44(%esp)
	movl	44(%esp), %ecx
	subl	$1, 44(%esp)
	movl	44(%esp), %edx
	addl	$1, 44(%esp)
	movl	$.LC0, %eax
	movl	44(%esp), %ebx
	.cfi_offset 3, -12
	movl	%ebx, 20(%esp)
	movl	44(%esp), %ebx
	movl	%ebx, 16(%esp)
	movl	44(%esp), %ebx
	movl	%ebx, 12(%esp)
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	movl	$0, %eax
	movl	-4(%ebp), %ebx
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits
