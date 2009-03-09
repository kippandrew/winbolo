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
*Name:          lgm
*Filename:      lgm.h 
*Author:        John Morrison
*Creation Date: 17/01/99
*Last Modified: 02/02/04
*Purpose:
*  Operations on tanks LGM are handled by this module
*********************************************************/

#ifndef LGM_H
#define LGM_H


/* Includes */
#include "global.h"
#include "types.h"

#define LGM_TREE_REQUEST 0
#define LGM_ROAD_REQUEST 1
#define LGM_BUILDING_REQUEST 2
#define LGM_PILL_REQUEST 3
#define LGM_MINE_REQUEST 4
#define LGM_BOAT_REQUEST 5
/* Man is doing nothing */
#define LGM_IDLE 6

#define LGM_STATE_IDLE 0
#define LGM_STATE_GOING 1
#define LGM_STATE_RETURN 2

#define LGM_COST_ROAD 2
#define LGM_COST_BUILDING 2
#define LGM_COST_REPAIRBUILDING 1
#define LGM_COST_BOAT 20
#define LGM_COST_PILLNEW 4
#define LGM_COST_PILLREPAIR 1
#define LGM_COST_MINE 1 

#define LGM_NO_PILL 37

#define LGM_GATHER_TREE 4

#define LGM_SIZE_X 3
#define LGM_SIZE_Y 4

/* There are 3 frames in animation 0-3 */
#define LGM_MAX_FRAMES 2

/* Blessed area surrounding the tank */
#define LGM_TANKBOAT_LEAVE 144
#define LGM_TANKBOAT_RETURN 160

/* Speed the helicpter flys at */
#define LGM_HELICOPTER_SPEED 3
/* The frame number for the helicopter */
#define LGM_HELICOPTER_FRAME 3

/* 20 ticks to builds something */
#define LGM_BUILD_TIME 20

/* Min and Max distance away from thing to achieve goal */
#define LGM_MIN_GOAL -16
#define LGM_MAX_GOAL 16

/* Min and Max distance away from return to the tank goal */
#define LGM_RETURN_MIN_GOAL (-16 * 5)
#define LGM_RETURN_MAX_GOAL (16 * 5)

/* The LGM's Brain State */
#define LGM_BRAIN_INTANK 0
#define LGM_BRAIN_DEAD 1
#define LGM_BRAIN_MOVING 2 /* Otherwise */

/* LGM Obstructed state */
#define LGM_BRAIN_FREE 0
#define LGM_BRAIN_PARTIAL 1
#define LGM_BRAIN_TOTAL 2

typedef struct lgmObj *lgm;

struct lgmObj {
  WORLD x;         /* X and Y positions */
  WORLD y;     
  WORLD destX;      /* Where the man is heading */
  WORLD destY;      
  BYTE state;      /* Present state 0 = Idle, 1 = Going to do work, 2 = returning to tank */
  BYTE waitTime;   /* Is it counting down waiting for something? */
  bool inTank;     /* Is the builder in the tank */
  bool isDead;     /* Is the builder dead ? */
  BYTE numTrees;   /* Number of trees carrying */
  BYTE numMines;   /* Number of mines carrying */
  BYTE numPills;   /* Number of pills being carried */
  BYTE action;     /* What its current task is */
  BYTE nextX;      /* Map Co-ordinates of next action */
  BYTE nextY; 
  BYTE nextAction; /* Next action to do */
  BYTE frame;      /* Animation frame */
  bool blessX;     /* The lgm blessed X map square (can travel over it) */
  bool blessY;     /* The lgm blessed X map square (can travel over it) */
  BYTE obstructed; /* For brain - 0 = free, 1 = touching wall, 2 = completely stuck */
  BYTE playerNum;  /* Our player Number */
};

/* Prototypes */

/*********************************************************
*NAME:          lgmCreate 
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 3/12/00
*PURPOSE:
*  Sets up the LGM structure
*
*ARGUMENTS:
*
*********************************************************/
lgm lgmCreate(BYTE playerNum);

