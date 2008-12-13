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
*Name:          Pillbox
*Filename:      pillbox.c
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 11/11/00
*Purpose:
*  Provides operations on pillbox and pillboxes
*********************************************************/

/* Includes */
#include "global.h"
#include "tilenum.h"
#include "pillbox.h"
#include "backend.h"
#include "players.h"

/*********************************************************
*NAME:          pillsCreate
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Creates and initilises the pillboxes structure.
*  Sets number of pillboxes to zero
*
*ARGUMENTS:
*  value - Pointer to the map file
*********************************************************/
void pillsCreate(pillboxes *value) {
  New(*value);
  ((*value)->numPills) = 0;
}


/*********************************************************
*NAME:          pillsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Destroys the pills data structure. Also frees memory.
*
*ARGUMENTS:
*  value - Pointer to the pills structure
*********************************************************/
void pillsDestroy(pillboxes *value) {
  Dispose(*value);
}

/*********************************************************
*NAME:          pillsSetNumPills
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets a specific pill with its item data
*
*ARGUMENTS:
*  value    - Pointer to the pillbox structure
*  numPills - The number of pills
*********************************************************/
void pillsSetNumPills(pillboxes *value, BYTE numPills) {
  if (numPills > 0 && numPills <= MAX_PILLS) {
    (*value)->numPills = numPills;
  }
}

/*********************************************************
*NAME:          pillsGetNumPills
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Gets the number of pills in the structure
*
*ARGUMENTS:
*  value    - Pointer to the pillbox structure
*********************************************************/
BYTE pillsGetNumPills(pillboxes *value) {
  return (*value)->numPills;
}

/*********************************************************
*NAME:          pillsSetPill
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets a specific pill with its item data
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  item    - Pointer to a pillbox
*  pillNum - The pillbox number
*********************************************************/
void pillsSetPill(pillboxes *value, pillbox *item, BYTE pillNum) {
  if (pillNum > 0 && pillNum  <= (*value)->numPills) {
    pillNum--;
    (((*value)->item[pillNum]).x) = item->x;
    (((*value)->item[pillNum]).y) = item->y;
    (((*value)->item[pillNum]).owner) = item->owner;
    (((*value)->item[pillNum]).armour) = item->armour;
    (((*value)->item[pillNum]).speed) = item->speed;
    (((*value)->item[pillNum]).inTank) = item->inTank;
  }
}

/*********************************************************
*NAME:          pillsExistPos
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Returns whether a pillbox exist at a specific location
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
bool pillsExistPos(pillboxes *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  returnValue = FALSE;
  count = 0;
  while (returnValue == FALSE && count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && (*value)->item[count].inTank == FALSE) {
      returnValue = TRUE;
    }
    count++;
  }

  return returnValue;
}


// Assumes that the pillbox exists otherwise returns 0
BYTE pillsItemNumAt(pillboxes *value, BYTE xValue, BYTE yValue) {
  BYTE count;       /* Looping Variable */

  count = 0;
  while (count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && (*value)->item[count].inTank == FALSE) {
      return count;
    }
    count++;
  }

  return 0;
}


/*********************************************************
*NAME:          pillsGetPill
*AUTHOR:        John Morrison
*CREATION DATE:   9/2/99
*LAST MODIFIED: 11/11/00
*PURPOSE:
*  Gets a specific pill.
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  item    - Pointer to a pillbox
*  pillNum - The pillbox number
*********************************************************/
void pillsGetPill(pillboxes *value, pillbox *item, BYTE pillNum) {
  if (pillNum > 0 && pillNum  <= (*value)->numPills) {
    pillNum--;
    item->x = ((*value)->item[pillNum]).x;
    item->y = ((*value)->item[pillNum]).y;
    item->owner = ((*value)->item[pillNum]).owner;
    item->armour = ((*value)->item[pillNum]).armour;
    item->speed = ((*value)->item[pillNum]).speed;
  }
}


