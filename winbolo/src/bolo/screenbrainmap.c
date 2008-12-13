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
*Name:          Screen Brain Map
*Filename:      screenBrainMap.c
*Author:        John Morrison
*Creation Date: 27/11/99
*Last Modified:  1/12/99
*Purpose:
*  Responsible for storing a copy of the map in form used
*  by Bolo Brains because doing a manual copy is too slow
*  each time a brain request is made
*********************************************************/

#include <memory.h>
#include "global.h"
#include "bolo_map.h"
#include "brain.h"
#include "screenbrainmap.h"

/* Prototypes */

BYTE sbm[MAP_ARRAY_SIZE][MAP_ARRAY_SIZE];

/*********************************************************
*NAME:          screenBrainMapCreate
*AUTHOR:        John Morrison
*CREATION DATE: 27/11/99
*LAST MODIFIED: 27/11/99
*PURPOSE:
*  Sets up the data structure
*
*ARGUMENTS:
*
*********************************************************/
void screenBrainMapCreate(void) {
  memset(sbm, TERRAIN_UNKNOWN, (MAP_ARRAY_SIZE * MAP_ARRAY_SIZE));
}


/*********************************************************
*NAME:          screenBrainMapDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 27/11/99
*LAST MODIFIED: 27/11/99
*PURPOSE:
*  Destroys and frees memory for the data structure
*
*ARGUMENTS:
*
*********************************************************/
void screenBrainMapDestroy(void) {
  return;
}

/*********************************************************
*NAME:          screenBrainMapGetPointer
*AUTHOR:        John Morrison
*CREATION DATE: 27/11/99
*LAST MODIFIED: 27/11/99
*PURPOSE:
*  Returns a pointer to the data structure
*
*ARGUMENTS:
*
*********************************************************/
BYTE *screenBrainMapGetPointer(void) {
  return (BYTE *) sbm;
}

/*********************************************************
*NAME:          screenBrainMapSetPos
*AUTHOR:        John Morrison
*CREATION DATE: 27/11/99
*LAST MODIFIED:  1/12/99
*PURPOSE:
*  Sets a position in the data structure
*
*ARGUMENTS:
*  xValue  - The X Value of the position to set
*  yValue  - The Y Value of the position to set
*  terrain - Terrain to set it to
*  isMine  - Is this square mined
*********************************************************/
void screenBrainMapSetPos(BYTE xValue, BYTE yValue, BYTE terrain, bool isMine) {
  sbm[yValue][xValue] = terrain;
  if (terrain >= MINE_START && terrain <= MINE_END) {
    sbm[yValue][xValue] -= MINE_SUBTRACT;
  } else if (terrain == DEEP_SEA) {
    sbm[yValue][xValue] = BDEEPSEA;
  }

  if (isMine == TRUE) {
    sbm[yValue][xValue] |= TERRAIN_MINE;
  }
}
