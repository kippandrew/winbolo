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
*Filename:      tank.c
*Author:        John Morrison
*Creation Date: 23/11/98
*Last Modified: 01/02/01
*Purpose:
*  Provides operations on your tank
*********************************************************/

/* Inludes */
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "global.h"
#include "util.h"
#include "shells.h"
#include "tilenum.h"
#include "messages.h"
#include "frontend.h"
#include "sounddist.h"
#include "explosions.h"
#include "lgm.h"
#include "tankexp.h"
#include "minesexp.h"
#include "gametype.h"
#include "mines.h"
#include "labels.h"
#include "tankexp.h"
#include "players.h"
#include "netpnb.h"
#include "netmt.h"
#include "network.h"
#include "netplayers.h"
#include "tank.h"
#include "screen.h"
#include "backend.h"
#include "crc.h"
#include "log.h"

bool tankShuttingDown = FALSE; // Enourmouse HACK. Please Fix Me FIXME
BYTE ct[50];

/*********************************************************
*NAME:          tankCreate
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/98
*LAST MODIFIED: 15/12/99
*PURPOSE:
*  Creates a new tank and sets its armour/mines etc. level 
*  to the arguments. New tanks always start with full armour
*
*ARGUMENTS:
*  value  - Pointer to the tank structure 
*  sts    - Pointer to player starts structure
*********************************************************/
void tankCreate(tank *value, starts *sts) {  
  BYTE minesAmount; /* Stuff the new tank is to start with */
  BYTE shellsAmount;
  BYTE armourAmount;
  BYTE treesAmount;
  BYTE x;    /* Things to pass to get the player start position */
  BYTE y;
  TURNTYPE dir;

  tankShuttingDown = FALSE;
  
  New(*value);
  (*value)->x = 0;
  (*value)->y = 0;
  gameTypeGetItems(screenGetGameType(), &shellsAmount, &minesAmount, &armourAmount, &treesAmount);
  (*value)->armour = armourAmount;
  (*value)->shells = shellsAmount;
  (*value)->mines = minesAmount;
  (*value)->trees = treesAmount;
  (*value)->onBoat = TRUE;
  (*value)->showSight = FALSE;
  (*value)->sightLen = GUNSIGHT_MAX;
  (*value)->numKills = 0;
  (*value)->numDeaths = 0;
  (*value)->reload = 0;
  (*value)->speed = 0;
  (*value)->waterCount = 0;
  (*value)->deathWait = 0;
  (*value)->carryPills= NULL;
  (*value)->obstructed = FALSE;
  (*value)->newTank = TRUE;
  (*value)->autoSlowdown = FALSE;
  (*value)->autoHideGunsight = FALSE;
  (*value)->justFired = FALSE;
  (*value)->tankHitCount = 0;
  (*value)->tankSlideTimer = 0;
  (*value)->tankSlideAngle = 0;

  /* Get the start position */
  screenSetInStartFind(TRUE);
  startsGetStart(sts, &x, &y, &dir, screenGetTankPlayer(value));
  (*value)->x = x;
  (*value)->x <<= TANK_SHIFT_MAPSIZE;
  (*value)->x += MAP_SQUARE_MIDDLE;
  (*value)->y = y;
  (*value)->y <<= TANK_SHIFT_MAPSIZE;
  (*value)->y += MAP_SQUARE_MIDDLE;
  (*value)->angle = dir;
  (*value)->crc = 0;
  
  screenCenterTank();
  if (netGetType() == netSingle) {
    screenSetInStartFind(FALSE);
  }
  if (threadsGetContext() == FALSE  && netGetType() != netSingle) {
    (*value)->crc = tankCalcCRCSetup(value);
  }
}

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
void tankDestroy(tank *value, map *mp, pillboxes *pb, bases *bs) {
  tankCarryPb q;

  tankShuttingDown = TRUE;
  if ((*value) != NULL && threadsGetContext() == TRUE) {
    tankDropPills(value, mp, pb, bs);
  }
  tankShuttingDown = FALSE;
  while ((*value) != NULL && !IsEmpty((*value)->carryPills)) {
    q = (*value)->carryPills;
    (*value)->carryPills = TankPillsTail(q);
    Dispose(q);
  }
  
  if ((*value) != NULL) {
    Dispose(*value);
  }
}


//extern bool netSendNow;

/*********************************************************
*NAME:          tankUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/98
*LAST MODIFIED:   4/1/00
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
void tankUpdate(tank *value, map *mp, bases *bs, pillboxes *pb, shells *shs, starts *sts, tankButton tb, bool tankShoot, bool inBrain) {
  WORLD conv;                   /* Used for conversions */
  BYTE bmx;                   /* Map x and y co-ords as bytes */
  BYTE bmy; 



  (*value)->obstructed = FALSE;
  tankRegisterChangeByte(value, CRC_OBSTRUCTED_OFFSET, FALSE);
  (*value)->justFired = FALSE;
  tankRegisterChangeByte(value, CRC_JUSTFIRED_OFFSET, FALSE);
  /* Extract co-ords from world co-ords */
  conv = (*value)->x;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmx = (BYTE) conv;
  conv = (*value)->y;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmy = (BYTE) conv;

  /* Update the reload wait if required */
  if (((*value)->reload) > 0) {
    ((*value)->reload)--;
    tankRegisterChangeByte(value, CRC_RELOAD_OFFSET, (*value)->reload);
  }

  
  /* Shoot if required */
  if (tankShoot == TRUE && (*value)->reload == 0 && (*value)->shells > 0 && (*value)->armour <= TANK_FULL_ARMOUR)  {
    TURNTYPE a;
    TURNTYPE b = 2;
    TURNTYPE c;
    a = (*value)->sightLen;
    c = a / b;
    shellsAddItem(shs, (*value)->x, (*value)->y, (*value)->angle, c, screenGetTankPlayer(value), (*value)->onBoat);
    (*value)->reload = TANK_RELOAD_TIME;
    tankRegisterChangeByte(value, CRC_RELOAD_OFFSET, TANK_RELOAD_TIME);
    (*value)->shells--;
    tankRegisterChangeByte(value, CRC_SHELLS_OFFSET, (*value)->shells);
    if (threadsGetContext() == FALSE) {
      frontEndPlaySound(shootSelf);
      frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
    }
    (*value)->justFired = TRUE;
    tankRegisterChangeByte(value, CRC_JUSTFIRED_OFFSET, TRUE);
  }



  if ((*value)->deathWait > 0) {
	/* Tank is still waiting to respawn */
    (*value)->deathWait--;
    tankRegisterChangeByte(value, CRC_DEATHWAIT_OFFSET, (*value)->deathWait);
    if ((*value)->deathWait == 0) {
      (*value)->newTank = TRUE;
      tankRegisterChangeByte(value, CRC_NEWTANK_OFFSET, TRUE);
    }
  } else if ((*value)->armour > TANK_FULL_ARMOUR) {
	/* Tank just took enough damage to die */
    if (screenGetInStartFind() == FALSE) {
      tankDeath(value, sts);
    }
  } else if ((*value)->onBoat == FALSE && (mapGetPos(mp,bmx, bmy)) == DEEP_SEA && threadsGetContext() == FALSE) {
    /* Check for death by drowning */
/*    if (threadsGetContext() == FALSE) { */
      soundDist(tankSinkNear, bmx, bmy);
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(MESSAGE_TANKSUNK));
      netMNTAdd(screenGetNetMnt(), NMNT_KILLME, 0, screenGetTankPlayer(value), 0xFF, 0xFF);
      tankDropPills(value, mp, pb, bs);
      (*value)->armour = TANK_FULL_ARMOUR+1;
      tankRegisterChangeByte(value, CRC_ARMOUR_OFFSET, (*value)->armour);
      (*value)->deathWait = 255;
      tankRegisterChangeByte(value, CRC_DEATHWAIT_OFFSET, 255);
/*    } */
  } else if ((*value)->onBoat == TRUE) {
    /* Tank Movement on Boat */
	  if ((*value)->tankSlideTimer > 0) { /* Do we really need this for tank movement on a boat?  Does a tank slide in river? */
		(*value)->tankSlideTimer--;
	}
    (*value)->newTank = FALSE;
    tankRegisterChangeByte(value, CRC_NEWTANK_OFFSET, FALSE);
    tankMoveOnBoat(value, mp, pb, bs, bmx, bmy, tb, inBrain);
  } else {
    /* Tank Movement on Land */
	if ((*value)->tankSlideTimer > 0) {
		(*value)->tankSlideTimer--;
	}
    (*value)->newTank = FALSE;
    tankRegisterChangeByte(value, CRC_NEWTANK_OFFSET, FALSE);
    tankMoveOnLand(value, mp, pb, bs, bmx, bmy, tb, inBrain);
  }
}

/*********************************************************
*NAME:          tankisMoving
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
*  Returns whether the tank is in motion or not 
*
*ARGUMENTS:
*  value      - Pointer to the tank structure
*********************************************************/
bool tankIsMoving(tank *value) {
  bool returnValue = FALSE; /* Value to return */

  if ((*value)->speed != 0) {
    returnValue = TRUE;
  }
  return returnValue;
}

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
TURNTYPE tankGetAngle(tank *value) {
  return (*value)->angle;
}


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
BYTE tankGetDir(tank *value) {
  return utilGetDir((*value)->angle);
}

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
BYTE tankGet256Dir(tank *value) {
  return (BYTE) ((*value)->angle);
}

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
BYTE tankGetTravelAngel(tank *value) {
  return utilGet16Dir((*value)->angle);
}

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
BYTE tankGetSpeed(tank *value) {
  return (BYTE) (*value)->speed;
}

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
SPEEDTYPE tankGetActualSpeed(tank *value) {
  return (*value)->speed;
}

/*********************************************************
*NAME:          tankGetArmour
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/98
*LAST MODIFIED: 23/11/98
*PURPOSE:
*  Returns the tanks armour
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetArmour(tank *value) {
  return ((*value)->armour);
}

/*********************************************************
*NAME:          tankGetScreenMX
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns whether the tanks X Co-ord on the map
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetScreenMX(tank *value) {
  WORLD conv;       /* Useful in converting short to unsigned char */
  BYTE returnValue; /* Value to return */
  
  conv = (WORLD) (*value)->x - TANK_SUBTRACT;
  conv >>= TANK_SHIFT_MAPSIZE;
  returnValue = (BYTE) conv;
  return returnValue;
}

