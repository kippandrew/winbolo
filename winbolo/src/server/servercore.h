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
*Name:          server Core
*Filename:      serverCore.h
*Author:        John Morrison
*Creation Date: 10/08/99
*Last Modified: 27/07/04
*Purpose:
*  WinBolo Server Core Simulation modelling.
*********************************************************/

#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include "../bolo/global.h"
#include "../bolo/gametype.h"
#include "../bolo/screen.h"
#include "../bolo/lgm.h"
#include "../bolo/building.h"
#include "../bolo/explosions.h"
#include "../bolo/floodfill.h"
#include "../bolo/grass.h"
#include "../bolo/mines.h"
#include "../bolo/minesexp.h"
#include "../bolo/rubble.h"
#include "../bolo/swamp.h"
#include "../bolo/tankexp.h"
#include "../bolo/netpnb.h"
#include "../bolo/netmt.h"
#include "../bolo/gametype.h"

/*********************************************************
*NAME:          serverCoreCreate
*AUTHOR:        John Morrison
*CREATION DATE: 10/8/99
*LAST MODIFIED:  5/5/00
*PURPOSE:
*  Creates a new server. Returns FALSE if an error occured
*  such as error loading map
*
*ARGUMENTS:
*  fileName - Points to the map file
*  game - The game type-Open/tournament/strict tournament
*  hiddenMines - Are hidden mines allowed
*  srtDelay    - Game start delay (50th second increments)
*  gmeLen      - Length of the game (in 50ths) 
*                (-1 =unlimited)
*********************************************************/
bool serverCoreCreate(char *fileName, gameType game, bool hiddenMines, int srtDelay, long gmeLen);

/*********************************************************
*NAME:          serverCoreCreateCompressed
*AUTHOR:        John Morrison
*CREATION DATE: 10/8/99
*LAST MODIFIED: 5/05/01
*PURPOSE:
*  Creates a new server via loading a compressed map
*  structure. Returns FALSE if an error occured such as 
*  error loading map
*
*ARGUMENTS:
*  buff        - Compressed data buffer containing map
*  buffLen     - Length of the buffer
*  mapn        - Name of the map 
*  game        - The game type-Open/tournament/strict 
*                tournament
*  hiddenMines - Are hidden mines allowed
*  srtDelay    - Game start delay (50th second increments)
*  gmeLen      - Length of the game (in 50ths) 
*                (-1 =unlimited)
*********************************************************/
bool serverCoreCreateCompressed(BYTE *buff, int buffLen, char *mapn, gameType game, bool hiddenMines, int srtDelay, long gmeLen);

/*********************************************************
*NAME:          serverDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 10/8/99
*LAST MODIFIED: 22/6/00
*PURPOSE:
*  Shuts down the server.
*
*ARGUMENTS:
*
*********************************************************/
void serverCoreDestroy();

/*********************************************************
*NAME:          serverCoreGameTick
*AUTHOR:        John Morrison
*CREATION DATE: 10/8/99
*LAST MODIFIED: 3/10/00
*PURPOSE:
*  Called once every game tick to update the game world
*
*ARGUMENTS:
*  fileName - Points to the map file
*********************************************************/
void serverCoreGameTick();

/*********************************************************
*NAME:          serverCoreGetTimeGameCreated
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99*PURPOSE:
* Returns the time the game was created
*
*ARGUMENTS:
*
*********************************************************/
long serverCoreGetTimeGameCreated(void);

/*********************************************************
*NAME:          serverCoreSetTimeGameCreated
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Sets the time the game was created
*
*ARGUMENTS:
*  value - Time to set it to
*********************************************************/
void serverCoreSetTimeGameCreated(long value);

/*********************************************************
*NAME:          serverCoreGetMapName
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* The front end wants to know what the map name is.
* Make a copy for it.
*
*ARGUMENTS:
*  value - Place to hold copy of the map name
*********************************************************/
void serverCoreGetMapName(char *value);

