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
*Name:          BackEnd
*Filename:      BackEnd.h
*Author:        John Morrison
*Creation Date: 25/11/99
*Last Modified: 21/05/06
*Purpose:
*  Functions called by the front end
*********************************************************/

#ifndef _BACKEND_H
#define _BACKEND_H

#include "global.h"
#include "brain.h"
#include "screenlgm.h"
#include "screenbullet.h"
#include "screentank.h"
#include "building.h"
#include "explosions.h"
#include "floodfill.h"
#include "grass.h"
#include "minesexp.h"
#include "mines.h"
#include "rubble.h"
#include "swamp.h"
#include "tankexp.h"
#include "netpnb.h"
#include "netmt.h"

/* Defines */

/* Defines the screen sizes */
#define MAIN_SCREEN_SIZE_X 15 /*16 is bolo window size  times 16 pixels */
#define MAIN_SCREEN_SIZE_Y 15 /*18 is bolo window size  times 16 pixels */


/* Size of the back buffer */
#define MAIN_BACK_BUFFER_SIZE_X (MAIN_SCREEN_SIZE_X + 2)
#define MAIN_BACK_BUFFER_SIZE_Y (MAIN_SCREEN_SIZE_Y + 2)
/* The game timer is 20 milliseconds between events */
#define GAME_TICK_LENGTH 10 /* 20 */
#define GAME_NUMTOTALTICKS_SEC (1000 / GAME_TICK_LENGTH);
#define GAME_NUMGAMETICKS_SEC (1000 / 20);


/* Messages status on/off*/
#define MSG_NEWSWIRE 0
#define MSG_ASSISTANT 1
#define MSG_AI 2
#define MSG_NETWORK 3
#define MSG_NETSTATUS 4

/* Flag to indicate no gunsight is to be drawn */
#define NO_GUNSIGHT -1


/* Number of squares a tank must be in to see it in the forests */
#define MIN_SIGHT_DISTANCE_LEFT -1
#define MIN_SIGHT_DISTANCE_RIGHT 1

/* The different types of games there are */


#ifndef _LABELLEN_ENUM
#define _LABELLEN_ENUM

typedef enum {
  lblNone,
  lblShort,
  lblLong
} labelLen;

#endif 

#ifndef _UPDATETYPE_ENUM
#define _UPDATETYPE_ENUM
typedef enum {
  left,
  right,
  up,
  down,
  redraw
} updateType;

#endif

#ifndef _SNDEFFECTS_ENUM
#define _SNDEFFECTS_ENUM
typedef enum {
  shootSelf,
  shootNear,
  shotTreeNear,
  shotTreeFar,
  shotBuildingNear,
  shotBuildingFar,
  hitTankNear,
  hitTankFar,
  hitTankSelf,
  bubbles,
  tankSinkNear,
  tankSinkFar,
  bigExplosionNear,
  bigExplosionFar,
  farmingTreeNear,
  farmingTreeFar,
  manBuildingNear,
  manBuildingFar,
  manDyingNear,
  manDyingFar,
  manLayingMineNear,
  mineExplosionNear,
  mineExplosionFar,
  shootFar
} sndEffects;

#endif

#ifndef _AITYPE_ENUM
#define _AITYPE_ENUM

typedef enum {
  aiNone,
  aiYes,
  aiYesAdvantage
} aiType;

#endif 

#ifndef _GAMETYPE_ENUM
#define _GAMETYPE_ENUM

typedef enum {
  gameOpen = 1,
  gameTournament,
  gameStrictTournament
} gameType;

#endif

#ifndef _TANK_H
typedef enum {
  TNONE,       /* No Buttons being pressed */
  TLEFT,       /* Left button is being pressed */
  TRIGHT,      /* Right button is being pressed */
  TACCEL,      /* Acellerate Button */
  TDECEL,      /* Decellerate Button is being pressed */
  TLEFTACCEL,  /* Left + Accelerate */
  TRIGHTACCEL, /* Right + Acellerate */
  TLEFTDECEL,  /* Left + Decellerate */
  TRIGHTDECEL  /* Right + Decellearate */
} tankButton;
#endif