/*********************************************************
*NAME:          tankGetScreenPX
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns whether the tanks x pixel offset
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetScreenPX(tank *value) {
  WORLD conv;       /* Useful in converting short to unsigned char */
  BYTE returnValue; /* Value to return */
  
  conv = (*value)->x - TANK_SUBTRACT;
  conv <<= TANK_SHIFT_MAPSIZE;
  conv >>= TANK_SHIFT_PIXELSIZE;
  returnValue = (BYTE) conv;
  return returnValue;
}

/*********************************************************
*NAME:          tankGetScreenMY
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns whether the tanks y Co-ord on the map
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetScreenMY(tank *value) {
  WORLD conv;       /* Useful in converting short to unsigned char */
  BYTE returnValue; /* Value to return */
  
  conv = (WORLD) (*value)->y - TANK_SUBTRACT;
  conv >>= TANK_SHIFT_MAPSIZE;
  returnValue = (BYTE) conv;
  return returnValue;
}

/*********************************************************
*NAME:          tankGetScreenPY
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 24/11/98
*PURPOSE:
* Returns whether the tanks y pixel offset
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
BYTE tankGetScreenPY(tank *value) {
  WORLD conv;       /* Useful in converting short to unsigned char */
  BYTE returnValue; /* Value to return */
  
  conv = (*value)->y - TANK_SUBTRACT;
  conv <<= TANK_SHIFT_MAPSIZE;
  conv >>= TANK_SHIFT_PIXELSIZE;
  returnValue = (BYTE) conv;
  return returnValue;
}

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
BYTE tankGetMX(tank *value) {
  WORLD conv;       /* Useful in converting short to unsigned char */
  BYTE returnValue; /* Value to return */
  
  returnValue = 0;
  if ((*value)->armour <= TANK_FULL_ARMOUR) {
    conv = (*value)->x;
    conv >>= TANK_SHIFT_MAPSIZE;
    returnValue = (BYTE) conv;
  }
  return returnValue;
}

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
BYTE tankGetPX(tank *value) {
  WORLD conv;       /* Useful in converting short to unsigned char */
  BYTE returnValue; /* Value to return */
  
  conv = (*value)->x;
  conv <<= TANK_SHIFT_MAPSIZE;
  conv >>= TANK_SHIFT_PIXELSIZE;
  returnValue = (BYTE) conv;
  return returnValue;
}

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
BYTE tankGetMY(tank *value) {
  WORLD conv;       /* Useful in converting short to unsigned char */
  BYTE returnValue; /* Value to return */
  
  returnValue = 0;
  if ((*value)->armour <= TANK_FULL_ARMOUR) {
    conv = (*value)->y;
    conv >>= TANK_SHIFT_MAPSIZE;
    returnValue = (BYTE) conv;
  }
  return returnValue;
}

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
BYTE tankGetPY(tank *value) {
  WORLD conv;       /* Useful in converting short to unsigned char */
  BYTE returnValue; /* Value to return */
  
  conv = (*value)->y;
  conv <<= TANK_SHIFT_MAPSIZE;
  conv >>= TANK_SHIFT_PIXELSIZE;
  returnValue = (BYTE) conv;
  return returnValue;
}

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
void tankGetStats(tank *value, BYTE *shellsAmount, BYTE *minesAmount, BYTE *armourAmount, BYTE *treesAmount) {
  *shellsAmount = (*value)->shells;
  *minesAmount = (*value)->mines;
  *armourAmount = (*value)->armour;
  *treesAmount = (*value)->trees;
}

/*********************************************************
*NAME:          tankSetStats
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
void tankSetStats(tank *value, BYTE shellsAmount, BYTE minesAmount, BYTE armourAmount, BYTE treesAmount) {
  (*value)->shells = shellsAmount;
  tankRegisterChangeByte(value, CRC_SHELLS_OFFSET, shellsAmount);
  (*value)->mines = minesAmount;
  tankRegisterChangeByte(value, CRC_MINES_OFFSET, minesAmount);
  (*value)->armour = armourAmount;
  tankRegisterChangeByte(value, CRC_ARMOUR_OFFSET, armourAmount);
  (*value)->trees = treesAmount;
  tankRegisterChangeByte(value, CRC_TREES_OFFSET, treesAmount);
}

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
bool tankIsGunsightShow(tank *value) {
  return (*value)->showSight;
}

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
void tankGetGunsight(tank *value, BYTE *xMap, BYTE *yMap, BYTE *xPixel, BYTE *yPixel) {
  WORLD x;
  WORLD y;
  WORLD conv;
  int xAmount;
  int yAmount;
  unsigned int speed;

  if ((*value)->armour <= TANK_FULL_ARMOUR) {
    speed = (*value)->sightLen;
    speed <<= 7; /* TANK_SHIFT_MAPSIZE */
    utilCalcDistance(&xAmount, &yAmount, (*value)->angle, (int) speed);

    x = (WORLD) ((*value)->x + xAmount - TANK_SUBTRACT);
    y = (WORLD) ((*value)->y + yAmount - TANK_SUBTRACT);

    conv = x;
    conv >>= TANK_SHIFT_MAPSIZE;
    *xMap = (BYTE) conv;

    conv = y;
    conv >>= TANK_SHIFT_MAPSIZE;
    *yMap = (BYTE) conv;

    conv = x;
    conv <<= TANK_SHIFT_MAPSIZE;
    conv >>= TANK_SHIFT_PIXELSIZE;

    *xPixel = (BYTE) conv; 

    conv = y;
    conv <<= TANK_SHIFT_MAPSIZE;
    conv >>= TANK_SHIFT_PIXELSIZE;
    *yPixel = (BYTE) conv;
  } else {
    *xMap =0;
    *yMap =0;
    *xPixel =0;
    *yPixel =0;
  }
}

/*********************************************************
*NAME:          tankGunsightIncrease
*AUTHOR:        John Morrison
*CREATION DATE: 24/12/98
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Adds a map unit on to the tank gunsight range 
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*********************************************************/
void tankGunsightIncrease(tank *value) {
  if ((*value)->sightLen < GUNSIGHT_MAX) {
    (*value)->sightLen++;
    tankRegisterChangeByte(value, CRC_SIGHTLEN_OFFSET, (*value)->sightLen);
  } else if ((*value)->autoHideGunsight == TRUE && (*value)->showSight == TRUE) {
    (*value)->showSight = FALSE;
    tankRegisterChangeByte(value, CRC_SHOWSIGHT_OFFSET, FALSE);
    if (threadsGetContext() == FALSE) {
      frontEndShowGunsight(FALSE);
    }
  }
}

/*********************************************************
*NAME:          tankGunsightDecrease
*AUTHOR:        John Morrison
*CREATION DATE: 24/12/98
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Adds a map unit on to the tank gunsight range 
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*********************************************************/
void tankGunsightDecrease(tank *value) {
  if ((*value)->sightLen > GUNSIGHT_MIN) {
    (*value)->sightLen--;
    tankRegisterChangeByte(value, CRC_SIGHTLEN_OFFSET, (*value)->sightLen);
  }
  if ((*value)->showSight == FALSE && (*value)->autoHideGunsight == TRUE) {
    (*value)->showSight = TRUE;
    tankRegisterChangeByte(value, CRC_SHOWSIGHT_OFFSET, TRUE);
    if (threadsGetContext() == FALSE) {
      frontEndShowGunsight(TRUE);
    }
  }
}

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
void tankSetGunsight(tank *value, bool shown) {
 if ((*value) != NULL) { 
   (*value)->showSight = shown;
   tankRegisterChangeByte(value, CRC_SHOWSIGHT_OFFSET, shown);
   if (shown == FALSE) {
     (*value)->sightLen = GUNSIGHT_MAX;
     tankRegisterChangeByte(value, CRC_SIGHTLEN_OFFSET, GUNSIGHT_MAX);
   }
 }
}

/*********************************************************
*NAME:          tankGetWorld
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Gets the tanks world co-ordinates
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  x      - Pointer to hold X co-ord
*  y      - Pointer to hold Y co-ord
*********************************************************/
void tankGetWorld(tank *value, WORLD *x, WORLD *y) {
  *x = (*value)->x; 
  *y = (*value)->y;
}

