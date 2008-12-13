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
*Filename:      Netmt.h
*Author:        John Morrison
*Creation Date: 19/3/99
*Last Modified: 21/8/00
*Purpose:
* Handles keeping of mines and terrain changes across 
* the network.
*********************************************************/

#ifndef NET_MNT_H
#define NET_MNT_H

#include "global.h"
#include "tank.h"

/* Defines */

/* Event Types */
#define NMNT_PILLMIGRATE 0
#define NMNT_MINEPLACE 1
#define NMNT_MINETANKPLACE 2
#define NMNT_MINEEXPLOSION 3
#define NMNT_MANRETURN 4
#define NMNT_MANACTION 5
#define NMNT_RUBBLE 6
#define NMNT_SWAMP 7
#define NMNT_KILLME 8
#define NMNT_BASEMIGRATE 9
#define NMNT_BASERELOAD 10
#define NMNT_BUILDBUILDING 11
#define NMNT_BUILDBOAT 12
#define NMNT_DROPBOAT 13
#define NMNT_RUNOVERBOAT 14
#define NMNT_TANKHIT 15

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define NetMNTTail(list) ((list)->next);



/* NetPNB Type */

typedef struct netMNTObj *netMnt;
struct netMNTObj {
  netMnt next;  /* Next item */ 
  BYTE item;    /* Value of this item */
  BYTE owner;   /* Who owns this anyway */
  BYTE id;      /* Item ID */
  BYTE x;       /* Item location */
  BYTE y;
};


typedef struct netMntContextObj *netMntContext;

struct netMntContextObj {
  netMnt nmt;
  BYTE netMntUpto;
};


/* Prototypes */

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
void netMNTCreate(netMntContext *nmtc);

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
void netNMTDestroy(netMntContext *nmtc);

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
void netMNTAdd(netMntContext *nmtc, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2);

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
int netMNTMake(netMntContext *nmtc, BYTE *buff);

/*********************************************************
*NAME:          netMNTExtractServer
*AUTHOR:        John Morrison
*CREATION DATE: 11/11/99
*LAST MODIFIED: 05/04/02
*PURPOSE:
* Extracts a netMNT item as a server. Returns wether this
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
bool netMNTExtractServer(netMntContext *nmtc, map *mp, pillboxes *pb, bases *bs, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2);

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
bool netMNTExtractClient(netMntContext *nmtc, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE event, BYTE itemNum, BYTE owner, BYTE opt1, BYTE opt2);

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
bool netMNTExtract(netMntContext *nmtc, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE *buff, int dataLen, bool isTcp);

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
void netMNTMessage(BYTE playerNum, char *msg);

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
void netMNTDeleteItem(netMntContext *nmtc, int itemNum);


#endif /* NET_MNT_H */
