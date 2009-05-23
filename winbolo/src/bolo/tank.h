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
*Name:          Tank
*Filename:      tank.h 
*Author:        John Morrison
*Creation Date: 23/11/98
*Last Modified: 01/02/03
*Purpose:
*  Provides operations on your tank
*********************************************************/

#ifndef _TANK_H
#define _TANK_H


/* Includes */
#include "global.h"
#include "types.h"
#include "bolo_map.h"
#include "bases.h"
#include "pillbox.h"
#include "starts.h"
#include "shells.h"

/* Defines */

/* Shift world co-ord 8 bits right to get map co-ord */
#define TANK_SHIFT_MAPSIZE 8
/* Shift world co-ord 12 bits right to get map co-ord */
#define TANK_SHIFT_PIXELSIZE 12

/* Shift things right 2 for readding to the world */
#define TANK_SHIFT_RIGHT2 4

/* There are 16 tank frames (or viewing angles the tank can take */
#define TANK_FRAMES 16

/* how many world coordinates to move per tank slide update */
#define TANK_SLIDE 16 
/* the number of times to update the tank slide, this is in world coordinates, the actual number is this +1 becuase it activates tank slide once right away */
#define TANK_SLIDE_TICKS 7

/* How many world coordinates to move per tank bump update */
#define TANK_BUMP 7
/* The number of times to update the tank slide, in ticks.
 * The actual number is this + 1 because it activates tank slide once right away */
#define TANK_BUMP_TICKS 7

#define TANK_DEATH_WAIT 255

/*
Wharf-Rat explains Acceleration

<wharf-rat> every other tick
<wharf-rat> the speed is either incremented or decremented by one
<wharf-rat> min of zero, max terrain dependent
<Min210> so in one second, its reduced or increased 25 times
<wharf-rat> yeah
<Min210> how did you find that out?
<wharf-rat> the problem is, i'm not sure what the speed units are
<wharf-rat> don't ask how, heh
<Min210> oh, those are world-coordinates
<Min210> speeds are given in world coord's per tick
<wharf-rat> max is 64?
* Min210 looks
<Min210> the doc's say the max is 16
<wharf-rat> oh, the units might be world * 4 then
<Min210> The speed of a bullet is 32 World units per tick. The top speed of a tank on a boat is 16 World units per tick.
<Min210> the max you found was 64?
<wharf-rat> yes
<Min210> ok
<Min210> I'll keep that in mind
<wharf-rat> so its really 0.25 worlds every other tick

we can get away with no using every other tick becuase we're not using a max of 64, so, this code below seems to work.
I made autoslowdown and keyslowdown the same, becuase they should be the same, the rest seems to behave similiarly to macbolo 
based on my testing.
*/
/* The acceleration and deceleration rates of the tank */
#define TANK_ACCELERATE_RATE 0.25 
#define TANK_TERRAIN_DECEL_RATE 0.25
/* The slow down button pressed rate */
#define TANK_SLOWKEY_RATE 0.25
/* Autoslowdown deceleration cosntant */
#define TANK_AUTOSLOW_SPEED ((float) (0.25))

/* The four points of the compass in terms of tank BRadians */
#define TANK_NORTH 0
#define TANK_EAST 64
#define TANK_SOUTH 128
#define TANK_WEST 192

/* This following value is the amount to subtract */
#define TANK_SUBTRACT 128
/* Was 128 */

/* The square to test to see if the tank is hit */
#define TANK_SUBTRACT_SHELL_HIT_X1 144
#define TANK_SUBTRACT_SHELL_HIT_X2 128
#define TANK_SUBTRACT_SHELL_HIT_Y 128

/* Minimum and maximuns lengths for the gunsight (in halve map squares) */
#define GUNSIGHT_MIN 2
#define GUNSIGHT_MAX 14

/* It takes 10 game ticks for the tank to reload */
#define TANK_RELOAD_TIME 13

/* Time it takes for the tank to lose some stuff in water */
#define TANK_WATER_TIME 15

/* Shells + mines must be greater then 20 for a big explosion */
#define TANK_BIG_EXPLOSION_THRESHOLD 20

