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
*Filename:      players.h
*Author:        John Morrison
*Creation Date: 18/02/99
*Last Modified: 25/07/04
*Purpose:
*  Looks after players. Alliences between etc.
*********************************************************/

#ifndef PLAYERS_H
#define PLAYERS_H

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

#ifndef UCHAR36_DEFINED
#define UCHAR36_DEFINED
typedef struct { u_char c[36]; } u_char36;
#endif

typedef struct playersObj *players;

typedef struct {
  bool inUse;                       /* Is player slot in use? */
  char playerName[PLAYER_NAME_LEN]; /* Player name */
  char location[512];               /* Location field */
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
} player;

/* Array of all the players */
struct playersObj {
  player item[MAX_TANKS];
  char playerBrainNames[MAX_TANKS][PLAYER_NAME_LEN];   /* Brain information */
  BYTE myPlayerNum; /* Your own player number */
};

/* Prototypes */

/*********************************************************
*NAME:          playersCreate
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Sets up the players structure.
*
*ARGUMENTS:
* plrs - Pointer to the players object to create
*********************************************************/
void playersCreate(players *plrs);

/*********************************************************
*NAME:          playersDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Destroys the playesr structure
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
void playersDestroy(players *plrs);

/*********************************************************
*NAME:          playersGetSelf
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns your own player number
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
BYTE playersGetSelf(players *plrs);

/*********************************************************
*NAME:          playersSetSelf
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Sets your own player number and player name. Returns 
* whether the operation succeeded.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* playerName - The player name to set
*********************************************************/
bool playersSetSelf(players *plrs, BYTE playerNum, char *playerName);

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
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* playerName - The player name to set
*********************************************************/
bool playersSetPlayerName(players *plrs, BYTE playerNum, char *playerName);

/*********************************************************
*NAME:          playersSetPlayersMenu
*AUTHOR:        John Morrison
*CREATION DATE: 09/02/02
*LAST MODIFIED: 09/02/02
*PURPOSE:
* Sets the entries in the players menu if they are in use
*
*ARGUMENTS:
* plrs - Pointer to the players object 
********************************************************/
void playersSetPlayersMenu(players *plrs);

/*********************************************************
*NAME:          playersSetPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Sets a player up.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* playerName - The player name to set
* location   - Location string of the player.
* mx         - Map X location
* my         - Map Y location
* px         - Pixel X location
* py         - Pixel Y location
* frame      - Tank animation frame.
* onBoat     - is the player on a boat?
* numAllies  - Number of Allies the player has
* allies     - BYTE buffer containing each allie
*********************************************************/
void playersSetPlayer(players *plrs, BYTE playerNum, char *playerName, char *location, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, bool onBoat, BYTE numAllies, BYTE *allies);

/*********************************************************
*NAME:          playerSetLocation
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
* Sets the location of the player at ip to the hostname
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* ip   - Current IP address of the player
* host - Hostname of the player
*********************************************************/
void playerSetLocation(players *plrs, char *ip, char *host);

/*********************************************************
*NAME:          playersUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 28/2/99
*PURPOSE:
* Updates a player with specific location data.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* mx         - Map X location
* my         - Map Y location
* px         - Pixel X location
* py         - Pixel Y location
* frame      - Tank animation frame.
* onBoat     - is the player on a boat?
* lgmMX      - Lgm Map X Position
* lgmMY      - Lgm Map Y Position
* lgmPX      - Lgm Map X Position
* lgmPY      - Lgm Map Y Position
* lgmFrame   - Lgm Frame number
*********************************************************/
void playersUpdate(players *plrs, BYTE playerNum, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, bool onBoat, BYTE lgmMX, BYTE lgmMY, BYTE lgmPX, BYTE lgmPY, BYTE lgmFrame);

/*********************************************************
*NAME:          playersGameTickUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 15/09/02
*LAST MODIFIED: 15/09/02
*PURPOSE:
* Updates a player with specific location data.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* mx         - Map X location
* my         - Map Y location
* px         - Pixel X location
* py         - Pixel Y location
* frame      - Tank animation frame.
* onBoat     - is the player on a boat?
* lgmMX      - Lgm Map X Position
* lgmMY      - Lgm Map Y Position
* lgmPX      - Lgm Map X Position
* lgmPY      - Lgm Map Y Position
* lgmFrame   - Lgm Frame number
*********************************************************/
void playersGameTickUpdate(players *plrs);

