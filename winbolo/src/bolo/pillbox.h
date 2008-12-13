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
*Filename:      pillbox.h
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 27/07/04
*Purpose:
*  Provides operations on pillbox and pillboxes
*********************************************************/

#ifndef PILLBOX_H
#define PILLBOX_H


/* Includes */
#include "global.h"
#include "types.h"
#include "tank.h"
#include "shells.h"

#define PILLBOX_ATTACK_NORMAL 100
#define PILLBOX_COOLDOWN_TIME 32
#define PILLBOX_MAX_FIRERATE 5 /* 6 */

#define PILLS_MAX_ARMOUR 15

/* A pillbox range is 8 map squares or 2048 world units */
#define PILLBOX_RANGE 2048

/* A pillbox fires 9 map squares */
#define PILLBOX_FIRE_DISTANCE 9

/* Pillbox not found return Value */
#define PILL_NOT_FOUND 254

/* Maximum amount of health a pillbox can have */
#define PILL_MAX_HEALTH 15

/* A pillbox has to be within 9 squares of a base to get angry if it is shot */
#define PILL_BASE_HIT_LEFT -9
#define PILL_BASE_HIT_RIGHT 9
#define PILL_BASE_HIT_TOP -9
#define PILL_BASE_HIT_BOTTOM 9

/* The maximum amount of time we will iterate to aim a shell to hit a tank */
/* Added to fix the rare tank run away bug so the shell will never hit the tank */
#define MAX_AIM_ITERATE 200

/* Brain stuff */
/* Bases Brain stuff */
#define PILLS_BRAIN_FRIENDLY 0
#define PILLS_BRAIN_NEUTRAL 2
#define PILLS_BRAIN_HOSTILE 1
#define PILLS_BRAIN_OBJECT_TYPE 2


/* Typedefs */

/* Determines the pill type, good, neutral or evil */
typedef enum {
  pillDead,
  pillAllie,
  pillGood,
  pillNeutral,
  pillEvil,
  pillTankGood,
  pillTankAllie,
  pillTankEvil
} pillAlliance;


/* Prototypes */

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
void pillsCreate(pillboxes *value);

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
void pillsDestroy(pillboxes *value);

/*********************************************************
*NAME:          pillsSetNumPills
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets the number of pills in the structure 
*
*ARGUMENTS:
*  value    - Pointer to the pillbox structure
*  numPills - The number of pills
*********************************************************/
void pillsSetNumPills(pillboxes *value, BYTE numPills);

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
BYTE pillsGetNumPills(pillboxes *value);

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
void pillsSetPill(pillboxes *value, pillbox *item, BYTE pillNum);

