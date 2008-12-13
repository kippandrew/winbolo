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
*Name:          Allience
*Filename:      Allience.c
*Author:        John Morrison
*Creation Date: 18/02/99
*Last Modified: 25/07/04
*Purpose:
*  Handles allience. Who is allied to who etc. Uses a 
*  simple set
*********************************************************/

#include "global.h"
#include "allience.h"

/*********************************************************
*NAME:          allienceCreate
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Creates an allience struncture
*
*ARGUMENTS:
*
*********************************************************/
allience allienceCreate(void) {
  return NULL;
}

/*********************************************************
*NAME:          allienceDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Destroys a allience structure
*
*ARGUMENTS:
*  value - The allience structure to destroy
*********************************************************/
void allienceDestroy(allience *value) {
  allience q;

  while (!IsEmpty(*value)) {
    q = *value;
    *value = AllienceTail(q);
    Dispose(q);
  }
}

/*********************************************************
*NAME:          allienceAdd
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Adds an player to an allience (if doesn't exist already)
*
*ARGUMENTS:
*  value     - The allience structure to add to
*  playerNum - The player number to add
*********************************************************/
void allienceAdd(allience *value, BYTE playerNum) {
  allience q;

  if ((allienceExist(value, playerNum)) == FALSE) {
    /* Doesn't exist yet. Add */
    New(q);
    q->playerNum = playerNum;
    q->next = *value;
    *value = q;
  }
}

/*********************************************************
*NAME:          allienceRemove
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Removes an player from an allience
*
*ARGUMENTS:
*  value     - The allience structure to remove from 
*  playerNum - The player number to add
*********************************************************/
void allienceRemove(allience *value, BYTE playerNum) {
  allience q;
  allience prev;
  BYTE test;     /* Number we are testing */
  bool first;    /* Is first item */

  first = TRUE;
  if ((allienceExist(value, playerNum)) == TRUE) {
    q = *value;
    prev = q;
    test = AllienceHead(q);
    while (test != playerNum) {
      first = FALSE;
      prev = q;
      q = AllienceTail(q);
      test = AllienceHead(q);
    }
    if (first == FALSE) {
      prev->next = q->next;
    } else {
      (*value) = (*value)->next;
    }
    Dispose(q);
  }
}

/*********************************************************
*NAME:          allienceExist
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns whether a player number exist in this allience
*
*ARGUMENTS:
*  value     - The allience structure to remove from 
*  playerNum - The player number to add
*********************************************************/
bool allienceExist(allience *value, BYTE playerNum) {
  bool returnValue; /* Value to return */
  allience q;
  BYTE test;

  q = *value;
  returnValue = FALSE;
  while (returnValue == FALSE && NonEmpty(q)) {

    test = AllienceHead(q);
    if (test == playerNum) {
      returnValue = TRUE;
    }
    q = AllienceTail(q);
  }
  return returnValue;
}

/*********************************************************
*NAME:          allienceNumAllies
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns the number of allies a player has
*
*ARGUMENTS:
*  value     - The allience structure to remove from 
*********************************************************/
BYTE allienceNumAllies(allience *value) {
  BYTE returnValue; /* Value to return */
  allience q;

  q = *value;
  returnValue = 0;
  while (NonEmpty(q)) {
    returnValue++;
    q = AllienceTail(q);
  }

  return returnValue;
}

/*********************************************************
*NAME:          allienceReturnNum
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns the player allience is allied with at num
*
*ARGUMENTS:
*  value - The allience structure to remove from 
*  num   - Position in the structure to get
*********************************************************/
BYTE allienceReturnNum(allience *value, BYTE num) {
  BYTE returnValue; /* Value to return */
  BYTE count;
  allience q;

  count = 0;
  returnValue = NEUTRAL;

  if (allienceNumAllies(value) >= num) {
    q = *value;
    while (NonEmpty(q) && count < num) {
      count++;
      q = AllienceTail(q);
    }
    if (NonEmpty(q)) {
      returnValue = q->playerNum;
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          allianceMakeLogAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 25/07/04
*LAST MODIFIED: 25/07/04
*PURPOSE:
* Creates the alliance log buffer. Returns the length
* of the buffer. Format is buff[0] number of allies
* each byte after is the alliance number.
* 
*
*ARGUMENTS:
*  value - The allience structure to remove from 
*  buff  - Buffer to write into
*********************************************************/
BYTE allianceMakeLogAlliance(allience *value, BYTE *buff) {
  BYTE returnValue = 1; /* Value to return */
  allience q;

  q = *value;
  while (NonEmpty(q)) {
    buff[returnValue] = q->playerNum;
    returnValue++;
    q = AllienceTail(q);
  }

  buff[0] = returnValue-1;
  return returnValue;
}