/*********************************************************
*NAME:          tankSetWorld
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED:  8/1/00
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
void tankSetWorld(tank *value, WORLD x, WORLD y, TURNTYPE angle, bool setResources) {
  BYTE armour;
  BYTE mines;
  BYTE shells;
  BYTE trees;

  (*value)->x = x;
  tankRegisterChangeWorld(value, CRC_WORLDX_OFFSET, x);
  (*value)->y = y;
  tankRegisterChangeWorld(value, CRC_WORLDY_OFFSET, y);
  (*value)->angle = angle;
  tankRegisterChangeFloat(value, CRC_ANGLE_OFFSET, angle);
  if (setResources == TRUE) {
    gameTypeGetItems(screenGetGameType(), &shells, &mines, &armour, &trees);
    (*value)->shells = shells;
    tankRegisterChangeByte(value, CRC_SHELLS_OFFSET, shells);
    (*value)->mines = mines;
    tankRegisterChangeByte(value, CRC_MINES_OFFSET, mines);
    (*value)->armour = armour;
    tankRegisterChangeByte(value, CRC_ARMOUR_OFFSET, armour);
    (*value)->trees = trees;
    tankRegisterChangeByte(value, CRC_TREES_OFFSET, trees);
    if (threadsGetContext() == FALSE) {
      frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
    }
  }
}

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
tankHit tankIsTankHit(tank *value, map *mp, pillboxes *pb, bases *bs, WORLD x, WORLD y, TURNTYPE angle, BYTE owner) {
  tankHit returnValue; /* Value to return */
  WORLD conv;          /* Used in the conversion */
  int newX;            /* Amount to add because the tank has been hit */
  int newY;
  WORLD newmx;
  WORLD newmy;
  BYTE bmx;
  BYTE bmy;
  BYTE newbmx;       /* Test locations to check for a collision */
  BYTE newbmy; 
  bool needSend = FALSE; /* Do we need to send to the network */


  returnValue = TH_MISSED;
  if (*value == NULL) {
    return TH_MISSED;
  }

  if (threadsGetContext() == FALSE) {
    if (owner == screenGetTankPlayer(value)) {
      return TH_MISSED;
    }
  }

  returnValue = TH_MISSED;

  /*
   * TODO: here is where we would call a collision-detection function.  For now, we check to see
   * if the shell is within 128 WORLD coordinates of a tank's WORLD coordinates.  Since a tank's
   * WORLD coordinates are from the center, we assume that the tank is basically a circle.
   */
  if (abs((*value)->x - x) < 128 && abs((*value)->y - y) < 128  && (*value)->armour <= TANK_FULL_ARMOUR) {
    returnValue = TH_HIT;
    needSend = TRUE;
    if (threadsGetContext() == FALSE) { /* Fix Me */
      (*value)->armour -= DAMAGE;
      tankRegisterChangeByte(value, CRC_ARMOUR_OFFSET, (*value)->armour);
    }
    if ((*value)->onBoat == TRUE) {
      (*value)->onBoat = FALSE;
      tankRegisterChangeByte(value, CRC_ONBOAT_OFFSET, FALSE);
      (*value)->speed = 0;
      tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, 0);
      screenReCalc();
    }
	/* 
	 * Tank was at zero armor before it was hit with a shell.  When we decrement the armor counter, it
	 * "wraps" around and becomes larger than what a tank is supposed to have.  This signals that the 
	 * tank should die.
	 */
    if ((*value)->armour > TANK_FULL_ARMOUR) {
      if (((*value)->shells + (*value)->mines) > TANK_BIG_EXPLOSION_THRESHOLD) {
        returnValue = TH_KILL_BIG;
      } else {
        returnValue = TH_KILL_SMALL;
      }
      (*value)->deathWait = 255;
      tankRegisterChangeByte(value, CRC_DEATHWAIT_OFFSET, 255);
      
/*      netSendNow = TRUE; */
      tankDropPills(value, mp, pb, bs);
    }
	/* Tank was hit and survived */
	else { /* if ((*value)->armour <= TANK_FULL_ARMOUR)  */

	  (*value)->tankSlideTimer = 2000; /* TODO: make this a non-magical number, aka, #define */
      (*value)->tankSlideAngle = angle;

/*      utilCalcDistance(&newX, &newY, angle, MAP_SQUARE_MIDDLE); */
	  utilCalcTankSlide((*value)->tankSlideTimer, angle, &newX, &newY, MAP_SQUARE_MIDDLE);

      /* Check for Colisions */
      conv = (*value)->x;  
      conv >>= TANK_SHIFT_MAPSIZE;
      bmx = (BYTE) conv;
      conv = (*value)->y;
      conv >>= TANK_SHIFT_MAPSIZE;
      bmy = (BYTE) conv;

      newmx = (WORLD) ((*value)->x + newX);
      newmy = (WORLD) ((*value)->y + newY);


      newmx >>= TANK_SHIFT_MAPSIZE;
      newmy >>= TANK_SHIFT_MAPSIZE;
      newbmx = (BYTE) newmx;
      newbmy = (BYTE) newmy;

      if ((mapGetSpeed(mp,pb,bs,bmx,newbmy,(*value)->onBoat, screenGetTankPlayer(value))) > 0) {
        (*value)->y = (WORLD) ((*value)->y + newY);
        tankRegisterChangeWorld(value, CRC_WORLDY_OFFSET, (*value)->y);
        bmy = newbmy;
      }
      if ((mapGetSpeed(mp,pb,bs,newbmx,bmy,(*value)->onBoat, screenGetTankPlayer(value))) > 0) {
        (*value)->x = (WORLD) ((*value)->x + newX);
        tankRegisterChangeWorld(value, CRC_WORLDX_OFFSET, (*value)->x);
        bmx = newbmx;
      }
      /* Check for scroll of screen */
      screenTankScroll();
    }
    if ((*value)->armour <= TANK_FULL_ARMOUR) {
      if (threadsGetContext() == FALSE) {
        frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
        if (needSend == TRUE) {
          /* Send hit to network */
          netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 1 , (BYTE) ((*value)->armour / 5) );
        }
      }
    } else {
      if (threadsGetContext() == FALSE) {
        frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, 0, (*value)->trees);
      }
    }
  }
  return returnValue;
}


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
********************************************************/
void tankInWater(tank *value) {
  bool modsMade; /* Has any modifications been made */

  modsMade = FALSE;
  if ((*value)->shells > 0) {
    (*value)->shells--;
    tankRegisterChangeByte(value, CRC_SHELLS_OFFSET, (*value)->shells);
    modsMade = TRUE;
  }
  if ((*value)->mines > 0) {
    (*value)->mines--;
    tankRegisterChangeByte(value, CRC_MINES_OFFSET, (*value)->mines);
    modsMade = TRUE;
  }

  if (modsMade == TRUE) {
    /* Update view and play sound */
    if (threadsGetContext() == FALSE) {
      frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
      frontEndPlaySound(bubbles);
    }
  }
}


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
BYTE tankGetFrame(tank *value) {
  BYTE returnValue; /* Value to return */
  
  if ((*value)->armour > TANK_FULL_ARMOUR) {
    returnValue = TANK_TRANSPARENT;
  } else {
    returnValue = utilGetDir((*value)->angle);
    if ((*value)->onBoat == TRUE) {
      returnValue += TANK_BOAT_ADD;
    }
  }
  return returnValue;
}

/*********************************************************
*NAME:          tankDeath
*AUTHOR:        John Morrison
*CREATION DATE: 7/1/99
*LAST MODIFIED: 9/1/00
*PURPOSE:
*  The tank has died. Reinit its location and increment
*  the death count
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*  sts   - Pointer to the starts structure
*********************************************************/
void tankDeath(tank *value, starts *sts) {
  BYTE shellAmount; /* Stuff the new tank is to start with */
  BYTE minesAmount;
  BYTE armourAmount;
  BYTE treesAmount;
  BYTE x;       /* New location of the tank */
  BYTE y;
  TURNTYPE dir;

  if (netGetType() == netSingle || threadsGetContext() == FALSE) { 
    logAddEvent(log_PlayerLocation, screenGetTankPlayer(value), 0, 0, 0, 0, NULL); 
    lgmTankDied(screenGetLgmFromPlayerNum(screenGetTankPlayer(value)));
    if (threadsGetContext() == FALSE) {
        if (netGetType() == netUdp && screenGetInStartFind() == FALSE) {
          screenSetInStartFind(TRUE);
          netRequestStartPosition();
        } else {
          gameTypeGetItems(screenGetGameType(), &shellAmount, &minesAmount, &armourAmount, &treesAmount);
          (*value)->armour = armourAmount;
          tankRegisterChangeByte(value, CRC_ARMOUR_OFFSET, (*value)->armour);
          (*value)->tankHitCount = 0;
          (*value)->shells = shellAmount;
          tankRegisterChangeByte(value, CRC_SHELLS_OFFSET, (*value)->shells);
          (*value)->mines = minesAmount;
          tankRegisterChangeByte(value, CRC_MINES_OFFSET, (*value)->mines);
          (*value)->trees = treesAmount;        
          tankRegisterChangeByte(value, CRC_TREES_OFFSET, (*value)->trees);
          screenSetInStartFind(TRUE);
          startsGetStart(sts, &x, &y, &dir, screenGetTankPlayer(value));
          (*value)->x = x;
          (*value)->x <<= TANK_SHIFT_MAPSIZE;
          (*value)->x += MAP_SQUARE_MIDDLE;
          tankRegisterChangeWorld(value, CRC_WORLDX_OFFSET, (*value)->x);
          (*value)->y = y;
          (*value)->y <<= TANK_SHIFT_MAPSIZE;
          (*value)->y += MAP_SQUARE_MIDDLE;
          tankRegisterChangeWorld(value, CRC_WORLDY_OFFSET, (*value)->y);
          (*value)->angle = dir;
          tankRegisterChangeFloat(value, CRC_ANGLE_OFFSET, dir);
          screenTankView();
          screenSetInStartFind(FALSE);
          if (threadsGetContext() == FALSE) {
            frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
          }
        }
      }
    (*value)->onBoat = TRUE;
    tankRegisterChangeByte(value, CRC_ONBOAT_OFFSET, TRUE);
    (*value)->numDeaths++;
    tankRegisterChangeInt(value, CRC_NUMDEATHS_OFFSET, (*value)->numDeaths);
    (*value)->reload = 0;
    tankRegisterChangeByte(value, CRC_RELOAD_OFFSET, 0);
    (*value)->speed = 0;
    tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, 0);
    (*value)->waterCount = 0;
    tankRegisterChangeByte(value, CRC_WATERCOUNT_OFFSET, 0);
    /* Get the start position */
    if (threadsGetContext() == FALSE) {
      frontEndKillsDeaths((*value)->numKills, (*value)->numDeaths);
    }
  } else if (threadsGetContext() == TRUE) { /* FIXME */
    (*value)->armour = 10;
  }

}