/*********************************************************
*NAME:          serverCoreGetAllowHiddenMines
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Returns whether hidden mines are allowed in the game
* or not
*
*ARGUMENTS:
*
*********************************************************/
bool serverCoreGetAllowHiddenMines();

/*********************************************************
*NAME:          serverCoreGetGameStartDelay
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Returns the time remaining to start the current game
*
*ARGUMENTS:
*
*********************************************************/
long serverCoreGetGameStartDelay();

/*********************************************************
*NAME:          serverCoreGetGameTimeLeft
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Returns the time remaining the current game
*
*ARGUMENTS:
*
*********************************************************/
long serverCoreGetGameTimeLeft();

/*********************************************************
*NAME:          serverCoreGetNumPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Returns the number of players in the game
*
*ARGUMENTS:
*
*********************************************************/
BYTE serverCoreGetNumPlayers();

/*********************************************************
*NAME:          serverCoreGetNumNeutralPills
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Returns the number of neutral pills
*
*ARGUMENTS:
*
*********************************************************/
BYTE serverCoreGetNumNeutralPills(void);

/*********************************************************
*NAME:          serverCoreGetNumNeutralBases
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Returns the number of neutral bases
*
*ARGUMENTS:
*
*********************************************************/
BYTE serverCoreGetNumNeutralBases(void);

/*********************************************************
*NAME:          serverCoreGetBaseNetData
*AUTHOR:        John Morrison
*CREATION DATE: 29/08/99
*LAST MODIFIED: 27/07/04
*PURPOSE:
*  Request for base data has been made.
*
*ARGUMENTS:
*  buff - Buffer to hold copy of base data
*********************************************************/
BYTE serverCoreGetBaseNetData(BYTE *buff);

/*********************************************************
*NAME:          serverCoreGetPillNetData
*AUTHOR:        John Morrison
*CREATION DATE: 29/08/99
*LAST MODIFIED: 27/07/04
*PURPOSE:
*  Request for pill data has been made.
*
*ARGUMENTS:
*  buff - Buffer to hold copy of pill data
*********************************************************/
BYTE serverCoreGetPillNetData(BYTE *buff);

/*********************************************************
*NAME:          serverCoreGetStartsNetData
*AUTHOR:        John Morrison
*CREATION DATE: 29/08/99
*LAST MODIFIED: 27/07/04
*PURPOSE:
*  Request for starts data has been made.
*
*ARGUMENTS:
*  buff - Buffer to hold copy of starts data
*********************************************************/
BYTE serverCoreGetStartsNetData(BYTE *buff);

/*********************************************************
*NAME:          serverCoreMakeMapNetRun
*AUTHOR:        John Morrison
*CREATION DATE: 29/8/99
*LAST MODIFIED:  7/1/00
*PURPOSE:
* Makes the map net run data. Returns data length
*
*ARGUMENTS:
*  buff - Buffer to hold data
*  yPos - Y position of the run
*********************************************************/
int serverCoreMakeMapNetRun(BYTE *buff, BYTE yPos);

/*********************************************************
*NAME:          serverCorePlayerJoin
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* A new player has joined the game
*
*ARGUMENTS:
*  playerNum - Player number of th new player
*********************************************************/
void serverCorePlayerJoin(BYTE playerNum);

/*********************************************************
*NAME:          serverCorePlayerLeave
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* A player has left the game
*
*ARGUMENTS:
*  playerNum - Player number of th new player
*********************************************************/
void serverCorePlayerLeave(BYTE playerNum);

/*********************************************************
*NAME:          serverCoreSetPosData
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
* Process a player pos packet
*
*ARGUMENTS:
*  buff - Data Buffer
*********************************************************/
void serverCoreSetPosData(BYTE *buff);