/* Amount of damage a mine does to the tank */
#define MINE_DAMAGE 10

/* Number of map squares around the tank postion that a mine can hurt a tank */
#define MINE_DAMAGE_DISTANCE_LEFT 0 /* was -1 */
#define MINE_DAMAGE_DISTANCE_RIGHT 0 /* Was 1 */

/* Tank slows down a speed unit if it hits a wall */
#define TANK_WALL_SLOW_DOWN 1

/* Amount to move for checking */
#define TANK_MOVE_BOAT_SUB 64
#define TANK_MOVE_LAND_SUB 128 /* 96 */

/* Speed we exit the boat at */
#define BOAT_EXIT_SPEED 16

/* Minimum distance for seeing tank in trees = 3 map squares or 768 world co-ords */
#define MIN_TREEHIDE_DIST 768

/* Macros to make life easier */

#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define TankPillsTail(list) ((list)->next);

/* Typedefs */

typedef enum {
  TH_MISSED,     /* Tank has been missed */
  TH_HIT,        /* Tank has been hit but not killed */
  TH_KILL_SMALL, /* The tank has been hit, killsed and isn't carrying a lot of stuff so a small explosion */
  TH_KILL_BIG    /* The tank has been hit, killsed and is carrying a lot of stuff so a big explosion */
} tankHit;

typedef enum {
  TNONE,       /* No Buttons being pressed */
  TLEFT,       /* Left button is being pressed */
  TRIGHT,      /* Right button is being pressed */
  TACCEL,      /* Acellerate Button */
  TDECEL,      /* Decellerate Button is being pressed */
  TLEFTACCEL,  /* Left + Accelerate */
  TRIGHTACCEL, /* Right + Acellerate */
  TLEFTDECEL,  /* Left + Decellerate */
  TRIGHTDECEL  /* Right + Decellearate */
} tankButton;




/* Prototypes */

/*********************************************************
*NAME:          tankCreate
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/98
*LAST MODIFIED: 2/2/99
*PURPOSE:
*  Creates a new tank and sets its armour/mines etc. level 
*  to the arguments. New tanks always start with full armour
*
*ARGUMENTS:
*  value  - Pointer to the tank structure 
*  sts    - Pointer to player starts structure
*********************************************************/
void tankCreate(tank *value, starts *sts);

/*********************************************************
*NAME:          tankDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/98
*LAST MODIFIED: 20/6/00
*PURPOSE:
*  Destroys and frees the memory for the tank
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*  mp    - Pointer to the map structure
*  pb    - Pointer to the pillbox structure
*  bs    - Pointer to the bases structure
*********************************************************/
void tankDestroy(tank *value, map *mp, pillboxes *pb, bases *bs);

/*********************************************************
*NAME:          tankGetAngle
*AUTHOR:        John Morrison
*CREATION DATE: 30/8/98
*LAST MODIFIED: 30/8/98
*PURPOSE:
*  Returns the tanks actual angle
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
TURNTYPE tankGetAngle(tank *value);

/*********************************************************
*NAME:          tankGetDir
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/98
*LAST MODIFIED: 23/11/98
*PURPOSE:
*  Returns the tank direction (16 frames)
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetDir(tank *value);

/*********************************************************
*NAME:          tankGet256Dir
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/98
*LAST MODIFIED:  29/4/00
*PURPOSE:
*  Returns the tank direction 0-255
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGet256Dir(tank *value);

/*********************************************************
*NAME:          tankUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/98
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  The timer has passed. Update the location and reload
*  if required.
*
*ARGUMENTS:
*  value      - Pointer to the tank structure
*  mp         - Pointer to the map structure
*  bs         - Pointer to the bases structure 
*  pb         - Pointer to the pillboxes structure 
*  shs        - Pointer to the shells structure 
*  sts        - Pointer to the starts structure
*  tb         - Whether the left/right/forward etc keys 
*               is being held down
*  tankShoot  - Is the fire button down  
*  inBrain    - TRUE if a brain is running 
*               (Ignore autoslowdown)
*********************************************************/
void tankUpdate(tank *value, map *mp, bases *bs, pillboxes *pb, shells *shs, starts *sts, tankButton tb, bool tankShoot, bool inBrain);