/*********************************************************
*NAME:          tankGetKillsDeaths
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/99
*PURPOSE:
*  Returns the number of kills and deaths the tank has had
*  into the passed pointers
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  kills  - Pointer to hold the number of kills
*  deaths - Pointer to hold the number of deaths
*********************************************************/
void tankGetKillsDeaths(tank *value, int *kills, int *deaths) {
  *kills = (*value)->numKills;
  *deaths = (*value)->numDeaths;
}

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
void tankAddArmour(tank *value, BYTE amount) {
  if ((*value)->armour + amount <= TANK_FULL_ARMOUR) {
    (*value)->armour += amount;
    tankRegisterChangeByte(value, CRC_ARMOUR_OFFSET, (*value)->armour);
    if (threadsGetContext() == FALSE) {
      frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
    }
  }
}

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
void tankAddShells(tank *value, BYTE amount) {
  if ((*value)->shells + amount <= TANK_FULL_SHELLS) {
    (*value)->shells += amount;
    tankRegisterChangeByte(value, CRC_SHELLS_OFFSET, (*value)->shells);
    if (threadsGetContext() == FALSE) {
      frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
    }
  }
}

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
void tankAddMines(tank *value, BYTE amount) {
  if ((*value)->mines + amount <= TANK_FULL_MINES) {
    (*value)->mines += amount;
    tankRegisterChangeByte(value, CRC_MINES_OFFSET, (*value)->mines);
    if (threadsGetContext() == FALSE) {
      frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
    }
  }
}

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
void tankMoveOnBoat(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE bmx, BYTE bmy, tankButton tb, bool inBrain) {
  WORLD newmx;   /* Move world co-ords */
  WORLD newmy;
  int xAmount;   /* The distance to add or subtract to th location */
  int yAmount;   /* depending on the speed of the tank */
  BYTE ang;
  BYTE newbmx;
  BYTE newbmy;
  BYTE boatExitSquare;

  /* Check terrain is clear */
  tankCheckGroundClear(value, mp, pb, bs);

  if (threadsGetContext() == FALSE) {
    tankTurn(value, mp, pb, bs, bmx, bmy, tb);
  }
  tankAccel(value, mp, pb, bs, bmx, bmy, tb);


  /* Update location if speed > 0 */
  if (((*value)->speed) > 0) {
    /* If we have autoslowdown turned on - SLOW DOWN! */
    if ((*value)->autoSlowdown == TRUE && inBrain == FALSE) {
      if (tb != TDECEL && tb != TLEFTDECEL && tb != TRIGHTDECEL && tb != TACCEL && tb != TLEFTACCEL && tb != TRIGHTACCEL) {
        (*value)->speed -= TANK_AUTOSLOW_SPEED;
        if ((*value)->speed < 0) {
          (*value)->speed = 0;
        }
        tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, (*value)->speed);
      }
    }

    ang = utilGet16Dir((*value)->angle);
    utilCalcDistance(&xAmount, &yAmount, (TURNTYPE) ang, (int) (*value)->speed);
    /* Check to make sure updating isn't going to runinto something. If not update co-ordinates */
    newmx = (WORLD) ((*value)->x + xAmount);
    newmy = (WORLD) ((*value)->y + yAmount);

    if (yAmount > 0) {
      newmy += TANK_MOVE_BOAT_SUB;
    } else {
      newmy -= TANK_MOVE_BOAT_SUB;
    }
    
    if (xAmount >= 0) {
      newmx += TANK_MOVE_BOAT_SUB;
    } else {
      newmx -= TANK_MOVE_BOAT_SUB;
    } 

    newmx >>= TANK_SHIFT_MAPSIZE;
    newmy >>= TANK_SHIFT_MAPSIZE;
    newbmx = (BYTE) newmx;
    newbmy = (BYTE) newmy;

    if ((mapIsLand(mp, pb, bs, bmx, newbmy)) == FALSE) {
      (*value)->y = (WORLD) ((*value)->y + yAmount);
    } else if ((mapGetSpeed(mp,pb,bs,bmx,newbmy,(*value)->onBoat, screenGetTankPlayer(value))) > 0 && (*value)->speed >= BOAT_EXIT_SPEED ) {
      (*value)->y = (WORLD) ((*value)->y + yAmount);
    } else {
      newbmy = bmy;
    } 
    tankRegisterChangeWorld(value, CRC_WORLDY_OFFSET, (*value)->y);
      
    if ((mapIsLand(mp, pb, bs, newbmx,newbmy)) == FALSE) {
      (*value)->x = (WORLD) ((*value)->x + xAmount);
    } else if ((mapGetSpeed(mp,pb,bs,newbmx,newbmy,(*value)->onBoat, screenGetTankPlayer(value))) > 0 && (*value)->speed >= BOAT_EXIT_SPEED ) {
      (*value)->x = (WORLD) ((*value)->x + xAmount);
    } else {
      newbmx = bmx;
    }
    tankRegisterChangeWorld(value, CRC_WORLDX_OFFSET, (*value)->x);
    
    /* Reget newbmx and newbmy */
    newmx = (*value)->x;
    newmy = (*value)->y;

    newmx >>= TANK_SHIFT_MAPSIZE;
    newmy >>= TANK_SHIFT_MAPSIZE;
    newbmx = (BYTE) newmx;
    newbmy = (BYTE) newmy;

    if (newbmy < bmy) {
      if (threadsGetContext() == FALSE) {
        if (frontEndTutorial(newbmy) == TRUE) {
          (*value)->speed = 0;
        }
      }
    }
    if (threadsGetContext() == TRUE || netGetType() == netSingle) {
      tankCheckPillCapture(value,pb);
    }
    /* Check for leaving boat */
    if ((mapIsLand(mp, pb, bs, newbmx, newbmy)) == TRUE) {
      boatExitSquare = mapGetPos(mp,newbmx,newbmy);
      if (boatExitSquare == BOAT) {
        if (threadsGetContext() == FALSE) {
          netMNTAdd(screenGetNetMnt(), NMNT_DROPBOAT, 0, screenGetTankPlayer(value), newbmx, newbmy);
          netMNTAdd(screenGetNetMnt(), NMNT_RUNOVERBOAT, 0, screenGetTankPlayer(value), newbmx, newbmy);
          mapSetPos(mp,newbmx,newbmy,RIVER, TRUE, FALSE);
        }
        explosionsAddItem(screenGetExplosions(), newbmx, newbmy, 0, 0, EXPLOSION_START);
        
        soundDist(shotBuildingNear, newbmx, newbmy);
        screenReCalc();
      } else if (boatExitSquare != BUILDING && boatExitSquare != HALFBUILDING) {
        if (mapGetPos(mp,bmx,bmy) == RIVER) {
          if (threadsGetContext() == FALSE) {
            mapSetPos(mp,bmx,bmy,BOAT, TRUE, FALSE);
            netMNTAdd(screenGetNetMnt(), NMNT_BUILDBOAT, 0, screenGetTankPlayer(value), bmx, bmy);
          }
        }
        (*value)->onBoat = FALSE;
        tankRegisterChangeByte(value, CRC_ONBOAT_OFFSET, FALSE);
        screenReCalc();
      }
      /* OK We have successfully left the boat */
      if ((*value)->onBoat == FALSE) {
        /* Check for Mine hit */
        if (mapIsMine(mp, newbmx, newbmy) == TRUE) {
          netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, screenGetTankPlayer(value), newbmx, newbmy);
          minesExpAddItem(screenGetMinesExp(), mp, newbmx, newbmy);
        }
      }
    }

    /* Check for hit mine on outer map edges */
    if (mapIsMine(mp, bmx, bmy) == TRUE) {
      soundDist(mineExplosionNear, bmx, bmy);
      explosionsAddItem(screenGetExplosions(), bmx, bmy, 0, 0, EXPLOSION_START);
      (*value)->onBoat = FALSE;
      tankRegisterChangeByte(value, CRC_ONBOAT_OFFSET, FALSE);
      (*value)->speed = 0;
      tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, 0);
    }

   /* Check for pb capture */
  }

  tankNearMines(mp, bmx, bmy);
  
}

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
void tankMoveOnLand(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE bmx, BYTE bmy, tankButton tb, bool inBrain) {
  WORLD newmx;   /* Move world co-ords */
  WORLD newmy;
  int xAmount;   /* The distance to add or subtract to th location */
  int yAmount;   /* depending on the speed of the tank */
  BYTE ang;
  BYTE newbmx;
  BYTE newbmy;
  BYTE baseNum;  /* The base number for a capture */
  bool slowDown; /* Only if we can't move in either direction we should slow down */
  bool isServer;

  isServer = threadsGetContext();

  /* Check terrain is clear */
  tankCheckGroundClear(value, mp, pb, bs);

  if (threadsGetContext() == FALSE) {
    tankTurn(value, mp, pb, bs, bmx, bmy, tb);
  }
  tankAccel(value, mp, pb, bs, bmx, bmy, tb);
  slowDown = FALSE;

  
  /* Update location if speed > 0 */
  if (((*value)->speed) > 0) {
    /* If we have autoslowdown turned on - SLOW DOWN! */
    if ((*value)->autoSlowdown == TRUE && inBrain == FALSE) {
      if (tb != TDECEL && tb != TLEFTDECEL && tb != TRIGHTDECEL && tb != TACCEL && tb != TLEFTACCEL && tb != TRIGHTACCEL) {
        (*value)->speed -= TANK_AUTOSLOW_SPEED;
        if ((*value)->speed < 0) {
          (*value)->speed = 0;
        }
      }
      tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, (*value)->speed);
    }
    ang = utilGet16Dir((*value)->angle);
    
	/* Was the tank hit by a shell recently? */
	if ((*value)->tankSlideTimer > 0) {
		utilCalcTankSlide((*value)->tankSlideTimer, (*value)->tankSlideAngle, &xAmount, &yAmount, (int)(*value)->speed);
	} else {
		utilCalcDistance(&xAmount, &yAmount, (TURNTYPE) ang, (int) (*value)->speed);
	}

    /* Check to make sure updating isn't going to run into something. If not update co-ordinates */
    newmx = (WORLD) ((*value)->x + xAmount);
    newmy = (WORLD) ((*value)->y + yAmount);

    if (yAmount > 0) {
      newmy += TANK_MOVE_LAND_SUB+32;
    } else if (yAmount < 0){
      newmy -= TANK_MOVE_LAND_SUB-32;
    }  else if (ang > BRADIANS_EAST && ang < BRADIANS_WEST && ang != BRADIANS_SOUTH) {
      newmy += TANK_MOVE_LAND_SUB+32;
    } else if (ang != BRADIANS_NORTH) {
      newmy -= TANK_MOVE_LAND_SUB-32;
    } 
    
    if (xAmount >= 0) {
      newmx += TANK_MOVE_LAND_SUB+32;
    } else if (xAmount < 0) {
      newmx -= TANK_MOVE_LAND_SUB-32;
    } else if (ang > BRADIANS_NORTH && ang < BRADIANS_SOUTH && ang != BRADIANS_EAST) {
      newmx += TANK_MOVE_LAND_SUB+32;
    } else if (ang != BRADIANS_WEST) {
      newmx -= TANK_MOVE_LAND_SUB-32;
    } 

    newmx >>= TANK_SHIFT_MAPSIZE;
    newmy >>= TANK_SHIFT_MAPSIZE;
    newbmx = (BYTE) newmx;
    newbmy = (BYTE) newmy;

    if ((mapGetSpeed(mp,pb,bs,bmx,newbmy,(*value)->onBoat, screenGetTankPlayer(value))) > 0) {
      (*value)->y = (WORLD) ((*value)->y + yAmount);
      tankRegisterChangeWorld(value, CRC_WORLDY_OFFSET, (*value)->y);
    } else {
      slowDown = TRUE; 
      (*value)->obstructed = TRUE;
      tankRegisterChangeByte(value, CRC_OBSTRUCTED_OFFSET, TRUE);
/*
      (*value)->speed -= TANK_WALL_SLOW_DOWN;
      if ((*value)->speed < 0) {
        (*value)->speed = 0;
      }
*/
    }
    if ((mapGetSpeed(mp,pb,bs,newbmx,bmy,(*value)->onBoat, screenGetTankPlayer(value))) > 0) {
      (*value)->x = (WORLD) ((*value)->x + xAmount);
      tankRegisterChangeWorld(value, CRC_WORLDX_OFFSET, (*value)->x);
    } else if (slowDown == TRUE) {
      (*value)->speed-= TANK_WALL_SLOW_DOWN;
      if ((*value)->speed < 0) {
        (*value)->speed = 0;
      }
      tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, (*value)->speed);
      (*value)->obstructed = TRUE;
      tankRegisterChangeByte(value, CRC_OBSTRUCTED_OFFSET, TRUE);
    } else {
      (*value)->obstructed = TRUE;
      tankRegisterChangeByte(value, CRC_OBSTRUCTED_OFFSET, TRUE);
    } 
    
    newmy = (*value)->y;
    newmx = (*value)->x;
    newmx >>= TANK_SHIFT_MAPSIZE;
    newmy >>= TANK_SHIFT_MAPSIZE;
    newbmx = (BYTE) newmx;
    newbmy = (BYTE) newmy;
    if (newbmy < bmy) {
      if (threadsGetContext() == FALSE) {
        if (frontEndTutorial(newbmy) == TRUE) {
          (*value)->speed = 0;
        }
      }
    }


    /* Check for entering Boat */
    if ((mapGetPos(mp,newbmx, newbmy)) == BOAT) {
      if (threadsGetContext() == FALSE || netGetType() == netSingle) {
        netMNTAdd(screenGetNetMnt(), NMNT_DROPBOAT, 0, screenGetTankPlayer(value), newbmx, newbmy);
        mapSetPos(mp,newbmx,newbmy,RIVER, TRUE, FALSE);
        (*value)->onBoat = TRUE;
        tankRegisterChangeByte(value, CRC_ONBOAT_OFFSET, TRUE);
      }

      screenReCalc();
    }

    /* Check for hit mine */
    if (newbmx != bmx || newbmy != bmy) { /* && isServer == FALSE */
      if (mapIsMine(mp, newbmx, newbmy) == TRUE) {
        netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, screenGetTankPlayer(value), newbmx, newbmy);
        minesExpAddItem(screenGetMinesExp(), mp, newbmx, newbmy);
      }
    }
  }

  
  /* Check for tank in water */
  if (((mapGetPos(mp, bmx, bmy)) == RIVER) && (*value)->speed <= MAP_SPEED_TRIVER && (*value)->onBoat == FALSE) {
    if (basesExistPos(bs, bmx, bmy) == FALSE) {
      (*value)->waterCount++;
      tankRegisterChangeByte(value, CRC_WATERCOUNT_OFFSET, (*value)->waterCount);
      if ((*value)->waterCount == TANK_WATER_TIME) {
        (*value)->waterCount = 0;
        tankRegisterChangeByte(value, CRC_WATERCOUNT_OFFSET, 0);
        tankInWater(value);
      }
    }
  }
  /* Check for capture base */
  if (isServer == TRUE || netGetType() == netSingle) {
    if (baseIsCapturable(bs, bmx, bmy) == TRUE) {
      if (basesAmOwner(bs, screenGetTankPlayer(value), bmx, bmy) == FALSE) {
        basesSetOwner(bs, bmx, bmy, screenGetTankPlayer(value), FALSE);
        baseNum = basesGetBaseNum(bs, bmx, bmy);
        netPNBAdd(screenGetNetPnb(), NPNB_BASE_CAPTURE, (BYTE) (baseNum-1), screenGetTankPlayer(value), bmx, bmy);
        if (threadsGetContext() == FALSE) {
          frontEndStatusBase(baseNum, (basesGetStatusNum(bs, baseNum)));
        }
        screenReCalc();
      }
    }
  }


  /* Check for pb capture */
  if (isServer == TRUE || netGetType() == netSingle) {
    tankCheckPillCapture(value,pb);
  }
  /* Check for near mines */
  tankNearMines(mp, bmx, bmy);
}

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
void tankTurn(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE bmx, BYTE bmy, tankButton tb) {
  static BYTE firstLeft = 0; /* The first turn left tick */
  static BYTE firstRight = 0; /* The first turn left tick */
  TURNTYPE turnAmount; /* Amount to turn */

  /* Left turn */
  if (tb == TLEFT || tb == TLEFTACCEL || tb == TLEFTDECEL) {
    turnAmount = mapGetTurnRate(mp,pb,bs,bmx,bmy,(*value)->onBoat, screenGetTankPlayer(value));
    if (firstLeft < 10) {
      firstLeft++;
      turnAmount /=2;
    }
    (*value)->angle -= turnAmount;
    if ((*value)->angle < 0) {
      (*value)->angle = (TURNTYPE) (BRADIANS_MAX + (*value)->angle);     
    }
    tankRegisterChangeFloat(value, CRC_ANGLE_OFFSET, (*value)->angle);
  } else {
    firstLeft = 0;
  }
  /* Right Turn */
  if (tb == TRIGHT || tb == TRIGHTACCEL || tb == TRIGHTDECEL) {
    turnAmount = mapGetTurnRate(mp,pb,bs,bmx,bmy,(*value)->onBoat, screenGetTankPlayer(value));
    if (firstRight < 10) {
      firstRight++;
      turnAmount /=2;
    }
    (*value)->angle += turnAmount;
    if ((*value)->angle > BRADIANS_MAX) {
      (*value)->angle = (TURNTYPE) ((*value)->angle - BRADIANS_MAX);
    }
    tankRegisterChangeFloat(value, CRC_ANGLE_OFFSET, (*value)->angle);
  } else {
    firstRight = 0;
  }
}

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
void tankAccel(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE bmx, BYTE bmy, tankButton tb) {
  BYTE displace; /* Amount to move */
  SPEEDTYPE subAmount;     /* Amount to subtract */

  displace = mapGetSpeed(mp,pb,bs,bmx,bmy,(*value)->onBoat, screenGetTankPlayer(value));
  if ((tb == TDECEL || tb == TLEFTDECEL || tb == TRIGHTDECEL) || (*value)->speed > displace)  {
    subAmount = (*value)->speed;
    if ((*value)->speed > displace) {
      subAmount = (SPEEDTYPE) ((*value)->speed - TANK_TERRAIN_DECEL_RATE);
    }
    if (tb == TDECEL || tb == TLEFTDECEL || tb == TRIGHTDECEL) {
      subAmount -= (float) TANK_SLOWKEY_RATE;
    }
    if (subAmount > (*value)->speed) {
      ((*value)->speed) = 0;      
    } else {
      (*value)->speed = subAmount;
      if ((*value)->speed < 0) {
        (*value)->speed = 0;
      }
    }
    tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, (*value)->speed);
  } else if ((*value)->speed < displace && (tb == TACCEL || tb == TLEFTACCEL || tb == TRIGHTACCEL))  {
    ((*value)->speed) += TANK_ACCELERATE_RATE;
    if ((*value)->speed > displace) {
      (*value)->speed = displace;
    }
    tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, (*value)->speed);
  }
}