/*********************************************************
*NAME:          playersGetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Gets a player name.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* dest       - Destination string
*********************************************************/
void playersGetPlayerName(players *plrs, BYTE playerNum, char *dest);

/*********************************************************
*NAME:          playersGetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Gets a player location.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* dest       - Destination string
*********************************************************/
void playersGetPlayerLocation(players *plrs, BYTE playerNum, char *dest);

/*********************************************************
*NAME:          playersMakeMessageName
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Makes the message name for a specific player
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* dest       - Destination string
*********************************************************/
void playersMakeMessageName(players *plrs, BYTE playerNum, char *dest);

/*********************************************************
*NAME:          playersMakeScreenName
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Makes the message name for a specific player
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* dest       - Destination string
*********************************************************/
void playersMakeScreenName(players *plrs, BYTE playerNum, char *dest);

/*********************************************************
*NAME:          playersIsAllie
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns whether playerA is allied to playerB
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerA - The player number to check
* playerB  - The player number to check
*********************************************************/
bool playersIsAllie(players *plrs, BYTE playerA, BYTE playerB);

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
* plrs - Pointer to the players object 
* playerNum - The player number to check
*********************************************************/
BYTE playersGetNumAllie(players *plrs, BYTE playerNum);

/*********************************************************
*NAME:          playersNameTaken
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns whether some player already is using that name
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* checkName - The player name to check
*********************************************************/
bool playersNameTaken(players *plrs, char *checkName);

/*********************************************************
*NAME:          playersScreenAllience
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns a player is screen allience type.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - Player number to check
*********************************************************/
tankAlliance playersScreenAllience(players *plrs, BYTE playerNum);

