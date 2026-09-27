	.text
	.file	"main"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%r14
.Ltmp0:
	.cfi_def_cfa_offset 16
	pushq	%rbx
.Ltmp1:
	.cfi_def_cfa_offset 24
	pushq	%rax
.Ltmp2:
	.cfi_def_cfa_offset 32
.Ltmp3:
	.cfi_offset %rbx, -24
.Ltmp4:
	.cfi_offset %r14, -16
	movl	$1, %ebx
	jmp	.LBB0_1
	.align	16, 0x90
.LBB0_6:                                # %if.then7
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	$.L.str, %edi
	movl	$5, %edx
	xorl	%eax, %eax
	movl	%ebx, %esi
	callq	printf
	incl	%ebx
.LBB0_1:                                # %while.cond
                                        # =>This Inner Loop Header: Depth=1
	cmpl	$10, %ebx
	jg	.LBB0_14
# BB#2:                                 # %while.body
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	$.L.str3, %edi
	xorl	%eax, %eax
	movl	%ebx, %esi
	callq	printf
	movl	%ebx, %eax
	shrl	$31, %eax
	addl	%ebx, %eax
	andl	$-2, %eax
	cmpl	%eax, %ebx
	jne	.LBB0_7
# BB#3:                                 # %if.then
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	$.L.str2, %edi
	movl	$2, %esi
	xorl	%eax, %eax
	callq	printf
	movslq	%ebx, %r14
	imulq	$1431655766, %r14, %rax # imm = 0x55555556
	movq	%rax, %rcx
	shrq	$63, %rcx
	shrq	$32, %rax
	addl	%ecx, %eax
	leal	(%rax,%rax,2), %eax
	cmpl	%eax, %ebx
	jne	.LBB0_5
# BB#4:                                 # %if.then2
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	$.L.str, %edi
	movl	$3, %edx
	xorl	%eax, %eax
	movl	%ebx, %esi
	callq	printf
.LBB0_5:                                # %if.end3
                                        #   in Loop: Header=BB0_1 Depth=1
	imulq	$1717986919, %r14, %rax # imm = 0x66666667
	movq	%rax, %rcx
	shrq	$63, %rcx
	sarq	$33, %rax
	addl	%ecx, %eax
	leal	(%rax,%rax,4), %eax
	cmpl	%eax, %ebx
	je	.LBB0_6
	jmp	.LBB0_13
	.align	16, 0x90
.LBB0_7:                                # %if.else
                                        #   in Loop: Header=BB0_1 Depth=1
	movslq	%ebx, %rdx
	imulq	$1431655766, %rdx, %rax # imm = 0x55555556
	movq	%rax, %rcx
	shrq	$63, %rcx
	shrq	$32, %rax
	addl	%ecx, %eax
	leal	(%rax,%rax,2), %eax
	cmpl	%eax, %ebx
	jne	.LBB0_10
# BB#8:                                 # %if.then13
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	$.L.str2, %edi
	movl	$3, %esi
	jmp	.LBB0_9
.LBB0_10:                               # %if.else14
                                        #   in Loop: Header=BB0_1 Depth=1
	imulq	$1717986919, %rdx, %rax # imm = 0x66666667
	movq	%rax, %rcx
	shrq	$63, %rcx
	sarq	$33, %rax
	addl	%ecx, %eax
	leal	(%rax,%rax,4), %eax
	cmpl	%eax, %ebx
	jne	.LBB0_12
# BB#11:                                # %if.then19
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	$.L.str2, %edi
	movl	$5, %esi
.LBB0_9:                                # %if.end
                                        #   in Loop: Header=BB0_1 Depth=1
	xorl	%eax, %eax
	callq	printf
	incl	%ebx
	jmp	.LBB0_1
.LBB0_12:                               # %if.else20
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	$.L.str1, %edi
	xorl	%eax, %eax
	callq	printf
	.align	16, 0x90
.LBB0_13:                               # %if.end
                                        #   in Loop: Header=BB0_1 Depth=1
	incl	%ebx
	jmp	.LBB0_1
.LBB0_14:                               # %while.end
	xorl	%eax, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%r14
	retq
.Ltmp5:
	.size	main, .Ltmp5-main
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.16,"aMS",@progbits,1
	.align	16
.L.str:
	.asciz	"%d is a multiple of %d.\n"
	.size	.L.str, 25

	.type	.L.str1,@object         # @.str1
	.align	16
.L.str1:
	.asciz	" is not a multiple of 2, 3 or 5.\n"
	.size	.L.str1, 34

	.type	.L.str2,@object         # @.str2
	.align	16
.L.str2:
	.asciz	" is a multiple of %d.\n"
	.size	.L.str2, 23

	.type	.L.str3,@object         # @.str3
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str3:
	.asciz	"%d"
	.size	.L.str3, 3


	.section	".note.GNU-stack","",@progbits
