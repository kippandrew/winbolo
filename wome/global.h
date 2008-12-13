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

#ifndef GLOBAL_H  /* Double inclusion protection */
#define GLOBAL_H

/* Standard Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Byte type def */
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef BYTE MAP_X, MAP_Y;


/* Boolean type */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0
typedef int bool;

/* The type compatible with all other pointers types;
 * used where the corresponding type is indeterminate
 */
typedef void *Generic;

/* Fixed Strings, STRINGSIZE and SHORTSTRINGSIZE set buffer length.*/
#ifndef FILENAME_MAX  /* Usually somwhere in stdio.h */
#define FILENAME_MAX 256
#endif


Generic emalloc(size_t size);
Generic erealloc(Generic obj);
void efree(Generic object);


#define New(p) ((p) = emalloc(sizeof(*(p))))
#define Dispose(p) (efree(p), p = NULL)

#define DEEP_SEA 0xFF
#define BUILDING 0
#define RIVER 1
#define SWAMP 2
#define CRATER 3
#define ROAD 4
#define FOREST 5
#define RUBBLE 6
#define GRASS 7
#define SHOT_BUILDING 8
#define BOAT 9
#define MINE_SWAMP 10
#define MINE_CRATER 11
#define MINE_ROAD 12
#define MINE_FOREST 13
#define MINE_RUBBLE 14
#define MINE_GRASS 15

/* Mines start at the 10th element */
#define MINE_START 10 
#define MINE_END 15
#define MINE_SUBTRACT 8

/* Urls */

#define TILES "/images/"
#define CGI_FILE "/cgi-bin/a.out"
#define UPLOAD_FILE "http://www.website.com/path/to/upload.html"
	
/* Header and footer files */
#define FILE_HEAD "/head.htm"
#define FILE_TAIL "/tail.htm"

#define ERROR_HEAD "/errhead.html"
#define ERROR_TAIL "/errtail.html"
	
#define E_MAP_PATH "/usr/lib/cgi-bin/e.map"

#endif /* GLOBAL_H */ 

