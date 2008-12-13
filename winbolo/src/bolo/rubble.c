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
*Name:          Rubble
*Filename:      rubble.c
*Author:        John Morrison
*Creation Date: 30/12/98
*Last Modified: 25/04/01
*Purpose:
*  Responsable for tracking lifetime of rubble.
*  buildings can be shot 5 times before being destroyed
*********************************************************/

#include "global.h"
#include "rubble.h"

/*********************************************************
*NAME:          rubbleCreate
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 30/12/98
*PURPOSE:
*  Sets up the rubble data structure
*
*ARGUMENTS:
*  rbl - Pointer to the rubbble object
*********************************************************/
void rubbleCreate(rubble *rbl) {
  *rbl = NULL;
}

/*********************************************************
*NAME:          rubbleDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 30/12/98
*PURPOSE:
*  Destroys and frees memory for the rubble data structure
*
*ARGUMENTS:
*  rbl - Pointer to the rubbble object
*********************************************************/
void rubbleDestroy(rubble *rbl) {
  rubble q;

  while (!IsEmpty(*rbl)) {
    q = *rbl;
    *rbl = RubbleTail(q);
    Dispose(q);
  }
}

/*********************************************************
*NAME:          rubbleAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 25/04/01
*PURPOSE:
*  Adds an item to the rubble data structure.
*  If it already exists returns the terrain type of the
*  item and decrements its lifetime.
*
*ARGUMENTS:
*  rbl - Pointer to the rubbble object
*  x   - X co-ord
*  y   - Y co-ord
*********************************************************/
BYTE rubbleAddItem(rubble *rbl, BYTE x, BYTE y) {
  BYTE returnValue; /* Value to return */
  bool found;       /* Is the item found */
  int count;        /* Looping Variable */
  rubble q;
  rubble inc;

  inc = *rbl;
  found = FALSE;
  returnValue = RUBBLE;
  count = 0;
  
  while (found == FALSE && NonEmpty(inc)) {
    count++;
    if (inc->x == x && inc->y == y) {
      found = TRUE;
      inc->life--;
      if (inc->life == RUBBLE_DEATH) {
        returnValue = RIVER;
        rubbleDeleteItem(rbl, count);
      } 
    }
    if (found == FALSE) {
      inc = RubbleTail(inc);
	  }
  }

  /* If not found add a new item */
  if (found == FALSE) {
    New (q);
    q->x = x;
    q->y = y;
    q->life = RUBBLE_LIFE;
    q->next = *rbl;
    *rbl = q;
  }

  return returnValue;
}

/*********************************************************
*NAME:          rubbleDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 30/12/98
*PURPOSE:
*  Deletes the item for the given number
*
*ARGUMENTS:
*  rbl     - Pointer to the rubbble object
*  itemNum - The item number to get
*********************************************************/
void rubbleDeleteItem(rubble *rbl, int itemNum) {
  rubble prev; /* The previous item to link to the delete items next */
  rubble del;  /* The item to delete */
  int count;   /* Looping variable */

  if (itemNum == 1) {
    del = *rbl;
    *rbl = del->next;
    Dispose(del);
  } else {
    count = 1;
    prev = *rbl;
    while (count < (itemNum-1)) {
      prev = RubbleTail(prev);
      count++;
    }
    del = RubbleTail(prev);
    prev->next = del->next;
    Dispose(del);
  }
}


/*********************************************************
*NAME:          rubbleRemovePos
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  Removes an item from the rubble data structure if it 
*  exists at a specific loaction. Otherwise the function
*  does nothing
*
*ARGUMENTS:
*  rbl - Pointer to the rubbble object
*  x   - X co-ord
*  y   - Y co-ord
*********************************************************/
void rubbleRemovePos(rubble *rbl, BYTE x, BYTE y) {
  bool found;       /* Is the item found */
  int count;        /* Looping Variable */
  rubble inc;

  inc = *rbl;
  found = FALSE;
  count = 0;
  
  while (found == FALSE && NonEmpty(inc)) {
    count++;
    if (inc->x == x && inc->y == y) {
      found = TRUE;
    }
    inc = RubbleTail(inc);
  }

  /* If found remove item */
  if (found == TRUE) {
    rubbleDeleteItem(rbl, count);
  }
}