void pillsDeletePill(pillboxes *value, BYTE x, BYTE y) {
  BYTE count = 0;
  BYTE count2;
  
  while (count < (*value)->numPills) {
    if ((*value)->item[count].x == x && (*value)->item[count].y == y) {
      count2 = count;
      while (count2 < (*value)->numPills - 1) {
        (*value)->item[count2].x = (*value)->item[count2+1].x;
        (*value)->item[count2].y = (*value)->item[count2+1].y;
        (*value)->item[count2].speed = (*value)->item[count2+1].speed;
        (*value)->item[count2].owner = (*value)->item[count2+1].owner;
        (*value)->item[count2].armour = (*value)->item[count2+1].armour;
        count2++;
      }
      (*value)->numPills--;
    }
    count++;
  }
}

/*********************************************************
*NAME:          pillsGetScreenHealth
*AUTHOR:        John Morrison
*CREATION DATE: 30/10/98
*LAST MODIFIED: 30/10/98
*PURPOSE:
*  Returns the health of a pillbox as a screen define.
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
BYTE pillsGetScreenHealth(pillboxes *value, BYTE xValue, BYTE yValue) {
  bool done;        /* Finished searching */
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  done = FALSE;
  count = 0;
  returnValue = PILL_EVIL_15;

  while (done == FALSE && count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && (*value)->item[count].inTank == FALSE) {
      /* Pillbox has been Hit */
      done = TRUE;
      
      if (playersIsAllie((*value)->item[count].owner, playersGetSelf() ) == FALSE) {
        switch((*value)->item[count].armour) {
        case PILLBOX_15:
          returnValue = PILL_EVIL_15;
          break;
        case PILLBOX_14:
          returnValue = PILL_EVIL_14;
          break;
        case PILLBOX_13:
          returnValue = PILL_EVIL_13;
          break;
        case PILLBOX_12:
          returnValue = PILL_EVIL_12;
          break;
        case PILLBOX_11:
          returnValue = PILL_EVIL_11;
          break;
        case PILLBOX_10:
          returnValue = PILL_EVIL_10;
          break;
        case PILLBOX_9:
          returnValue = PILL_EVIL_9;
          break;
        case PILLBOX_8:
          returnValue = PILL_EVIL_8;
          break;
        case PILLBOX_7:
          returnValue = PILL_EVIL_7;
          break;
        case PILLBOX_6:
          returnValue = PILL_EVIL_6;
          break;
        case PILLBOX_5:
          returnValue = PILL_EVIL_5;
          break;
        case PILLBOX_4:
          returnValue = PILL_EVIL_4;
          break;
        case PILLBOX_3:
          returnValue = PILL_EVIL_3;
          break;
        case PILLBOX_2:
          returnValue = PILL_EVIL_2;
          break;
        case PILLBOX_1:
          returnValue = PILL_EVIL_1;
          break;
        case PILLBOX_0:
          returnValue = PILL_EVIL_0;
          break;
        }
      } else {
        switch((*value)->item[count].armour) {
        case PILLBOX_15:
          returnValue = PILL_GOOD_15;
          break;
        case PILLBOX_14:
          returnValue = PILL_GOOD_14;
          break;
        case PILLBOX_13:
          returnValue = PILL_GOOD_13;
          break;
        case PILLBOX_12:
          returnValue = PILL_GOOD_12;
          break;
        case PILLBOX_11:
          returnValue = PILL_GOOD_11;
          break;
        case PILLBOX_10:
          returnValue = PILL_GOOD_10;
          break;
        case PILLBOX_9:
          returnValue = PILL_GOOD_9;
          break;
        case PILLBOX_8:
          returnValue = PILL_GOOD_8;
          break;
        case PILLBOX_7:
          returnValue = PILL_GOOD_7;
          break;
        case PILLBOX_6:
          returnValue = PILL_GOOD_6;
          break;
        case PILLBOX_5:
          returnValue = PILL_GOOD_5;
          break;
        case PILLBOX_4:
          returnValue = PILL_GOOD_4;
          break;
        case PILLBOX_3:
          returnValue = PILL_GOOD_3;
          break;
        case PILLBOX_2:
          returnValue = PILL_GOOD_2;
          break;
        case PILLBOX_1:
          returnValue = PILL_GOOD_1;
          break;
        case PILLBOX_0:
          returnValue = PILL_GOOD_0;
          break;
        }
      }

    }
    count++;
  }

  return returnValue;
}

