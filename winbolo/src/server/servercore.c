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
*Filename:      serverCore.c
*Author:        John Morrison
*Creation Date: 10/08/99
*Last Modified: 27/07/04
*Purpose:
*  WinBolo Server Core Simulation modelling.
*********************************************************/

#include <string.h>
#include <time.h>

#include "../bolo/global.h"
#include "../bolo/bolo_map.h"
#include "../bolo/pillbox.h"
#include "../bolo/bases.h"
#include "../bolo/starts.h"
#include "../bolo/tank.h"
#include "../bolo/shells.h"
#include "../bolo/rubble.h"
#include "../bolo/explosions.h"
#include "../bolo/messages.h"
#include "../bolo/grass.h"
#include "../bolo/swamp.h"
#include "../bolo/building.h"
#include "../bolo/tilenum.h"
#include "../bolo/screencalc.h"
#include "../bolo/tankexp.h"
#include "../bolo/scroll.h"
#include "../bolo/lgm.h"
#include "../bolo/floodfill.h"
#include "../bolo/minesexp.h"
#include "../bolo/mines.h"
#include "../bolo/netpnb.h"
#include "../bolo/netmt.h"
#include "../bolo/labels.h"
#include "../bolo/playersrejoin.h"
#include "../bolo/players.h"
#include "../bolo/players.h"
#include "../bolo/network.h"
#include "../bolo/screen.h"
#include "../bolo/frontend.h"
#include "../bolo/log.h"
#include "../winbolonet/winbolonet.h"
#include "servernet.h"
#include "servercore.h"

#ifdef _WIN32
#define BOLO_VERSION_STRING "WinBolo Server - v1.15 (09/04/06)\0"
#define COPYRIGHT_STRING "WinBolo Copyright 1998-2006 John Morrison\nBolo Copyright 1987-1995 Stuart Cheshire\0"
#else 
#define BOLO_VERSION_STRING "LinBolo Server - v1.15 (21/04/06)\0"
#define COPYRIGHT_STRING "LinBolo Copyright 1998-2006 John Morrison\nBolo Copyright 1987-1995 Stuart Cheshire\0"
#endif

map mp;
bases bs;
pillboxes pb;
starts ss;
tank tk[MAX_TANKS];
lgm lgman[MAX_TANKS];
shells shs;
players splrs;
building serverBlds = NULL;
explosions serverExpl = NULL;
floodFill serverFF = NULL;
grass serverGrass = NULL;
mines serverMines = NULL;
minesExp serverMinesExp = NULL;
rubble serverRubble = NULL;
swamp serverSwamp = NULL;
tkExplosion serverTankExp = NULL;
netPnbContext serverPNB = NULL; 
netMntContext serverNMT = NULL;

gameType sGame;

/* Map name */
char sMapName[MAP_STR_SIZE];

/* Game length and start delay */
int sGmeStartDelay;
long sGmeLength;

/* Time game was created */
time_t sTimeStart;

/* Are we running? */
bool serverCoreGameRunning = FALSE;

