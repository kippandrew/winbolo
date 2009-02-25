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
*Filename:      screen.c
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 17/12/03
*Purpose:
*  Provides Interfaces with the front end
*********************************************************/


/* NOTE: This really just here for historical reasons. It was present in each of the WIP releases
 * and used to scroll across the newswire as the game started.
 * Stuarts email address no longer works */
/*#define BOLO_VERSION_STRING "  WinBolo - WIP R11 (13/6/99) - DO NOT DISTRIBUTE\0"
#define OLD_BOLO_COPYRIGHT_STRING "Bolo © 1987-1995 Stuart Cheshire <>\0"
#define NEW_BOLO_COPYRIGHT_STRING "- WinBolo © 1998-1999 John Morrison <john@winbolo.com>          \0" */

/* Includes */
#include <string.h>
#include <time.h>
#include "global.h"
#include "bolo_map.h"
#include "pillbox.h"
#include "bases.h"
#include "starts.h"
#include "tank.h"
#include "shells.h"
#include "rubble.h"
#include "explosions.h"
#include "screenbullet.h"
#include "frontend.h"
#include "messages.h"
#include "grass.h"
#include "swamp.h"
#include "building.h"
#include "tilenum.h"
#include "screencalc.h"
#include "tankexp.h"
#include "scroll.h"
#include "lgm.h"
#include "log.h"
#include "floodfill.h"
#include "minesexp.h"
#include "treegrow.h"
#include "mines.h"
#include "labels.h"
#include "players.h"
#include "screentank.h"
#include "screenlgm.h"
#include "network.h"
#include "netpnb.h"
#include "screenbrainmap.h"
#include "backend.h"
#include "netmt.h"
#include "netpnb.h"
#include "../gui/netclient.h"
#include "screen.h"

/* Module Level Variables */

screen view = NULL;
screenMines mineView = NULL;
map mymp = NULL;
bases mybs = NULL;
pillboxes mypb = NULL;
starts myss = NULL;
tank mytk = NULL;
shells myshs = NULL;
lgm mylgman = NULL;
players plyrs = NULL;
building clientBlds = NULL;
explosions clientExpl = NULL;
floodFill clientFF = NULL;
grass clientGrass = NULL;
mines clientMines = NULL;
minesExp clientMinesExp = NULL;
rubble clientRubble = NULL;
swamp clientSwamp = NULL;
tkExplosion clientTankExplosions = NULL;
netPnbContext clientPNB = NULL;
netMntContext clientNMT = NULL;
gameType myGame;

/* The offset from the top and left of the map */
BYTE xOffset;
BYTE yOffset;

/* Pillbox View Stuff */
bool inPillView = FALSE; /* Are we in tank view or pillbox view */
BYTE pillViewX = 0;      /* Pillbox View X and Y co-ordinates */
BYTE pillViewY = 0;

/* Map name */
char mapName[MAP_STR_SIZE];

/* Game length and start delay */
int gmeStartDelay;
long gmeLength;

/* Time game was created */
time_t timeStart;

/* Are we running? */
bool screenGameRunning = FALSE;

bool needScreenReCalc = FALSE;

/* Whether computer tanks are allowed */
aiType allowComputerTanks = aiNone;

/* In start calculation */
bool inStart = FALSE;

/* Used by Brain */
unsigned long brainHoldKeys; /* Keys the brain is holding down */
unsigned long brainTapKeys;      /* Keys the brain has tapped - Executed once */
BuildInfo *brainBuildInfo = NULL;
PlayerBitMap brainsWantAllies;   /* Who you want to be allied to */
PlayerBitMap brainsMessageDest;  /* Destination for messages */
char brainsMessage[FILENAME_MAX]; /* Message to send */
unsigned short brainsNumObjects;  /* Number of brain objects */
ObjectInfo brainObjects[1024];    /* Array of brain objects  */


/* Cursor Positions */
int cursorPosX, cursorPosY;

/*********************************************************
*NAME:          screenSetup
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 05/05/01
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
void screenSetup(gameType game, bool hiddenMines, int srtDelay, long gmeLen) { 

  srand((unsigned int) time(NULL));
  xOffset = 0;
  yOffset = 0;
  mapName[0] = '\0';
  minesCreate(&clientMines, hiddenMines);
  gmeStartDelay = srtDelay;
  gmeLength = gmeLen;
  needScreenReCalc = FALSE;
  cursorPosX = -1;
  cursorPosY = -1;

  gameTypeSet(&myGame, game);
  mapCreate(&mymp);
  startsCreate(&myss);
  basesCreate(&mybs);
  playersCreate(&plyrs);
  myshs = shellsCreate();
  explosionsCreate(&clientExpl);
  rubbleCreate(&clientRubble);
  buildingCreate(&clientBlds);
  messageCreate();
  grassCreate(&clientGrass);
  swampCreate(&clientSwamp);
  mylgman = lgmCreate(0);
  floodCreate(&clientFF);
  tkExplosionCreate(&clientTankExplosions);
  minesExpCreate(&clientMinesExp);
  treeGrowCreate();
  netPNBCreate(&clientPNB);
  netMNTCreate(&clientNMT);
  pillsCreate(&mypb);
  logCreate();
  screenBrainMapCreate();
  brainHoldKeys = 0;
  brainTapKeys = 0;
  brainsWantAllies = 0;
  brainsMessageDest = 0;
  brainsMessage[0] = '\0';
  brainBuildInfo = (BuildInfo*) malloc(sizeof(brainBuildInfo));
  brainBuildInfo->action = 0;

  New(view);
  New(mineView);
/*  messageAdd(globalMessage, BOLO_VERSION_STRING, OLD_BOLO_COPYRIGHT_STRING);
  messageAdd(globalMessage, BOLO_VERSION_STRING,NEW_BOLO_COPYRIGHT_STRING);  */
  
  inStart = TRUE;
  screenGameRunning = TRUE;
}

/*********************************************************
*NAME:          screenDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 05/05/01
*PURPOSE:
*  Destroys the structures Should be called on
*  program exit
*
*ARGUMENTS:
*
*********************************************************/
void screenDestroy() {
  logDestroy();
  screenGameRunning = FALSE;
  tankDestroy(&mytk, &mymp, &mypb, &mybs);
  mytk = NULL; 
  mapDestroy(&mymp);
  startsDestroy(&myss);
  basesDestroy(&mybs);
  shellsDestroy(&myshs);
  explosionsDestroy(&clientExpl);
  rubbleDestroy(&clientRubble);
  buildingDestroy(&clientBlds );
  messageDestroy();
  grassDestroy(&clientGrass);
  netPNBDestroy(&clientPNB);
  netNMTDestroy(&clientNMT);
  floodDestroy(&clientFF);
  lgmDestroy(&mylgman);
  swampDestroy(&clientSwamp);
  clientSwamp = NULL;
  screenBrainMapDestroy();
  tkExplosionDestroy(&clientTankExplosions);
  minesExpDestroy(&clientMinesExp);
  treeGrowDestroy();
  pillsDestroy(&mypb);
  playersDestroy(&plyrs);
  if (view != NULL) {
    Dispose(view);
  }
  if (mineView != NULL) {
    Dispose(mineView);
  }
  if (brainBuildInfo != NULL) {
    free(brainBuildInfo);
    brainBuildInfo =NULL;
  }
  view = NULL;
  mineView = NULL;
  mymp = NULL;
  mybs = NULL;
  mypb = NULL;
  myss = NULL;
  mytk = NULL;
  myshs = NULL;
  mylgman = NULL;
  plyrs = NULL;
}

