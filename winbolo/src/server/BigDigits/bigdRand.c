/* $Id: bigdRand.c $ */

/******************** SHORT COPYRIGHT NOTICE**************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 2.2 originally written by David Ireland,
copyright (c) 2001-8 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with this
library or can be obtained from <www.di-mgt.com.au/bigdigits.html>.
This notice must always be retained in any copy.
******************* END OF COPYRIGHT NOTICE***************************/
/*
	Last updated:
	$Date: 2008-05-04 15:12:00 $
	$Revision: 2.2.0 $
	$Author: dai $
*/

/* Random number BIGD functions that rely on spBetterRand */

#include "bigdRand.h"
#include "bigdigitsRand.h"

#include <stdio.h>

bdigit_t bdRandDigit(void)
/* Return a random digit. */
{
	return spBetterRand();
}

size_t bdRandomBits(BIGD a, size_t nbits)
/* Generate a random BIGD number <= 2^nbits using internal RNG */
{
	const int bits_per_digit = OCTETS_PER_DIGIT * 8;
	size_t i;
	int j;
	bdigit_t r;

	bdSetZero(a);
	bdSetBit(a, nbits-1, 0);
	r = bdRandDigit();
	j = bits_per_digit;
	for (i = 0; i < nbits; i++)
	{
		if (j <= 0)
		{
			r = bdRandDigit();
			j = bits_per_digit;
		}
		bdSetBit(a, i, r & 0x1);
		r >>= 1;
		j--;
	}

	return i;
}
