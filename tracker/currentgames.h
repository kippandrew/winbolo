/*
 * $Id$
 *
 * Copyright (c) 2000-2008 John Morrison.
 * Copyright (c) 2000-2008 Andrew Roth.
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
*Creation Date: 00/01/18
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for holding the list of current games
*********************************************************/


#ifndef _CURRENT_GAMES_H
#define _CURRENT_GAMES_H


#include <pthread.h>
#include <time.h>
#include "global.h"
#include "irc.h"


/* Games expire after 4 minutes, 10 seconds of no updates 
   As far as I can see servers send info every 4 minutes
   The new timeout is 2 mins, 10 secs for >= 1.09  -Andrew
   Now put back to 4, in effort to fix games dropping
   off the tracker -Andrew */
#define CURRENT_GAME_TRACKER_EXPIRE_OLD (4 * 60) + 10
#define CURRENT_GAME_TRACKER_EXPIRE_NEW (4 * 60) + 10

#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define CurrentTail(list) ((list)->next);

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
  unsigned long starttime;
  long startdelay;
  long timelimit;
  time_t lastpacket;
  unsigned char ip[IP_SIZE];
  unsigned long gameLogOffset;
};

/* Prototypes */

/*********************************************************
 *NAME:          currentGamesCreateIrcString
 *AUTHOR:        Andrew Roth
 *CREATION DATE: 03/03/29 YMD
 *LAST MODIFIED: 03/03/29
 *PURPOSE:
 *  Creates the irc string into str
 *
 *  Note the currentGames structure is not locked in this
 *  method but it should be locked by a calling method
 *
 *ARGUMENTS:
 *  str - a pointer to the irc string
 *  cg - a pointer to the currentGames structure
 *********************************************************/
void currentGamesCreateIrcString(char str[IRC_MAX_GAME_LEN], currentGames cg);

/*********************************************************
*NAME:          currentGamesCreate
*AUTHOR:        John Morrison
*CREATION DATE: 00/01/18
*LAST MODIFIED: 00/01/18
*PURPOSE:
*  Sets up the currentGames data structure
*
*ARGUMENTS:
*
*********************************************************/
currentGames currentGamesCreate(void);

/*********************************************************
 *NAME:          currentGamesLock
 *AUTHOR:        Andrew Roth
 *CREATION DATE: 03/01/22
 *LAST MODIFIED: 03/01/22
 *PURPOSE:
 *  Waits until a lock is obtained on the currentGames
 *  structure.  When it is finished with the currentGames
 *  structure, it should call currentGamesUnlock
 *
 *ARGUMENTS:
 **********************************************************/
void currentGamesLock(void);

/*********************************************************
 *NAME:          currentGamesUnlock
 *AUTHOR:        Andrew Roth
 *CREATION DATE: 03/01/22
 *LAST MODIFIED: 03/01/22
 *PURPOSE:
 *  unlocks the currentgames structure.  only a thread
 *  which has a lock should do this
 *
 *ARGUMENTS:
 **********************************************************/
void currentGamesUnlock(void);

/*********************************************************
*NAME:          currentGamesDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 00/01/18
*LAST MODIFIED: 00/01/18
*PURPOSE:
*  Destroys the currentGames data structure
*
*ARGUMENTS:
*  value - The structure to destroy
*********************************************************/
void currentGamesDestroy(currentGames *value);

/*********************************************************
*NAME:          currentGamesUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* This function adds a game to the game list. (or updates
* if it exists) As it goes through the list it deletes
* expired games.
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
*  starttime  - Time the game started
*  startdelay - Start delay time
*  timelimit  - Game time limit
*  ip         - 4 byte IP for stats purposes
*********************************************************/
void currentGamesUpdate(currentGames *value, char *address, unsigned short port, char *mapName, char *version, BYTE numPlayers, BYTE numBases, BYTE numPills, bool mines, gameType game, aiType ai, bool password, unsigned long starttime, long startdelay, long timelimit, unsigned char *ip);

/*********************************************************
*NAME:          currentGamesAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 00/01/18
*LAST MODIFIED: 00/10/01
*PURPOSE:
*  Adds an item to the currentGames data structure.
*  Updates if it exists
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
*  starttime  - Time the game started
*  startdelay - Start delay time
*  timelimit  - Game time limit
*  ip         - 4 byte IP for stats purposes
*********************************************************/
void currentGamesAddItem(currentGames *value, char *address, unsigned short port, char *mapName, char *version, BYTE numPlayers, BYTE numBases, BYTE numPills, bool mines, gameType game, aiType ai, bool password, unsigned long starttime, long startdelay, long timelimit, unsigned char *ip);

/*********************************************************
*NAME:          currentGamesItemCount
*AUTHOR:        John Morrison
*CREATION DATE: 00/01/18
*LAST MODIFIED: 00/10/01
*PURPOSE:
*  Returns the number of elements in the structure. Also
*  purges empty games
*
*ARGUMENTS:
*  value - The currentGames structure
*********************************************************/
int currentGamesItemCount(currentGames *value);

/*********************************************************
*NAME:          currentGamesGetItem
*AUTHOR:        John Morrison
*CREATION DATE: 00/01/18
*LAST MODIFIED: 00/10/01
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
*  starttime  - Time the game started
*  startdelay - Start delay time
*  timelimit  - Game time limit
*********************************************************/
void currentGamesGetItem(currentGames *value, int itemNum, char *address, unsigned short *port, char *mapName, char *version, BYTE *numPlayers, BYTE *numBases, BYTE *numPills, bool *mines, gameType *game, aiType *ai, bool *password, unsigned long *starttime, long *startdelay, long *timelimit);

/*********************************************************
*NAME:          currentGamesGetServerName
*AUTHOR:        John Morrison
*CREATION DATE: 00/01/18
*LAST MODIFIED: 00/01/18
*PURPOSE:
*  Gets the server name for an itemNum
*
*ARGUMENTS:
*  value   - The currentGames structure
*  itemNum - Item number to get record for
*  address - Server address
*********************************************************/
void currentGamesGetServerName(currentGames *value, int itemNum, char *address);

/*********************************************************
*NAME:          currentGamesItemCount
*AUTHOR:        Andrew Roth
*CREATION DATE: 01/05/01
*LAST MODIFIED: 01/05/01
*PURPOSE:
*  purges timed out games
*
*ARGUMENTS:
*  value - The currentGames structure
*********************************************************/
void currentGamesPurge(currentGames *value);

#endif /* _CURRENTGAMES_H */

