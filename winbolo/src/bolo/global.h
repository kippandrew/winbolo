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
typedef BYTE MAP_X, MAP_Y;
typedef float TURNTYPE;
typedef float SPEEDTYPE;
#ifdef _WIN32
typedef unsigned char u_char;
#endif

/* Boolean type */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0
typedef BYTE bool;
/*typedef BYTE Bool; */


/* The world co-ordinate for tank - It is a 16 bit number*/
/* Top 8 bits (or byte) represent position on the map */
/* Next 2 bits represent the tanks sub-map position in pixels (0-15) */
/* Last 2 bits sub 40 pixels */
typedef unsigned short WORLD;

/* WORD data type */
typedef unsigned short WORD;



typedef unsigned long PlayerBitMap;

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

#define STRINGSIZE FILENAME_MAX
#define SHORTSTRINGSIZE 32
typedef char *StringRef;
typedef char StringBuf[STRINGSIZE];
typedef char ShortStringBuf[SHORTSTRINGSIZE];


Generic emalloc(size_t size);
void efree(Generic object);

#define	New(p)		((p) = emalloc(sizeof(*(p))))
#define	Dispose(p)	(efree(p))

/* An Null Charector */
#define EMPTY_CHAR '\0'

/* The terrain types */

#define DEEP_SEA 0xFF 
#define BUILDING 0
#define RIVER 1
#define SWAMP 2
#define CRATER 3
#define ROAD 4
#define FOREST 5
#define RUBBLE 6
#define GRASS 7
#define HALFBUILDING 8
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

/* Base Numbers 1-16 instead of "Magic Numbers" */
#define BASE_0 0
#define BASE_1 1
#define BASE_2 2
#define BASE_3 3
#define BASE_4 4
#define BASE_5 5
#define BASE_6 6
#define BASE_7 7
#define BASE_8 8
#define BASE_9 9
#define BASE_10 10
#define BASE_11 11
#define BASE_12 12
#define BASE_13 13
#define BASE_14 14
#define BASE_15 15
#define BASE_16 16

/* PILLBOX Numbers 1-16 instead of "Magic Numbers" */
#define PILLBOX_0 0
#define PILLBOX_1 1
#define PILLBOX_2 2
#define PILLBOX_3 3
#define PILLBOX_4 4
#define PILLBOX_5 5
#define PILLBOX_6 6
#define PILLBOX_7 7
#define PILLBOX_8 8
#define PILLBOX_9 9
#define PILLBOX_10 10
#define PILLBOX_11 11
#define PILLBOX_12 12
#define PILLBOX_13 13
#define PILLBOX_14 14
#define PILLBOX_15 15
#define PILLBOX_16 16

/* Tank Numbers 1-16 instead of "Magic Numbers" */
#define TANK_0 0
#define TANK_1 1
#define TANK_2 2
#define TANK_3 3
#define TANK_4 4
#define TANK_5 5
#define TANK_6 6
#define TANK_7 7
#define TANK_8 8
#define TANK_9 9
#define TANK_10 10
#define TANK_11 11
#define TANK_12 12
#define TANK_13 13
#define TANK_14 14
#define TANK_15 15
#define TANK_16 16

/* Angular Meausrement stuff */
#define BRADIANS_MAX 256.0
#define DEGREES_MAX 360.0
#define RADIANS_MAX (2.0 * 3.14159)
#define BRADIANS_NORTH 0
#define BRADIANS_EAST 64
#define BRADIANS_SOUTH 128
#define BRADIANS_WEST 192
#define BRADIANS_NNEAST 16
#define BRADIANS_NEAST 32
#define BRADIANS_NEASTE 48
#define BRADIANS_SEASTE 80
#define BRADIANS_SEAST 96
#define BRADIANS_SSEAST 112
#define BRADIANS_SSWEST 144
#define BRADIANS_SWEST 160
#define BRADIANS_SWESTW 176
#define BRADIANS_NWESTW 208
#define BRADIANS_NWEST 224
#define BRADIANS_NNWEST 240
/* The gap between north to north north east to north east etc is 16 Bradians */
#define BRADIANS_GAP 16

/* Maths */
#define RADIAN_TO_DEGREE_FACTOR 57.2957795
#define DEGREE_TO_RADIAN_FACTOR 0.0174532925
#define BRADIAN_TO_RADIAN_FACTOR 0.024544
#define RADIAN_TO_BRADIAN_FACTOR 40.743665

/* Max Tanks in the game */
#define MAX_TANKS 16

/* Largest Value that will fill the WORLD data type */
#define WORLD_MAX 65535

/* Middle of a map square in world units */
#define MAP_SQUARE_MIDDLE 128
/* Was 128 */
/* In pixels */
#define MIDDLE_PIXEL 8

/* Game time is set to -1 if it an untimed game */
#define UNLIMITED_GAME_TIME -1

/* Player Name string lengths */
#define PLAYER_NAME_LEN 33
#define PLAYER_NAME_LAST 32 

/* These are for the static view when a tank dies.  The static
 * appears at different times depending on how the tank died. */
#define STATIC_ON_TICKS 145
#define STATIC_ON_TICKS_DEEPSEA 195
#define STATIC_ON_TICKS_MINES 145
#define STATIC_ON_TICKS_SHELL 145

/* How did the player's tank die? */
#define LAST_DEATH_BY_DEEPSEA 1
#define LAST_DEATH_BY_SHELL 2
#define LAST_DEATH_BY_MINES 0		/* changed this from 3 to 0 on feb 10, 2009 -- jhood */

/* Change the static bitmap displayed every 10 ticks */
#define STATIC_CHANGE_TICKS 10

/* 0xFF is neutral */
#define NEUTRAL 0xFF

/* Everytime something gets hit armour is decreased by this amount */
#define DAMAGE 5

#define M_W_SHIFT_SIZE 8

/* Version number */
#define STRVER "1.17"

/* Used to bitmask with a key code to see if it's pressed or not */
#define PRESSED 0x80

#include "../server/threads.h"

#endif /* GLOBAL_H */ 

