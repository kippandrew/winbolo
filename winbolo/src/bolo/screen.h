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
*Name:          Screen
*Filename:      screen.h 
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 17/12/03
*Purpose:
*  Provides Interfaces with the front end
*********************************************************/

#ifndef SCREEN_H
#define SCREEN_H


/* Includes */
#include "global.h"
#include "gametype.h"
#include "screenbullet.h"
#include "screentank.h"
#include "lgm.h"
#include "brain.h"
#include "players.h"
#include "screenlgm.h"
#include "building.h"
#include "explosions.h"
#include "floodfill.h"
#include "grass.h"
#include "mines.h"
#include "minesexp.h"
#include "rubble.h"
#include "swamp.h"
#include "tankexp.h"
#include "netpnb.h"
#include "netmt.h"

/* Button Pressed - These are the valid items 
   that should be passed to this module*/

#ifndef _GAMETYPE_ENUM
#define _GAMETYPE_ENUM

typedef enum {
  gameOpen = 1,
  gameTournament,
  gameStrictTournament
} gameType;

#endif


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

/* Prototypes */

/*********************************************************
*NAME:          screenUpdateView
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED:  4/11/98
*PURPOSE:
*  Updates the values in the view area
*
*ARGUMENTS:
* value - The update type (Helps in optimisations)
*********************************************************/
void screenUpdateView(updateType value);

/*********************************************************
*NAME:          screenCalcSquare
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Calculates the terrain type for a given location
*
*ARGUMENTS:
*  xValue - The x co-ordinate
*  yValue - The y co-ordinate
*********************************************************/
BYTE screenCalcSquare(BYTE xValue, BYTE yValue, BYTE scrX, BYTE scrY);

/*********************************************************
*NAME:          screenReCalc
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 30/12/98
*PURPOSE:
*  Recalculates the screen data
*
*ARGUMENTS:
*
*********************************************************/
void screenReCalc(void);

/*********************************************************
*NAME:          screenCenterTank
*AUTHOR:        John Morrison
*CREATION DATE: 3/1/99
*LAST MODIFIED: 3/1/99
*PURPOSE:
*  Centers the screen around the tank.
*
*ARGUMENTS:
*
*********************************************************/
void screenCenterTank();

/*********************************************************
*NAME:          screenGetTankMapCoord
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
* Places the tank map co-ordinates into the parmeters
* passed
*
*ARGUMENTS:
*  mx - Pointer to hold Map X Co-ordinates
*  my - Pointer to hold Map Y Co-ordinates
*********************************************************/
void screenGetTankMapCoord(BYTE *mx, BYTE *my);

/*********************************************************
*NAME:          screenGetRandStart
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
* Places a random start position into the parmeters passed
*
*ARGUMENTS:
*  mx - Pointer to hold Map X Co-ordinates
*  my - Pointer to hold Map Y Co-ordinates
*  dir - Direction facing
*********************************************************/
void screenGetRandStart(BYTE *mx, BYTE *my, TURNTYPE *dir);

/*********************************************************
*NAME:          screenGetTankWorld
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
* Gets the tank world co-ordinates
*
*ARGUMENTS:
*  x - Pointer to hold World X Co-ordinates
*  y - Pointer to hold World Y Co-ordinates
*********************************************************/
void screenGetTankWorld(WORLD *x, WORLD *y);

/*********************************************************
*NAME:          screenCheckTankMineDamage
*AUTHOR:        John Morrison
*CREATION DATE: 21/1/99
*LAST MODIFIED: 21/1/99
*PURPOSE:
* Check for damage to a tank by a mine being set off.
*
*ARGUMENTS:
*  mx - X Map Co-ordinate
*  my - Y Map Co-ordinate
*********************************************************/
void screenCheckTankMineDamage(BYTE mx, BYTE my);

/*********************************************************
*NAME:          screenIsItemInTrees
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED:  6/1/00
*PURPOSE:
* Returns whether a item is surrounded by trees.
*
*ARGUMENTS:
*  bmx - X position
*  bmy - Y position
*********************************************************/
bool screenIsItemInTrees(WORLD bmx, WORLD bmy);

