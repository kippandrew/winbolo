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

#ifndef NET_PANDB_H
#define NET_PANDB_H

#include "global.h"

/* Defines */

/* Event Types */
#define NPNB_BASE_CAPTURE 0
#define NPNB_BASE_REFUEL_ARMOUR 1
#define NPNB_BASE_REFUEL_SHELLS 2
#define NPNB_BASE_REFUEL_MINES 3
#define NPNB_BASE_HIT 4
#define NPNB_PILL_CAPTURE 5
#define NPNB_PILL_HIT 6
#define NPNB_PILL_PICKUP 7
#define NPNB_PILL_DROP 8
#define NPNB_PILL_SHOOT 9
#define NPNB_PILL_REPAIR 10
#define NPNB_PILL_DEAD 11
#define NPNB_LGM_DEAD 12
#define NPNB_LGM_FARMTREE 13
#define NPNB_LGM_BUILDROAD 14
#define NPNB_SAVEMAP 15

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define NetPNBTail(list) ((list)->next);



/* NetPNB Type */

typedef struct netPNBObj *netPnb;
struct netPNBObj {
  netPnb next;  /* Next item               */ 
  BYTE item;    /* Value of this item      */
  BYTE owner;   /* Owner of this event     */
  BYTE id;      /* Item Id                 */
  BYTE x;       /* Item X & Y Co-ordinates */
  BYTE y;
};

typedef struct netPnbContextObj *netPnbContext;

struct netPnbContextObj {
  netPnb pnb;
  BYTE netPnbUpto;
};

/* Prototypes */

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
void netPNBCreate(netPnbContext *pnbc);

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
void netPNBDestroy(netPnbContext *pnbc);

/*********************************************************
*NAME:          netPNBAdd
*AUTHOR:        John Morrison
*CREATION DATE:  9/3/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Adds an item to the netPND structure
*
*ARGUMENTS:
*  pnbc    - Pointer to the netPnbContext object
*  event   - Event type
*  itemNum - Item Number which the even occured to
*  owner   - Owner of the event
*  opt1    - Optional location data
*  opt2    - Optional location data
*********************************************************/
void netPNBAdd(netPnbContext *pnbc, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2);

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
int netPNBMake(netPnbContext *pnbc, BYTE *buff);

/*********************************************************
*NAME:          netPNBExtractItemServer
*AUTHOR:        John Morrison
*CREATION DATE: 11/11/99
*LAST MODIFIED:  21/9/00
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
bool netPNBExtractItemServer(netPnbContext *pnbc, map *mp, bases *bs, pillboxes *pb, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2);

/*********************************************************
*NAME:          netPNBExtractItemClient
*AUTHOR:        John Morrison
*CREATION DATE: 11/11/99
*LAST MODIFIED: 11/11/99
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
bool netPNBExtractItemClient(netPnbContext *pnbc, map *mp, bases *bs, pillboxes *pb, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2);

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
bool netPNBExtract(netPnbContext *pnbc, map *mp, bases *bs, pillboxes *pb, BYTE *buff, int dataLen, bool isTcp);

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
void netPNBMessage(BYTE playerNum, char *msg);

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
void netPNBDeleteItem(netPnbContext *pnbc, int itemNum);

#endif /* NET_PANDB_H */
