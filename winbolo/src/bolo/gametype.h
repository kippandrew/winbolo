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
*Name:          gametype
*Filename:      gametype.h
*Author:        John Morrison
*Creation Date: 29/01/99
*Last Modified: 17/12/03
*Purpose:
*  Responsable for tracking amounts of armour, health etc.
*  that should be given to a tank and gametype
*********************************************************/

#ifndef GAMETYPE_H
#define GAMETYPE_H

#include "global.h"

/* Defines */

/* Full items on tank */
#define TANK_FULL_ARMOUR 40
#define TANK_FULL_SHELLS 40
#define TANK_FULL_MINES 40
#define TANK_FULL_TREES 40

/* Typedefs */

/* The different types of games there are */
#ifndef _GAMETYPE_ENUM
#define _GAMETYPE_ENUM

typedef enum {
  gameOpen = 1,
  gameTournament,
  gameStrictTournament
} gameType;

#endif

/* Prototypes */

/*********************************************************
*NAME:          gameTypeSet
*AUTHOR:        John Morrison
*CREATION DATE: 29/01/99
*LAST MODIFIED: 17/12/03
*PURPOSE:
* Sets the game type. Should be set before any other
* calls are made to the module
*
*ARGUMENTS:
* value - Value to set the game type to
*********************************************************/
void gameTypeSet(gameType *gmtType, gameType value);

/*********************************************************
*NAME:          gameTypeGet
*AUTHOR:        John Morrison
*CREATION DATE: 29/01/99
*LAST MODIFIED: 17/12/03
*PURPOSE:
* Returns the game type.
*
*ARGUMENTS:
*  gmeType      - Pointer to the game type
*********************************************************/
gameType gameTypeGet(gameType *gmeType);

/*********************************************************
*NAME:          gameTypeGetItems
*AUTHOR:        John Morrison
*CREATION DATE: 29/01/99
*LAST MODIFIED: 17/12/03
*PURPOSE:
* Called when a tank needs to restart. Fills the 
* parameters with how much stuff it should hold.
*
*ARGUMENTS:
*  gmeType      - Pointer to the game type
*  shellsAmount - Pointer to hold the number of shells
*  mines  - Pointer to hold the number of mines
*  armour - Pointer to hold the amount of armour
*  trees  - Pointer to hold the number of trees
*********************************************************/
void gameTypeGetItems(gameType *gmeType, BYTE *shellsAmount, BYTE *mines, BYTE *armour, BYTE *trees);

#endif /* GAMETYPE_H */