/*********************************************************
*NAME:          tankIsMoving
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
*  Returns whether the tank is in motion or not 
*
*ARGUMENTS:
*  value      - Pointer to the tank structure
*********************************************************/
bool tankIsMoving(tank *value);

/*********************************************************
*NAME:          tankGetDir
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/98
*LAST MODIFIED: 23/11/98
*PURPOSE:
*  Returns the tanks armour
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetArmour(tank *value);

/*********************************************************
*NAME:          tankGetScreenMX
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns the tanks X Co-ord on the map for screen
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetScreenMX(tank *value);

/*********************************************************
*NAME:          tankGetScreenPX
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns the tanks x pixel offset for screen
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetScreenPX(tank *value);

/*********************************************************
*NAME:          tankGetScreenMY
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns the tanks y Co-ord on the map for screen
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetScreenMY(tank *value);

/*********************************************************
*NAME:          tankGetScreenPY
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns the tanks y pixel offset For screen
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetScreenPY(tank *value);

/*********************************************************
*NAME:          tankGetMX
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns whether the tanks X Co-ord on the map
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetMX(tank *value);

/*********************************************************
*NAME:          tankGetPX
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns whether the tanks x pixel offset
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetPX(tank *value);

/*********************************************************
*NAME:          tankGetMY
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns whether the tanks y Co-ord on the map
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetMY(tank *value);

/*********************************************************
*NAME:          tankGetPY
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns whether the tanks y pixel offset
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetPY(tank *value);

/*********************************************************
*NAME:          tankGetStats
*AUTHOR:        John Morrison
*CREATION DATE: 22/12/98
*LAST MODIFIED: 22/12/98
*PURPOSE:
*  Returns the tank shells, mines, armour and trees 
*
*ARGUMENTS:
*  value        - Pointer to the tank structure
*  shellsAmount - Pointer to hold number of shells
*  minesAmount  - Pointer to hold number of mines
*  armourAmount - Pointer to hold amount of armour
*  treesAmount  - Pointer to hold amount of trees
*********************************************************/
void tankGetStats(tank *value, BYTE *shellsAmount, BYTE *minesAmount, BYTE *armourAmount, BYTE *treesAmount);

/*********************************************************
*NAME:          tankGetStats
*AUTHOR:        John Morrison
*CREATION DATE: 22/12/98
*LAST MODIFIED: 22/12/98
*PURPOSE:
*  Returns the tank shells, mines, armour and trees 
*
*ARGUMENTS:
*  value        - Pointer to the tank structure
*  shellsAmount - Number of shells
*  minesAmount  - Number of mines
*  armourAmount - Amount of armour
*  treesAmount  - Amount of trees
*********************************************************/
void tankSetStats(tank *value, BYTE shellsAmount, BYTE minesAmount, BYTE armourAmount, BYTE treesAmount);

/*********************************************************
*NAME:          tankIsGunsightShow
*AUTHOR:        John Morrison
*CREATION DATE: 24/12/98
*LAST MODIFIED: 24/12/98
*PURPOSE:
*  Returns whether the gunsight is visible or not
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*********************************************************/
bool tankIsGunsightShow(tank *value);

/*********************************************************
*NAME:          tankGetGunsight
*AUTHOR:        John Morrison
*CREATION DATE: 24/12/98
*LAST MODIFIED: 24/12/98
*PURPOSE:
*  Returns the Map and Pixel co-ordinates for the tanks
*  gunsight.
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  xMap   - Pointer to hold Map X Co-ord
*  yMap   - Pointer to hold Map X Co-ord
*  xPixel - Pointer to hold X Pixel
*  yPixel - Pointer to hold Y Pixel
*********************************************************/
void tankGetGunsight(tank *value, BYTE *xMap, BYTE *yMap, BYTE *xPixel, BYTE *yPixel);

/*********************************************************
*NAME:          tankGunsightIncrease
*AUTHOR:        John Morrison
*CREATION DATE: 24/12/98
*LAST MODIFIED: 24/12/98
*PURPOSE:
*  Adds a map unit on to the tank gunsight range 
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*********************************************************/
void tankGunsightIncrease(tank *value);