/*********************************************************
*NAME:          screenGetNumNeutralPills
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Returns the number of neutral pills
*
*ARGUMENTS:
*
*********************************************************/
BYTE screenGetNumNeutralPills(void);

/*********************************************************
*NAME:          screenGetNumNeutralBases
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Returns the number of neutral bases
*
*ARGUMENTS:
*
*********************************************************/
BYTE screenGetNumNeutralBases(void);

/*********************************************************
*NAME:          screenGetTimeGameCreated
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Returns the time the game was created
*
*ARGUMENTS:
*
*********************************************************/
long screenGetTimeGameCreated(void);

/*********************************************************
*NAME:          screenSetTimeGameCreated
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Sets the time the game was created
*
*ARGUMENTS:
*  value - Time to set it to
*********************************************************/
void screenSetTimeGameCreated(long value);

/*********************************************************
*NAME:          screenNetMapItem
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED: 23/2/99
*PURPOSE:
* Sets the time the game was created
*
*ARGUMENTS:
*  mx      - X position of the data item
*  my      - Y position of the data item
*  terrain - Terrain the data item
*********************************************************/
void screenNetMapItem(BYTE mx, BYTE my, BYTE terrain);

/*********************************************************
*NAME:          screenNetToken
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED: 23/2/99
*PURPOSE:
* The token has arrived. Make all the changes to the map 
* from the mapNet structure
*
*ARGUMENTS:
*
*********************************************************/
void screenNetToken();

/*********************************************************
*NAME:          screenSetMapName
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 26/2/99
*PURPOSE:
* Sets the map name to the parameter passed.
* (Came from network module)
*
*ARGUMENTS:
*  name - Map name
*********************************************************/
void screenSetMapName(char *name);

/*********************************************************
*NAME:          screenSetTimeLengths
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 26/2/99
*PURPOSE:
* Sets the time lengths. (Came from network module)
*
*ARGUMENTS:
*  srtDelay    - Game start delay (50th second increments)
*  gmeLen      - Length of the game (in 50ths) 
*                (-1 =unlimited)
*********************************************************/
void screenSetTimeLengths(int srtDelay, long gmeLen);

/*********************************************************
*NAME:          screenSetAllowHiddenMines
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 26/2/99
*PURPOSE:
* Sets whether hidden mines are allowed or not 
*
*ARGUMENTS:
*  hidden - TRUE if hidden mines are allowed
*********************************************************/
void screenSetAllowHiddenMines(bool hidden);

/*********************************************************
*NAME:          screenSetGameType
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 26/2/99
*PURPOSE:
* Sets the game type. (Came from network module)
*
*ARGUMENTS:
*  gt - The game type to set to
*********************************************************/
void screenSetGameType(gameType gt);

/*********************************************************
*NAME:          screenNetSetupTank
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 13/12/99
*PURPOSE:
*  Sets up the tank so it exists while we are download map
*
*ARGUMENTS:
* isInStart - Are we in In start mode?
*********************************************************/
void screenNetSetupTank(bool isInStart);

/*********************************************************
*NAME:          screenSetBaseNetData
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 27/2/99
*PURPOSE:
*  We now have the bases data. Set it here
*
*ARGUMENTS:
*  buff - Data buffer containing base data
*  len  - Length of the data
*********************************************************/
void screenSetBaseNetData(BYTE *buff, int length);

/*********************************************************
*NAME:          screenSetPillNetData
*AUTHOR:        John Morrison
*CREATION DATE: 27/02/99
*LAST MODIFIED: 24/07/04
*PURPOSE:
*  We now have the pillbox data. Set it here
*
*ARGUMENTS:
*  buff    - Data buffer containing pill data
*  dataLen - Length of the data
*********************************************************/
void screenSetPillNetData(BYTE *buff, BYTE dataLen);

/*********************************************************
*NAME:          screenSetStartsNetData
*AUTHOR:        John Morrison
*CREATION DATE: 27/02/99
*LAST MODIFIED: 24/07/04
*PURPOSE:
*  We now have the starts data. Set it here
*
*ARGUMENTS:
*  buff    - Data buffer containing starts data
*  dataLen - Length of the data
*********************************************************/
void screenSetStartsNetData(BYTE *buff, BYTE dataLen);