void serverMessagesSetLogFile(char *logFile);

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
bool serverCoreCreate(char *fileName, gameType game, bool hiddenMines, int srtDelay, long gmeLen) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */

  srand((unsigned int) time(NULL));
  messageAdd(globalMessage, (char *) BOLO_VERSION_STRING, (char *) COPYRIGHT_STRING);
  messageAdd(globalMessage, (char *) "", (char *) "Core Simulation Startup");

  sMapName[0] = '\0';
  sGmeStartDelay = srtDelay;
  sGmeLength = gmeLen;

  for (count=0;count<MAX_TANKS;count++) {
    tk[count] = NULL;
  }

  gameTypeSet(&sGame, game);
  mapCreate(&mp);
  pillsCreate(&pb);
  startsCreate(&ss);
  basesCreate(&bs);
  playersCreate(&splrs);
  shs = shellsCreate();
  explosionsCreate(&serverExpl);
  rubbleCreate(&serverRubble);
  buildingCreate(&serverBlds);
  grassCreate(&serverGrass);
  swampCreate(&serverSwamp);
  netPNBCreate(&serverPNB);
  netMNTCreate(&serverNMT);
  logCreate();
  floodCreate(&serverFF);
  tkExplosionCreate(&serverTankExp);
  minesCreate(&serverMines, hiddenMines);
  minesExpCreate(&serverMinesExp);
  playersRejoinCreate();

  returnValue = mapRead(fileName, &mp, &pb, &bs, &ss);

  if (returnValue == TRUE) {
    utilExtractMapName(fileName, sMapName);
    basesClearMines(&bs, &mp);
    serverCoreGameRunning = TRUE;
  } else {
    serverCoreDestroy();
  }

  return returnValue;
}

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
bool serverCoreCreateCompressed(BYTE *buff, int buffLen, char *mapn, gameType game, bool hiddenMines, int srtDelay, long gmeLen) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */

  messageAdd(globalMessage, (char *) BOLO_VERSION_STRING, (char *) COPYRIGHT_STRING);
  messageAdd(globalMessage, (char *) "", (char *) "Core Simulation Startup");

  sMapName[0] = '\0';
  sGmeStartDelay = srtDelay;
  sGmeLength = gmeLen;

  for (count=0;count<MAX_TANKS;count++) {
    tk[count] = NULL;
    lgman[count] = NULL;
  }

  gameTypeSet(&sGame, game);
  mapCreate(&mp);
  pillsCreate(&pb);
  startsCreate(&ss);
  basesCreate(&bs);
  playersCreate(&splrs);
  playersRejoinCreate();
  shs = shellsCreate();
  explosionsCreate(&serverExpl);
  rubbleCreate(&serverRubble);
  buildingCreate(&serverBlds);
  grassCreate(&serverGrass);
  swampCreate(&serverSwamp);
  floodCreate(&serverFF);
  tkExplosionCreate(&serverTankExp);
  netPNBCreate(&serverPNB);
  netMNTCreate(&serverNMT);
  minesCreate(&serverMines, hiddenMines);
  minesExpCreate(&serverMinesExp);
  logCreate();

  returnValue = mapLoadCompressedMap(&mp, &pb, &bs, &ss, buff, buffLen);

  if (returnValue == TRUE) {
    strcpy(sMapName, mapn);
    basesClearMines(&bs, &mp);
    serverCoreGameRunning = TRUE;
  } else {
    serverCoreDestroy();
  }

  return returnValue;
}

/*********************************************************
*NAME:          serverCoreDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 10/8/99
*LAST MODIFIED: 22/6/00
*PURPOSE:
*  Shuts down the server.
*
*ARGUMENTS:
*
*********************************************************/
void serverCoreDestroy() {
  BYTE count; /* Looping variable */

  messageAdd(globalMessage, (char *) "Core Simulation Shutdown", (char *) "");
  serverCoreGameRunning = FALSE;
  for (count=0;count<MAX_TANKS;count++) {
    tankDestroy(&tk[count], &mp, &pb, &bs);
    lgmDestroy(&lgman[count]);
  }

  mapDestroy(&mp);
  pillsDestroy(&pb);
  startsDestroy(&ss);
  basesDestroy(&bs);
  shellsDestroy(&shs);
  explosionsDestroy(&serverExpl);
  playersRejoinDestroy();
  rubbleDestroy(&serverRubble);
  buildingDestroy(&serverBlds);
  grassDestroy(&serverGrass);
  floodDestroy(&serverFF);
  swampDestroy(&serverSwamp);
  netPNBDestroy(&serverPNB);
  netNMTDestroy(&serverNMT);
  tkExplosionDestroy(&serverTankExp);
  minesDestroy(&serverMines);
  minesExpDestroy(&serverMinesExp);
  logDestroy();
  playersDestroy(&splrs);
}

/* FIXME: Comment */
void serverCoreLogTick() {
  if (logIsRecording() == TRUE) {
    BYTE temp1, temp2, temp3, mx, my;
    int count, entries;
    screenBullets sb = screenBulletsCreate();
    /* Item locations */
    for (count=0;count<MAX_TANKS;count++) {
      if (tk[count] != NULL) {
        mx = tankGetMX(&tk[count]);
        my = tankGetMY(&tk[count]);
        logAddEvent(log_PlayerLocation, (BYTE) count, mx, my, utilPutNibble(tankGetPX(&tk[count]), tankGetPY(&tk[count])), (BYTE) utilPutNibble(tankGetDir(&tk[count]), tankIsOnBoat(&tk[count])), NULL);
        if (lgmIsOut(&lgman[count]) == TRUE) {
          logAddEvent(log_LgmLocation, utilPutNibble((BYTE) count, lgmGetFrame(&lgman[count])), lgmGetMX(&lgman[count]), lgmGetMY(&lgman[count]), utilPutNibble(lgmGetPX(&lgman[count]), lgmGetPY(&lgman[count])), 0, NULL);
        }
      }
    }

    /* Shells */
    shellsCalcScreenBullets(&shs, &sb, 0, MAP_ARRAY_LAST, 0, MAP_ARRAY_LAST);
    explosionsCalcScreenBullets(&serverExpl, &sb, 0, MAP_ARRAY_LAST, 0, MAP_ARRAY_LAST);
    tkExplosionCalcScreenBullets(&serverTankExp, &sb, 0, MAP_ARRAY_LAST, 0, MAP_ARRAY_LAST);
    entries = screenBulletsGetNumEntries(&sb);
    for (count=1;count<=entries;count++) {
      screenBulletsGetItem(&sb, count, &mx, &my, &temp1, &temp2, &temp3);
      logAddEvent(log_Shell, mx, my, utilPutNibble(temp1, temp2), temp3, 0, NULL);
    }
    screenBulletsDestroy(&sb);
  }  
  logWriteTick();
}


