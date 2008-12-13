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
*Name:          WinBolo.net
*Filename:      winbolonet.h
*Author:        John Morrison
*Creation Date: 23/09/01
*Last Modified: 10/01/04
*Purpose:
*  Responsable for interacting with WinBolo.net
*********************************************************/

#ifndef __WINBOLO_NET_H
#define __WINBOLO_NET_H

#include <stdio.h>
#include <string.h>
#include "../bolo/global.h"

/* Version information */
#define WINBOLO_NET_VERSION_MAJOR 0x01
#define WINBOLO_NET_VERSION_MINOR 0x00
#define WINBOLO_NET_VERSION_REVISION 0x0

/* Message types */
#define WINBOLO_NET_MESSAGE_VERSION_REQ 1
#define WINBOLO_NET_MESSAGE_SERVERKEY_REQ 2
#define WINBOLO_NET_MESSAGE_CLIENTKEY_REQ 3
#define WINBOLO_NET_MESSAGE_VERIFYCLIENTKEY_REQ 4
#define WINBOLO_NET_MESSAGE_CLIENTLEAVE_REQ 5
#define WINBOLO_NET_MESSAGE_SERVERQUIT_REQ 6
#define WINBOLO_NET_MESSAGE_SERVERUPDATE_REQ 7
#define WINBOLO_NET_MESSAGE_TEAMS 8
#define WINBOLO_NET_VERSION 9
#define WINBOLO_NET_LOCK 10 /* Game lock message */

/* Size of keys */
#define WINBOLONET_KEY_LEN 32

/* Event Types */
#define WINBOLO_NET_EVENT_ALLY_JOIN 0 /* Player B has joined Alliance A */
#define WINBOLO_NET_EVENT_ALLY_LEAVE 1 /* Player B has left Alliance A */
#define WINBOLO_NET_EVENT_BASE_CAPTURE 2 /* Player A has captured a base */
#define WINBOLO_NET_EVENT_PILL_CAPTURE 3 /* Player A has captured a pill */
#define WINBOLO_NET_EVENT_TANK_KILL 4 /* Player A has killed a tank */
#define WINBOLO_NET_EVENT_LGM_KILL 5 /* Player A has killed a lgm */
#define WINBOLO_NET_EVENT_LGM_LOST 6 /* Player A has lost their lgm */
#define WINBOLO_NET_EVENT_BASE_STEAL 7 /* Player A has captured a base */
#define WINBOLO_NET_EVENT_PILL_STEAL 8 /* Player A has captured a pill */
#define WINBOLO_NET_EVENT_PLAYER_JOIN 9  /* Player A has joined the game */
#define WINBOLO_NET_EVENT_PLAYER_LEAVE 10 /* Player A has left the game */
#define WINBOLO_NET_EVENT_WIN 11          /* Player A has won the game */
/* FIXME 12 is rejoining? */
#define WINBOLO_NET_EVENT_QUITTING 13 /* Player A is quiting */

#define WINBOLO_NET_NO_PLAYER 100 /* If no player use this holder */

#define WINBOLO_NET_MAX_NOSEND 60 /* Maximum non transmission time */

#define WINBOLO_NET_TEAM_MARKER (254)

void winbolonetServerSendTeams(BYTE *array, BYTE length, BYTE numTeams);

/*********************************************************
*NAME:          winbolonetCreateServer
*AUTHOR:        John Morrison
*CREATION DATE: 23/09/01
*LAST MODIFIED: 02/04/02
*PURPOSE:
* Initialises the WinBolo.net module. Returns success.
* Tries to contact server to verify versions.
*
*ARGUMENTS:
* mapName - Name of the map
* port - Port we are running on
* gameType - Game Type
* ai - Is AI allowed
* mines - Mines allowed
* password - Has password
* numBases - Number of bases
* numPills - Number of pills
* freeBases - Free bases
* freePills - Free pills
* numPlayers -  Number of players in the game
* startTime - Game start time
*********************************************************/
bool winbolonetCreateServer(char *mapName, unsigned short port, BYTE gameType, BYTE ai, bool mines, bool password, BYTE numBases, BYTE numPills, BYTE freeBases, BYTE freePills, BYTE numPlayers, long startTime);