#ifndef BASES_H
typedef enum {
  baseDead,
  baseOwnGood,
  baseAllieGood,
  baseNeutral,
  baseEvil
} baseAlliance;
#endif

#ifndef PILLBOX_H
typedef enum {
  pillDead,
  pillAllie,
  pillGood,
  pillNeutral,
  pillEvil,
  pillTankGood,
  pillTankAllie,
  pillTankEvil
} pillAlliance;
#endif



/* Type definitions */
/* The screen object - Details what tiles are on the screen*/
typedef struct screenObj *screen;
struct screenObj {
  BYTE screenItem[MAIN_BACK_BUFFER_SIZE_X][MAIN_BACK_BUFFER_SIZE_Y];
};
/* Screen Mines - Array of boolean values that report whether
a tile on the screen whold have a mine on it*/
typedef struct screenMineObj *screenMines;
struct screenMineObj {
  bool mineItem[MAIN_BACK_BUFFER_SIZE_X][MAIN_BACK_BUFFER_SIZE_Y];
};

/* Defines the gunsight position on the screen */
/* If turned off mapX is set to NO_GUNSIGHT */
typedef struct {
  int mapX;
  BYTE mapY;
  BYTE pixelX;
  BYTE pixelY;
} screenGunsight;


/* Button Pressed - These are the valid items 
   that should be passed to this module*/
#ifndef _UPDATETYPE_ENUM
#define _UPDATETYPE_ENUM
typedef enum {
  left,
  right,
  up,
  down,
  redraw
} updateType;

#endif

#ifndef _BUILDSELECT_ENUM
#define _BUILDSELECT_ENUM

/* The type of building operation currently being selected */
typedef enum {
  BsTrees,
  BsRoad,
  BsBuilding,
  BsPillbox,
  BsMine
} buildSelect;
#endif


#ifndef _PLAYERNUMBERS_ENUM 
#define _PLAYERNUMBERS_ENUM 
/* Player Numbers */
typedef enum {
  player01,
  player02,
  player03,
  player04,
  player05,
  player06,
  player07,
  player08,
  player09,
  player10,
  player11,
  player12,
  player13,
  player14,
  player15,
  player16
} playerNumbers;

#endif
/* Prototypes */

/*********************************************************
*NAME:          screenSetup
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 26/1/99
*PURPOSE:
*  Sets up all the variables - Should be run when the
*  program starts.
*
*ARGUMENTS:
*  game - The game type-Open/tournament/strict tournament
*  hiddenMines - Are hidden mines allowed
*  srtDelay    - Game start delay (50th second increments)
*  gmeLen      - Length of the game (in 50ths)
*                (-1 =unlimited)
*********************************************************/
void screenSetup(gameType game, bool hiddenMines, int srtDelay, long gmeLen);

/*********************************************************
*NAME:          screenDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Destroys the structures Should be called on
*  program exit
*
*ARGUMENTS:
*
*********************************************************/
void screenDestroy();

/*********************************************************
*NAME:          screenUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Updates the screen. Takes numerous directions
*
*ARGUMENTS:
*  value - Pointer to the screen structure
*********************************************************/
void screenUpdate(updateType value);