/*********************************************************
*NAME:          pillsGetPill
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 9/2/99
*PURPOSE:
*  Gets a specific pill.
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  item    - Pointer to a pillbox
*  pillNum - The pillbox number
*********************************************************/
void pillsGetPill(pillboxes *value, pillbox *item, BYTE pillNum);

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
bool pillsExistPos(pillboxes *value, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          pillsGetAllianceNum
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Returns the alliance type of a pillbox for drawing
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  pillNum - Pillbox Number to get 
*********************************************************/
pillAlliance pillsGetAllianceNum(pillboxes *value, BYTE pillNum);

/*********************************************************
*NAME:          pillsUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 17/1/99
*PURPOSE:
*  Updates each pillboxs reload time and fires if the
*  tank is in range
*
*ARGUMENTS:
*  value - Pointer to the pillbox structure
*  mp    - Pointer to the map structure
*  bs    - Pointer to the bases structure
*  tnk   - Tank Structure
*  shs   - Shells Structure
*********************************************************/
void pillsUpdate(pillboxes *value, map *mp, bases *bs, tank *tnk, shells *shs);

/*********************************************************
*NAME:          pillsIsPillHit
*AUTHOR:        John Morrison
*CREATION DATE: 30/10/98
*LAST MODIFIED: 19/3/98
*PURPOSE:
*  Returns whether a pillbox is hit at a specific location
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
bool pillsIsPillHit(pillboxes *value, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          pillsDamagePos
*AUTHOR:        John Morrison
*CREATION DATE: 18/3/98
*LAST MODIFIED: 24/4/01
*PURPOSE:
*  Does damage to a pillbox at xValue, yValue.
*  Returns if the pill is dead
*
*ARGUMENTS:
*  value      - Pointer to the pillbox structure
*  xValue     - X Location
*  yValue     - Y Location
*  wantDamage - TRUE if we just want to do damage to it
*  wantAngry  - TRUE if we just want to make it angry
*********************************************************/
bool pillsDamagePos(pillboxes *value, BYTE xValue, BYTE yValue, bool wantDamage, bool wantAngry);

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
BYTE pillsGetScreenHealth(pillboxes *value, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          pillsTargetTank
*AUTHOR:        John Morrison
*CREATION DATE:  1/12/98
*LAST MODIFIED:  1/1/99
*PURPOSE:
*  Returns the angle need to fire to hit a tank
*
*ARGUMENTS:
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pills structure
*  bs     - Pointer to the bases structure
*  xValue - X Location of pillbox
*  yValue - Y Location of pillbox
*  tankX  - X Location of tank
*  tankY  - Y Location of tank
*  angle  - Angle of the tank
*  speed  - The speed of the tank
*  onBoat - Is the tank on a boat
*********************************************************/
TURNTYPE pillsTargetTank(map *mp, pillboxes *pb, bases *bs, WORLD xValue, WORLD yValue, WORLD tankX, WORLD tankY, TURNTYPE angle, BYTE speed, bool onBoat);

/*********************************************************
*NAME:          pillsTargetTankMove
*AUTHOR:        John Morrison
*CREATION DATE: 31/12/98
*LAST MODIFIED: 17/1/99
*PURPOSE:
*  Returns the angle need to fire to hit a moving tank
*
*ARGUMENTS:
*  mp     - Pointer to the map structyre
*  pb     - Pointer to the pillbox structure
*  xValue - X Location of pillbox
*  yValue - Y Location of pillbox
*  tankX  - X Location of tank
*  tankY  - Y Location of tank
*  angle  - Angle of the tank
*  speed  - The speed of the tank
*  onBoat - Is the tank on a boat
*********************************************************/
TURNTYPE pillsTargetTankMove(map *mp, pillboxes *pb, bases *bs, WORLD xValue, WORLD yValue, WORLD tankX, WORLD tankY, TURNTYPE angle, BYTE speed, bool onBoat);

/*********************************************************
*NAME:          pillsDeadPos
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Returns whether a pillbox a specific location is dead
*  or not.
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  xValue - X Location of pillbox
*  yValue - Y Location of pillbox
*********************************************************/
bool pillsDeadPos(pillboxes *value, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          pillsGetPillNum
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 14/9/00
*PURPOSE:
*  Returns the pill number of a pillbox at that location
*  If not found returns PILL_NOT_FOUND
*
*ARGUMENTS:
*  value      - Pointer to the pillbox structure
*  xValue     - X Location of pillbox
*  yValue     - Y Location of pillbox
*  careInTank - Whether we are about the in tank state
*  inTank     - The intank state to check if we care
*********************************************************/
BYTE pillsGetPillNum(pillboxes *value, BYTE xValue, BYTE yValue, bool careInTank, bool inTank);

/*********************************************************
*NAME:          pillsSetPillInTank
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Returns the pill number of a pillbox at that location
*  If not found returns PILL_NOT_FOUND
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  pillNum - The pillbox number to apply it to
*  inTank  - Is it in tank or not
*********************************************************/
void pillsSetPillInTank(pillboxes *value, BYTE pillNum, bool inTank);

/*********************************************************
*NAME:          pillsGetPillOwner
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Returns the owner of the pill.
*  If not found returns PILL_NOT_FOUND
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  pillNum - The pillbox number to apply it to
*********************************************************/
BYTE pillsGetPillOwner(pillboxes *value, BYTE pillNum);

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
BYTE pillsSetPillOwner(pillboxes *value, BYTE pillNum, BYTE owner, bool migrate);

/*********************************************************
*NAME:          pillsGetDamagePos
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Does the amount of damage to the pillbox at the give
*  location. Usually caused by explosions
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  xValue - X Location of pillbox
*  yValue - Y Location of pillbox
*  amount - Amount of damage done to the pillbox
*********************************************************/
void pillsGetDamagePos(pillboxes *value, BYTE xValue, BYTE yValue, BYTE amount);

/*********************************************************
*NAME:          pillsNumInRect
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 17/1/99
*PURPOSE:
*  Returns the number of hostile pillboxs inside the
*  given rectangle
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  leftPos   - Left of the rectangle
*  rightPos  - Right of the rectangle
*  top    - Top of the rectangle
*  bottom - Bottom of the rectangle
*********************************************************/
BYTE pillsNumInRect(pillboxes *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

/*********************************************************
*NAME:          pillsRepairPos
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  Repair a pillbox at specific location
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  xValue - X Map position
*  yValue - Y Map position
*********************************************************/
void pillsRepairPos(pillboxes *value, BYTE mx, BYTE my);

/*********************************************************
*NAME:          pillsGetArmourPos
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  Returns the amount of armour a pillbox has at a specific
*  location. Returns PILL_NOT_FOUND if no pills exist at
*  that location
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  xValue - X Map position
*  yValue - Y Map position
*********************************************************/
BYTE pillsGetArmourPos(pillboxes *value, BYTE mx, BYTE my);

/*********************************************************
*NAME:          pillsGetNextView
*AUTHOR:        John Morrison
*CREATION DATE: 3/2/99
*LAST MODIFIED: 3/2/99
*PURPOSE:
* Returns the next allied (and alive) pill exists. If so
* then it puts its map co-ordinates into the parameters
* passed. If a previous pill is being used the the 
* parameter 'prev' is true and mx & my are set to the last
* pills location
*
*ARGUMENTS:
*  value - Pointer to the pillbox structure
*  mx    - Pointer to hold X Map position (and prev)
*  my    - Pointer to hold Y Map position (and prev)
*  prev  - Whether a previos pill is being passed
*********************************************************/
bool pillsGetNextView(pillboxes *value, BYTE *mx, BYTE *my, bool prev);

/*********************************************************
*NAME:          pillsCheckView
*AUTHOR:        John Morrison
*CREATION DATE: 3/2/99
*LAST MODIFIED: 3/2/99
*PURPOSE:
* We are currently in pillview with the pill at position
* mx and my. This function checks that it is still alive
* so we can continue viewing through it.
*
*ARGUMENTS:
*  value - Pointer to the pillbox structure
*  mx    - X Map position
*  my    - Y Map position
*********************************************************/
bool pillsCheckView(pillboxes *value, BYTE mx, BYTE my);

/*********************************************************
*NAME:          pillsBaseHit
*AUTHOR:        John Morrison
*CREATION DATE: 16/2/99
*LAST MODIFIED: 16/2/99
*PURPOSE:
* A base has been hit. Check to see if any pillboxes are 
* in range and are allied to the base to see if they should
* get angry
*
*ARGUMENTS:
*  value - Pointer to the pillbox structure
*  mx    - X location of the base that was hit
*  my    - Y location of the base that was hit
*  baseOwner - Owner of the base that was hit
*********************************************************/
void pillsBaseHit(pillboxes *value, BYTE mx, BYTE my, BYTE baseOwner);

/*********************************************************
*NAME:          pillsGetNumNeutral
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Returns the number of neutral pills
*
*ARGUMENTS:
*  value - Pointer to the pills structure
*********************************************************/
BYTE pillsGetNumNeutral(pillboxes *value);

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
void pillsSetPillNetData(pillboxes *value, BYTE *buff, BYTE dataLen);

/*********************************************************
*NAME:          pillsGetPillNetData
*AUTHOR:        John Morrison
*CREATION DATE: 27/02/99
*LAST MODIFIED: 24/07/04
*PURPOSE:
* Gets a copy of the pills data and copies it to buff.
*
*ARGUMENTS:
*  value - Pointer to the pills structure
*  buff  - Buffer to hold copy of pills data
*********************************************************/
BYTE pillsGetPillNetData(pillboxes *value, BYTE *buff);

/*********************************************************
*NAME:          pillsDropSetNeutralOwner
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
*  Changes all pills of owner owner back to neutral. Also
*  if they are inside a tank it drops them.
*
*ARGUMENTS:
*  value - Pointer to the pillbox structure
*  owner - Owner to set back to neutral
*********************************************************/
void pillsDropSetNeutralOwner(pillboxes *value, BYTE owner);

/*********************************************************
*NAME:          basesMigrate
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
*  Causes all bases owned by owner  to migrate to a new 
* owner because its old owner left the game.
*
*ARGUMENTS:
*  value    - Pointer to the bases structure
*  oldOwner - Old Owner to remove
*  newOwner - Owner to replace with
*********************************************************/
void pillsMigrate(pillboxes *value, BYTE oldOwner, BYTE newOwner);

/*********************************************************
*NAME:          pillsExplicitDrop
*AUTHOR:        John Morrison
*CREATION DATE: 10/11/99
*LAST MODIFIED: 10/11/99
*PURPOSE:
*  Drops all pills of owner owner
*
*
*ARGUMENTS:
*  value - Pointer to the pillbox structure
*  owner - Owner to set back to neutral
*********************************************************/
void pillsExplicitDrop(pillboxes *value, BYTE owner);

/*********************************************************
*NAME:          pillsGetBrainPillsInRect
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 28/11/99
*PURPOSE:
*  Makes the brain pill info for each pill inside the
*  rectangle formed by the function parameters
*
*ARGUMENTS:
*  value  - Pointer to the pills structure
*  leftPos   - Left position of rectangle
*  rightPos  - Right position of rectangle
*  top    - Top position of rectangle
*  bottom - Bottom position of rectangle
*********************************************************/
void pillsGetBrainPillsInRect(pillboxes *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom);

/*********************************************************
*NAME:          pillsSetBrainView
*AUTHOR:        John Morrison
*CREATION DATE: 13/12/99
*LAST MODIFIED: 13/12/99
*PURPOSE:
*  Returns if we are allowed to view from this pillbox.
*  By allowed the pill must not be in a tank, must not be
*  dead and must be on the same alliance as us.
*
*ARGUMENTS:
*  value     - Pointer to the pills structure
*  pillNum   - The pill number we are requesting the view
*  playerNum - Our player number
*********************************************************/
bool pillsSetView(pillboxes *value, BYTE pillNum, BYTE playerNum);

/*********************************************************
*NAME:          pillsGetMaxs
*AUTHOR:        John Morrison
*CREATION DATE: 13/6/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  Gets the maximum positions values of all the pillboxs.
*  eg left most, right most etc.
*
*ARGUMENTS:
*  value  - Pointer to the pills structure
*  leftPos   - Pointer to hold the left most value
*  rightPos  - Pointer to hold the right most value
*  top    - Pointer to hold the top most value
*  bottom - Pointer to hold the bottom most value
*********************************************************/
void pillsGetMaxs(pillboxes *value, int *leftPos, int *rightPos, int *top, int *bottom);

/*********************************************************
*NAME:          pillsMoveAll
*AUTHOR:        John Morrison
*CREATION DATE: 13/6/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  Repositions the pillboxes by moveX, moveY
*
*ARGUMENTS:
*  value  - Pointer to the pills structure
*  moveX  - The X move amount 
*  moveY  - The Y move amount 
*********************************************************/
void pillsMoveAll(pillboxes *value, int moveX, int moveY);

/*********************************************************
*NAME:          pillsGetOwnerBitMask
*AUTHOR:        John Morrison
*CREATION DATE: 22/6/00
*LAST MODIFIED: 22/6/00
*PURPOSE:
* Returns the owner bitmask for all the pills own by a 
* player.
*
*ARGUMENTS:
*  value - Pointer to the pills structure
*  owner - Owner to return pills owned for
*  moveY  - The Y move amount 
*********************************************************/
PlayerBitMap pillsGetOwnerBitMask(pillboxes *value, BYTE owner);

/*********************************************************
*NAME:          pillsGetAttackSpeed
*AUTHOR:        John Morrison
*CREATION DATE: 29/7/00
*LAST MODIFIED: 29/7/00
*PURPOSE:
* Returns the pillboxes aattacking speed. Returns 
* PILLBOX_ATTACK_NORMAL if out of range.
*
*ARGUMENTS:
*  value - Pointer to the pills structure
*  pillNum - Pillbox number to get
*********************************************************/
BYTE pillsGetAttackSpeed(pillboxes *value, BYTE pillNum);

/*********************************************************
*NAME:          pillsIsInView
*AUTHOR:        John Morrison
*CREATION DATE: 29/7/00
*LAST MODIFIED: 29/7/00
*PURPOSE:
* Returns if a location is in view (range +/- 10 squares
* of a pill allied to playerNum
*
*ARGUMENTS:
*  value     - Pointer to the pills structure
*  playerNum - Player number for pill alliance
*  mx        - X Map location
*  my        - Y Map location
*********************************************************/
bool pillsIsInView(pillboxes *value, BYTE playerNum, BYTE mx, BYTE my);

/*********************************************************
*NAME:          pillsMoveView
*AUTHOR:        John Morrison
*CREATION DATE: 21/1/01
*LAST MODIFIED: 21/1/01
*PURPOSE:
* Allows players to scroll through their pills. Returns
* whether a pill was found in that direction. 
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  mx     - Pointer to hold X Map position (and prev)
*  my     - Pointer to hold Y Map position (and prev)
*  xMove  - -1 for moving left, 1 for right, 0 for neither
*  yMove  - -1 for moving up, 1 for down, 0 for neither
*********************************************************/
bool pillsMoveView(pillboxes *value, BYTE *mx, BYTE *my, int xMove, int yMove);

/*********************************************************
*NAME:          pillsGetNumberOwnedByPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 19/11/03
*LAST MODIFIED: 19/11/03
*PURPOSE:
* Returns the number of pillboxes owned by this player
*
*ARGUMENTS:
*  value     - Pointer to the pills structure
*  playerNum - Player number for pills
*********************************************************/
BYTE pillsGetNumberOwnedByPlayer(pillboxes *value, BYTE playerNum);

void pillsSetPillCompressData(pillboxes *value, BYTE *buff, int dataLen);

#endif /* PILLBOX_H */
