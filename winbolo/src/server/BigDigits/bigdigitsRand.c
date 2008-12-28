/* $Id: bigdigitsRand.c $ */

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

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bigdigits.h"
#include "bigdigitsRand.h"

static uint32_t btrrand(void);

/**********************/
/* EXPORTED FUNCTIONS */
/**********************/

DIGIT_T spBetterRand(void)
{	/*	Returns a "better" pseudo-random digit. */
	return (DIGIT_T)btrrand();
}

/* Added [v2.2] */
size_t mpRandomBits(DIGIT_T a[], size_t ndigits, size_t nbits)
	/* Generate a random mp number <= 2^nbits using internal RNG */
{
	const int bits_per_digit = BITS_PER_DIGIT;
	size_t i;
	int j;
	DIGIT_T r;

	mpSetZero(a, ndigits);
	r = spBetterRand();
	j = bits_per_digit;
	for (i = 0; i < nbits; i++)
	{
		if (j <= 0)
		{
			r = spBetterRand();
			j = bits_per_digit;
		}
		mpSetBit(a, ndigits, i, r & 0x1);
		r >>= 1;
		j--;
	}

	return i;
}


/******************************************************************************
Generates a pseudo-random DIGIT value by using
the ANSI X9.31 algorithm but with the `Tiny Encryption Algorithm' 
replacing the `Triple DES' algorithm (much less code to copy, and faster).

CAUTION: not thread-safe as it uses a static variable.

Not quite cryptographically secure but much better than 
just using the plain-old-rand() function. 
Output should always pass the FIPS 140-2 statistical test.
Users can make their own call as to the security of this approach.
It's certainly sufficient for generating random digits for tests.

[v2.1] Changed to `new variant' TEAX of encipher algorithm
(this is unlikely to make any change in the security of this RNG).
******************************************************************************/

/******************************************************************************
ANSI X9.31 ALGORITHM:
Given

    * D, a 64-bit representation of the current date-time
    * S, a secret 64-bit seed that will be updated by this process
    * K, a secret [Triple DES] key

Step 1. Compute the 64-bit block X = G(S, K, D) as follows:

   1. I = E(D, K)
   2. X = E(I XOR S, K)
   3. S' = E(X XOR I, K)

where E(p, K) is the [Triple DES] encryption of the 64-bit block p using key K.

Step 2. Return X and set S = S' for the next cycle. 
******************************************************************************/

#define KEY_WORDS 4
static void encipher(uint32_t *const v,uint32_t *const w, const uint32_t *const k);

/* CAUTION: We use a static structure to store our values in. */
static struct {
	int seeded;
	uint32_t seed[2];
	uint32_t key[KEY_WORDS];
} m_generator;

/* Cross-platform ways to get a 64-bit time value */
#if defined(unix) || defined(__unix__) || defined(linux) || defined(__linux__)
static void get_time64(uint32_t t[2])
{
	#include <sys/time.h>
	struct timeval tv;
	gettimeofday(&tv, NULL);
	memcpy(t, &tv, 2*sizeof(uint32_t));
}
#elif defined(_WIN32) || defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static void get_time64(uint32_t t[2])
{
	FILETIME ft;
	GetSystemTimeAsFileTime (&ft);
	t[0] = ft.dwHighDateTime;
	t[1] = ft.dwLowDateTime;
}
#else
static void get_time64(uint32_t t[2])
{
	/* Best we can do with strict ANSI */
	/* [v2.2] used clock() as well as time() to improve precision  */
	t[0] = (uint32_t)time(NULL);
	t[1] = (uint32_t)clock();
}
#endif

static void btrseed(uint32_t seed)
{
	int i;
	uint32_t t[2];

	/* Use plain old rand function to generate our internal seed and key */
	srand(seed);
	for (i = 0; i < 2; i++)
		m_generator.seed[i] = (rand() & 0xFFFF) << 16 | (rand() & 0xFFFF);
	for (i = 0; i < KEY_WORDS; i++)
		m_generator.key[i] = (rand() & 0xFFFF) << 16 | (rand() & 0xFFFF);

	/* Set flag so we only do it once */
	m_generator.seeded = 1;

	/* Set key = key XOR time */
	get_time64(t);
	m_generator.key[0] ^= t[0];
	m_generator.key[1] ^= t[1];

}

static uint32_t btrrand(void)
/* Returns one 32-bit word */
{
	uint32_t inter[2], x[2];

	/* Set seed if not already seeded */
	if (!m_generator.seeded)
	{
		btrseed((uint32_t)time(NULL));
	}

	/* I = E(D, K) */
	get_time64(inter);
	encipher(inter, inter, m_generator.key);

	/* X = E(I XOR S, K) */
	x[0] = inter[0] ^ m_generator.seed[0];
	x[1] = inter[1] ^ m_generator.seed[1];
	encipher(x, x, m_generator.key);

	/* S' = E(X XOR I, K) */
	inter[0] ^= x[0];
	inter[1] ^= x[1];
	encipher(inter, m_generator.seed, m_generator.key);

	return x[0];
}

/************************************************

The Tiny Encryption Algorithm (TEA) by 
David Wheeler and Roger Needham of the
Cambridge Computer Laboratory.

Placed in the Public Domain by
David Wheeler and Roger Needham.

**** ANSI C VERSION (New Variant) ****

Notes:

TEA is a Feistel cipher with XOR and
and addition as the non-linear mixing
functions. 

Takes 64 bits of data in v[0] and v[1].
Returns 64 bits of data in w[0] and w[1].
Takes 128 bits of key in k[0] - k[3].

TEA can be operated in any of the modes
of DES. Cipher Block Chaining is, for example,
simple to implement.

n is the number of iterations. 32 is ample,
16 is sufficient, as few as eight may be OK.
The algorithm achieves good dispersion after
six iterations. The iteration count can be
made variable if required.

Note this is optimised for 32-bit CPUs with
fast shift capabilities. It can very easily
be ported to assembly language on most CPUs.

delta is chosen to be the real part of (the
golden ratio Sqrt(5/4) - 1/2 ~ 0.618034
multiplied by 2^32).

This version has been amended to foil two
weaknesses identified by David A. Wagner
(daw@cs.berkeley.edu): 1) effective key
length of old-variant TEA was 126 not 128
bits 2) a related key attack was possible
although impractical.

************************************************/

static void encipher(uint32_t *const v,uint32_t *const w, const uint32_t *const k)
{
	register uint32_t y=v[0],z=v[1],sum=0,delta=0x9E3779B9,n=32;

	while(n-->0)
	{
		y+= (((z<<4) ^ (z>>5)) + z) ^ (sum + k[sum & 3]);
		sum += delta;
		z+= (((y<<4) ^ (y>>5)) + y) ^ (sum + k[sum>>11 & 3]);
	}

	w[0]=y; w[1]=z;
}

