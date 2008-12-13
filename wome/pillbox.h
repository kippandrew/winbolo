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
*Name:          Pillbox
*Filename:      pillbox.h
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 11/11/00
*Purpose:
*  Provides operations on pillbox and pillboxes
*********************************************************/

#ifndef PILLBOX_H
#define PILLBOX_H


/* Includes */
#include "global.h"


/* Defines */
#define MAX_PILLS 16


/* Typedefs */

typedef struct {
  BYTE x;     /* Co-ordinates on the map */
  BYTE y;
  BYTE owner;  /* should be 0xFF except in speciality maps */
  BYTE armour; /* range 0-15 (dead pillbox = 0, full strength = 15) */
  BYTE speed;  /* typically 50. Time between shots, in 20ms units */
               /* Lower values makes the pillbox start off 'angry' */
  bool inTank; /* Is the pillbox in the tank? */
} pillbox;

typedef struct pillsObj *pillboxes;

struct pillsObj {
  pillbox item[MAX_PILLS];
  BYTE numPills;
};

/* Prototypes */

/*********************************************************
*NAME:          pillsCreate
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Creates and initilises the pillboxes structure.
*  Sets number of pillboxes to zero
*
*ARGUMENTS:
*  value - Pointer to the map file
*********************************************************/
void pillsCreate(pillboxes *value);

/*********************************************************
*NAME:          pillsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Destroys the pills data structure. Also frees memory.
*
*ARGUMENTS:
*  value - Pointer to the pills structure
*********************************************************/
void pillsDestroy(pillboxes *value);

/*********************************************************
*NAME:          pillsSetNumPills
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets the number of pills in the structure 
*
*ARGUMENTS:
*  value    - Pointer to the pillbox structure
*  numPills - The number of pills
*********************************************************/
void pillsSetNumPills(pillboxes *value, BYTE numPills);

/*********************************************************
*NAME:          pillsGetNumPills
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Gets the number of pills in the structure
*
*ARGUMENTS:
*  value    - Pointer to the pillbox structure
*********************************************************/
BYTE pillsGetNumPills(pillboxes *value);

/*********************************************************
*NAME:          pillsSetPill
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets a specific pill with its item data
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  item    - Pointer to a pillbox
*  pillNum - The pillbox number
*********************************************************/
void pillsSetPill(pillboxes *value, pillbox *item, BYTE pillNum);

/*********************************************************
*NAME:          pillsExistPos
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Returns whether a pillbox exist at a specific location
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
bool pillsExistPos(pillboxes *value, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          pillsGetPill
*AUTHOR:        John Morrison
*CREATION DATE:   9/2/99
*LAST MODIFIED: 11/11/00
*PURPOSE:
*  Gets a specific pill.
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  item    - Pointer to a pillbox
*  pillNum - The pillbox number
*********************************************************/
void pillsGetPill(pillboxes *value, pillbox *item, BYTE pillNum);

void pillsDeletePill(pillboxes *value, BYTE x, BYTE y);
#endif /* PILLBOX_H */
