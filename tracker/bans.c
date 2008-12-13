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
*Filename:      bans.c
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for holding the list of banned useres
*********************************************************/


#include "global.h"
#include "bans.h"

bansList bans;

/*********************************************************
*NAME:          bansCreate
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Creates the bans
*
*ARGUMENTS:
*  fileName - Filename to load with bans (NULL for none)
*             Format: 1 line per ban
*********************************************************/
void bansCreate(char *fileName) {
  FILE *fp;                /* File Pointer */
  char line[FILENAME_MAX]; /* Each entry */

  bans = NULL;
  if (fileName != NULL) {
    fp = fopen(fileName, "r");
    if (fp) {
      fgets(line, sizeof(line), fp);
      while (!feof(fp)) {
        if (line[strlen(line)-1] == NEWLINE_CHAR) {
          line[strlen(line)-1] = EMPTY_CHAR;
        }
        bansAddBan(line);
        fgets(line, sizeof(line), fp);
      }
      fclose(fp);
    }
  }
}

/*********************************************************
*NAME:          bansDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Destroys the bans data structure. If filename is not
* NULL it saves it to a file
*
*ARGUMENTS:
*  fileName - Filename to save bans list to
*********************************************************/
void bansDestroy(char *fileName) {
  bansList q; /* Temp Pointer */
  FILE *fp;   /* File pointer */
  
  fp = NULL;
  if (fileName != NULL) {
    fp = fopen(fileName, "w");
  }

  while (NonEmpty(bans)) {
    if (fp != NULL) {
      fprintf(fp, "%s\n", bans->name);
    }
    q = bans;
    bans = BansTail(bans);
    Dispose(q);
  }

  if (fp != NULL) {
    fclose(fp);
  }
}


/*********************************************************
*NAME:          bansAddBan
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Adds a ban
*
*ARGUMENTS:
*  name - Name to add
*********************************************************/
void bansAddBan(char *name) {
  bansList q; /* Temp Pointer */

  New(q);
  strcpy(q->name, name);
  q->next = bans;
  if (bans != NULL) {
    bans->prev = q;
  }
  q->prev = NULL;
  bans = q;
}


/*********************************************************
*NAME:          bansExist
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Returns TRUE if a ban is inplace against a user. A ban
* exists if it is a substring of an item in the bans list
*
*ARGUMENTS:
*  name - Name to check
*********************************************************/
bool bansExist(char *name) {
  bool returnValue; /* Value to return */
  bansList q;           /* Temp Pointer */

  returnValue = FALSE;
  q = bans;
  while (NonEmpty(q) && returnValue == FALSE) {
    if (strstr(q->name, name) != NULL) {
      returnValue = TRUE;
    }
    q = BansTail(q);
  }

  return returnValue;
}

/*********************************************************
*NAME:          bansRemoveBan
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Removes a ban on a name
*
*ARGUMENTS:
*  name - Name to add
*********************************************************/
void bansRemoveBan(char *name) {
  bool found;      /* Have we found the ban */
  bansList q, del; /* Temp Pointers */
  int nameLen;     /* length of the name */
  

  found = FALSE;
  q = bans;
  nameLen = strlen(name);
  while (NonEmpty(q) && found == FALSE) {
    if (strncmp(q->name, name, nameLen) == 0) {
      found = TRUE;
      del = q;
      q->next = q;
      if (q->prev != NULL) {
        q->prev->next = q->next;
      } else {
        bans = q->next;
      }
      if (q->next != NULL) {
        q->next->prev = q->prev;
      }
      Dispose(del);
    } 
    q = BansTail(q);
  }
}