/*********************************************************
*NAME:          tankGunsightDecrease
*AUTHOR:        John Morrison
*CREATION DATE: 24/12/98
*LAST MODIFIED: 24/12/98
*PURPOSE:
*  Adds a map unit on to the tank gunsight range 
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*********************************************************/
void tankGunsightDecrease(tank *value);

/*********************************************************
*NAME:          tankSetGunsight
*AUTHOR:        John Morrison
*CREATION DATE: 24/12/98
*LAST MODIFIED: 24/12/98
*PURPOSE:
*  Sets the gunsight on or off
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  shown  - if TRUE then gunsight shown
*********************************************************/
void tankSetGunsight(tank *value, bool shown);

/*********************************************************
*NAME:          tankGetWorld
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Sets the gunsight on or off
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  x      - Pointer to hold X co-ord
*  y      - Pointer to hold Y co-ord
*********************************************************/
void tankGetWorld(tank *value, WORLD *x, WORLD *y);

/*********************************************************
*NAME:          tankSetWorld
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED:  7/1/00
*PURPOSE:
*  Sets the tanks world co-ordinates and angle
*
*ARGUMENTS:
*  value        - Pointer to the tank structure
*  x            - X co-ord
*  y            - Y co-ord
*  angle        - Angle to set to
*  setResources - TRUE if we should call gameTypeGetItems
*                 to fuel up the tank
*********************************************************/
void tankSetWorld(tank *value, WORLD x, WORLD y, TURNTYPE angle, bool setResources);

/*********************************************************
*NAME:          tankIsTankHit
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 29/7/00
*PURPOSE:
*  Returns whether the tank has been hit or not, if it
*  it is killed etc. 
*  Also updates its location if hit but not dead.
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the bases structure
*  x      - X co-ord of shell
*  y      - Y co-ord of shell
*  angle  - The direction the shell came from
*  owner  - Shells owner
*********************************************************/
tankHit tankIsTankHit(tank *value, map *mp, pillboxes *pb, bases *bs, WORLD x, WORLD y, TURNTYPE angle, BYTE owner);

/*********************************************************
*NAME:          tankNetTankHit
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  A player across the network has hit us. Update things as
*  required,
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pills structure
*  bs     - Pointer to the bases structure
*  angle  - The direction the shell came from
*  owner  - Who fired the shell
*********************************************************/
void tankNetTankHit(tank *value, map *mp, pillboxes *pb, bases *bs, TURNTYPE angle, BYTE owner);

/*********************************************************
*NAME:          tankGetSpeed
*AUTHOR:        John Morrison
*CREATION DATE: 31/12/98
*LAST MODIFIED: 31/12/98
*PURPOSE:
*  Returns the tank speed
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetSpeed(tank *value);

/*********************************************************
*NAME:          tankGetActualSpeed
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
*  Returns the actual tank speed
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
SPEEDTYPE tankGetActualSpeed(tank *value);

/*********************************************************
*NAME:          tankGetTravelAngel
*AUTHOR:        John Morrison
*CREATION DATE: 31/12/98
*LAST MODIFIED: 31/12/98
*PURPOSE:
*  Returns the tank direction (16 frames)
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetTravelAngel(tank *value);

/*********************************************************
*NAME:          tankInWater
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/99
*LAST MODIFIED: 4/1/99
*PURPOSE:
*  The tank is in water. Reduce ammo and mines count.
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*********************************************************/
void tankInWater(tank *value);

/*********************************************************
*NAME:          tankGetFrame
*AUTHOR:        John Morrison
*CREATION DATE: 6/1/99
*LAST MODIFIED: 6/1/99
*PURPOSE:
*  Returns the tank frame to draw (16 frames)
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetFrame(tank *value);

/*********************************************************
*NAME:          tankGetFrame
*AUTHOR:        John Morrison
*CREATION DATE: 7/1/99
*LAST MODIFIED: 7/1/99
*PURPOSE:
*  The tank has died. Reinit its location and increment
*  the death count
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*  sts   - Pointer to the starts structure
*********************************************************/
void tankDeath(tank *value, starts *sts);

