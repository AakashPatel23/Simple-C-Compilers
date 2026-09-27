	.text
	.file	"main"
	.globl	partition
	.align	16, 0x90
	.type	partition,@function
partition:                              # @partition
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp0:
	.cfi_def_cfa_offset 16
.Ltmp1:
	.cfi_offset %rbx, -16
	movl	%edx, %r11d
	movslq	%ecx, %rcx
	movsbl	(%rdi,%rcx), %r10d
	movslq	%r11d, %rbx
	movb	(%rdi,%rbx), %al
	movb	%al, (%rdi,%rcx)
	movb	%r10b, (%rdi,%rbx)
	movl	%esi, %r9d
	jmp	.LBB0_1
	.align	16, 0x90
.LBB0_4:                                # %if.end
                                        #   in Loop: Header=BB0_1 Depth=1
	incl	%esi
.LBB0_1:                                # %while.cond
                                        # =>This Inner Loop Header: Depth=1
	cmpl	%r11d, %esi
	jge	.LBB0_5
# BB#2:                                 # %while.body
                                        #   in Loop: Header=BB0_1 Depth=1
	movslq	%esi, %rdx
	movsbl	(%rdi,%rdx), %eax
	cmpl	%r10d, %eax
	jge	.LBB0_4
# BB#3:                                 # %if.then
                                        #   in Loop: Header=BB0_1 Depth=1
	movb	(%rdi,%rdx), %r8b
	movslq	%r9d, %rcx
	movb	(%rdi,%rcx), %al
	movb	%al, (%rdi,%rdx)
	movb	%r8b, (%rdi,%rcx)
	incl	%r9d
	jmp	.LBB0_4
.LBB0_5:                                # %while.end
	movslq	%r9d, %rcx
	movb	(%rdi,%rcx), %dl
	movb	(%rdi,%rbx), %al
	movb	%al, (%rdi,%rcx)
	movb	%dl, (%rdi,%rbx)
	movl	%ecx, %eax
	popq	%rbx
	retq
.Ltmp2:
	.size	partition, .Ltmp2-partition
	.cfi_endproc

	.globl	quicksort
	.align	16, 0x90
	.type	quicksort,@function
quicksort:                              # @quicksort
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp3:
	.cfi_def_cfa_offset 16
	pushq	%r15
.Ltmp4:
	.cfi_def_cfa_offset 24
	pushq	%r14
.Ltmp5:
	.cfi_def_cfa_offset 32
	pushq	%rbx
.Ltmp6:
	.cfi_def_cfa_offset 40
	pushq	%rax
.Ltmp7:
	.cfi_def_cfa_offset 48
.Ltmp8:
	.cfi_offset %rbx, -40
.Ltmp9:
	.cfi_offset %r14, -32
.Ltmp10:
	.cfi_offset %r15, -24
.Ltmp11:
	.cfi_offset %rbp, -16
	movl	%edx, %ebx
	movl	%esi, %ebp
	movq	%rdi, %r14
	cmpl	%ebx, %ebp
	jge	.LBB1_2
# BB#1:                                 # %if.then
	movl	%ebx, %eax
	subl	%ebp, %eax
	movl	%eax, %ecx
	shrl	$31, %ecx
	addl	%eax, %ecx
	sarl	%ecx
	addl	%ebp, %ecx
	movq	%r14, %rdi
	movl	%ebp, %esi
	movl	%ebx, %edx
	callq	partition
	movl	%eax, %r15d
	leal	-1(%r15), %edx
	movq	%r14, %rdi
	movl	%ebp, %esi
	callq	quicksort
	leal	1(%r15), %esi
	movq	%r14, %rdi
	movl	%ebx, %edx
	callq	quicksort
.LBB1_2:                                # %if.end
	addq	$8, %rsp
	popq	%rbx
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
.Ltmp12:
	.size	quicksort, .Ltmp12-quicksort
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp13:
	.cfi_def_cfa_offset 16
	subq	$48, %rsp
.Ltmp14:
	.cfi_def_cfa_offset 64
.Ltmp15:
	.cfi_offset %rbx, -16
	movaps	.L.str1+16(%rip), %xmm0
	movaps	%xmm0, 16(%rsp)
	movaps	.L.str1(%rip), %xmm0
	movaps	%xmm0, (%rsp)
	movl	$6778724, 32(%rsp)      # imm = 0x676F64
	leaq	(%rsp), %rbx
	xorl	%esi, %esi
	movl	$34, %edx
	movq	%rbx, %rdi
	callq	quicksort
	movl	$.L.str, %edi
	xorl	%eax, %eax
	movq	%rbx, %rsi
	callq	printf
	xorl	%eax, %eax
	addq	$48, %rsp
	popq	%rbx
	retq
.Ltmp16:
	.size	main, .Ltmp16-main
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"%s\n"
	.size	.L.str, 4

	.type	.L.str1,@object         # @.str1
	.section	.rodata.str1.16,"aMS",@progbits,1
	.align	16
.L.str1:
	.asciz	"thequickbrownfoxjumpsoverthelazydog"
	.size	.L.str1, 36


	.section	".note.GNU-stack","",@progbits