/*********************************************************
*NAME:          serverCoreTankInView
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED:  8/9/00
*PURPOSE:
* Returns if the item at checkX, checkY is in viewing
* range of playre playerNum and any of its pillboxes
*
*ARGUMENTS:
* playerNum - PlayerNum to check
* checkX    - X Position to check
* checkY    - Y Position to check
*********************************************************/
bool serverCoreTankInView(BYTE playerNum, BYTE checkX, BYTE checkY);

/*********************************************************
*NAME:          serverCorePreparePosPackets
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED:  8/9/00
*PURPOSE:
* Prepares the player pos data in the array
*
*ARGUMENTS:
*********************************************************/
void serverCorePreparePosPackets();

/*********************************************************
*NAME:          serverCoreMakePosPackets
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 30/7/00
*PURPOSE:
* Makes a position packet for all players in the game.
* Returns packet length
*
*ARGUMENTS:
*  buff      - Data Buffer of packet
*  noPlayer  - Player not to make data for
*  sendStale - TRUE if we want to send out stale values
*              aswell
*********************************************************/
int serverCoreMakePosPackets(BYTE *buff, BYTE noPlayer, bool sendStale);

/*********************************************************
*NAME:          serverCoreExtractShellData
*AUTHOR:        John Morrison
*CREATION DATE: 30/10/99
*LAST MODIFIED: 30/10/99
*PURPOSE:
* Extracts the shell data from a network packet
*
*ARGUMENTS:
*  buff - Buffer containing the data
*  len  - Length of the buffer
*********************************************************/
void serverCoreExtractShellData(BYTE *buff, BYTE len);

/*********************************************************
*NAME:          serverCoreMakeShellData
*AUTHOR:        John Morrison
*CREATION DATE: 30/10/99
*LAST MODIFIED:  8/9/00
*PURPOSE:
* Makes the shell data for a network packet. Returns the
* packet length
*
*ARGUMENTS:
*  buff        - Buffer containing the data
*  noPlayerNum - Player not to make data for
*  sentState   - What to set the shell sent state to
*********************************************************/
BYTE serverCoreMakeShellData(BYTE *buff, BYTE noPlayerNum, bool sentState);

/*********************************************************
*NAME:          serverCoreMakeMapData
*AUTHOR:        John Morrison
*CREATION DATE: 30/10/99
*LAST MODIFIED: 30/10/99
*PURPOSE:
* Makes the map data for a network packet. Returns the
* packet length
*
*ARGUMENTS:
*  buff - Buffer containing the data
*********************************************************/
BYTE serverCoreMakeMapData(BYTE *buff);

/*********************************************************
*NAME:          serverCoreMapIsMine
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
* Returns whether a mines exists at a map square
*
*ARGUMENTS:
*  bmx - Map X position
*  bmy - Map X position
*********************************************************/
bool serverCoreMapIsMine(BYTE bmx, BYTE bmy);

/*********************************************************
*NAME:          serverCoreGetTankFromPlayer
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
tank* serverCoreGetTankFromPlayer(BYTE playerNum);

/*********************************************************
*NAME:          serverCoreGetTankPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Returns this tanks player number
*
*ARGUMENTS:
*  value - Pointer to the tank
*********************************************************/
BYTE serverCoreGetTankPlayer(tank *value);

/*********************************************************
*NAME:          severCoreExtractClientMapData
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED:  21/9/00
*PURPOSE:
* Extracts the client map data into the map file.
*
*ARGUMENTS:
*  buff      - Buffer containing the data
*  len       - Length of the buffer
*  playerNum - Player number this data came from
*********************************************************/
void severCoreExtractClientMapData(BYTE *buff, BYTE len, BYTE playerNum);

/*********************************************************
*NAME:          serverCoreExtractPNBData
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Extracts the netPNB Data
*
*ARGUMENTS:
*  buff - Buffer containing the data
*  len  - Length of the buffer
*********************************************************/
void serverCoreExtractPNBData(BYTE *buff, BYTE len);

/*********************************************************
*NAME:          serverCoreExtractTkExpData
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Extracts the tank explosion Data
*
*ARGUMENTS:
*  buff - Buffer containing the data
*  len  - Length of the buffer
*********************************************************/
void serverCoreExtractTkExpData(BYTE *buff, BYTE len);