/*********************************************************
*NAME:          tankGetKillsDeaths
*AUTHOR:        John Morrison
*CREATION DATE: 6/1/99
*LAST MODIFIED: 6/1/99
*PURPOSE:
*  Returns the number of kills and deaths the tank has had
*  into the passed pointers
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  kills  - Pointer to hold the number of kills
*  deaths - Pointer to hold the number of deaths
*********************************************************/
void tankGetKillsDeaths(tank *value, int *kills, int *deaths);

/*********************************************************
*NAME:          tankAddArmour
*AUTHOR:        John Morrison
*CREATION DATE: 11/1/99
*LAST MODIFIED: 11/1/99
*PURPOSE:
*  Adds the amount of armour specified to the tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  amount - Amount to add 
*********************************************************/
void tankAddArmour(tank *value, BYTE amount);

/*********************************************************
*NAME:          tankAddShells
*AUTHOR:        John Morrison
*CREATION DATE: 11/1/99
*LAST MODIFIED: 11/1/99
*PURPOSE:
*  Adds the amount of shells specified to the tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  amount - Amount to add 
*********************************************************/
void tankAddShells(tank *value, BYTE amount);

/*********************************************************
*NAME:          tankAddMines
*AUTHOR:        John Morrison
*CREATION DATE: 11/1/99
*LAST MODIFIED: 11/1/99
*PURPOSE:
*  Adds the amount of mines specified to the tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  amount - Amount to add 
*********************************************************/
void tankAddMines(tank *value, BYTE amount);

/*********************************************************
*NAME:          tankMoveOnBoat
*AUTHOR:        John Morrison
*CREATION DATE: 13/1/99
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  The tank is moving on a boat
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*  mp      - Pointer to the map structure
*  pb      - Pointer to the pillbox structure
*  bs      - Pointer to the bases structure
*  bmx     - X Map Position
*  bmy     - Y Map position
*  tb      - The tank buttons being pressed
*  inBrain - TRUE if a brain is running (ignore autoslow)
*********************************************************/
void tankMoveOnBoat(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE bmx, BYTE bmy, tankButton tb, bool inBrain);

/*********************************************************
*NAME:          tankMoveOnLand
*AUTHOR:        John Morrison
*CREATION DATE: 13/1/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
*  The tank is moving on land
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*  mp      - Pointer to the map structure
*  pb      - Pointer to the pillbox structure
*  bs      - Pointer to the bases structure
*  bmx     - X Map Position
*  bmy     - Y Map position
*  tb      - The tank buttons being pressed
*  inBrain - TRUE if a brain is running (ignore autoslow)
*********************************************************/
void tankMoveOnLand(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE bmx, BYTE bmy, tankButton tb, bool inBrain);

/*********************************************************
*NAME:          tankTurn
*AUTHOR:        John Morrison
*CREATION DATE: 13/1/99
*LAST MODIFIED: 13/1/99
*PURPOSE:
*  Function called to update the tanks facing angle
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the bases structure
*  bmx    - X Map Position
*  bmy    - Y Map position
*  tb     - The tank buttons being pressed
*********************************************************/
void tankTurn(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE bmx, BYTE bmy, tankButton tb);

void tankMovement(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE bmx, BYTE bmy, tankButton tb);

/*********************************************************
*NAME:          tankAccel
*AUTHOR:        John Morrison
*CREATION DATE: 13/1/99
*LAST MODIFIED: 13/1/99
*PURPOSE:
* Apply accleration or decalleration as required
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the bases structure
*  bmx    - X Map Position
*  bmy    - Y Map position
*  tb     - The tank buttons being pressed
*********************************************************/
void tankAccel(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE bmx, BYTE bmy, tankButton tb);

/*********************************************************
*NAME:          tankCheckPillCapture
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
* Function checks for pillbox captures. If it does capture
* one then it updates everything.
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  pb     - Pointer to the pillbox structure
*********************************************************/
void tankCheckPillCapture(tank *value, pillboxes *pb);

