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
*Name:          Snapshot
*Filename:      snapshot.c
*Author:        John Morrison
*Purpose:
*  Sets up the snapshots used for jumping forwards and
*  backwards when fast forward or rewind is pressed.
*********************************************************/

#include "global.h"
#include "snapshot.h"

/*
typedef struct snapshotObj *snapshot;
struct snapshotObj {
  snapshot next;
  unsigned long filePos;
  unsigned long time;
}; */


/*********************************************************
*NAME:          snapshotCreate
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
* Creates the snapshot list. Returns empty snapshot list
*
*ARGUMENTS:
*
*********************************************************/
snapshot snapshotCreate() {
  return NULL;
}

/*********************************************************
*NAME:          snapshotCount
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
* Returns the number of snapshots we have.
*
*ARGUMENTS:
*
*********************************************************/
int snapshotCount(snapshot *value) {
  int returnValue = 0;
  snapshot q = *value;

  while (q != NULL) {
    q = q->next;
    returnValue++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          snapshotDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
* Destroys the snapshots we have created
*
*ARGUMENTS:
* value - Snapshot to destroy
*********************************************************/
void snapshotDestroy(snapshot *value) {
  snapshot q;

  while ((*value) != NULL) {
    q = *value;
    *value = q->next;
    Dispose(q);
  }
}

/*********************************************************
*NAME:          snapshotExists
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
* Returns whether a snapshot exists at this time
*
*ARGUMENTS:
* value - Snapshot to check
* filePos - Unuesd variable
* time    - Time in game ticks
*********************************************************/
bool snapshotExist(snapshot *value, unsigned long filePos, unsigned long time) {
  bool returnValue = FALSE;
  snapshot q;

  q = (*value);
  while (returnValue == FALSE && q != NULL) {
    if (q->time == time) {
      return TRUE;
    }
    q = q->next;
  }

  return FALSE;
}

/*********************************************************
*NAME:          snapshotAdd
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
* Adds a new snapshot to the snapshot list
*
*ARGUMENTS:
* value - Snapshot list to add to
* filePos - File position this snapshot is at
* time - Time in ticks this snapshot is at
* key - Key at this snapshot
* pteams - Player Teams to copy
*********************************************************/
void snapshotAdd(snapshot *value, unsigned long filePos, unsigned long time, BYTE key, BYTE *pTeams) {
  snapshot q;
  snapshot add;
  snapshot prev = NULL;
  bool found = FALSE;
  BYTE count = 0;

  // Make sure we aren't there
  if (snapshotExist(value, filePos, time) == TRUE) {
    return;
  }

  if ((*value) == NULL) {
    q = New(q);
    q->filePos = filePos;
    q->time = time;
    q->key = key;
    q->next = NULL;
    while (count < MAX_TANKS) {
      q->pTeams[count] = pTeams[count];
      count++;
    }
    (*value) = q;
  } else {
    // Add it based on time
    q = *value;
    while (q != NULL && found == FALSE) {
      prev = q;

      if (q->time > time && prev == NULL) {
        // Must be the first element
        add = New(add);
        add->filePos = filePos;
        add->time = time;
        add->key = key;
        add->next = (*value);
        while (count < MAX_TANKS) {
          add->pTeams[count] = pTeams[count];
          count++;
        }

        (*value) = add;
        found = TRUE;
      } else if (q->time > time && prev->time < time) {
        add = New(add);
        add->filePos = filePos;
        add->time = time;
        add->key = key;
        add->next = q;
        prev->next = add;
        while (count < MAX_TANKS) {
          add->pTeams[count] = pTeams[count];
          count++;
        }

        found = TRUE;
      }
      q = q->next;
    }

    // Add at the end, new highest position
    if (found == FALSE) {
      q = New(q);
      q->filePos = filePos;
      q->time = time;
      q->key = key;
      while (count < MAX_TANKS) {
        q->pTeams[count] = pTeams[count];
        count++;
      }
      prev->next = q;
      q->next = NULL;
    }
  }
}

/*********************************************************
*NAME:          snapshotFoward
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
* Moves forward to the next snapshot from the given time
* Returns whether a snapshot exists forward of this point.
*
*ARGUMENTS:
* value - Snapshot list to add to
* filePos - File position that the foward snapshot is
* time - Time in ticks that the foward (to check then set)
* key - Key at this snapshot to set to
* pteams - Player Teams to copy to set to
*********************************************************/
bool snapshotForward(snapshot *value, unsigned long *filePos, unsigned long *time, BYTE *key, BYTE **pTeams) {
  bool returnValue = FALSE;
  snapshot q;

  q = *value;
  while (returnValue == FALSE && q != NULL) {
    if (q->time > *time) {
      returnValue = TRUE;
      *time = q->time;
      *filePos = q->filePos;
      *key = q->key;
      *pTeams = q->pTeams;
    }
    q = q->next;
  }

  return returnValue;
}

/*********************************************************
*NAME:          snapshotBackwards
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
* Moves backwards to the previous snapshot from the given time
* Returns whether a snapshot exists backwards of this point.
*
*ARGUMENTS:
* value - Snapshot list to add to
* filePos - File position that the foward snapshot is
* time - Time in ticks that the foward (to check then set)
* key - Key at this snapshot to set to
* pteams - Player Teams to copy to set to
*********************************************************/
bool snapshotBackwards(snapshot *value, unsigned long *filePos, unsigned long *time, BYTE *key, BYTE **pTeams) {
  bool returnValue = FALSE;
  snapshot prev;
  snapshot q;

  if (*value == NULL || *time == 0) {
    return FALSE;
  } else if ((*value)->time > *time) {
    return FALSE;
  } else {
    prev = *value;
    q = prev->next;
    while (returnValue == FALSE && q != NULL) {
      if (prev->time < *time && q->time >= *time) {
        returnValue = TRUE;
        *time = prev->time;
        *filePos = prev->filePos;
        *key = prev->key;
        *pTeams = prev->pTeams;
      }
      prev = q;
      q = q->next;
    }

    if (returnValue == FALSE) {
      returnValue = TRUE;
      *time = prev->time;
      *filePos = prev->filePos;
      *key = prev->key;
      *pTeams = prev->pTeams;
    }
  }
  return returnValue;

}
