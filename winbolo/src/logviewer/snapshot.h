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
*Filename:      snapshot.h
*Author:        John Morrison
*Purpose:
*  Sets up the snapshots used for jumping forwards and
*  backwards when fast forward or rewind is pressed.
*********************************************************/

#ifndef __SNAPSHOT_H
#define __SNAPSHOT_H


/** Snapshot data structure */
typedef struct snapshotObj *snapshot;
struct snapshotObj {
  snapshot next; /* Next snapshot to goto */
  unsigned long filePos; /* Position in the file */
  unsigned long time; /* Time in ticks in the game */
  BYTE pTeams[MAX_TANKS]; /* Team IDs */
  BYTE key; /* Decryption XOR key */
};


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
snapshot snapshotCreate();

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
void snapshotDestroy(snapshot *value);


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
bool snapshotExist(snapshot *value, unsigned long filePos, unsigned long time);

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
void snapshotAdd(snapshot *value, unsigned long filePos, unsigned long time, BYTE key, BYTE *pTeams);


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
bool snapshotForward(snapshot *value, unsigned long *filePos, unsigned long *time, BYTE *key, BYTE **pTeams);

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
bool snapshotBackwards(snapshot *value, unsigned long *filePos, unsigned long *time, BYTE *key, BYTE **pTeams);

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
int snapshotCount(snapshot *value);

#endif /* __SNAPSHOT_H */
