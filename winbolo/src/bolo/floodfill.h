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
*Name:          Flood Fill
*Filename:      floodfill.h
*Author:        John Morrison
*Creation Date: 19/1/99
*Last Modified: 21/1/99
*Purpose:
*  Responsible for tracking flood fills caused by craters
*  next to water.
*********************************************************/

#ifndef FLOOD_H
#define FLOOD_H

#include "global.h"
#include "types.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define FloodFillTail(list) ((list)->next);

/* Time between glood fill removal checks */
#define FLOOD_FILL_WAIT 16

/* Type structure */

typedef struct floodFillObj *floodFill;
struct floodFillObj {
  floodFill next; /* Next item */ 
  BYTE x;         /* X and Y positions */
  BYTE y;
  BYTE time;      /* Time remaing before it fills and checks next spot */
};

/* Prototypes */

/*********************************************************
*NAME:          floodCreate
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/99
*LAST MODIFIED: 19/1/99
*PURPOSE:
*  Sets up the flood fill data structure
*
*ARGUMENTS:
*  ff - Pointer to the floodFill item
*********************************************************/
void floodCreate(floodFill *ff);

/*********************************************************
*NAME:          floodDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/99
*LAST MODIFIED: 19/1/99
*PURPOSE:
*  Destroys and frees memory for the flood fill data 
*  structure
*
*ARGUMENTS:
*  ff - Pointer to the floodFill item
*********************************************************/
void floodDestroy(floodFill *ff);

/*********************************************************
*NAME:          floodAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/99
*LAST MODIFIED: 19/1/99
*PURPOSE:
*  Adds an item to the flood data structure.
*
*ARGUMENTS:
*  ff - Pointer to the floodFill item
*  x  - X co-ord
*  y  - Y co-ord
*********************************************************/
void floodAddItem(floodFill *ff, BYTE x, BYTE y);

/*********************************************************
*NAME:          floodUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/99
*LAST MODIFIED: 21/1/99
*PURPOSE:
*  Game tick has happened. Update flooding
*
*ARGUMENTS:
*  ff - Pointer to the floodFill item
*  mp - Pointer to the map structure
*  pb - Pointer to the pillboxes structure
*  bs - Pointer to the bases structure
*********************************************************/
void floodUpdate(floodFill *ff, map *mp, pillboxes *pb, bases *bs);

/*********************************************************
*NAME:          floodDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/99
*LAST MODIFIED: 19/1/99
*PURPOSE:
*  Deletes the item for the given number
*
*ARGUMENTS:
*  ff      - Pointer to the floodFill item
*  itemNum - The item number to get
*********************************************************/
void floodDeleteItem(floodFill *ff, int itemNum);

/*********************************************************
*NAME:          floodCheckFill
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/99
*LAST MODIFIED: 21/1/99
*PURPOSE:
*  Time to fill if required. Also if it does adds 
*  surrounding items to flood Data Structure.
*
*ARGUMENTS:
*  ff - Pointer to the floodFill item
*  mp - Pointer to the map structure
*  pb - Pointer to the pillboxes structure
*  bs - Pointer to the bases structure
*  mx - Map X Position
*  my - Map Y Position
*********************************************************/
void floodCheckFill(floodFill *ff, map *mp, pillboxes *pb, bases *bs, BYTE mx, BYTE my);

#endif /* FLOOD_H */