/*********************************************************
*NAME:          serverCoreMakeTkData
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Makes the tank explosion Data
*
*ARGUMENTS:
*  buff - Buffer containing the data
*********************************************************/
BYTE serverCoreMakeTkData(BYTE *buff);

/*********************************************************
*NAME:          serverCoreExtractNMTData
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Extracts the NMT data.
*
*ARGUMENTS:
*  buff - Buffer containing the data
*  len  - Length of the buffer
********************************************************/
void serverCoreExtractNMTData(BYTE *buff, BYTE len);

/*********************************************************
*NAME:          serverCoreNumBases
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Returns the number of bases
*
*ARGUMENTS:
*
*********************************************************/
BYTE serverCoreNumBases(void);

/*********************************************************
*NAME:          serverCoreNumPills
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Returns the number of pillboxes
*
*ARGUMENTS:
*
*********************************************************/
BYTE serverCoreNumPills(void);

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

/*********************************************************
*NAME:          serverCoreSetQuietMode
*AUTHOR:        John Morrison
*CREATION DATE: 8/7/00
*LAST MODIFIED: 8/7/00
*PURPOSE:
* Sets whether the server is in quiet mode or not
*
*ARGUMENTS:
*  mode - Server mode
********************************************************/
void serverCoreSetQuietMode(bool mode);

void serverMessageConsoleMessage(char *msg);

/*********************************************************
*NAME:          screenCoreServerConsoleMessage
*AUTHOR:        John Morrison
*CREATION DATE: 8/7/00
*LAST MODIFIED: 8/7/00
*PURPOSE:
* Displays a server message on the console
*
*ARGUMENTS:
*  mode - Server mode
********************************************************/
void screenServerConsoleMessage(char *msg);


/*********************************************************
*NAME:          serverCoreServerConsoleMessage
*AUTHOR:        John Morrison
*CREATION DATE: 8/7/00
*LAST MODIFIED: 8/7/00
*PURPOSE:
* Displays a server message on the console
*
*ARGUMENTS:
*  mode - Server mode
********************************************************/
void serverCoreServerConsoleMessage(char *msg);


bool serverCoreRunning();

/*********************************************************
*NAME:          serverCoreGetLgmFromPlayerNum
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
lgm *serverCoreGetLgmFromPlayerNum(BYTE playerNum);

/*********************************************************
*NAME:          serverCoreLgmOperation
*AUTHOR:        John Morrison
*CREATION DATE: 3/10/00
*LAST MODIFIED: 3/10/00
*PURPOSE:
* A client has requested a lgm operataion
* Always your own for a client.
*
*ARGUMENTS:
*  playerNum - the player numbers lgm to get
*  destX     - X Destination
*  destY     - Y Destination
*  operation - Operation to perform
*********************************************************/
void serverCoreLgmOperation(BYTE playerNum, BYTE destX, BYTE destY, BYTE operation);

/*********************************************************
*NAME:          serverCoreGetTankWorldFromLgm
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
void serverCoreGetTankWorldFromLgm(lgm *lgmans, WORLD *x, WORLD *y);

/*********************************************************
*NAME:          serverCoreGetRandStart
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
void serverCoreGetRandStart(BYTE *mx, BYTE *my, TURNTYPE *dir);

/*********************************************************
*NAME:          serverCoreGetStartPosition
*AUTHOR:        John Morrison
*CREATION DATE: 3/10/01
*LAST MODIFIED: 8/04/01
*PURPOSE:
* Client has request a start position.
*
*ARGUMENTS:
*  playerNum - Player Number to get
*  xValue    - Pointer to hold X Value
*  yValue    - Pointer to hold Y Value
*  angel     - Angel to get
*  numShells - Pointer to hold number of shells
*  numMines  - Pointer to hold number of mines
*********************************************************/
void serverCoreGetStartPosition(BYTE playerNum, BYTE *xValue, BYTE *yValue, TURNTYPE *angel, BYTE *numShells, BYTE *numMines);

