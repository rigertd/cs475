#include "mulsum.p5.h"

float MulSum(float *a, float *b, int len)
{
    float result = 0.;
    
	__asm
	(
		".att_syntax\n\t"
		"movq    -24(%rbp), %rbx\n\t"		// a
		"movq    -32(%rbp), %rcx\n\t"		// b
		"movss   -40(%rbp),  %xmm2\n\t"		// result
	);

	for( int i = 0; i < len; i++ )
	{
		__asm
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

	__asm
	(
		".att_syntax\n\t"
		"movss	 %xmm2, -40(%rbp)\n\t"	// copy the sums back to sum[ ]
	);
    
    return result;
}