/*********************************************************
*NAME:          lgmDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
* Destroys the LGM structure
*
*ARGUMENTS:
*  value - Pointer to the lgm sturcture
*********************************************************/
void lgmDestroy(lgm *value);

/*********************************************************
*NAME:          lgmUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Game tick has passed. Update the lgm position
*
*ARGUMENTS:
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the base structure
*  tnk    - Pointer to the tank structure
*********************************************************/
void lgmUpdate(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk);

/*********************************************************
*NAME:          lgmAddRequest
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Adds a new request to the lgm structure
*
*ARGUMENTS:
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  tnk    - Pointer to the tank structure
*  bs     - Pointer to the base structure
*  mapX   - X Co-ordinate of the new action
*  mapY   - Y Co-ordinate of the new action
*  action - What the new action is
*********************************************************/
void lgmAddRequest(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE mapX, BYTE mapY, BYTE action);


/*********************************************************
*NAME:          lgmTankDied
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  The tank has died. Cancel all pending orders if any
*
*ARGUMENTS:
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the base structure
*  tnk    - Pointer to the tank structure
*********************************************************/
void lgmTankDied(lgm *lgman);

/*********************************************************
*NAME:          lgmNewPrimaryRequest
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  A new primary request is wanted. Check if it is
*  possible. If so then make it so.
*
*ARGUMENTS:
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  tnk    - Pointer to the tank structure
*  bs     - Pointer to the base structure
*  mapX   - X Co-ordinate of the new action
*  mapY   - Y Co-ordinate of the new action
*  action - What the new action is
*********************************************************/
void lgmNewPrimaryRequest(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE mapX, BYTE mapY, BYTE action);

/*********************************************************
*NAME:          lgmMoveAway
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Man is moving towrads his destination.
*
*ARGUMENTS:
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the base structure
*  tnk    - Pointer to the tank structure
*********************************************************/
void lgmMoveAway(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk);

/*********************************************************
*NAME:          lgmReturn
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Man is moving towards the tank
*
*ARGUMENTS:
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the base structure
*  tnk    - Pointer to the tank structure
*********************************************************/
void lgmReturn(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk);

/*********************************************************
*NAME:          lgmDoWork
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Man has arrived at objective. Do whatever it is he
*  came to do.
*
*ARGUMENTS:
*  mp - Pointer to the map structure
*  pb - Pointer to the pillbox structure 
*  bs - Pointer to the bases structure 
*********************************************************/
void lgmDoWork(lgm *lgman, map *mp, pillboxes *pb, bases *bs);

/*********************************************************
*NAME:          lgmBackInTank
*AUTHOR:        John Morrison
*CREATION DATE: 18/01/99
*LAST MODIFIED: 01/02/03
*PURPOSE:
*  Man has arrived back in tank. Dump stuff off
*
*ARGUMENTS:
*  mp        - Pointer to the map structure
*  pb        - Pointer to the pillbox structure
*  bs        - Pointer to the base structure
*  tnk       - Pointer to the tank structure
*  sendItems - if TRUE, send the carriend items back 
*              to the client
*********************************************************/
void lgmBackInTank(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnki, bool sendItems);