/*********************************************************
*NAME:          screenUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 21/01/01
*PURPOSE:
*  Updates the screen. Takes numerous directions
*
*ARGUMENTS:
*  value - Pointer to the starts structure
*********************************************************/
void screenUpdate(updateType value) {
  static bool b = FALSE;
  BYTE x; /* X and Y co-ords of the tank */
  BYTE y; 
  BYTE px; /* Pixel X and Y co-ords of the tank */
  BYTE py;
  screenGunsight gs;
  screenBullets sBullets;
  screenLgm lgms;
  screenTanks scnTnk;
  BYTE gsX;
  netStatus ns;
  BYTE oldXOffset = xOffset;
  BYTE oldYOffset = yOffset;

  if (needScreenReCalc == TRUE) {
    needScreenReCalc = FALSE;
    screenUpdateView((updateType) 0);
  }
  if (b == TRUE) {
    return;
  }
  ns = netGetStatus();
  if (ns != netRunning && ns != netFailed) {
    frontEndDrawDownload(FALSE);
    return;
  }
  if (inStart == TRUE) {
    frontEndDrawDownload(TRUE);
    return;
  }

  b = TRUE;

  x = tankGetScreenMX(&mytk);
  y = tankGetScreenMY(&mytk);
  px = tankGetScreenPX(&mytk);
  py = tankGetScreenPY(&mytk);
  
  if (px >3 || (x - xOffset) == 0) { //|| value == right 
    x++;
//    cursorPosX--;
  }
  if (py >2) {
    y++;
//    cursorPosY++;
  }

  screenLgmCreate(&lgms);
  sBullets = screenBulletsCreate();
  screenTanksCreate(&scnTnk);


  switch (value) {
  case redraw:
    break;
  case left:
    /* Left button Pressed */
    if (inPillView == FALSE) {
      if (scrollCheck((BYTE) (xOffset-1), yOffset, x, y) == TRUE) {
        xOffset--;
//        screenTankScroll();
		moveMousePointer(left);
        if (xOffset != oldXOffset) {
          cursorPosX++;
        }
        //messageAdd(globalMessage, "l", "l");
      }
      
    } else {
      screenPillView(-1, 0);
    }
    needScreenReCalc = TRUE;
    break;
  case right:
    /* Right button Pressed */
    if (inPillView == FALSE) {
      if (scrollCheck((BYTE) (xOffset+1), yOffset, x, y) == TRUE) {     
        char str[255];
        xOffset++;
//        screenTankScroll();
		moveMousePointer(right);
        if (xOffset != oldXOffset) {
          cursorPosX--;
        }
        sprintf(str, "%d,%d", xOffset, cursorPosX);
        //messageAdd(globalMessage, "r", str);
      }
    } else {
      screenPillView(1, 0);
    }
    needScreenReCalc = TRUE;
    break;
  case up:
    /* Up button Pressed */
    if (inPillView == FALSE) {
      if (scrollCheck(xOffset, (BYTE) (yOffset-1), x, y) == TRUE) {
        yOffset--;
  //      screenTankScroll();
		moveMousePointer(up);
        if (yOffset != oldYOffset) {
          cursorPosY++;
        }
        //messageAdd(globalMessage, "up", "up");
      }
    } else {
      screenPillView(0, -1);
    }
    needScreenReCalc = TRUE;
    break;
  case down:
  default:
    /* Down button pressed */
    if (inPillView == FALSE) {
      if (scrollCheck(xOffset, (BYTE) (yOffset+1), x, y) == TRUE) {
        yOffset++;
    //    screenTankScroll();
		moveMousePointer(down);
        if (yOffset != oldYOffset) {
          cursorPosY--;
        }

      //  messageAdd(globalMessage, "d", "d");
      }
    } else {
      screenPillView(0, 1);
    }
    needScreenReCalc = TRUE;
    break;
  }
  
  
  if (value == redraw) {
    /* Prepare the tanks */
    screenTanksPrepare(&scnTnk, &mytk, xOffset, (BYTE) (xOffset + MAIN_BACK_BUFFER_SIZE_X), yOffset, (BYTE) (yOffset + MAIN_BACK_BUFFER_SIZE_Y));
    /* Prepare the lgms */
    screenLgmPrepare(&lgms, xOffset, (BYTE) (xOffset + MAIN_BACK_BUFFER_SIZE_X-1 ), yOffset, (BYTE) (yOffset + MAIN_BACK_BUFFER_SIZE_Y-1));
    if (tankIsGunsightShow(&mytk) == TRUE) {
      tankGetGunsight(&mytk, &gsX, &(gs.mapY), &(gs.pixelX), &(gs.pixelY));
      gs.mapX = gsX;

      if (gs.mapX >= xOffset && gs.mapX < (xOffset + MAIN_BACK_BUFFER_SIZE_X-1) && gs.mapY >= yOffset && gs.mapY < (yOffset + MAIN_BACK_BUFFER_SIZE_Y -1 )) {
        gs.mapX -= xOffset;
        gs.mapY -= yOffset;        
      } else {
        gs.mapX = NO_GUNSIGHT;
      }
    } else {
      gs.mapX = NO_GUNSIGHT;
    }

    shellsCalcScreenBullets(&myshs, &sBullets, xOffset, (BYTE) (xOffset + MAIN_BACK_BUFFER_SIZE_X-1), yOffset, (BYTE) (yOffset + MAIN_BACK_BUFFER_SIZE_Y-1));
    explosionsCalcScreenBullets(&clientExpl, &sBullets, xOffset, (BYTE) (xOffset + MAIN_BACK_BUFFER_SIZE_X-1), yOffset, (BYTE) (yOffset + MAIN_BACK_BUFFER_SIZE_Y-1));
    tkExplosionCalcScreenBullets(&clientTankExplosions, &sBullets, xOffset, (BYTE) (xOffset + MAIN_BACK_BUFFER_SIZE_X-1), yOffset, (BYTE) (yOffset + MAIN_BACK_BUFFER_SIZE_Y-1));
    frontEndDrawMainScreen(&view, &mineView, &scnTnk, &gs, &sBullets, &lgms, gmeStartDelay, inPillView, &mytk, 0, 0);
  }
  screenBulletsDestroy(&sBullets);
  screenLgmDestroy(&lgms);
  screenTanksDestroy(&scnTnk);
  b = FALSE;
}

/*********************************************************
*NAME:          screenGetPos
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 30/12/2008
*PURPOSE:
*  Gets the value of a square in the structure
*  Return RIVER if out of range
*
*ARGUMENTS:
*  value  - Pointer to the screen structure
*  xValue - The X co-ordinate
*  yValue - The Y co-ordinate
*********************************************************/
BYTE screenGetPos(screen *value,BYTE xValue, BYTE yValue) {
  BYTE returnValue = RIVER; /* Value to return */

  if (xValue < MAIN_BACK_BUFFER_SIZE_X && yValue < MAIN_BACK_BUFFER_SIZE_Y) {
    returnValue = (*value)->screenItem[xValue][yValue];
  }
  return returnValue;
}

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
bool screenIsMine(screenMines *value,BYTE xValue, BYTE yValue) {
  bool returnValue = FALSE; /* Value to return */

  if (xValue <= MAIN_SCREEN_SIZE_X && yValue <= MAIN_SCREEN_SIZE_Y) {
    returnValue = ((*value)->mineItem[xValue][yValue]);
  }
  return returnValue;
}

/*********************************************************
*NAME:          screenUpdateView
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Updates the values in the view area
*
*ARGUMENTS:
* value - The update type (Helps in optimisations)
*********************************************************/
void screenUpdateView(updateType value) {
/* NOTE: value UNUSED */
  BYTE count;   /* Looping Variables */
  BYTE count2;

  for (count=0;count<MAIN_BACK_BUFFER_SIZE_X;count++) {
    for (count2=0;count2<MAIN_BACK_BUFFER_SIZE_Y;count2++) {
      (*view).screenItem[count][count2] = screenCalcSquare((BYTE) (count+xOffset),(BYTE) (count2+yOffset), count, count2);
      screenBrainMapSetPos((BYTE) (count+xOffset), (BYTE) (count2+yOffset), mapGetPos(&mymp, (BYTE) (count+xOffset), (BYTE) (count2+yOffset)), minesExistPos(&clientMines, (BYTE) (count+xOffset), (BYTE) (count2+yOffset)));
    }
  }
}

