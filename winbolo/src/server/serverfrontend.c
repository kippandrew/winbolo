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
*Name:          FrontEnd
*Filename:      frontEnd
*Author:        John Morrison
*Creation Date: 10/08/99
*Last Modified: 05/05/01
*Purpose:
*  Provides dummy functions that can be called in lui
*  of a front end.
*********************************************************/

#include <stdio.h>
#include "../bolo/global.h"
#include "../bolo/screen.h"
#include "servernet.h"
#include "../bolo/frontend.h"
#include "../bolo/messages.h"
#include "../bolo/log.h"

/*********************************************************
*NAME:          frontEndUpdateTankStatusBars
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 28/12/98
*PURPOSE:
* Function is called when the tanks status bars need to 
* be updated
*
*ARGUMENTS:
*  shells  - Number of shells
*  mines   - Number of mines
*  armour  - Amount of armour
*  trees   - Amount of trees
*********************************************************/
void frontEndUpdateTankStatusBars(BYTE shells, BYTE mines, BYTE armour, BYTE trees) {
  return;
}

/*********************************************************
*NAME:          frontEndUpdateBaseStatusBars
*AUTHOR:        John Morrison
*CREATION DATE: 11/1/99
*LAST MODIFIED: 11/1/99
*PURPOSE:
* Function is called when the tanks status bars need to 
* be updated
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  shells  - Number of shells
*  mines   - Number of mines
*  armour  - Amount of armour
*********************************************************/
void frontEndUpdateBaseStatusBars(BYTE shells, BYTE mines, BYTE armour) {
  return;
}

/*********************************************************
*NAME:          screenLgmAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/98
*LAST MODIFIED: 19/2/98
*PURPOSE:
*  Adds a data set for a specific lgm
*
*ARGUMENTS:
*  value      - Pointer to the screenBullets data structure
*  mx         - X co-ord of the map position
*  my         - Y co-ord of the map position
*  px         - X pixel offset
*  py         - Y pixel offset
*  frame      - Frame identifer of the tank
*********************************************************/
void screenLgmAddItem(screenLgm *value, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame) {
  return;
}

/*********************************************************
*NAME:          frontEndSoundEffects
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 28/12/98
*PURPOSE:
*  Plays a sound effect if it is turned on
*
*ARGUMENTS:
*  value - The sound effect to play
*********************************************************/
void frontEndPlaySound(sndEffects value) {
  return;
}

/*********************************************************
*NAME:          frontEndDrawMainScreen
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 18/11/99
*PURPOSE:
*  Updates the Window View
*
*ARGUMENTS:
*  value      - Pointer to the sceen structure
*  mineView   - Pointer to the screen mine structure
*  tks        - Pointer to the screen tank structure
*  gs         - pointer to the screen gunsight structure
*  lgms       - Pointer to the screen builder structure
*  srtDelay   - Start delay. If this is greater then 0
*               Then the delay screen should be drawn
*  isPillView - TRUE if we are in pillbox view
*  tank       - Pointer to player's tank structure
*  edgeX      - X Offset for smooth scrolling
*  edgeY      - Y Offset for smooth scrolling
*********************************************************/
void frontEndDrawMainScreen(screen *value, screenMines *mineView, screenTanks *tks, screenGunsight *gs, screenBullets *sBullet, screenLgm *lgms, long srtDelay, bool isPillView, tank *tank, int edgeX, int edgeY) {
  return;
}

/*********************************************************
*NAME:          frontEndStatusPillbox
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  3/1/99
*PURPOSE:
*  Sets the pillbox status for a particular pillbox
*
*ARGUMENTS:
*  pillNum - The base number to draw (1-16)
*  pa      - The allience of the pillbox
*********************************************************/
void frontEndStatusPillbox(BYTE pillNum, pillAlliance pb) {
  return;
}

/*********************************************************
*NAME:          frontEndStatusTank
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
*  Sets the tank status for a particular tank
*
*ARGUMENTS:
*  tankNum - The tank number to draw (1-16)
*  ts      - The allience of the tank
*********************************************************/
void frontEndStatusTank(BYTE tankNum, tankAlliance ts) {
  return;
}

/*********************************************************
*NAME:          frontEndStatusBase
*AUTHOR:        John Morrison
*CREATION DATE: 10/1/99
*LAST MODIFIED: 10/1/99
*PURPOSE:
*  Sets the base status for a particular base
*
*ARGUMENTS:
*  pillNum - The base number to draw (1-16)
*  bs      - The allience of the pillbox
*********************************************************/
void frontEndStatusBase(BYTE baseNum, baseAlliance bs) {
  return;
}

