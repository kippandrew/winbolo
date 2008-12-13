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
*Name:          Swamp
*Filename:      swamp.h
*Author:        John Morrison
*Creation Date: 5/1/99
*Last Modified: 18/1/99
*Purpose:
*  Responsable for tracking lifetime of swamp when shot
*  from a boat
*********************************************************/

#ifndef SWAMP_H
#define SWAMP_H

#include "global.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define SwampHeadX(list) ((list)->x);
#define SwampHeadY(list) ((list)->y);
#define SwampHeadLength(list) ((list)->life);
#define SwampTail(list) ((list)->next);

/* How manu shots it takes to destroy a peice of grass */
#define SWAMP_LIFE 4

/* Shells die when there length equals */
#define SWAMP_DEATH 0

/* What grass truns into when it dies */
#define SWAMP_DEATH_RETURN RIVER

/* Type structure */

typedef struct swampObj *swamp;
struct swampObj {
  swamp next; /* Next item */ 
  BYTE x;     /* X and Y positions */
  BYTE y;
  BYTE life;  /* Life time of the swamp */
};

/* Prototypes */

/*********************************************************
*NAME:          swampCreate
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 5/1/99
*PURPOSE:
*  Sets up the swamp data structure
*
*ARGUMENTS:
*
*********************************************************/
void swampCreate(swamp *swmp);

/*********************************************************
*NAME:          swampDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 5/1/99
*PURPOSE:
*  Destroys and frees memory for the swamp data structure
*
*ARGUMENTS:
*
*********************************************************/
void swampDestroy(swamp *swmp);

/*********************************************************
*NAME:          swampAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 25/04/01
*PURPOSE:
*  Adds an item to the swamp data structure.
*  If it already exists returns the terrain type of the
*  item and decrements its lifetime.
*
*ARGUMENTS:
*  x     - X co-ord
*  y     - Y co-ord
*********************************************************/
BYTE swampAddItem(swamp *swmp, BYTE x, BYTE y);

/*********************************************************
*NAME:          swampDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 5/1/99
*PURPOSE:
*  Deletes the item for the given number
*
*ARGUMENTS:
*  itemNum - The item number to get
*********************************************************/
void swampDeleteItem(swamp *swmp, int itemNum);

/*********************************************************
*NAME:          swampRemovePos
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  Removes an item from the swamp data structure if it 
*  exists at a specific loaction. Otherwise the function
*  does nothing
*
*ARGUMENTS:
*  x     - X co-ord
*  y     - Y co-ord
*********************************************************/
void swampRemovePos(swamp *swmp, BYTE x, BYTE y);

#endif /* SWAMP_H */


