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
*Filename:      grass.c
*Author:        John Morrison
*Creation Date: 5/1/99
*Last Modified: 25/04/01
*Purpose:
*  Responsable for tracking lifetime of grass when shot
*  from a boat
*********************************************************/

#include "global.h"
#include "grass.h"

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
void grassCreate(grass *grs) {
  *grs = NULL;
}

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
void grassDestroy(grass *grs) {
  grass q;

  while (!IsEmpty(*grs)) {
    q = *grs;
    *grs = GrassTail(q);
    Dispose(q);
  }
}

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
BYTE grassAddItem(grass *grs, BYTE x, BYTE y) {
  BYTE returnValue; /* Value to return */
  bool found;       /* Is the item found */
  int count;        /* Looping Variable */
  grass q;
  grass inc;

  inc = *grs;
  found = FALSE;
  returnValue = GRASS;
  count = 0;
  
  while (found == FALSE && NonEmpty(inc)) {
    count++;
    if (inc->x == x && inc->y == y) {
      found = TRUE;
      inc->life--;
      if (inc->life == GRASS_DEATH) {
        returnValue = GRASS_DEATH_RETURN;
        grassDeleteItem(grs, count);
      } 
	  }
    if (found == FALSE) {
		  inc = GrassTail(inc);
    }
  }

  /* If not found add a new item */
  if (found == FALSE) {
    New (q);
    q->x = x;
    q->y = y;
    q->life = GRASS_LIFE;
    q->next = *grs;
    *grs = q;
  }

  return returnValue;
}

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
void grassDeleteItem(grass *grs, int itemNum) {
  grass prev; /* The previous item to link to the delete items next */
  grass del;  /* The item to delete */
  int count;  /* Looping variable */

  if (itemNum == 1) {
    del = *grs;
    *grs = del->next;
    Dispose(del);
  } else {
    count = 1;
    prev = *grs;
    while (count < (itemNum-1)) {
      prev = GrassTail(prev);
      count++;
    }
    del = GrassTail(prev);
    prev->next = del->next;
    Dispose(del);
  }
}

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
void grassRemovePos(grass *grs, BYTE x, BYTE y) {
  bool found;       /* Is the item found */
  int count;        /* Looping Variable */
  grass inc;

  inc = *grs;
  found = FALSE;
  count = 0;
  
  while (found == FALSE && NonEmpty(inc)) {
    count++;
    if (inc->x == x && inc->y == y) {
      found = TRUE;
    }
    inc = GrassTail(inc);
  }

  /* If found remove item */
  if (found == TRUE) {
    grassDeleteItem(grs, count);
  }
}