/*********************************************************
*NAME:          frontEndMessages
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  3/1/99
*PURPOSE:
*  The messages must be drawn on the screen
*
*ARGUMENTS:
*  top    - The top line to write
*  bottom - The bottom line to write
*********************************************************/
void frontEndMessages(char *top, char *bottom) {
  return;
}

/*********************************************************
*NAME:          frontEndKillsDeaths
*AUTHOR:        John Morrison
*CREATION DATE:  8/1/99
*LAST MODIFIED:  8/1/99
*PURPOSE:
*  The tank kills/deaths must be updated
*
*ARGUMENTS:
*  kills  - The number of kills the tank has.
*  deaths - The number of times the tank has died
*********************************************************/
void frontEndKillsDeaths(int kills, int deaths) {
  return;
}

/*********************************************************
*NAME:          frontEndManStatus
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  The tank kills/deaths must be updated
*
*ARGUMENTS:
*  isDead - Is the man dead
*  angle  - The angle the man is facing
*********************************************************/
void frontEndManStatus(bool isDead, TURNTYPE angle) {
  return;
}

/*********************************************************
*NAME:          frontEndManStatus
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  Clears the man status (ie man is in tank)
*
*ARGUMENTS:
*
*********************************************************/
void frontEndManClear(void) {
  return;
}

/*********************************************************
*NAME:          frontEndGameOver
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 29/1/99
*PURPOSE:
*  Time limit is up. The game is over
*
*ARGUMENTS:
*
*********************************************************/
void frontEndGameOver(void) {

  messageAdd(globalMessage, (char *) "Game Timelimit has expired. Shutting down\n", (char *) ""); /* FIXME - message localisation */
  return;
}

/*********************************************************
*NAME:          frontEndClearPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 4/2/99
*LAST MODIFIED: 4/2/99
*PURPOSE:
* Clears a player name from the menu and disables it.
* 
*ARGUMENTS:
*  value - The player number to clear
*********************************************************/
void frontEndClearPlayer(playerNumbers value) {
  return;
}

/*********************************************************
*NAME:          frontEndSetPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 4/2/99
*LAST MODIFIED: 4/2/99
*PURPOSE:
* Sets a player name in the menu and enables it.
* 
*ARGUMENTS:
*  value - The player number to set
*  str   - String identifier of the name
*********************************************************/
void frontEndSetPlayer(playerNumbers value, char *str) {
  return;
}

/*********************************************************
*NAME:          frontEndDrawDownload
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/99
*LAST MODIFIED: 13/12/99
*PURPOSE:
* A screen redraw request has been made but we are still
* downloading network data. Draw progress line instead.
* 
*ARGUMENTS:
*  justBlack - TRUE if we want just a black screen
*********************************************************/
void frontEndDrawDownload(bool justBlack) {
  return;
}

/*********************************************************
*NAME:          frontEndSetPlayerCheckState
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/99
*LAST MODIFIED: 27/3/99
*PURPOSE:
* Checks/unchecks a player
* 
*ARGUMENTS:
* value     - The player number
* isChecked - Is the item checked
*********************************************************/
void frontEndSetPlayerCheckState(playerNumbers value, bool isChecked) {
  return;
}


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
void screenReCalc(void) {
  return;
}

/*********************************************************
*NAME:          clientSoundDist
*AUTHOR:        John Morrison
*CREATION DATE: 19/01/99
*LAST MODIFIED: 05/05/01
*PURPOSE:
*  Calculates whether a soft sound of a loud sound should 
*  be played and passes paremeters to frontend
*
*ARGUMENTS:
*  value - Sound effect to be played
*  mx    - Map X co-ordinatate for the sound origin
*  my    - Map Y co-ordinatate for the sound origin
*********************************************************/
void clientSoundDist(sndEffects value, BYTE mx, BYTE my) {
  return;
}