/*********************************************************
*NAME:          screenCalcSquare
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 30/01/02
*PURPOSE:
*  Calculates the terrain type for a given location
*
*ARGUMENTS:
*  xValue - The x co-ordinate
*  yValue - The y co-ordinate
*********************************************************/
BYTE screenCalcSquare(BYTE xValue, BYTE yValue, BYTE scrX, BYTE scrY) {
  baseAlliance ba;  /* The allience of a base */
  BYTE returnValue; /* Value to return */
  BYTE currentPos;
  BYTE aboveLeft;
  BYTE above;
  BYTE aboveRight;
  BYTE leftPos;
  BYTE rightPos;
  BYTE belowLeft;
  BYTE below;
  BYTE belowRight;

  (*mineView).mineItem[scrX][scrY] = FALSE;
  /* Set up Items */
  if ((pillsExistPos(&mypb,xValue,yValue)) == TRUE) {
    returnValue = pillsGetScreenHealth(&mypb, xValue, yValue);
  } else if ((basesExistPos(&mybs,xValue,yValue)) == TRUE) {
     ba = basesGetAlliancePos(&mybs, xValue, yValue);
    switch (ba) {
    case baseOwnGood:
      returnValue = BASE_GOOD;
      break;
    case baseAllieGood:
      returnValue = BASE_GOOD;
      break;
    case baseNeutral:
      returnValue = BASE_NEUTRAL;
      break;
    case baseDead:
      if (basesAmOwner(&mybs, playersGetSelf(screenGetPlayers()), xValue, yValue) == TRUE) {
        returnValue = BASE_GOOD;
      } else {
        returnValue = BASE_EVIL;
      }
      break;
    case baseEvil:
    default:
      /* Base Evil */
      returnValue = BASE_EVIL;
    }
  }  else {
    currentPos = mapGetPos(&mymp,xValue,yValue);
	// this is a invisiwall check, if there is supposed to be a mine here, but its a building, thats impossible, so, remove the mine and reset the terrain back to its correct value.
	if (currentPos>=HALFBUILDING+MINE_SUBTRACT&&currentPos!=DEEP_SEA){
	  minesRemoveItem(screenGetMines(), xValue, yValue);
	  (*mineView).mineItem[scrX][scrY] = FALSE;
	  currentPos = currentPos - MINE_SUBTRACT;
	  mapSetPos(&mymp, xValue, yValue, currentPos,TRUE,TRUE);
	}
    if (mapIsMine(&mymp, xValue, yValue) == TRUE) {
      if (minesExistPos(&clientMines, xValue, yValue) == TRUE) {
        (*mineView).mineItem[scrX][scrY] = TRUE;
      }
	  if (currentPos != DEEP_SEA) {
        currentPos = currentPos - MINE_SUBTRACT;
      }
    } else {
      (*mineView).mineItem[scrX][scrY] = FALSE;
    }

    if (basesExistPos(&mybs, (BYTE) (xValue-1), (BYTE) (yValue-1)) == TRUE) {
      aboveLeft = ROAD;
    } else {
      aboveLeft = mapGetPos(&mymp,(BYTE) (xValue-1),(BYTE) (yValue-1));
      if (aboveLeft >= MINE_START && aboveLeft <= MINE_END) {
        aboveLeft = aboveLeft - MINE_SUBTRACT;
      }
    }

    if (basesExistPos(&mybs, xValue, (BYTE) (yValue-1)) == TRUE) {
      above = ROAD;
    } else {
      above = mapGetPos(&mymp,xValue,(BYTE) (yValue-1));
      if (above >= MINE_START && above <= MINE_END) {
        above = above - MINE_SUBTRACT;
      }
    }

    if (basesExistPos(&mybs, (BYTE) (xValue+1), (BYTE) (yValue-1)) == TRUE) {
      aboveRight = ROAD;
    } else {
      aboveRight = mapGetPos(&mymp,(BYTE) (xValue+1),(BYTE) (yValue-1));
      if (aboveRight >= MINE_START && aboveRight <= MINE_END) {
        aboveRight = aboveRight - MINE_SUBTRACT;
      }
    }

    if (basesExistPos(&mybs, (BYTE) (xValue-1), yValue) == TRUE) {
      leftPos = ROAD;
    } else {
      leftPos = mapGetPos(&mymp,(BYTE) (xValue-1),yValue);
      if (leftPos >= MINE_START && leftPos <= MINE_END) {
        leftPos = leftPos - MINE_SUBTRACT;
      }
    }

    if (basesExistPos(&mybs, (BYTE) (xValue+1), yValue) == TRUE) {
      rightPos = ROAD;
    } else {
      rightPos = mapGetPos(&mymp,(BYTE) (xValue+1),yValue);
      if (rightPos >= MINE_START && rightPos <= MINE_END) {
        rightPos = rightPos - MINE_SUBTRACT;
      }
    }

    if (basesExistPos(&mybs, (BYTE) (xValue-1), (BYTE) (yValue+1)) == TRUE) {
      belowLeft = ROAD;
    } else {
      belowLeft = mapGetPos(&mymp,(BYTE) (xValue-1),(BYTE) (yValue+1));
      if (belowLeft >= MINE_START && belowLeft <= MINE_END) {
        belowLeft = belowLeft - MINE_SUBTRACT;
      }
    }


    if (basesExistPos(&mybs, xValue, (BYTE) (yValue+1)) == TRUE) {
      below = ROAD;
    } else {
      below = mapGetPos(&mymp,xValue,(BYTE) (yValue+1));
      if (below >= MINE_START && below <= MINE_END) {
        below = below - MINE_SUBTRACT;
      }
    }

    if (basesExistPos(&mybs, (BYTE) (xValue+1), (BYTE) (yValue+1)) == TRUE) {
      belowRight = ROAD;
    } else {
      belowRight = mapGetPos(&mymp,(BYTE) (xValue+1),(BYTE) (yValue+1));
      if (belowRight >= MINE_START && belowRight <= MINE_END) {
        belowRight = belowRight - MINE_SUBTRACT;
      }
    }

    switch (currentPos) {
    case ROAD:
      returnValue = screenCalcRoad(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
      break;
    case BUILDING:
      returnValue = screenCalcBuilding(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
      break;
    case FOREST:
      returnValue = screenCalcForest(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
      break;
    case RIVER:
      returnValue = screenCalcRiver(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
      break;
    case DEEP_SEA:
      returnValue = screenCalcDeepSea(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
      break;
    case BOAT:
      returnValue = screenCalcBoat(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
      break;
    case CRATER:
      returnValue = screenCalcCrater(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
      break;
    default:
      returnValue = currentPos;
      break;
    }
  }
  return returnValue;
}


/*********************************************************
*NAME:          screenLoadMap
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 30/01/02
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
bool screenLoadMap(char *fileName, gameType game, bool hiddenMines, long srtDelay, long gmeLen, char *playerName, bool wantFree) {
  bool returnValue; /* Value to return */
  bool doneFree;    /* If we have done the free */
  
  returnValue = FALSE;
  doneFree = FALSE;
  screenSetup(game, hiddenMines,srtDelay,gmeLen);
  returnValue = mapRead(fileName, &mymp, &mypb, &mybs, &myss);

/* 
 * Used to write out linux compressed map file.
  {
  BYTE buff[16*1024];
  int buffLen;
  FILE *fp;

  fp = fopen("c:\\map.txt", "wb");
  buffLen = mapSaveCompressedMap(&mp, &mypb, &mybs, &ss, buff);
  fwrite(buff, buffLen, 1, fp);
  fclose(fp);

  }   */

  if (returnValue == TRUE) {
    utilExtractMapName(fileName, mapName);
    utilStripNameReplace(playerName);
    screenSetupTank(playerName);
    screenUpdateView(redraw);
    basesClearMines(&mybs, &mymp);

  } else {
    screenDestroy();
    doneFree = TRUE;
  }
  if (doneFree == FALSE && wantFree == TRUE) {
    screenDestroy();
  }

  return returnValue;
}

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
tankButton screenTranslateBrainButtons(bool *isShoot, bool isGameTick) {
  tankButton returnValue; /* Value to return */
  unsigned long temp;     /* Used to temp store brainHoldKeys */
  unsigned long temp2;    /* Used to temp store keyShoot if not game tick */

  temp = brainHoldKeys;
  temp2 = 0;

  /* Handle tap keys */
  if (testkey(brainTapKeys, KEY_faster)) {
    setkey(brainHoldKeys, KEY_faster);
  }
  if (testkey(brainTapKeys, KEY_slower)) {
    setkey(brainHoldKeys, KEY_slower);
  }
  if (testkey(brainTapKeys, KEY_turnleft)) {
    setkey(brainHoldKeys, KEY_turnleft);
  }
  if (testkey(brainTapKeys, KEY_turnright)) {
    setkey(brainHoldKeys, KEY_turnright);
  }
  if (testkey(brainTapKeys, KEY_morerange)) {
    tankGunsightIncrease(&mytk);
  }
  if (testkey(brainTapKeys, KEY_lessrange)) {
    tankGunsightDecrease(&mytk);
  }
  if (testkey(brainTapKeys, KEY_shoot)) {
    if (isGameTick == TRUE) {
      *isShoot = TRUE;
    } else {
      setkey(temp2, KEY_shoot);
    }
  }
  if (testkey(brainTapKeys, KEY_dropmine)) {
    tankLayMine(&mytk, &mymp, &mypb, &mybs);
  }
  if (testkey(brainTapKeys, KEY_TankView)) {
    inPillView = FALSE;
    screenCenterTank();
  }
  if (testkey(brainTapKeys, KEY_PillView)) {
    screenPillView(0, 0);
  }

  brainTapKeys = temp2;

  /* Do the lookups */
  if (testkey(brainHoldKeys, KEY_faster) && testkey(brainHoldKeys, KEY_turnright)) {
    returnValue = TRIGHTACCEL;
  } else if (testkey(brainHoldKeys, KEY_faster) && testkey(brainHoldKeys, KEY_turnleft)) {
    returnValue = TLEFTACCEL;
  } else if (testkey(brainHoldKeys, KEY_slower) && testkey(brainHoldKeys, KEY_turnleft)) {
    returnValue = TLEFTDECEL;
  } else if (testkey(brainHoldKeys, KEY_slower) && testkey(brainHoldKeys, KEY_turnright)) {
    returnValue = TRIGHTDECEL;
  } else if (testkey(brainHoldKeys, KEY_faster)) {
    returnValue = TACCEL;
  } else if (testkey(brainHoldKeys, KEY_slower)) {
    returnValue = TDECEL;
  } else if (testkey(brainHoldKeys, KEY_turnleft)) {
    returnValue = TLEFT;
  } else if (testkey(brainHoldKeys, KEY_turnright)) {
    returnValue = TRIGHT;
  } else {
    returnValue = TNONE;
  }

  /* Restore it */
  brainHoldKeys = temp;

  /* Handle remaining keys */
  if (testkey(brainHoldKeys, KEY_morerange)) {
    tankGunsightIncrease(&mytk);
  }
  if (testkey(brainHoldKeys, KEY_lessrange)) {
    tankGunsightDecrease(&mytk);
  }
  if (testkey(brainHoldKeys, KEY_shoot) && isGameTick == TRUE) {
    *isShoot = TRUE;
  }
  if (testkey(brainHoldKeys, KEY_dropmine)) {
    tankLayMine(&mytk, &mymp, &mypb, &mybs);
  }
  if (testkey(brainHoldKeys, KEY_TankView)) {
    inPillView = FALSE;
    screenCenterTank();
  }
  if (testkey(brainHoldKeys, KEY_PillView)) {
    screenPillView(0, 0);
  }

  return returnValue;
}

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
* isBrain   - TRUE if a brain is running. Other 
*             parameters are ignored.
*********************************************************/
void screenGameTick(tankButton tb, bool tankShoot, bool isBrain) {
  static BYTE messageTime = 0;
  tank ta[MAX_TANKS];
  WORLD tankX;
  WORLD tankY;
  BYTE armour;
  BYTE shellsAmount;
  BYTE minesAmount;
  
  BYTE ngs;  /* Network status */

  BYTE tmx; /* Get the tanks gunsight */
  BYTE tmy;
  BYTE pmx;
  BYTE pmy;
  lgm *test;

  ngs = netGetStatus();


  if (screenGameRunning == FALSE || ngs == netFailed) {
    return;
  }


  /* Check Tank CRC for changes */
  if (threadsGetContext() == FALSE) {
//    if (tankCheck(&mytk) == TRUE) {
//      netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 0xFF , 0xFF);
//    }
  }

  
  if (gmeStartDelay > 0) {
    gmeStartDelay--;
    messageTime++;
    if (messageTime == MESSAGE_SCROLL_TIME) {
      messageUpdate();
      messageTime = 0;
    }
    return;
  }
  if (gmeLength > 0) {
    gmeLength--;
    if (gmeLength == 0) {
    /* Game Times Up. */
      screenGameRunning = FALSE;
      frontEndGameOver();
      return;
    }
  }


  pillsUpdate(&mypb, &mymp, &mybs, &mytk, &myshs);
  basesUpdate(&mybs, &mytk);

  if (ngs != netRunning) {
    /* Messaging */
    messageTime++;
    if (messageTime == MESSAGE_SCROLL_TIME) {
      messageUpdate();
      messageTime = 0;
    }
    return;
  }
  
  /* Do the brain translation if required */
  if (isBrain == TRUE) {
    tb = screenTranslateBrainButtons(&tankShoot, TRUE);
  }

  /* Update if the tank is moving or a button is being pressed */
  tankUpdate(&mytk,&mymp,&mybs,&mypb,&myshs, &myss, tb, tankShoot, isBrain);
  
  if (tankGetSpeed(&mytk) > 0) {
    tankGetGunsight(&mytk, &tmx, &tmy, &pmx, &pmy);
    if (inPillView == FALSE) {
      int oldXOffset = xOffset;
      int oldYOffset = yOffset;
      if (scrollUpdate(&mypb, &xOffset, &yOffset, (tankGetScreenMX(&mytk)), (tankGetScreenMY(&mytk)), TRUE, tmx, tmy,(tankGetSpeed(&mytk)), (tankGetArmour(&mytk)), (TURNTYPE) (tankGetTravelAngel(&mytk)),  FALSE) == TRUE) {
        if (oldXOffset < xOffset) {
          cursorPosX--;
		  moveMousePointer(right);
        } else if (oldXOffset > xOffset) {
          cursorPosX++;
		  moveMousePointer(left);
        }
        if (oldYOffset < yOffset) {
          cursorPosY--;
		  moveMousePointer(down);
        } else if (oldYOffset > yOffset) {
          cursorPosY++;
		  moveMousePointer(up);
        }
          
        screenReCalc();
      }
    }
  }


  /* Check we are still allowed to be in pillbox view */
  if (inPillView == TRUE) {
    if (pillsCheckView(&mypb, pillViewX, pillViewY) == FALSE) {
      screenTankView();
    }
  }

  tankGetWorld(&mytk, &tankX, &tankY);
  ta[0] = mytk;
  basesGetStats(&mybs,(basesGetClosest(&mybs,tankX,tankY)), &shellsAmount, &armour, &minesAmount);
  frontEndUpdateBaseStatusBars(shellsAmount, minesAmount, armour);
  shellsUpdate(&myshs, &mymp, &mypb, &mybs, ta, 1, FALSE);
  lgmUpdate(&mylgman, &mymp, &mypb, &mybs, &mytk);
  test = &mylgman;
  tkExplosionUpdate(&clientTankExplosions, &mymp, &mypb, &mybs, (lgm **) &test, 1, &mytk);
  explosionsUpdate(&clientExpl);
  minesExpUpdate(&clientMinesExp, &mymp, &mypb, &mybs, (lgm **) &(test), 1);
  floodUpdate(&clientFF, &mymp, &mypb, &mybs);
  treeGrowUpdate(&mymp, &mypb, &mybs);
  mapNetUpdate(&mymp, &mypb, &mybs);


  /* Update network LGM frames as the are client controlled */
  playersGameTickUpdate(screenGetPlayers());
  
  /* Messaging */
  messageTime++;
  if (messageTime == MESSAGE_SCROLL_TIME) {
    messageUpdate();
    messageTime = 0;
  }

}

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
void screenKeysTick(tankButton tb, bool isBrain) {
  BYTE bmx;   /* Tank x and y Co-ordinates */
  BYTE bmy;
  bool dummy; /* Dummy variable used for parameter passing */
  
  if (screenGameRunning == FALSE || netGetStatus() == netFailed) {
    return;
  }

  
  /* Check to make sure the game is running */
  if (gmeStartDelay > 0 || gmeLength == 0) {
    return;
  }

  /* Do the brain translation if required */
  if (isBrain == TRUE) {
    tb = screenTranslateBrainButtons(&dummy, FALSE);
  }


  bmx = tankGetMX(&mytk);
  bmy = tankGetMY(&mytk);
  tankTurn(&mytk, &mymp, &mypb, &mybs, bmx, bmy, tb);
}

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
  return basesGetNumBases(&mybs);
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
  return basesGetStatusNum(&mybs, baseNum);
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
  return pillsGetNumPills(&mypb);
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
  return pillsGetAllianceNum(&mypb, pillNum);
}

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
void screenGetTankStats(BYTE *shellsAmount, BYTE *minesAmount, BYTE *armourAmount, BYTE *treesAmount) {
  tankGetStats(&mytk, shellsAmount, minesAmount, armourAmount, treesAmount);
  if (*armourAmount > TANK_FULL_ARMOUR) {
    *armourAmount = 0;
  }
}

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
void screenGunsightRange(bool increase) {
  if (increase == TRUE) {
    tankGunsightIncrease(&mytk);
  } else {
    tankGunsightDecrease(&mytk);
  }
}

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
void screenSetGunsight(bool shown) {
  tankSetGunsight(&mytk, shown);
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
  if (threadsGetContext() == FALSE) {
    needScreenReCalc = TRUE;
  }
}

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
void screenGetMessages(char *top, char *bottom) {
  messageGetMessage(top,bottom);
}

/*********************************************************
*NAME:          clientCenterTank
*AUTHOR:        John Morrison
*CREATION DATE: 5/11/99
*LAST MODIFIED: 5/11/99
*PURPOSE:
*  Centers the screen around the tank.
*
*ARGUMENTS:
*
*********************************************************/
void clientCenterTank() {
  BYTE high, low, health, dummy;

  tankGetStats(&mytk, &high, &low, &health, &dummy);
  if (health <= TANK_FULL_ARMOUR) {
    /* Tank isn't dead */
    scrollCenterObject(&xOffset, &yOffset, (tankGetMX(&mytk)), (tankGetMY(&mytk)));
    screenReCalc();
  }
}

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
void screenSetupTank(char *playerName) {
  if (mytk != NULL) {
    tankDestroy(&mytk, &mymp, &mypb, &mybs);
    mytk = NULL;
  }
  tankCreate(&mytk,&myss);
  playersSetSelf(screenGetPlayers(), (playerNumbers) 0, playerName);
  frontEndSetPlayer((playerNumbers) playersGetSelf(screenGetPlayers()), playerName);
}

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
void screenGetKillsDeaths(int *kills, int *deaths) {
  tankGetKillsDeaths(&mytk, kills, deaths);
}

/*********************************************************
*NAME:          screenShowMessages
*AUTHOR:        John Morrison
*CREATION DATE:  8/1/99
*LAST MODIFIED:  1/6/00
*PURPOSE:
*  Turns on/off messages menus stuff
*
*ARGUMENTS:
*  msgType - The message type that is being set
*  isShown - Is it being turned on or off
*********************************************************/
void screenShowMessages(BYTE msgType, bool isShown) {
  switch (msgType) {
  case MSG_NEWSWIRE:
    messageSetNewswire(isShown);
    break;
  case MSG_ASSISTANT:
    messageSetAssistant(isShown);
    break;
  case MSG_AI:
    messageSetAI(isShown);
    break;
  case MSG_NETSTATUS:
    messageSetNetStatus(isShown);
    break;
  default:
    /* MSG_Network */
    messageSetNetwork(isShown);
    break;
  }
}

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
void screenManMove(buildSelect buildS) {
  if (tankGetArmour(&mytk) <= TANK_FULL_ARMOUR && netGetStatus() != netFailed) {
    /* If not dead */
    lgmAddRequest(&mylgman, &mymp, &mypb,  &mybs, &mytk, (BYTE) (cursorPosX + xOffset), (BYTE) (cursorPosY + yOffset), (BYTE) buildS);
  }
}

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
void screenSetAutoScroll(bool isAuto) {
  scrollSetScrollType(isAuto);
}

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
void screenGetTankMapCoord(BYTE *mx, BYTE *my) {
  *mx = tankGetScreenMX(&mytk);
  *my = tankGetScreenMY(&mytk);
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
  startsGetRandStart(&myss, mx, my ,dir);
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
  tankGetWorld(&mytk, x, y);
}

/*********************************************************
*NAME:          screenLgmDropPill
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 14/3/99
*PURPOSE:
* Man has died and needs to drop a pill. Pass this onto
* the pillbox structure
*
*ARGUMENTS:
*  mx      - Pointer to hold Map X Co-ordinates
*  my      - Pointer to hold Map Y Co-ordinates
*  owner   - Who owns the pill
*  pillNum - Pill number to place 
*********************************************************/
void screenLgmDropPill(BYTE mx, BYTE my, BYTE owner, BYTE pillNum) {
  pillbox item;   /* Item to add to the pillbox */

  item.armour = 0;
  item.owner = owner;
  item.speed = PILLBOX_ATTACK_NORMAL;
  item.reload = PILLBOX_ATTACK_NORMAL;
  item.coolDown = 0;
  item.inTank = FALSE;
  item.x = mx;
  item.y = my;
  item.justSeen = FALSE;
  pillsSetPill(&mypb,&item, pillNum);
  netPNBAdd(&clientPNB, NPNB_PILL_DEAD, (BYTE) (pillNum-1), playersGetSelf(screenGetPlayers()), mx, my);
  frontEndStatusPillbox(pillNum, (pillsGetAllianceNum(&mypb, pillNum)));
}

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
void screenTankLayMine() {
  tankLayMine(&mytk, &mymp, &mypb, &mybs);
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
  tankMineDamage(&mytk, &mymp, &mypb, &mybs, mx, my);
}

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
BYTE screenPillNumPos(BYTE mx, BYTE my) {
  return pillsGetPillNum(&mypb, (BYTE) (xOffset+mx), (BYTE) (yOffset+my), FALSE, FALSE);
}

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
BYTE screenBaseNumPos(BYTE mx, BYTE my) {
  return basesGetBaseNum(&mybs, (BYTE) (xOffset+mx), (BYTE) (yOffset+my));
}

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
void screenGetMapName(char *value) {
  strcpy(value, mapName);
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
  return playersGetNumPlayers(screenGetPlayers());
}

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
bool screenGetAllowHiddenMines() {
  return minesGetAllowHiddenMines(&clientMines);
}

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
void screenSetAllowHiddenMines(bool hidden) {
  minesDestroy(&clientMines);
  minesCreate(&clientMines, hidden);
}

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
long screenGetGameTimeLeft() {
  return gmeLength;
}

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
long screenGetGameStartDelay() {
  return gmeStartDelay;
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
  if (*(screenGetPlayers()) == NULL) {
    playersGetPlayerName(NULL, 0, value);
  } else {
    playersGetPlayerName(screenGetPlayers(), playersGetSelf(screenGetPlayers()), value);
  }
}

/*********************************************************
*NAME:          screenSetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 01/02/99
*LAST MODIFIED: 30/01/02
*PURPOSE:
* Checks to see if the new name is taken. If not the it
* changes the players name to the new name. Returns whether
* the change took place or not
*
*ARGUMENTS:
*  value - New Player Name
*********************************************************/
bool screenSetPlayerName(char *value) {
  bool returnValue;              /* Value to return */

  utilStripNameReplace(value);
  returnValue = playersSetPlayerName(screenGetPlayers(), playersGetSelf(screenGetPlayers()), value);
  if (returnValue == TRUE) {
    if (netGetType() != netSingle) {
      netSendChangePlayerName(playersGetSelf(screenGetPlayers()), value);
    }
  }
  return returnValue;
}

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
void screenSetLabelOwnTank(bool value) {
  labelSetLabelOwnTank(value);
}

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
void screenSetMesageLabelLen(labelLen value) {
  labelSetSenderLength(value);
}

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
void screenSetTankLabelLen(labelLen value) {
  labelSetTankLength(value);
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
  inPillView = FALSE;
  screenCenterTank();
}

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
void screenPillView(int horz, int vert) {
  bool result; /* Was the operation successful */

  if (inPillView == FALSE) {
    if (pillsCheckView(&mypb, pillViewX, pillViewY) == TRUE) {
      inPillView = TRUE;
      scrollCenterObject(&xOffset, &yOffset, pillViewX, pillViewY);
      screenReCalc();
    } else {
      result = pillsGetNextView(&mypb, &pillViewX, &pillViewY, FALSE);
      if (result == TRUE) {
        /* Center on the object */
        inPillView = TRUE;
        scrollCenterObject(&xOffset, &yOffset, pillViewX, pillViewY);
        screenReCalc();
      } else {
        inPillView = FALSE;
      }
    }
  } else {
    if (horz == 0 && vert == 0) {
      result = pillsGetNextView(&mypb, &pillViewX, &pillViewY, TRUE);
      if (result == FALSE) {
        screenTankView();
      } else {
        /* Center on the object */
        scrollCenterObject(&xOffset, &yOffset, pillViewX, pillViewY);
        screenReCalc();
      }
    } else {
      if (pillsMoveView(&mypb, &pillViewX, &pillViewY, horz, vert) == TRUE) {
        scrollCenterObject(&xOffset, &yOffset, pillViewX, pillViewY);
        screenReCalc();
      }
    }
  }
}

/*********************************************************
*NAME:          screenSendMessageAllPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 5/2/99
*LAST MODIFIED: 24/4/99
*PURPOSE:
* Front end wants to send a message
*
*ARGUMENTS:
*  messageStr - Message to send
*********************************************************/
void screenSendMessageAllPlayers(char *messageStr) {
  char topLine[FILENAME_MAX];       /* The message topline */

  topLine[0] = '\0';
  playersMakeMessageName(screenGetPlayers(), playersGetSelf(screenGetPlayers()), topLine);
  messageAdd((messageType) (playersGetSelf(screenGetPlayers()) + PLAYER_MESSAGE_OFFSET), topLine, messageStr);
  netMessageSendAllPlayers(playersGetSelf(screenGetPlayers()), messageStr);
}

/*********************************************************
*NAME:          screenSaveMap
*AUTHOR:        John Morrison
*CREATION DATE:  5/2/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Saves the map. Returns whether the operation was 
* sucessful or not.
*
*ARGUMENTS:
*  fileName - path and filename to save
*********************************************************/
bool screenSaveMap(char *fileName) {
  bool returnValue;                 /* Value to return */
  char name[FILENAME_MAX];         /* The Me@This computer line */
  char output[FILENAME_MAX];        /* The message eg Me just saved map etc */

  output[0] = '\0';
  name[0] = '\0';

  returnValue = mapWrite(fileName, &mymp, &mypb, &mybs, &myss);
  if (returnValue == TRUE) {
    netPNBAdd(&clientPNB, NPNB_SAVEMAP, 0, playersGetSelf(screenGetPlayers()), 0, 0);
    if (netGetType() == netSingle) {
      playersMakeMessageName(screenGetPlayers(), playersGetSelf(screenGetPlayers()), name);
      strcat(output, MESSAGE_QUOTES);
      strcat(output, name);
      strcat(output, MESSAGE_QUOTES);
      strcat(output, langGetText(MESSAGE_SAVED_MAP));
      messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), output);
    }
  }
  return returnValue;
}

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
tankAlliance screenTankAlliance(BYTE playerNum) {
  return playersScreenAllience(screenGetPlayers(), (BYTE) (playerNum-1));
}

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
bool screenIsItemInTrees(WORLD bmx, WORLD bmy) {
  bool returnValue; /* Value to return */
  int xDiff;        /* X and Y differences in location */
  int yDiff;

  xDiff = tankGetScreenMX(&mytk) - bmx;
  yDiff = tankGetScreenMY(&mytk) - bmy;
  if (xDiff >= MIN_SIGHT_DISTANCE_LEFT && xDiff <= MIN_SIGHT_DISTANCE_RIGHT && yDiff >= MIN_SIGHT_DISTANCE_LEFT && yDiff <= MIN_SIGHT_DISTANCE_RIGHT) {
    returnValue = FALSE;
  } else {
    returnValue = utilIsTankInTrees(&mymp, &mypb, &mybs, bmx, bmy);
  }
  return returnValue;
}

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
BYTE screenGetNumNeutralPills(void) {
  return pillsGetNumNeutral(&mypb);
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
  return basesGetNumNeutral(&mybs);
}

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
long screenGetTimeGameCreated(void) {
  return (long) timeStart;
}

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
void screenSetTimeGameCreated(long value) {
  timeStart = value;
}

/*********************************************************
*NAME:          screenNetMapItem
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED: 23/2/99
*PURPOSE:
* A map item sent from the network
*
*ARGUMENTS:
*  mx      - X position of the data item
*  my      - Y position of the data item
*  terrain - Terrain the data item
*********************************************************/
void screenNetMapItem(BYTE mx, BYTE my, BYTE terrain) {
  mapNetPacket(&mymp, mx, my, terrain);
}

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
void screenNetToken() {
  //mapNetMerge(&mymp);
}

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
void screenSetMapName(char *name) {
  strcpy(mapName, name);
}

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
void screenSetTimeLengths(int srtDelay, long gmeLen) {
  gmeStartDelay = srtDelay;
  gmeLength = gmeLen;
}


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
void screenSetGameType(gameType gt) {
  gameTypeSet(&myGame, gt);
}

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
void screenNetSetupTank(bool isInStart) {
  inStart = isInStart;
  tankCreate(&mytk,&myss);
  tankSetWorld(&mytk, 0, 0, (TURNTYPE) 0, FALSE);
}

/*********************************************************
*NAME:          screenNetSetupTankGo
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 27/11/99
*PURPOSE:
*  Map download is complete and we are ready to start
*  playing
*
*ARGUMENTS:
*
*********************************************************/
void screenNetSetupTankGo() {
  WORLD wx;  /* World co-ordinates to set */
  WORLD wy;
  BYTE x;    /* Things to pass to get the player start position */
  BYTE y;
  TURNTYPE dir;
  BYTE count;  /* Looping variables */
  BYTE count2;

  clientSetInStartFind(TRUE);
  startsGetStart(&myss, &x, &y, &dir, screenGetTankPlayer(&mytk));
  wx = x;
  wx <<= TANK_SHIFT_MAPSIZE;
  wx += MAP_SQUARE_MIDDLE;
  wy = y;
  wy <<= TANK_SHIFT_MAPSIZE;
  wy += MAP_SQUARE_MIDDLE;
  tankSetWorld(&mytk, wx, wy, dir, TRUE);
  screenCenterTank();
  clientSetInStartFind(FALSE);

  for (count=0;count<MAIN_BACK_BUFFER_SIZE_X;count++) {
    for (count2=0;count2<MAIN_BACK_BUFFER_SIZE_Y;count2++) {
      (*mineView).mineItem[count][count2] = FALSE;
    }
  }

  screenReCalc();
}

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
void screenSetBaseNetData(BYTE *buff, int length) {
  BYTE count; /* Looping variable */
  BYTE max;   /* Max items */
  char pn[PLAYER_NAME_LEN];
  
  basesSetBaseNetData(&mybs, buff, length);
  count = 1;
  max = basesGetNumBases(&mybs);
  while (count <= max) {
    frontEndStatusBase(count, basesGetStatusNum(&mybs, count));
    count++;
  }
  /* Set our player name in the menu */
  max = playersGetSelf(screenGetPlayers());
  playersGetPlayerName(screenGetPlayers(), max, pn);
  frontEndSetPlayer((playerNumbers) max, pn);
  /* Set The other players in the menu */
  playersSetMenuItems(screenGetPlayers());
}


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
void screenSetPillNetData(BYTE *buff, BYTE dataLen) {
  BYTE count; /* Looping variable */
  BYTE max;   /* Max items */
  
  pillsSetPillNetData(&mypb, buff, dataLen);
  count = 1;
  max = pillsGetNumPills(&mypb);
  while (count <= max) {
    frontEndStatusPillbox(count, pillsGetAllianceNum(&mypb, count));
    count++;
  }
}

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
void screenSetStartsNetData(BYTE *buff, BYTE dataLen) {
  startsSetStartNetData(&myss, buff, dataLen); 
}

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
BYTE screenMakePosInfo(BYTE *buff) {
  BYTE returnValue; /* Value to return */
  WORLD wx;     /* Tank World positions */
  WORLD wy;
  BYTE *ptr;    /* Position in the buffer for adding */
  BYTE high;    /* Nibble high */
  BYTE low;     /* Nibble Low  */
  TURNTYPE ang; /* Tank Angle  */
  SPEEDTYPE ts; /* Tank speed  */
  BYTE health;
  BYTE dummy;
/*  BYTE lgmMX;
  BYTE lgmMY; */

  returnValue = 0;
  ptr = buff;
  high = playersGetSelf(screenGetPlayers());
  low = tankGetDir(&mytk);
  *ptr = utilPutNibble(high, low);
  ptr++;
  returnValue++;
  /* Tank position */
  tankGetStats(&mytk, &high, &low, &health, &dummy);
  if (health > TANK_FULL_ARMOUR) {
    /* Tank is dead */
    wx = 0;
    wy = 0;
  } else {
    tankGetWorld(&mytk, &wx, &wy);
  }
  memcpy(ptr, &wx, sizeof(WORLD));
  returnValue += sizeof(WORLD);
  ptr += sizeof(WORLD);
  memcpy(ptr, &wy, sizeof(WORLD));
  returnValue += sizeof(WORLD);
  ptr += sizeof(WORLD);
  /* Tank Angle */
  ang = tankGetAngle(&mytk);
  returnValue += sizeof(TURNTYPE);
  memcpy(ptr, &ang, sizeof(TURNTYPE));
  ptr += sizeof(TURNTYPE);
  /* Speed */
  ts = tankGetActualSpeed(&mytk);
  returnValue += sizeof(SPEEDTYPE);
  memcpy(ptr, &ts, sizeof(SPEEDTYPE));
  ptr += sizeof(SPEEDTYPE);

  high = tankIsOnBoat(&mytk);
  

/*  lgmMX = lgmGetMX(&mylgman);
  lgmMY = lgmGetMY(&mylgman); */
  /* Lgm position */
/*  if (lgmMX != 0 && lgmMY != 0) {
    low = lgmGetFrame(&mylgman);
    *ptr = utilPutNibble(high, low);
    ptr++;
    returnValue++;

    *ptr = lgmMX;
    ptr++;
    returnValue++;
    *ptr = lgmGetMY(&mylgman);
    ptr++;
    returnValue++;
    *ptr = utilPutNibble(lgmGetPX(&mylgman), lgmGetPY(&mylgman));
    ptr++;
    returnValue++;
  } else { */
    *ptr = utilPutNibble(high, 0xF);
    ptr++;
    returnValue++;
/*  } */

 
  return returnValue;
}

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
BYTE screenMakeMapTokenData(BYTE *buff) {
  return mapNetClientPacket(&mymp, buff);
}

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
void screenSetMapNetRun(BYTE *buff, BYTE yPos, int dataLen) {
  mapSetNetRun(&mymp, buff, yPos, dataLen);
}

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
void screenExtractPlayerData(BYTE *buff, int buffLen) {
  BYTE pos;       /* Position in the buffer */
  BYTE playerNum; /* Player number */
  BYTE frame;     /* Animation frame */
  BYTE mx;        /* Map X and Y position */
  BYTE my;
  BYTE px;        /* Pixel X and Y position */
  BYTE py;
  bool onBoat;    /* Is player on boat */
  BYTE lgmMX;     /* Lgm Data */
  BYTE lgmMY;
  BYTE lgmPX;
  BYTE lgmPY;
  BYTE lgmFrame;
  BYTE myPlayerNum;
  bool tankInView;
  bool lgmInView;
  BYTE lgmObstructed;
  BYTE options;

  tankInView = FALSE;
  lgmInView = FALSE;
  pos = 0;
  myPlayerNum = playersGetSelf(screenGetPlayers());
  while (pos < buffLen) {
    mx = my = px = py = frame = onBoat = lgmMX = lgmMY = lgmPX = lgmPY = lgmFrame = 0;
    utilGetNibbles(buff[pos], &playerNum, &options);
    pos++;
    if (options != 0xF) { /* Stale check */
      tankInView = options >> 2;
      tankInView = tankInView  & 1;
      lgmInView = options & 1;
      if (tankInView == TRUE) {
        /* Tank position */
        mx = buff[pos];
        pos++;
        my = buff[pos];
        pos++;
        utilGetNibbles(buff[pos], &px, &py);
        pos++;
        utilGetNibbles(buff[pos], &onBoat, &frame);
        pos++;
      }

      if (lgmInView == TRUE) {
        /* Lgm position */
        lgmMX = buff[pos];
        pos++;
        lgmMY =  buff[pos];
        pos++;
        utilGetNibbles(buff[pos], &lgmPX, &lgmPY);
        pos++;
        utilGetNibbles(buff[pos], &lgmFrame , &lgmObstructed);
        pos++;
      }
    }
  
    /* Update Players */
    if (myPlayerNum != playerNum) {
      playersUpdate(screenGetPlayers(), playerNum, mx, my, px, py, frame, onBoat, lgmMX, lgmMY, lgmPX, lgmPY, lgmFrame);
    } else if (lgmInView == TRUE) {
      /* Put our lgm values in */
      WORLD wx;
      WORLD wy;
      wx = (WORLD) ((lgmMX << 8) + (lgmPX << 4));
      wy = (WORLD) ((lgmMY << 8) + (lgmPY << 4));
      lgmPutWorld(&mylgman, wx, wy, lgmFrame);
      lgmSetBrainObstructed(&mylgman, lgmObstructed);
    }
  }
}

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
void screenExtractMapData(BYTE *buff, BYTE len, BYTE yPos) {
  BYTE pos;     /* Position through the data */
  BYTE mx;      /* Map X position */
  BYTE my;      /* Map Y position */
  BYTE terrain; /* Terrain position */

  len--;
  pos = 0;
  while (pos < len) {
    mx = buff[pos];
    pos++;
    my = buff[pos];
    pos++;
    terrain = buff[pos];
    pos++;
    if (my < yPos) {
      /* Update map with it */
      mapNetIncomingItem(&mymp, mx, my, terrain);
    }
  }
  screenReCalc();
}

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
BYTE screenMakeShellData(BYTE *buff) {
  return shellsNetMake(&myshs, buff, 0xFF, TRUE);
}

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
void screenExtractShellData(BYTE *buff, BYTE dataLen) {
  shellsNetExtract(&myshs, &mypb, buff, dataLen, FALSE);
}

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
bool screenExtractPNBData(BYTE *buff, BYTE dataLen, bool isTcp) {
  return netPNBExtract(&clientPNB, &mymp, &mybs, &mypb, buff, dataLen, isTcp);
}

/*********************************************************
*NAME:          screenExtractMNTData
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED:  8/1/00
*PURPOSE:
*  Extracts netMNT data from a network packet
*
*ARGUMENTS:
*  buff   - Pointer that holds Packet data
*  datLen - Length of the packet
*  isTcp  - Is this TCP data
*********************************************************/
bool screenExtractMNTData(BYTE *buff, BYTE dataLen, bool isTcp) {
  return netMNTExtract(&clientNMT, &mymp, &mypb, &mybs, &mytk, buff, dataLen, isTcp);
}

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
BYTE screenMakeTKData(BYTE *buff) {
  return tkExplosionNetMake(&clientTankExplosions, buff);
}

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
void screenExtractTKData(BYTE *buff, BYTE dataLen) {
  tkExplosionNetExtract(&clientTankExplosions, buff, dataLen, playersGetSelf(screenGetPlayers()));
}


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
void screenLeaveGame(void) {

  /* Only have to do this if not a single player game */
  if (threadsGetContext() == TRUE) {
    tankDropPills(&mytk, &mymp, &mypb, &mybs);
//    netMNTAdd(NMNT_QUIT, 0, playersGetSelf(), 0, 0);
//    netCheckLast();
  }
}

void screenIncomingMessage(BYTE playerNum, char *messageStr) {
  char topLine[FILENAME_MAX];       /* The message topline */

  topLine[0] = '\0';
  playersMakeMessageName(screenGetPlayers(), playerNum, topLine);
  messageAdd((messageType) (playerNum + PLAYER_MESSAGE_OFFSET), topLine, messageStr);
}

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
void screenTogglePlayerCheckState(BYTE playerNum) {
  playersToggleCheckedState(screenGetPlayers(), playerNum);
}

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
void screenCheckAllNonePlayers(bool isChecked) {
  playersCheckAllNone(screenGetPlayers(), isChecked);
}

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
void screenCheckAlliedPlayers(void) {
  playersCheckAllies(screenGetPlayers());
}

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
void screenCheckNearbyPlayers(void) {
  playersCheckNearbyPlayers(screenGetPlayers(), tankGetMX(&mytk), tankGetMY(&mytk));
}

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
int screenNumCheckedPlayers(void) {
  return playersGetNumChecked(screenGetPlayers());
}

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
int screenNumAllies(void) {
  return playersGetNumAllies(screenGetPlayers());
}

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
int screenNumNearbyTanks(void) {
  return playersNumNearbyPlayers(screenGetPlayers(), tankGetMX(&mytk), tankGetMY(&mytk));
}

/*********************************************************
*NAME:          screenSendMessageAllAllies
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends a message to all allied players
*
*ARGUMENTS:
*  message - The message to send
*********************************************************/
void screenSendMessageAllAllies(char *messageStr) {
  char topLine[FILENAME_MAX];       /* The message topline */

  topLine[0] = '\0';
  playersMakeMessageName(screenGetPlayers(), playersGetSelf(screenGetPlayers()), topLine);
  messageAdd((messageType) (playersGetSelf(screenGetPlayers()) + PLAYER_MESSAGE_OFFSET), topLine, messageStr);
  playersSendMessageAllAllies(screenGetPlayers(), messageStr);
}

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
void screenSendMessageAllSelected(char *messageStr) {
  playersSendMessageAllSelected(screenGetPlayers(), messageStr);
}

/*********************************************************
*NAME:          screenSendMessageAllNearby
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends a message to all nearby players
*
*ARGUMENTS:
*  message - The message to send
*********************************************************/
void screenSendMessageAllNearby(char *messageStr) {
  char topLine[FILENAME_MAX];       /* The message topline */

  topLine[0] = '\0';
  playersMakeMessageName(screenGetPlayers(), playersGetSelf(screenGetPlayers()), topLine);
  messageAdd((messageType) (playersGetSelf(screenGetPlayers()) + PLAYER_MESSAGE_OFFSET), topLine, messageStr);
  playersSendMessageAllNearby(screenGetPlayers(), tankGetMX(&mytk), tankGetMY(&mytk), messageStr);
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
bool clientMapIsMine(BYTE bmx, BYTE bmy) {
  return mapIsMine(&mymp, bmx, bmy);
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
  return &mytk;
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
  return playersGetSelf(screenGetPlayers());
}

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
void screenRequestAlliance() {
  playersRequestAlliance(screenGetPlayers());
}

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
void screenLeaveAlliance() {
  netLeaveAlliance(playersGetSelf(screenGetPlayers()));
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
  basesMigrate(&mybs, oldOwner, playersGetSelf(screenGetPlayers()));
  pillsMigrate(&mypb, oldOwner, playersGetSelf(screenGetPlayers()));
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
  if (isLeft == TRUE) {
    xOffset--;
  } else {
    xOffset++;
  }
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
  if (isUp == TRUE) {
    yOffset--;
  } else {
    yOffset++;
  }
}


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
bool screenTankIsDead() {
  bool returnValue;
  BYTE high, low, health, dummy;

  returnValue = FALSE;
  tankGetStats(&mytk, &high, &low, &health, &dummy);
  if (health > TANK_FULL_ARMOUR) {
    /* Tank is dead */
    returnValue = TRUE;
  }
  return returnValue;
}

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
void screenGetLgmStatus(bool *isOut, bool *isDead, TURNTYPE *angle) {
  lgmGetStatus(&mylgman, &mytk, isOut, isDead, angle);
}

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
bool screenTankScroll() {
  BYTE x;  /* Tank X and Y Co-ordinated       */
  BYTE y;
  BYTE px; /* Pixel X and Y co-ords of the tank */
  BYTE py;

  x = tankGetScreenMX(&mytk);
  y = tankGetScreenMY(&mytk);
  px = tankGetScreenPX(&mytk);
  py = tankGetScreenPY(&mytk);
/*  if (px >3 || (x - xOffset -1) == 0) {
    x++;
  }
  if (py >2) {
    y++;
  } */
  return scrollManual(&xOffset, &yOffset, x, y, (TURNTYPE) tankGetTravelAngel(&mytk));
}

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
void screenGetSubMapSquareOffset(int *xPos, int *yPos) {
  *xPos = 0; /* edgeX; */
  *yPos = 0; /* edgeY; */
}

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
void screenMakeBrainViewData(BYTE *buff, BYTE leftPos, BYTE rightPos, BYTE topPos, BYTE bottomPos) {
  BYTE count1; /* Looping variable */
  BYTE count2; /* Looping variable */
  BYTE pos;    /* Upto position    */

  pos = 0;
  for (count1=topPos;count1<=bottomPos;count1++) {
    for (count2=leftPos;count2<=rightPos;count2++) {
      if (basesExistPos(&mybs, count2, count1) == TRUE) {
        buff[pos] = BREFBASE_T;
      } else if (pillsExistPos(&mypb, count2, count1) == TRUE) {
        buff[pos] = BPILLBOX_T;
      } else {
        buff[pos] = mapGetPos(&mymp, count2, count1);
        if (buff[pos] == DEEP_SEA) {
          buff[pos] = BDEEPSEA;
        } else if (buff[pos] >= MINE_START && buff[pos] <= MINE_END) {
          buff[pos] = buff[pos] - MINE_SUBTRACT;
        }
        if (minesExistPos(&clientMines, count2, count1) == TRUE) {
          buff[pos] |= TERRAIN_MINE;
        }
      }
      pos++;
    }
  }
}

/*********************************************************
*NAME:          screenMakeBrainInfo
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 28/11/99
*PURPOSE:
*  Makes the information for a particular brain pass
*
*ARGUMENTS:
*  value - Pointer to the brain info structure
*  first - TRUE if this is the first time we have been
*          called
*********************************************************/
void screenMakeBrainInfo(BrainInfo *value, bool first) {
  BYTE tx;        /* Tank X and Y Co-ordinates */
  BYTE ty; 
  BYTE closeBase; /* The closest base to our current position */

  tx = tankGetMX(&mytk);
  ty = tankGetMY(&mytk);

  /* Max's */
  value->max_players = MAX_TANKS;//-1; /* FIXME: Huh? */
  value->max_refbases = basesGetNumBases(&mybs);//-1;
  value->max_pillboxes = pillsGetNumPills(&mypb);//-1;
  value->player_number = playersGetSelf(screenGetPlayers());
  value->num_players = playersGetNumPlayers(screenGetPlayers());
  value->playernames = playersGetBrainsNamesArray(screenGetPlayers());
  value->allies = malloc(sizeof(PlayerBitMap));
  *(value->allies) = playersGetAlliesBitMap(screenGetPlayers(), playersGetSelf(screenGetPlayers()));
  
  /* Tank */
  tankGetWorld(&mytk, &(value->tankx), &(value->tanky));
  value->direction = tankGet256Dir(&mytk);
  value->speed = (BYTE) (tankGetSpeed(&mytk) * 4);
  value->inboat = tankIsOnBoat(&mytk);
  value->hidden = utilIsTankInTrees(&mymp, &mypb, &mybs, value->tankx, value->tanky);

  tankGetStats(&mytk, &(value->shells), &(value->mines), &(value->armour), &(value->trees));
  value->armour /= 5; /* Has to be between 0-8 */

  
  value->carriedpills = tankGetNumCarriedPills(&mytk);
  value->carriedbases = 0;
  
  value->gunrange = tankGetGunsightLength(&mytk);
  value->reload = tankGetReloadTime(&mytk);
  if (first == TRUE) {
    /* Reset stuff */
    brainHoldKeys = 0;
    brainTapKeys = 0;
    brainsWantAllies = 0;
    brainsMessageDest = 0;
    brainsMessage[0] = '\0';
    /* Set tank */
    value->newtank = TRUE;
  } else {
    value->newtank = tankIsNewTank(&mytk);
  }
  value->tankobstructed = tankIsObstructed(&mytk);

  /* Base nearby */
  closeBase = basesGetClosest(&mybs, value->tankx, value->tanky);
  if (closeBase == BASE_NOT_FOUND) {
    value->base = NULL;
  } else {
    value->base = (ObjectInfo*) malloc(sizeof(ObjectInfo));
    value->base->object = OBJECT_REFBASE;
    value->base->idnum = closeBase;
    basesGetBrainBaseItem(&mybs, closeBase, &(value->base->x), &(value->base->y), &(value->base->info), &(value->base_shells), &(value->base_mines), &(value->base_armour));
    value->base->direction = value->base_armour;
  }

  /* Lgm */
  value->man_status = lgmGetBrainState(&mylgman);
  value->man_direction = lgmGetDir(&mylgman, &mytk);
  value->man_x = lgmGetWX(&mylgman);
  value->man_y = lgmGetWY(&mylgman);
  value->manobstructed = lgmGetBrainObstructed(&mylgman);

  /* Pillview */
  value->pillview = malloc(sizeof(WORD));
  if (inPillView == TRUE) {
    *(value->pillview) = pillsGetPillNum(&mypb, pillViewX, pillViewY, FALSE, FALSE) -1;
    value->view_left = pillViewX-7;
    value->view_width = 15; 
    value->view_top = pillViewY-7;
    value->view_height = 15;
  } else {
    *(value->pillview) = 0x8000;
    value->view_left = tx-14;
    value->view_width = 29;
    value->view_top = ty-14;
    value->view_height = 29; 
  }
  //value->viewdata = malloc((value->view_width+1) * (value->view_height+1));
  value->viewdata = malloc(30 * 30);
  screenMakeBrainViewData(value->viewdata, value->view_left, (BYTE) (value->view_left+value->view_width), value->view_top, (BYTE) (value->view_top+value->view_height));

  /* From Bolo Version History:
  Added option to give Brains an advantage to make them more 
  challenging opponents. With this option enabled, Bolo tells 
  Brains the location of every base, pillbox and tank on the map 
  even if they are out of visual range. (Use with caution in 
  public games -- cyborgs currently get the same advantage.) */

  value->gameinfo.allow_AI = TRUE;
  value->gameinfo.assist_AI = FALSE;
  value->objects = brainObjects;;
  if (allowComputerTanks  == aiYesAdvantage) {
    value->gameinfo.assist_AI = TRUE;
    basesGetBrainBaseInRect(&mybs, 0, 255, 0, 255);
    pillsGetBrainPillsInRect(&mypb, 0, 255, 0, 255);
    shellsGetBrainShellsInRect(&myshs, value->view_left, (BYTE) (value->view_left+value->view_width), value->view_top, (BYTE) (value->view_top+value->view_height));
    playersGetBrainTanksInRect(screenGetPlayers(), value->view_left, (BYTE) (value->view_left+value->view_width), value->view_top, (BYTE) (value->view_top+value->view_height), value->tankx, value->tanky);
    playersGetBrainLgmsInRect(screenGetPlayers(), value->view_left, (BYTE) (value->view_left+value->view_width), value->view_top, (BYTE) (value->view_top+value->view_height));
  } else {
    /* Must be aiYes else we wouldn't be called would we? */
    basesGetBrainBaseInRect(&mybs, value->view_left, (BYTE) (value->view_left+value->view_width), value->view_top, (BYTE) (value->view_top+value->view_height));
    pillsGetBrainPillsInRect(&mypb, value->view_left, (BYTE) (value->view_left+value->view_width), value->view_top, (BYTE) (value->view_top+value->view_height));
    shellsGetBrainShellsInRect(&myshs, value->view_left, (BYTE) (value->view_left+value->view_width), value->view_top, (BYTE) (value->view_top+value->view_height));
    playersGetBrainTanksInRect(screenGetPlayers(), value->view_left, (BYTE) (value->view_left+value->view_width), value->view_top, (BYTE) (value->view_top+value->view_height), value->tankx, value->tanky);
    playersGetBrainLgmsInRect(screenGetPlayers(), value->view_left, (BYTE) (value->view_left+value->view_width), value->view_top, (BYTE) (value->view_top+value->view_height));
  }
  value->num_objects = brainsNumObjects;
  brainsNumObjects = 0;

  /* Message */
  if (messageIsNewMessage() == TRUE) {
    unsigned long *ul;
    value->message = (MessageInfo*) malloc(sizeof(MessageInfo));
    value->message->receivers = malloc(sizeof(value->message->receivers));
    value->message->message = malloc(512);
    ul = value->message->receivers;
    value->message->sender = messageGetNewMessage((char *) value->message->message, &(value->message->receivers)); /* FIXME: Second parameter? */
  } else {
    value->message = NULL;
  }

  /* Controling the tank */
  value->holdkeys = &brainHoldKeys;
  value->tapkeys = &brainTapKeys;

  /* Building */
  value->build = brainBuildInfo;

  /* Allies */
  //FIXME!!!!
  brainsWantAllies = *(value->allies);
  value->wantallies = &brainsWantAllies;

  /* Message Sending */
  value->messagedest = &brainsMessageDest;
  brainsMessage[0] = '\0';
  value->sendmessage = (u_char *) brainsMessage;

  /* Game World */
  value->theWorld = screenBrainMapGetPointer();

  /* Game Info */
  strcpy(((char *) &(value->gameinfo.mapname)), mapName);
  value->gameinfo.gametype = gameTypeGet(&myGame);
  value->gameinfo.start_delay = gmeStartDelay;
  value->gameinfo.time_limit = gmeLength;

  if (minesGetAllowHiddenMines(&clientMines) == TRUE) {
    value->gameinfo.hidden_mines = GAMEINFO_HIDDENMINES;
  } else {
    value->gameinfo.hidden_mines = GAMEINFO_ALLMINES_VISIBLE;
  }
  value->gameinfo.gameid.start_time = (unsigned long) timeStart;
  netClientGetServerAddress(&(value->gameinfo.gameid.serveraddress), &(value->gameinfo.gameid.serverport));
}

/*********************************************************
*NAME:          screenExtractBrainInfo
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 13/12/99
*PURPOSE:
*  Called after the brain has executed. Extract the data
*  and cleanup
*
*ARGUMENTS:
*  value - Pointer to the brain info structure
*********************************************************/
void screenExtractBrainInfo(BrainInfo *value) {
  BYTE pillNum;

  free(value->allies);
  if (value->base != NULL) {
    free(value->base);
  }
   
  if (*(value->pillview) != 0x8000) {
    pillNum = (BYTE) (*(value->pillview));
    if (pillNum != (pillsGetPillNum(&mypb, pillViewX, pillViewY, FALSE, FALSE)-1)) {
      if (pillsSetView(&mypb, pillNum, playersGetSelf(screenGetPlayers())) == TRUE) {
        /* We can set the new view */
        pillbox p;
        pillsGetPill(&mypb, &p, (BYTE) (pillNum+ 1));
        inPillView = TRUE;
        pillViewX = p.x;
        pillViewY = p.y;
        scrollCenterObject(&xOffset, &yOffset, pillViewX, pillViewY);
        screenReCalc();
      }
    }
  } 

  free(value->pillview);
  free(value->viewdata);
  if (value->message != NULL) {
    free(value->message->receivers);
    free(value->message->message);
    free(value->message);
  }

  /* Controling the tank */
  brainHoldKeys = *(value->holdkeys);
  brainTapKeys = *(value->tapkeys);

  /* Extract building stuff */
  if (value->build->action != 0) {
    if (tankGetArmour(&mytk) <= TANK_FULL_ARMOUR) {
      value->build->action--;
      lgmAddRequest(&mylgman, &mymp, &mypb,  &mybs, &mytk, value->build->x, value->build->y, value->build->action);
      value->build->action = 0;
    }
  }

  /* Allies */
  /* FIXME!!!! Get want allies stuff */
/*  brainsWantAllies = *(value->allies);
    value->wantallies = &brainsWantAllies; */

  /* Message Sending */
  if (value->sendmessage[0] != 0) {
    char msg[255];
    utilPtoCString((char *) value->sendmessage, msg);
    if (*(value->messagedest) == 0) {
      /* Its a debug message */
      messageAdd(AIMessage, langGetText(MESSAGE_AI), msg);
    } else {
      /* Send this message to the appropriate players */
      playersSendAiMessage(screenGetPlayers(), *(value->messagedest), msg);
    }
    brainsMessage[0] = '\0';
  }
}

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
void screenSetAiType(aiType value) {
  allowComputerTanks = value;
}

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
aiType screenGetAiType() {
  return allowComputerTanks;
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
  brainObjects[brainsNumObjects].object = object;
  brainObjects[brainsNumObjects].x = wx;
  brainObjects[brainsNumObjects].y = wy;
  brainObjects[brainsNumObjects].idnum = idNum;
  brainObjects[brainsNumObjects].direction = dir;
  brainObjects[brainsNumObjects].info = info;
  brainsNumObjects++;
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
  inStart = entering;
}


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
bool clientGetInStartFind() {
  return inStart;
}


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
bool screenGetTankAutoSlowdown() {
  return tankGetAutoSlowdown(&mytk);
}

/*********************************************************
*NAME:          screenGetTankAutoSlowdown
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/00
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Sets whether tank autoslowdown is enabled or not
*
*ARGUMENTS:
*  useSlowdown - TRUE if auto slowdown is used
*********************************************************/
void screenSetTankAutoSlowdown(bool useSlowdown) {
  tankSetAutoSlowdown(&mytk, useSlowdown);
}

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
bool screenGetTankAutoHideGunsight() {
  return tankGetAutoHideGunsight(&mytk);
}

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
void screenSetTankAutoHideGunsight(bool useAutohide) {
  tankSetAutoHideGunsight(&mytk, useAutohide);
}


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
void screenSetCursorPos(BYTE posX, BYTE posY) {
/*  char str[255];
  static BYTE oldCursorXPos = 255;
  static BYTE oldCursorYPos = 255; 

  sprintf(str, "pos (%d, %d)", posX, posY); */
  if(posX != 0 && posY != 0) {
/*    if (posX >15  || posY > 15) {
      strcat(str, " bad");
    } */
    cursorPosX = posX;
    cursorPosY = posY;
/*    if (oldCursorXPos != cursorPosX || oldCursorYPos != cursorPosY) {
      oldCursorXPos = cursorPosX;
      oldCursorYPos = cursorPosY;
      messageAdd(globalMessage, "mouse", str);
    } */
  }
}

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
bool screenGetCursorPos(BYTE *posX, BYTE *posY) {
  bool returnValue; /* Value to return */

  returnValue = FALSE;
  if (cursorPosX >= 0 && cursorPosX <= MAIN_SCREEN_SIZE_X && cursorPosY >= 0 && cursorPosY <= MAIN_SCREEN_SIZE_Y) {
    returnValue = TRUE;
    *posX = (BYTE) cursorPosX;
    *posY = (BYTE) cursorPosY;
  } else {
    *posX = 0;
    *posY = 0;
  }

  return returnValue;
}

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
void screenNetStatusMessage(char *messageStr) {
  messageAdd(networkStatus, (char *) "Network Status", messageStr);
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
  tankStopCarryingPill(&mytk, itemNum);
}

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
bool screenGenerateMapPreview(char *fileName, BYTE *buff) {
  bool returnValue; /* Value to return */
  map prevmp;       /* Items in loading */
  bases prevbs;
  pillboxes prevpb;
  starts prevss;
  BYTE xValue;           /* Looping variables */
  BYTE yValue;
  BYTE *ptr;


  /* Create */
  mapCreate(&prevmp);
  startsCreate(&prevss);
  basesCreate(&prevbs);
  pillsCreate(&prevpb);

  /* Load */
  returnValue = mapRead(fileName, &prevmp, &prevpb, &prevbs, &prevss);
  if (returnValue == TRUE) {
    /* Make preview map info */
    xValue = 0;
    yValue = 0;
    ptr = buff;
    /* Set up Items */
    while (yValue < 255) {
      while (xValue < 255) {
        if ((pillsExistPos(&prevpb, xValue, yValue)) == TRUE) {
          *ptr = 17;
        } else if ((basesExistPos(&prevbs, xValue, yValue)) == TRUE) {
          *ptr = 18;
        } else if ((startsExistPos(&prevss, xValue, yValue)) == TRUE) {
          *ptr = 19;
        } else {
          *ptr = mapGetPos(&prevmp, xValue, yValue);
          if (*ptr == DEEP_SEA) {
            *ptr = 16;
          }

        }
        xValue++;
        ptr++;
      }
      xValue = 0;
      yValue++;
    }
  }

  /* Clean up */
  mapDestroy(&prevmp);
  startsDestroy(&prevss);
  basesDestroy(&prevbs);
  pillsDestroy(&prevpb);

  return returnValue;

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
  return &mylgman;
}

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
void screenNetLgmReturn(BYTE numTrees, BYTE numMines, BYTE pillNum) {
  lgmNetBackInTank(&mylgman, &mymp, &mypb, &mybs, &mytk, numTrees, numMines, pillNum);
}

/*********************************************************
*NAME:          lgmNetManWorking
*AUTHOR:        John Morrison
*CREATION DATE: 01/02/03
*LAST MODIFIED: 01/02/03
*PURPOSE:
*  Network message. LGM Back in tank
*
*ARGUMENTS:
*  mapX     - Bless X map position 
*  mapY     - Bless Y map position 
*  numMines - The number of mines
*  pillNum  - Pillbox being carries
*  numTrees - The number of trees
*********************************************************/
void screenNetManWorking(BYTE mapX, BYTE mapY, BYTE numMines, BYTE pillNum, BYTE numTrees) {
  lgmNetManWorking(&mylgman, &mytk, mapX, mapY, numTrees, numMines, pillNum);
}

/*********************************************************
*NAME:          clientGetTankWorldFromLgm
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/00
*LAST MODIFIED: 28/12/00
*PURPOSE:
* Gets the tank world co-ordinates from a mylgman object
* returns own tank co-ordinates in a client game
*
*ARGUMENTS:
*  lgmans - Pointer to the lgm structure
*  x      - Pointer to hold World X Co-ordinates
*  y      - Pointer to hold World Y Co-ordinates
*********************************************************/
void clientGetTankWorldFromLgm(lgm *lgmans, WORLD *x, WORLD *y) {
  tankGetWorld(&mytk, x, y);
}

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
void screenSetTankStartPosition(BYTE xValue, BYTE yValue, TURNTYPE angle, BYTE numShells, BYTE numMines) {
  BYTE numTrees;
  tankSetLocationData(&mytk, (WORLD) ((xValue << TANK_SHIFT_MAPSIZE ) + MAP_SQUARE_MIDDLE), (WORLD) ((yValue << TANK_SHIFT_MAPSIZE ) + MAP_SQUARE_MIDDLE), (TURNTYPE) angle, (SPEEDTYPE) 0, (bool) TRUE);
  numTrees = 0;
  if (gameTypeGet(&myGame) == gameOpen) {
    numTrees = TANK_FULL_TREES;
  }
  tankSetStats(&mytk, numShells, numMines, TANK_FULL_ARMOUR, numTrees);
  frontEndUpdateTankStatusBars(numShells, numMines, TANK_FULL_ARMOUR, numTrees);
  screenTankView();
  clientSetInStartFind(FALSE);
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
  /* We are a single player game. This will always be TRUE */
  return TRUE;
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
  bool returnValue; /* Value to return  */
  BYTE count;       /* Looping variable */
  BYTE total;
  pillbox pill;
  WORLD startX;
  WORLD startY;
  WORLD testX;
  WORLD testY;
  double dummy;
  

  returnValue = TRUE;
  startX = (xValue << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE;
  startY = (yValue << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE;
  count = 1;
  total = pillsGetNumPills(&mypb);
  while (returnValue == TRUE && count <= total) {
    pillsGetPill(&mypb, &pill, count);
    if (playersIsAllie(screenGetPlayers(), playerNum, pill.owner) == FALSE) {
      testX = (pill.x << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE;
      testY = (pill.y << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE;
      returnValue = !(utilIsItemInRange(startX, startY, testX, testY, (WORLD) distance, &dummy));
    }
    count++;
  }

  return returnValue;
}

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
void screenSetPlayersMenu() {
  playersSetPlayersMenu(screenGetPlayers());
}

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
bool screenGetGameRunning() {
  return screenGameRunning;
} 

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
void screenConnectionLost() {
  lgmConnectionLost(&mylgman, &mytk);
  playersConnectionLost(screenGetPlayers());
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
  return TRUE;
}

//FIXME
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
    logMessageType = log_SoundExplosion;
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
  return &plyrs;
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
  return &clientBlds;
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
  return &clientExpl;
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
  return &clientFF;
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
  return &clientGrass;
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
  return &clientMines;
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
  return &clientMinesExp;
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
  return &clientRubble;
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
  return &clientSwamp;
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
  return &clientTankExplosions;
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
  return  &clientPNB;
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
  return  &clientNMT;
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
    return &myGame;
}

void screenSendMem() {
  netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 0xFF , 0xFF);
}
