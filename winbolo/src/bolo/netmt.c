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
*Name:          Net Mines n Terrain
*Filename:      Netmt.c
*Author:        John Morrison
*Creation Date: 19/03/99
*Last Modified: 05/04/02
*Purpose:
* Handles keeping of mines and terrain changes across 
* the network.
*********************************************************/


#include "bolo_map.h"
#include "building.h"
#include "explosions.h"
#include "frontend.h"
#include "global.h"
#include "grass.h"
#include "lgm.h"
#include "log.h"
#include "messages.h"
#include "mines.h"
#include "minesexp.h"
#include "netmt.h"
#include "network.h"
#include "players.h"
#include "rubble.h"
#include "screen.h"
#include "sounddist.h"
#include "swamp.h"
#include "tank.h"
#include "../server/servercore.h"
#include "../winbolonet/winbolonet.h"


/* Protos */
void netErrorOccured();

/*********************************************************
*NAME:          netMNTCreate
*AUTHOR:        John Morrison
*Creation Date: 19/3/99
*Last Modified: 11/11/99
*PURPOSE:
* Creates an netMNT struncture
*
*ARGUMENTS:
*  nmtc - Pointer to the netMntContext object
*********************************************************/
void netMNTCreate(netMntContext *nmtc) {
  New(*nmtc);
  (*nmtc)->nmt = NULL;
  (*nmtc)->netMntUpto = 0;
}

/*********************************************************
*NAME:          netNMTDestroy
*AUTHOR:        John Morrison
*Creation Date: 19/3/99
*Last Modified: 19/3/99
*PURPOSE:
* Destroys a netPNT structure
*
*ARGUMENTS:
*  nmtc - Pointer to the netMntContext object
*********************************************************/
void netNMTDestroy(netMntContext *nmtc) {
  netMnt q;

  if (*nmtc != NULL) {
    q = (*nmtc)->nmt;
    while (NonEmpty((*nmtc)->nmt)) {
      q = (*nmtc)->nmt;
      (*nmtc)->nmt = NetMNTTail(q);
      Dispose(q);
    }
    Dispose(*nmtc);
    *nmtc = NULL;
  }
}

/*********************************************************
*NAME:          netMNTAdd
*AUTHOR:        John Morrison
*Creation Date: 19/3/99
*Last Modified: 31/10/99
*PURPOSE:
* Adds an item to the netMNT structure
*
*ARGUMENTS:
*  nmtc    - Pointer to the netMntContext object
*  event   - Event type
*  itemNum - Item Number which the even occured to
*  Owner   - Who created this event
*  opt1    - Optional location data
*  opt2    - Optional location data
*********************************************************/
void netMNTAdd(netMntContext *nmtc, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2) {
  netMnt add1; /* Items to add */

  if (netGetType() != netSingle && playersGetNumPlayers(screenGetPlayers()) > 0) {
  /* Check for multiplayer game */
    New(add1);
    add1->item = utilPutNibble(event, itemNum);
    add1->owner = owner;
    if (threadsGetContext() == TRUE) {
      add1->id = (*nmtc)->netMntUpto;
      (*nmtc)->netMntUpto++;
      if ((*nmtc)->netMntUpto == 250) {
        (*nmtc)->netMntUpto = 0;
      }
    } else {
      add1->id = 0;
    }
    add1->x = opt1;
    add1->y = opt2;
    add1->next = (*nmtc)->nmt;
    (*nmtc)->nmt = add1;
  }
}

/*********************************************************
*NAME:          netMNTMake
*AUTHOR:        John Morrison
*Creation Date: 19/3/99
*Last Modified: 11/11/99
*PURPOSE:
* Makes the network packet and returns its length. Also
* destroys the netPNT structure.
*
*ARGUMENTS:
*  nmtc - Pointer to the netMntContext object
*  buff - Pointer to buffer to put data in
*********************************************************/
int netMNTMake(netMntContext *nmtc, BYTE *buff) {
  int returnValue; /* Value to return */
  BYTE *pnt;       /* Pointer to the buffer */
  bool isServer;   /* Are we the server */
  int count;       /* Looping variable */
  netMnt q;

  
  isServer = threadsGetContext();
  returnValue = 0;
  pnt = buff;
  count = 1;

  q = (*nmtc)->nmt;
  while (NonEmpty(q)) {
    *pnt = q->item;
    pnt++;
    returnValue++;
    *pnt = q->owner;
    pnt++;
    returnValue++;
    *pnt = q->id;
    pnt++;
    returnValue++;
    *pnt = q->x;
    pnt++;
    returnValue++;
    *pnt = q->y;
    pnt++;
    returnValue++;      
    q = NetMNTTail(q);
    netMNTDeleteItem(nmtc, count);
  }
  return returnValue;
}