/*********************************************************
*NAME:          tankDropPills
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 24/6/00
*PURPOSE:
* Drops all the pillboxes at the tanks current location.
* Called when tank dies
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the bases structure
*********************************************************/
void tankDropPills(tank *value, map *mp, pillboxes *pb, bases *bs);

/*********************************************************
*NAME:          tankIsOnBoat
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 17/1/99
*PURPOSE:
* Returns wether the tank is on a boat or not 
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*********************************************************/
bool tankIsOnBoat(tank *value);

/*********************************************************
*NAME:          tankGetLgmTrees
*AUTHOR:        John Morrison
*CREATION DATE: 17/01/99
*LAST MODIFIED: 01/02/03
*PURPOSE:
* Returns whether the amount of trees request is availble
* if it is then it subtracts the amount then updates the
* display
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*  amount  - Amount of trees requested
*  perform - If perform is FALSE then only test if this
*            will work. Don't actually deduct
*********************************************************/
bool tankGetLgmTrees(tank *value, BYTE amount, bool perform);

/*********************************************************
*NAME:          tankGiveTrees
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 17/1/99
*PURPOSE:
* Adds the amount given in the amount to the tanks stocks
* and updates the display
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  amount - Amount of trees to add
*********************************************************/
void tankGiveTrees(tank *value, BYTE amount);

/*********************************************************
*NAME:          tankGetLgmMines
*AUTHOR:        John Morrison
*CREATION DATE: 17/01/99
*LAST MODIFIED: 01/02/03
*PURPOSE:
* Returns whether the amount of mines request is availble
* if it is then it subtracts the amount then updates the
* display
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*  amount  - Amount of mines requested
*  perform - If perform is FALSE then only test if this
*            will work. Don't actually deduct
*********************************************************/
bool tankGetLgmMines(tank *value, BYTE amount, bool perform);

/*********************************************************
*NAME:          tankGiveMines
*AUTHOR:        John Morrison
*CREATION DATE: 17/01/99
*LAST MODIFIED: 17/01/99
*PURPOSE:
* Adds the amount given in the amount to the tanks stocks
* and updates the displat
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*  amount  - Amount of mines to add
*********************************************************/
void tankGiveMines(tank *value, BYTE amount);

/*********************************************************
*NAME:          tankGetCarriedPill
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 01/02/03
*PURPOSE:
* Gets the first available carried tree. If none are 
* avaiable it returns FALSE
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*  pillNum - Pointer to hold the pillbox number
*  perform - If perform is FALSE then only test if this
*            will work. Don't actually deduct
*********************************************************/
bool tankGetCarriedPill(tank *value, BYTE *pillNum, bool perform);

/*********************************************************
*NAME:          tankPutCarriedPill
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 17/1/99
*PURPOSE:
* Puts a pillbox in the tank (Comes from LGM)
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*  pillNum - The pillbox number to add
*********************************************************/
void tankPutCarriedPill(tank *value, BYTE pillNum);

/*********************************************************
*NAME:          tankStopCarryingPill
*AUTHOR:        John Morrison
*CREATION DATE: 21/6/00
*LAST MODIFIED: 21/6/00
*PURPOSE:
* Someone else has picked up a pill. We should check that
* we aren't carrying it ourselves and if so drop it (The
* server said so) because this can lead to desync problems
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*  pillNum - Pillbox number to drop if we are holding it
*********************************************************/
void tankStopCarryingPill(tank *value, BYTE pillNum);

/*********************************************************
*NAME:          tankLayMine
*AUTHOR:        John Morrison
*CREATION DATE: 21/1/99
*LAST MODIFIED: 21/1/99
*PURPOSE:
* Tank has been requested to lay a mine
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*  mp    - Pointer to the map strructure
*  pb    - Pointer to the pillboxes structure
*  bs    - Pointer to the bases structure
*********************************************************/
void tankLayMine(tank *value, map *mp, pillboxes *pb, bases *bs);

/*********************************************************
*NAME:          tankMineDamage
*AUTHOR:        John Morrison
*CREATION DATE: 21/1/99
*LAST MODIFIED: 20/6/00
*PURPOSE:
* A mine has exploded. Check to see if it has hurt the 
* tank
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*  mp    - Pointer to the map structure
*  pb    - Pointer to the pillboxes structure
*  bs    - Pointer to the bases structure
*  mx    - Map X Co-ordinate
*  my    - Map Y Co-ordinate
*********************************************************/
void tankMineDamage(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE mx, BYTE my);