/*********************************************************
*NAME:          serverCoreSoundDist
*AUTHOR:        John Morrison
*CREATION DATE: 19/01/99
*LAST MODIFIED: 05/05/01
*PURPOSE:
*  Calculates whether a soft sound of a loud sound should 
*  be played and passes paremeters to frontend
*
*ARGUMENTS:
*  value - Sound effect to be played
*  mx    - Map X co-ordinatate for the sound origin
*  my    - Map Y co-ordinatate for the sound origin
*********************************************************/
void serverCoreSoundDist(sndEffects value, BYTE mx, BYTE my) {
  BYTE logMessageType; /* Log item type */
  bool wantLog;        /* Do we want to log this? */
  
  wantLog = TRUE;
  switch (value) {
  case shootSelf:
  case shootNear:
  case shootFar:
    logMessageType = log_SoundShoot;
    break;

  case shotTreeNear:
  case shotTreeFar:
    logMessageType = log_SoundHitTree;
    break;

  case shotBuildingNear:
  case shotBuildingFar:
    logMessageType = log_SoundHitWall;
    break;

  case hitTankNear:
  case hitTankFar:
  case hitTankSelf:
    logMessageType = log_SoundHitTank;
    break;
  case bubbles:
  case tankSinkNear:
  case tankSinkFar:
    wantLog = FALSE;
    break;
  case bigExplosionNear:
    logMessageType = log_SoundBigExplosion;
    break;
  case bigExplosionFar:
    logMessageType = log_SoundBigExplosion;
    break;
  case farmingTreeNear:
  case farmingTreeFar:
    logMessageType = log_SoundFarm;
    break;
  case manBuildingNear:
  case manBuildingFar:
    logMessageType = log_SoundBuild;
    break;
  case manDyingNear:
  case manDyingFar:
    logMessageType = log_SoundManDie;
    break;
  
  case manLayingMineNear:
    logMessageType = log_SoundMineLay;
    break;

  case mineExplosionNear:
  case mineExplosionFar:
    logMessageType = log_SoundMineExplode;
    break;
  }

  if (wantLog == TRUE) {
    logAddEvent(logMessageType, mx, my, 0, 0, 0, NULL);
  }

}

/*********************************************************
*NAME:          netGetType
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Returns the network game type being played
*
*ARGUMENTS:
*
*********************************************************/
netType netGetType(void) {
  return 0; //FIXME
}

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
void screenTankView() {
  return;
}

/*********************************************************
*NAME:          clientCenterTank
*AUTHOR:        John Morrison
*CREATION DATE: 3/1/99
*LAST MODIFIED: 3/1/99
*PURPOSE:
*  Centers the screen around the tank.
*
*ARGUMENTS:
*
*********************************************************/
void clientCenterTank() {
  return;
}

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
void screenGetTankWorld(WORLD *x, WORLD *y) {
  messageAdd(globalMessage, (char *) "DANGEROUS: screenGetTankWorld", (char *) ""); /* This code should never be called by the server. */
  return;
}

/*********************************************************
*NAME:          screenIsItemInTrees
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED: 19/2/99
*PURPOSE:
* Returns whether a item is surrounded by trees.
*
*ARGUMENTS:
*  bmx - X position
*  bmy - Y position
*********************************************************/
bool screenIsItemInTrees(WORLD bmx, WORLD bmy) {
  messageAdd(globalMessage, (char *) "DANGEROUS: screenIsItemInTrees", (char *) ""); /* This code should never be called by the server. */
  return TRUE;
}

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
void screenGetPlayerName(char *value) {
   messageAdd(globalMessage, (char *) "DANGEROUS: screenGetPlayerName", (char *) ""); /* This code should never be called by the server. */
  return;
}

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
void screenCheckTankMineDamage(BYTE mx, BYTE my) {
  return;
}

/*********************************************************
*NAME:          netGetStatus
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 26/2/99
*PURPOSE:
* Returns the status of the network
*
*ARGUMENTS:
*
*********************************************************/
netStatus netGetStatus(void) {
  return netFailed;
}

/*********************************************************
*NAME:          screenTanksAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/98
*LAST MODIFIED: 18/2/98
*PURPOSE:
*  Adds a data set for a specific tank
*
*ARGUMENTS:
*  value      - Pointer to the screenBullets data structure
*  mx         - X co-ord of the map position
*  my         - Y co-ord of the map position
*  px         - X pixel offset
*  py         - Y pixel offset
*  frame      - Frame identifer of the tank
*  playerName - String to hold the player name
*********************************************************/
void screenTanksAddItem(screenTanks *value, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, BYTE playerNum, char *playerName) {
  messageAdd(globalMessage, (char *) "DANGEROUS: screenTanksAddItem - Something MUST go here", (char *) ""); /* This code should never be called by the server. */
  return;
}

/*********************************************************
*NAME:          netRemovePlayer
*AUTHOR:        John Morrison
*CREATION DATE: 21/3/99
*LAST MODIFIED: 21/3/99
*PURPOSE:
* Removes a player from the netplayers structure
*
*ARGUMENTS:
*  playerNum - The player number to remove.
*********************************************************/
void netRemovePlayer(BYTE playerNum) {
  messageAdd(globalMessage, (char *) "DANGEROUS: netRemovePlayer - Something MUST go here", (char *) ""); /* This code should never be called by the server. */
  return;
}


