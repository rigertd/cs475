	.file	"mulsum.p5.cpp"
	.text
	.globl	_Z6MulSumPfS_i
	.type	_Z6MulSumPfS_i, @function
_Z6MulSumPfS_i:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	%edx, -36(%rbp)
	movl	.LC0(%rip), %eax
	movl	%eax, -4(%rbp)
#APP
# 13 "mulsum.p5.cpp" 1
	.att_syntax
	movq    -24(%rbp), %rbx
	movq    -32(%rbp), %rcx
	movss   -40(%rbp),  %xmm2
	
# 0 "" 2
#NO_APP
	movl	$0, -8(%rbp)
	jmp	.L2
.L3:
#APP
# 26 "mulsum.p5.cpp" 1
	.att_syntax
	movss	(%rbx), %xmm0
	movss	(%rcx), %xmm1
	mulss	%xmm1, %xmm0
	addss	%xmm0, %xmm2
	addq $4, %rbx
	addq $4, %rcx
	
# 0 "" 2
#NO_APP
	addl	$1, -8(%rbp)
.L2:
	movl	-8(%rbp), %eax
	cmpl	-36(%rbp), %eax
	jl	.L3
#APP
# 33 "mulsum.p5.cpp" 1
	.att_syntax
	movss	 %xmm2, -40(%rbp)
	
# 0 "" 2
#NO_APP
	movl	-4(%rbp), %eax
	movl	%eax, -40(%rbp)
	movss	-40(%rbp), %xmm0
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	_Z6MulSumPfS_i, .-_Z6MulSumPfS_i
	.section	.rodata
	.align 4
.LC0:
	.long	0
	.ident	"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04.3) 4.8.4"
	.section	.note.GNU-stack,"",@progbits
