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
*Filename:      Allience.h
*Author:        John Morrison
*Creation Date: 18/02/99
*Last Modified: 25/07/04
*Purpose:
*  Handles allience. Who is allied to who etc. Uses a 
*  simple set
*********************************************************/

#ifndef ALLIENCE_H
#define ALLIENCE_H

#include "global.h"

/* Defines */
/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define AllienceHead(list) ((list)->playerNum);
#define AllienceTail(list) ((list)->next);



/* Allience */

typedef struct allienceObj *allience;
struct allienceObj {
  allience next;  /* Next item */ 
  BYTE playerNum; /* The player this person is allied to */
};


/* Prototypes */

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
allience allienceCreate(void);

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
void allienceDestroy(allience *value);

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
void allienceAdd(allience *value, BYTE playerNum);

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
void allienceRemove(allience *value, BYTE playerNum);

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
bool allienceExist(allience *value, BYTE playerNum);

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
BYTE allienceNumAllies(allience *value);

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
BYTE allienceReturnNum(allience *value, BYTE num);

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
BYTE allianceMakeLogAlliance(allience *value, BYTE *buff);

#endif /* ALLIENCE_H */