bool pillsIsCapturable(pillboxes *value, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          tankCheckPillCapture
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 23/6/00
*PURPOSE:
* Function checks for pillbox captures. If it does capture
* one then it updates everything.
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  pb     - Pointer to the pillbox structure
*********************************************************/
void tankCheckPillCapture(tank *value, pillboxes *pb) {
  WORLD conv;     /* Used for conversion */
  BYTE bmx;       /* Current MAP x-coord of tank */
  BYTE bmy;       /* Current MAP y-coord of tank */
  BYTE pillNum;   /* The pill number */
  tankCarryPb q;  /* Temp pointer for adding PBs to tank */

  /* Tank is alive and we are either in a server context or a non-network game */
  if ((*value)->armour <= TANK_FULL_ARMOUR && (threadsGetContext() == TRUE || netGetType() == netSingle)) {

	conv = (*value)->x;
    conv >>= TANK_SHIFT_MAPSIZE;
    bmx = (BYTE) conv;
    conv = (*value)->y;
    conv >>= TANK_SHIFT_MAPSIZE;
    bmy = (BYTE) conv;

    if (bmx != 0 && bmy != 0 && pillsIsCapturable(pb, bmx,bmy) == TRUE) {
      pillNum = pillsGetPillNum(pb, bmx, bmy, TRUE, FALSE);
      while (pillNum != PILL_NOT_FOUND) {
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_PICKUP, (BYTE) (pillNum-1), screenGetTankPlayer(value), 0, 0);
        pillsSetPillInTank(pb,pillNum, TRUE);
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_CAPTURE, (BYTE) (pillNum-1) , screenGetTankPlayer(value), 0, 0);
        if (threadsGetContext() == FALSE) {
          frontEndStatusPillbox(pillNum, (pillsGetAllianceNum(pb, pillNum)));
        }
        New(q);
        q->pillNum = pillNum;
        q->next = (*value)->carryPills;
        (*value)->carryPills = q;
        if ((pillsGetPillOwner(pb, pillNum)) != screenGetTankPlayer(value)) {
          pillsSetPillOwner(pb, pillNum, screenGetTankPlayer(value), FALSE);
        }
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_PICKUP, (BYTE) (pillNum-1), screenGetTankPlayer(value), 0, 0);
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_CAPTURE, (BYTE) (pillNum-1) , screenGetTankPlayer(value), 0, 0);
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_PICKUP, (BYTE) (pillNum-1), screenGetTankPlayer(value), 0, 0);
        if (pillsExistPos(pb, bmx, bmy) == TRUE) {
          pillNum = pillsGetPillNum(pb, bmx, bmy, TRUE, FALSE);
        } else {
          pillNum = PILL_NOT_FOUND;
        }
      }
      screenReCalc();
    } 
  }
}