/*********************************************************
*NAME:          screenMakePosInfo
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED:  8/9/00
*PURPOSE:
*  Makes the player position maditory info. 
*  Tank position and lgm position. Returns length
*
*ARGUMENTS:
*  buff - Buffer to hold data
*********************************************************/
BYTE screenMakePosInfo(BYTE *buff);

/*********************************************************
*NAME:          screenMakeMapTokenData
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 27/2/99
*PURPOSE:
* Makes the map token data. Returns the length of the
* data
*
*ARGUMENTS:
*  buff - Buffer to hold data
*********************************************************/
BYTE screenMakeMapTokenData(BYTE *buff);

/*********************************************************
*NAME:          screenSetMapNetRun
*AUTHOR:        John Morrison
*CREATION DATE: 28/2/99
*LAST MODIFIED:  7/1/00
*PURPOSE:
* Sets the map net run data
*
*ARGUMENTS:
*  buff    - Buffer that holds net run data
*  yPos    - Y position of the run
*  dataLen - Length of the data
*********************************************************/
void screenSetMapNetRun(BYTE *buff, BYTE yPos, int dataLen);

/*********************************************************
*NAME:          screenExtractPlayerData
*AUTHOR:        John Morrison
*CREATION DATE: 28/2/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
*  Extracts player position data from the packet
*
*ARGUMENTS:
*  buff    - Packet data
*  buffLen - Length of the buffer 
*********************************************************/
void screenExtractPlayerData(BYTE *buff, int buffLen);

/*********************************************************
*NAME:          screenExtractMapData
*AUTHOR:        John Morrison
*CREATION DATE: 28/2/99
*LAST MODIFIED: 28/2/99
*PURPOSE:
*  Extracts map data from the packet
*
*ARGUMENTS:
*  buff - Packet data
*  len  - Length of the data
*  yPos - Position data must be less then to use
*********************************************************/
void screenExtractMapData(BYTE *buff, BYTE len, BYTE yPos);

/*********************************************************
*NAME:          screenMakeShellData
*AUTHOR:        John Morrison
*CREATION DATE: 6/3/99
*LAST MODIFIED: 6/3/99
*PURPOSE:
*  Makes the shells token data. Returns the length of the
*  data
*
*ARGUMENTS:
*  buff - Pointer to hold Packet data
*********************************************************/
BYTE screenMakeShellData(BYTE *buff);

/*********************************************************
*NAME:          screenExtractShellData
*AUTHOR:        John Morrison
*CREATION DATE:  6/3/99
*LAST MODIFIED: 18/3/99
*PURPOSE:
*  Extracts shell data from a network packet here
*
*ARGUMENTS:
*  buff   - Pointer that holds Packet data
*  datLen - Length of the packet
*********************************************************/
void screenExtractShellData(BYTE *buff, BYTE dataLen);

/*********************************************************
*NAME:          screenExtractPNBData
*AUTHOR:        John Morrison
*CREATION DATE: 10/3/99
*LAST MODIFIED:  8/1/00
*PURPOSE:
*  Extracts Pills and base data from a network packet
*
*ARGUMENTS:
*  buff   - Pointer that holds Packet data
*  datLen - Length of the packet
*  isTcp  - Is this TCP data
*********************************************************/
bool screenExtractPNBData(BYTE *buff, BYTE dataLen, bool isTcp);

/*********************************************************
*NAME:          screenExtractMNTData
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
*  Extracts netMNT data from a network packet
*
*ARGUMENTS:
*  buff   - Pointer that holds Packet data
*  datLen - Length of the packet
*  isTcp  - Is this TCP data
*********************************************************/
bool screenExtractMNTData(BYTE *buff, BYTE dataLen, bool isTcp);

/*********************************************************
*NAME:          screenMakeTKData
*AUTHOR:        John Morrison
*CREATION DATE: 11/3/99
*LAST MODIFIED: 11/3/99
*PURPOSE:
*  Makes tk explosion data.
*
*ARGUMENTS:
*  buff   - Pointer that holds Packet data
*********************************************************/
BYTE screenMakeTKData(BYTE *buff);

