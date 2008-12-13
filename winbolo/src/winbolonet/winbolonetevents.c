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
*Filename:      winbolonetevents.c
*Author:        John Morrison
*Creation Date: 04/04/02
*Last Modified: 04/04/02
*Purpose:
*  Responsable for tracking winbolonetEvents
*********************************************************/

#include "../bolo/global.h"
#include "winbolonetevents.h"

winbolonetEvents wbe; /* Winbolo.net Event */

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
void winbolonetEventsCreate(void) {
  wbe = NULL;
}

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
void winbolonetEventsDestroy(void) {
  winbolonetEvents q;

  while (!IsEmpty(wbe)) {
    q = wbe;
    wbe = winbolonetEventsTail(q);
    Dispose(q);
  }
}

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
void winbolonetEventsAddItem(BYTE itemType, BYTE *keyA, BYTE *keyB) {
  winbolonetEvents q;

  q = wbe;
  New (q);
  q->itemType = itemType;
  memcpy(q->keyA, keyA, WINBOLONET_KEY_LEN);
  memcpy(q->keyB, keyB, WINBOLONET_KEY_LEN);
  q->next = wbe;
  wbe = q;
}

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
int winbolonetEventsGetSize() {
  int returnValue;    /* Size to return */
  winbolonetEvents q;

  q = wbe;
  returnValue = 0;
  while (NonEmpty(q)) {
    q = winbolonetEventsTail(q);
    returnValue++;
  }

  return returnValue;
}

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
BYTE winbolonetEventsRemove(BYTE *keyA, BYTE *keyB) {
  BYTE returnValue;       /* Return Value - Item type */
  winbolonetEvents inc;
  winbolonetEvents prev;
  winbolonetEvents prev2;
  inc = wbe;
  prev = NULL;
  prev2 = NULL;
  
  returnValue = WINBOLONET_EVENT_NOITEM;

  if (inc != NULL) {
    while (NonEmpty(inc)) {
      prev2 = prev;
      prev = inc;
      inc = winbolonetEventsTail(inc);
    }

    if (prev != NULL) {
      /* We are not the first item */
      /* Copy and remove it - prev contains the item. prev2 contains the item before this one */
      returnValue = prev->itemType;
      memcpy(keyA, prev->keyA, WINBOLONET_KEY_LEN);
      memcpy(keyB, prev->keyB, WINBOLONET_KEY_LEN);
      if (prev2 != NULL) {
        prev2->next = NULL;
      } else {
        /* We are the first item */
        wbe = NULL;
      }
      Dispose(prev);
    }
  }
  return returnValue;
}

