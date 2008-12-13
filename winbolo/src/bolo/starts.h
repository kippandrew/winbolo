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
*Name:          Starts 
*Filename:      starts.h 
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 24/07/04
*Purpose:
*  Provides operations on player starts 
*********************************************************/

#ifndef STARTS_H
#define STARTS_H


/* Includes */
#include "global.h"
#include "types.h"

#define START_TIMES_16 16
#define START0 0
#define START1 1
#define START2 2
#define START3 3
#define START4 4
#define START5 5
#define START6 6
#define START7 7
#define START8 8
#define START9 9
#define START10 10
#define START11 11
#define START12 12
#define START13 13
#define START14 14
#define START15 15

/* Prototypes */

/*********************************************************
*NAME:          startsCreate
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Creates and initilises the player starts structure.
*  Sets number of starts to zero
*
*ARGUMENTS:
*  value - Pointer to the starts structure 
*********************************************************/
void startsCreate(starts *value);

/*********************************************************
*NAME:          startsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Destroys the starts data structure. Also frees memory.
*
*ARGUMENTS:
*  value - Pointer to the starts structure
*********************************************************/
void startsDestroy(starts *value);

/*********************************************************
*NAME:          startsSetNumStarts
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets the number of starts in the structure 
*
*ARGUMENTS:
*  value     - Pointer to the starts structure
*  numStarts - The number of starts 
*********************************************************/
void startsSetNumStarts(starts *value, BYTE numStarts);

/*********************************************************
*NAME:          startsGetNumStarts
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Gets the number of starts in the structure
*
*ARGUMENTS:
*  value  - Pointer to the starts structure
*********************************************************/
BYTE startsGetNumStarts(starts *value);

/*********************************************************
*NAME:          startsSetStart
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets a specific start with its item data
*
*ARGUMENTS:
*  value    - Pointer to the starts structure
*  item     - Pointer to a player start 
*  startNum - The start number
*********************************************************/
void startsSetStart(starts *value, start *item, BYTE startNum);

/*********************************************************
*NAME:          startsExistPos
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  Returns if a start exists at a specific location
*
*ARGUMENTS:
*  value    - Pointer to the starts structure
*  xValue   - X Location to test
*  yValue   - Y Location to test
*********************************************************/
bool startsExistPos(starts *value, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          startsGetStart
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/98
*LAST MODIFIED: 9/2/98
*PURPOSE:
*  Gets a specific start.
*
*ARGUMENTS:
*  value    - Pointer to the starts structure
*  item     - Pointer to a player start 
*  startNum - The start number
*********************************************************/
void startsGetStartStruct(starts *value, start *item, BYTE startNum);

/*********************************************************
*NAME:          startsGetStart
*AUTHOR:        John Morrison
*CREATION DATE: 7/1/99
*LAST MODIFIED: 9/4/01
*PURPOSE:
*  Returns a random start position
*
*ARGUMENTS:
*  value     - Pointer to the starts structure
*  x         - X Value of the start
*  y         - Y Value of the start
*  dir       - Direction it is facing
*  playerNum - Player Number requesting start
*********************************************************/
void startsGetStart(starts *value, BYTE *x, BYTE *y, TURNTYPE *dir, BYTE playerNum);

/*********************************************************
*NAME:          startsGetRandStart
*AUTHOR:        John Morrison
*CREATION DATE: 7/1/99
*LAST MODIFIED: 9/4/01
*PURPOSE:
*  Returns a random start position. Was the original
*  startsGetStart() code
*
*ARGUMENTS:
*  value     - Pointer to the starts structure
*  x         - X Value of the start
*  y         - Y Value of the start
*  dir       - Direction it is facing
*********************************************************/ 
void startsGetRandStart(starts *value, BYTE *x, BYTE *y, TURNTYPE *dir);

/*********************************************************
*NAME:          startsConvertDir
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Converts a Bolo Map start (0 =East, 4 = North) to my
*  starts 0 = North, 4 = East etc.
*
*ARGUMENTS:
*  dir   - Direction it is facing
*********************************************************/
BYTE startsConvertDir(BYTE dir);

/*********************************************************
*NAME:          startsSetStartNetData
*AUTHOR:        John Morrison
*CREATION DATE: 27/02/99
*LAST MODIFIED: 24/07/04
*PURPOSE:
* Sets the starts data to buff from the network.
*
*ARGUMENTS:
*  value   - Pointer to the starts structure
*  buff    - Buffer of data to set starts structure to
*  dataLen - Length of the data
*********************************************************/
void startsSetStartNetData(starts *value, BYTE *buff, BYTE dataLen);

/*********************************************************
*NAME:          startsGetStartNetData
*AUTHOR:        John Morrison
*CREATION DATE: 27/02/99
*LAST MODIFIED: 24/07/04
*PURPOSE:
* Prepares the starts data for sending across the network
* Returns data length
*ARGUMENTS:
*  value - Pointer to the starts structure
*  buff  - Buffer to hold copy of starts data
*********************************************************/
BYTE startsGetStartNetData(starts *value, BYTE *buff);

/*********************************************************
*NAME:          startsGetMaxs
*AUTHOR:        John Morrison
*CREATION DATE: 13/6/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  Gets the maximum positions values of all the starts.
*  eg left most, right most etc.
*
*ARGUMENTS:
*  value  - Pointer to the starts structure
*  left   - Pointer to hold the left most value
*  right  - Pointer to hold the right most value
*  top    - Pointer to hold the top most value
*  bottom - Pointer to hold the bottom most value
*********************************************************/
void startsGetMaxs(starts *value, int *leftPos, int *rightPos, int *topPos, int *bottomPos);

/*********************************************************
*NAME:          startsMoveAll
*AUTHOR:        John Morrison
*CREATION DATE: 13/6/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  Repositions the starts by moveX, moveY
*
*ARGUMENTS:
*  value  - Pointer to the pills structure
*  moveX  - The X move amount 
*  moveY  - The Y move amount 
*********************************************************/
void startsMoveAll(starts *value, int moveX, int moveY);

void startsSetStartCompressData(starts *value, BYTE *buff, int dataLen);

#endif /* STARTS_H */