/*********************************************************
*NAME:          netMNTExtractServer
*AUTHOR:        John Morrison
*CREATION DATE: 11/11/99
*LAST MODIFIED: 05/04/02
*PURPOSE:
* Extracts a netMNT item as a server. Returns whether this
* is an allowable operation (cheat checking)
*
*ARGUMENTS:
*  nmtc    - Pointer to the netMntContext object
*  mp      - Pointer to the map sturcture
*  pb      - Pointer to the pillbox sturcture
*  bs      - Pointer to the bases sturcture
*  event   - netPNB Event
*  itemNum - Item number event occurred to
*  owner   - Owner of the event
*  opt1    - Optional data 1
*  opt2    - Optional data 2
*********************************************************/
bool netMNTExtractServer(netMntContext *nmtc, map *mp, pillboxes *pb, bases *bs, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2) {
  BYTE pos; /* Map position */
  bool returnValue = TRUE; /* Value to return */
  tank *tk;
  BYTE amount;
  switch (event) {
  case NMNT_PILLMIGRATE:
    pillsSetPillOwner(pb, (BYTE) (itemNum+1), owner, TRUE);
    break;
  case NMNT_KILLME:
    /* WinBolo.net Stuff */
    if (opt1 != NEUTRAL) {
      winbolonetAddEvent(WINBOLO_NET_EVENT_TANK_KILL, TRUE, opt1, owner);
      logAddEvent(log_KillPlayer, owner, opt1, 0, 0, 0, NULL);
    } else {
      logAddEvent(log_PlayerDied, owner, 0, 0, 0, 0, NULL);
    }
    tk = screenGetTankFromPlayer(owner);
    tankDropPills(tk, mp, pb, bs);
    tankSetWorld(tk, 0, 0, 0, FALSE);
    tankSetOnBoat(tk, TRUE);
    tankResetHitCount(tk);
    break;
  case NMNT_MINEPLACE:
  case NMNT_MINETANKPLACE:
    tk = screenGetTankFromPlayer(owner);
    pos = mapGetPos(mp, opt1, opt2);
    amount = tankGetMines(tk);
    if (pos < MINE_START && amount >= 1 ) {
      amount--;
      tankSetMines(tk, amount); 
      mapSetPos(mp, opt1, opt2, (BYTE) (pos + MINE_SUBTRACT), FALSE, FALSE);
    } else {
      returnValue = FALSE;
    }
    logAddEvent(log_SoundMineLay, opt1, opt2, 0, 0, 0, NULL);
    break;
  case NMNT_MINEEXPLOSION:
    minesExpAddItem(screenGetMinesExp(), mp, opt1, opt2);
    break;
  case NMNT_MANRETURN:
    break;
  case NMNT_MANACTION:
    serverCoreLgmOperation(owner, opt1, opt2, itemNum);
    returnValue = FALSE;
    break;
  case NMNT_RUBBLE:
    rubbleAddItem(screenGetRubble(), opt1, opt2);
    break;
  case NMNT_SWAMP:
    swampAddItem(screenGetSwamp(), opt1, opt2);
    break;
  case NMNT_BASEMIGRATE:
    basesSetOwner(bs, opt1, opt2, owner, TRUE);
    break;
  case NMNT_BASERELOAD:
    basesServerRefuel(bs, itemNum, owner);
    break;
  case NMNT_BUILDBOAT:
    break;
  case NMNT_BUILDBUILDING:
    break;
  case NMNT_DROPBOAT:
    break;
  case NMNT_RUNOVERBOAT:
    break;
  case NMNT_TANKHIT:
    /* Process a tank hit */
    tk = serverCoreGetTankFromPlayer(owner);
    if (tk != NULL) {
      if (opt1 == 0xFF && opt2 == 0xFF) {
        /* Cheaters */

        netPlayersSetCheater(serverNetGetNetPlayers(), owner);
//        printf("Cheat received - memory hack...\n");
      } else if (opt1 < 127) {
        tankAddHit(tk, opt1);
      } else {
        tankAddHit(tk, abs(127 - opt1));
      }
      if (opt1 == 1) {
        logAddEvent(log_SoundHitTank, tankGetMX(tk), tankGetMY(tk), 0, 0, 0, NULL); 
      }
    }
    break;
  }
  return returnValue;
}