/*********************************************************
*NAME:          serverCoreGameTick
*AUTHOR:        John Morrison
*CREATION DATE: 10/8/99
*LAST MODIFIED: 3/10/00
*PURPOSE:
*  Called once every game tick to update the game world
*
*ARGUMENTS:
*  
*********************************************************/
void serverCoreGameTick() {
  static int tickCount = 0; /* Tick Count */
  BYTE count;          /* Looping variable */
  tank ta[MAX_TANKS];  /* Array of tanks   */
  lgm lgms[MAX_TANKS]; /* Array of lgms    */
  BYTE numTanks;       /* Number of tanks  */
  

  if (serverCoreGameRunning == FALSE ) {
    return;
  }
  if (sGmeStartDelay > 0) {
    sGmeStartDelay--;
    return;
  }

  if (sGmeLength > 0) {
    sGmeLength--;
    if (sGmeLength == 0) {
      /* Game Times Up. */
      serverCoreGameRunning = FALSE;
      frontEndGameOver();
      serverCoreGameRunning = FALSE;
      return;
    }
  }

 
  pillsUpdate(&pb, &mp, &bs, NULL, &shs);
  basesUpdate(&bs, NULL);
  
  numTanks = 0;
  for (count=0;count<MAX_TANKS;count++) {
    if (tk[count] != NULL) {
      tankUpdate(&(tk[count]), &mp, &bs, &pb, &shs, &ss, TNONE, FALSE, FALSE);
      lgmUpdate(&lgman[count], &mp, &pb, &bs, &tk[count]);
      ta[numTanks] = tk[count];
      lgms[numTanks] = (struct lgmObj *) (&(lgman[count])); /* FIXME: Yeah bigass type cast warning there */
      numTanks++;
    }
  } 
  
  tkExplosionUpdate(&serverTankExp, &mp, &pb, &bs, (lgm **) lgms, numTanks, &tk);
  shellsUpdate(&shs, &mp, &pb, &bs, ta, numTanks, TRUE);
  explosionsUpdate(&serverExpl);
  minesExpUpdate(&serverMinesExp, &mp, &pb, &bs, (lgm **) lgms, numTanks); 
  floodUpdate(&serverFF, &mp, &pb, &bs);
  playersRejoinUpdate();

  /* Prepare log entries if required */
  serverCoreLogTick();
  /* tkExplosionUpdate(&mp, &pb, &bs);
  shellsUpdate(&shs, &mp, &pb, &bs, &tk);
  explosionsUpdate();
  minesExpUpdate(&mp); 
  floodUpdate(&mp, &pb, &bs);  */

  tickCount++;
  if (tickCount == 20 * 30) {
    tickCount = 0;
    logWriteSnapshot(&mp, &pb, &bs, &ss, &splrs, TRUE);
  }
}

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
long serverCoreGetTimeGameCreated(void) {
  return (long) sTimeStart;
}

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
void serverCoreSetTimeGameCreated(long value) {
  sTimeStart = value;
}

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
void serverCoreGetMapName(char *value) {
  strcpy(value, sMapName);
}

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
gameType serverCoreGetActualGameType() {
  return gameTypeGet(&sGame);
}

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
bool serverCoreGetAllowHiddenMines() {
  return minesGetAllowHiddenMines(&serverMines);
}

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
long serverCoreGetGameStartDelay() {
  return sGmeStartDelay;
}

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
long serverCoreGetGameTimeLeft() {
  return sGmeLength;
}

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
BYTE serverCoreGetNumPlayers() {
  return playersGetNumPlayers(&splrs);
}

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
BYTE serverCoreGetNumNeutralPills(void) {
  return pillsGetNumNeutral(&pb);
}


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
BYTE serverCoreGetNumNeutralBases(void) {
  return basesGetNumNeutral(&bs);
}

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
BYTE serverCoreGetBaseNetData(BYTE *buff) {
  return basesGetBaseNetData(&bs, buff);
}

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
BYTE serverCoreGetPillNetData(BYTE *buff) {
  return pillsGetPillNetData(&pb, buff);
}

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
BYTE serverCoreGetStartsNetData(BYTE *buff) {
  return startsGetStartNetData(&ss, buff);
}

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
int serverCoreMakeMapNetRun(BYTE *buff, BYTE yPos) {
  return mapMakeNetRun(&mp, buff, yPos);
}

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
void serverCorePlayerJoin(BYTE playerNum) {
  tankCreate(&tk[playerNum], &ss);
  lgman[playerNum] = lgmCreate(playerNum);
  tankSetWorld(&tk[playerNum], 0, 0, 0.0f , FALSE);
}

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
void serverCorePlayerLeave(BYTE playerNum) {
  BYTE count; /* Looping variable */
  bool found; /* Finished searching */
  char playerName[PLAYER_NAME_LEN]; /* Player Name */
  PlayerBitMap pills;               /* Pills they own */
  PlayerBitMap basesOwn;            /* Bases they own */


  playersGetPlayerName(&splrs, playerNum, playerName);
  tankDestroy(&tk[playerNum], &mp, &pb, &bs);
  pills = pillsGetOwnerBitMask(&pb, playerNum);
  basesOwn = basesGetOwnerBitMask(&bs, playerNum);
  playersRejoinAddPlayer(playerName, pills, basesOwn);
  tk[playerNum] = NULL;
  if (playersGetNumAllie(&splrs, playerNum) == 1) {
    /* Has no allies - Set all things owned to neutral */
    pillsDropSetNeutralOwner(&pb, playerNum);
    basesSetNeutralOwner(&bs, playerNum);
  } else {
    count = 0;
    found = FALSE;
    while (count < MAX_TANKS && found == FALSE) {
      if (playerNum != count) {
        if (playersIsAllie(&splrs, count, playerNum) == TRUE) {
          found = TRUE;
        }
      }
      count++;
    }
    count--;
    basesMigrate(&bs, playerNum, count);
    pillsMigrate(&pb, playerNum, count);
  }

  playersLeaveGame(&splrs, playerNum);
}

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
void serverCoreSetPosData(BYTE *buff) {
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
  BYTE playerNum; /* Player number */

  WORLD wx;     /* Tank World positions */
  WORLD wy;
  BYTE *ptr;    /* Position in the buffer for adding */
  TURNTYPE ang; /* Tank Angle */
  SPEEDTYPE st;
  WORLD testX;
  WORLD testY;

  ptr = buff;
  utilGetNibbles(*ptr, &playerNum, &frame);
  ptr++;
  if (tk[playerNum] != NULL) {
    memcpy(&wx, ptr, sizeof(WORLD));
    ptr += sizeof(WORLD);
    memcpy(&wy, ptr, sizeof(WORLD));
    ptr += sizeof(WORLD);
    /* Tank Angle */
    memcpy(&ang, ptr, sizeof(TURNTYPE));
    ptr += sizeof(TURNTYPE);
    /* Tank Speed */
    memcpy(&st, ptr, sizeof(SPEEDTYPE));
    ptr += sizeof(SPEEDTYPE);

    utilGetNibbles(*ptr, &onBoat, &lgmFrame);
    ptr++;
    if (lgmFrame != 0xF) {
      /* Lgm position */
      lgmMX = *ptr;
      ptr++;
      lgmMY = *ptr;
      ptr++;
      utilGetNibbles(*ptr, &lgmPX, &lgmPY);
      ptr++;
    } else {
      lgmFrame = lgmMX = lgmMY = lgmPX = lgmPY = 0;
    }


    if (wx == 0 && wy == 0) {
      onBoat = TRUE;
    }

    /* Update Players */
    mx=my=px=py=0;
    if (tk[playerNum] != NULL) {
      if (wx == 0 && wy == 0) {
        tankGetWorld(&tk[playerNum], &testX, &testY);
        if (testX != 0 || testY != 0) {
          /* Tank is dead but we received a pos packet before we received the death packet */
          tankDropPills(&tk[playerNum], &mp, &pb, &bs);
          tankSetArmour(&tk[playerNum], TANK_FULL_ARMOUR+1);
        }
      }
      tankSetLocationData(&tk[playerNum], wx, wy, ang, st, onBoat);
    }
    playersUpdate(&splrs, playerNum, mx, my, px, py, frame, onBoat, lgmMX, lgmMY, lgmPX, lgmPY, lgmFrame);
  }
}

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
bool serverCoreTankInView(BYTE playerNum, BYTE checkX, BYTE checkY) {
  bool returnValue; /* Value to return */
  BYTE mx;
  BYTE my;
  BYTE gapX;
  BYTE gapY;

  returnValue = FALSE;
  if (playerNum < MAX_TANKS) {
    mx = tankGetMX(&tk[playerNum]);
    my = tankGetMY(&tk[playerNum]);

    if (checkX - mx < 0) {
      gapX = mx - checkX;
    } else {
      gapX = checkX - mx;
    }
    if (checkY - my < 0) {
      gapY = my - checkY;
    } else {
      gapY = checkY - my;
    }
  

    if (gapX <= MAIN_SCREEN_SIZE_X +2 && gapY <= MAIN_SCREEN_SIZE_Y +2) { /* Give it a littl buffer */
      returnValue = TRUE;
    } else {
      /* Check for pill views */
      returnValue = pillsIsInView(&pb, playerNum, checkX, checkY);
    }
  }
  return returnValue;
}

 typedef struct {
  BYTE buff[50];
  bool lgmOut;
  int len;
} posData;

