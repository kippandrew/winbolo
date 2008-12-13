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
*Name:          Building
*Filename:      building.h
*Author:        John Morrison
*Creation Date: 30/12/98
*Last Modified: 04/10/03
*Purpose:
*  Responsable for tracking lifetime of buildings.
*  buildings can be shot 5 times before being destroyed
*********************************************************/

#include "global.h"
#include "building.h"

/*********************************************************
*NAME:          buildingCreate
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 30/12/98
*PURPOSE:
*  Sets up the building data structure
*
*ARGUMENTS:
* bld - Pointer to the buildings object
*********************************************************/
void buildingCreate(building *bld) {
  *bld = NULL;
}

/*********************************************************
*NAME:          buildingDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 30/12/98
*PURPOSE:
*  Destroys and frees memory for the building data structure
*
*ARGUMENTS:
* bld - Pointer to the buildings object
*********************************************************/
void buildingDestroy(building *bld) {
  building q;

  while (!IsEmpty(*bld)) {
    q = *bld;
    *bld = BuildingTail(q);
    Dispose(q);
  }
}

/*********************************************************
*NAME:          buildingeAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 25/04/01
*PURPOSE:
*  Adds an item to the building data structure.
*  If it already exists returns the terrain type of the
*  item and decrements its lifetime.
*
*ARGUMENTS:
*  bld   - Pointer to the buildings object
*  x     - X co-ord
*  y     - Y co-ord
*********************************************************/
BYTE buildingAddItem(building *bld, BYTE x, BYTE y) {
  BYTE returnValue; /* Value to return */
  bool found;       /* Is the item found */
  int count;        /* Looping Variable */
  building q;
  building inc;

  inc = *bld;
  found = FALSE;
  returnValue = HALFBUILDING;
  count = 0;
  
  while (found == FALSE && NonEmpty(inc)) {
    count++;
    if (inc->x == x && inc->y == y) {
      found = TRUE;
      inc->life--;
      if (inc->life == BUILDING_DEATH) {
        returnValue = RUBBLE;
        buildingDeleteItem(bld, count);
      } 
	  }
    if (found == FALSE) {
		  inc = BuildingTail(inc);
    }    
  }

  /* If not found add a new item */
  if (found == FALSE) {
    New (q);
    q->x = x;
    q->y = y;
    q->life = BUILDING_LIFE;
    q->next = *bld;
    *bld = q;
  }

  return returnValue;
}

/*********************************************************
*NAME:          buildingDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 30/12/98
*PURPOSE:
*  Deletes the item for the given number
*
*ARGUMENTS:
*  bld     - Pointer to the buildings object
*  itemNum - The item number to get
*********************************************************/
void buildingDeleteItem(building *bld, int itemNum) {
  building prev; /* The previous item to link to the delete items next */
  building del;  /* The item to delete */
  int count;     /* Looping variable */

  if (itemNum == 1) {
    del = *bld;
    *bld = del->next;
    Dispose(del);
  } else {
    count = 1;
    prev = *bld;
    while (count < (itemNum-1)) {
      prev = BuildingTail(prev);
      count++;
    }
    del = BuildingTail(prev);
    prev->next = del->next;
    Dispose(del);
  }
}


/*********************************************************
*NAME:          buildingRemovePos
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  Removes an item from the building data structure if it 
*  exists at a specific loaction. Otherwise the function
*  does nothing
*
*ARGUMENTS:
*  bld   - Pointer to the buildings object
*  x     - X co-ord
*  y     - Y co-ord
*********************************************************/
void buildingRemovePos(building *bld, BYTE x, BYTE y) {
  bool found;       /* Is the item found */
  int count;        /* Looping Variable */
  building inc;

  inc = *bld;
  found = FALSE;
  count = 0;
  
  while (found == FALSE && NonEmpty(inc)) {
    count++;
    if (inc->x == x && inc->y == y) {
      found = TRUE;
    }
    inc = BuildingTail(inc);
  }

  /* If found remove item */
  if (found == TRUE) {
    buildingDeleteItem(bld, count);
  }
}
