/* $Id: bigdRand.h $ */

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

/* Random number BIGD functions that rely on spRandom. Not thread-safe.  */

#ifndef BIGDRAND_H_
#define BIGDRAND_H_ 1

#include "bigd.h"

#ifdef __cplusplus
extern "C" {
#endif

bdigit_t bdRandDigit(void);
	/* Generate a single random digit using internal RNG. */

size_t bdRandomBits(BIGD a, size_t nbits);
	/* Generate a random BIGD number <= 2^nbits using internal RNG */

#ifdef __cplusplus
}
#endif

#endif /* BIGDRAND_H_ */