/*********************************************************
*NAME:          tankDropPills
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 21/6/00
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
void tankDropPills(tank *value, map *mp, pillboxes *pb, bases *bs) {
  WORLD conv;     /* Used for conversion */
  BYTE bmx;       /* Current Position of Tank */
  BYTE bmy;
  BYTE numPills;  /* The number of pills on the tank */
  BYTE width;     /* How wide the pills length should be */
  tankCarryPb q;  /* Temp pointer for removing pills */
  pillbox item;   /* Item to add to the pillbox */
  BYTE count;     /* Looping variable */
  BYTE pos;       /* The Map position */


  /* Get the number of pills on the tank */
  numPills = 0;
  q = (*value)->carryPills;
  while (NonEmpty(q)) {
    numPills++;
    q = TankPillsTail(q);
  }
  if (numPills > 0 && (threadsGetContext() == TRUE || netGetType() == netSingle)) {
    count = 0;
    item.armour = 0;
    item.owner = screenGetTankPlayer(value);
    item.speed = PILLBOX_ATTACK_NORMAL;
    item.reload = PILLBOX_ATTACK_NORMAL;
    item.coolDown = 0;
    item.inTank = FALSE;
    item.justSeen = FALSE;
    /* Get tank location */
    conv = (*value)->x;
    conv >>= TANK_SHIFT_MAPSIZE;
    bmx = (BYTE) conv;
    conv = (*value)->y;
    conv >>= TANK_SHIFT_MAPSIZE;
    bmy = (BYTE) conv;
    /* Get the width for depositing them */
    width = (BYTE) sqrt((double) numPills);
    bmx -= width/2;
    bmy -= width/2;

    if (bmx >= 0 && bmx <= MAP_MINE_EDGE_LEFT) {
      bmx = MAP_MINE_EDGE_LEFT+1;
    } else if (bmx >= MAP_MINE_EDGE_RIGHT) {
      bmx = MAP_MINE_EDGE_RIGHT - 10;
    }

    if (bmy >= 0 && bmy <= MAP_MINE_EDGE_TOP) {
      bmy = MAP_MINE_EDGE_TOP+1;
    } else if (bmy >= MAP_MINE_EDGE_BOTTOM) {
      bmy = MAP_MINE_EDGE_BOTTOM - 10;
    }

    while (NonEmpty((*value)->carryPills)) {
      q = (*value)->carryPills;
      if (threadsGetContext() == TRUE || netGetType() == netSingle) { // threadsGetContext() == FALSE || tankShuttingDown == TRUE
        item.x = bmx;
        item.y = bmy+count;
        if (item.x > MAP_MINE_EDGE_LEFT && item.x < MAP_MINE_EDGE_RIGHT && item.y > MAP_MINE_EDGE_TOP && item.y < MAP_MINE_EDGE_BOTTOM) {
          pos = mapGetPos(mp, item.x, item.y);
          if (pillsExistPos(pb, item.x, item.y) == FALSE && basesExistPos(bs, item.x, item.y) == FALSE && pos != BUILDING && pos != HALFBUILDING && pos != BOAT) {
            if (threadsGetContext() == TRUE || netGetType() == netSingle) {
              pillsSetPill(pb,&item,q->pillNum);
              netPNBAdd(screenGetNetPnb(), NPNB_PILL_DEAD, (BYTE) ((q->pillNum) - 1), screenGetTankPlayer(value), item.x, item.y);
            }
            if (threadsGetContext() == FALSE) {
              frontEndStatusPillbox(q->pillNum, (pillsGetAllianceNum(pb, q->pillNum)));
            }
            (*value)->carryPills = TankPillsTail(q);
            Dispose(q);
          }
        }
        count++;
        if (count == width) {
          count = 0;
          item.y = bmy;
          bmx++;
        }
      } else {
//        (*value)->carryPills = TankPillsTail(q);
//        Dispose(q);
      }

    }
    screenReCalc();
  }
//  if (threadsGetContext() == FALSE && netGetType() != netSingle) {
//    pillsExplicitDrop(pb, screenGetTankPlayer(value));
//  }
  return;
}

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
bool tankIsOnBoat(tank *value) {
  return ((*value)->onBoat);
}

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
bool tankGetLgmTrees(tank *value, BYTE amount, bool perform) {
  bool returnValue; /* Value to return */

  returnValue = FALSE;
  if(((*value)->trees - amount) >= 0) {
    returnValue = TRUE;
    if (perform == TRUE) {
      (*value)->trees -= amount;
      tankRegisterChangeByte(value, CRC_TREES_OFFSET, (*value)->trees);
      if ((*value)->armour <= TANK_FULL_ARMOUR) {
        if (threadsGetContext() == FALSE) {
          frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
        }
      } else {
        if (threadsGetContext() == FALSE) {
          frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, 0, (*value)->trees);
        }
      }
    }
  }
  return returnValue;
}

/*********************************************************
*NAME:          tankGiveTrees
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 17/1/99
*PURPOSE:
* Adds the amount given in the amount to the tanks stocks
* and updates the displat
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  amount - Amount of trees to add
*********************************************************/
void tankGiveTrees(tank *value, BYTE amount) {
  (*value)->trees += amount;
  if ((*value)->trees > TANK_FULL_TREES) {
    (*value)->trees = TANK_FULL_TREES;
  }
  tankRegisterChangeByte(value, CRC_TREES_OFFSET, (*value)->trees);
  if ((*value)->armour <= TANK_FULL_ARMOUR) {
    if (threadsGetContext() == FALSE) {
      frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
    }
  } else {
    if (threadsGetContext() == FALSE) {
      frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, 0, (*value)->trees);
    }
  }
}

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
bool tankGetLgmMines(tank *value, BYTE amount, bool perform) {
  bool returnValue; /* Value to return */

  returnValue = FALSE;
  if(((*value)->mines - amount) >= 0) {
    returnValue = TRUE;
    if (perform == TRUE) {
      (*value)->mines -= amount;
      tankRegisterChangeByte(value, CRC_MINES_OFFSET, (*value)->mines);
      if ((*value)->armour <= TANK_FULL_ARMOUR) {
        if (threadsGetContext() == FALSE) {
          frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
        }
      } else {
        if (threadsGetContext() == FALSE) {
          frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, 0, (*value)->trees);
        }
      }
    }
  }
  return returnValue;
}

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
*  value  - Pointer to the tank structure
*  amount - Amount of mines to add
*********************************************************/
void tankGiveMines(tank *value, BYTE amount) {
  (*value)->mines += amount;
  if ((*value)->mines > TANK_FULL_MINES) {
    (*value)->mines = TANK_FULL_MINES;
  }
  tankRegisterChangeByte(value, CRC_MINES_OFFSET, (*value)->mines);
  if ((*value)->armour <= TANK_FULL_ARMOUR) {
    if (threadsGetContext() == FALSE) {
      frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
    }
  } else {
    if (threadsGetContext() == FALSE) {
      frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, 0, (*value)->trees);
    }
  }
}

/*********************************************************
*NAME:          tankGetCarriedPill
*AUTHOR:        John Morrison
*CREATION DATE: 17/01/99
*LAST MODIFIED: 01/02/03
*PURPOSE:
* Gets the first available carried pill. If none are 
* avaiable it returns FALSE
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*  pillNum - Pointer to hold the pillbox number
*  perform - If perform is FALSE then only test if this
*            will work. Don't actually deduct
*********************************************************/
bool tankGetCarriedPill(tank *value, BYTE *pillNum, bool perform) {
  bool returnValue; /* Value to return */
  tankCarryPb q;    /* temp pointer */

  returnValue = FALSE;
  if (!IsEmpty((*value)->carryPills)) {
    if (perform == TRUE) {
      q = (*value)->carryPills;
      (*value)->carryPills = TankPillsTail(q);
      *pillNum = q->pillNum;
      Dispose(q);
    }
    returnValue = TRUE;
  }
  return returnValue;
}

void tankGetCarriedPillNum(tank *value, BYTE pillNum) {
  tankCarryPb q;
  tankCarryPb prev;
  bool done;
  done = FALSE;
  prev = NULL;
  q = (*value)->carryPills;
  while (!IsEmpty(q) && done == FALSE) {
    if (q->pillNum == pillNum) {
      done = TRUE;
      if (prev == NULL) {
        (*value)->carryPills = q->next;
      } else {
        prev->next = q->next;
      }
      Dispose(q);
    } else {
      prev = q;
      q = q->next;
    }
  }
}
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
void tankPutCarriedPill(tank *value, BYTE pillNum) {
  tankCarryPb q;  /* Temp pointer for adding PBs to tank */

  New(q);
  q->pillNum = pillNum;
  q->next = (*value)->carryPills;
  (*value)->carryPills = q;
}


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
void tankStopCarryingPill(tank *value, BYTE pillNum) {
  bool done;        /* Have we done looping */
  tankCarryPb q;    /* temp pointer */
  tankCarryPb prev; /* temp pointer */

  done = FALSE;
  pillNum++;
  if (*value != NULL) {
    if (!IsEmpty((*value)->carryPills)) {
      prev = NULL;
      q = (*value)->carryPills;
      while (NonEmpty(q) && done == FALSE) {
        if (q->pillNum == pillNum) {
          /* We are carrying it */
          done = TRUE;
          if (prev == NULL) {
            (*value)->carryPills = q->next;
          } else {
            prev->next = q->next;
          }
          Dispose(q);
        } else {
          prev = q;
          q = q->next;
        }
      }
    }
  }
}

