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
*Name:          Util
*Filename:      util.h 
*Author:        John Morrison
*Creation Date: 25/12/98
*Last Modified: 22/04/01
*Purpose:
*  Provides misc functions
*********************************************************/

#ifndef UTILS_H
#define UTILS_H

#define BRADIAN_ADD8 8
#define NIBBLE_SHIFT_SIZE 4

/* Used in extracting the map name from the filename and path */
#define STRTOK_SEPS "\\/"
#define END3 4
#define END2 3
#define END1 2
#define END0 1
#define END3CHRA '.'
#define END2CHRA 'm'
#define END1CHRA 'a'
#define END0CHRA 'p'
#define END2CHRB 'M'
#define END1CHRB 'A'
#define END0CHRB 'P'


/* We allow big file names */
#define BIG_FILENAME 1024

/* Includes */
#include "global.h"


/*********************************************************
*NAME:          utilPtoCString
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Convert Bolo's network pascal string to C strings
*
*ARGUMENTS:
*  src  - Source string
*  dest - Destination string 
*********************************************************/
void utilPtoCString(char *src, char *dest);

/*********************************************************
*NAME:          utilPtoCString
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Convert a C string to a Bolo's network pascal string 
*
*ARGUMENTS:
*  src  - Source string
*  dest - Destination string 
*********************************************************/
void utilCtoPString(char *src, char *dest);

/*********************************************************
*NAME:          utilGetNibbles
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 27/2/99
*PURPOSE:
* Extacts the high and low nibbles out of a byte
*
*ARGUMENTS:
*  value - The byte the nibbles come from
*  high - Pointer to hold high nibble
*  low  - Pointer to hold low nibble
*********************************************************/
void utilGetNibbles(BYTE value, BYTE *high, BYTE *low);

/*********************************************************
*NAME:          utilPutNibble
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 27/2/99
*PURPOSE:
* Returns the high and low nibbles as a combined byte
*
*ARGUMENTS:
*  high - High nibble
*  low  - Low nibble
*********************************************************/
BYTE utilPutNibble(BYTE high, BYTE low);

#endif /* UTILS_H */