void netMessageSendPlayer(BYTE playerNum, BYTE destPlayer, char *message) {
  messageAdd(globalMessage, (char *) "DANGEROUS: netMessageSendPlayer - Something MUST go here", (char *)""); /* This code should never be called by the server. */
  return;
}


/*********************************************************
*NAME:          frontEndEnableRequestAllyMenu
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Request to enable/disable the request alliance menu
* item
* 
*ARGUMENTS:
*  enabled - TRUE for enabled/FALSE for diabled
*********************************************************/
void frontEndEnableRequestAllyMenu(bool enabled) {
  return;
}

/*********************************************************
*NAME:          frontEndEnableRequestMenu
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Request to enable/disable the leave alliance menu item
* 
*ARGUMENTS:
*  enabled - TRUE for enabled/FALSE for diabled
*********************************************************/
void frontEndEnableLeaveAllyMenu(bool enabled) {
  return;
}

/*********************************************************
*NAME:          netRequestAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Sends a request alliance message.
*
*ARGUMENTS:
*  playerNum - Player who is leaving alliance
*  requestTo - Who we are requesting to
*********************************************************/
void netRequestAlliance(BYTE playerNum, BYTE reqestTo) {
  messageAdd(globalMessage, (char *) "DANGEROUS: netRequestAlliance", (char *) ""); /* This code should never be called by the server. */
}

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
pillAlliance screenPillAlliance(BYTE pillNum) {
  messageAdd(globalMessage,  (char *) "DANGEROUS: screenPillAlliance", (char *) ""); /* This code should never be called by the server. */
  return 0;
}

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
baseAlliance screenBaseAlliance(BYTE baseNum) {
  messageAdd(globalMessage, (char *) "DANGEROUS: screenBaseAlliance", (char *) ""); /* This code should never be called by the server. */
  return 0;
}

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
BYTE screenGetNumPlayers() {
  messageAdd(globalMessage, (char *) "DANGEROUS: screenGetNumPlayers", (char *) ""); /* This code should never be called by the server. */
  return 0;
}

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
void screenChangeOwnership(BYTE oldOwner) {
  messageAdd(globalMessage, (char *) "DANGEROUS: screenChangeOwnership", (char *) ""); /* This code should never be called by the server. */
}

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
void screenMoveViewOffsetLeft(bool isLeft) {

}

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
void screenMoveViewOffsetUp(bool isUp) {
}

bool screenTankScroll() {
  return FALSE;
}

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
void screenAddBrainObject(unsigned short object, WORLD wx, WORLD wy, unsigned short idNum, BYTE dir, BYTE info) {

}

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
void clientSetInStartFind(bool entering) {

}

bool clientGetInStartFind() {
  return FALSE;
}
void frontEndShowGunsight(bool isShown){}

void netErrorOccured() {}

/*********************************************************
*NAME:          frontEndTutorial
*AUTHOR:        John Morrison
*CREATION DATE: 27/5/00
*LAST MODIFIED: 27/5/00
*PURPOSE:
* Set the front end that we have reached a new position
* 
*ARGUMENTS:
*  pos - Y Position on the map
*********************************************************/
bool frontEndTutorial(BYTE pos) {
  return FALSE;
}

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
void screenTankStopCarryingPill(BYTE itemNum) {
  return;
}

/*********************************************************
*NAME:          netRequestStartPosition
*AUTHOR:        John Morrison
*CREATION DATE: 2/4/01
*LAST MODIFIED: 2/4/01
*PURPOSE:
*  Request a new starting position off the server.
*
*ARGUMENTS:
*
*********************************************************/
void netRequestStartPosition() {
  return;
}


/* These functions never get called */

/*********************************************************
*NAME:          clientNumBases
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Returns the number of bases
*
*ARGUMENTS:
*
*********************************************************/
BYTE clientNumBases(void) {
  return 0;
}

/*********************************************************
*NAME:          clientNumPills
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Returns the number of pillboxes
*
*ARGUMENTS:
*
*********************************************************/
BYTE clientNumPills(void) {
  return 0;
}

/*********************************************************
*NAME:          clientGetNumNeutralBases
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Returns the number of neutral bases
*
*ARGUMENTS:
*
*********************************************************/
BYTE clientGetNumNeutralBases(void) {
  return 0;
}