/*********************************************************
*NAME:          screenExtractTKData
*AUTHOR:        John Morrison
*CREATION DATE: 11/3/99
*LAST MODIFIED: 11/3/99
*PURPOSE:
*  Extracts tk explosion data from a network packet
*
*ARGUMENTS:
*  buff   - Pointer that holds Packet data
*  datLen - Length of the packet
*********************************************************/
void screenExtractTKData(BYTE *buff, BYTE dataLen);

void screenIncomingMessage(BYTE playerNum, char *messageStr);

/*********************************************************
*NAME:          screenMapIsMine
*AUTHOR:        John Morrison
*CREATION DATE: 31/11/99
*LAST MODIFIED: 31/11/99
*PURPOSE:
* Returns whether a mines exists at a map square
*
*ARGUMENTS:
*  bmx - Map X position
*  bmy - Map X position
*********************************************************/
bool screenMapIsMine(BYTE bmx, BYTE bmy);

/*********************************************************
*NAME:          screenGetTankFromPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/00
*LAST MODIFIED: 21/10/00
*PURPOSE:
* Returns the tank for a specific player num. In a client
* it always return our own tank
*
*ARGUMENTS:
*  playerNum - Tank to get for this player number
*********************************************************/
tank *screenGetTankFromPlayer(BYTE playerNum);

/*********************************************************
*NAME:          screenGetTankPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Returns this tanks player number
*
*ARGUMENTS:
*  value - Pointer to the tank
*********************************************************/
BYTE screenGetTankPlayer(tank *value);

/*********************************************************
*NAME:          screenChangeOwnership
*AUTHOR:        John Morrison
*CREATION DATE: 2/11/99
*LAST MODIFIED: 2/11/99
*PURPOSE:
* Called because we have lost our connection to the server
* Set all of our allies stuff to be owned by us.
*
*ARGUMENTS:
*  oldOwner - The old Owner of the stuff
*********************************************************/
void screenChangeOwnership(BYTE oldOwner);

/*********************************************************
*NAME:          screenMoveViewOffsetLeft
*AUTHOR:        John Morrison
*CREATION DATE: 2/11/99
*LAST MODIFIED: 2/11/99
*PURPOSE:
* Moves our view left or right depending on the argument
*
*ARGUMENTS:
*  isLeft - TRUE for left, FALSE for right
*********************************************************/
void screenMoveViewOffsetLeft(bool isLeft);

/*********************************************************
*NAME:          screenMoveViewOffsetUp
*AUTHOR:        John Morrison
*CREATION DATE: 2/11/99
*LAST MODIFIED: 2/11/99
*PURPOSE:
* Moves our view up or down depending on the argument
*
*ARGUMENTS:
*  isup - TRUE for up, FALSE for dpwm
*********************************************************/
void screenMoveViewOffsetUp(bool isUp);

/*********************************************************
*NAME:          screenTankIsDead
*AUTHOR:        John Morrison
*CREATION DATE: 2/11/99
*LAST MODIFIED: 2/11/99
*PURPOSE:
* Returns if our tank is dead or not
*
*ARGUMENTS:
*
*********************************************************/
bool screenTankIsDead();

/*********************************************************
*NAME:          screenGetSubMapSquareOffset
*AUTHOR:        John Morrison
*CREATION DATE: 18/11/99
*LAST MODIFIED: 18/11/99
*PURPOSE:
*  Gets the map sqaure sub offsets.
*
*ARGUMENTS:
*  xPos - Pointer to hold X maps square offset
*  yPos - Pointer to hold Y maps square offset
*********************************************************/
void screenGetSubMapSquareOffset(int *xPos, int *yPos);

/*********************************************************
*NAME:          screenTankScroll
*AUTHOR:        John Morrison
*CREATION DATE: 21/11/99
*LAST MODIFIED: 10/06/01
*PURPOSE:
*  Does an scroll update check/move for the tank. Returns
*  if a move occurs
*
*ARGUMENTS:
*
*********************************************************/
bool screenTankScroll();

/*********************************************************
*NAME:          screenSmoothScrollUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 18/11/99
*LAST MODIFIED: 18/11/99
*PURPOSE:
*  Does an scroll update check for smooth scrolling
*
*ARGUMENTS:
*
*********************************************************/
void screenSmoothScrollUpdate();

