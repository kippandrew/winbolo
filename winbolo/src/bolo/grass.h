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
*Name:          Grass
*Filename:      grass.h
*Author:        John Morrison
*Creation Date: 5/1/99
*Last Modified: 18/1/99
*Purpose:
*  Responsable for tracking lifetime of grass when shot
*  from a boat
*********************************************************/

#ifndef GRASS_H
#define GRASS_H

#include "global.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define GrassHeadX(list) ((list)->x);
#define GrassHeadY(list) ((list)->y);
#define GrassHeadLength(list) ((list)->life);
#define GrassTail(list) ((list)->next);

/* How manu shots it takes to destroy a peice of grass */
#define GRASS_LIFE 4

/* Shells die when there length equals */
#define GRASS_DEATH 0

/* What grass truns into when it dies */
#define GRASS_DEATH_RETURN SWAMP

/* Type structure */

typedef struct grassObj *grass;
struct grassObj {
  grass next; /* Next item */ 
  BYTE x;     /* X and Y positions */
  BYTE y;
  BYTE life;  /* Life time of the Grass */
};

/* Prototypes */

/*********************************************************
*NAME:          grassCreate
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 5/1/99
*PURPOSE:
*  Sets up the grass data structure
*
*ARGUMENTS:
* grs - Pointer to the grass object
*********************************************************/
void grassCreate(grass *grs);

/*********************************************************
*NAME:          grassDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 5/1/99
*PURPOSE:
*  Destroys and frees memory for the grass data structure
*
*ARGUMENTS:
* grs - Pointer to the grass object
*********************************************************/
void grassDestroy(grass *grs);

/*********************************************************
*NAME:          grassAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 25/04/01
*PURPOSE:
*  Adds an item to the grass data structure.
*  If it already exists returns the terrain type of the
*  item and decrements its lifetime.
*
*ARGUMENTS:
*  grs   - Pointer to the grass object
*  x     - X co-ord
*  y     - Y co-ord
*********************************************************/
BYTE grassAddItem(grass *grs, BYTE x, BYTE y);

/*********************************************************
*NAME:          grassDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 5/1/99
*PURPOSE:
*  Deletes the item for the given number
*
*ARGUMENTS:
*  grs     - Pointer to the grass object
*  itemNum - The item number to get
*********************************************************/
void grassDeleteItem(grass *grs, int itemNum);

/*********************************************************
*NAME:          grassRemovePos
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  Removes an item from the grass data structure if it 
*  exists at a specific loaction. Otherwise the function
*  does nothing
*
*ARGUMENTS:
*  grs - Pointer to the grass object
*  x   - X co-ord
*  y   - Y co-ord
*********************************************************/
void grassRemovePos(grass *grs, BYTE x, BYTE y);

#endif /* GRASS_H */


