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
*Name:          Screen LGM
*Filename:      screenLgm.c
*Author:        John Morrison
*Creation Date: 19/2/99
*Last Modified: 26/11/99
*Purpose:
*  Responsable for Lgms on the screen
*********************************************************/

#include "global.h"
#include "players.h"
#include "backend.h"
#include "screenlgm.h"

/* Prototypes */

/*********************************************************
*NAME:          screenLgmCreate
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED: 19/2/99
*PURPOSE:
*  Sets up the screen lgms data structure
*
*ARGUMENTS:
*  value - New item to create
*********************************************************/
void screenLgmCreate(screenLgm *value) {
  (*value) = NULL;
}

/*********************************************************
*NAME:          screenLgmPrepare
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED: 19/2/99
*PURPOSE:
*  Prepares the screenLgms data structure prior to
*  displaying
*
*ARGUMENTS:
*  value    - Pointer to the screenLgm data structure
*  leftPos  - Left bounds of the screen
*  rightPos - Right bounds of the screen
*  top      - Top bounds of the screen
*  bottom   - Bottom bounds of the screen
*********************************************************/
void screenLgmPrepare(screenLgm *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  playersMakeScreenLgm(value, leftPos, rightPos, top, bottom);
} 

/*********************************************************
*NAME:          screenLgmGetNumEntries
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED: 19/2/99
*PURPOSE:
*  Returns the number of elements in the data structure
*
*ARGUMENTS:
*  value - Pointer to the screenLgm data structure
*********************************************************/
BYTE screenLgmGetNumEntries(screenLgm *value) {
  BYTE returnValue; /* Value to return */
  screenLgm q;

  returnValue = 0;
  q = *value;
  while (NonEmpty(q)) {
    returnValue++;
    q = ScreenLgmsTail(q);
  }
  return returnValue;
}

/*********************************************************
*NAME:          screenLgmDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED: 19/2/99
*PURPOSE:
*  Destroys and frees memory for the data structure
*
*ARGUMENTS:
*  value - Pointer to the screenLgm data structure
*********************************************************/
void screenLgmDestroy(screenLgm *value) {
  screenLgm q;

  while(NonEmpty(*value)) {
    q = *value;
    (*value) = ScreenLgmsTail(q);
    Dispose(q);
  }
}

/*********************************************************
*NAME:          screenLgmAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/98
*LAST MODIFIED: 19/2/98
*PURPOSE:
*  Adds a data set for a specific lgm
*
*ARGUMENTS:
*  value      - Pointer to the screenBullets data structure
*  mx         - X co-ord of the map position
*  my         - Y co-ord of the map position
*  px         - X pixel offset
*  py         - Y pixel offset
*  frame      - Frame identifer of the tank
*********************************************************/
void screenLgmAddItem(screenLgm *value, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame) {
  screenLgm q;

  New(q);
  q->mx = mx;
  q->my = my;
  q->px = px;
  q->py = py;
  q->frame = frame;
  q->next = (*value);
  (*value) = q;
}

/*********************************************************
*NAME:          screenLgmGetItem
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/98
*LAST MODIFIED: 19/2/98
*PURPOSE:
*  Gets data for a specific item
*
*ARGUMENTS:
*  value      - Pointer to the screenLgm data structure
*  itemNum    - The item number to get
*  mx         - X co-ord of the map position
*  my         - Y co-ord of the map position
*  px         - X pixel offset
*  py         - Y pixel offset
*  frame      - Frame identifer of the LGM
*********************************************************/
void screenLgmGetItem(screenLgm *value, BYTE itemNum, BYTE *mx, BYTE *my, BYTE *px, BYTE *py, BYTE *frame) {
  BYTE count;  /* Looping variable */
  screenLgm q; 

  count = 1;
  q = *value;

  while (count < itemNum && NonEmpty(q)) {
    count++;
    q = ScreenLgmsTail(q);
  }
  if (q != NULL) {
    *mx = q->mx;
    *my = q->my;
    *px = q->px;
    *py = q->py;
    *frame = q->frame;
  } else {
    // FIXME
  }
}


