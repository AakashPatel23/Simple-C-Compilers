	.text
	.file	"main"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp0:
	.cfi_def_cfa_offset 16
	xorl	%ecx, %ecx
	movl	$360, %edx              # imm = 0x168
	xorl	%esi, %esi
	jmp	.LBB0_1
	.align	16, 0x90
.LBB0_2:                                # %while.body
                                        #   in Loop: Header=BB0_1 Depth=1
	incl	%ecx
	leal	8(%rax), %edx
	movl	%eax, %esi
.LBB0_1:                                # %while.cond
                                        # =>This Inner Loop Header: Depth=1
	movl	%edx, %eax
	testl	%ecx, %ecx
	jle	.LBB0_2
# BB#3:                                 # %while.end
	movl	$.L.str, %edi
	xorl	%eax, %eax
	callq	printf
	xorl	%eax, %eax
	popq	%rdx
	retq
.Ltmp1:
	.size	main, .Ltmp1-main
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Result: %d\n"
	.size	.L.str, 12


	.section	".note.GNU-stack","",@progbits
