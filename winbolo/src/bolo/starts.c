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
*Filename:      starts.c 
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified:   9/4/00
*Purpose:
*  Provides operations on player starts 
*********************************************************/

/* Includes */
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "global.h"
#include "backend.h"
#include "players.h"
#include "screen.h"
#include "starts.h"

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
void startsCreate(starts *value) {
  New(*value);
  memset(*value, 0, sizeof(**value));
  ((*value)->numStarts) = 0;
}


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
void startsDestroy(starts *value) {
  if (*value != NULL) {
    Dispose(*value);
  }
}

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
void startsSetNumStarts(starts *value, BYTE numStarts) {
  if (numStarts <= MAX_STARTS) {
    (*value)->numStarts = numStarts;
  }
}


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
BYTE startsGetNumStarts(starts *value) {
  return (*value)->numStarts;
}

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
void startsSetStart(starts *value, start *item, BYTE startNum) {
  if (startNum > 0 && startNum  <= (*value)->numStarts) {
    startNum--;
    (((*value)->item[startNum]).x) = item->x;
    (((*value)->item[startNum]).y) = item->y;
    (((*value)->item[startNum]).dir) = item->dir;
  }
}

/*********************************************************
*NAME:          startsGetStartStruct
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
void startsGetStartStruct(starts *value, start *item, BYTE startNum) {
  if (startNum > 0 && startNum  <= (*value)->numStarts) {
    startNum--;
    item->x = ((*value)->item[startNum]).x;
    item->y = ((*value)->item[startNum]).y;
    item->dir = ((*value)->item[startNum]).dir;
  }
}

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
bool startsExistPos(starts *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return  */
  BYTE count;       /* Looping variable */

  returnValue = FALSE;
  count = 0;
  while (count < (*value)->numStarts && returnValue == FALSE) {
    if ((*value)->item[count].x == xValue && (*value)->item[count].y == yValue) {
      returnValue = TRUE;
    }
    count++;
  }

  return returnValue;
}

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
void startsGetStart(starts *value, BYTE *x, BYTE *y, TURNTYPE *dir, BYTE playerNum) {
  BYTE count; /* Looping variable */
  BYTE val;
  bool okTank;
  bool found;
  BYTE rnd;
  BYTE bt;      /* Used to convert the BMAP starts (0 = east) to my starts */

  val = 0;
  if (*value == NULL) {
    return;
  }
  if ((*value)->numStarts ==0) {
    return;
  }
  srand((unsigned int) time(NULL));
  srand((unsigned int) (rand() * time(NULL)));
  found = FALSE;
  count = 0;
  while (count < (*value)->numStarts && found == FALSE) {
    okTank = TRUE;
    rnd = (BYTE) ((rand()+1)% (*value)->numStarts);
    okTank = screenCheckTankRange((*value)->item[rnd].x, (*value)->item[rnd].y, playerNum, 512.0);
    if (okTank == TRUE) {
      found = TRUE;
      val = rnd;
    }
    count++;
  }
  if (found == TRUE) {
    *x = (*value)->item[val].x;
    *y = (*value)->item[val].y;
    bt = startsConvertDir(((*value)->item[val].dir));
    *dir = (TURNTYPE) (bt * START_TIMES_16);
  } else {
    /* Choose any */
    rnd = (BYTE) ((rand()+1)% (*value)->numStarts);
    *x = (*value)->item[rnd].x;
    *y = (*value)->item[rnd].y;
    bt = startsConvertDir(((*value)->item[rnd].dir));
    *dir = (TURNTYPE) (bt * START_TIMES_16);
  }
}

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
void startsGetRandStart(starts *value, BYTE *x, BYTE *y, TURNTYPE *dir) {
  int rnd;      /* Random number */
  BYTE bt;      /* Used to convert the BMAP starts (0 = east) to my starts */
  bool found;   /* Have we found a start? */
  WORLD testX;  /* Test world co-ordinates */
  WORLD testY;
  int dummy1;   /* Dummy variable for function parameters */
  int dummy2;
  srand((unsigned int) time(NULL));
  srand((unsigned int) (rand() * time(NULL)));
  rnd = rand();

  if ((*value)->numStarts > 0) {
    found = FALSE;
    while (rnd < 0 || rnd > (*value)->numStarts) {
      rnd = rand() % (*value)->numStarts;
    }
    if (rnd == 0) {
      rnd = 1;
    }
    while (found == FALSE) {
      testX = (*value)->item[(rnd-1)].x;
      testX <<= TANK_SHIFT_MAPSIZE;
      testY = (*value)->item[(rnd-1)].y;
      testY <<= TANK_SHIFT_MAPSIZE;
      testX += 256;
      testY += 256;
      if (playersCheckCollision(screenGetPlayers(), playersGetSelf(screenGetPlayers()), testX, testY, &dummy1, &dummy2) == FALSE) {
        found = TRUE;
        *x = (*value)->item[(rnd-1)].x;
        *y = (*value)->item[(rnd-1)].y;
        bt = startsConvertDir(((*value)->item[(rnd-1)].dir));
        *dir = (TURNTYPE) (bt * START_TIMES_16);
      } else {
        rnd++;
        if (rnd > (*value)->numStarts) {
          rnd = 1;
        }
      }
    }
  } else {
    *x = BRADIANS_GAP;
    *y = BRADIANS_GAP;
    *dir = BRADIANS_SOUTH;
  } 
}

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
BYTE startsConvertDir(BYTE dir) {
  BYTE returnValue; /* Value to return */

  switch (dir) {
  case START0:
    returnValue = START4;
    break;
    case START1:
    returnValue = START3;
    break;
  case START2:
    returnValue = START2;
    break;
  case START3:
    returnValue = START1;
    break;
  case START4:
    returnValue = START0;
    break;
  case START5:
    returnValue = START15;
    break;
  case START6:
    returnValue = START14;
    break;
  case START7:
    returnValue = START13;
    break;
  case START8:
    returnValue = START12;
    break;
  case START9:
    returnValue = START11;
    break;
  case START10:
    returnValue = START10;
    break;
  case START11:
    returnValue = START9;
    break;
  case START12:
    returnValue = START8;
    break;
  case START13:
    returnValue = START7;
    break;
  case START14:
    returnValue = START6;
    break;
  default:
    /* START15 */
    returnValue = START5;
    break;
  }

  return returnValue;
}

