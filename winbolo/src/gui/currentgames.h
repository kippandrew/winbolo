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
*Filename:      currentGames.h
*Author:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 19/1/00
*Purpose:
*  Responsable for holding the list of current games
*********************************************************/

#ifndef CURRENT_GAMES_H
#define CURRENT_GAMES_H

#include "../bolo/global.h"
#include "../bolo/gametype.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define CurrentTail(list) ((list)->next);

#ifndef _AITYPE_ENUM
#define _AITYPE_ENUM

typedef enum {
  aiNone,
  aiYes,
  aiYesAdvantage
} aiType;

#endif 



/* 



Address: 132.234.100.39:27500 Version: 1.0.1 
Players: 0, Bases: 11, Pills: 16, 
Map: e, GameType: 48, 
Mines: Normal, 
Bots: Yes, 
PW: Yes, 
first update received 1/18/00 3:16:33 AM, 
last update received 3 minutes 37 seconds ago. */

/* Type structure */

typedef struct currentGamesObj *currentGames;
struct currentGamesObj {
  currentGames next; /* Next item */ 
  char address[FILENAME_MAX];
  char mapName[MAP_STR_SIZE];
  char version[FILENAME_MAX];
  unsigned short port;
  BYTE numPlayers;
  BYTE numBases;
  BYTE numPills;
  bool mines;
  gameType game;
  aiType ai;
  bool password;
};

/* Prototypes */

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
currentGames currentGamesCreate(void);


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
void currentGamesDestroy(currentGames *value);

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
void currentGamesAddItem(currentGames *value, char *address, unsigned short port, char *mapName, char *version, BYTE numPlayers, BYTE numBases, BYTE numPills, bool mines, gameType game, aiType ai, bool password);

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
int currentGamesItemCount(currentGames *value);

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
void currentGamesGetItem(currentGames *value, int itemNum, char *address, unsigned short *port, char *mapName, char *version, BYTE *numPlayers, BYTE *numBases, BYTE *numPills, bool *mines, gameType *game, aiType *ai, bool *password);

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
void currentGamesGetServerName(currentGames *value, int itemNum, char *address);

#endif /* CURRENT_GAMES_H */