/*********************************************************
*NAME:          serverCoreCheckTankRange
*AUTHOR:        John Morrison
*CREATION DATE: 9/04/01
*LAST MODIFIED: 11/04/01
*PURPOSE:
* Checks to see if there is enemy tank within range
* supplied. Returns TRUE if we are in the clear
*
*ARGUMENTS:
*  xValue    - X Value
*  yValue    - Y Value
*  playerNum - Player number to check against
*  distance  - Distance to be less then to return FALSE
*********************************************************/
bool serverCoreCheckTankRange(BYTE x, BYTE y, BYTE playerNum, double distance);

/*********************************************************
*NAME:          serverCoreCheckPillsRange
*AUTHOR:        John Morrison
*CREATION DATE: 9/04/01
*LAST MODIFIED: 9/04/01
*PURPOSE:
* Checks to see if there is enemy pill within range
* supplied. Returns TRUE if we are in the clear
*
*ARGUMENTS:
*  xValue    - X Value
*  yValue    - Y Value
*  playerNum - Player number to check against
*  distance  - Distance to be less then to return true
*********************************************************/
bool serverCoreCheckPillsRange(BYTE xValue, BYTE yValue, BYTE playerNum, double distance);

/*********************************************************
*NAME:          serverCoreSendTeams
*AUTHOR:        John Morrison
*CREATION DATE: 21/02/03
*LAST MODIFIED: 21/02/03
*PURPOSE:
* Generates a list of teams and send them to winbolo.net
*
*ARGUMENTS:
*
*********************************************************/
void serverCoreSendTeams();

/*********************************************************
*NAME:          serverCoreCheckGameWin
*AUTHOR:        John Morrison
*CREATION DATE: 22/04/01
*LAST MODIFIED: 14/04/02
*PURPOSE:
* Checks to see if all the bases are owned by a specific
* alliance
*
*ARGUMENTS:
*  printWinners - If TRUE print out the list of winners
*********************************************************/
bool serverCoreCheckGameWin(bool printWinners);

/*********************************************************
*NAME:          serverCoreStartLog
*AUTHOR:        John Morrison
*CREATION DATE: 05/05/01
*LAST MODIFIED: 25/07/04
*PURPOSE:
* Starts logging game events to a log file
*
*ARGUMENTS:
*  fileName    - Filename to log to
*  ai          - Games AI type
*  maxPlayers  - Maximum number of players allowed in the
*                game
*  usePassword - Is the game password protected
*********************************************************/
bool serverCoreStartLog(char *fileName, BYTE ai, BYTE maxPlayers, bool usePassword);

/*********************************************************
*NAME:          serverCoreStopLog
*AUTHOR:        John Morrison
*CREATION DATE: 05/05/01
*LAST MODIFIED: 05/05/01
*PURPOSE:
* Stops logging
*
*ARGUMENTS:
*
*********************************************************/
void serverCoreStopLog();

/*********************************************************
*NAME:          serverCoreSetServerLogFile
*AUTHOR:        John Morrison
*CREATION DATE: 26/02/03
*LAST MODIFIED: 26/02/03
*PURPOSE:
* Sets the ServerMessages calls to write to a file instead
* of console output
*
*ARGUMENTS:
*  logFile - File to write to
*********************************************************/
void serverCoreSetServerLogFile(char *logFile);

/*********************************************************
*NAME:          serverCoreSaveMap
*AUTHOR:        John Morrison
*CREATION DATE: 26/02/03
*LAST MODIFIED: 01/03/03
*PURPOSE:
* Saves the map file to fileName and returns success
*
*ARGUMENTS:
*  fileName - File to write to
*********************************************************/
bool serverCoreSaveMap(char *fileName);