/*********************************************************
*NAME:          clientMapIsMine
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
bool clientMapIsMine(BYTE bmx, BYTE bmy){
  return 0;
}

/*********************************************************
*NAME:          clientGetTankFromPlayer
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
tank* clientGetTankFromPlayer(BYTE playerNum) {
  return NULL;
}

/*********************************************************
*NAME:          clientGetTankPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Returns this tanks player number
*
*ARGUMENTS:
*  value - Pointer to the tank
*********************************************************/
BYTE clientGetTankPlayer(tank *value) {
  return 0;
}

/*********************************************************
*NAME:          clientGetLgmFromPlayerNum
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
lgm *clientGetLgmFromPlayerNum(BYTE playerNum) {
  return NULL;
}

/*********************************************************
*NAME:          clientGetTankWorldFromLgm
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
void clientGetTankWorldFromLgm(lgm *lgmans, WORLD *x, WORLD *y) {
  return;
}

/*********************************************************
*NAME:          clientCheckTankRange
*AUTHOR:        John Morrison
*CREATION DATE: 9/04/01
*LAST MODIFIED: 9/04/01
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
bool clientCheckTankRange(BYTE x, BYTE y, BYTE playerNum, double distance) {
  return FALSE;
}

/*********************************************************
*NAME:          clientCheckPillsRange
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
bool clientCheckPillsRange(BYTE xValue, BYTE yValue, BYTE playerNum, double distance) {
  return FALSE;
}

/*********************************************************
*NAME:          clientGetRandStart
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
void clientGetRandStart(BYTE *mx, BYTE *my, TURNTYPE *dir) {
  return;
}

/*********************************************************
*NAME:          clientMessageAdd
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
*  Functions call this to display a message. They must
*  pass the message type so that it can be determined
*  whether the header should be printed etc.
*
*ARGUMENTS:
*  msgType - The type of the message
*  top     - The message to print in the top line
*  bottom  - The message to print in the bottom line
*********************************************************/
void clientMessageAdd(messageType msgType, char *top, char *bottom) {
  return;
}

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
bool clientTankInView(BYTE playerNum, BYTE checkX, BYTE checkY) {
  return FALSE;
}

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
players *clientGetPlayers() {
  return NULL;
}

/*********************************************************
*NAME:          clientBasesMigrate
*AUTHOR:        Minhiriath
*CREATION DATE: 15/03/2009
*LAST MODIFIED: 15/03/2009
*PURPOSE:
* migrates the clients bases
*
*ARGUMENTS:
*
*********************************************************/
void clientBasesMigrate(BYTE playerNumOldOwner, BYTE playerNumNewOwner) {
  // do nothing here.
}

/*********************************************************
*NAME:          clientPillsMigratePlanted
*AUTHOR:        Minhiriath
*CREATION DATE: 15/03/2009
*LAST MODIFIED: 15/03/2009
*PURPOSE:
* migrates the clients planted pills
*
*ARGUMENTS:
*
*********************************************************/
void clientPillsMigratePlanted(BYTE playerNumOldOwner, BYTE playerNumNewOwner) {
  // do nothing here.
}

/*********************************************************
*NAME:          frontEndRedrawAll
*AUTHOR:        Minhiriath
*CREATION DATE: 16/3/2009
*LAST MODIFIED: 16/3/2009
*PURPOSE:
*  called windowRedrawAll()
* 
*ARGUMENTS:
*  none
*********************************************************/
void frontEndRedrawAll(void){
	// do nothing here.
}

/*********************************************************
*NAME:          clientGetBuildings
*AUTHOR:        John Morrison
*CREATION DATE: 04/10/03
*LAST MODIFIED: 04/10/03
*PURPOSE:
*  Returns a pointer to the buildings Object
*
*ARGUMENTS:
*
*********************************************************/
building *clientGetBuildings() {
  return NULL;
}

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
explosions *clientGetExplosions() {
  return NULL;
}

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
floodFill *clientGetFloodFill() {
  return NULL;
}

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
grass *clientGetGrass() {
  return NULL;
}

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
mines *clientGetMines() {
  return NULL;
}

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
minesExp *clientGetMinesExp() {
  return NULL;
}

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
rubble *clientGetRubble() {
  return NULL;
}

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
swamp *clientGetSwamp() {
  return NULL;
}

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
tkExplosion  *clientGetTankExplosions() {
  return NULL;
}

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
netPnbContext *clientGetNetPnb() {
  return NULL;
}

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
netMntContext *clientGetNetMnt() {
  return NULL;
}

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
gameType *clientGetGameType() {
  return NULL;
}
