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
*Name:          Net Pills n Bases
*Filename:      Netpnb.h
*Author:        John Morrison
*Creation Date: 09/03/99
*Last Modified: 20/08/02
*Purpose:
*  Handles keeping of base and pill events for transport
*  across the network.
*********************************************************/

#include "bases.h"
#include "global.h"
#include "frontend.h"
#include "log.h"
#include "messages.h"
#include "netpnb.h"
#include "network.h"
#include "pillbox.h"
#include "players.h"
#include "screen.h"
#include "sounddist.h"
#include "util.h"

#include "../server/servercore.h"


/*********************************************************
*NAME:          netPNBCreate
*AUTHOR:        John Morrison
*CREATION DATE: 9/3/99
*LAST MODIFIED: 9/3/99
*PURPOSE:
* Creates an netPNB struncture
*
*ARGUMENTS:
*  pnbc    - Pointer to the netPnbContext object
*********************************************************/
void netPNBCreate(netPnbContext *pnbc) {
  New(*pnbc);
  (*pnbc)->pnb = NULL;
  (*pnbc)->netPnbUpto = 0;
}

/*********************************************************
*NAME:          netPNBDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 9/3/99
*LAST MODIFIED: 9/3/99
*PURPOSE:
* Destroys a netPNB structure
*
*ARGUMENTS:
*  pnbc    - Pointer to the netPnbContext object
*********************************************************/
void netPNBDestroy(netPnbContext *pnbc) {
  netPnb q;

  if (*pnbc != NULL) {
    while (NonEmpty((*pnbc)->pnb)) {
      q = (*pnbc)->pnb;
      (*pnbc)->pnb = NetPNBTail(q);
      Dispose(q);
    }
    Dispose(*pnbc);
    *pnbc = NULL;
  }
}

/*********************************************************
*NAME:          netPNBAdd
*AUTHOR:        John Morrison
*CREATION DATE: 9/3/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Adds an item to the netPNB structure
*
*ARGUMENTS:
*  pnbc    - Pointer to the netPnbContext object
*  event    - Event type
*  itemNum  - Item Number which the event occured to
*  owner    - Owner of this event
*  opt1     - Optional location data
*  opt2     - Optional location data
*********************************************************/
void netPNBAdd(netPnbContext *pnbc, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2, BYTE opt3) {
	netPnb add1; /* Items to add */

	/* We are in a multiplayer game that has one or more players.
	* Could be client or server.
	*/
	if (netGetType() != netSingle && playersGetNumPlayers(screenGetPlayers()) > 0) {
		New(add1);
		add1->item = utilPutNibble(event, itemNum);
		add1->owner = owner;
		add1->x = opt1;
		add1->y = opt2;
		add1->opt = opt3;
		/* We are a server */
		if (threadsGetContext() == TRUE) {
			add1->id = (*pnbc)->netPnbUpto;
			(*pnbc)->netPnbUpto++;
			if ((*pnbc)->netPnbUpto == 250) {
				(*pnbc)->netPnbUpto = 0;
			}
		} else {
			/* We are a client */
			add1->id = 0;
		}
		add1->next = (*pnbc)->pnb;
		(*pnbc)->pnb = add1;
	}
}

/*********************************************************
*NAME:          netPNBMake
*AUTHOR:        John Morrison
*CREATION DATE: 9/3/99
*LAST MODIFIED: 20/08/02
*PURPOSE:
* Makes the network packet and returns its length.
*
*ARGUMENTS:
*  pnbc    - Pointer to the netPnbContext object
*  buff    - Pointer to buffer to put data in
*********************************************************/
int netPNBMake(netPnbContext *pnbc, BYTE *buff) {
  int returnValue; /* Value to return */
  BYTE *pnt;       /* Pointer to the buffer */
  bool isServer;   /* Are we the server */
  int count;       /* Looping variable */
  netPnb q;

  isServer = threadsGetContext();
  returnValue = 0;
  pnt = buff;
  count = 1;

  q = (*pnbc)->pnb;
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
	*pnt = q->opt;
	pnt++;
    returnValue++;
    q = NetPNBTail(q);
    netPNBDeleteItem(pnbc, count);
  }

  return returnValue;
}