/*********************************************************
*NAME:          tankLayMine
*AUTHOR:        John Morrison
*CREATION DATE: 21/1/99
*LAST MODIFIED: 19/1/00
*PURPOSE:
* Tank has been requested to lay a mine
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*  mp    - Pointer to the map strructure
*  pb    - Pointer to the pillboxes structure
*  bs    - Pointer to the bases structure
*********************************************************/
void tankLayMine(tank *value, map *mp, pillboxes *pb, bases *bs) {
  WORLD conv;   /* Used in the W->M conversion */
  BYTE bmx;     /* Tank X and Y Co-ordintes */
  BYTE bmy;
  BYTE terrain; /* The current terrain */

  conv = (*value)->x;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmx = (BYTE) conv;
  conv = (*value)->y;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmy = (BYTE) conv;

  if (mapIsMine(mp, bmx, bmy) == FALSE) {
    terrain = mapGetPos(mp, bmx, bmy);

    if (terrain != BUILDING && terrain != HALFBUILDING && terrain != BOAT && terrain != RIVER && terrain < MINE_START && (*value)->mines > 0 && (*value)->onBoat == FALSE && pillsExistPos(pb, bmx, bmy) == FALSE && basesExistPos(bs, bmx, bmy) == FALSE && (*value)->armour <= TANK_FULL_ARMOUR) {
      (*value)->mines--;
      tankRegisterChangeByte(value, CRC_MINES_OFFSET, (*value)->mines);
      mapSetPos(mp, bmx, bmy, (BYTE) (terrain + MINE_SUBTRACT), FALSE, FALSE);
      netMNTAdd(screenGetNetMnt(), NMNT_MINETANKPLACE, 0, screenGetTankPlayer(value), bmx, bmy);
      soundDist(manLayingMineNear, bmx, bmy);
      if ((*value)->armour <= TANK_FULL_ARMOUR) {
        if (threadsGetContext() == FALSE) {
          frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
        }
      } else {
        if (threadsGetContext() == FALSE) {
          frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, 0, (*value)->trees);
        }
      }
      screenReCalc();
    }
  }
}

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
void tankMineDamage(tank *value, map *mp, pillboxes *pb, bases *bs, BYTE mx, BYTE my) {
  WORLD mineX; /* Mine X and Y World Co-ords */
  WORLD mineY;
  WORLD diffY; /* Difference beteween tank and mine */
  WORLD diffX; 
 
  mineX =(mx << TANK_SHIFT_MAPSIZE) + MAP_SQUARE_MIDDLE;
  mineY =(my << TANK_SHIFT_MAPSIZE) + MAP_SQUARE_MIDDLE;

  if ((*value)->x > mineX) {
    diffX = (*value)->x - mineX;
  } else {
    diffX = mineX - (*value)->x;
  }

  if ((*value)->y > mineY) {
    diffY = (*value)->y - mineY;
  } else {
    diffY = mineY - (*value)->y;
  }
  
  
  if (diffX < 384 && diffY < 384 && (*value)->armour <= TANK_FULL_ARMOUR) {
    (*value)->armour -= MINE_DAMAGE;
    tankRegisterChangeByte(value, CRC_ARMOUR_OFFSET, (*value)->armour);
    netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 2, (*value)->armour);
    if ((*value)->armour > TANK_FULL_ARMOUR) {
      if (threadsGetContext() == FALSE) {
        netMNTAdd(screenGetNetMnt(), NMNT_KILLME, 0, screenGetTankPlayer(value), 0xFF, 0xFF);
      }
      if (((*value)->shells + (*value)->mines) > TANK_BIG_EXPLOSION_THRESHOLD) {
        tkExplosionAddItem(screenGetTankExplosions(), (*value)->x, (*value)->y, (TURNTYPE) ((*value)->angle), (BYTE) ((*value)->speed), (BYTE) TH_KILL_BIG);
      } else {
        tkExplosionAddItem(screenGetTankExplosions(), (*value)->x, (*value)->y, (TURNTYPE) ((*value)->angle), (BYTE) ((*value)->speed), (BYTE) TH_KILL_SMALL);
      }
      (*value)->deathWait = 255;
      tankRegisterChangeByte(value, CRC_DEATHWAIT_OFFSET, 255);
      tankDropPills(value, mp, pb, bs);
    }
    if ((*value)->onBoat == TRUE) {
      (*value)->onBoat = FALSE;
      tankRegisterChangeByte(value, CRC_ONBOAT_OFFSET, FALSE);
      (*value)->speed = 0;
      tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, 0);
    }
    if ((*value)->armour <= TANK_FULL_ARMOUR) {
      if (threadsGetContext() == FALSE) {
        frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, (*value)->armour, (*value)->trees);
      }
    } else {
      if (threadsGetContext() == FALSE) {
        frontEndUpdateTankStatusBars((*value)->shells, (*value)->mines, 0, (*value)->trees);
      }
    }

    screenReCalc();
  }
}

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
void tankNearMines(map *mp, BYTE mx, BYTE my) {
  bool needRecalc; /* Is a screen recalc required */

  needRecalc = FALSE;
  if (mapIsMine(mp, mx, my) == TRUE) {
    if ((minesAddItem(screenGetMines(), mx, my)) == FALSE) {
      needRecalc = TRUE;
    }
  }
  if (mapIsMine(mp, (BYTE) (mx-1), (BYTE) (my-1)) == TRUE) {
    if ((minesAddItem(screenGetMines(), (BYTE) (mx-1), (BYTE) (my-1))) == FALSE) {
      needRecalc = TRUE;
    }
  }
  if (mapIsMine(mp, (BYTE) (mx-1), my) == TRUE) {
    if ((minesAddItem(screenGetMines(), (BYTE) (mx-1), my)) == FALSE) {
      needRecalc = TRUE;
    }
  }
  if (mapIsMine(mp, (BYTE) (mx-1), (BYTE) (my+1)) == TRUE) {
    if ((minesAddItem(screenGetMines(), (BYTE) (mx-1), (BYTE) (my+1))) == FALSE) {
      needRecalc = TRUE;
    }
  }
  if (mapIsMine(mp, mx, (BYTE) (my-1)) == TRUE) {
    if ((minesAddItem(screenGetMines(), mx, (BYTE) (my-1))) == FALSE) {
      needRecalc = TRUE;
    }
  }
  if (mapIsMine(mp, mx, (BYTE) (my+1)) == TRUE) {
    if ((minesAddItem(screenGetMines(), mx, (BYTE) (my+1))) == FALSE) {
      needRecalc = TRUE;
    }
  }
  if (mapIsMine(mp, (BYTE) (mx+1), (BYTE) (my-1)) == TRUE) {
    if ((minesAddItem(screenGetMines(), (BYTE) (mx+1), (BYTE) (my-1))) == FALSE) {
      needRecalc = TRUE;
    }
  }
  if (mapIsMine(mp, (BYTE) (mx+1), my) == TRUE) {
    if ((minesAddItem(screenGetMines(), (BYTE) (mx+1), my)) == FALSE) {
      needRecalc = TRUE;
    }
  }
  if (mapIsMine(mp, (BYTE) (mx+1), (BYTE) (my+1)) == TRUE) {
    if ((minesAddItem(screenGetMines(), (BYTE) (mx+1), (BYTE) (my+1))) == FALSE) {
      needRecalc = TRUE;
    }
  }

  if (needRecalc == TRUE) {
    screenReCalc();
  }
}

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
void tankCheckGroundClear(tank *value, map *mp, pillboxes *pb, bases *bs) {
  static BYTE modAmount = 1;
  WORLD conv;   /* Used for conversions */
  BYTE bmx;     /* Tank map offsets */
  BYTE bmy;
  BYTE px;      /* Tank pixel offsets */
  BYTE py;
  BYTE terrain; /* The terrain we are on */
  bool needFix; /* True if location is obstructed and we must move the tank */
  int leftPos;     /* Used in checking for tank collisions */
  int downPos;

  needFix = FALSE;
  conv = (*value)->x;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmx = (BYTE) conv;
  conv = (*value)->y;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmy = (BYTE) conv;

  /* Pill check */
  if ((pillsExistPos(pb, bmx, bmy)) == TRUE) {
    /* Check to make sure its not dead */
    if ((pillsDeadPos(pb, bmx, bmy)) == FALSE) {
      needFix = TRUE;
    }
  }
  
  /* Base check */
  if ((basesExistPos(bs, bmx, bmy)) == TRUE) {
    /* Check to make sure its not allied to us */
    terrain = basesGetOwnerPos(bs, bmx, bmy);
    if (playersIsAllie(screenGetPlayers(), terrain, screenGetTankPlayer(value)) == FALSE && terrain != NEUTRAL) {
      (*value)->obstructed = TRUE;
      tankRegisterChangeByte(value, CRC_OBSTRUCTED_OFFSET, TRUE);
      needFix = TRUE;
    } else {
      return;
    }
  }

  /* Terrain check */
  if (needFix == FALSE) {
    terrain = mapGetPos(mp, bmx, bmy);
    if (terrain == BUILDING || terrain == HALFBUILDING) {
      needFix = TRUE;
      (*value)->obstructed = TRUE;
      tankRegisterChangeByte(value, CRC_OBSTRUCTED_OFFSET, TRUE);
    }
  }

  /* Fix if required */
  if (needFix == TRUE) {
    (*value)->obstructed = TRUE;
    tankRegisterChangeByte(value, CRC_OBSTRUCTED_OFFSET, TRUE);
    conv = (*value)->x;
    conv <<= TANK_SHIFT_MAPSIZE;
    conv >>= TANK_SHIFT_PIXELSIZE;
    px = (BYTE) conv;
    conv = (*value)->y;
    conv <<= TANK_SHIFT_MAPSIZE;
    conv >>= TANK_SHIFT_PIXELSIZE;
    py = (BYTE) conv;
    if (px >= MIDDLE_PIXEL) {
      (*value)->x += 1;
    } else {
      (*value)->x -= 1;
    }
    tankRegisterChangeWorld(value, CRC_WORLDX_OFFSET, (*value)->x);
    if (py >= MIDDLE_PIXEL) {
      (*value)->y += 1;
    } else {
      (*value)->y -= 1;
    }
    tankRegisterChangeWorld(value, CRC_WORLDY_OFFSET, (*value)->y);
    (*value)->speed--;
    if ((*value)->speed < 0) {
      (*value)->speed = 0;
    }
    tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, (*value)->speed);
  }
  /* Check for tanks */
  if (needFix == FALSE && playersCheckCollision(screenGetPlayers(), screenGetTankPlayer(value), (*value)->x, (*value)->y, &leftPos, &downPos) == TRUE) { //&& threadsGetContext() == FALSE 
    (*value)->obstructed = TRUE;
    tankRegisterChangeByte(value, CRC_OBSTRUCTED_OFFSET, TRUE);
    (*value)->speed =0;
    tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, (*value)->speed);
    if (leftPos > 0) {
      (*value)->x += 8;
      tankRegisterChangeWorld(value, CRC_WORLDX_OFFSET, (*value)->x);
    } else if (leftPos < 0) {
      (*value)->x -= 8;
      tankRegisterChangeWorld(value, CRC_WORLDX_OFFSET, (*value)->x);
    } else {
      int rnd = rand() % 6;
//      srand((unsigned int) (rand() * time(NULL)));
      if (rnd < 3 ) {
        (*value)->x = (WORLD) ((*value)->x + rand() % modAmount);
      } else {
        (*value)->x = (WORLD) ((*value)->x - rand() % modAmount);
      }
      tankRegisterChangeWorld(value, CRC_WORLDX_OFFSET, (*value)->x);
      modAmount++;
    }
    if (downPos > 0) {
      (*value)->y = (WORLD) ((*value)->y - rand() % modAmount);
    } else if (downPos < 0) {
      (*value)->y = (WORLD) ((*value)->y + rand() % modAmount);
    } else {
      int rnd = rand() % 6;
//      srand((unsigned int) (rand() * time(NULL)));
      if (rnd < 3 ) {
        (*value)->y = (WORLD) ((*value)->y + rand() % modAmount);
      } else {
        (*value)->y = (WORLD) ((*value)->y - rand() % modAmount);
      }
      modAmount++;
    }
    tankRegisterChangeWorld(value, CRC_WORLDY_OFFSET, (*value)->y);
  } else {
    modAmount = 1;
  }

}

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
void tankAddKill(tank *value) {
  (*value)->numKills++;
  tankRegisterChangeInt(value, CRC_NUMKILLS_OFFSET, (*value)->numKills);
  if (threadsGetContext() == FALSE) {
    frontEndKillsDeaths((*value)->numKills, (*value)->numDeaths);
  }
}