/*********************************************************
*NAME:          playersMakeScreenTanks
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED:  8/1/00
*PURPOSE:
* Adds each player to the screen tanks structure
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* value    - screen tanks data structure
* leftPos  - Left bound
* rightPos - Left bound
* top      - top bound
* bottom   - Bottom bound
*********************************************************/
void playersMakeScreenTanks(players *plrs, screenTanks *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

/*********************************************************
*NAME:          playersMakeScreenLgm
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED:  7/3/99
*PURPOSE:
* Adds each players lgm to the screen LGM structure
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* value    - screen LGM data structure
* leftPos  - Left bound
* rightPos - Left bound
* top      - top bound
* bottom   - Bottom bound
*********************************************************/
void playersMakeScreenLgm(players *plrs, screenLgm *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

/*********************************************************
*NAME:          playersGetNumPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns the number of players in the game
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
BYTE playersGetNumPlayers(players *plrs);

/*********************************************************
*NAME:          playersIsTankCloser
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED: 19/2/99
*PURPOSE:
* Returns whether a tank not allied to the pillbox 
* is closer then this players tank.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* x          - X co-ordinate of the pillbox
* y          - Y co-ordinate of the pillbox
* pillOwner  - Who owns the pill
* tankAmount - My tanks distance from pill
*********************************************************/
bool playersIsTankCloser(players *plrs, WORLD x, WORLD y, BYTE pillOwner, double tankAmount);

/*********************************************************
*NAME:          playersIsTankHit
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED: 31/7/00
*PURPOSE:
* Returns the player number if a player was hit otherwise
* returns NEUTRAL (255)
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* x     - X co-ordinate of the shell
* y     - Y co-ordinate of the shell
* angle - Angle the shell is traveling
* owner - Owner of the shell (players can not be hit
*         by there own shells
*********************************************************/
BYTE playersIsTankHit(players *plrs, WORLD x, WORLD y, TURNTYPE angle, BYTE owner);

/*********************************************************
*NAME:          playersMakeNetAlliences
*AUTHOR:        John Morrison
*CREATION DATE: 25/2/99
*LAST MODIFIED: 29/2/99
*PURPOSE:
* Returns the number of alliences playerNum has. Also
* copies each into the array value
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - Player number to make for
* value     - Array to hold the alliences
*********************************************************/
BYTE playersMakeNetAlliences(players *plrs, BYTE playerNum, BYTE *value);

/*********************************************************
*NAME:          playersGetFirstNotUsed
*AUTHOR:        John Morrison
*CREATION DATE: 25/2/99
*LAST MODIFIED: 25/2/99
*PURPOSE:
* Returns the first not used player number.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* value - Array to hold the alliences
*********************************************************/
BYTE playersGetFirstNotUsed(players *plrs);

/*********************************************************
*NAME:          playersLeaveGame
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED: 2/11/99
*PURPOSE:
* A player has left the game.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The number of the player that has left
*********************************************************/
void playersLeaveGame(players *plrs, BYTE playerNum);

/*********************************************************
*NAME:          playersSetMenuItems
*AUTHOR:        John Morrison
*CREATION DATE: 23/3/99
*LAST MODIFIED: 23/3/99
*PURPOSE:
* Sets all the players in the players menu
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
void playersSetMenuItems(players *plrs);

/*********************************************************
*NAME:          playersGetNumAllies
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Returns the number of allies to your player. (Includes
* self
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
int playersGetNumAllies(players *plrs);

/*********************************************************
*NAME:          playersGetNumChecked
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Gets the number of players with the checked bytes set to
* TRUE
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
int playersGetNumChecked(players *plrs);

/*********************************************************
*NAME:          playersCheckAllies
*AUTHOR:        John Morrison
*CREATION DATE:  6/4/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Checks all your players allies bytes and sends frontend
* messages
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
void playersCheckAllies(players *plrs);

/*********************************************************
*NAME:          playersCheckAllNone
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Checks/Unchecks all players allies bytes and sends 
* frontend messages
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* isChecked - TRUE if check all
*********************************************************/
void playersCheckAllNone(players *plrs, bool isChecked);

/*********************************************************
*NAME:          playersToggleCheckedState
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Toggles the checked state of a player.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The number of the player to check
*********************************************************/
void playersToggleCheckedState(players *plrs, BYTE playerNum);

/*********************************************************
*NAME:          playersCheckNearbyPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Checks nearby players
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* xValue - Your tanks X Map position
* yValue - Your tanks Y Map position
*********************************************************/
void playersCheckNearbyPlayers(players *plrs, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          playersNumNearbyPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Returns number of nearby players. (Includes self)
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* xValue - Your tanks X Map position
* yValue - Your tanks Y Map position
*********************************************************/
int playersNumNearbyPlayers(players *plrs, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          playersSendMessageAllAllies
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends message to all allies
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* message - Message to send
*********************************************************/
void playersSendMessageAllAllies(players *plrs, char *messageStr);

/*********************************************************
*NAME:          playersSendMessageAllSelected
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends message to all selected players
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* message - Message to send
*********************************************************/
void playersSendMessageAllSelected(players *plrs, char *messageStr);

/*********************************************************
*NAME:          playersSendMessageAllNearby
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends message to all nearby players
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* xValue - Your tanks X Map position
* yValue - Your tanks Y Map position
* message - Message to send
*********************************************************/
void playersSendMessageAllNearby(players *plrs, BYTE xValue, BYTE yValue, char *messageStr);

/*********************************************************
*NAME:          playersIsInUse
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
* Returns whether a player Number is in use
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The player num to check
*********************************************************/
bool playersIsInUse(players *plrs, BYTE playerNumber);

/*********************************************************
*NAME:          playersGetLgmDetails
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
* Gets the LGM details for a player
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The player num to check
* mx        - LGM Map X Position
* my        - LGM Map Y Position
* px        - LGM Pixel X Position
* py        - LGM Pixel Y Position
* frame     - LGM Frame
*********************************************************/
void playersGetLgmDetails(players *plrs, BYTE playerNumber, BYTE *mx, BYTE *my , BYTE *px, BYTE *py, BYTE *frame);

/*********************************************************
*NAME:          playersCheckCollision
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED:  4/11/99
*PURPOSE:
* Checks for a collision between our tank (given as 
* variables & the tanks in the players structure)
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The player num to check
* xValue  - Tank X World co-ordinate
* yValue  - Tank Y World co-ordinate
* leftPos - If left < 0 then hit from left > 0 from right
* downPos - If up < 0 then hit from above > 0 from below
*********************************************************/
bool playersCheckCollision(players *plrs, BYTE playerNum, WORLD xValue, WORLD yValue, int *leftPos, int *downPos);

/*********************************************************
*NAME:          playersSetAllieMenu
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED:  1/11/99
*PURPOSE:
* Determines whether the request and leave alliance menu
* items should be checked or not and passes it onto the
* frontend
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
void playersSetAllieMenu(players *plrs);

/*********************************************************
*NAME:          playersRequestAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Process a request alliance request.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
void playersRequestAlliance(players *plrs);

/*********************************************************
*NAME:          playersLeaveAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Process a leave alliance request.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - Player number that is leaving the alliance
*********************************************************/
void playersLeaveAlliance(players *plrs, BYTE playerNum);

/*********************************************************
*NAME:          playersAcceptAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
* A player has been accepted into an alliance
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* acceptedBy - Who accepted them in
* newMember  - Who the new member is
*********************************************************/
void playersAcceptAlliance(players *plrs, BYTE acceptedBy, BYTE newMember);

/*********************************************************
*NAME:          playersConnectionLost
*AUTHOR:        John Morrison
*CREATION DATE: 2/11/99
*LAST MODIFIED: 2/11/99
*PURPOSE:
* Called if your connection is lost to a network game.
* Drops all other players except yourself.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The number of the player that has left
*********************************************************/
void playersConnectionLost(players *plrs);

/*********************************************************
*NAME:          playersGetBrainTanksInRect
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 9/1/00
*PURPOSE:
*  Makes the brain tank info for each tank inside the
*  rectangle formed by the function parameters.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* leftPos   - Left position of rectangle
* rightPos  - Right position of rectangle
* top    - Top position of rectangle
* bottom - Bottom position of rectangle
* tankX  - Our tanks X position
* tankY  - Our tanks Y position
*********************************************************/
void playersGetBrainTanksInRect(players *plrs, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom, WORLD tankX, WORLD tankY);

/*********************************************************
*NAME:          playersGetBrainLgmsInRect
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 28/11/99
*PURPOSE:
*  Makes the brain lgm info for each lgm inside the
*  rectangle formed by the function parameters.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* left   - Left position of rectangle
* rightPos  - Right position of rectangle
* topPos    - Top position of rectangle
* bottom - Bottom position of rectangle
*********************************************************/
void playersGetBrainLgmsInRect(players *plrs, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

/*********************************************************
*NAME:          playersGetBrainsNamesArray
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Gets the players brain name array.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
u_char36 **playersGetBrainsNamesArray(players *plrs);

/*********************************************************
*NAME:          playersGetAlliesBitMap
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED:   4/7/00
*PURPOSE:
*  Returns the playerBitMap of all players that are allied
*  to us.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - Player number to get for
*********************************************************/
unsigned long playersGetAlliesBitMap(players *plrs, BYTE playerNum);

/*********************************************************
*NAME:          playersSendAiMessage
*AUTHOR:        John Morrison
*CREATION DATE: 12/12/99
*LAST MODIFIED: 12/12/99
*PURPOSE:
*  Called when a brain wishes to send a message to players
*  in the game.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* bitMap     - The checked and unchecked player bitMap
* messageStr - The message to be sent.
*********************************************************/
void playersSendAiMessage(players *plrs, unsigned long bitMap, char *messageStr);

/*********************************************************
*NAME:          playersPrepareLogSnapshotForPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 25/07/04
*LAST MODIFIED: 25/07/04
*PURPOSE:
* Prepares a single player log snapshot. Returns if the
* player spot is in use
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - Player to get snapshot for
* buff      - Destination buffer
* len       - Length of the buffer
*********************************************************/
bool playersPrepareLogSnapshotForPlayer(players *value, BYTE playerNum, BYTE *buff, BYTE *len);

void playersCheckUpdate(players *plrs, BYTE playerNum);

bool playersNeedUpdate(players *plrs, BYTE playerNum);

void playerNeedUpdateDone(players *plrs);


/*********************************************************
*NAME:          playersSetMyLastPlayerName
*AUTHOR:        Chris Lesnieski
*CREATION DATE: 14/02/09
*LAST MODIFIED: 14/02/09
*PURPOSE:
* Will copy a string to the myLastPlayerName variable.
* This should be done on initial start-up of WinBolo when
* it loads the winbolo.ini file.
*
*ARGUMENTS:
* dest       - Name to be set as player's previous name 
*********************************************************/
void playersSetMyLastPlayerName(char *dest);

#endif /* PLAYERS_H */