/*********************************************************
*NAME:          netPNBExtractItemServer
*AUTHOR:        John Morrison
*CREATION DATE: 11/11/99
*LAST MODIFIED: 05/05/01
*PURPOSE:
* Extracts a ntPNB item as a server. Returns whether we
* should add the item (cheater check)
*
*ARGUMENTS:
*  pnbc    - Pointer to the netPnbContext object
*  mp      - Pointer to the map sturcture
*  pb      - Pointer to the pillbox sturcture
*  bs      - Pointer to the bases sturcture
*  event   - netPNB Event
*  itemNum - Item number event occurred to
*  owner   - Owner of the event
*  opt1    - Optional data 1
*  opt2    - Optional data 2
*********************************************************/
bool netPNBExtractItemServer(netPnbContext *pnbc, map *mp, bases *bs, pillboxes *pb, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2, BYTE opt3) {
	bool returnValue; /* Value to return */
	pillbox addPill; /* Pill to place if required */
	tank *tnk;
/*	BYTE dummy; */
/*	BYTE amount; */
	double dummyD;
	base bb;
	WORLD tankWx;
	WORLD tankWy;

	returnValue = TRUE;

	switch (event) {
		case NPNB_BASE_CAPTURE:
			/* Base capture */
			returnValue = FALSE;
			break;
		case NPNB_BASE_REFUEL_ARMOUR:
			tnk = screenGetTankFromPlayer(owner);
			basesGetBase(bs, &bb, (BYTE) (itemNum + 1));
			tankGetWorld(tnk, &tankWx, &tankWy);
/*			basesGetStats(bs, basesGetBaseNum(bs, tankGetMX(tnk), tankGetMY(tnk)), &dummy, &dummy, &amount); */
			if (bb.armour >= BASE_ARMOUR_GIVE && utilIsItemInRange(tankWx, tankWy, (WORLD) ((bb.x << 8) + MAP_SQUARE_MIDDLE), (WORLD) ((bb.y << 8) + MAP_SQUARE_MIDDLE), 512, &dummyD) == TRUE && playersIsAllie(screenGetPlayers(), bb.owner, owner) == TRUE) {
				basesNetGiveArmour(bs, itemNum);
				tankSetArmour(tnk, (BYTE) (tankGetArmour(tnk) + BASE_ARMOUR_GIVE));
			} else {
				returnValue = FALSE;
			}
			tankAddHit(tnk, -1);
			break;
		case NPNB_BASE_REFUEL_SHELLS:
			tnk = screenGetTankFromPlayer(owner);
			basesGetBase(bs, &bb, (BYTE) (itemNum + 1));
			tankGetWorld(tnk, &tankWx, &tankWy);
/*			basesGetStats(bs, basesGetBaseNum(bs, tankGetMX(tnk), tankGetMY(tnk)), &amount, &dummy, &dummy); */
			if (bb.shells >= BASE_SHELLS_GIVE && utilIsItemInRange(tankWx, tankWy, (WORLD) ((bb.x << 8) + MAP_SQUARE_MIDDLE), (WORLD) ((bb.y << 8) + MAP_SQUARE_MIDDLE), 512, &dummyD) == TRUE && playersIsAllie(screenGetPlayers(), bb.owner, owner) == TRUE) {
				basesNetGiveShells(bs, itemNum);
				tankSetShells(tnk, (BYTE) (tankGetShells(tnk) + BASE_SHELLS_GIVE));
			} else {
				returnValue = FALSE;
			}
			break;
		case NPNB_BASE_REFUEL_MINES:
			tnk = screenGetTankFromPlayer(owner);
			basesGetBase(bs, &bb, (BYTE) (itemNum + 1));
			tankGetWorld(tnk, &tankWx, &tankWy);

/*			basesGetStats(bs, basesGetBaseNum(bs, tankGetMX(tnk), tankGetMY(tnk)), &dummy, &amount, &dummy); */
			if (bb.mines >= BASE_MINES_GIVE && utilIsItemInRange(tankWx, tankWy, (WORLD) ((bb.x << 8) + MAP_SQUARE_MIDDLE), (WORLD) ((bb.y << 8) + MAP_SQUARE_MIDDLE), 512, &dummyD) == TRUE && playersIsAllie(screenGetPlayers(), bb.owner, owner) == TRUE) {
				basesNetGiveMines(bs, itemNum);
				tankSetMines(tnk, (BYTE) (tankGetMines(tnk) + BASE_MINES_GIVE));
			} else {
				returnValue = FALSE;
			}
			break;
		case NPNB_BASE_HIT:
		case NPNB_PILL_HIT:
			/* This shouldn't happen. Why not? */
			returnValue = FALSE;
			break;
		case NPNB_PILL_CAPTURE:
			pillsSetPillOwner(pb, (BYTE) (itemNum+1), owner, FALSE);
			break;
		case NPNB_PILL_PICKUP:
			pillsSetPillInTank(pb, (BYTE) (itemNum + 1), TRUE);
			if (threadsGetContext() == FALSE) {
				frontEndStatusPillbox((BYTE) (itemNum+1), (pillsGetAllianceNum(pb, (BYTE) (itemNum+1))));
			}
			break;
		case NPNB_PILL_DROP:
			addPill.x = opt1;
			addPill.y = opt2;
			addPill.owner = owner;
			addPill.armour = PILLS_MAX_ARMOUR;
			addPill.speed = pillsGetAttackSpeed(pb, (BYTE) (itemNum +1));
			addPill.coolDown = 0;
			addPill.inTank = FALSE;
			addPill.justSeen = FALSE;
			pillsSetPill(pb, &addPill, (BYTE) (itemNum+1));
			tankStopCarryingPill(screenGetTankFromPlayer(owner), itemNum);
			break;
		case NPNB_PILL_DEAD:
			addPill.x = opt1;
			addPill.y = opt2;
			addPill.owner = owner;
			addPill.armour = 0;
			addPill.speed = pillsGetAttackSpeed(pb, (BYTE) (itemNum +1));
			addPill.coolDown = 0;
			addPill.inTank = FALSE;
			addPill.justSeen = FALSE;
			pillsSetPill(pb, &addPill, (BYTE) (itemNum+1));
			tankStopCarryingPill(screenGetTankFromPlayer(owner), itemNum);    
			break;
		case NPNB_PILL_REPAIR:
			/* Pill repair */
			pillsRepairPos(pb, opt1, opt2, opt3);
			break;
		case NPNB_LGM_DEAD:
			/* LGM Died */
			break;
		case NPNB_LGM_FARMTREE:
			/* LGM Farmed a tree */
			serverCoreLgmOperation(owner, opt1, opt2, LGM_TREE_REQUEST);
			returnValue = FALSE;
			break;
		case NPNB_LGM_BUILDROAD:
			/* LGM Built a road */
			break;
		case NPNB_SAVEMAP:
			netPNBMessage(owner, langGetText(MESSAGE_SAVED_MAP));
    logAddEvent(log_SaveMap, owner, 0, 0, 0, 0, NULL);
			break;
	}

  return returnValue;
}


