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
*Name:          Screen Tanks
*Filename:      screenTanks.h
*Author:        John Morrison
*Creation Date: 15/2/99
*Last Modified: 26/1/02
*Purpose:
*  Responsable for tanks on the screen
*********************************************************/

#ifndef SCREENTANKS_H
#define SCREENTANKS_H

#include "global.h"
#include "tank.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define ScreenTanksHeadMX(list) ((list)->mx);
#define ScreenTanksHeadMY(list) ((list)->my);
#define ScreenTanksHeadPX(list) ((list)->px);
#define ScreenTanksHeadPY(list) ((list)->py);
#define ScreenTanksHeadFrame(list) ((list)->frame);
#define ScreenTanksTail(list) ((list)->next);

/* Type structure */

typedef struct {
  BYTE mx;  /* The map x co-ordinate it is on */
  BYTE my;  /* The map y co-ordinate it is on */
  BYTE px;  /* The pixel offset from the left it is on */
  BYTE py;  /* The pixel offset from the top it is on */
  BYTE frame; /* The direction it is facing */
  BYTE playerNum; /* The player Number */
  char playerName[PLAYER_NAME_LEN];
} screenPosTank;

/*typedef struct screenTanksObj *screenTanks;
struct {
  BYTE numTanksScreen; * The number of tanks on screen *
  screenPosTank pos[MAX_TANKS]; * Data for each tank *
} screenTanksObj; */


typedef struct {
  BYTE numTanksScreen; /* The number of tanks on screen */
  screenPosTank pos[MAX_TANKS];
} screenTanks;

/* Determines the tank type relative to ourselves, good, neutral or evil */
typedef enum {
  tankNone,
  tankSelf,
  tankAllie,
  tankEvil
} tankAlliance;

/* Prototypes */

/*********************************************************
*NAME:          screenTanksCreate
*AUTHOR:        John Morrison
*CREATION DATE: 15/2/99
*LAST MODIFIED: 15/2/99
*PURPOSE:
*  Sets up the screen tanks data structure
*
*ARGUMENTS:
*  value - New item to create
*********************************************************/
void screenTanksCreate(screenTanks *value);

/*********************************************************
*NAME:          screenTanksPrepare
*AUTHOR:        John Morrison
*CREATION DATE: 15/2/99
*LAST MODIFIED: 15/2/99
*PURPOSE:
*  Prepares the screenTanks data structure prior to
*  displaying
*
*ARGUMENTS:
*  value    - Pointer to the screenTanks data structure
*  tnk      - Pointer to your tank data structure
*  leftPos  - Left bounds of the screen
*  rightPos - Right bounds of the screen
*  top      - Top bounds of the screen
*  bottom   - Bottom bounds of the screen
*********************************************************/
void screenTanksPrepare(screenTanks *value, tank *tnk, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

/*********************************************************
*NAME:          screenTanksGetNumEntries
*AUTHOR:        John Morrison
*CREATION DATE: 15/2/99
*LAST MODIFIED: 15/2/99
*PURPOSE:
*  Returns the number of elements in the data structure
*
*ARGUMENTS:
*  value - Pointer to the screenTanks data structure
*********************************************************/
BYTE screenTanksGetNumEntries(screenTanks *value);

/*********************************************************
*NAME:          screenTanksDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 15/2/99
*LAST MODIFIED: 15/2/99
*PURPOSE:
*  Destroys and frees memory for the data structure
*
*ARGUMENTS:
*  value - Pointer to the screenTanks data structure
*********************************************************/
void screenTanksDestroy(screenTanks *value);

/*********************************************************
*NAME:          screenTanksAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/98
*LAST MODIFIED: 26/1/02
*PURPOSE:
*  Adds a data set for a specific tank
*
*ARGUMENTS:
*  value      - Pointer to the screenBullets data structure
*  mx         - X co-ord of the map position
*  my         - Y co-ord of the map position
*  px         - X pixel offset
*  py         - Y pixel offset
*  frame      - Frame identifer of the tank
*  playerNum  - Player Number of this tank
*  playerName - String to hold the player name
*********************************************************/
void screenTanksAddItem(screenTanks *value, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, BYTE playerNum, char *playerName);

/*********************************************************
*NAME:          screenTanksGetItem
*AUTHOR:        John Morrison
*CREATION DATE: 15/2/98
*LAST MODIFIED: 26/1/02
*PURPOSE:
*  Gets data for a specific item
*
*ARGUMENTS:
*  value      - Pointer to the screenBullets data structure
*  itemNum    - The item number to get
*  mx         - X co-ord of the map position
*  my         - Y co-ord of the map position
*  px         - X pixel offset
*  py         - Y pixel offset
*  frame      - Frame identifer of the tank
*  playerNum  - Player Number of this tank
*  playerName - String to hold the player name
*********************************************************/
void screenTanksGetItem(screenTanks *value, BYTE itemNum, BYTE *mx, BYTE *my, BYTE *px, BYTE *py, BYTE *frame, BYTE *playerNum, char *playerName);

#endif /* SCREENTANKS_H */