/*********************************************************
*NAME:          tankSetLocationData
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED: 20/3/99
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
void tankSetLocationData(tank *value, WORLD wx, WORLD wy, TURNTYPE tt, SPEEDTYPE speed, bool onBoat) {
  (*value)->x = wx;
  tankRegisterChangeWorld(value, CRC_WORLDX_OFFSET, wx);
  (*value)->y = wy;
  tankRegisterChangeWorld(value, CRC_WORLDY_OFFSET, wy);
  (*value)->angle = tt;
  tankRegisterChangeFloat(value, CRC_ANGLE_OFFSET, tt);
  (*value)->speed = speed;
  tankRegisterChangeFloat(value, CRC_SPEED_OFFSET, speed);
  (*value)->onBoat = onBoat;
  tankRegisterChangeByte(value, CRC_ONBOAT_OFFSET, onBoat);
}


/*********************************************************
*NAME:          tankGetNumCarriedPills
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 17/1/99
*PURPOSE:
* Returns the number of carried pills
*
*ARGUMENTS:
*  value   - Pointer to the tank structure
*********************************************************/
BYTE tankGetNumCarriedPills(tank *value) {
  BYTE returnValue; /* Value to return */
  tankCarryPb q;    /* temp pointer */

  q = (*value)->carryPills;
  returnValue =0;
  while (NonEmpty(q)) {
    returnValue++;
    q = TankPillsTail(q);
  }
 
  return returnValue;
}

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
BYTE tankGetGunsightLength(tank *value) {
  return (*value)->sightLen;
}

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
BYTE tankGetReloadTime(tank *value) {
  return (*value)->reload;
}

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
bool tankIsObstructed(tank *value) {
  return (*value)->obstructed;
}

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
bool tankIsNewTank(tank *value) {
  return (*value)->newTank;
}

/*********************************************************
*NAME:          tankGetAutoSlowdown
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/00
*LAST MODIFIED: 4/1/0
*PURPOSE:
*  Returns whether tank autoslowdown is enabled or not
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
bool tankGetAutoSlowdown(tank *value) {
  return (*value)->autoSlowdown;
}

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
void tankSetAutoSlowdown(tank *value, bool useSlowdown) {
  if ((*value) != NULL) {
    (*value)->autoSlowdown = useSlowdown;
    tankRegisterChangeByte(value, CRC_AUTOSLOWDOWN_OFFSET, useSlowdown);
  }
}

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
bool tankGetAutoHideGunsight(tank *value) {
  return (*value)->autoHideGunsight;
}

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
void tankSetAutoHideGunsight(tank *value, bool useAutohide) {
  if ((*value) != NULL) {
    (*value)->autoHideGunsight = useAutohide;
    tankRegisterChangeByte(value, CRC_AUTOHIDE_OFFSET, useAutohide);
  }
}

/*********************************************************
*NAME:          tankJustFired
*AUTHOR:        John Morrison
*CREATION DATE: 24/5/00
*LAST MODIFIED: 24/5/00
*PURPOSE:
*  Returns if the tank just fired or not
*
*ARGUMENTS:
*  value - Pointer to the tank structure
*********************************************************/
bool tankJustFired(tank *value) {
  return (*value)->justFired;
}

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
BYTE tankGetShells(tank *value) {
  return (*value)->shells;
}

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
BYTE tankGetMines(tank *value) {
  return (*value)->mines;
}

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
BYTE tankGetTrees(tank *value) {
  return (*value)->trees;
}

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
void tankSetShells(tank *value, BYTE amount) {
  if (amount <= TANK_FULL_SHELLS) {
    (*value)->shells = amount;
  } else {
    (*value)->shells = TANK_FULL_SHELLS;
  }
  tankRegisterChangeByte(value, CRC_SHELLS_OFFSET, (*value)->shells);
}

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
void tankSetArmour(tank *value, BYTE amount) {
  (*value)->armour = amount;
  tankRegisterChangeByte(value, CRC_ARMOUR_OFFSET, amount);
}

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
void tankSetMines(tank *value, BYTE amount) {
  if (amount <= TANK_FULL_MINES) {
    (*value)->mines = amount;
  } else {
    (*value)->mines = TANK_FULL_MINES;
  }
  tankRegisterChangeByte(value, CRC_MINES_OFFSET, (*value)->mines);
}

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
void tankSetTrees(tank *value, BYTE amount) {
  if (amount <= TANK_FULL_TREES) {
    (*value)->trees = amount;
  } else {
    (*value)->trees = TANK_FULL_TREES;
  }
  tankRegisterChangeByte(value, CRC_TREES_OFFSET, (*value)->trees);
}


void tankPutPill(tank *value, pillboxes *pb, BYTE pillNum) {
  tankCarryPb q;  /* Temp pointer for adding PBs to tank */

  if (threadsGetContext() == FALSE) {
    frontEndStatusPillbox(pillNum, (pillsGetAllianceNum(pb, pillNum)));
  }
  New(q);
  q->pillNum = pillNum;
  q->next = (*value)->carryPills;
  (*value)->carryPills = q;
}

void tankResetHitCount(tank *value) {
  (*value)->tankHitCount = 0;
}

void tankAddHit(tank *value, int amount) {
  if ((*value)->tankHitCount == 0 && amount < 0) {
    return;
  }

  if (amount> -2 || amount < 3) {
    (*value)->tankHitCount += amount;
    if (((*value)->tankHitCount > 10 && amount > 1) || ((*value)->tankHitCount > 9 && amount == 1)) {
      /* Cheater */
//      printf("Cheat received form of armour!...\n");
      netPlayersSetCheater((netPlayers *) serverNetGetNetPlayers(), (BYTE) serverCoreGetTankPlayer(value));
    }
  } else {
    /* Cheater? */
    /* Do nothing for now */

  }
}

int tankCalcCRCSetup(tank *value) {
  memcpy(ct, *value, CRC_TANK_SIZE);
  return CRCCalc((BYTE *) ct, CRC_TANK_SIZE);
}

void tankRegisterChangeFloat(tank *value, int offset, float newValue) {
  float tempValue;
  int crc;
  BYTE *original = (BYTE *) *value;

  if (threadsGetContext() == FALSE && netGetType() != netSingle) {
    memcpy(&tempValue, (ct+offset), sizeof(float));
    memcpy(ct, *value, CRC_TANK_SIZE);
    memcpy(ct+offset, &tempValue, sizeof(float));
    crc = CRCCalc((BYTE *) ct, CRC_TANK_SIZE);
    if (crc != (*value)->crc) {
      netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 0xFF , 0xFF);
    }

    memcpy(ct+offset, &newValue, sizeof(float));
    crc = CRCCalc((BYTE *) *value, CRC_TANK_SIZE);
    (*value)->crc = crc;
    memcpy(&tempValue, (original+offset), sizeof(float));
    if (newValue != tempValue) {
      netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 0xFF , 0xFF);
    }
  }
}

void tankRegisterChangeWorld(tank *value, int offset, WORLD newValue) {
  WORLD tempValue;
  int crc = sizeof(WORLD);
  BYTE *original = (BYTE *) *value;

  if (threadsGetContext() == FALSE && netGetType() != netSingle) {
    memcpy(&tempValue, (ct+offset), sizeof(WORLD));
    memcpy(ct, *value, CRC_TANK_SIZE);
    memcpy(ct+offset, &tempValue, sizeof(WORLD));
    crc = CRCCalc((BYTE *) ct, CRC_TANK_SIZE);
    if (crc != (*value)->crc) {
      netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 0xFF , 0xFF);
    }

    memcpy(ct+offset, &newValue, sizeof(WORLD));
    crc = CRCCalc((BYTE *) *value, CRC_TANK_SIZE);
    (*value)->crc = crc;
    memcpy(&tempValue, original+offset, sizeof(WORLD));
    if (newValue != tempValue) {
      netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 0xFF , 0xFF);
    }
  }
}

void tankRegisterChangeInt(tank *value, int offset, int newValue) {
  int tempValue;
  int crc;
  BYTE *original = (BYTE *) *value;

  if (threadsGetContext() == FALSE && netGetType() != netSingle) {
    memcpy(&tempValue, (ct+offset), sizeof(int));
    memcpy(ct, *value, CRC_TANK_SIZE);
    memcpy(ct+offset, &tempValue, sizeof(int));
    crc = CRCCalc((BYTE *) ct, CRC_TANK_SIZE);
    if (crc != (*value)->crc) {
      netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 0xFF , 0xFF);
    }

    memcpy(ct+offset, &newValue, sizeof(int));
    crc = CRCCalc((BYTE *) *value, CRC_TANK_SIZE);
    (*value)->crc = crc;
    memcpy(&tempValue, original+offset, sizeof(int));
    if (newValue != tempValue) {
      netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 0xFF , 0xFF);
    }
  }
}

void tankRegisterChangeByte(tank *value, int offset, BYTE newValue) {
  BYTE tempValue;
  int crc = sizeof(BYTE);
  BYTE *original = (BYTE *) *value;
  

  if (threadsGetContext() == FALSE && netGetType() != netSingle) {
    //memcpy(&tempValue, (ct+offset), sizeof(BYTE));
    tempValue = *(ct+offset);
    memcpy(ct, *value, CRC_TANK_SIZE);
    
    //memcpy(ct+offset, &tempValue, sizeof(BYTE));
    *(ct+offset) = tempValue;
    crc = CRCCalc((BYTE *) ct, CRC_TANK_SIZE);
    if (crc != (*value)->crc) {
      netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 0xFF , 0xFF);
    }

    *(ct+offset) = newValue;
    crc = CRCCalc((BYTE *) *value, CRC_TANK_SIZE);
    (*value)->crc = crc;
    memcpy(&tempValue, original+offset, sizeof(BYTE));
    if (newValue != tempValue) {
      netMNTAdd(screenGetNetMnt(), NMNT_TANKHIT, playersGetSelf(screenGetPlayers()), playersGetSelf(screenGetPlayers()), 0xFF , 0xFF);
    }
  }
}


/*********************************************************
*NAME:          tankSetOnBoat
*AUTHOR:        John Morrison
*CREATION DATE: 21/8/00
*LAST MODIFIED: 21/8/00
*PURPOSE:
*  Sets the number of trees in tank
*
*ARGUMENTS:
*  value  - Pointer to the tank structure
*  onBoat - On Boat value to set
*********************************************************/
void tankSetOnBoat(tank *value, bool onBoat) {
  (*value)->onBoat = onBoat;
}
