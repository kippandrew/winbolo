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
*Name:          currentGames
*Filename:      currentGames.c
*Author:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 19/1/00
*Purpose:
*  Responsable for holding the list of current games
*********************************************************/

#include "../../bolo/global.h"
#include "../../bolo/gametype.h"
#include "../currentgames.h"

/*********************************************************
*NAME:          currentGamesCreate
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 18/1/00
*PURPOSE:
*  Sets up the currentGames data structure
*
*ARGUMENTS:
*
*********************************************************/
currentGames currentGamesCreate(void) {
  return NULL;
}

/*********************************************************
*NAME:          currentGamesDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 18/1/00
*PURPOSE:
*  Destroys the currentGames data structure
*
*ARGUMENTS:
*  value - The structure to destroy
*********************************************************/
void currentGamesDestroy(currentGames *value) {
  currentGames q;

  while (NonEmpty(*value)) {
    q = *value;
    *value = CurrentTail(*value);
    Dispose(q);
  }
}

/*********************************************************
*NAME:          currentGamesAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 19/1/00
*PURPOSE:
*  Adds an item to the currentGames data structure.
*
*ARGUMENTS:
*  value      - The currentGames structure
*  address    - Server address
*  port       - Server port
*  mapName    - Name of the map
*  version    - Game version
*  numPlayers - Number of players
*  numBases   - Number of bases
*  numPills   - Number of pills
*  mines      - Allow hidden mines
*  game       - Game type
*  ai         - Ai type
*  password   - Has password
*********************************************************/
void currentGamesAddItem(currentGames *value, char *address, unsigned short port, char *mapName, char *version, BYTE numPlayers, BYTE numBases, BYTE numPills, bool mines, gameType game, aiType ai, bool password) {
  currentGames q;

  New(q);
  strcpy(q->address, address);
  strcpy(q->mapName, mapName);
  strcpy(q->version, version);
  q->port = port;
  q->numPlayers = numPlayers;
  q->numBases = numBases;
  q->numPills = numPills;
  q->mines = mines;
  q->game = game;
  q->ai = ai;
  q->password = password;
  
  q->next = *value;
  *value = q;
}

/*********************************************************
*NAME:          currentGamesItemCount
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 18/1/00
*PURPOSE:
*  Returns the number of elements in the structure
*
*ARGUMENTS:
*  value - The currentGames structure
*********************************************************/
int currentGamesItemCount(currentGames *value) {
  currentGames q;
  int returnValue; /* Value to return */

  returnValue = 0;
  q = *value;
  while (NonEmpty(q)) {
    returnValue++;
    q = CurrentTail(q);
  }
  return returnValue;
}

/*********************************************************
*NAME:          currentGamesGetItem
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 19/1/00
*PURPOSE:
*  Gets the item for an itemNum
*
*ARGUMENTS:
*  value      - The currentGames structure
*  itemNum    - Item number to get record for
*  address    - Server address
*  port       - Server port
*  mapName    - Name of the map
*  version    - Game version
*  numPlayers - Number of players
*  numBases   - Number of bases
*  numPills   - Number of pills
*  mines      - Allow hidden mines
*  game       - Game type
*  ai         - Ai type
*  password   - Has password
*********************************************************/
void currentGamesGetItem(currentGames *value, int itemNum, char *address, unsigned short *port, char *mapName, char *version, BYTE *numPlayers, BYTE *numBases, BYTE *numPills, bool *mines, gameType *game, aiType *ai, bool *password) {
  currentGames q;
  int count; /* Looping variable */

  if (itemNum <= currentGamesItemCount(value)) {
    count = 1;
    q = *value;
    while (count < itemNum) {
      q = CurrentTail(q);
      count++;
    }
    strcpy(address, q->address);
    strcpy(mapName, q->mapName);
    strcpy(version, q->version);
    *port = q->port;
    *numPlayers = q->numPlayers;
    *numBases = q->numBases;
    *numPills = q->numPills;
    *mines = q->mines;
    *game = q->game;
    *ai = q->ai;
    *password = q->password;
  }
}

/*********************************************************
*NAME:          currentGamesGetServerName
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 18/1/00
*PURPOSE:
*  Gets the server name for an itemNum
*
*ARGUMENTS:
*  value   - The currentGames structure
*  itemNum - Item number to get record for
*  address - Server address
*********************************************************/
void currentGamesGetServerName(currentGames *value, int itemNum, char *address) {
  currentGames q;
  int count; /* Looping variable */

  if (itemNum <= currentGamesItemCount(value)) {
    count = 1;
    q = *value;
    while (count < itemNum) {
      q = CurrentTail(q);
      count++;
    }
    strcpy(address, q->address);
  }
}
