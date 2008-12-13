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
*Filename:      floodfill.c
*Author:        John Morrison
*Creation Date: 19/1/99
*Last Modified: 19/1/99
*Purpose:
*  Responsible for tracking flood fills caused by craters
*  next to water.
*********************************************************/

#include "global.h"
#include "bolo_map.h"
#include "bases.h"
#include "pillbox.h"
#include "screen.h"
#include "mines.h"
#include "backend.h"
#include "floodfill.h"

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
void floodCreate(floodFill *ff) {
  *ff = NULL;
}

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
void floodDestroy(floodFill *ff) {
  floodFill q;

  while (!IsEmpty(*ff)) {
    q = *ff;
    *ff = FloodFillTail(q);
    Dispose(q);
  }
}

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
void floodAddItem(floodFill *ff, BYTE x, BYTE y) {
  floodFill q;
  floodFill inc;
  floodFill prev;
  bool found;       /* Is the item found */
  
  prev = inc = *ff;
  found = FALSE;
  
  while (found == FALSE && NonEmpty(inc)) {
    if (inc->x == x && inc->y == y) {
      found = TRUE;
    }
    prev = inc;
    inc = FloodFillTail(inc);
  }

  /* If not found add a new item */
  if (found == FALSE && IsEmpty(*ff)) {
    New(q);
    q->x = x;
    q->y = y;
    q->time = FLOOD_FILL_WAIT;
    q->next = *ff;
    *ff = q;
  } else if (found == FALSE) {
    New (q);
    q->x = x;
    q->y = y;
    q->time = FLOOD_FILL_WAIT;
    q->next = NULL;
    prev->next = q;
  }
}

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
void floodUpdate(floodFill *ff, map *mp, pillboxes *pb, bases *bs) {
  floodFill position; /* Position throught the items */
  int itemCount;      /* Counts the number of items */
  bool needUpdate;    /* Whether an update is needed or not */

  position = *ff;
  itemCount = 0;

  while (NonEmpty(position)) {
    needUpdate = TRUE;
    itemCount++;
    if (position->time > 0) {
        position->time--;
    } else {
      /* Check for fill and remove from data structure */
      needUpdate = FALSE;
      floodCheckFill(ff, mp, pb, bs, position->x, position->y);
      position = FloodFillTail(position);
      floodDeleteItem(ff, itemCount);
      itemCount--;
    }
    
    /* Get the next Item */
    if (*ff != NULL && needUpdate == TRUE) {
      position = FloodFillTail(position);
    } else {
      position = NULL;
    }
  }
}

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
void floodDeleteItem(floodFill *ff, int itemNum) {
  floodFill prev; /* The previous item to link to the delete items next */
  floodFill del;  /* The item to delete */
  int count;      /* Looping variable */

  if (itemNum == 1) {
    del = *ff;
    *ff = del->next;
    Dispose(del);
  } else {
    count = 1;
    prev = *ff;
    while (count < (itemNum-1)) {
      prev = FloodFillTail(prev);
      count++;
    }
    del = FloodFillTail(prev);
    prev->next = del->next;
    Dispose(del);
  }
}

/*********************************************************
*NAME:          floodCheckFill
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/99
*LAST MODIFIED: 21/3/99
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
void floodCheckFill(floodFill *ff, map *mp, pillboxes *pb, bases *bs, BYTE mx, BYTE my) {
  BYTE above;       /* Squares around */
  BYTE below;
  BYTE leftPos;
  BYTE rightPos;

  above = mapGetPos(mp, mx, (BYTE) (my-1));
  below = mapGetPos(mp, mx, (BYTE) (my+1));
  leftPos = mapGetPos(mp, (BYTE) (mx-1), my);
  rightPos = mapGetPos(mp, (BYTE) (mx+1), my);

  /* Check for pills, bases etc. If found change to non crater / water */
  if (pillsExistPos(pb, mx, (BYTE) (my-1)) == TRUE) {
    above = ROAD;
  } else if (basesExistPos(bs, mx, (BYTE) (my-1)) == TRUE) {
    above = ROAD;
  }

  if (pillsExistPos(pb, mx, (BYTE) (my+1)) == TRUE) {
    below  = ROAD;
  } else if (basesExistPos(bs, mx, (BYTE) (my+1)) == TRUE) {
    below = ROAD;
  }

  if (pillsExistPos(pb, (BYTE) (mx-1), my) == TRUE) {
    leftPos = ROAD;
  } else if (basesExistPos(bs, (BYTE) (mx-1),  my) == TRUE) {
    leftPos = ROAD;
  }

  if (pillsExistPos(pb, (BYTE) (mx+1), my) == TRUE) {
    rightPos = ROAD;
  } else if (basesExistPos(bs, (BYTE) (mx-1),  my) == TRUE) {
    rightPos = ROAD;
  }



  if (leftPos == DEEP_SEA || leftPos == BOAT || leftPos == RIVER || rightPos == DEEP_SEA || rightPos == BOAT || rightPos == RIVER || above == DEEP_SEA || above == RIVER || above == BOAT || below == DEEP_SEA || below == BOAT || below == RIVER) {
    /* Do fill */
    mapSetPos(mp, mx, my, RIVER, FALSE, FALSE);
    minesRemoveItem(screenGetMines(), mx, my);

    /* Add items if craters */
    if (leftPos == CRATER || leftPos == MINE_CRATER) {
      floodAddItem(ff, (BYTE) (mx-1), my);
    }
    if (rightPos == CRATER || rightPos == MINE_CRATER) {
      floodAddItem(ff, (BYTE) (mx+1), my);
    }
    if (above == CRATER || above == MINE_CRATER) {
      floodAddItem(ff, mx, (BYTE) (my-1));
    }
    if (below == CRATER || below == MINE_CRATER) {
      floodAddItem(ff, mx, (BYTE) (my+1));
    }
    screenReCalc();
  }
}
