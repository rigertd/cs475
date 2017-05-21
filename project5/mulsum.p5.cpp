#include "mulsum.p5.h"

float MulSum(float *a, float *b, int len)
{
	// Maintain these local variables for fair comparison
	float sum[4] = { 0., 0., 0., 0. };
	int limit = len;

	__asm __volatile
	(
		".att_syntax\n\t"
		"movq    -40(%rbp), %rbx\n\t"		// a
		"movq    -48(%rbp), %rcx\n\t"		// b
		"leaq    -32(%rbp), %rdx\n\t"		// &sum[0]
		"movss   (%rdx), %xmm2\n\t"		// 1 copy of 0. in xmm2
	);

	for( int i = 0; i < limit; i++ )
	{
		__asm __volatile
		(
			".att_syntax\n\t"
			"movss	(%rbx), %xmm0\n\t"	// load the first sse register
			"movss	(%rcx), %xmm1\n\t"	// load the second sse register
			"mulss	%xmm1, %xmm0\n\t"	// do the multiply
			"addss	%xmm0, %xmm2\n\t"	// do the add
			"addq $4, %rbx\n\t"
			"addq $4, %rcx\n\t"
		);
	}

	__asm __volatile
	(
		".att_syntax\n\t"
		"movss	 %xmm2, (%rdx)\n\t"	// copy the sum back to sum[ ]
	);

	return sum[0];
}