/*********************************************************
*NAME:          winbolonetCreateClient
*AUTHOR:        John Morrison
*CREATION DATE: 31/03/02
*LAST MODIFIED: 31/03/02
*PURPOSE:
* Initialises the WinBolo.net module for a client.
* Returns success.
*
*ARGUMENTS:
* userName  - WinBolo.net account name
* password  - Password for the account
* serverKey - Session key for the server
* errorMsg  - Buffer to hold Error message if required
*********************************************************/
bool winbolonetCreateClient(char *userName, char *password, BYTE *serverKey, char *errorMsg);

/*********************************************************
*NAME:          winbolonetDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 23/9/01
*LAST MODIFIED: 23/9/01
*PURPOSE:
* Destroys the winbolonet module.
* Cleans up any open libraries
*
*ARGUMENTS:
* 
*********************************************************/
void winbolonetDestroy();

/*********************************************************
*NAME:          winbolonetRequestServerKey
*AUTHOR:        John Morrison
*CREATION DATE: 29/03/02
*LAST MODIFIED: 02/04/02
*PURPOSE:
* Tries to get a server key for this session. Returns
* success.
*
*ARGUMENTS:
* mapName - Name of the map
* port - Port we are running on
* gameType - Game Type
* ai - Is AI allowed
* mines - Mines allowed
* password - Has password
* numBases - Number of bases
* numPills - Number of pills
* freeBases - Free bases
* freePills - Free pills
* numPlayers - number of players
* startTime - Game start time
*********************************************************/
bool winbolonetRequestServerKey(char *mapName, unsigned short port, BYTE gameType, BYTE ai, bool mines, bool password, BYTE numBases, BYTE numPills, BYTE freeBases, BYTE freePills, BYTE numPlayers, long startTime);

/*********************************************************
*NAME:          winbolonetRequestClientKey
*AUTHOR:        John Morrison
*CREATION DATE: 31/03/02
*LAST MODIFIED: 31/03/02
*PURPOSE:
* Attempts to get a client session key from WinBolo.net
* Returns success.
*
*ARGUMENTS:
* userName  - WinBolo.net account name
* password  - Password for the account
* serverKey - Session key for the server
* errorMsg  - Buffer to hold Error message if required
*********************************************************/
bool winbolonetRequestClientKey(char *userName, char *password, BYTE *serverKey, char *errorMsg);

/*********************************************************
*NAME:          winboloNetGetServerKey
*AUTHOR:        John Morrison
*CREATION DATE: 01/04/02
*LAST MODIFIED: 01/04/02
*PURPOSE:
* Copies the server key into keyBuff. Will be NULL if
* not participating in WinBolo.net
*
*ARGUMENTS:
* keyBuff - Buffer to hold key
*********************************************************/
void winboloNetGetServerKey(BYTE *keyBuff);

/*********************************************************
*NAME:          winboloNetGetMyClientKey
*AUTHOR:        John Morrison
*CREATION DATE: 01/04/02
*LAST MODIFIED: 01/04/02
*PURPOSE:
* Copies this clients key into keyBuff. Will be NULL if
* not set or not participating in WinBolo.net. Expected
* to be called by clients
*
*ARGUMENTS:
* keyBuff - Buffer to hold key
*********************************************************/
void winboloNetGetMyClientKey(BYTE *keyBuff);

/*********************************************************
*NAME:          winboloNetVerifyClientKey
*AUTHOR:        John Morrison
*CREATION DATE: 01/04/02
*LAST MODIFIED: 01/04/02
*PURPOSE:
* Verifies a client key by sending it to the server for
* authentication. Returns if its a valid key for this
* session
*
*ARGUMENTS:
* keyBuff   - Buffer to hold key
* userName  - Username of the player
* playerNum - Player position Number
*********************************************************/
bool winboloNetVerifyClientKey(BYTE *keyBuff, char *userName, BYTE playerNum);