posData playersPosData[MAX_TANKS];

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
void serverCorePreparePosPackets() {
  BYTE pos;   /* Position in the buffer for adding */
  BYTE high;  /* Nibble high */
  BYTE low;   /* Nibble Low */
  BYTE count; /* Looping variable */

  count = 0;
  while (count < MAX_TANKS) {
    if (playersIsInUse(&splrs, count) == TRUE && tk[count] != NULL) {
      playersPosData[count].len = 0;
      pos = 0;

      /* Tank position */
      playersPosData[count].buff[pos] = tankGetMX(&tk[count]);
      pos++;
      playersPosData[count].buff[pos] = tankGetMY(&tk[count]);
      pos++;
      high = tankGetPX(&tk[count]);
      low = tankGetPY(&tk[count]);
      playersPosData[count].buff[pos] = utilPutNibble(high, low);
      pos++;

      /* Tank Options */
      high = tankIsOnBoat(&tk[count]);
      low = tankGetDir(&tk[count]);
      playersPosData[count].buff[pos] = utilPutNibble(high, low);
      pos++;


      /* Lgm position */
      playersPosData[count].buff[pos] = lgmGetMX(&lgman[count]);
      pos++;
      playersPosData[count].buff[pos] = lgmGetMY(&lgman[count]);
      pos++;
      playersPosData[count].buff[pos] = utilPutNibble(lgmGetPX(&lgman[count]), lgmGetPY(&lgman[count]));
      pos++;
      /* Frame */
      playersPosData[count].buff[pos] = utilPutNibble(lgmGetFrame(&lgman[count]), lgmGetBrainObstructed(&lgman[count]));
      pos++;
      playersPosData[count].len = pos;
      playersPosData[count].lgmOut = lgmIsOut(&lgman[count]);
    } else {
      playersPosData[count].len = -1;
    }
    count++;
  }
} 

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
int serverCoreMakePosPackets(BYTE *buff, BYTE noPlayer, bool sendStale) {
  BYTE pos;   /* Position in the buffer for adding */
  BYTE count; /* Looping variable */
  BYTE *loc;
  bool tankInView;
  bool lgmInView;

  pos = 0;
  count = 0;
  loc = buff;

  while (count < MAX_TANKS) {
    if (playersPosData[count].len != -1) {
      /* Is the player's LGM out of their tank? */
      lgmInView = lgmIsOut(&lgman[count]);
      if (lgmInView == TRUE) {
		/* LGM is parachuting in */
		if (lgmGetFrame(&lgman[count]) == LGM_HELICOPTER_FRAME) {
          lgmInView = TRUE;
		}
		else {
		  /* Is the LGM within view of their tank or any friendly pillbox? */
          lgmInView = serverCoreTankInView(noPlayer, (BYTE) (lgmGetWX(&lgman[count]) >> M_W_SHIFT_SIZE), (BYTE) (lgmGetWY(&lgman[count])  >> M_W_SHIFT_SIZE));
		}
      }
      tankInView = FALSE;
      if (noPlayer != count) {
        tankInView = playersNeedUpdate(&splrs, count);
        if (tankInView == FALSE) {
          tankInView = serverCoreTankInView(noPlayer, tankGetMX(&tk[count]), tankGetMY(&tk[count]));
        }
      }
      if (tankInView == TRUE || lgmInView == TRUE) {
        *loc = utilPutNibble(count, (BYTE) (((tankInView << 2) + lgmInView)));
        loc++;
        pos++;
        if (tankInView == TRUE) {
          memcpy(loc, playersPosData[count].buff, 4);
          loc += 4;
          pos += 4;
        }
        if (lgmInView == TRUE) {
          memcpy(loc, (playersPosData[count].buff+4), 4);
          loc += 4;
          pos += 4;
        }
      } else if (sendStale == TRUE && noPlayer != count) {
        /* Send off stale data */
        *loc = utilPutNibble(count, 0xF);
        loc++;
        pos++;
      }
    }
    count++;
  }
  return pos;

}

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
void serverCoreExtractShellData(BYTE *buff, BYTE len) {
  shellsNetExtract(&shs, &pb, buff, len, TRUE);
}

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
BYTE serverCoreMakeShellData(BYTE *buff, BYTE noPlayerNum, bool sentState) {
  return shellsNetMake(&shs, buff, noPlayerNum, sentState);
}

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
BYTE serverCoreMakeMapData(BYTE *buff) {
  return mapNetMakePacket(&mp, buff);
}

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
bool serverCoreMapIsMine(BYTE bmx, BYTE bmy) {
  return mapIsMine(&mp, bmx, bmy);
}

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
tank* serverCoreGetTankFromPlayer(BYTE playerNum) {
  return &tk[playerNum];
}

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
BYTE serverCoreGetTankPlayer(tank *value) {
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  
  returnValue = NEUTRAL;
  count = 0;
  while (returnValue == NEUTRAL && count < MAX_TANKS) {
    if (*value == tk[count]) {
      returnValue = count;
    }
    count++;
  }
  return returnValue;
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
void serverCoreCenterTank() {
  return;
}

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
void severCoreExtractClientMapData(BYTE *buff, BYTE len, BYTE playerNum) {
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
    mapSetPos(&mp, mx, my, terrain, TRUE, FALSE);
  }
}

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
void serverCoreExtractPNBData(BYTE *buff, BYTE len) {
  netPNBExtract(&serverPNB, &mp, &bs, &pb, buff, len, TRUE);
}

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
void serverCoreExtractTkExpData(BYTE *buff, BYTE len) {
  tkExplosionNetExtract(&serverTankExp, buff, len, NEUTRAL);
}

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
BYTE serverCoreMakeTkData(BYTE *buff) {
  return tkExplosionNetMake(&serverTankExp, buff);
}

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
void serverCoreExtractNMTData(BYTE *buff, BYTE len) {
  netMNTExtract(&serverNMT, &mp, &pb, &bs, NULL, buff, len, FALSE);
}

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
BYTE serverCoreNumBases(void) {
  return basesGetNumBases(&bs);
}

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
BYTE serverCoreNumPills(void) {
  return pillsGetNumPills(&pb);
}

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
void serverCoreRequestRejoin(BYTE playerNum) {
  char playerName[PLAYER_NAME_LEN]; /* Player Name */

  playersGetPlayerName(&splrs, playerNum, playerName);
  playersRejoinRequest(playerName, playerNum, &pb, &bs);
}