void tankPutPill(tank *value, pillboxes *pb, BYTE pillNum);

/*********************************************************
*NAME:          netPNBExtractItemClient
*AUTHOR:        John Morrison
*CREATION DATE: 11/11/99
*LAST MODIFIED: 05/05/01
*PURPOSE:
* Extracts a ntPNB item as a client. Returns if an screen
* update is required.
*
*ARGUMENTS:
*  pnbc    - Pointer to the netPnbContext object
*  mp      - Pointer to the map sturcture
*  pb      - Pointer to the pillbox sturcture
*  bs      - Pointer to the bases sturcture
*  event   - netPNB Event
*  itemNum - Item number event occurred to
*  owner   - Owner of the event
*  opt1    - Optional data 1
*  opt2    - Optional data 2
*********************************************************/
bool netPNBExtractItemClient(netPnbContext *pnbc, map *mp, bases *bs, pillboxes *pb, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2, BYTE opt3) {

	pillbox addPill; /* Pill to place if required */
	bool needCalc; /* Needs a recalc */
	BYTE playerNum;  /* Our player Number */
	tank *tnk;


	playerNum = playersGetSelf(screenGetPlayers());
	needCalc = FALSE;
	switch (event) {
		case NPNB_BASE_CAPTURE: /* Base capture */
			basesSetOwner(bs, opt1, opt2, owner, FALSE);
			/* Client context */
			if (threadsGetContext() == FALSE) {
				frontEndStatusBase((BYTE) (itemNum+1), (basesGetStatusNum(bs, (BYTE) (itemNum+1))));
			}
			needCalc = TRUE;
			break;
		case NPNB_BASE_REFUEL_ARMOUR:
			if (playerNum != owner) {
				basesNetGiveArmour(bs, itemNum);
			}
			break;
		case NPNB_BASE_REFUEL_SHELLS:
			if (playerNum != owner) {
				basesNetGiveShells(bs, itemNum);
			}
			break;
		case NPNB_BASE_REFUEL_MINES:
			if (playerNum != owner) {
				basesNetGiveMines(bs, itemNum);
			}
			break;
		case NPNB_BASE_HIT:
			basesDamagePos(bs, opt1, opt2);
			if (owner != playerNum) {
				pillsBaseHit(pb, opt1, opt2, (basesGetOwnerPos(bs, opt1, opt2)));
				soundDist(shotBuildingNear, opt1, opt2); /* Play sound */
			}
			break;
		case NPNB_PILL_HIT:
			if (owner != playerNum) {
				pillsDamagePos(pb, opt1, opt2, TRUE, FALSE); 
			} else {
				pillsDamagePos(pb, opt1, opt2, TRUE, TRUE); 
			}
			needCalc = TRUE;
			soundDist(shotBuildingNear, opt1, opt2); /* Play sound */
			break;
		case NPNB_PILL_CAPTURE:
			/* Some other player captured a pill */
			if (owner != playerNum) {
				pillsSetPillOwner(pb, (BYTE) (itemNum+1), owner, FALSE);
				screenTankStopCarryingPill(itemNum);
			} else {
				/* The player picked up their own pillbox */
				tnk = screenGetTankFromPlayer(owner);
				if (tankGetArmour(tnk) > TANK_FULL_ARMOUR ) {
					/* We are dead */
					pillbox p; 
					pillsGetPill(pb, &p, (BYTE) (itemNum+1));
					/* Drop the pill */
					netPNBAdd(pnbc, NPNB_PILL_DEAD, itemNum, owner, p.x, p.y, 0);
					tankStopCarryingPill(tnk, itemNum);
				} else {
					/* We are still alive */
					pillsSetPillOwner(pb, (BYTE) (itemNum+1), owner, FALSE);
				}
			}
			needCalc = TRUE;
			break;
		case NPNB_PILL_PICKUP:
			/* The player picked up a pill */
			if (owner == playerNum) {
				tnk = screenGetTankFromPlayer(owner);
				if (tankGetArmour(tnk) > TANK_FULL_ARMOUR ) {
					/* We are dead */
					pillbox p; 
					pillsGetPill(pb, &p, (BYTE) (itemNum+1));
					/* Drop the pill */
					netPNBAdd(pnbc, NPNB_PILL_DEAD, itemNum, owner, p.x, p.y, 0);
					tankStopCarryingPill(tnk, itemNum);
				} else {
					/* The player is still alive */
					tankPutPill(screenGetTankFromPlayer(playerNum), pb, (BYTE) (itemNum+1));
					pillsSetPillInTank(pb, (BYTE) (itemNum+ 1), TRUE);
					if (threadsGetContext() == FALSE) {
						/* We are a client */
						frontEndStatusPillbox((BYTE) (itemNum+1), (pillsGetAllianceNum(pb, (BYTE) (itemNum+1))));
					}
				}
			} else {
				/* Some other tank picked up a pill */
				pillsSetPillInTank(pb, (BYTE) (itemNum+1), TRUE);
				if (threadsGetContext() == FALSE) {
					/* We are a client */
					frontEndStatusPillbox((BYTE) (itemNum+1), (pillsGetAllianceNum(pb, (BYTE) (itemNum+1))));
				}
			}
			needCalc = TRUE;
			break;
		case NPNB_PILL_DROP:
			addPill.x = opt1;
			addPill.y = opt2;
			addPill.owner = owner;
			addPill.armour = PILLS_MAX_ARMOUR;
			addPill.speed = pillsGetAttackSpeed(pb, (BYTE) (itemNum +1));
			addPill.coolDown = 0;
			addPill.inTank = FALSE;
			addPill.justSeen = FALSE;
			pillsSetPill(pb, &addPill, (BYTE) (itemNum+1));
			if (threadsGetContext() == FALSE) {
				frontEndStatusPillbox((BYTE) (itemNum+1), (pillsGetAllianceNum(pb, (BYTE) (itemNum+1))));
			}
			soundDist(manBuildingNear, opt1, opt2);
			needCalc = TRUE;
			break;
		case NPNB_PILL_DEAD:
/*			if (owner != playerNum) { */
			addPill.x = opt1;
			addPill.y = opt2;
			addPill.owner = owner;
			addPill.armour = 0;
			addPill.speed = pillsGetAttackSpeed(pb, (BYTE) (itemNum +1));
			addPill.coolDown = 0;
			addPill.inTank = FALSE;
			addPill.justSeen = FALSE;
			pillsSetPill(pb, &addPill, (BYTE) (itemNum+1));
			if (threadsGetContext() == FALSE) {
				frontEndStatusPillbox((BYTE) (itemNum+1), (pillsGetAllianceNum(pb, (BYTE) (itemNum+1))));
			}
			needCalc = TRUE;
/*			} */
			break;
		case NPNB_PILL_REPAIR:
			/* Pill repair */
			pillsRepairPos(pb, opt1, opt2, opt3);
			soundDist(manBuildingNear, opt1, opt2);
			if (threadsGetContext() == FALSE) {
				frontEndStatusPillbox((BYTE) (itemNum+1), (pillsGetAllianceNum(pb, (BYTE) (itemNum+1))));
/*				frontEndStatusPillbox((BYTE) (itemNum+1), (pillsGetAllianceNum(pb, (BYTE) (itemNum+1)))); */
			}
			needCalc = TRUE;
			break;
		case NPNB_LGM_DEAD:
			/* LGM Died */
			if (itemNum == FALSE) {
				netPNBMessage(owner, langGetText(MESSAGE_LGM_DEAD));
				soundDist(manDyingNear, opt1, opt2);
				if (owner == playerNum) {
					lgmSetIsDead(screenGetLgmFromPlayerNum(playerNum), TRUE);
				}
			} else if (itemNum == TRUE && owner == playerNum) {
				lgmSetIsDead(screenGetLgmFromPlayerNum(playerNum), FALSE);
			}
			break;
		case NPNB_LGM_FARMTREE:
			/* LGM Farmed a tree */
			if (mapGetPos(mp, opt1, opt2) == FOREST) {
				mapSetPos(mp, opt1, opt2, GRASS, FALSE, FALSE);
			}
			needCalc = TRUE;
			soundDist(farmingTreeNear, opt1, opt2);
			break;
		case NPNB_LGM_BUILDROAD:
			/* LGM Built a road */
			mapSetPos(mp, opt1, opt2, ROAD, FALSE, FALSE);
			soundDist(manBuildingNear, opt1, opt2);
			needCalc = TRUE;
			break;
		case NPNB_SAVEMAP:
			netPNBMessage(owner, langGetText(MESSAGE_SAVED_MAP));
    logAddEvent(log_SaveMap, owner, 0, 0, 0, 0, NULL);
			break;
	}
	return needCalc;
}



