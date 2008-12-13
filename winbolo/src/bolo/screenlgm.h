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
*Filename:      screenLgm.h
*Author:        John Morrison
*Creation Date: 19/2/99
*Last Modified: 19/2/99
*Purpose:
*  Responsable for Lgms on the screen
*********************************************************/

#ifndef SCREENLGMS_H
#define SCREENLGMS_H

#include "global.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define ScreenLgmsHeadMX(list) ((list)->mx);
#define ScreenLgmsHeadMY(list) ((list)->my);
#define ScreenLgmsHeadPX(list) ((list)->px);
#define ScreenLgmsHeadPY(list) ((list)->py);
#define ScreenLgmsHeadFrame(list) ((list)->frame);
#define ScreenLgmsTail(list) ((list)->next);

/* Type structure */

typedef struct screenLgmObj *screenLgm;
struct screenLgmObj {
  screenLgm next; /* Next item */
  BYTE mx;        /* The map x co-ordinate it is on */
  BYTE my;        /* The map y co-ordinate it is on */
  BYTE px;        /* The pixel offset from the left it is on */
  BYTE py;        /* The pixel offset from the top it is on */
  BYTE frame;     /* The direction it is facing */
};


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
void screenLgmCreate(screenLgm *value);

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
*  value  - Pointer to the screenLgm data structure
*  leftPos   - Left bounds of the screen
*  rightPos  - Right bounds of the screen
*  top    - Top bounds of the screen
*  bottom - Bottom bounds of the screen
*********************************************************/
void screenLgmPrepare(screenLgm *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

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
BYTE screenLgmGetNumEntries(screenLgm *value);

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
void screenLgmDestroy(screenLgm *value);

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
void screenLgmAddItem(screenLgm *value, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame);

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
void screenLgmGetItem(screenLgm *value, BYTE itemNum, BYTE *mx, BYTE *my, BYTE *px, BYTE *py, BYTE *frame);

#endif /* SCREENLGMS_H */

