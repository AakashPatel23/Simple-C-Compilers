	.text
	.file	"main"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp0:
	.cfi_def_cfa_offset 16
	pushq	%r14
.Ltmp1:
	.cfi_def_cfa_offset 24
	pushq	%rbx
.Ltmp2:
	.cfi_def_cfa_offset 32
.Ltmp3:
	.cfi_offset %rbx, -32
.Ltmp4:
	.cfi_offset %r14, -24
.Ltmp5:
	.cfi_offset %rbp, -16
	movl	$80, %r8d
	movl	$70, %r10d
	movl	$60, %ebx
	movl	$50, %r11d
	movl	$40, %esi
	movl	$30, %r9d
	movl	$20, %edi
	movl	$10, %ecx
	xorl	%r14d, %r14d
	xorl	%edx, %edx
	xorl	%ebp, %ebp
	jmp	.LBB0_1
	.align	16, 0x90
.LBB0_5:                                # %while.end28
                                        #   in Loop: Header=BB0_1 Depth=1
	incl	%r14d
.LBB0_1:                                # %while.cond
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
	testl	%r14d, %r14d
	jg	.LBB0_6
# BB#2:                                 # %while.body
                                        #   in Loop: Header=BB0_1 Depth=1
	leal	(%rcx,%rdi), %ebp
	addl	%r9d, %ebp
	addl	%esi, %ebp
	addl	%r11d, %ebp
	addl	%ebx, %ebp
	addl	%r10d, %ebp
	addl	%r8d, %ebp
	incl	%edi
	incl	%r9d
	incl	%esi
	incl	%r11d
	incl	%ebx
	incl	%r10d
	incl	%r8d
	jmp	.LBB0_3
	.align	16, 0x90
.LBB0_4:                                # %while.body27
                                        #   in Loop: Header=BB0_3 Depth=2
	subl	%ecx, %esi
	leal	-3(%rax), %r9d
	movl	$-3, %r11d
	movl	$-6, %ebx
	leal	-3(%rsi), %r10d
	incl	%edx
	leal	-7(%rax), %ecx
	movl	%eax, %r8d
	movl	%eax, %edi
.LBB0_3:                                # %while.cond23
                                        #   Parent Loop BB0_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	%ecx, %eax
	leal	1(%rax), %ecx
	cmpl	$99, %edx
	jle	.LBB0_4
	jmp	.LBB0_5
.LBB0_6:                                # %while.end
	movl	$.L.str, %edi
	xorl	%eax, %eax
	movl	%ebp, %esi
	callq	printf
	xorl	%eax, %eax
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
.Ltmp6:
	.size	main, .Ltmp6-main
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Result: %d\n"
	.size	.L.str, 12


	.section	".note.GNU-stack","",@progbits
