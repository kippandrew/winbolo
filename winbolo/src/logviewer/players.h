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
*Name:          Players
*Filename:      players.c
*Author:        John Morrison
*Creation Date: 18/02/99
*Last Modified: 05/05/01
*Purpose:
*  Looks after players. Alliences between etc.
*********************************************************/

#ifndef _PLAYERS_H
#define _PLAYERS_H
#include "global.h"

#include "allience.h"
#include "screentank.h"
#include "screenlgm.h"

/* Defines */

#define NO_TANK "???\0"

/* Area tanks must be in to be selected in the 'select nearby' option */
#define PLAYER_MAX_SELECT_LEFT -9
#define PLAYER_MAX_SELECT_RIGHT 9
#define PLAYER_MAX_SELECT_TOP -9
#define PLAYER_MAX_SELECT_BOTTOM 9

/* Brain Stuff */
#define PLAYERS_BRAIN_OBJECT_TYPE_TANK 0
#define PLAYERS_BRAIN_OBJECT_TYPE_LGM 4
#define PLAYERS_BRAIN_OBJECT_TYPE_PARA 5
#define PLAYERS_BRAIN_NEUTRAL 2
#define PLAYERS_BRAIN_HOSTILE 1
#define PLAYERS_BRAIN_FRIENDLY 0

#define NO_TEAM_SET 0xFF

/* The different types of games there are */
typedef struct {
  bool inUse;                       /* Is player slot in use? */
  char playerName[PLAYER_NAME_LEN]; /* Player name */
  char location[512];   /* Location field */
  allience allie;                   /* Alliences this player has */
  BYTE mapX;                        /* Map X and Y co-ordinates */
  BYTE mapY;
  BYTE pixelX;                      /* Pixel X and Y co-ordinates */
  BYTE pixelY;
  BYTE frame;                       /* Animation frame */
  bool onBoat;                      /* Is this player on a boat ? */
  /* LGM Stuff */
  BYTE lgmMapX;                     /* LGM Map X and Y positions */
  BYTE lgmMapY;
  BYTE lgmPixelX;                   /* LGM Pixel X and Y positions */
  BYTE lgmPixelY;
  BYTE lgmFrame;                    /* LGM Frame */
  /* Misc */
  bool isChecked;                   /* Is this item checked */
  bool needUpdate;
  BYTE team;
} player;

/* Array of all the players */
typedef struct {
  player item[MAX_TANKS];
} players;



/*********************************************************
*NAME:          playersCreate
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Sets up the players structure.
*
*ARGUMENTS:
*
*********************************************************/
void playersCreate();

/*********************************************************
*NAME:          playersDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Destroys the playesr structure
*
*ARGUMENTS:
*
*********************************************************/
void playersDestroy();

/*********************************************************
*NAME:          playersGetSelf
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns your own player number
*
*ARGUMENTS:
*
*********************************************************/
BYTE playersGetSelf();

void playersSetSelf(BYTE pn);
  

/*********************************************************
*NAME:          playersSetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 18/02/99
*LAST MODIFIED: 05/05/01
*PURPOSE:
* Sets/changes a player name. Returns whether the operation
* succeed or not. (Fails if name is already in use) If it
* sccueeds then it makes the appropriate anouncement
*
*ARGUMENTS:
*  playerNum  - The player number to set
*  playerName - The player name to set
*********************************************************/
bool playersSetPlayerName(BYTE playerNum, char *playerName);

/*********************************************************
*NAME:          playersSetPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Sets a player up.
*
*ARGUMENTS:
*  playerNum  - The player number to set
*  playerName - The player name to set
*  location   - Location string of the player.
*  mx         - Map X location
*  my         - Map Y location
*  px         - Pixel X location
*  py         - Pixel Y location
*  frame      - Tank animation frame.
*  onBoat     - is the player on a boat?
*  numAllies  - Number of Allies the player has
*  allies     - BYTE buffer containing each allie
*  announce   - Do we want to announce this fact?
*  override   - Override if the game is fastfowarding
*********************************************************/
void playersSetPlayer(BYTE playerNum, char *playerName, char *location, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, bool onBoat, BYTE numAllies, BYTE *allies, bool announce, bool override);

void playersUpdateTank(BYTE playerNum, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, bool onBoat);


void playersUpdateLgm(BYTE playerNum, BYTE lgmMX, BYTE lgmMY, BYTE lgmPX, BYTE lgmPY, BYTE lgmFrame);

/*********************************************************
*NAME:          playersGetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Gets a player name.
*
*ARGUMENTS:
*  playerNum  - The player number to set
*  dest       - Destination string
*********************************************************/
void playersGetPlayerName(BYTE playerNum, char *dest);