/*********************************************************
*NAME:          tankNearMines
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 29/1/99
*PURPOSE:
* Check to see if the tank is near any hidden mines 
* tank
*
*ARGUMENTS:
*  mp    - Pointer to the map
*  mx    - Map X Co-ordinate
*  my    - Map Y Co-ordinate
*********************************************************/
void tankNearMines(map *mp, BYTE mx, BYTE my);

/*********************************************************
*NAME:          tankCheckGroundClear
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 9/1/00
*PURPOSE:
* Checks that the ground beneath the tank is clear (ie no
* one has built under our tank) If it is not it moves the
* tank.
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*  mp    - Pointer to the map structure
*  pb    - Pointer to the pillboxes structure
*  bs    - Pointer to the bases structure
*********************************************************/
void tankCheckGroundClear(tank *value, map *mp, pillboxes *pb, bases *bs);

/*********************************************************
*NAME:          tankAddKill
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED: 20/3/99
*PURPOSE:
* We just killed a player. Add it here and update the 
* frontend repectively.
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
void tankAddKill(tank *value);

/*********************************************************
*NAME:          tankAddKill
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
* We just killed a player. Add it here and update the 
* frontend repectively.
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  wx     - World X co-ord
*  wy     - World Y co-ord
*  tt     - Angle
*  speed  - Speed of tank
*  onBoat - are we on a boat
*********************************************************/
void tankSetLocationData(tank *value, WORLD wx, WORLD wy, TURNTYPE tt, SPEEDTYPE speed, bool onBoat);

/*********************************************************
*NAME:          tankGetNumCarriedPills
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 25/11/99
*PURPOSE:
* Returns the number of carried pills
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*********************************************************/
BYTE tankGetNumCarriedPills(tank *value);

/*********************************************************
*NAME:          tankGetGunsightLength
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 25/11/99
*PURPOSE:
* Returns the gunsight length
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*********************************************************/
BYTE tankGetGunsightLength(tank *value);

/*********************************************************
*NAME:          tankGetGunsightLength
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 25/11/99
*PURPOSE:
* Returns the tank reloading time (0 for ready to shoot)
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*********************************************************/
BYTE tankGetReloadTime(tank *value);

/*********************************************************
*NAME:          tankIsObstructed
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 25/11/99
*PURPOSE:
* Returns whether the tank is obstructed or not
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*********************************************************/
bool tankIsObstructed(tank *value);

/*********************************************************
*NAME:          tankIsNewTank
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Returns if this is a new tank or not (ie just died)
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*********************************************************/
bool tankIsNewTank(tank *value);

/*********************************************************
*NAME:          tankGetAutoSlowdown
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/00
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Returns whether tank autoslowdown is enabled or not
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
bool tankGetAutoSlowdown(tank *value);

/*********************************************************
*NAME:          tankSetAutoSlowdown
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/00
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Sets whether tank autoslowdown is enabled or not
*
*ARGUMENTS:
*  value       - Pointer to the tank structure
*  useSlowdown - TRUE if auto slowdown is used
*********************************************************/
void tankSetAutoSlowdown(tank *value, bool useSlowdown);

/*********************************************************
*NAME:          tankGetAutoHideGunsight
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/00
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Returns whether tank auto show/hide gunsight is enabled 
*  or not
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
bool tankGetAutoHideGunsight(tank *value);

/*********************************************************
*NAME:          tankSetAutoHideGunsight
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/00
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Sets whether tank auto show/hide gunsight is enabled 
*  or not
*
*ARGUMENTS:
*  value       - Pointer to the tank structure
*  useAutohide - TRUE if auto slowdown is used
*********************************************************/
void tankSetAutoHideGunsight(tank *value, bool useAutohide);