/*********************************************************
*NAME:          screenMakeBrainViewData
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Makes the view information including base and pills 
*  for the brain.
*
*ARGUMENTS:
*  buff      - Pointer to the buffer to hold the data
*  leftPos   - left position on the map to get data from
*  rightPos  - Right position on the map to get data from
*  topPos    - top position on the map to get data from
*  bottomPos - bottom position on the map to get data from
*********************************************************/
void screenMakeBrainViewData(BYTE *buff, BYTE leftPos, BYTE rightPos, BYTE topPos, BYTE bottomPos);

/*********************************************************
*NAME:          screenTranslateBrainButtons
*AUTHOR:        John Morrison
*CREATION DATE: 27/11/99
*LAST MODIFIED: 27/11/99
*PURPOSE:
* Translates the brain's key pressed into our keys presses
*.
*ARGUMENTS:
* isShoot    - Pointer to hold if we are shooting
* isGameTick - TRUE if this is a game tick
*********************************************************/
tankButton screenTranslateBrainButtons(bool *isShoot, bool isGameTick);

/*********************************************************
*NAME:          screenAddBrainObject
*AUTHOR:        John Morrison
*CREATION DATE: 28/11/99
*LAST MODIFIED: 28/11/99
*PURPOSE:
*  Adds a brain object to the list of brain objects
*
*ARGUMENTS:
*  object - The type of the object
*  wx     - X position of the object
*  wy     - Y position of the object
*  idNum  - Objects identifier number
*  dir    - Direction of the object
*  info   - Object info
*********************************************************/
void screenAddBrainObject(unsigned short object, WORLD wx, WORLD wy, unsigned short idNum, BYTE dir, BYTE info);

/*********************************************************
*NAME:          clientSetInStartFind
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
void clientSetInStartFind(bool entering);

/*********************************************************
*NAME:          screenNetStatusMessage
*AUTHOR:        John Morrison
*CREATION DATE: 1/6/00
*LAST MODIFIED: 1/6/00
*PURPOSE:
*  A network status message has arrived from the server
*
*ARGUMENTS:
*  messageStr - The message text
*********************************************************/
void screenNetStatusMessage(char *messageStr);

/*********************************************************
*NAME:          screenTankStopCarryingPill
*AUTHOR:        John Morrison
*CREATION DATE: 21/6/00
*LAST MODIFIED: 21/6/00
*PURPOSE:
* Someone else has picked up a pill. We should check that
* we aren't carrying it ourselves and if so drop it (The
* server said so) because this can lead to desync problems
*
*ARGUMENTS:
*  message - The message text
*********************************************************/
void screenTankStopCarryingPill(BYTE itemNum);

/*********************************************************
*NAME:          screenGetLgmFromPlayerNum
*AUTHOR:        John Morrison
*CREATION DATE: 23/9/00
*LAST MODIFIED: 23/9/00
*PURPOSE:
* Returns the lgm structure for a particular player number.
* Always your own for a client.
*
*ARGUMENTS:
*  playerNum - the player numbers lgm to get
*********************************************************/
lgm *screenGetLgmFromPlayerNum(BYTE playerNum);

/*********************************************************
*NAME:          screenNetLgmReturn
*AUTHOR:        John Morrison
*CREATION DATE: 2/12/00
*LAST MODIFIED: 2/12/00
*PURPOSE:
*  Network message. LGM Back in tank
*
*ARGUMENTS:
*  numTrees - Amount of trees lgm is carrying
*  numMines - Amount of mines the lgm is carrying
*  pillNum  - Pillbox being carries
*********************************************************/
void screenNetLgmReturn(BYTE numTrees, BYTE numMines, BYTE pillNum);

/*********************************************************
*NAME:          lgmNetManWorking
*AUTHOR:        John Morrison
*CREATION DATE: 01/02/03
*LAST MODIFIED: 01/02/03
*PURPOSE:
*  Network message. LGM Back in tank
*
*ARGUMENTS:
*  mapX    - Bless X map position 
*  mapY    - Bless Y map position 
*  numMines - The number of mines
*  pillNum  - Pillbox being carries
*  numTrees - The number of trees
*********************************************************/
void screenNetManWorking(BYTE mapX, BYTE mapY, BYTE numMines, BYTE pillNum, BYTE numTrees);

