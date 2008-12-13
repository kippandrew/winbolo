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
*Last Modified: 18/2/99
*Purpose:
*  Responsable for tanks on the screen
*********************************************************/

#include <string.h>
#include "global.h"
#include "labels.h"
#include "screentank.h"
//#include "players.h"
//#include "frontend.h"

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
*LAST MODIFIED: 18/2/98
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
*  playerName - String to hold the player name
*********************************************************/
void screenTanksAddItem(screenTanks *value, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, BYTE team, BYTE dir, bool onBoat, char *playerName) {
  (*value).pos[(*value).numTanksScreen].mx = mx;
  (*value).pos[(*value).numTanksScreen].my = my;
  (*value).pos[(*value).numTanksScreen].px = px;
  (*value).pos[(*value).numTanksScreen].py = py;
  (*value).pos[(*value).numTanksScreen].frame = frame;
  (*value).pos[(*value).numTanksScreen].team = team;
  (*value).pos[(*value).numTanksScreen].dir = dir;
  (*value).pos[(*value).numTanksScreen].onBoat = onBoat;

  /* Get the tanks names */
  strcpy(((*value).pos[(*value).numTanksScreen].playerName), playerName);
  (*value).numTanksScreen++;
}

/*********************************************************
*NAME:          screenTanksGetItem
*AUTHOR:        John Morrison
*CREATION DATE: 15/2/98
*LAST MODIFIED: 15/2/98
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
*  playerName - String to hold the player name
*********************************************************/
void screenTanksGetItem(screenTanks *value, BYTE itemNum, BYTE *mx, BYTE *my, BYTE *px, BYTE *py, BYTE *frame, BYTE *team, BYTE *dir, bool *onBoat, char *playerName) {
  itemNum--;
  if (itemNum <= (*value).numTanksScreen) {
    *mx = (*value).pos[itemNum].mx;
    *my = (*value).pos[itemNum].my;
    *px = (*value).pos[itemNum].px;
    *py = (*value).pos[itemNum].py;
    *frame = (*value).pos[itemNum].frame;
    *team = (*value).pos[itemNum].team;
    *dir = (*value).pos[itemNum].dir;
    *onBoat = (*value).pos[itemNum].onBoat;

    strcpy(playerName, (*value).pos[itemNum].playerName);
  }
}
