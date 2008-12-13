/*
 * $Id$
 *
 * Copyright (c) 2000-2008 John Morrison.
 * Copyright (c) 2000-2008 Andrew Roth.
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
*Name:          Bans
*Filename:      bans.h
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for holding the list of banned useres
*********************************************************/

#ifndef _BANS_H
#define _BANS_H

#include "global.h"


#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define BansTail(list) ((list)->next);

/* Type structure */

typedef struct bansObj *bansList;
struct bansObj {
  bansList next; /* Next item */ 
  bansList prev; /* prev item */ 
  char name[FILENAME_MAX];
};


/*********************************************************
*NAME:          bansCreate
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 18/1/00
*PURPOSE:
* Creates the bans
*
*ARGUMENTS:
*  fileName - Filename to load with bans (NULL for none)
*             Format: 1 line per ban
*********************************************************/
void bansCreate(char *fileName);

/*********************************************************
*NAME:          bansDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 18/1/00
*PURPOSE:
* Destroys the bans data structure. If filename is not
* NULL it saves it to a file
*
*ARGUMENTS:
*  fileName - Filename to save bans list to
*********************************************************/
void bansDestroy(char *fileName);

/*********************************************************
*NAME:          bansAddBan
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 18/1/00
*PURPOSE:
* Adds a ban
*
*ARGUMENTS:
*  name - Name to add
*********************************************************/
void bansAddBan(char *name);

/*********************************************************
*NAME:          bansExist
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 18/1/00
*PURPOSE:
* Returns TRUE if a ban is inplace against a user. A ban
* exists if it is a substring of an item in the bans list
*
*ARGUMENTS:
*  name - Name to check
*********************************************************/
bool bansExist(char *name);

/*********************************************************
*NAME:          bansRemoveBan
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/00
*LAST MODIFIED: 18/1/00
*PURPOSE:
* Removes a ban on a name
*
*ARGUMENTS:
*  name - Name to add
*********************************************************/
void bansRemoveBan(char *name);

#endif /* _BANS_H */
