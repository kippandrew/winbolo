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
*Name:          TankExplosions
*Filename:      tankexp.h
*Author:        John Morrison
*Creation Date: 15/1/99
*Last Modified: 3/10/00
*Purpose:
*  Responsable for moving dead tank explosions
*********************************************************/

#ifndef TK_EXPLOSION_H
#define TK_EXPLOSION_H

#include "global.h"
#include "lgm.h"

#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define TkExplosionTail(list) ((list)->next);

/* Things leave the data structure when they cout down to 0 */
#define TK_EXPLODE_DEATH 0
/* What frame number they should display */
#define TANK_EXPLOSION_FRAME 8
#define TK_MOVE_AMOUNT 48
/* 96 */
#define TK_EXPLODE_LENGTH 40
/* 20 */

#define TK_DAMAGE 5

#define TK_SMALL_EXPLOSION 0
#define TK_LARGE_EXPLOSION 1

/* Check for colision numbers. Half the size of the frame - In world units */
#define TK_WIDTH_CHECK 24
#define TK_HEIGHT_CHECK 32

#define TK_UPDATE_TIME 2
/* 4 */

/* Add or remove one map square */
#define TK_MOVE_RIGHT 1
#define TK_MOVE_LEFT -1

/* Type structure */

typedef struct tkExplosionObj *tkExplosion;
struct tkExplosionObj {
  tkExplosion next;  /* Next Explosion */
  tkExplosion prev;  /* Previous Item */
  WORLD x;           /* X position */
  WORLD y;           /* Y position */
  TURNTYPE angle;    /* Direction of travel */
  BYTE length;       /* Distance to travel  */
  BYTE explodeType;  /* Type of the explosion - Big or small */
  bool packSent;     /* Has this tkexplosion been included in a network packet yet */
  bool own;          /* Did we create this explosions or the network? */
  BYTE creator;      /* Creators player number */
};

/* Prototypes */

/*********************************************************
*NAME:          tkExplosionCreate
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Sets up the tkExplosion data structure
*
*ARGUMENTS:
*  tke - Pointer to the tank explosions object
*********************************************************/
void tkExplosionCreate(tkExplosion *tke);

/*********************************************************
*NAME:          tkExplosionDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Destroys and frees memory for the tkExplosion data 
*  structure
*
*ARGUMENTS:
*  tke - Pointer to the tank explosions object
*********************************************************/
void tkExplosionDestroy(tkExplosion *tke);

/*********************************************************
*NAME:          tkExplosionAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
*  Adds an item to the tkExplosion data structure. 
*
*ARGUMENTS:
*  tke         - Pointer to the tank explosions object
*  x           - World X Co-orindate
*  y           - World X Co-orindate
*  angle       - Angle of travel
*  length      - Length of travel 
*  explodeType - Type of explosions (big - small)
*********************************************************/
void tkExplosionAddItem(tkExplosion *tke, WORLD x, WORLD y, TURNTYPE angle, BYTE length, BYTE explodeType);

/*********************************************************
*NAME:          tkExplosionUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 15/01/99
*LAST MODIFIED: 04/04/02
*PURPOSE:
*  Updates each tkExplosion position
*
*ARGUMENTS:
*  tke    - Pointer to the tank explosions object
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the bases structure
*  lgms   - Array of lgms
*  numLgm - Number of lgms in the array
*********************************************************/
void tkExplosionUpdate(tkExplosion *tke, map *mp, pillboxes *pb, bases *bs, lgm **lgms, BYTE numLgm);

/*********************************************************
*NAME:          tkExplosionDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Deletes the item for the given number
*
*ARGUMENTS:
*  tke     - Pointer to the tank explosions object
*  itemNum - The item number to get
*********************************************************/
void tkExplosionDeleteItem(tkExplosion *tkem, tkExplosion *value);

/*********************************************************
*NAME:          tkExplosionCalcScreenBullets
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/98
*LAST MODIFIED: 15/1/98
*PURPOSE:
*  Adds items to the sceenBullets data structure if they
*  are on screen
*
*ARGUMENTS:
*  tke       - Pointer to the tank explosions object
*  sBullet   - The screenBullets Data structure
*  leftPos   - X Map offset start
*  rightPos  - X Map offset end
*  topPos    - Y Map offset end
*  bottomPos - Y Map offset end
*********************************************************/
void tkExplosionCalcScreenBullets(tkExplosion *tke, screenBullets *sBullets, BYTE leftPos, BYTE rightPos, BYTE topPos, BYTE bottomPos);

/*********************************************************
*NAME:          tkExplosionCheckRemove
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/98
*LAST MODIFIED: 18/1/98
*PURPOSE:
*  An explosion has happened. Check to see if it should
*  remove from grass/building data structures etc.
*
*ARGUMENTS:
*  tke     - Pointer to the tank explosions object
*  terrain - Terrain type of the sqaure
*  mx      - Map X position 
*  my      - Map Y position
*********************************************************/
void tkExplosionCheckRemove(tkExplosion *tke, BYTE terrain, BYTE mx, BYTE my);

/*********************************************************
*NAME:          tkExplosionBigExplosion
*AUTHOR:        John Morrison
*CREATION DATE: 18/01/98
*LAST MODIFIED: 04/04/02
*PURPOSE:
*  An explosion has happened. do all the work involved 
*  to do it.
*
*ARGUMENTS:
*  tke     - Pointer to the tank explosions object
*  mp      - Pointer to map structure 
*  pb      - Pointer to pillboxes structure
*  bs      - Pointer to bases strucutre
*  mx      - Map X position 
*  my      - Map Y position
*  moveX   - Moving X direction (positive/Negative)
*  moveY   - Moving Y direction (positive/Negative)
*  own     - Do we own this tkExplosion?
*  lgms   - Array of lgms
*  numLgm - Number of lgms in the array
*********************************************************/
void tkExplosionBigExplosion(tkExplosion *tke, map *mp, pillboxes *pb, bases *bs, BYTE mx, BYTE my, int moveX, int moveY, bool own, lgm **lgms, BYTE numLgm);

/*********************************************************
*NAME:          tkExplosionNetMake
*AUTHOR:        John Morrison
*CREATION DATE: 11/3/99
*LAST MODIFIED: 11/3/99
*PURPOSE:
*  When we have the token we inform all the players of
*  tke we have made since last time we had the token.
*  Returns the length of the data created
*  
*ARGUMENTS:
*  tke   - Pointer to the tank explosions object
*  buff  - Pointer to a buffer to hold the shells 
*          net data
*********************************************************/
BYTE tkExplosionNetMake(tkExplosion *tke, BYTE *buff);

/*********************************************************
*NAME:          tkExplosionNetExtract
*AUTHOR:        John Morrison
*CREATION DATE: 11/3/99
*LAST MODIFIED: 14/9/00
*PURPOSE:
* Network tke data have arrived. Add them to our 
* tke structure here.
*  
*ARGUMENTS:
*  tke       - Pointer to the tank explosions object
*  buff      - Pointer to a buffer to hold the shells 
*              net data
*  dataLen   - Length of the data
*  playerNum - This players number 
*********************************************************/
void tkExplosionNetExtract(tkExplosion *tke, BYTE *buff, BYTE dataLen, BYTE playerNum);

#endif /* TK_EXPLOSION_H */