/*********************************************************
*NAME:          netPNBExtract
*AUTHOR:        John Morrison
*CREATION DATE: 9/3/99
*LAST MODIFIED: 8/1/00
*PURPOSE:
* Extracts the data and plays appropriate sounds. Returns
* FALSE if a pnb-no error occurs else TRUE
*
*ARGUMENTS:
*  pnbc    - Pointer to the netPnbContext object
*  mp      - Pointer to the map sturcture
*  pb      - Pointer to the pillbox sturcture
*  bs      - Pointer to the bases sturcture
*  buff    - Pointer to buffer to put data in
*  dataLen - Length of the data.
*  isTcp   - Did this data arrive via TCP
*********************************************************/
bool netPNBExtract(netPnbContext *pnbc, map *mp, bases *bs, pillboxes *pb, BYTE *buff, int dataLen, bool isTcp) {
  bool returnValue; /* Value to return */
  int count;        /* Looping variable */
  BYTE event;       /* Event Type */
  BYTE itemNum;     /* Item Number */
  BYTE owner;       /* Owner       */
  BYTE id;          /* item Id     */
  BYTE opt1;        /* Optional Data */
  BYTE opt2;
  BYTE opt3;
  bool needCalc;    /* Does the screen need a recalc? */
  bool testCalc;    /* Does the screen need a recalc? */
  bool isServer;    /* Are we a server */
  BYTE eventNibble; 
  bool errOccurred; /* Has an packet avert error occurred? */

  errOccurred = FALSE;
  returnValue = TRUE;
  count = 0;
  needCalc = FALSE;
  isServer = threadsGetContext();

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
	opt3 = buff[count];
	count++;

    /* Process the occurence */
    if (isServer == TRUE) {
      if (netPNBExtractItemServer(pnbc, mp, bs, pb, event, itemNum, owner, opt1, opt2, opt3) == TRUE) {
        netPNBAdd(pnbc, event, itemNum, owner, opt1, opt2, 0);
      }
    } else {
      testCalc = netPNBExtractItemClient(pnbc, mp, bs, pb, event, itemNum, owner, opt1, opt2, opt3);
      if (needCalc == FALSE) {
        needCalc = testCalc;
      }
    }
  }
  
  if (needCalc == TRUE) {
    /* Recalc Screen if required */
    screenReCalc();
  }
  if (returnValue == FALSE) {
    /* Print error */
    messageAdd(networkMessage, (char *) "\0", (char *) "pnb-no");
  }
  if (errOccurred == TRUE) {
    messageAdd(networkMessage, (char *) "\0", (char *) "pnb-a");
  }

