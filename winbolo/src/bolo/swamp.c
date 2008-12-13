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
*Filename:      swamp.c
*Author:        John Morrison
*Creation Date: 5/1/99
*Last Modified: 25/04/01
*Purpose:
*  Responsable for tracking lifetime of swamp when shot
*  from a boat
*********************************************************/

#include "global.h"
#include "swamp.h"

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
void swampCreate(swamp *swmp) {
  *swmp = NULL;
}

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
void swampDestroy(swamp *swmp) {
  swamp q;

  while (!IsEmpty(*swmp)) {
    q = *swmp;
    *swmp = SwampTail(q);
    Dispose(q);
  }
}

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
BYTE swampAddItem(swamp *swmp, BYTE x, BYTE y) {
  BYTE returnValue; /* Value to return */
  bool found;       /* Is the item found */
  int count;        /* Looping Variable */
  swamp q;
  swamp inc;

  inc = *swmp;
  found = FALSE;
  returnValue = SWAMP;
  count = 0;
  
  while (found == FALSE && NonEmpty(inc)) {
    count++;
    if (inc->x == x && inc->y == y) {
      found = TRUE;
      inc->life--;
      if (inc->life == SWAMP_DEATH) {
        returnValue = SWAMP_DEATH_RETURN;
        swampDeleteItem(swmp, count);
      }
        
	  }
    if (found == FALSE) {
      inc = SwampTail(inc);
    }
  }

  /* If not found add a new item */
  if (found == FALSE) {
    New (q);
    q->x = x;
    q->y = y;
    q->life = SWAMP_LIFE;
    q->next = *swmp;
    *swmp = q;
  }

  return returnValue;
}

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
void swampDeleteItem(swamp *swmp, int itemNum) {
  swamp prev; /* The previous item to link to the delete items next */
  swamp del;  /* The item to delete */
  int count;  /* Looping variable */

  if (itemNum == 1) {
    del = *swmp;
    *swmp = del->next;
    Dispose(del);
  } else {
    count = 1;
    prev = *swmp;
    while (count < (itemNum-1)) {
      prev = SwampTail(prev);
      count++;
    }
    del = SwampTail(prev);
    prev->next = del->next;
    Dispose(del);
  }
}

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
void swampRemovePos(swamp *swmp, BYTE x, BYTE y) {
  bool found;       /* Is the item found */
  int count;        /* Looping Variable */
  swamp inc;

  inc = *swmp;
  found = FALSE;
  count = 0;
  
  while (found == FALSE && NonEmpty(inc)) {
    count++;
    if (inc->x == x && inc->y == y) {
      found = TRUE;
    }
    inc = SwampTail(inc);
  }

  /* If found remove item */
  if (found == TRUE) {
    swampDeleteItem(swmp, count);
  }
}