/*********************************************************
*NAME:          screenGetTankWorldFromLgm
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/00
*LAST MODIFIED: 28/12/00
*PURPOSE:
* Gets the tank world co-ordinates from a lgman object
* returns own tank co-ordinates in a client game
*
*ARGUMENTS:
*  lgmans - Pointer to the lgm structure
*  x      - Pointer to hold World X Co-ordinates
*  y      - Pointer to hold World Y Co-ordinates
*********************************************************/
void screenGetTankWorldFromLgm(lgm *lgmans, WORLD *x, WORLD *y);

/*********************************************************
*NAME:          screenSetTankStartPosition
*AUTHOR:        John Morrison
*CREATION DATE: 3/10/00
*LAST MODIFIED: 2/04/01
*PURPOSE:
* Start position recieved
*
*ARGUMENTS:
*  xValue    - X Value
*  yValue    - Y Value
*  angel     - Angel the tank is facing
*  numShells - Number of shells
*  numMines  - Number of mines
*********************************************************/
void screenSetTankStartPosition(BYTE xValue, BYTE yValue, TURNTYPE angle, BYTE numShells, BYTE numMines);

/*********************************************************
*NAME:          clientGetInStartFind
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
bool clientGetInStartFind();

/*********************************************************
*NAME:          screenCheckTankRange
*AUTHOR:        John Morrison
*CREATION DATE: 9/04/01
*LAST MODIFIED: 9/04/01
*PURPOSE:
* Checks to see if there is enemy tank within range
* supplied
*
*ARGUMENTS:
*  xValue    - X Value
*  yValue    - Y Value
*  playerNum - Player number to check against
*  distance  - Distance to be less then to return true
*********************************************************/
bool screenCheckTankRange(BYTE x, BYTE y, BYTE playerNum, double distance);

/*********************************************************
*NAME:          screenCheckPillsRange
*AUTHOR:        John Morrison
*CREATION DATE: 9/04/01
*LAST MODIFIED: 9/04/01
*PURPOSE:
* Checks to see if there is enemy pill within range
* supplied
*
*ARGUMENTS:
*  xValue    - X Value
*  yValue    - Y Value
*  playerNum - Player number to check against
*  distance  - Distance to be less then to return true
*********************************************************/
bool screenCheckPillsRange(BYTE xValue, BYTE yValue, BYTE playerNum, double distance);

/*********************************************************
*NAME:          screenGetGameRunning
*AUTHOR:        John Morrison
*CREATION DATE: 20/02/03
*LAST MODIFIED: 20/02/03
*PURPOSE:
* Called when we have lost our connection to the server 
*
*ARGUMENTS:
*
*********************************************************/
void screenConnectionLost();


/*********************************************************
*NAME:          clientTankInView
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED:  8/9/00
*PURPOSE:
* Returns TRUE in a client.
*
*ARGUMENTS:
* playerNum - PlayerNum to check
* checkX    - X Position to check
* checkY    - Y Position to check
*********************************************************/
bool clientTankInView(BYTE playerNum, BYTE checkX, BYTE checkY) ;

/*********************************************************
*NAME:          clientTankInView
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED:  8/9/00
*PURPOSE:
* Returns in playerNum is in view from position X,Y. 
* In a client returns TRUE.
*
*ARGUMENTS:
* playerNum - PlayerNum to check
* checkX    - X Position to check
* checkY    - Y Position to check
*********************************************************/
bool screenTankInView(BYTE playerNum, BYTE checkX, BYTE checkY);

/*********************************************************
*NAME:          clientGetPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 26/02/03
*LAST MODIFIED: 01/03/03
*PURPOSE:
* Returns the players object
*
*ARGUMENTS:
*
*********************************************************/
players *clientGetPlayers();

/*********************************************************
*NAME:          screenGetPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 19/01/99
*LAST MODIFIED: 05/05/01
*PURPOSE:
* Returns the players object
*
*ARGUMENTS:
*
*********************************************************/
players *screenGetPlayers();