/*********************************************************
*NAME:          tankJustFired
*AUTHOR:        John Morrison
*CREATION DATE: 24/5/00
*LAST MODIFIED: 24/5/00
*PURPOSE:
*  Returns if the tank just fired or not
*
*ARGUMENTS:
*  value       - Pointer to the tank structure
*********************************************************/
bool tankJustFired(tank *value);

/*********************************************************
*NAME:          tankGetShells
*AUTHOR:        John Morrison
*CREATION DATE: 21/8/00
*LAST MODIFIED: 21/8/00
*PURPOSE:
*  Returns the number of shells in tank
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetShells(tank *value);

/*********************************************************
*NAME:          tankGetMines
*AUTHOR:        John Morrison
*CREATION DATE: 21/8/00
*LAST MODIFIED: 21/8/00
*PURPOSE:
*  Returns the number of mines in tank
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetMines(tank *value);

/*********************************************************
*NAME:          tankGetTrees
*AUTHOR:        John Morrison
*CREATION DATE: 21/8/00
*LAST MODIFIED: 21/8/00
*PURPOSE:
*  Returns the number of trees in tank
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetTrees(tank *value);

/*********************************************************
*NAME:          tankSetShells
*AUTHOR:        John Morrison
*CREATION DATE: 21/8/00
*LAST MODIFIED: 21/8/00
*PURPOSE:
*  Sets the number of shells in tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  amount - The amount to set to
*********************************************************/
void tankSetShells(tank *value, BYTE amount);

/*********************************************************
*NAME:          tankSetArmour
*AUTHOR:        John Morrison
*CREATION DATE: 21/8/00
*LAST MODIFIED: 21/8/00
*PURPOSE:
*  Sets the amount of armour in tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  amount - The amount to set to
*********************************************************/
void tankSetArmour(tank *value, BYTE amount);

/*********************************************************
*NAME:          tankSetMines
*AUTHOR:        John Morrison
*CREATION DATE: 21/8/00
*LAST MODIFIED: 21/8/00
*PURPOSE:
*  Sets the number of mines in tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  amount - The amount to set to
*********************************************************/
void tankSetMines(tank *value, BYTE amount);

/*********************************************************
*NAME:          tankSetTrees
*AUTHOR:        John Morrison
*CREATION DATE: 21/8/00
*LAST MODIFIED: 21/8/00
*PURPOSE:
*  Sets the number of trees in tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  amount - The amount to set to
*********************************************************/
void tankSetTrees(tank *value, BYTE amount);

void tankGetCarriedPillNum(tank *value, BYTE pillNum);
void tankRegisterChangeFloat(tank *value, int offset, float newValue);
void tankRegisterChangeWorld(tank *value, int offset, WORLD newValue);
void tankRegisterChangeInt(tank *value, int offset, int newValue);
void tankRegisterChangeByte(tank *value, int offset, BYTE newValue);

/*********************************************************
*NAME:          tankSetOnBoat
*AUTHOR:        John Morrison
*CREATION DATE: 21/8/00
*LAST MODIFIED: 21/8/00
*PURPOSE:
*  Sets the onBoat value for the tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  onBoat - On Boat value to set
*********************************************************/
void tankSetOnBoat(tank *value, bool onBoat);


/*********************************************************
*NAME:          tankSetLastTankDeath
*AUTHOR:        Chris Lesnieski
*CREATION DATE: 04/2/09
*LAST MODIFIED: 04/2/09
*PURPOSE:
*  Sets the previous death type of the tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  deathType - deathType value to set
*********************************************************/
void tankSetLastTankDeath(tank *value, int deathType);


/*********************************************************
*NAME:          tankGetLastTankDeath
*AUTHOR:        Chris Lesnieski
*CREATION DATE: 04/2/09
*LAST MODIFIED: 04/2/09
*PURPOSE:
*  Gets the previous death type of the tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*********************************************************/
int tankGetLastTankDeath(tank *value);


/*********************************************************
*NAME:          tankGetDeathWait
*AUTHOR:        Chris Lesnieski
*CREATION DATE: 04/2/09
*LAST MODIFIED: 04/2/09
*PURPOSE:
*  Gets the previous number of ticks left until respawn
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*********************************************************/
int tankGetDeathWait(tank *value);

#endif

