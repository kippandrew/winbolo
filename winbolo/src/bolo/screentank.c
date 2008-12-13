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
*Filename:      screenTanks.c
*Author:        John Morrison
*Creation Date: 15/2/99
*Last Modified: 26/1/02
*Purpose:
*  Responsable for tanks on the screen
*********************************************************/

#include <string.h>
#include "global.h"
#include "tank.h"
#include "labels.h"
#include "messages.h"
#include "screentank.h"
#include "players.h"
#include "frontend.h"

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
void screenTanksCreate(screenTanks *value) {
  BYTE count; /* Looping variable */
  /*  New(*value); */
  (*value).numTanksScreen = 0;
  for (count=0;count<MAX_TANKS;count++) {
    (*value).pos[count].playerName[0] = '\0';
  }
}

/*********************************************************
*NAME:          screenTanksPrepare
*AUTHOR:        John Morrison
*CREATION DATE: 15/2/99
*LAST MODIFIED: 26/1/02
*PURPOSE:
*  Prepares the screenTanks data structure prior to
*  displaying
*
*ARGUMENTS:
*  value  - Pointer to the screenTanks data structure
*  tnk    - Pointer to your tank data structure
*  left   - Left bounds of the screen
*  right  - Right bounds of the screen
*  top    - Top bounds of the screen
*  bottom - Bottom bounds of the screen
*********************************************************/
void screenTanksPrepare(screenTanks *value, tank *tnk, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  char playerName[PLAYER_NAME_LEN] = "\0"; /* Player Name */
  BYTE x; /* X and Y map pos of it */
  BYTE y;
  BYTE count; /* Looping variable */

  for (count=0;count<MAX_TANKS;count++) {
    (*value).pos[count].playerName[0] = '\0';
  }
  (*value).numTanksScreen = 0;

  x = tankGetScreenMX(tnk);
  y = tankGetScreenMY(tnk);

  if (x >= leftPos && x <= rightPos && y >= top && y <= bottom) {
    (*value).numTanksScreen = 1;
    (*value).pos[0].mx = x - leftPos;
    (*value).pos[0].my = y - top;
    (*value).pos[0].px = tankGetScreenPX(tnk);
    (*value).pos[0].py = tankGetScreenPY(tnk);
    (*value).pos[0].frame = tankGetFrame(tnk);
    (*value).pos[0].playerNum = playersGetSelf(screenGetPlayers());
    /* Get the tanks names */
    (*value).pos[0].playerName[0] = '\0';
    if (tankGetArmour(tnk) <= TANK_FULL_ARMOUR) {
      playersGetPlayerName(screenGetPlayers(), playersGetSelf(screenGetPlayers()), playerName);
      labelMakeTankLabel((*value).pos[0].playerName, playerName, langGetText(MESSAGE_THIS_COMPUTER), TRUE);
    }
  }
  /* Add the rest of the tanks as required */
  playersMakeScreenTanks(screenGetPlayers(), value, leftPos, rightPos, top, bottom);
}

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
BYTE screenTanksGetNumEntries(screenTanks *value) {
  return ((*value).numTanksScreen);
}

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
void screenTanksDestroy(screenTanks *value) {
/*  Dispose(*value); */
}

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
void screenTanksAddItem(screenTanks *value, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, BYTE playerNum, char *playerName) {
  (*value).pos[(*value).numTanksScreen].mx = mx;
  (*value).pos[(*value).numTanksScreen].my = my;
  (*value).pos[(*value).numTanksScreen].px = px;
  (*value).pos[(*value).numTanksScreen].py = py;
  (*value).pos[(*value).numTanksScreen].frame = frame;
  (*value).pos[(*value).numTanksScreen].playerNum = playerNum;
  /* Get the tanks names */
  strcpy(((*value).pos[(*value).numTanksScreen].playerName), playerName);
  (*value).numTanksScreen++;
}

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
void screenTanksGetItem(screenTanks *value, BYTE itemNum, BYTE *mx, BYTE *my, BYTE *px, BYTE *py, BYTE *frame, BYTE *playerNum, char *playerName) {
  itemNum--;
  if (itemNum <= (*value).numTanksScreen) {
    *mx = (*value).pos[itemNum].mx;
    *my = (*value).pos[itemNum].my;
    *px = (*value).pos[itemNum].px;
    *py = (*value).pos[itemNum].py;
    *frame = (*value).pos[itemNum].frame;
    *playerNum = (*value).pos[itemNum].playerNum;
    strcpy(playerName, (*value).pos[itemNum].playerName);
  }
}
