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
*Name:          Shells
*Filename:      shells.h
*Author:        John Morrison
*Creation Date: 25/12/98
*Last Modified:  23/9/00
*Purpose:
*  Responsable for Shells tracking/collision detect etc.
*********************************************************/

#ifndef SHELLS_H
#define SHELLS_H

#include "global.h"
#include "types.h"
#include "util.h"
#include "screenbullet.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define ShellsHeadX(list) ((list)->x);
#define ShellsHeadY(list) ((list)->y);
#define ShellsHeadAngle(list) (NonEmpty(list),(list)->angle);
#define ShellsHeadLength(list) ((list)->length);
#define ShellsTail(list) ((list)->next);

/* Shell life is 2 times map length */
#define SHELL_LIFE 8

/* Shells die when there length equals */
#define SHELL_DEATH 0

/* Shell Speed is 32 world units per tick */
#define SHELL_SPEED 32

/* Shells have 4 frames for explosions so add 4 to their life spans */
#define SHELL_START_EXPLODE 8

/* Fudge factor for shootong offset */
#define SHELL_START_ADD 6


/* Brain Stuff */
#define SHELLS_BRAIN_FRIENDLY 0
#define SHELLS_BRAIN_NEUTRAL 2
#define SHELLS_BRAIN_HOSTILE 1
#define SHELLS_BRAIN_OBJECT_TYPE 1


/* Type structure */

typedef struct shellsObj *shells;
struct shellsObj {
  shells next;      /* Pointer to the next shell */
  shells prev;      /* Pointer to previous item */
  WORLD x;          /* Co-ords of the shell */
  WORLD y;
  TURNTYPE angle;   /* The angle the shell is firing */
  BYTE length;      /* Number of map squares for the shell to fire */
  BYTE owner;       /* Who owns the shell */
  bool onBoat;      /* Was the shell launched from a boat */
  bool packSent;    /* Has this shell been included in a network packet yet */
  BYTE creator;     /* Creator machines player Number */
  bool shellDead;   /* Used to over come the if shell dies straight away and 
                       hasn't been sent it never does. So we mark it dead
                       and it doesn't get updated any more but exists till
                       it gets sent (ie packSent == TRUE) */
};


typedef struct shellsNetHitObj *shellsNetHit;
struct shellsNetHitObj {
  shellsNetHit next; /* Next item */
  TURNTYPE angle;    /* Angle the shell wa traveling on */
  BYTE playerHit;    /* The player hit */
  BYTE hitBy;        /* The player it was hit by */
  BYTE bmx;          /* X and Y positions */
  BYTE bmy;
};

/* Prototypes */

/*********************************************************
*NAME:          shellsCreate
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 25/12/98
*PURPOSE:
*  Sets up the shells data structure
*
*ARGUMENTS:
*
*********************************************************/
shells shellsCreate(void);

/*********************************************************
*NAME:          shellsAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 16/2/99
*PURPOSE:
*  Adds an item to the shells data structure. Function
*  also calls the sound playing function
*
*ARGUMENTS:
*  value  - Pointer to the shells data structure
*  x      - X co-ord of the start position
*  y      - Y co-ord of the start position
*  angle  - angle of the shot
*  len    - Length in map units of the item
*  owner  - Who fired the shell
*  onBoat - Was the shell launched from a boat
*********************************************************/
void shellsAddItem(shells *value, WORLD x, WORLD y, TURNTYPE angle, TURNTYPE len, BYTE owner, bool onBoat);

/*********************************************************
*NAME:          shellsUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 30/10/99
*PURPOSE:
*  Updates each shells position and checks for colisions
*
*ARGUMENTS:
*  value    - Pointer to the shells data structure
*  mp       - Pointer to the map Structure
*  pb       - Pointer to the pillbox Structure
*  tk       - Pointer to an array of tank structures
*  numTanks - Number of tanks in the array
*  isServer - TRUE if we are a server
*********************************************************/
void shellsUpdate(shells *value, map *mp, pillboxes *pb, bases *bs, tank *tk, BYTE numTanks, bool isServer);

/*********************************************************
*NAME:          shellsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 25/12/98
*PURPOSE:
*  Destroys and frees memory for the shells data structure
*
*ARGUMENTS:
*  value - Pointer to the shells data structure
*********************************************************/
void shellsDestroy(shells *value);