/*********************************************************
*NAME:          lgmOnScreen
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns wehther the man is on screen
*
*ARGUMENTS:
*  leftPos   - Left bounds of screen
*  rightPos  - Right bounds of screen
*  top    - Top bounds of screen
*  bottom - Bottom bounds of screen
*********************************************************/
bool lgmOnScreen(lgm *lgman, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

/*********************************************************
*NAME:          lgmGetScreenCoords
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns wehther the man is on screen
*
*ARGUMENTS:
*  leftPos - Left bounds of screen
*  topPos  - Top bounds of screen
*  mx      - X Map co ord
*  my      - Y Map co ord
*  px      - X pixel co ord
*  py      - Y pixel co ord
*********************************************************/
void lgmGetScreenCoords(lgm *lgman, BYTE leftPos, BYTE topPos, BYTE *mx, BYTE *my, BYTE *px, BYTE *py, BYTE *frame);

/*********************************************************
*NAME:          lgmDeathCheck
*AUTHOR:        John Morrison
*CREATION DATE: 18/01/99
*LAST MODIFIED: 02/02/04
*PURPOSE:
*  Called when an item explodes to check to see if the
*  lgm should be killed because he is on the screen.
*
*ARGUMENTS:
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillboxes structure
*  bs     - Pointer to the bases structure
*  wx     - X World co ord
*  wy     - Y World co ord
*  owner  - Who owned the firing shell (NEUTRAL for mines)
*********************************************************/
void lgmDeathCheck(lgm *lgman, map *mp, pillboxes *pb, bases *bs, WORLD wx, WORLD wy, BYTE owner);

/*********************************************************
*NAME:          lgmParchutingIn
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Man is parachuting back in.
*
*ARGUMENTS:
*
*********************************************************/
void lgmParchutingIn(lgm *lgman);

/*********************************************************
*NAME:          lgmCheckRemove
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  An building operation has happened. Check to see if 
*  it should remove from grass/building data structures.
*
*ARGUMENTS:
*  terrain - Terrain type of the sqaure
*  mx      - Map X position 
*  my      - Map Y position
*********************************************************/
void lgmCheckRemove(BYTE terrain, BYTE mx, BYTE my);

/*********************************************************
*NAME:          lgmCheckTankBoat
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  This is to check the builder is close to the tank. If 
*  he is then it is considered "blessed" and can freely
*  travel to it.
*
*ARGUMENTS:
*  tnk  - Pointer to the tank structure
*  dist - Distance to check
*********************************************************/
bool lgmCheckTankBoat(lgm *lgman, tank *tnk, WORLD dist);

/*********************************************************
*NAME:          lgmGetMX
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm map X position
*
*ARGUMENTS:
*
*********************************************************/
BYTE lgmGetMX(lgm *lgman);

/*********************************************************
*NAME:          lgmGetMY
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm map Y position
*
*ARGUMENTS:
*
*********************************************************/
BYTE lgmGetMY(lgm *lgman);

/*********************************************************
*NAME:          lgmGetPX
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm pixel X position
*
*ARGUMENTS:
*
*********************************************************/
BYTE lgmGetPX(lgm *lgman);

/*********************************************************
*NAME:          lgmGetPY
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm pixel Y position
*
*ARGUMENTS:
*
*********************************************************/
BYTE lgmGetPY(lgm *lgman);

/*********************************************************
*NAME:          lgmGetFrame
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm animation frame
*
*ARGUMENTS:
*
*********************************************************/
BYTE lgmGetFrame(lgm *lgman);

bool lgmIsOut(lgm *lgman);

/*********************************************************
*NAME:          lgmGetStatus
*AUTHOR:        John Morrison
*CREATION DATE: 14/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Gets the man status for drawing on the status bars
*
*ARGUMENTS:
*  tnk    - Pointer to this LGM's tank.
*  isOut  - TRUE if man is out of tank
*  isDead - TRUE if man is dead
*  angle  - Angle man is travelling on
*********************************************************/
void lgmGetStatus(lgm *lgman, tank *tnk, bool *isOut, bool *isDead, TURNTYPE *angle);

/*********************************************************
*NAME:          lgmGetWX
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns lgm World X co-ord
*
*ARGUMENTS:
*
*********************************************************/
WORLD lgmGetWX(lgm *lgman);

/*********************************************************
*NAME:          lgmGetWY
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns lgm World Y co-ord
*
*ARGUMENTS:
*
*********************************************************/
WORLD lgmGetWY(lgm *lgman);

/*********************************************************
*NAME:          lgmGetDir
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns lgm direction
*
*ARGUMENTS:
*  tnk - Pointer to the tank object
*********************************************************/
BYTE lgmGetDir(lgm *lgman, tank *tnk);

/*********************************************************
*NAME:          lgmGetBrainState
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns lgm's state required for brain.
*
*ARGUMENTS:
*
*********************************************************/
BYTE lgmGetBrainState(lgm *lgman);

/*********************************************************
*NAME:          lgmGetBrainObstructed
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns lgm's obstructed state required for brain
*
*ARGUMENTS:
*
*********************************************************/
BYTE lgmGetBrainObstructed(lgm *lgman);

/*********************************************************
*NAME:          lgmSetBrainObstructed
*AUTHOR:        John Morrison
*CREATION DATE: 16/4/01
*LAST MODIFIED: 16/4/01
*PURPOSE:
*  Stores lgm's obstructed state required for brain
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  obstructed - The obstructed state to set
*********************************************************/
void lgmSetBrainObstructed(lgm *lgman, BYTE obstructed);

/*********************************************************
*NAME:          lgmPutWorld
*AUTHOR:        John Morrison
*CREATION DATE: 23/9/00
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm map X position
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  x      - World X Co-ordinate
*  y      - World Y Co-ordinate
*********************************************************/
void lgmPutWorld(lgm *lgmman, WORLD x, WORLD y, BYTE frame);

/*********************************************************
*NAME:          lgmNetBackInTank
*AUTHOR:        John Morrison
*CREATION DATE: 2/12/00
*LAST MODIFIED: 2/12/00
*PURPOSE:
*  Network message. LGM Back in tank
*
*ARGUMENTS:
*  lgman    - Pointer to the lgm sturcture
*  mp       - Pointer to the map structure
*  pb       - Pointer to the pillbox structure
*  bs       - Pointer to the base structure
*  tnk      - Pointer to the tank structure
*  numTrees - Amount of trees lgm is carrying
*  numMines - Amount of mines the lgm is carrying
*  pillNum  - Pillbox being carries
*********************************************************/
void lgmNetBackInTank(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE numTrees, BYTE numMines, BYTE pillNum);

/*********************************************************
*NAME:          lgmNetManWorking
*AUTHOR:        John Morrison
*CREATION DATE: 01/02/03
*LAST MODIFIED: 01/02/03
*PURPOSE:
*  Network message. LGM out doing work 
*
*ARGUMENTS:
*  lgman    - Pointer to the lgm sturcture
*  tnk      - Pointer to the tank structure
*  mapX     - Bless X map position 
*  mapY     - Bless Y map position 
*  numTrees - Amount of trees lgm is carrying
*  numMines - Amount of mines the lgm is carrying
*  pillNum  - Pillbox being carries
*********************************************************/
void lgmNetManWorking(lgm *lgman, tank *tnk, BYTE mapX, BYTE mapY, BYTE numTrees, BYTE numMines, BYTE pillNum);

/*********************************************************
*NAME:          lgmSetPlayerNum
*AUTHOR:        John Morrison
*CREATION DATE: 3/12/00
*LAST MODIFIED: 3/12/00
*PURPOSE:
*  Sets the player number
*
*ARGUMENTS:
*  lgman     - Pointer to the lgm sturcture
*  playerNum - Player number to set to
*********************************************************/
void lgmSetPlayerNum(lgm *lgman, BYTE playerNum);

/*********************************************************
*NAME:          lgmSetIsDead
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/00
*LAST MODIFIED: 28/12/00
*PURPOSE:
*  Sets if the lgm is dead of not
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  isDead - Flag whether the LGM is dead or not
*********************************************************/
void lgmSetIsDead(lgm *lgman, bool isDead);

/*********************************************************
*NAME:          lgmConnectionLost
*AUTHOR:        John Morrison
*CREATION DATE: 24/02/03
*LAST MODIFIED: 24/02/03
*PURPOSE:
* Called if our connection is lost from the server 
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  tnk    - Pointer to our tank structure
*********************************************************/
void lgmConnectionLost(lgm *lgman, tank *tnk);

#endif /* LGM_H */