/*********************************************************
*NAME:          playersMakeMessageName
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Makes the message name for a specific player
*
*ARGUMENTS:
*  playerNum  - The player number to set
*  dest       - Destination string
*********************************************************/
void playersMakeMessageName(BYTE playerNum, char *dest);

/*********************************************************
*NAME:          playersMakeScreenName
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Makes the message name for a specific player
*
*ARGUMENTS:
*  playerNum  - The player number to set
*  dest       - Destination string
*********************************************************/
void playersMakeScreenName(BYTE playerNum, char *dest);

/*********************************************************
*NAME:          playersIsAllie
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns whether playerA is allied to playerB
*
*ARGUMENTS:
*  playerA - The player number to check
*  playerB  - The player number to check
*********************************************************/
bool playersIsAllie(BYTE playerA, BYTE playerB);

/*********************************************************
*NAME:          playersGetNumAllie
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns the number allies a player has. (Atleast 1 as it
* includes themselves)
*
*ARGUMENTS:
*  playerNum - The player number to check
*********************************************************/
BYTE playersGetNumAllie(BYTE playerNum);

/*********************************************************
*NAME:          playersScreenAllience
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns a player is screen allience type.
*
*ARGUMENTS:
*  playerNum - Player number to check
*********************************************************/
tankAlliance playersScreenAllience(BYTE playerNum);

/*********************************************************
*NAME:          playersMakeScreenTanks
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED:  8/1/00
*PURPOSE:
* Adds each player to the screen tanks structure
*
*ARGUMENTS:
*  value    - screen tanks data structure
*  leftPos  - Left bound
*  rightPos - Left bound
*  top      - top bound
*  bottom   - Bottom bound
*********************************************************/
void playersMakeScreenTanks(screenTanks *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

/*********************************************************
*NAME:          playersMakeScreenLgm
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED:  7/3/99
*PURPOSE:
* Adds each players lgm to the screen LGM structure
*
*ARGUMENTS:
*  value    - screen LGM data structure
*  leftPos  - Left bound
*  rightPos - Left bound
*  top      - top bound
*  bottom   - Bottom bound
*********************************************************/
void playersMakeScreenLgm(screenLgm *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

/*********************************************************
*NAME:          playersGetNumPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns the number of players in the game
*
*ARGUMENTS:
*
*********************************************************/
BYTE playersGetNumPlayers();

/*********************************************************
*NAME:          playersLeaveGame
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED: 2/11/99
*PURPOSE:
* A player has left the game.
*
*ARGUMENTS:
*  playerNum - The number of the player that has left
*  announce  - Whether to announce this or not
*********************************************************/
void playersLeaveGame(BYTE playerNum, bool announce);

/*********************************************************
*NAME:          playersIsInUse
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
* Returns whether a player Number is in use
*
*ARGUMENTS:
*  playerNum - The player num to check
*********************************************************/
bool playersIsInUse(BYTE playerNumber);

/*********************************************************
*NAME:          playersGetLgmDetails
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
* Gets the LGM details for a player
*
*ARGUMENTS:
*  playerNum - The player num to check
*  mx        - LGM Map X Position
*  my        - LGM Map Y Position
*  px        - LGM Pixel X Position
*  py        - LGM Pixel Y Position
*  frame     - LGM Frame
*********************************************************/
void playersGetLgmDetails(BYTE playerNumber, BYTE *mx, BYTE *my , BYTE *px, BYTE *py, BYTE *frame);


/*********************************************************
*NAME:          playersLeaveAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Process a leave alliance request.
*
*ARGUMENTS:
*  playerNum - Player number that is leaving the alliance
*********************************************************/
void playersLeaveAlliance(BYTE playerNum);

/*********************************************************
*NAME:          playersAcceptAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
* A player has been accepted into an alliance
*
*ARGUMENTS:
*  acceptedBy - Who accepted them in
*  newMember  - Who the new member is
*********************************************************/
void playersAcceptAlliance(BYTE acceptedBy, BYTE newMember);

void playersLgmZero();

bool playersChooseView(int x, int y);

BYTE playersGetCentredX();
BYTE playersGetCentredY();
BYTE playersGetTeamId(BYTE playerNum);
BYTE playersGetUnusedTeam(BYTE playerNum);
BYTE playersGetTeamForOwner(BYTE owner);
void playersSetTeams(BYTE *pTeams);
void playersCopyPTeams(BYTE *dest);

#endif /* _PLAYERS_H */