/*********************************************************
*NAME:          winboloNetClientLeaveGame
*AUTHOR:        John Morrison
*CREATION DATE: 01/04/02
*LAST MODIFIED: 01/04/02
*PURPOSE:
* Called when a player leaves the game. Tells the server
* so.
*
*ARGUMENTS:
* playerNum  - Player position Number
* numPlayers - Number of players now in the game
* freeBases  - Number of free bases
* freePills  - Number of free pills
*********************************************************/
void winboloNetClientLeaveGame(BYTE playerNum, BYTE numPlayers, BYTE freeBases, BYTE freePills);

/*********************************************************
*NAME:          winbolonetGoodbye
*AUTHOR:        John Morrison
*CREATION DATE: 02/04/02
*LAST MODIFIED: 02/04/02
*PURPOSE:
* Destroys the winbolonet module.
* Cleans up any open libraries
*
*ARGUMENTS:
* 
*********************************************************/
void winbolonetGoodbye();

/*********************************************************
*NAME:          winbolonetServerSendTeams
*AUTHOR:        John Morrison
*CREATION DATE: 21/02/03
*LAST MODIFIED: 21/02/03
*PURPOSE:
* Sends the list of teams at the end of the game.
*
*ARGUMENTS:
* array    - BYTE array containing team memberships
* length   - Length of the array
* numTeams - Number of teams in the array
*********************************************************/
void winbolonetServerSendTeams(BYTE *array, BYTE length, BYTE numTeams);


/*********************************************************
*NAME:          winbolonetAddEventTwoPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 04/04/02
*LAST MODIFIED: 04/04/02
*PURPOSE:
* Adds a WinBolo.net Event for sending to the server
*
*ARGUMENTS:
*  eventType - Type of event this is
*  isServer  - Are we the server for this and not a client
*  playerA   - Player A player Number
*  playerB   - Player B player Number
*********************************************************/
void winbolonetAddEvent(BYTE eventType, bool isServer, BYTE playerA, BYTE playerB);

/*********************************************************
*NAME:          winbolonetServerUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 04/04/02
*LAST MODIFIED: 16/02/03
*PURPOSE:
* Sends a server winbolo.net update
*
*ARGUMENTS:
* numPlayer    - Number of pleyers in the game
* numFreePills - Number of free bases in the game
* numFreePills - Number of free pills in the game
* sendNow      - If TRUE the data should not be queued
*********************************************************/
void winbolonetServerUpdate(BYTE numPlayers, BYTE numFreeBases, BYTE numFreePills, bool sendNow);

/*********************************************************
*NAME:          winbolonetIsRunning
*AUTHOR:        John Morrison
*CREATION DATE: 14/04/02
*LAST MODIFIED: 14/04/02
*PURPOSE:
* Returns if the winbolonet module is running or not
*
*ARGUMENTS:
*
*********************************************************/
bool winbolonetIsRunning();

/*********************************************************
*NAME:          winboloNetIsPlayerParticipant
*AUTHOR:        John Morrison
*CREATION DATE: 07/09/02
*LAST MODIFIED: 07/09/02
*PURPOSE:
* Returns if this player number is a winbolo.net
* particpant or not
*
*ARGUMENTS:
* playerNum - Player number to check
*********************************************************/
bool winboloNetIsPlayerParticipant(BYTE playerNum);

/*********************************************************
*NAME:          winbolonetIsRunning
*AUTHOR:        John Morrison
*CREATION DATE: 10/01/04
*LAST MODIFIED: 10/01/04
*PURPOSE:
* Sends the game version to winbolo.net
*
*ARGUMENTS:
*
*********************************************************/
void winboloNetSendVersion();

/*********************************************************
*NAME:          winbolonetSendLock
*AUTHOR:        John Morrison
*CREATION DATE: 10/01/04
*LAST MODIFIED: 10/01/04
*PURPOSE:
* Sends whether the game is locked or not to winbolo.net
*
*ARGUMENTS:
* isLocked - Is this game locked or not
*********************************************************/
void winboloNetSendLock(bool isLocked);

#endif /* __WINBOLO_NET_H */

