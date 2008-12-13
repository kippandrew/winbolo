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
*Name:          Mines
*Filename:      mines.c 
*Author:        John Morrison
*Creation Date: 29/1/99
*Last Modified: 2/11/99
*Purpose:
*  Handles what mines are visible to the user
*********************************************************/

#include "global.h"
#include "bolo_map.h" 
#include "screen.h"
#include "mines.h"

/* Prototypes */

/*********************************************************
*NAME:          minesCreate
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/98
*LAST MODIFIED: 29/1/98
*PURPOSE:
*  Creates and initilises the mines structure.
*
*ARGUMENTS:
*  value - Pointer to the map file
*********************************************************/
void minesCreate(mines *visMines, bool allowHiddenMines) {
  int count1; /* Looping Variables */
  int count2;

  New(*visMines);
  for (count1=0;count1<MINES_ARRAY_SIZE;count1++) {
    for (count2=0;count2<MINES_ARRAY_SIZE;count2++) {
      (*visMines)->pos[count1][count2] = FALSE;
    }
  }
  (*visMines)->minesHiddenMines = allowHiddenMines;
}

/*********************************************************
*NAME:          mapDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Destroys the mines data structure. Also frees memory.
*
*ARGUMENTS:
*
*********************************************************/
void minesDestroy(mines *visMines) {
  Dispose(*visMines);
}

/*********************************************************
*NAME:          minesGetAllowHiddenMines
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 29/1/99
*PURPOSE:
*  Returns whether hidden mines are allowed or not
*
*ARGUMENTS:
*
*********************************************************/
bool minesGetAllowHiddenMines(mines *visMines) {
  return (*visMines)->minesHiddenMines;
}

/*********************************************************
*NAME:          minesAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED:  3/2/99
*PURPOSE:
* Adds a mine to the structure. Returns whether a mine
* already existed at that position.
*
*ARGUMENTS:
*  xValue   - X Map Coordinate
*  yValue   - Y Map Coordinate
*********************************************************/
bool minesAddItem(mines *visMines, BYTE xValue, BYTE yValue) {
  bool returnValue = FALSE; /* Value to return */

  returnValue = (*visMines)->pos[xValue][yValue];
  (*visMines)->pos[xValue][yValue] = TRUE;
  return returnValue;
}

/*********************************************************
*NAME:          minesRemoveItem
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/2/99
*PURPOSE:
* Removes a mine to the structure.
*
*ARGUMENTS:
*  xValue   - X Map Coordinate
*  yValue   - Y Map Coordinate
*********************************************************/
void minesRemoveItem(mines *visMines, BYTE xValue, BYTE yValue) {
  (*visMines)->pos[xValue][yValue] = FALSE;
}


/*********************************************************
*NAME:          minesExistPos
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 29/1/99
*PURPOSE:
* Returns whether a mine can be seen at that position.
* Only called if a mine does exist at the map square
* but checking here to see if the player knows about it
*
*ARGUMENTS:
*  xValue   - X Map Coordinate
*  yValue   - Y Map Coordinate
*********************************************************/
bool minesExistPos(mines *visMines, BYTE xValue, BYTE yValue) {
  bool returnValue = TRUE; /* Value to return */
  
  if (xValue <= MAP_MINE_EDGE_LEFT || xValue >= MAP_MINE_EDGE_RIGHT || yValue <= MAP_MINE_EDGE_TOP || yValue >= MAP_MINE_EDGE_BOTTOM) {
    returnValue = TRUE;
  } else if ((*visMines)->minesHiddenMines == TRUE) {
    returnValue = (*visMines)->pos[xValue][yValue];
  } else {
    returnValue = screenMapIsMine(xValue, yValue);
  }
  return returnValue;
}

