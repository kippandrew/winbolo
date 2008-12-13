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
*Name:          PlayersRejoin
*Filename:      playersrejoin.h
*Author:        John Morrison
*Creation Date: 22/6/00
*Last Modified: 22/6/00
*Purpose:
*  Looks after players rejoin and ownerships.
*********************************************************/

#ifndef _PLAYERS_REJOIN_H
#define _PLAYERS_REJOIN_H

#include "global.h"
#include "pillbox.h"
#include "bases.h"

#define MAX_REJOIN_TIME 6000

/* The different player rejoin information */
typedef struct {
  bool inUse;                       /* Is player slot in use? */
  char playerName[PLAYER_NAME_LEN]; /* Player name */
  PlayerBitMap pills;               /* pills and base ownership */
  PlayerBitMap bases;
  int timeOut;                     /* Time left */
} playerRejoinInfo;

/* Array of all the players */
typedef struct {
  playerRejoinInfo item[MAX_TANKS];
} playersRejoin;

/* Prototypes */

/*********************************************************
*NAME:          playersRejoinCreate
*AUTHOR:        John Morrison
*CREATION DATE: 22/6/00
*LAST MODIFIED: 22/6/00
*PURPOSE:
* Sets up the players rejoin structure.
*
*ARGUMENTS:
*
*********************************************************/
void playersRejoinCreate();

/*********************************************************
*NAME:          playersRejoinDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 22/6/00
*LAST MODIFIED: 22/6/00
*PURPOSE:
* Destroys the players rejoin structure
*
*ARGUMENTS:
*
*********************************************************/
void playersRejoinDestroy();

/*********************************************************
*NAME:          playersRejoinUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 22/6/00
*LAST MODIFIED: 22/6/00
*PURPOSE:
* Updates the players rejoin structure. Timeouts etc.
*
*ARGUMENTS:
*
*********************************************************/
void playersRejoinUpdate();

/*********************************************************
*NAME:          playersAddPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 22/6/00
*LAST MODIFIED: 22/6/00
*PURPOSE:
* Adds a player to the rejoin structure
*
*ARGUMENTS:
*
*********************************************************/
void playersRejoinAddPlayer(char *playerName, PlayerBitMap pills, PlayerBitMap bases);

/*********************************************************
*NAME:          playersRejoinRequest
*AUTHOR:        John Morrison
*CREATION DATE: 22/6/00
*LAST MODIFIED: 22/6/00
*PURPOSE:
* A player wish to rejoin. See if they exists and assign
* items to his ownership if they aren't owned by someone
* else.
*
*ARGUMENTS:
*  playerName - The player name requesting the rejoin
*  playerNum  - The rejoining players player number
*  pb         - Pointer to the pillboxes structure
*  bs         - Pointer to the bases structure
*********************************************************/
void playersRejoinRequest(char *playerName, BYTE playerNum, pillboxes *pb, bases *bs);

/*********************************************************
*NAME:          serverCoreRequestRejoin
*AUTHOR:        John Morrison
*CREATION DATE: 22/6/00
*LAST MODIFIED: 22/6/00
*PURPOSE:
* A player number has requested a rejoin
*
*ARGUMENTS:
*  playerNum - Player number requesting join
********************************************************/
void serverCoreRequestRejoin(BYTE playerNum);

#endif /* _PLAYERS_REJOIN_H */