/*********************************************************
*NAME:          netMNTExtractClient
*AUTHOR:        John Morrison
*CREATION DATE: 11/11/99
*LAST MODIFIED: 04/04/02
*PURPOSE:
* Extracts a netMNT item as a server. Returns whether a 
* screen recalc is necessaru
*
*ARGUMENTS:
*  nmtc    - Pointer to the netMntContext object
*  mp      - Pointer to the map sturcture
*  pb      - Pointer to the pillbox sturcture
*  bs      - Pointer to the bases sturcture
*  tnk     - Pointer to the tank structure
*  event   - netPNB Event
*  itemNum - Item number event occurred to
*  owner   - Owner of the event
*  opt1    - Optional data 1
*  opt2    - Optional data 2
*********************************************************/
bool netMNTExtractClient(netMntContext *nmtc, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2) {
  bool needCalc; /* Screen needs a recalc */
  BYTE pos;      /* Map position */

  needCalc = FALSE;
  switch (event) {
  case NMNT_PILLMIGRATE:
    pillsSetPillOwner(pb, (BYTE) (itemNum+1), owner, TRUE);
    break;
  case NMNT_KILLME:
    {
      BYTE lgmMX;
      BYTE lgmMY;
      BYTE lgmPX;
      BYTE lgmPY;
      BYTE lgmFrame;
      if (opt1 == 0xFF && opt2 == 0xFF) {
        playersGetLgmDetails(screenGetPlayers(), owner, &lgmMX, &lgmMY, &lgmPX, &lgmPY, &lgmFrame);
        playersUpdate(screenGetPlayers(), owner, 0, 0, 0, 0, 0, 0, lgmMX, lgmMY, lgmPX, lgmPY, lgmFrame);
      } else if (opt1 == playersGetSelf(screenGetPlayers())) {
      /* We killed a player */
        tankAddKill(tnk);
        playersGetLgmDetails(screenGetPlayers(), opt1, &lgmMX, &lgmMY, &lgmPX, &lgmPY, &lgmFrame);
        playersUpdate(screenGetPlayers(), opt1, 0, 0, 0, 0, 0, 0, lgmMX, lgmMY, lgmPX, lgmPY, lgmFrame);

      }
    }
    break;
  case NMNT_MINEPLACE:
    pos = mapGetPos(mp, opt1, opt2);
    if (pos < MINE_START) {
      mapSetPos(mp, opt1, opt2, (BYTE) (pos + MINE_SUBTRACT), FALSE, FALSE);
    }
    if (playersIsAllie(screenGetPlayers(), playersGetSelf(screenGetPlayers()), owner) == TRUE) {
      needCalc = TRUE;
      minesAddItem(screenGetMines(), opt1, opt2);
    }
    /* Play the sound */
    soundDist(manLayingMineNear, opt1, opt2);
    break;
  case NMNT_MINETANKPLACE:
    needCalc = TRUE;
    minesAddItem(screenGetMines(), opt1, opt2);
    pos = mapGetPos(mp, opt1, opt2);
    if (pos < MINE_START) {
      mapSetPos(mp, opt1, opt2, (BYTE) (pos + MINE_SUBTRACT), FALSE, FALSE);
    }
    if (owner != playersGetSelf(screenGetPlayers())) {
      /* Play the sound */
      soundDist(manLayingMineNear, opt1, opt2);
    }
    break;
  case NMNT_MINEEXPLOSION:
    if (owner != playersGetSelf(screenGetPlayers())) {
      explosionsAddItem(screenGetExplosions(), opt1, opt2, 0, 0, EXPLOSION_START);
      soundDist(mineExplosionNear, opt1, opt2);
      lgmDeathCheck(screenGetLgmFromPlayerNum(playersGetSelf(screenGetPlayers())), mp, pb, bs, (WORLD) ((opt1 << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), (WORLD) ((opt2 << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), NEUTRAL);
      screenCheckTankMineDamage(opt1, opt2);
      minesExpAddItem(screenGetMinesExp(), mp, opt1, opt2);
      /* Play the sound */
      soundDist(mineExplosionNear, opt1, opt2);
    }
    break;
  case NMNT_MANRETURN:
    break;
  case NMNT_MANACTION:
    break;
  case NMNT_RUBBLE:
    rubbleAddItem(screenGetRubble(), opt1, opt2);
    break;
  case NMNT_SWAMP:
    swampAddItem(screenGetSwamp(), opt1, opt2);
    break;
  case NMNT_BASEMIGRATE:
    basesSetOwner(bs, opt1, opt2, owner, TRUE);
    break;
  case NMNT_BASERELOAD:
    basesServerRefuel(bs, itemNum, owner);
    break;
  case NMNT_BUILDBOAT:
    mapSetPos(mp, opt1, opt2, BOAT, FALSE, FALSE);
    needCalc = TRUE;
    break;
  case NMNT_BUILDBUILDING:
    mapSetPos(mp, opt1, opt2, BUILDING, FALSE, FALSE);
    soundDist(manBuildingNear, opt1, opt2);
    buildingRemovePos(screenGetBuildings(), opt1, opt2);
    needCalc = TRUE;
    break;
  case NMNT_DROPBOAT:
    if (mapGetPos(mp, opt1, opt2) == BOAT) {
      mapSetPos(mp, opt1, opt2, RIVER, FALSE, FALSE);
    }
    break;
  case NMNT_RUNOVERBOAT:
    if (owner != playersGetSelf(screenGetPlayers())) {
      explosionsAddItem(screenGetExplosions(), opt1, opt2, 0, 0, EXPLOSION_START);
      soundDist(shotBuildingNear, opt1, opt2);
    }
    break;
  case NMNT_TANKHIT:
    /* Do nothing */
    break;
  }

  return needCalc;
}

/*********************************************************
*NAME:          netMNTExtract
*AUTHOR:        John Morrison
*Creation Date: 19/3/99
*Last Modified:  8/1/00
*PURPOSE:
* Extracts the netNMT data. Returns FALSE upon an error
*
*ARGUMENTS:
*  nmtc    - Pointer to the netMntContext object
*  mp      - Pointer to the map sturcture
*  pb      - Pointer to the pillbox structure
*  bs      - Pointer to the bases structure
*  tnk     - Pointer to the tank structure
*  buff    - Pointer to buffer to put data in
*  dataLen - Length of the data.
*  isTcp   - Is this TCP data
*********************************************************/
bool netMNTExtract(netMntContext *nmtc, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE *buff, int dataLen, bool isTcp) {
  bool returnValue; /* Value to return */
  int count;        /* Looping variable */
  BYTE event;       /* Event Type */
  BYTE itemNum;     /* Item Number */
  BYTE opt1;        /* Optional Data */
  BYTE opt2;
  bool testCalc;    /* Does the screen need a recalc? */
  bool needCalc;    /* Does the screen need a recalc? */
  BYTE id;          /* Item ID */
  BYTE owner;       /* Who owns this */
  BYTE eventNibble; 
  bool isServer;    /* Are we server */
  bool errOccurred; /* Has an packet avert error occurred? */

  errOccurred = FALSE;
  count = 0;
  needCalc = FALSE;
  isServer = threadsGetContext();
  returnValue = TRUE;

  while (count < dataLen) {
    utilGetNibbles(buff[count],&event, &itemNum);
    eventNibble = buff[count];
    count++;
    owner = buff[count];
    count++;
    id = buff[count];
    count++;
    opt1 = buff[count];
    count++;
    opt2 = buff[count];
    count++;
    /* Process the occurence */
    if (isServer == TRUE) {
      if (netMNTExtractServer(nmtc, mp, pb, bs, event, itemNum, owner, opt1, opt2) == TRUE) {
        netMNTAdd(nmtc, event, itemNum, owner, opt1, opt2);
      }
    } else {
      testCalc = netMNTExtractClient(nmtc, mp, pb, bs, tnk, event, itemNum, owner, opt1, opt2);
      if (needCalc == FALSE) {
        needCalc = testCalc;
      }
    }

  }
  /* Recalc Screen if required */
  if (needCalc == TRUE) {
    screenReCalc();
  }

  if (returnValue == FALSE) {
    messageAdd(networkMessage, (char *) "\0", (char *) "mnt-no");
  }
  if (errOccurred == TRUE) {
    messageAdd(networkMessage, (char *) "\0", (char *) "nmt-a");
  }
  return TRUE;
//  return returnValue;
}

/*********************************************************
*NAME:          netMNTMessage
*AUTHOR:        John Morrison
*Creation Date: 19/3/99
*Last Modified: 19/3/99
*PURPOSE:
* Some messages need to be displayed to the screen. Do so
* here
*
*ARGUMENTS:
*  playerNum - The player number.
*  msg       - Message to display
*********************************************************/
void netMNTMessage(BYTE playerNum, char *msg) {
  char name[FILENAME_MAX];   /* The Player Name */
  char output[FILENAME_MAX]; /* The message */

  output[0] = '\0';
  name[0] = '\0';

  playersMakeMessageName(screenGetPlayers(), playerNum, name);
  strcat(output, MESSAGE_QUOTES);
  strcat(output, name);
  strcat(output, MESSAGE_QUOTES);
  strcat(output, msg);
  messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), output);
}

/*********************************************************
*NAME:          netMNTDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/99
*PURPOSE:
*  Deletes the item for the given number
*
*ARGUMENTS:
*  nmtc    - Pointer to the netMntContext object
*  itemNum - The item number to delete
*********************************************************/
void netMNTDeleteItem(netMntContext *nmtc, int itemNum) {
  netMnt prev; /* The previous item to link to the delete items next */
  netMnt del;  /* The item to delete */
  int count;   /* Looping variable */

  if (itemNum == 1) {
    del = (*nmtc)->nmt;
    (*nmtc)->nmt = del->next;
    Dispose(del);
  } else {
    count = 1;
    prev = (*nmtc)->nmt;
    while (count < (itemNum-1)) {
      prev = NetMNTTail(prev);
      count++;
    }
    del = NetMNTTail(prev);
    prev->next = del->next;
    Dispose(del);
  }
}