/*********************************************************
*NAME:          screenGetPos
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Gets the value of a square in the structure
*  Return RIVER if out of range
*
*ARGUMENTS:
*  value  - Pointer to the screen structure
*  xValue - The X co-ordinate
*  yValue - The Y co-ordinate
*********************************************************/
BYTE screenGetPos(screen *value,BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          screenIsMine
*AUTHOR:        John Morrison
*CREATION DATE: 6/11/98
*LAST MODIFIED: 6/11/98
*PURPOSE:
*  Returns if a square on the screen should have a mine
*  drawn on it.
*  If value is out of range returns FALSE
*
*ARGUMENTS:
*  value  - Pointer to the screenMines structure
*  xValue - The X co-ordinate
*  yValue - The Y co-ordinate
*********************************************************/
bool screenIsMine(screenMines *value,BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          screenLoadMap
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 12/12/99
*PURPOSE:
*  Loads a map. Returns if it was sucessful reading the
*  map or not.
*
*ARGUMENTS:
* fileName - File name and path to map to open
*  game - The game type-Open/tournament/strict tournament
*  hiddenMines - Are hidden mines allowed
*  srtDelay    - Game start delay (50th second increments)
*  gmeLen      - Length of the game (in 50ths) 
*                (-1 =unlimited)
*  playerName  - Name of the player
*  wantFree    - Should we free the backend after loading
*                Usually TRUE if you only want to check
*                if a map is valid
*********************************************************/
bool screenLoadMap(char *fileName, gameType game, bool hiddenMines, long srtDelay, long gmeLen, char *playerName, bool wantFree);

/*********************************************************
*NAME:          screenGameTick
*AUTHOR:        John Morrison
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  This function is called whenever a game tick occurs,
*
*ARGUMENTS:
* tb        - The combinations of tank buttons being 
*             pressed
* tankShoot - Is the fire key being pressed?
* isBrain   - True if a brain is running. Other 
*             parameters are ignored.
*********************************************************/
void screenGameTick(tankButton tb, bool tankShoot, bool isBrain);

/*********************************************************
*NAME:          screenKeysTick
*AUTHOR:        John Morrison
*CREATION DATE: 8/2/99
*LAST MODIFIED: 27/11/99
*PURPOSE:
* This function is called whenever a update keys tick
* happens (twice the game tick length)
*
*ARGUMENTS:
* tb      - The combinations of tank buttons being pressed
* isBrain - TRUE if brain is running. Other parameter is
*           ignored
*********************************************************/
void screenKeysTick(tankButton tb, bool isBrain);

/*********************************************************
*NAME:          screenNumBases
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Returns the number of bases
*
*ARGUMENTS:
*
*********************************************************/
BYTE screenNumBases(void);

/*********************************************************
*NAME:          screenBaseAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Returns the base alliance of a particular base for 
*  drawing.
*
*ARGUMENTS:
*  baseNum - The base number to get
*********************************************************/
baseAlliance screenBaseAlliance(BYTE baseNum);

/*********************************************************
*NAME:          screenNumPills
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Returns the number of pillboxes
*
*ARGUMENTS:
*
*********************************************************/
BYTE screenNumPills(void);

/*********************************************************
*NAME:          screenPillAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Returns the pill alliance of a particular pill for 
*  status drawing.
*
*ARGUMENTS:
*  pillNum - The pillbox number to get
*********************************************************/
pillAlliance screenPillAlliance(BYTE pillNum);

/*********************************************************
*NAME:          screenTanksAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns the tank allience of a specific player slot.
* sucessful or not.
*
*ARGUMENTS:
*  fileName - path and filename to save
*********************************************************/
tankAlliance screenTankAlliance(BYTE playerNum);

/*********************************************************
*NAME:          screenGetTankStats
*AUTHOR:        John Morrison
*CREATION DATE: 22/12/98
*LAST MODIFIED: 22/12/98
*PURPOSE:
*  Returns the tank shells, mines, armour and trees 
*
*ARGUMENTS:
*  shellsAmount - Pointer to hold number of shells
*  minesAmount  - Pointer to hold number of mines
*  armourAmount - Pointer to hold amount of armour
*  treesAmount  - Pointer to hold amount of trees
*********************************************************/
void screenGetTankStats(BYTE *shellsAmount, BYTE *minesAmount, BYTE *armourAmount, BYTE *treesAmount);

/*********************************************************
*NAME:          screenGunsightRange
*AUTHOR:        John Morrison
*CREATION DATE: 24/12/98
*LAST MODIFIED: 24/12/98
*PURPOSE:
*  Changes the gunsight range
*
*ARGUMENTS:
*  increase - Set to TRUE if should increase else 
*             decrease range
*********************************************************/
void screenGunsightRange(bool increase);

/*********************************************************
*NAME:          screenGunsightRange
*AUTHOR:        John Morrison
*CREATION DATE: 24/12/98
*LAST MODIFIED: 24/12/98
*PURPOSE:
*  Shows / Hides the gunsight
*
*ARGUMENTS:
*  shown - TRUE = Gunsight on.
*********************************************************/
void screenSetGunsight(bool shown);

/*********************************************************
*NAME:          screenGetMessages
*AUTHOR:        John Morrison
*CREATION DATE: 3/1/99
*LAST MODIFIED: 3/1/99
*PURPOSE:
*  Gets the messages on screen
*
*ARGUMENTS:
*  top - Top message line to get
*  top - Top message line to get
*********************************************************/
void screenGetMessages(char *top, char *bottom);

/*********************************************************
*NAME:          screenGetKillsDeaths
*AUTHOR:        John Morrison
*CREATION DATE:  8/1/99
*LAST MODIFIED:  8/1/99
*PURPOSE:
*  Gets the tanks kills/deaths
*
*ARGUMENTS:
*  kills  - The number of kills the tank has.
*  deaths - The number of times the tank has died
*********************************************************/
void screenGetKillsDeaths(int *kills, int *deaths);

/*********************************************************
*NAME:          screenShowMessages
*AUTHOR:        John Morrison
*CREATION DATE:  8/1/99
*LAST MODIFIED:  8/1/99
*PURPOSE:
*  Turns on/off messages menus stuff
*
*ARGUMENTS:
*  msgType - The message type that is being set
*  isShown - Is it being turned on or off
*********************************************************/
void screenShowMessages(BYTE msgType, bool isShown);

/*********************************************************
*NAME:          screenManMove
*AUTHOR:        John Morrison
*CREATION DATE: 10/1/99
*LAST MODIFIED: 27/5/00
*PURPOSE:
* The mouse has been clicked indicating a build operation
* is requested. Send request to lgm structure
*
*ARGUMENTS:
*  buildS - The building type selected
*********************************************************/
void screenManMove(buildSelect buildS);

/*********************************************************
*NAME:          screenSetAutoScroll
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
* The autoscrolling item has been checked on the frontend
* Pass the value onto the scroll module.
*
*ARGUMENTS:
*  isAuto - Is the scrolling option automatic or not?
*********************************************************/
void screenSetAutoScroll(bool isAuto);

/*********************************************************
*NAME:          screenTankLayMine
*AUTHOR:        John Morrison
*CREATION DATE: 21/1/99
*LAST MODIFIED: 21/1/99
*PURPOSE:
* Request for tank to lay mine has occured
*
*ARGUMENTS:
*
*********************************************************/
void screenTankLayMine();

/*********************************************************
*NAME:          screenPillNumPos
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
* The front end needs the pillbox number at a specific
* location for drawing its label
*
*ARGUMENTS:
*  mx - X Map Co-ordinate relative to the screen
*  my - Y Map Co-ordinate relative to the screen
*********************************************************/
BYTE screenPillNumPos(BYTE mx, BYTE my);

/*********************************************************
*NAME:          screenBaseNumPos
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
* The front end needs the base number at a specific
* location for drawing its label
*
*ARGUMENTS:
*  mx - X Map Co-ordinate relative to the screen
*  my - Y Map Co-ordinate relative to the screen
*********************************************************/
BYTE screenBaseNumPos(BYTE mx, BYTE my);

/*********************************************************
*NAME:          screenGetMapName
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
* The front end eants to know what the map name is.
* Make a copy for it.
*
*ARGUMENTS:
*  value - Place to hold copy of the map name
*********************************************************/
void screenGetMapName(char *value);

/*********************************************************
*NAME:          screenGetNumPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
* Returns the number of players in the game
*
*ARGUMENTS:
*
*********************************************************/
BYTE screenGetNumPlayers();

/*********************************************************
*NAME:          screenGetAllowHiddenMines
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
* Returns whether hidden mines are allowed in the game
* or not
*
*ARGUMENTS:
*
*********************************************************/
bool screenGetAllowHiddenMines();

/*********************************************************
*NAME:          screenGetGameTimeLeft
*AUTHOR:        John Morrison
*CREATION DATE: 27/1/99
*LAST MODIFIED: 27/1/99
*PURPOSE:
* Returns the time remaining the current game
*
*ARGUMENTS:
*
*********************************************************/
long screenGetGameTimeLeft();

/*********************************************************
*NAME:          screenGetGameStartDelay
*AUTHOR:        John Morrison
*CREATION DATE: 27/1/99
*LAST MODIFIED: 27/1/99
*PURPOSE:
* Returns the time remaining to start the current game
*
*ARGUMENTS:
*
*********************************************************/
long screenGetGameStartDelay();

/*********************************************************
*NAME:          screenGetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 1/2/99
*LAST MODIFIED: 1/2/99
*PURPOSE:
* Copies the player name into the value passed
*
*ARGUMENTS:
*  value - String to hold copy of the player name
*********************************************************/
void screenGetPlayerName(char *value);

/*********************************************************
*NAME:          screenSetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 1/2/99
*LAST MODIFIED: 1/2/99
*PURPOSE:
* Checks to see if the new name is taken. If not the it
* changes the players name to the new name. Returns whether
* the change took place or not
*
*ARGUMENTS:
*  value - New Player Name
*********************************************************/
bool screenSetPlayerName(char *value);

/*********************************************************
*NAME:          screenSetLabelOwnTank
*AUTHOR:        John Morrison
*CREATION DATE: 2/2/99
*LAST MODIFIED: 2/2/99
*PURPOSE:
* Sets whether should label itself to the value passed
*
*ARGUMENTS:
*  value - Should the tank be labeled?
*********************************************************/
void screenSetLabelOwnTank(bool value);

/*********************************************************
*NAME:          screenSetMesageLabelLen
*AUTHOR:        John Morrison
*CREATION DATE: 2/2/99
*LAST MODIFIED: 2/2/99
*PURPOSE:
* Sets the message label len to the value passed
*
*ARGUMENTS:
*  value - New length of the labels
*********************************************************/
void screenSetMesageLabelLen(labelLen value);

/*********************************************************
*NAME:          screenSetTankLabelLen
*AUTHOR:        John Morrison
*CREATION DATE: 2/2/99
*LAST MODIFIED: 2/2/99
*PURPOSE:
* Sets the tank label len to the value passed
*
*ARGUMENTS:
*  value - New length of the labels
*********************************************************/
void screenSetTankLabelLen(labelLen value);

/*********************************************************
*NAME:          screenTankView
*AUTHOR:        John Morrison
*CREATION DATE: 3/2/99
*LAST MODIFIED: 3/2/99
*PURPOSE:
* Frontend has requested a tank view.
*
*ARGUMENTS:
*
*********************************************************/
void screenTankView();

/*********************************************************
*NAME:          screenPillView
*AUTHOR:        John Morrison
*CREATION DATE: 03/02/99
*LAST MODIFIED: 21/01/01
*PURPOSE:
* Front end has requested a pill view.
*
*ARGUMENTS:
*  horz - If we are moving left or right (0 for neither)
*  vert - If we are moving up or down (0 for neither)
*********************************************************/
void screenPillView(int horz, int vert);

/*********************************************************
*NAME:          screenSendMessageAllPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 5/2/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Front end wants to send a message
*
*ARGUMENTS:
*  messageStr - Message to send
*********************************************************/
void screenSendMessageAllPlayers(char *messageStr);

/*********************************************************
*NAME:          screenSaveMap
*AUTHOR:        John Morrison
*CREATION DATE: 5/2/99
*LAST MODIFIED: 5/2/99
*PURPOSE:
* Saves the map. Returns whether the operation was 
* sucessful or not.
*
*ARGUMENTS:
*  fileName - path and filename to save
*********************************************************/
bool screenSaveMap(char *fileName);

/*********************************************************
*NAME:          screenLeaveGame
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED: 20/3/99
*PURPOSE:
* We want to quit a network game. Post the notification
* to all players.
*
*ARGUMENTS:
*
*********************************************************/
void screenLeaveGame(void);
/*********************************************************
*NAME:          screenTogglePlayerCheckState
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Toggles the check mark state on a player
*
*ARGUMENTS:
*
*********************************************************/
void screenTogglePlayerCheckState(BYTE playerNum);

/*********************************************************
*NAME:          screenCheckAllNonePlayers
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Checks/UnChecks all players.
*
*ARGUMENTS:
*  isChecked - TRUE if check all
*********************************************************/
void screenCheckAllNonePlayers(bool isChecked);

/*********************************************************
*NAME:          screenCheckAlliedPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Checks your allies
*
*ARGUMENTS:
*
*********************************************************/
void screenCheckAlliedPlayers(void);

/*********************************************************
*NAME:          screenCheckAlliedPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Checks nearby players
*
*ARGUMENTS:
*
*********************************************************/
void screenCheckNearbyPlayers(void);

/*********************************************************
*NAME:          screenNumCheckedPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Returns the number of checked players
*
*ARGUMENTS:
*
*********************************************************/
int screenNumCheckedPlayers(void);

/*********************************************************
*NAME:          screenNumCheckedPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Returns the number of allied players
*
*ARGUMENTS:
*
*********************************************************/
int screenNumAllies(void);

/*********************************************************
*NAME:          screenNumNearbyTanks
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Returns the number of nearby tanks
*
*ARGUMENTS:
*
*********************************************************/
int screenNumNearbyTanks(void);

/*********************************************************
*NAME:          screenSendMessageAllAllies
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends a message to all allied players
*
*ARGUMENTS:
*  messageStr - The message to send
*********************************************************/
void screenSendMessageAllAllies(char *messageStr);


/*********************************************************
*NAME:          screenSendMessageAllSelected
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends a message to all selected players
*
*ARGUMENTS:
*  messageStr - The message to send
*********************************************************/
void screenSendMessageAllSelected(char *messageStr);

/*********************************************************
*NAME:          screenSendMessageAllNearby
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends a message to all nearby players
*
*ARGUMENTS:
*  messageStr - The message to send
*********************************************************/
void screenSendMessageAllNearby(char *messageStr);

/*********************************************************
*NAME:          screenRequestAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Frontend has clicked the request alliance menu item
*
*ARGUMENTS:
*
*********************************************************/
void screenRequestAlliance();

/*********************************************************
*NAME:          screenLeaveAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Frontend has clicked the leave alliance menu item
*
*ARGUMENTS:
*
*********************************************************/
void screenLeaveAlliance();

/*********************************************************
*NAME:          screenGetLgmStatus
*AUTHOR:        John Morrison
*CREATION DATE: 14/11/99
*LAST MODIFIED: 14/11/99
*PURPOSE:
*  Gets the man status for drawing on the status bars
*
*ARGUMENTS:
*  isOut  - TRUE if man is out of tank
*  isDead - TRUE if man is dead
*  angle  - Angle man is travelling on
*********************************************************/
void screenGetLgmStatus(bool *isOut, bool *isDead, TURNTYPE *angle);

/*********************************************************
*NAME:          screenMakeBrainInfo
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 25/11/99
*PURPOSE:
*  Makes the information for a particular brain pass
*
*ARGUMENTS:
*  value - Pointer to the brain info structure
*  first - TRUE if this is the first time we have been
*          called
*********************************************************/
void screenMakeBrainInfo(BrainInfo *value, bool first);

/*********************************************************
*NAME:          screenExtractBrainInfo
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Called after the brain has executed. Extract the data
*  and cleanup
*
*ARGUMENTS:
*  value - Pointer to the brain info structure
*********************************************************/
void screenExtractBrainInfo(BrainInfo *value);

/*********************************************************
*NAME:          screenSetAiType
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Sets the AI type for this game
*
*ARGUMENTS:
*
*********************************************************/
void screenSetAiType(aiType value);

/*********************************************************
*NAME:          screenGetAiType
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Returns the AI type for this game
*
*ARGUMENTS:
*
*********************************************************/
aiType screenGetAiType();


/*********************************************************
*NAME:          screenGetTankAutoSlowdown
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/00
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Returns whether tank autoslowdown is enabled or not
*
*ARGUMENTS:
*
*********************************************************/
bool screenGetTankAutoSlowdown();

/*********************************************************
*NAME:          screenSetTankAutoSlowdown
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/00
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Sets whether tank autoslowdown is enabled or not
*
*ARGUMENTS:
*  useSlowdown - TRUE if auto slowdown is used
*********************************************************/
void screenSetTankAutoSlowdown(bool useSlowdown);

/*********************************************************
*NAME:          screenGetTankAutoHideGunsight
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/00
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Returns whether tank auto show/hide gunsight is enabled 
*  or not
*
*ARGUMENTS:
*
*********************************************************/
bool screenGetTankAutoHideGunsight();

/*********************************************************
*NAME:          screenSetTankAutoHideGunsight
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/00
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Sets whether tank auto show/hide gunsight is enabled 
*  or not
*
*ARGUMENTS:
*  useAutohide - TRUE if auto show/hide is used
*********************************************************/
void screenSetTankAutoHideGunsight(bool useAutohide);

/*********************************************************
*NAME:          screenSetCursorPos
*AUTHOR:        John Morrison
*CREATION DATE: 27/05/00
*LAST MODIFIED: 27/05/00
*PURPOSE:
*  Sets the cursor position in steps from the top and 
*  left corner position of the active screen
*
*ARGUMENTS:
*  posX - Left position
*  posY - Top position
*********************************************************/
void screenSetCursorPos(BYTE posX, BYTE posY);

/*********************************************************
*NAME:          screenGetCursorPos
*AUTHOR:        John Morrison
*CREATION DATE: 27/05/00
*LAST MODIFIED: 27/05/00
*PURPOSE:
*  Gets the cursor position in steps from the top and 
*  left corner position of the active screen. Returns if
*  the cursor should be shown or not
*
*ARGUMENTS:
*  posX - Pointer to hold left position
*  posY - Pointer to hold top position
*********************************************************/
bool screenGetCursorPos(BYTE *posX, BYTE *posY);

/*********************************************************
*NAME:          screenGenerateMapPreview
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
* Generates a map preview for the front end. A map preview
* is a 256x256 byte buff with each square equal to a map
* tile. tile numbers are identical to map square numbers
* except DEEP_SEA = 16, pills = 17, bases = 18 and starts
* = 19. Returns success
*
*ARGUMENTS:
*  fileName - Map file name to open
*  buff     - Buffer to copy into
*********************************************************/
bool screenGenerateMapPreview(char *fileName, BYTE *buff) ;

/*********************************************************
*NAME:          screenNetSetupTankGo
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 27/2/99
*PURPOSE:
*  Map download is complete and we are ready to star
*  playing
*
*ARGUMENTS:
*
*********************************************************/
void screenNetSetupTankGo();

/*********************************************************
*NAME:          screenSetupTank
*AUTHOR:        John Morrison
*CREATION DATE: 3/1/99
*LAST MODIFIED: 21/1/01
*PURPOSE:
*  Creates and sets up a tank. MUST be called after
*  map is loaded and screen setup is called.
*  Should only be called directly by single player
*  games.
*
*ARGUMENTS:
*  playerName - The player name controling the tank
*********************************************************/
void screenSetupTank(char *playerName);

/*********************************************************
*NAME:          screenSetPlayersMenu
*AUTHOR:        John Morrison
*CREATION DATE: 9/02/02
*LAST MODIFIED: 9/02/02
*PURPOSE:
* Sets the player menu entries
*
*ARGUMENTS:
*
*********************************************************/
void screenSetPlayersMenu();

/*********************************************************
*NAME:          screenGetGameRunning
*AUTHOR:        John Morrison
*CREATION DATE: 20/02/03
*LAST MODIFIED: 20/02/03
*PURPOSE:
* Returns if a game is running
*
*ARGUMENTS:
*
*********************************************************/
bool screenGetGameRunning();

/*********************************************************
*NAME:          screenGetBuildings
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the buildings Object
*
*ARGUMENTS:
*
*********************************************************/
building *screenGetBuildings();

/*********************************************************
*NAME:          screenGetExplosions
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the explosions Object
*
*ARGUMENTS:
*
*********************************************************/
explosions *screenGetExplosions();

/*********************************************************
*NAME:          screenGetFloodFill
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the floodfill object
*
*ARGUMENTS:
*
*********************************************************/
floodFill *screenGetFloodFill();

/*********************************************************
*NAME:          screenGetGrass
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the grass object
*
*ARGUMENTS:
*
*********************************************************/
grass *screenGetGrass();

/*********************************************************
*NAME:          screenGetMines
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the mines object
*
*ARGUMENTS:
*
*********************************************************/
mines *screenGetMines();

/*********************************************************
*NAME:          screenGetMinesExp
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the mines explosion object
*
*ARGUMENTS:
*
*********************************************************/
minesExp *screenGetMinesExp();

/*********************************************************
*NAME:          screenGetRubble
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the rubble object
*
*ARGUMENTS:
*
*********************************************************/
rubble *screenGetRubble();

/*********************************************************
*NAME:          screenGetSwamp
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the swamp object
*
*ARGUMENTS:
*
*********************************************************/
swamp *screenGetSwamp();


/*********************************************************
*NAME:          screenGetTankExplosions
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the tank explosions object
*
*ARGUMENTS:
*
*********************************************************/
tkExplosion *screenGetTankExplosions();

/*********************************************************
*NAME:          screenGetNetPnb
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the Pnb Object
*
*ARGUMENTS:
*
*********************************************************/
netPnbContext *screenGetNetPnb();

/*********************************************************
*NAME:          screenGetNetMnt
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the Pnb Object
*
*ARGUMENTS:
*
*********************************************************/
netMntContext *screenGetNetMnt();

/*********************************************************
*NAME:          screenSetInStartFind
*AUTHOR:        John Morrison
*CREATION DATE: 28/11/99
*LAST MODIFIED: 28/11/99
*PURPOSE:
*  Sets if we are entering of leaving the find a start
*  position
*
*ARGUMENTS:
*  entering - TRUE if entering, FALSE if leaving
*********************************************************/
void screenSetInStartFind(bool entering);


/*********************************************************
*NAME:          screenGetInStartFind
*AUTHOR:        John Morrison
*CREATION DATE: 28/11/99
*LAST MODIFIED: 28/11/99
*PURPOSE:
*  Gets if we are entering of leaving the find a start
*  position
*
*ARGUMENTS:
*
*********************************************************/
bool screenGetInStartFind();

bool backendGetContext();

#endif /* _BACKEND_H */
