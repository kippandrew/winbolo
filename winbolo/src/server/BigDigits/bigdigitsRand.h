/* $Id: bigdigitsRand.h $ */

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
	$Date: 2008-05-04 15:13:00 $
	$Revision: 2.2.0 $
	$Author: dai $
*/

/*	Interface for BIGDIGITS mp random functions */
/*	[v2.2] changed name from spRandom to bigdigitsRand */

#ifndef BIGDIGITSRAND_H_
#define BIGDIGITSRAND_H_ 1

#include "bigdigits.h"

#ifdef __cplusplus
extern "C" {
#endif

DIGIT_T spBetterRand(void);
	/*	Returns a "better" pseudo-random digit. */

size_t mpRandomBits(DIGIT_T a[], size_t ndigits, size_t nbits);
	/* Generate a random mp number <= 2^nbits using spBetterRand */

#ifdef __cplusplus
}
#endif

#endif /* BIGDIGITSRAND_H_ */
