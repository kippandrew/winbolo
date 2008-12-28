/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


/*********************************************************
*Name:          rsa algorithm
*Filename:      rsaalorithm.h
*Author:        Minhiriath
*Creation Date: 26/12/2008
*Last Modified: 26/12/2008
*Purpose:
*  rsa algorithm function definitions.
*********************************************************/

#if _MSC_VER >= 1100
	/* Detect memory leaks in MSVC++ */ 
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#else
	#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "BigDigits\bigd.h"
#include "BigDigits\bigdRand.h"

static void pr_msg(const char *msg, BIGD b)
/* Display a message followed by a BIGD value */
{
	printf("%s", msg);
	bdPrint(b, BD_PRINT_NL);
}
