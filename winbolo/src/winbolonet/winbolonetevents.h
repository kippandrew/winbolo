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
*Name:          WinBolo.net Event
*Filename:      winbolonetevents.h
*Author:        John Morrison
*Creation Date: 04/04/02
*Last Modified: 04/04/02
*Purpose:
*  Responsable for tracking winbolonetEvents
*********************************************************/


#ifndef __WINBOLONET_EVENT
#define __WINBOLONET_EVENT


#include "../bolo/global.h"
#include "winbolonet.h"


/* Defines */
#define WINBOLONET_EVENT_NOITEM 255 /* We have no more items to send */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define winbolonetEventsTail(list) ((list)->next);


/* Types */

typedef struct winboloNetObj *winbolonetEvents;
struct winboloNetObj {
  winbolonetEvents next; /* Next item */ 
  BYTE itemType;
  BYTE keyA[WINBOLONET_KEY_LEN];
  BYTE keyB[WINBOLONET_KEY_LEN];
};


/*********************************************************
*NAME:          winbolonetEventsCreate
*AUTHOR:        John Morrison
*CREATION DATE: 04/04/02
*LAST MODIFIED: 04/04/02
*PURPOSE:
*  Sets up the winbolonetEvents data structure
*
*ARGUMENTS:
*
*********************************************************/
void winbolonetEventsCreate(void);

/*********************************************************
*NAME:          winbolonetEventsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 04/04/02
*LAST MODIFIED: 04/04/02
*PURPOSE:
*  Destroys and frees memory for the winbolonetEvents
*  data structure
*
*ARGUMENTS:
*
*********************************************************/
void winbolonetEventsDestroy(void);

/*********************************************************
*NAME:          winbolonetEventsAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 04/04/02
*LAST MODIFIED: 04/04/02
*PURPOSE:
*  Adds an item to the winbolonetEvents data structure.
*
*ARGUMENTS:
*  itemType - The WinBolo.net Item Event Type
*  keyA     - Key of user A
*  keyB     - Key of user B
*********************************************************/
void winbolonetEventsAddItem(BYTE itemType, BYTE *keyA, BYTE *keyB);

/*********************************************************
*NAME:          winbolonetEventsGetSize
*AUTHOR:        John Morrison
*CREATION DATE: 04/04/02
*LAST MODIFIED: 04/04/02
*PURPOSE:
*  Returns the size of the winbolonetEvents data structure.
*
*ARGUMENTS:
*  itemType - The WinBolo.net Item Event Type
*  keyA     - Key of user A
*  keyB     - Key of user B
*********************************************************/
int winbolonetEventsGetSize();

/*********************************************************
*NAME:          winbolonetEventsRemove
*AUTHOR:        John Morrison
*CREATION DATE: 04/04/02
*LAST MODIFIED: 04/04/02
*PURPOSE:
*  Removes an item from the winbolonetEvents data structure
*  Always returns the first event added. Returns itemType
*  or WINBOLONET_EVENT_NOITEM if empty
*
*ARGUMENTS:
*  keyA     - Buffer to copy key of user A
*  keyB     - Buffer to copy key of user B
*********************************************************/
BYTE winbolonetEventsRemove(BYTE *keyA, BYTE *keyB);

#endif /* __WINBOLONET_EVENT */
