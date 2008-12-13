/*
 * $Id$
 *
 * Copyright (c) 2000-2008 John Morrison.
 * Copyright (c) 2000-2008 Andrew Roth.
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



#ifndef _GLOBAL_H  /* Double inclusion protection */
#define _GLOBAL_H
#define _UNICODE
#define UNICODE

/* Standard Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Byte type def */
typedef unsigned char  BYTE;

/* Boolean type */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0
typedef BYTE bool;
typedef BYTE Bool;

/* WORD data type */
typedef unsigned short WORD;


/* Strings of 36 charectors */
#define MAP_STR_SIZE 36


/* The type compatible with all other pointers types;
 * used where the corresponding type is indeterminate
 */
typedef void *Generic;

/* Fixed Strings, STRINGSIZE and SHORTSTRINGSIZE set buffer length.*/
#ifndef FILENAME_MAX  /* Usually somwhere in stdio.h */
#define FILENAME_MAX 256
#endif

#define	IP_SIZE	4


Generic emalloc(size_t size);
void efree(Generic object);

#define	New(p)		((p) = emalloc(sizeof(*(p))))
#define	Dispose(p)	(efree(p), p = NULL)


/* Game types  */
typedef enum {
  gameOpen = 1,
  gameTournament,
  gameStrictTournament
} gameType;


typedef enum {
  aiNone,
  aiYes,
  aiYesAdvantage
} aiType;


#define MAX_CONNECTIONS 128

typedef unsigned int SOCKET;

#define NEWLINE_CHAR '\n'
#define EMPTY_CHAR '\0'

#ifdef _WIN32
typedef unsigned int socklen_t;
#define	fd_set	FD_SET
#endif

#endif /* _GLOBAL_H */ 