void startsSetStartCompressData(starts *value, BYTE *buff, int dataLen) {
  memcpy(&(**value), buff, SIZEOF_STARTS);
}

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
void startsSetStartNetData(starts *value, BYTE *buff, BYTE dataLen) {
  BYTE count = 0;
  BYTE len = 1;

  (*value)->numStarts = buff[0];
  while (count < (*value)->numStarts) {
    (*value)->item[count].x = buff[len];
    len++;
    (*value)->item[count].y = buff[len];
    len++;
    (*value)->item[count].dir = buff[len];
    len++;
    count++;
  }
}

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
BYTE startsGetStartNetData(starts *value, BYTE *buff) {
  BYTE len = 1;
  BYTE count = 0;

  buff[0] = (*value)->numStarts;
  while (count < (*value)->numStarts) {
    buff[len] = (*value)->item[count].x;
    len++;
    buff[len] = (*value)->item[count].y;
    len++;
    buff[len] = (*value)->item[count].dir;
    len++;
    count++;
  }
  return len;
}

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
*  valuePos  - Pointer to the starts structure
*  leftPos   - Pointer to hold the left most value
*  rightPos  - Pointer to hold the right most value
*  topPos    - Pointer to hold the top most value
*  bottomPos - Pointer to hold the bottom most value
*********************************************************/
void startsGetMaxs(starts *value, int *leftPos, int *rightPos, int *topPos, int *bottomPos) {
  BYTE count; /* Looping Variable */

  *topPos = MAP_ARRAY_SIZE;
  *bottomPos = -1;
  *leftPos = MAP_ARRAY_SIZE;
  *rightPos = -1;

  count = 0;
  while (count < ((*value)->numStarts)) {
    if ((*value)->item[count].x < *leftPos) {
      *leftPos = (*value)->item[count].x;
    }
    if ((*value)->item[count].x > *rightPos) {
      *rightPos = (*value)->item[count].x;
    }
    if ((*value)->item[count].y < *topPos) {
      *topPos = (*value)->item[count].y;
    }
    if ((*value)->item[count].y > *bottomPos) {
      *bottomPos = (*value)->item[count].y;
    }  
    count++;
  }
}

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
void startsMoveAll(starts *value, int moveX, int moveY) {
  BYTE count; /* Looping Variable */

  count = 0;
  while (count < ((*value)->numStarts)) {
    (*value)->item[count].x = (BYTE) ((*value)->item[count].x + moveX);
    (*value)->item[count].y = (BYTE) ((*value)->item[count].y + moveY);
    count++;
  }
}
