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
*Name:          Bases 
*Filename:      bases.c 
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 11/11/00
*Purpose:
*  Provides operations on bases 
*********************************************************/

/* Includes */
#include "global.h"
#include "bases.h"

/*********************************************************
*NAME:         basesCreate 
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Creates and initilises the bases structure.
*  Sets number of bases to zero
*
*ARGUMENTS:
*  value - Pointer to the bases structure 
*********************************************************/
void basesCreate(bases *value) {
  New(*value);
  (*value)->numBases = 0;
}

/*********************************************************
*NAME:          basesDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Destroys the bases data structure. Also frees memory.
*
*ARGUMENTS:
*  value - Pointer to the bases structure
*********************************************************/
void basesDestroy(bases *value) {
  Dispose(*value);
}

/*********************************************************
*NAME:          basesSetNumBases
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets the number of bases in the structure 
*
*ARGUMENTS:
*  value     - Pointer to the bases structure
*  numBases - The number of bases  
*********************************************************/
void basesSetNumBases(bases *value, BYTE numBases) {
  if (numBases <= MAX_BASES) {
    (*value)->numBases = numBases;
  }
}


/*********************************************************
*NAME:          basesGetNumBases 
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Gets the number of bases in the structure
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*********************************************************/
BYTE basesGetNumBases(bases *value) {
  return (*value)->numBases;
}

/*********************************************************
*NAME:          basesSetBase
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets a specific base with its item data
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  item    - Pointer to a base item 
*  baseNum - The base number
*********************************************************/
void basesSetBase(bases *value, base *item, BYTE baseNum) {
  if (baseNum > 0 && baseNum <= (*value)->numBases) {
    baseNum--;
    (((*value)->item[baseNum]).x) = item->x;
    (((*value)->item[baseNum]).y) = item->y;
    (((*value)->item[baseNum]).owner) = item->owner;
    (((*value)->item[baseNum]).armour) = item->armour;
    (((*value)->item[baseNum]).shells) = item->shells;
    (((*value)->item[baseNum]).mines) = item->mines;
  }
}

/*********************************************************
*NAME:          basesExistPos
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Returns whether a base exist at a specific location
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
bool basesExistPos(bases *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  returnValue = FALSE;
  count = 0;
  while (returnValue == FALSE && count < ((*value)->numBases)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
      returnValue = TRUE;
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesGetBase
*AUTHOR:        John Morrison
*CREATION DATE:   9/2/98
*LAST MODIFIED: 11/11/00
*PURPOSE:
*  Gets a specific base
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  item    - Pointer to a base item 
*  baseNum - The base number
*********************************************************/
void basesGetBase(bases *value, base *item, BYTE baseNum) {
  if (baseNum > 0 && baseNum <= (*value)->numBases) {
    baseNum--;
    item->x = ((*value)->item[baseNum]).x;
    item->y = ((*value)->item[baseNum]).y;
    item->owner = ((*value)->item[baseNum]).owner;
    item->armour = ((*value)->item[baseNum]).armour;
    item->shells = ((*value)->item[baseNum]).shells;
    item->mines = ((*value)->item[baseNum]).mines;
  }
}

void basesDeleteBase(bases *value, BYTE x, BYTE y) {
  BYTE count = 0;
  BYTE count2;

  while (count < (*value)->numBases) {
    if ((*value)->item[count].x == x && (*value)->item[count].y == y) {
      count2 = count;
      while (count2 < (*value)->numBases - 1) {
        (*value)->item[count2].x = (*value)->item[count2+1].x;
        (*value)->item[count2].y = (*value)->item[count2+1].y;
        (*value)->item[count2].owner = (*value)->item[count2+1].owner;
        (*value)->item[count2].armour = (*value)->item[count2+1].armour;
        (*value)->item[count2].shells = (*value)->item[count2+1].shells;
        (*value)->item[count2].mines = (*value)->item[count2+1].mines;
	count2++;
      }
      (*value)->numBases--;
    }
    count++;
  }
}