/*********************************************************
*NAME:          clientGetBuildings
*AUTHOR:        John Morrison
*CREATION DATE: 19/01/99
*LAST MODIFIED: 05/05/01
*PURPOSE:
*  Returns a pointer to the buildings Object
*
*ARGUMENTS:
*
*********************************************************/
building *clientGetBuildings();

/*********************************************************
*NAME:          clientGetExplosions
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the explosions Object
*
*ARGUMENTS:
*
*********************************************************/
explosions *clientGetExplosions();

/*********************************************************
*NAME:          clientGetFloodFill
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the floodfill object
*
*ARGUMENTS:
*
*********************************************************/
floodFill *clientGetFloodFill();

/*********************************************************
*NAME:          clientGetGrass
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the grass object
*
*ARGUMENTS:
*
*********************************************************/
grass *clientGetGrass();

/*********************************************************
*NAME:          clientGetMines
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the mines object
*
*ARGUMENTS:
*
*********************************************************/
mines *clientGetMines();

/*********************************************************
*NAME:          clientGetMinesExp
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the mines explosion object
*
*ARGUMENTS:
*
*********************************************************/
minesExp *clientGetMinesExp();

/*********************************************************
*NAME:          clientGetRubble
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the rubble object
*
*ARGUMENTS:
*
*********************************************************/
rubble *clientGetRubble();

/*********************************************************
*NAME:          clientGetSwamp
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the swamp object
*
*ARGUMENTS:
*
*********************************************************/
swamp *clientGetSwamp();

/*********************************************************
*NAME:          clientGetTankExplosions
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the tank explosions object
*
*ARGUMENTS:
*
*********************************************************/
tkExplosion  *clientGetTankExplosions();

/*********************************************************
*NAME:          clientGetNetPnb
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the Pnb Object
*
*ARGUMENTS:
*
*********************************************************/
netPnbContext *clientGetNetPnb();

/*********************************************************
*NAME:          clientGetNetPnb
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the Pnb Object
*
*ARGUMENTS:
*
*********************************************************/
netMntContext *clientGetNetMnt();

/*********************************************************
*NAME:          clientGetGameType
*AUTHOR:        John Morrison
*CREATION DATE: 17/12/03
*LAST MODIFIED: 17/12/03
*PURPOSE:
*  Returns a pointer to the game type
*
*ARGUMENTS:
*
*********************************************************/
gameType *clientGetGameType();

/*********************************************************
*NAME:          clientBasesMigrate
*AUTHOR:        Minhiriath
*CREATION DATE: 15/03/2009
*LAST MODIFIED: 15/03/2009
*PURPOSE:
*  Migrates bases
*
*ARGUMENTS:
* playerNumOldOwner - old owner
* playerNumNewOwner - new owner
*********************************************************/
void clientBasesMigrate(BYTE playerNumOldOwner, BYTE playerNumNewOwner);

/*********************************************************
*NAME:          screenBasesMigrate
*AUTHOR:        Minhiriath
*CREATION DATE: 15/03/2009
*LAST MODIFIED: 15/03/2009
*PURPOSE:
*  Migrates bases
*
*ARGUMENTS:
* playerNumOldOwner - old owner
* playerNumNewOwner - new owner
*********************************************************/
void screenBasesMigrate(BYTE playerNumOldOwner, BYTE playerNumNewOwner);

/*********************************************************
*NAME:          clientPillsMigratePlanted
*AUTHOR:        Minhiriath
*CREATION DATE: 15/03/2009
*LAST MODIFIED: 15/03/2009
*PURPOSE:
*  Migrates planted pills
*
*ARGUMENTS:
* playerNumOldOwner - old owner
* playerNumNewOwner - new owner
*********************************************************/
void clientPillsMigratePlanted(BYTE playerNumOldOwner, BYTE playerNumNewOwner);

/*********************************************************
*NAME:          screenPillsMigratePlanted
*AUTHOR:        Minhiriath
*CREATION DATE: 15/03/2009
*LAST MODIFIED: 15/03/2009
*PURPOSE:
*  Migrates planted pills
*
*ARGUMENTS:
* playerNumOldOwner - old owner
* playerNumNewOwner - new owner
*********************************************************/
void screenPillsMigratePlanted(BYTE playerNumOldOwner, BYTE playerNumNewOwner);

void screenSendMem();

#endif /* SCREEN_H */
