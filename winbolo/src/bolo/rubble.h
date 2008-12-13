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
*Filename:      rubble.h
*Author:        John Morrison
*Creation Date: 30/12/98
*Last Modified: 30/12/98
*Purpose:
*  Responsable for tracking lifetime of rubble.
*  buildings can be shot 5 times before being destroyed
*********************************************************/

#ifndef RUBBLE_H
#define RUBBLE_H

#include "global.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define RubbleHeadX(list) ((list)->x);
#define RubbleHeadY(list) ((list)->y);
#define RubbleHeadLength(list) ((list)->life);
#define RubbleTail(list) ((list)->next);

/* How manu shots it takes to destroy a building */
#define RUBBLE_LIFE 4

/* Shells die when there length equals */
#define RUBBLE_DEATH 0

/* Type structure */

typedef struct rubbleObj *rubble;
struct rubbleObj {
  rubble next; /* Next item */ 
  BYTE x;      /* X and Y positions */
  BYTE y;
  BYTE life;   /* Life time of the rubble */
};

/* Prototypes */

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
void rubbleCreate(rubble *rbl);

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
void rubbleDestroy(rubble *rbl);

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
BYTE rubbleAddItem(rubble *rbl, BYTE x, BYTE y);

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
void rubbleDeleteItem(rubble *rbl, int itemNum);

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
void rubbleRemovePos(rubble *rbl, BYTE x, BYTE y);

#endif /* RUBBLE_H */