void serverMessageSetQuietMode(bool modeOn);

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
void serverCoreSetQuietMode(bool mode) {
  serverMessageSetQuietMode(mode);
}

void serverMessageConsoleMessage(char *msg);

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
void serverCoreServerConsoleMessage(char *msg) {
  serverMessageConsoleMessage(msg);
}


bool serverCoreRunning() {
  return serverCoreGameRunning;
}

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
lgm *serverCoreGetLgmFromPlayerNum(BYTE playerNum) {
  return &lgman[playerNum];
}

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
void serverCoreLgmOperation(BYTE playerNum, BYTE destX, BYTE destY, BYTE operation) {
  if (tankGetArmour(&tk[playerNum]) <= TANK_FULL_ARMOUR) {
    /* If not dead */
    lgmAddRequest(&lgman[playerNum], &mp, &pb,  &bs, &tk[playerNum], destX, destY, operation);
  } else {
    /* Failed */
    serverNetSendManReturnMessge(playerNum, 0, 0, LGM_NO_PILL);
  }     
}

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
void serverCoreGetTankWorldFromLgm(lgm *lgmans, WORLD *x, WORLD *y) {
  bool found = FALSE; /* Loop guard */
  BYTE count = 0;     /* Loop variant */

  *x = 0;
  *y = 0;
  while (count < MAX_TANKS && found == FALSE) {
    if (lgmans == &lgman[count]) {
      found = TRUE;
      tankGetWorld(&tk[count], x, y);
    }
    count++;
  }
}

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
void serverCoreGetRandStart(BYTE *mx, BYTE *my, TURNTYPE *dir) {
   startsGetRandStart(&ss, mx, my ,dir);
  return;
}

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
void serverCoreGetStartPosition(BYTE playerNum, BYTE *xValue, BYTE *yValue, TURNTYPE *angel, BYTE *numShells, BYTE *numMines) {
  BYTE numArmour, numTrees; /* Number of trees */

  startsGetStart(&ss, xValue, yValue, angel, playerNum);
  gameTypeGetItems(&sGame, numShells, numMines, &numArmour, &numTrees);
  tankSetWorld(&tk[playerNum], *xValue, *yValue, *angel, FALSE);
  tankSetStats(&tk[playerNum], *numShells, *numMines, numArmour, numTrees);
  tankSetOnBoat(&tk[playerNum], TRUE);
  tankResetHitCount(&tk[playerNum]);

}

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
bool serverCoreCheckTankRange(BYTE x, BYTE y, BYTE playerNum, double distance) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */
  WORLD ourWX;
  WORLD ourWY;
  WORLD testWX;
  WORLD testWY;
  double dummy;

  returnValue = TRUE;
  ourWX = (x << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE;
  ourWY = (y << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE;
  
  count = 0;
  while (count < MAX_TANKS && returnValue == TRUE) {
    if (playerNum != count && tk[count] != NULL && count != playerNum) {
      tankGetWorld(&tk[count], &testWX, &testWY);
      returnValue = !(utilIsItemInRange(ourWX, ourWY, testWX, testWY, (WORLD) distance, &dummy));
    }
    count++;
  }
  
  return returnValue;
}

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
bool serverCoreCheckPillsRange(BYTE xValue, BYTE yValue, BYTE playerNum, double distance) {
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
  total = pillsGetNumPills(&pb);
  while (returnValue == TRUE && count <= total) {
    pillsGetPill(&pb, &pill, count);
    if (playersIsAllie(&splrs, playerNum, pill.owner) == FALSE) {
      testX = (pill.x << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE;
      testY = (pill.y << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE;
      returnValue = !(utilIsItemInRange(startX, startY, testX, testY, (WORLD) distance, &dummy));
    }
    count++;
  }
  return returnValue;
}

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
void serverCoreSendTeams() {
  BYTE count; /* Looping variables */
  BYTE count2;
  BYTE donePlayers[MAX_TANKS]; /* Players we have search through */
  BYTE numTeams; /* number of Teams */
  BYTE arraySize; /* Size of our target array */
  BYTE arrayPos; /* Array position */
  BYTE array[64]; /* Array position containing data */

  count = 0;
  while (count < MAX_TANKS) {
    donePlayers[count] = FALSE;
    count++;
  }
  arraySize = 0;
  numTeams = 0;
  count = 0;
  arrayPos = 1;
  while (count < MAX_TANKS) {
    if (donePlayers[count] == FALSE && tk[count] != NULL) {
      /* This is a new team */
      numTeams++;
      count2 = 0;
      array[arrayPos] = WINBOLO_NET_TEAM_MARKER;
      arrayPos++;
      arraySize++;
      array[arrayPos] = count;
      arrayPos++;
      arraySize++;
      donePlayers[count] = TRUE;
      while (count2 < MAX_TANKS) {
        if (count2 != count && tk[count2] != NULL && playersIsAllie(&splrs, count, count2)) {
          donePlayers[count2] = TRUE;
          array[arrayPos] = count2;
          arrayPos++;
          arraySize++;
	}
        count2++;
      }
    }
    count++;
  }
  array[0] = numTeams;

  /* Send WBN message */
  winbolonetServerSendTeams(array, arraySize, numTeams);
}

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
bool serverCoreCheckGameWin(bool printWinners) {
  static bool doneOnce = FALSE; /* Have we done this once successfully */
  static bool doneWbnOnce = FALSE; /* Have we done this once successfully */
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  BYTE max;         /* Max number of bases */
  BYTE first;       /* First Owner */
  BYTE current;     /* Current owner */
  char dest[FILENAME_MAX]; /* Player Name */
  BYTE shellsAmount;
  BYTE minesAmount;
  BYTE armourAmount;

  returnValue = TRUE;
  max = basesGetNumBases(&bs);
  count = 1;
  while (count <= max && returnValue == TRUE) {
    current = basesGetBaseOwner(&bs, count);
    basesGetStats(&bs, count, &shellsAmount, &minesAmount, &armourAmount);
    if (current == NEUTRAL || armourAmount <= MIN_ARMOUR_CAPTURE) {
      returnValue = FALSE;
    } else {
      if (count == 1) {
        first = current;
      } else {
        returnValue = playersIsAllie(&splrs, current, first);
      }
    }
    count++;
  }

  if (returnValue == TRUE && doneWbnOnce == FALSE) {
    serverCoreSendTeams();
    /* Tell WinBolo.net about it */
    winbolonetAddEvent(WINBOLO_NET_EVENT_WIN, TRUE, first, WINBOLO_NET_NO_PLAYER);
    doneWbnOnce = TRUE;
  }

  if (returnValue == TRUE && printWinners == TRUE && doneOnce == FALSE) {
    doneOnce = TRUE;
    serverMessageConsoleMessage("Winners");
    count = 0;
    while (count < MAX_TANKS) {
      if (tk[count] != NULL) {
        if (playersIsAllie(&splrs, first, count) == TRUE) {
          /* Print it */
          playersGetPlayerName(&splrs, count, dest);
          serverMessageConsoleMessage(dest);
        }
      }
      count++;
    }
  }
  return returnValue;
}

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
bool serverCoreCheckAutoClose() {
  static bool hadPlayers = FALSE; /* Have we done this once successfully */
  bool returnValue = FALSE; /* Value to return */

  if (hadPlayers == FALSE) {
    if (playersGetNumPlayers(&splrs) > 0) {
      hadPlayers = TRUE;
    }
  } else {
    if (playersGetNumPlayers(&splrs) == 0) {
      returnValue = TRUE;
    }
  }

  return returnValue;
}

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
bool serverCoreStartLog(char *fileName, BYTE ai, BYTE maxPlayers, bool usePassword) {
  return logStart(fileName, &mp, &bs, &pb, &ss, &splrs, ai, maxPlayers, usePassword);
}

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
void serverCoreStopLog() {
  logStop();
}

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
void serverCoreSetServerLogFile(char *logFile) {
  serverMessagesSetLogFile(logFile);
}

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
bool serverCoreSaveMap(char *fileName) {
  serverNetSendServerMessageAllPlayers("Server Admin saved map file.");
  return mapWrite(fileName, &mp, &pb, &bs, &ss);
}

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
players *serverCoreGetPlayers() {
  return &splrs;
}

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
building *serverCoreGetBuildings() {
  return &serverBlds;
}

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
explosions *serverCoreGetExplosions() {
  return &serverExpl;
}

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
floodFill *serverCoreGetFloodFill() {
  return &serverFF;
}

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
grass *serverCoreGetGrass() {
  return &serverGrass;
}

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
mines *serverCoreGetMines() {
  return &serverMines;
}

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
minesExp *serverGetMinesExp() {
  return &serverMinesExp;
}

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
rubble *serverCoreGetRubble() {
  return &serverRubble;
}

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
swamp *serverCoreGetSwamp() {
  return &serverSwamp;
}

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
tkExplosion  *serverCoreGetTankExplosions() {
  return &serverTankExp;
}

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
netPnbContext *serverCoreGetNetPnb() {
  return &serverPNB;
}

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
netMntContext *serverCoreGetNetMnt() {
  return &serverNMT;
}

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
gameType *serverCoreGetGameType() {
  return &sGame;
}

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
void serverCoreInformation() {
  BYTE count; /* Looping variable */
  BYTE numPlayers; /* Number of players in the game */
  BYTE maxPlayers;
  char name[255];
  char location[255];

  numPlayers = playersGetNumPlayers(&splrs);
  maxPlayers = serverNetGetMaxPlayers();
  if (maxPlayers == 0) {
    maxPlayers = MAX_TANKS;
  }
  fprintf(stdout, "\n");
  fprintf(stdout, BOLO_VERSION_STRING);
  fprintf(stdout, "\nGame start time: %sMap Name: %s - Locked: ", asctime(gmtime(&sTimeStart)), sMapName);
  if (serverNetGetIsLocked() == TRUE) {
    fprintf(stdout, "Yes");
  } else {
    fprintf(stdout, "No");
  }
  fprintf(stdout, "\nPlayers: (%d/%d) Neutral Pillboxes: (%d/%d), Neutral Bases: (%d/%d)\n\n", numPlayers, maxPlayers, pillsGetNumNeutral(&pb), pillsGetNumPills(&pb), basesGetNumNeutral(&bs), basesGetNumBases(&bs));
  
  if (numPlayers > 0) {
    fprintf(stdout, "Players:\n");
    
    count = 0;
    while (count < MAX_TANKS) {
      if (playersIsInUse(&splrs, count) == TRUE) {
        playersGetPlayerName(&splrs, count, name);
        playersGetPlayerLocation(&splrs, count, location);
        fprintf(stderr, "%s@%s - (P:%d B: %d)\n", name, location, pillsGetNumberOwnedByPlayer(&pb, count), basesGetNumberOwnedByPlayer(&bs, count));
      }
      count++;
    }
  }
}