/*********************************************************
*NAME:          serverCoreGetPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 26/02/03
*LAST MODIFIED: 01/03/03
*PURPOSE:
* Returns the players object
*
*ARGUMENTS:
*
*********************************************************/
players *serverCoreGetPlayers();

/*********************************************************
*NAME:          serverCoreGetBuildings
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the buildings Object
*
*ARGUMENTS:
*
*********************************************************/
building *serverCoreGetBuildings();

/*********************************************************
*NAME:          serverCoreGetExplosions
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the explosions Object
*
*ARGUMENTS:
*
*********************************************************/
explosions *serverCoreGetExplosions();

/*********************************************************
*NAME:          serverCoreGetFloodFill
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the floodfill object
*
*ARGUMENTS:
*
*********************************************************/
floodFill *serverCoreGetFloodFill();

/*********************************************************
*NAME:          serverCoreGetGrass
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the grass object
*
*ARGUMENTS:
*
*********************************************************/
grass *serverCoreGetGrass();

/*********************************************************
*NAME:          serverCoreGetMines
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the mines object
*
*ARGUMENTS:
*
*********************************************************/
mines *serverCoreGetMines();

/*********************************************************
*NAME:          serverGetMinesExp
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the mines explosion object
*
*ARGUMENTS:
*
*********************************************************/
minesExp *serverGetMinesExp();

/*********************************************************
*NAME:          serverCoreGetRubble
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the rubble object
*
*ARGUMENTS:
*
*********************************************************/
rubble *serverCoreGetRubble();

/*********************************************************
*NAME:          serverCoreGetSwamp
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the swamp object
*
*ARGUMENTS:
*
*********************************************************/
swamp *serverCoreGetSwamp();

/*********************************************************
*NAME:          serverCoreGetTankExplosions
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the tank explosions object
*
*ARGUMENTS:
*
*********************************************************/
tkExplosion  *serverCoreGetTankExplosions();

/*********************************************************
*NAME:          serverCoreGetNetPnb
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the Pnb Object
*
*ARGUMENTS:
*
*********************************************************/
netPnbContext *serverCoreGetNetPnb();

/*********************************************************
*NAME:          serverCoreGetNetMnt
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the Pnb Object
*
*ARGUMENTS:
*
*********************************************************/
netMntContext *serverCoreGetNetMnt();

/*********************************************************
*NAME:          serverCoreInformation
*AUTHOR:        John Morrison
*CREATION DATE: 19/11/03
*LAST MODIFIED: 19/11/03
*PURPOSE:
*  Prints out the server information to stdout
*
*ARGUMENTS:
*
*********************************************************/
void serverCoreInformation();

/*********************************************************
*NAME:          serverCoreCheckAutoClose
*AUTHOR:        John Morrison
*CREATION DATE: 10/12/03
*LAST MODIFIED: 10/12/03
*PURPOSE:
* Checks if the autoclose server function has completed
* It has completed if one player has joined and then
* all players have quit the server
*
*ARGUMENTS:
*
*********************************************************/
bool serverCoreCheckAutoClose();

/*********************************************************
*NAME:          serverCoreGetGameType
*AUTHOR:        John Morrison
*CREATION DATE: 17/12/03
*LAST MODIFIED: 17/12/03
*PURPOSE:
*  Returns a pointer to the game type
*
*ARGUMENTS:
*
*********************************************************/
gameType *serverCoreGetGameType();

/*********************************************************
*NAME:          serverCoreGetActualGameType
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Returns the game type (open/tournament etc.)
*
*ARGUMENTS:
*
*********************************************************/
gameType serverCoreGetActualGameType();

/*********************************************************
*NAME:          serverCoreBasesMigrate
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
void serverCoreBasesMigrate(BYTE playerNumOldOwner, BYTE playerNumNewOwner);

/*********************************************************
*NAME:          serverCorePillsMigratePlanted
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
void serverCorePillsMigratePlanted(BYTE playerNumOldOwner, BYTE playerNumNewOwner);

#endif /* SERVER_CORE_H */
