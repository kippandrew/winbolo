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
*Name:          Screen Bullets
*Filename:      screenbullet.h
*Author:        John Morrison
*Creation Date: 25/12/98
*Last Modified: 25/12/98
*Purpose:
*  Responsable for Shells tracking/collision detect etc.
*********************************************************/

#ifndef SCREENBULLETS_H
#define SCREENBULLETS_H

#include "global.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define ScreenBulletHeadMX(list) ((list)->mx);
#define ScreenBulletHeadMY(list) ((list)->my);
#define ScreenBulletHeadPX(list) ((list)->px);
#define ScreenBulletHeadPY(list) ((list)->py);
#define ScreenBulletHeadDir(list) ((list)->dir);
#define ScreenBulletTail(list) ((list)->next);

/* Type structure */

typedef struct screenBulletsObj *screenBullets;
struct screenBulletsObj {
  screenBullets next;
  BYTE mx;    /* X Map co-ord of the bullet (Mapped to screen) */
  BYTE my;    /* Y Map co-ord of the bullet (Mapped to screen) */
  BYTE px;    /* X Pixel offset of the bullet */
  BYTE py;    /* Y Pixel offset of the bullet */
  BYTE frame; /* Frame identifier type */
};

/* Prototypes */

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
screenBullets screenBulletsCreate(void);

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
void screenBulletsAddItem(screenBullets *value, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame);

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
int screenBulletsGetNumEntries(screenBullets *value);

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
void screenBulletsDestroy(screenBullets *value);

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
void screenBulletsGetItem(screenBullets *value, int itemNum, BYTE *mx, BYTE *my, BYTE *px, BYTE *py, BYTE *frame);

#endif /* SCREENBULLETS_H */