//  return returnValue;
  return TRUE;
}

/*********************************************************
*NAME:          netPNBMessage
*AUTHOR:        John Morrison
*CREATION DATE: 14/3/99
*LAST MODIFIED: 14/3/99
*PURPOSE:
* Some messages need to be displayed to the screen. Do so
* here
*
*ARGUMENTS:
*  playerNum - The player number.
*  msg       - Message to display
*********************************************************/
void netPNBMessage(BYTE playerNum, char *msg) {
  char name[FILENAME_MAX];   /* The Player Name */
  char msg2[FILENAME_MAX];   /* Temp buffer */
  char output[FILENAME_MAX]; /* The message */

  output[0] = '\0';
  name[0] = '\0';
  strcpy(msg2, msg);
  playersMakeMessageName(screenGetPlayers(), playerNum, name);
  strcat(output, MESSAGE_QUOTES);
  strcat(output, name);
  strcat(output, MESSAGE_QUOTES);
  strcat(output, msg2);
  messageAdd(newsWireMessage, langGetText2(MESSAGE_NEWSWIRE), output);
}

/*********************************************************
*NAME:          netPNBDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/99
*PURPOSE:
*  Deletes the item for the given number
*
*ARGUMENTS:
*  pnbc    - Pointer to the netPnbContext object
*  itemNum - The item number to delete
*********************************************************/
void netPNBDeleteItem(netPnbContext *pnbc, int itemNum) {
  netPnb prev; /* The previous item to link to the delete items next */
  netPnb del;  /* The item to delete */
  int count;   /* Looping variable */

  if (itemNum == 1) {
    del = (*pnbc)->pnb;
    (*pnbc)->pnb = del->next;
    Dispose(del);
  } else {
    count = 1;
    prev = (*pnbc)->pnb;
    while (count < (itemNum-1)) {
      prev = NetPNBTail(prev);
      count++;
    }
    del = NetPNBTail(prev);
    prev->next = del->next;
    Dispose(del);
  }
}
