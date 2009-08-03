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
*Filename:      gametype.c
*Author:        John Morrison
*Creation Date: 29/1/99
*Last Modified:  6/1/00
*Purpose:
*  Responsable for tracking amounts of armour, health etc.
*  that should be given to a tank and gametype
*********************************************************/

#include <stdlib.h>
#include "global.h"
#include "screen.h"
#include "backend.h"
#include "gametype.h"

/* 2 as a define to eliminate magic number warnings */
#define TWO 2.0
/* Max bases = 16 as a double */
#define BASE_MAX_DOUBLE 16.0

gameType gmeType = gameOpen; /* Type of game being played */

/*********************************************************
*NAME:          gameTypeSet
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 17/12/03
*PURPOSE:
* Sets the game type. Should be set before any other
* calls are made to the module
*
*ARGUMENTS:
*  gmeType      - Pointer to the game type
* value - Value to set the game type to
*********************************************************/
void gameTypeSet(gameType *gmeType, gameType value) {
  *gmeType = value;
}

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
gameType gameTypeGet(gameType *gmeType) {
  return *gmeType;
}

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
void gameTypeGetItems(gameType *gmeType, BYTE *shellsAmount, BYTE *mines, BYTE *armour, BYTE *trees) {
  double percent; /* Percent of free bases */
  BYTE numBases;  /* Number of bases on the map */

  *armour = TANK_FULL_ARMOUR;
  switch (*gmeType) {
  case gameOpen:
    *shellsAmount = TANK_FULL_SHELLS;
    *mines = TANK_FULL_MINES;
    *trees = TANK_FULL_TREES;
    break;
  case gameTournament:
    numBases = screenNumBases();
    if (numBases == 0) {
      numBases = 1;
    }
    percent = (double) screenGetNumNeutralBases();
    percent /= numBases;
    percent *= BASE_MAX_DOUBLE;
    *shellsAmount = (BYTE) (TWO * percent);
    *mines = 0;
    *trees = 0;
    break;
  case gameStrictTournament:
  default:
    /* gameStrictTournament */
    *shellsAmount = 0;
    *mines = 0;
    *trees = 0;
    break;
  }
}