void pillsSetHealth(pillboxes *value, BYTE pillNum, BYTE health) {
  (*value)->item[pillNum].armour = health;
}

void pillsSetInTank(pillboxes *value, BYTE pillNum, bool inTank) {
  (*value)->item[pillNum].inTank = inTank;
}

void pillsSetPos(pillboxes *value, BYTE pillNum, BYTE mx, BYTE my) {
  (*value)->item[pillNum].x = mx;
  (*value)->item[pillNum].y = my;
}

/*********************************************************
*NAME:          pillsSetPillOwner
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 16/2/99
*PURPOSE:
* Sets the pillbox pillNum to owner. Returns the previous
* owner. If migrate is set to TRUE then it has migrated 
* from a alliance when a player left and we shouldn't 
* make a message
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  pillNum - The pillbox number to apply it to
*  owner   - The new owner
*  migrate - TRUE if it is migrating.
*********************************************************/
BYTE pillsSetPillOwner(pillboxes *value, BYTE pillNum, BYTE owner, bool migrate) {
  /* Message stuff */
  char oldOwner[FILENAME_MAX];
  char messageStr[FILENAME_MAX];
  BYTE returnValue; /* Value to return */
  
  oldOwner[0] = '\0';
  messageStr[0] = '\0';

  returnValue = NEUTRAL;
  returnValue = (*value)->item[pillNum].owner;
  (*value)->item[pillNum].owner = owner;
  /* Make the message if required */
  if (returnValue == NEUTRAL && migrate == FALSE) {
    /* Neutral pill */
    playersMakeMessageName(owner, messageStr);
    strcat(messageStr, " just captured a neutral pillbox");
    messageAdd(messageStr);
  } else if (owner == NEUTRAL) {
    /* Do nothing */
  } else if (playersIsAllie(returnValue, owner) == FALSE && migrate == FALSE) {
    /* Stole pill */
    playersMakeMessageName(owner, messageStr);
    strcat(messageStr, " just stole pillbox from ");
    playersGetPlayerName(returnValue, oldOwner);
    strcat(messageStr, oldOwner);
    messageAdd(messageStr);
  }
  (*value)->item[pillNum].owner = owner;
//    frontEndStatusPillbox(pillNum, (pillsGetAllianceNum(value, pillNum)));
  return returnValue;
}


bool pillsChooseView(pillboxes *value, int x, int y) {
  BYTE count = 0;
  while (count < (*value)->numPills) {
    if ((*value)->item[count].inTank == FALSE && (*value)->item[count].x == x && (*value)->item[count].y == y && (*value)->item[count].owner != NEUTRAL) {
      playersSetSelf((*value)->item[count].owner);
      return TRUE;
    }
    count++;
  }
  return FALSE;
}

/*********************************************************
*NAME:          pillsSetPillNetData
*AUTHOR:        John Morrison
*CREATION DATE: 27/02/99
*LAST MODIFIED: 27/07/04
*PURPOSE:
* Sets the pills data to buff.
*
*ARGUMENTS:
*  value   - Pointer to the pills structure
*  buff    - Buffer of data to set pills structure to
*  dataLen - Length of the data
*********************************************************/
void pillsSetPillNetData(pillboxes *value, BYTE *buff, BYTE dataLen) {
  BYTE count = 0;
  BYTE len = 1;
  
  (*value)->numPills = buff[0];
  while (count < (*value)->numPills) {
    (*value)->item[count].x = buff[len];
    len++;
    (*value)->item[count].y = buff[len];
    len++;
    (*value)->item[count].armour = buff[len];
    len++;
    (*value)->item[count].owner = buff[len];
    len++;
    (*value)->item[count].speed = buff[len];
    len++;
    (*value)->item[count].inTank = buff[len];
    len++;
/*    (*value)->item[count].reload = buff[len]; */
    len++;
/*    (*value)->item[count].justSeen = buff[len]; */
    len++;
/*    (*value)->item[count].coolDown = buff[len]; */
    len++;
    count++;
  }
}