/*********************************************************
*NAME:          shellsDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 25/12/98
*PURPOSE:
*  Deletes the value from the master list
*
*ARGUMENTS:
*  master  - The master list of all shells
*  value   - Pointer to the shells to delete. Also puts
*            next shell its position
*********************************************************/
void shellsDeleteItem(shells *master, shells *value);

/*********************************************************
*NAME:          shellsCalcScreenBullets
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Adds items to the sceenBullets data structure if they
*  are on screen
*
*ARGUMENTS:
*  value    - Pointer to the shells data structure
*  sBullet  - The screenBullets Data structure
*  leftPos  - X Map offset start
*  rightPos - X Map offset end
*  top      - Y Map offset end
*  bottom   - Y Map offset end
*********************************************************/
void shellsCalcScreenBullets(shells *value, screenBullets *sBullets, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

/*********************************************************
*NAME:          shellsCalcCollision
*AUTHOR:        John Morrison
*CREATION DATE: 29/12/98
*LAST MODIFIED: 30/10/99
*PURPOSE:
*  Returns whether the collision has occured.
*  
*ARGUMENTS:
*  map      - Pointer to the Map Structure
*  pb       - Pointer to the pillboxes structure
*  tk       - Pointer to an array of tank structures
*  bs       - Pointer to the bases structure
*  xValue   - X position
*  yValue   - Y position
*  angle    - The angle the shell is travelling
*  owner    - Who fired the shell
*  onBoat   - Was the shell launched from a boat
*  numTanks - Number of tanks in the array
*  isServer - TRUE if we are a server
*********************************************************/
bool shellsCalcCollision(map *mp, pillboxes *pb, tank *tk, bases *bs, WORLD *xValue, WORLD *yValue, TURNTYPE angle, BYTE owner, bool onBoat, BYTE numTanks, bool isServer);

/*********************************************************
*NAME:          shellsCheckRoad
*AUTHOR:        John Morrison
*CREATION DATE: 6/1/99
*LAST MODIFIED: 6/1/99
*PURPOSE:
*  A shell has been fired from a boat and has hit a road.
*  This function returns whether the road should be kept
*  as road or destroyed and replaced as river
*  
*ARGUMENTS:
*  mp    - Pointer to the Map Data structure
*  pb    - Pointer to the pillbox structure
*  bs    - Pointer to the bases structure
*  mapX  - X position of the hit
*  mapY  - Y position of the hit
*  angle - The angle the shell is travelling
*********************************************************/
BYTE shellsCheckRoad(map *mp, pillboxes *pb, bases *bs, BYTE mapX, BYTE mapY, TURNTYPE dir);

/*********************************************************
*NAME:          shellsNetMake
*AUTHOR:        John Morrison
*CREATION DATE: 6/3/99
*LAST MODIFIED: 8/9/00
*PURPOSE:
*  When we have the token we inform all the players of
*  shells we have fired since last time we had the token.
*  Returns the length of the data created
*  
*ARGUMENTS:
*  value       - Pointer to shells structure
*  buff        - Pointer to a buffer to hold the shells 
*                net data
*  noPlayerNum - If the shells ->creator equals this
*                do not send it
*  sentState   - What to set the send state to
*********************************************************/
BYTE shellsNetMake(shells *value, BYTE *buff, BYTE noPlayerNum, bool sentState);

/*********************************************************
*NAME:          shellsNetExtract
*AUTHOR:        John Morrison
*CREATION DATE:  6/3/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
* Network shells data have arrived. Add them to our 
* shells structure here.
*  
*ARGUMENTS:
*  value    - Pointer to shells structure
*  pb       - Pointer to the pillboxes structure
*  buff     - Pointer to a buffer to hold the shells 
*             net data
*  dataLen  - Length of the data
*  isServer - TRUE if we are the game server.
*********************************************************/
void shellsNetExtract(shells *value, pillboxes *pb, BYTE *buff, BYTE dataLen, bool isServer);

/*********************************************************
*NAME:          shellsGetBrainShellsInRect
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 28/11/99
*PURPOSE:
*  Makes the brain shell info for each shell inside the
*  rectangle formed by the function parameters.
*
*ARGUMENTS:
*  value     - Pointer to the shells structure
*  leftPos   - Left position of rectangle
*  rightPos  - Right position of rectangle
*  topPos    - Top position of rectangle
*  bottomPos - Bottom position of rectangle
*********************************************************/
void shellsGetBrainShellsInRect(shells *value, BYTE leftPos, BYTE rightPos, BYTE topPos, BYTE bottomPos);

#endif /* SHELLS_H */
