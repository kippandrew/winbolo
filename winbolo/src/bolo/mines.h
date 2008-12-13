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
*Filename:      mines.h 
*Author:        John Morrison
*Creation Date: 29/1/99
*Last Modified:  8/2/99
*Purpose:
*  Handles what mines are visible to the user
*********************************************************/

#ifndef MINES_H
#define MINES_H

/* Defines */
#include "global.h"


#define MINES_ARRAY_SIZE 256 /* maps are 256x256 units square */

/* Type definitions */

typedef struct minesObj *mines;

struct minesObj {
  bool pos[MINES_ARRAY_SIZE][MINES_ARRAY_SIZE];
  bool minesHiddenMines; /* Are hidden mines allowed */
};

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
void minesCreate(mines *visMines, bool allowHiddenMines);

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
void minesDestroy(mines *visMines);

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
bool minesGetAllowHiddenMines(mines *visMines);

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
bool minesAddItem(mines *visMines, BYTE xValue, BYTE yValue);

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
void minesRemoveItem(mines *visMines, BYTE xValue, BYTE yValue);

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
bool minesExistPos(mines *visMines, BYTE xValue, BYTE yValue);

#endif /* MINES_H */

