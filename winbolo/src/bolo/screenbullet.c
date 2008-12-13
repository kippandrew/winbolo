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
*Name:          Shells
*Filename:      shells.c
*Author:        John Morrison
*Creation Date: 25/12/98
*Last Modified: 25/12/98
*Purpose:
*  Responsable for Shells tracking/collision detect etc.
*********************************************************/

#include "global.h"
#include "util.h"
#include "screenbullet.h"

/*********************************************************
*NAME:          screenBulletsCreate
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Sets up the screen bullets data structure
*
*ARGUMENTS:
*
*********************************************************/
screenBullets screenBulletsCreate(void) {
	return NULL;
}

/*********************************************************
*NAME:          screenBulletsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Destroys and frees memory for the data structure
*
*ARGUMENTS:
*  value - Pointer to the screenBullets data structure
*********************************************************/
void screenBulletsDestroy(screenBullets *value) {
  screenBullets q;

  while (!IsEmpty(*value)) {
    q = *value;
    *value = ScreenBulletTail(q);
    Dispose(q);
  }
}

/*********************************************************
*NAME:          screenBulletsAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Adds an item to the screenBullets data structure.
*
*ARGUMENTS:
*  value - Pointer to the screenBullets data structure
*  mx    - X co-ord of the map position
*  my    - Y co-ord of the map position
*  px    - X pixel offset
*  py    - Y pixel offset
*  frame - Frame identifer of the bullet
*********************************************************/
void screenBulletsAddItem(screenBullets *value, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame) {
  screenBullets q;
  
  New (q);
  q->mx = mx;
  q->my = my;
  q->px = px;
  q->py = py;
  q->frame = frame;
  q->next = *value;
  *value = q;
}

/*********************************************************
*NAME:          screenBulletsGetNumEntries
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Returns the number of elements in the data structure
*
*ARGUMENTS:
*  value - Pointer to the screenBullets data structure
*********************************************************/
int screenBulletsGetNumEntries(screenBullets *value) {
  int returnValue; /* Value to return */
  screenBullets q;

  q = *value;
  returnValue = 0;

  while (NonEmpty(q)) {
    returnValue++;
    q = ScreenBulletTail(q);
  }

  return returnValue;
}

/*********************************************************
*NAME:          screenBulletsGetItem
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Gets data for a specific item
*
*ARGUMENTS:
*  value   - Pointer to the screenBullets data structure
*  itemNum - The item number to get
*  mx    - X co-ord of the map position
*  my    - Y co-ord of the map position
*  px    - X pixel offset
*  py    - Y pixel offset
*  frame - Frame identifer of the bullet
*********************************************************/
void screenBulletsGetItem(screenBullets *value, int itemNum, BYTE *mx, BYTE *my, BYTE *px, BYTE *py, BYTE *frame) {
  int count; /* Looping variable */
  screenBullets q;

  count = 1;
  q = *value;

  while (NonEmpty(q) && count < itemNum) {
    count++;
    q = ScreenBulletTail(q);
  }

  if (q != NULL) {
    *mx = q->mx;
    *my = q->my;
    *px = q->px;
    *py = q->py;
    *frame = q->frame;
  } else {
    //FIXME: This shouldn't happen though
  }
}
