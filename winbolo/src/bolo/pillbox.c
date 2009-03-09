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
*Filename:      pillbox.c
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 11/02/09
*Purpose:
*  Provides operations on pillbox and pillboxes
*********************************************************/

/* Includes */
#include <math.h>
#include <memory.h>
#include "global.h"
#include "tank.h"
#include "tilenum.h"
#include "frontend.h"
#include "sounddist.h"
#include "screen.h"
#include "messages.h"
#include "players.h"
#include "log.h"
#include "netpnb.h"
#include "netmt.h"
#include "pillbox.h"
#include "../winbolonet/winbolonet.h"

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
void pillsCreate(pillboxes *value) {
  BYTE count; /* Looping variable */

  New(*value);
  memset(*value, 0, sizeof(**value));
  ((*value)->numPills) = 0;

  for(count=0;count<MAX_PILLS;count++) {
    (*value)->item[count].speed = PILLBOX_ATTACK_NORMAL;
    (*value)->item[count].reload = PILLBOX_ATTACK_NORMAL;
    (*value)->item[count].coolDown = 0;
    (*value)->item[count].inTank = FALSE;
    (*value)->item[count].justSeen = FALSE;
  }
}


/*********************************************************
*NAME:          pillsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 11/12/98
*PURPOSE:
*  Destroys the pills data structure. Also frees memory.
*
*ARGUMENTS:
*  value - Pointer to the pills structure
*********************************************************/
void pillsDestroy(pillboxes *value) {
  if (*value != NULL) {
    Dispose(*value);
  }
}

/*********************************************************
*NAME:          pillsSetNumPills
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets a specific pill with its item data
*
*ARGUMENTS:
*  value    - Pointer to the pillbox structure
*  numPills - The number of pills
*********************************************************/
void pillsSetNumPills(pillboxes *value, BYTE numPills) {
  if (numPills > 0 && numPills <= MAX_PILLS) {
    (*value)->numPills = numPills;
  }
}

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
BYTE pillsGetNumPills(pillboxes *value) {
  if ((*value) != NULL) {
    return (*value)->numPills;
  }
  return 0;
}

/*********************************************************
*NAME:          pillsSetPill
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED:  29/4/00
*PURPOSE:
*  Sets a specific pill with its item data
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  item    - Pointer to a pillbox
*  pillNum - The pillbox number
*********************************************************/
void pillsSetPill(pillboxes *value, pillbox *item, BYTE pillNum) {
  if (pillNum > 0 && pillNum  <= (*value)->numPills) {
    pillNum--;
    (((*value)->item[pillNum]).x) = item->x;
    (((*value)->item[pillNum]).y) = item->y;
    if (item->owner > (MAX_TANKS-1) && item->owner != NEUTRAL) {
      item->owner = NEUTRAL;
    }
    (((*value)->item[pillNum]).owner) = item->owner;
    (((*value)->item[pillNum]).armour) = item->armour;


    if (item->speed != PILLBOX_ATTACK_NORMAL) {
      (*value)->item[pillNum].coolDown = PILLBOX_COOLDOWN_TIME;
    }
    (((*value)->item[pillNum]).speed) = item->speed;
    (((*value)->item[pillNum]).inTank) = item->inTank;
    (((*value)->item[pillNum]).justSeen) = item->justSeen;
    logAddEvent(log_PillSetOwner, pillNum, item->owner, TRUE, 0, 0, NULL);
    logAddEvent(log_PillSetHealth, utilPutNibble(pillNum, item->armour), 0, 0, 0, 0, NULL);
    logAddEvent(log_PillSetInTank, utilPutNibble(pillNum, FALSE), 0, 0, 0, 0, NULL);
    logAddEvent(log_PillSetPlace, pillNum, item->x, item->y, 0, 0, NULL);
  }
}


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
void pillsGetPill(pillboxes *value, pillbox *item, BYTE pillNum) {
  if (pillNum > 0 && pillNum  <= (*value)->numPills) {
    pillNum--;
    item->x = ((*value)->item[pillNum]).x;
    item->y = ((*value)->item[pillNum]).y;
    item->owner = ((*value)->item[pillNum]).owner;
    item->armour = ((*value)->item[pillNum]).armour;
    item->speed = ((*value)->item[pillNum]).speed;
  }
}


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
bool pillsExistPos(pillboxes *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  returnValue = FALSE;
  count = 0;
  while (returnValue == FALSE && count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && ((*value)->item[count].inTank) == FALSE) {
      returnValue = TRUE;
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          pillsGetAllianceNum
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 16/2/99
*PURPOSE:
*  Returns the alliance type of a pillbox for drawing
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  pillNum - Pillbox Number to get 
*********************************************************/
pillAlliance pillsGetAllianceNum(pillboxes *value, BYTE pillNum) {
  pillAlliance returnValue; /* Value to return */

  returnValue = pillNeutral;
  pillNum--;
  if ((*value) != NULL) {
    if ((pillNum) <= ((*value)->numPills)) {
      if ((*value)->item[pillNum].armour == 0 && (*value)->item[pillNum].inTank == FALSE) {
        returnValue = pillDead;
      } else if ((*value)->item[pillNum].owner == playersGetSelf(screenGetPlayers())) {
        if ((*value)->item[pillNum].inTank == TRUE) {
          returnValue = pillTankGood;
        } else {
          returnValue = pillGood;
        } 
      } else if (playersIsAllie(screenGetPlayers(), (*value)->item[pillNum].owner, playersGetSelf(screenGetPlayers())) == TRUE) {
        if ((*value)->item[pillNum].inTank == TRUE) {
          returnValue = pillTankAllie;
        } else {
          returnValue = pillAllie;
        } 
      } else if ((*value)->item[pillNum].owner != NEUTRAL) {
        if ((*value)->item[pillNum].inTank == TRUE) {
          returnValue = pillTankEvil;
        } else {
          returnValue = pillEvil;
        } 
      }
    }
  }
  return returnValue;
}

/*********************************************************
*NAME:          pillsUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 30/10/99
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
void pillsUpdate(pillboxes *value, map *mp, bases *bs, tank *tnk, shells *shs) {
  WORLD x;         /* X and Y co-ords of the pillbox */
  WORLD y;
  WORLD tankX;     /* X and Y co-ords of the tank */
  WORLD tankY;
  WORLD diffX;     /* X and Y differences in distances */
  WORLD diffY;
  BYTE tankDir;    /* Tank direction 1-16 */
  BYTE tankSpeed;  /* Speed of the tank */
  BYTE tankArmour; /* Speed of the tank */
  TURNTYPE dir;    /* Direction Pillbox should fire */
  BYTE count;      /* Looping variable */
  double amount;   /* Distance of player away from pill */

  
  tankX = 0;
  tankY = 0;
  tankArmour = 0;
  tankDir = 0;
  tankSpeed = 0;

  /* Get the tank info */
  if (threadsGetContext() == FALSE) {
    tankGetWorld(tnk, &tankX, &tankY);
    tankDir = tankGetTravelAngel(tnk);
    tankSpeed = tankGetSpeed(tnk);
    tankArmour = tankGetArmour(tnk);
  }


  for (count=0;count<(*value)->numPills;count++) {
    /* Set world Co-ords for the Pillbox */
    x = (*value)->item[count].x;
    x <<= TANK_SHIFT_MAPSIZE;
    x += MAP_SQUARE_MIDDLE;
    y = (*value)->item[count].y;
    y <<= TANK_SHIFT_MAPSIZE;
    y += MAP_SQUARE_MIDDLE;
    
    /* Update the reload time */
    if ((*value)->item[count].reload < (*value)->item[count].speed) {
      (*value)->item[count].reload++;
    }
    /* Adjust the cool down time */
    if ((*value)->item[count].coolDown > 0) {
      (*value)->item[count].coolDown--;
      if ((*value)->item[count].coolDown ==0) {
        (*value)->item[count].speed++;
        if ((*value)->item[count].speed < PILLBOX_ATTACK_NORMAL) {
          (*value)->item[count].coolDown = PILLBOX_COOLDOWN_TIME;
        }
      }
    }
    /* Check to see if it should fire ie Is alive, not allied to the tank and not reloading */
    if (threadsGetContext() == FALSE) {
      if ((*value)->item[count].armour > 0 && (playersIsAllie(screenGetPlayers(), (*value)->item[count].owner, playersGetSelf(screenGetPlayers())) == FALSE) && (*value)->item[count].reload >= (*value)->item[count].speed && (*value)->item[count].inTank == FALSE) {
        /* Check to see if tank is in range */
        if (tankX > ((((*value)->item[count].x) << TANK_SHIFT_MAPSIZE) + MAP_SQUARE_MIDDLE)) {
          diffX = tankX - ((((*value)->item[count].x) << TANK_SHIFT_MAPSIZE) + MAP_SQUARE_MIDDLE);
        } else {
          diffX = ((((*value)->item[count].x) << TANK_SHIFT_MAPSIZE) + MAP_SQUARE_MIDDLE) - tankX;
        }
        if (tankY > ((((*value)->item[count].y) << TANK_SHIFT_MAPSIZE) + MAP_SQUARE_MIDDLE)) {
          diffY = tankY - ((((*value)->item[count].y) << TANK_SHIFT_MAPSIZE) + MAP_SQUARE_MIDDLE);
        } else {
          diffY = ((((*value)->item[count].y) << TANK_SHIFT_MAPSIZE) + MAP_SQUARE_MIDDLE) - tankY;
        }

        if (tankArmour <= TANK_FULL_ARMOUR && ((utilIsTankInTrees(mp, value, bs, tankX, tankY)) == FALSE || (diffX < MIN_TREEHIDE_DIST && diffY < MIN_TREEHIDE_DIST) || tankJustFired(tnk) == TRUE)) {
          if ((utilIsItemInRange(x, y, tankX, tankY, PILLBOX_RANGE, &amount)) == TRUE) {
          /* Check for closer tanks here */
            if (playersIsTankCloser(screenGetPlayers(), x, y, (*value)->item[count].owner, amount) == FALSE) {
              /* Fire at player */
              if ((*value)->item[count].justSeen == TRUE) {
                dir = pillsTargetTank(mp, value, bs, x, y, tankX, tankY, (TURNTYPE) tankDir, tankSpeed, (tankIsOnBoat(tnk)));
                shellsAddItem(shs, x, y, dir, (float) (PILLBOX_FIRE_DISTANCE), NEUTRAL, FALSE);
                (*value)->item[count].reload = 0;
                soundDist(shootNear, (*value)->item[count].x, (*value)->item[count].y);
              } else {
                /* They were just seen */
                (*value)->item[count].justSeen = TRUE;
                (*value)->item[count].reload = 0;
              }
            }
          } else {
            (*value)->item[count].justSeen = FALSE;
          }
        }
      }
    }
  }
}

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
bool pillsIsPillHit(pillboxes *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  returnValue = FALSE;
  count = 0;
  while (returnValue == FALSE && count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && ((*value)->item[count].armour >0) && (*value)->item[count].inTank == FALSE) {
      /* Pillbox has been Hit */
      returnValue = TRUE;
    }
    count++;
  }

  return returnValue;
}

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
bool pillsDamagePos(pillboxes *value, BYTE xValue, BYTE yValue, bool wantDamage, bool wantAngry) {
  bool returnValue;  /* Value to return */
  bool done;         /* Loop guard */
  BYTE count;        /* Looping Variable */

  returnValue = FALSE;
  done = FALSE;
  count = 0;
  while (done == FALSE && count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && ((*value)->item[count].armour >0) && (*value)->item[count].inTank == FALSE) {
      /* Pillbox has been Hit */
      done = TRUE;
      if (wantDamage == TRUE && (*value)->item[count].armour > 0) {
        (*value)->item[count].armour--;
      }
      logAddEvent(log_PillSetHealth, utilPutNibble(count, (*value)->item[count].armour), 0, 0, 0, 0, NULL);
      if ((*value)->item[count].armour == 0) {
        returnValue = TRUE;
        if (threadsGetContext() == FALSE) {
          frontEndStatusPillbox((BYTE) (count+1), pillDead);
        }
      } else if (wantDamage == TRUE) {
        (*value)->item[count].coolDown = PILLBOX_COOLDOWN_TIME;
        if ((*value)->item[count].speed > PILLBOX_MAX_FIRERATE) {
          (*value)->item[count].speed /=2;
          if ((*value)->item[count].speed < PILLBOX_MAX_FIRERATE) {
            (*value)->item[count].speed = PILLBOX_MAX_FIRERATE;
          }
        }
      }
    }
    count++;
  }

  return returnValue;
}

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
BYTE pillsGetScreenHealth(pillboxes *value, BYTE xValue, BYTE yValue) {
  bool done;        /* Finished searching */
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  done = FALSE;
  count = 0;
  returnValue = PILL_EVIL_15;

  while (done == FALSE && count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && (*value)->item[count].inTank == FALSE) {
      /* Pillbox has been Hit */
      done = TRUE;
      
      if (playersIsAllie(screenGetPlayers(), (*value)->item[count].owner, playersGetSelf(screenGetPlayers()) ) == FALSE) {
        switch((*value)->item[count].armour) {
        case PILLBOX_15:
          returnValue = PILL_EVIL_15;
          break;
        case PILLBOX_14:
          returnValue = PILL_EVIL_14;
          break;
        case PILLBOX_13:
          returnValue = PILL_EVIL_13;
          break;
        case PILLBOX_12:
          returnValue = PILL_EVIL_12;
          break;
        case PILLBOX_11:
          returnValue = PILL_EVIL_11;
          break;
        case PILLBOX_10:
          returnValue = PILL_EVIL_10;
          break;
        case PILLBOX_9:
          returnValue = PILL_EVIL_9;
          break;
        case PILLBOX_8:
          returnValue = PILL_EVIL_8;
          break;
        case PILLBOX_7:
          returnValue = PILL_EVIL_7;
          break;
        case PILLBOX_6:
          returnValue = PILL_EVIL_6;
          break;
        case PILLBOX_5:
          returnValue = PILL_EVIL_5;
          break;
        case PILLBOX_4:
          returnValue = PILL_EVIL_4;
          break;
        case PILLBOX_3:
          returnValue = PILL_EVIL_3;
          break;
        case PILLBOX_2:
          returnValue = PILL_EVIL_2;
          break;
        case PILLBOX_1:
          returnValue = PILL_EVIL_1;
          break;
        case PILLBOX_0:
        default:
          returnValue = PILL_EVIL_0;
          break;
        }
      } else {
        switch((*value)->item[count].armour) {
        case PILLBOX_15:
          returnValue = PILL_GOOD_15;
          break;
        case PILLBOX_14:
          returnValue = PILL_GOOD_14;
          break;
        case PILLBOX_13:
          returnValue = PILL_GOOD_13;
          break;
        case PILLBOX_12:
          returnValue = PILL_GOOD_12;
          break;
        case PILLBOX_11:
          returnValue = PILL_GOOD_11;
          break;
        case PILLBOX_10:
          returnValue = PILL_GOOD_10;
          break;
        case PILLBOX_9:
          returnValue = PILL_GOOD_9;
          break;
        case PILLBOX_8:
          returnValue = PILL_GOOD_8;
          break;
        case PILLBOX_7:
          returnValue = PILL_GOOD_7;
          break;
        case PILLBOX_6:
          returnValue = PILL_GOOD_6;
          break;
        case PILLBOX_5:
          returnValue = PILL_GOOD_5;
          break;
        case PILLBOX_4:
          returnValue = PILL_GOOD_4;
          break;
        case PILLBOX_3:
          returnValue = PILL_GOOD_3;
          break;
        case PILLBOX_2:
          returnValue = PILL_GOOD_2;
          break;
        case PILLBOX_1:
          returnValue = PILL_GOOD_1;
          break;
        case PILLBOX_0:
        default:
          returnValue = PILL_GOOD_0;
          break;
        }
      }

    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          pillsTargetTank
*AUTHOR:        John Morrison
*CREATION DATE:  1/12/98
*LAST MODIFIED: 17/1/99
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
TURNTYPE pillsTargetTank(map *mp, pillboxes *pb, bases *bs, WORLD xValue, WORLD yValue, WORLD tankX, WORLD tankY, TURNTYPE angle, BYTE speed, bool onBoat) {
  TURNTYPE returnValue; /* Value to return */

  if (speed == 0) {
    returnValue = utilCalcAngle(xValue, yValue, tankX, tankY);
  } else {
    returnValue = pillsTargetTankMove(mp, pb, bs, xValue, yValue, tankX, tankY, angle, speed, onBoat);
  }

  return returnValue;
}

/*********************************************************
*NAME:          pillsTargetTankMove
*AUTHOR:        John Morrison
*CREATION DATE: 31/12/98
*LAST MODIFIED: 11/02/09
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
TURNTYPE pillsTargetTankMove(map *mp, pillboxes *pb, bases *bs, WORLD xValue, WORLD yValue, WORLD tankX, WORLD tankY, TURNTYPE angle, BYTE speed, bool onBoat) {
  TURNTYPE returnValue; /* Value to return */
  TURNTYPE estimate;    /* Current Estimate */
  BYTE count;           /* Looping Variable */
  bool found;           /* Finished looping */
  int tankAddX;         /* Tank add X */
  int tankAddY;         /* Tank add Y */
  WORLD tankTestAddX;   /* Check for tank colisions */
  WORLD tankTestAddY;
  BYTE bmx;
  BYTE bmy;
  BYTE newbmx;
  BYTE newbmy;
  int shellAddX;        /* Tank add X */
  int shellAddY;        /* Tank add Y */
  WORLD shellX;         /* Shell X location */
  WORLD shellY;         /* Shell Y locatiopn */
  bool isLand;          /* Is this square land */

  found = FALSE;
  count = 1;
  shellX = xValue;
  shellY = yValue;

  returnValue = 0;

  /* Get tank Add amounts */
  utilCalcDistance(&tankAddX, &tankAddY, angle, speed);
    
  /* Get initial estimate */
  estimate = utilCalcAngle(xValue, yValue, tankX,tankY);
  /* Calculate distance */
  utilCalcDistance(&shellAddX, &shellAddY, estimate, SHELL_SPEED);
  shellX = (WORLD) (xValue + shellAddX);
  shellY = (WORLD) (yValue + shellAddY);
  
  while (found == FALSE && count < MAX_AIM_ITERATE) {
    if ((utilIsTankHit(tankX, tankY, angle, shellX, shellY, estimate)) == TRUE  ) {
      found = TRUE;
      returnValue = estimate;
    }
    count++;
    
    tankTestAddX = (WORLD) (tankX + tankAddX);
    tankTestAddY = (WORLD) (tankY + tankAddY);
    if (tankAddX >= 0) {
      tankTestAddX += TANK_SUBTRACT;
    } else {
      tankTestAddX -= TANK_SUBTRACT;
    }

    if (tankAddY >= 0) {
      tankTestAddY += TANK_SUBTRACT;
    } else {
      tankTestAddY -= TANK_SUBTRACT;
    }

    tankTestAddX >>= TANK_SHIFT_MAPSIZE;
    tankTestAddY >>= TANK_SHIFT_MAPSIZE;
    newbmx = (BYTE) tankTestAddX;
    newbmy = (BYTE) tankTestAddY;

    tankTestAddX = tankX;
    tankTestAddY = tankY;
    tankTestAddX >>= TANK_SHIFT_MAPSIZE;
    tankTestAddY >>= TANK_SHIFT_MAPSIZE;
    bmx = (BYTE) tankTestAddX;
    bmy = (BYTE) tankTestAddY;

    isLand = mapIsLand(mp, pb,bs, bmx, newbmy);
    if (mapGetSpeed(mp,pb,bs,bmx,newbmy, onBoat, playersGetSelf(screenGetPlayers())) > 0 && (onBoat == FALSE || (onBoat == TRUE && isLand == FALSE) || (onBoat == TRUE && isLand == TRUE && speed >= BOAT_EXIT_SPEED))) {
      tankY = (WORLD) (tankY + tankAddY);
    }

    isLand = mapIsLand(mp, pb,bs, newbmx, bmy);
    if (mapGetSpeed(mp,pb,bs,newbmx,bmy, onBoat, playersGetSelf(screenGetPlayers())) > 0 && (onBoat == FALSE || (onBoat == TRUE && isLand == FALSE) || (onBoat == TRUE && isLand == TRUE && speed >= BOAT_EXIT_SPEED))) {
      tankX = (WORLD) (tankX + tankAddX);
    }
    
    estimate = utilCalcAngle(xValue, yValue, tankX,tankY);
    utilCalcDistance(&shellAddX, &shellAddY, estimate, SHELL_SPEED);    
    shellX = (WORLD) (xValue + ((count+  5) * shellAddX));
    shellY = (WORLD) (yValue + ((count +  5) * shellAddY));
    /* Set it anyway */
    returnValue = estimate;
  }

  return returnValue;
} 

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
bool pillsDeadPos(pillboxes *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  returnValue = FALSE;
  count = 0;
  while (returnValue == FALSE && count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && ((*value)->item[count].armour == 0)) {
      returnValue = TRUE;
    }
    count++;
  }
  return returnValue;
}

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
BYTE pillsGetPillNum(pillboxes *value, BYTE xValue, BYTE yValue, bool careInTank, bool inTank) {
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  returnValue = PILL_NOT_FOUND-1;
  count = 0;
  while (count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
      if (careInTank == FALSE || ((*value)->item[count].inTank == inTank)) {
        returnValue = (BYTE) (count+1);
        count = (*value)->numPills;
      }
    }
    count++;
  }
  return returnValue;
}

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
void pillsSetPillInTank(pillboxes *value, BYTE pillNum, bool inTank) {
  if (pillNum > 0 && pillNum<= (*value)->numPills) {
    pillNum--;
    (*value)->item[pillNum].inTank = inTank;
    logAddEvent(log_PillSetInTank, utilPutNibble(pillNum, inTank), 0, 0, 0, 0, NULL);

  }
}

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
BYTE pillsGetPillOwner(pillboxes *value, BYTE pillNum) {
  BYTE returnValue; /* Value to return */

  returnValue = PILL_NOT_FOUND;
  if (pillNum > 0 && pillNum<= (*value)->numPills) {
    pillNum--;
    returnValue = (*value)->item[pillNum].owner;
  }
  return returnValue;
}

/*********************************************************
*NAME:          pillsSetPillOwner
*AUTHOR:        John Morrison
*CREATION DATE: 15/01/99
*LAST MODIFIED: 04/04/02
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
BYTE pillsSetPillOwner(pillboxes *value, BYTE pillNum, BYTE owner, bool migrate) {
  /* Message stuff */
  char oldOwner[FILENAME_MAX];
  char messageStr[FILENAME_MAX];
  BYTE returnValue; /* Value to return */
  
  oldOwner[0] = '\0';
  messageStr[0] = '\0';

  returnValue = NEUTRAL;
  if (pillNum > 0 && pillNum<= (*value)->numPills) {
    pillNum--;
    returnValue = (*value)->item[pillNum].owner;
    (*value)->item[pillNum].owner = owner;
    /* Make the message if required */
    if (returnValue == NEUTRAL && migrate == FALSE && owner != NEUTRAL) {
      /* Neutral pill */
      playersMakeMessageName(screenGetPlayers(), owner, messageStr);
      strcat(messageStr, langGetText(MESSAGE_CAPTURE_PILL));
      messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), messageStr);
    } else if (owner == NEUTRAL) {
      /* Do nothing */
    } else if (playersIsAllie(screenGetPlayers(), returnValue, owner) == FALSE && migrate == FALSE) {
      /* Stole pill */
      playersMakeMessageName(screenGetPlayers(), owner, messageStr);
      strcat(messageStr, langGetText(MESSAGE_STOLE_PILL));
      playersGetPlayerName(screenGetPlayers(), returnValue, oldOwner);
      strcat(messageStr, oldOwner);
      messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), messageStr);
    }
    (*value)->item[pillNum].owner = owner;
    logAddEvent(log_PillSetOwner, pillNum, owner, migrate, 0, 0, NULL);
    pillNum++;
    /* Winbolo.Net stuff */
    /* FIXME: Check if in an alliance?????? */
    if (migrate == FALSE && owner != NEUTRAL) {
      if (returnValue == NEUTRAL) {
        winbolonetAddEvent(WINBOLO_NET_EVENT_PILL_CAPTURE, threadsGetContext(), owner, WINBOLO_NET_NO_PLAYER);
      } else {
        winbolonetAddEvent(WINBOLO_NET_EVENT_PILL_STEAL, threadsGetContext(), owner, returnValue);
      }
    }
    if (threadsGetContext() == FALSE) {
      frontEndStatusPillbox(pillNum, (pillsGetAllianceNum(value, pillNum)));
    }
  }
  return returnValue;
}

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
void pillsGetDamagePos(pillboxes *value, BYTE xValue, BYTE yValue, BYTE amount) {
  BYTE count;       /* Looping Variable */

  count = 0;
  while (count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
      (*value)->item[count].armour -= amount;
      if ((*value)->item[count].armour > PILL_MAX_HEALTH) {
        (*value)->item[count].armour = 0;
      }
      if ((*value)->item[count].armour == 0) {
        if (threadsGetContext() == FALSE) {
          frontEndStatusPillbox((BYTE) (count+1), pillDead);
        }
      } 
      logAddEvent(log_PillSetHealth, utilPutNibble(count, (*value)->item[count].armour), 0, 0, 0, 0, NULL);

      count = (*value)->numPills;
    }
    count++;
  }
}

/*********************************************************
*NAME:          pillsNumInRect
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 27/3/99
*PURPOSE:
*  Returns the number of hostile pillboxs inside the
*  given rectangle
*
*ARGUMENTS:
*  value   - Pointer to the pillbox structure
*  leftPos   - Left of the rectangle
*  rightPos  - Right of the rectangle
*  top    - Top of the rectangle
*  bottom - Bottom of the rectangle
*********************************************************/
BYTE pillsNumInRect(pillboxes *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  returnValue = 0;
  count = 0;
  while (count < ((*value)->numPills)) {
    if ((*value)->item[count].x >= leftPos && (*value)->item[count].x <= rightPos && (*value)->item[count].y >= top && (*value)->item[count].y <= bottom && (playersIsAllie(screenGetPlayers(), (*value)->item[count].owner, playersGetSelf(screenGetPlayers())) == FALSE)) {
      if ((*value)->item[count].armour > 0) {
        returnValue++;
      }
    }
    count++;
  }
  return returnValue;
}

/*********************************************************
*NAME:          pillsRepairPos
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  Repairs a pill at the specific location
*  given rectangle
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  xValue - X Map position
*  yValue - Y Map position
*********************************************************/
void pillsRepairPos(pillboxes *value, BYTE xValue, BYTE yValue, BYTE treeAmount) {
  BYTE count; /* Looping variable */
  BYTE repairAmount =0;

  count = 0;
  while (count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && ((*value)->item[count].inTank) == FALSE) {
      repairAmount = treeAmount*PILL_REPAIR_AMOUNT;
	  (*value)->item[count].armour = (*value)->item[count].armour + repairAmount;
	  if((*value)->item[count].armour>PILLS_MAX_ARMOUR) {
	    (*value)->item[count].armour = PILLS_MAX_ARMOUR;
      }
      if (threadsGetContext() == FALSE) {
        frontEndStatusPillbox((BYTE) (count+1), (pillsGetAllianceNum(value, (BYTE) (count+1))));
      }
      logAddEvent(log_PillSetHealth, utilPutNibble(count, (*value)->item[count].armour), 0, 0, 0, 0, NULL);
      count = (*value)->numPills;

    }
    count++;
  }
}

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
BYTE pillsGetArmourPos(pillboxes *value, BYTE mx, BYTE my) {
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  
  returnValue = PILL_NOT_FOUND;
  count = 0;
  while (count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == mx && ((*value)->item[count].y) == my && ((*value)->item[count].inTank) == FALSE) {
      returnValue = (*value)->item[count].armour;
      count = (*value)->numPills;
    }
    count++;
  }
  return returnValue;
}


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
bool pillsMoveView(pillboxes *value, BYTE *mx, BYTE *my, int xMove, int yMove) {
  bool returnValue; /* Value to return */
  double nearest;   /* Nearest */
  BYTE count;       /* Looping variable */
  BYTE myPlayerNum; /* Out player number */
  BYTE found;       /* Have we found the item */
  double dist;
  BYTE oldPill;

  nearest = 65000;
  returnValue = FALSE;
  found = 0;
  count = 0;
  oldPill = pillsGetPillNum(value, *mx, *my, FALSE, FALSE);
  oldPill--;
  myPlayerNum = playersGetSelf(screenGetPlayers());
  while (count < (*value)->numPills) {
    if (count != oldPill && (playersIsAllie(screenGetPlayers(), myPlayerNum, (*value)->item[count].owner) == TRUE) && ((*value)->item[count].armour) > 0 && ((*value)->item[count].inTank) == FALSE) {
      if ((yMove == 0 && (xMove < 0 && (*value)->item[count].x < *mx) || (xMove > 0 && (*value)->item[count].x > *mx)) || (xMove == 0 && (yMove < 0 && (*value)->item[count].y < *my) || (yMove > 0 && (*value)->item[count].y > *my))) {
        if (utilIsItemInRange(*mx, *my, (*value)->item[count].x, (*value)->item[count].y, (WORLD) nearest, &dist) == TRUE) {
          nearest = dist;
          found = count;
          returnValue = TRUE;
        }
      }
    }
    count++;
  }


  if (returnValue == TRUE) {
    *mx = (*value)->item[found].x;
    *my = (*value)->item[found].y;
  }
  return returnValue;
}

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
*  value  - Pointer to the pillbox structure
*  mx     - Pointer to hold X Map position (and prev)
*  my     - Pointer to hold Y Map position (and prev)
*  prev   - Whether a previous pill is being passed
*********************************************************/
bool pillsGetNextView(pillboxes *value, BYTE *mx, BYTE *my, bool prev) {
  bool returnValue; /* Value to return */
  bool done;        /* Finished */
  bool okLoop;      /* Ok to loop */
  BYTE playNumber;  /* My player number */
  BYTE count;       /* Counting variable */

  count = 0;
  returnValue = TRUE;
  done = FALSE;
  okLoop = FALSE;
  playNumber = playersGetSelf(screenGetPlayers());

  /* Find out the previous amount */
  if (prev == TRUE) {
    count = pillsGetPillNum(value, *mx, *my, FALSE, FALSE);
    if (count == PILL_NOT_FOUND) {
      count = 0;
    } else {
      count--;
      if ((*value)->item[count].armour > 0 && ((*value)->item[count].inTank) == FALSE) {
        okLoop = TRUE;
        count++;
      } else {
        count = 0;
      }
    }
  }

  /* Find the next item */
  while (done == FALSE && count < ((*value)->numPills)) {
    if ((playersIsAllie(screenGetPlayers(), playNumber, (*value)->item[count].owner) == TRUE) && ((*value)->item[count].armour) > 0 && ((*value)->item[count].inTank) == FALSE) {
      done = TRUE;
      *mx = (*value)->item[count].x;
      *my = (*value)->item[count].y;
    }
    count++;
  }
 
  /* If not found still and we are loop do it here */
  if (done == FALSE && okLoop == TRUE) {
    count = 0;
    while (done == FALSE && count < ((*value)->numPills)) {
      if ((playersIsAllie(screenGetPlayers(), playNumber, (*value)->item[count].owner) == TRUE) && ((*value)->item[count].armour) > 0 && ((*value)->item[count].inTank) == FALSE) {
        done = TRUE;
        *mx = (*value)->item[count].x;
        *my = (*value)->item[count].y;
      }
      count++;
    }
  }

  /* If we still haven't found one then one doesn't exist at all */
  if (done == FALSE) {
    returnValue = FALSE;
  }

  return returnValue;
}

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
bool pillsCheckView(pillboxes *value, BYTE mx, BYTE my) {
  bool returnValue; /* Value to return */
  BYTE playNumber;  /* My player number */
  BYTE pillNum;     /* The pillbox number */

    returnValue = TRUE;
  pillNum = pillsGetPillNum(value, mx, my, FALSE, FALSE);
  playNumber = playersGetSelf(screenGetPlayers());

  if (pillNum == PILL_NOT_FOUND || pillNum == (PILL_NOT_FOUND-1)) {
    returnValue = FALSE;
  } else {
    pillNum--;
    if ((playersIsAllie(screenGetPlayers(), playNumber, (*value)->item[pillNum].owner) == FALSE) || ((*value)->item[pillNum].armour) == 0 || ((*value)->item[pillNum].inTank) == TRUE) {
      returnValue = FALSE;
    }
  }
  
  return returnValue;
}

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
void pillsBaseHit(pillboxes *value, BYTE mx, BYTE my, BYTE baseOwner) {
  BYTE count; /* Looping variable */
  int xDist;  /* x distance from the base */
  int yDist;  /* y distance from the base */

  for (count=0;count<(*value)->numPills;count++) {
    xDist = ((*value)->item[count].x) - mx;
    yDist = ((*value)->item[count].y) - my;
    if (xDist >= PILL_BASE_HIT_LEFT && xDist <= PILL_BASE_HIT_RIGHT && yDist >= PILL_BASE_HIT_TOP && yDist <= PILL_BASE_HIT_BOTTOM && (*value)->item[count].owner != NEUTRAL && (playersIsAllie(screenGetPlayers(), baseOwner, (*value)->item[count].owner) == TRUE) && (*value)->item[count].armour > 0) {
      /* It is in range make it angry */
      (*value)->item[count].coolDown = PILLBOX_COOLDOWN_TIME;
      if ((*value)->item[count].speed > PILLBOX_MAX_FIRERATE) {
        (*value)->item[count].speed /=2;
        if ((*value)->item[count].speed < PILLBOX_MAX_FIRERATE) {
          (*value)->item[count].speed = PILLBOX_MAX_FIRERATE;
        }
      }
    }
  }
}

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
BYTE pillsGetNumNeutral(pillboxes *value) {
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  
  returnValue = 0;
  for (count=0;count<(*value)->numPills;count++) {
    if ((*value)->item[count].owner == NEUTRAL) {
      returnValue++;
    }
  }

  return returnValue;
}

void pillsSetPillCompressData(pillboxes *value, BYTE *buff, int dataLen) {
  memcpy(&(**value), buff, SIZEOF_PILLS);
}

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
void pillsSetPillNetData(pillboxes *value, BYTE *buff, BYTE dataLen) {
  BYTE count = 0;
  BYTE len = 1;
  
  (*value)->numPills = buff[0];
  while (count < (*value)->numPills) {
    (*value)->item[count].x = buff[len];
    len++;
    (*value)->item[count].y = buff[len];
    len++;
    (*value)->item[count].armour = buff[len];
    len++;
    (*value)->item[count].owner = buff[len];
    len++;
    (*value)->item[count].speed = buff[len];
    len++;
    (*value)->item[count].inTank = buff[len];
    len++;
    (*value)->item[count].reload = buff[len];
    len++;
    (*value)->item[count].justSeen = buff[len];
    len++;
    (*value)->item[count].coolDown = buff[len];
    len++;
    count++;
  }

/* was in old code to reset this on join? 
 while (count < MAX_TANKS) {
    (*value)->item[count].reload = PILLBOX_ATTACK_NORMAL;
    (*value)->item[count].speed = PILLBOX_ATTACK_NORMAL;
    count++;
  } */
}

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
BYTE pillsGetPillNetData(pillboxes *value, BYTE *buff) {
  BYTE count = 0;
  BYTE len = 1;

  buff[0] = (*value)->numPills;
  while (count < (*value)->numPills) {
    buff[len] = (*value)->item[count].x;
    len++;
    buff[len] = (*value)->item[count].y;
    len++;
    buff[len] = (*value)->item[count].armour;
    len++;
    buff[len] = (*value)->item[count].owner;
    len++;
    buff[len] = (*value)->item[count].speed;
    len++;
    buff[len] = (*value)->item[count].inTank;
    len++;
    buff[len] = (*value)->item[count].reload;
    len++;
    buff[len] = (*value)->item[count].justSeen;
    len++;
    buff[len] = (*value)->item[count].coolDown;
    len++;
    count++;
  }
  return len;
}


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
void pillsDropSetNeutralOwner(pillboxes *value, BYTE owner) {
  BYTE count;       /* Looping Variable */

  count = 0;
  while (count < ((*value)->numPills)) {
    if (((*value)->item[count].owner) == owner) {
      (*value)->item[count].owner = NEUTRAL;
      netPNBAdd(screenGetNetPnb(), NPNB_PILL_CAPTURE, count, NEUTRAL, (*value)->item[count].x, (*value)->item[count].y, 0);
      if (((*value)->item[count].inTank) == TRUE) {
        (*value)->item[count].inTank = FALSE;
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_DEAD, count, NEUTRAL, (*value)->item[count].x, (*value)->item[count].y, 0);
        logAddEvent(log_PillSetInTank, utilPutNibble(count, FALSE), 0, 0, 0, 0, NULL);
        logAddEvent(log_PillSetOwner, count, NEUTRAL, FALSE, 0, 0, NULL);
      }
    }
    count++;
  }
}

/*********************************************************
*NAME:          pillsMigrate
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
void pillsMigrate(pillboxes *value, BYTE oldOwner, BYTE newOwner) {
  BYTE count;    /* Looping Variable */
  bool isServer; /* Are we a server */

  count = 0;
  isServer = threadsGetContext();
  while (count < ((*value)->numPills)) {
    if (((*value)->item[count].owner) == oldOwner) {
      (*value)->item[count].owner = newOwner;
      netMNTAdd(screenGetNetMnt(), NMNT_PILLMIGRATE, count, newOwner, (*value)->item[count].x, (*value)->item[count].y);
      if (((*value)->item[count].inTank) == TRUE && isServer == TRUE) {
        (*value)->item[count].inTank = FALSE;
        logAddEvent(log_PillSetOwner, count, newOwner, FALSE, 0, 0, NULL);
        logAddEvent(log_PillSetInTank, utilPutNibble(count, FALSE), 0, 0, 0, 0, NULL);
        logAddEvent(log_PillSetPlace, count, (*value)->item[count].x, (*value)->item[count].y, 0, 0, NULL);
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_DEAD, count, newOwner, (*value)->item[count].x, (*value)->item[count].y, 0);
      }
    }
    count++;
  }
}


/*********************************************************
*NAME:          pillsIsCapturable
*AUTHOR:        John Morrison
*CREATION DATE: 10/11/99
*LAST MODIFIED: 10/11/99
*PURPOSE:
*  Returns whether a pillbox a specific location is dead
*  or not.
*
*ARGUMENTS:
*  value  - Pointer to the pillbox structure
*  xValue - X Location of pillbox
*  yValue - Y Location of pillbox
*********************************************************/
bool pillsIsCapturable(pillboxes *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  returnValue = FALSE;
  count = 0;
  while (returnValue == FALSE && count < ((*value)->numPills)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && ((*value)->item[count].armour == 0)  && ((*value)->item[count].inTank == FALSE)) {
      returnValue = TRUE;
      count = (*value)->numPills;
    }
    count++;
  }
  return returnValue;
}


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
void pillsExplicitDrop(pillboxes *value, BYTE owner) {
  BYTE count;       /* Looping Variable */

  count = 0;
  while (count < ((*value)->numPills)) {
    if (((*value)->item[count].owner) == owner) {
      if (((*value)->item[count].inTank) == TRUE) {
        (*value)->item[count].inTank = FALSE;
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_DEAD, count, NEUTRAL, (*value)->item[count].x, (*value)->item[count].y, 0);
        logAddEvent(log_PillSetInTank, utilPutNibble(count, FALSE), 0, 0, 0, 0, NULL);
        logAddEvent(log_PillSetPlace, count, (*value)->item[count].x, (*value)->item[count].y, 0, 0, NULL);
      }
    }
    count++;
  }
}

/*********************************************************
*NAME:          pillsGetBrainPillsInRect
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 09/06/01
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
void pillsGetBrainPillsInRect(pillboxes *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  WORLD wx;        /* X and Y Position of the object */
  WORLD wy;
  BYTE owner;      /* Owner of the object */
  BYTE playerNum;  /* Our player Number */
  bool isAllie;    /* Is this item our allie */
  BYTE count;      /* Looping variable */

  count = 0;
  playerNum = playersGetSelf(screenGetPlayers());

/* typedef struct
	{
	OBJECT object; = 2
	WORLD_X x;
	WORLD_Y y;
	WORD idnum;
	BYTE direction;
	BYTE info;
	} ObjectInfo;
*/

  while (count < ((*value)->numPills)) {
    isAllie = FALSE;
    if ((*value)->item[count].owner != NEUTRAL) {
      isAllie = playersIsAllie(screenGetPlayers(), playerNum, (*value)->item[count].owner);
    }
    if (((((*value)->item[count].x) >= leftPos && ((*value)->item[count].x) <= rightPos && ((*value)->item[count].y) >= top && ((*value)->item[count].y) <= bottom) || isAllie == TRUE) && ((*value)->item[count].inTank == FALSE)) {
      /* In the rectangle */
      wx = (*value)->item[count].x;
      wx <<= TANK_SHIFT_MAPSIZE;
      wy = (*value)->item[count].y;
      wy <<= TANK_SHIFT_MAPSIZE;
      wx += MAP_SQUARE_MIDDLE;
      wy += MAP_SQUARE_MIDDLE;
      if ((*value)->item[count].owner == NEUTRAL) {
        owner = PILLS_BRAIN_NEUTRAL;
      } else if (isAllie == TRUE) {
        owner = PILLS_BRAIN_FRIENDLY;
      } else {
        owner = PILLS_BRAIN_HOSTILE;
      }
      screenAddBrainObject(PILLS_BRAIN_OBJECT_TYPE, wx, wy, count, (*value)->item[count].armour, owner);
    }
    count++;
  }
}

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
bool pillsSetView(pillboxes *value, BYTE pillNum, BYTE playerNum) {
  bool returnValue; /* Value to return */

  returnValue = FALSE;
  if (pillNum < ((*value)->numPills)) {
    if ((*value)->item[pillNum].inTank == FALSE && (*value)->item[pillNum].armour > 0 && playersIsAllie(screenGetPlayers(), playerNum, (*value)->item[pillNum].owner) == TRUE) {
      returnValue = TRUE;
    }
  }
  return returnValue;
}

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
void pillsGetMaxs(pillboxes *value, int *leftPos, int *rightPos, int *top, int *bottom) {
  BYTE count; /* Looping Variable */

  *top = MAP_ARRAY_SIZE;
  *bottom = -1;
  *leftPos = MAP_ARRAY_SIZE;
  *rightPos = -1;

  count = 0;
  while (count < ((*value)->numPills)) {
    if ((*value)->item[count].x < *leftPos) {
      *leftPos = (*value)->item[count].x;
    }
    if ((*value)->item[count].x > *rightPos) {
      *rightPos = (*value)->item[count].x;
    }
    if ((*value)->item[count].y < *top) {
      *top = (*value)->item[count].y;
    }
    if ((*value)->item[count].y > *bottom) {
      *bottom = (*value)->item[count].y;
    }  
    count++;
  }
}

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
void pillsMoveAll(pillboxes *value, int moveX, int moveY) {
  BYTE count; /* Looping Variable */

  count = 0;
  while (count < ((*value)->numPills)) {
    (*value)->item[count].x = (BYTE) ((*value)->item[count].x + moveX);
    (*value)->item[count].y = (BYTE) ((*value)->item[count].y + moveY);
    count++;
  }
}

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
PlayerBitMap pillsGetOwnerBitMask(pillboxes *value, BYTE owner) {
  PlayerBitMap returnValue; /* Value to return */
  BYTE count;               /* Looping variable */

  count = 0;
  returnValue = 0;
  while (count < (*value)->numPills) {
    if ((*value)->item[count].owner == owner) {
      returnValue |= 1 << count;
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          pillsGetAttackSpeed
*AUTHOR:        John Morrison
*CREATION DATE: 29/7/00
*LAST MODIFIED: 29/7/00
*PURPOSE:
* Returns the pillboxes attacking speed. Returns 
* PILLBOX_ATTACK_NORMAL if out of range.
*
*ARGUMENTS:
*  value - Pointer to the pills structure
*  pillNum - Pillbox number to get
*********************************************************/
BYTE pillsGetAttackSpeed(pillboxes *value, BYTE pillNum) {
  BYTE returnValue; /* Value to return */

  returnValue = PILLBOX_ATTACK_NORMAL;
  if (pillNum > 0 && pillNum  <= (*value)->numPills) {
    pillNum--;
    returnValue = (*value)->item[pillNum].speed;
  }

  return returnValue;
}

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
bool pillsIsInView(pillboxes *value, BYTE playerNum, BYTE mx, BYTE my) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  int gapX;
  int gapY;

  count = 0;
  returnValue = FALSE;
  while (count < (*value)->numPills && returnValue == FALSE) {
    if (playersIsAllie(screenGetPlayers(), (*value)->item[count].owner, playerNum) == TRUE) {
      gapX = (*value)->item[count].x - mx;
      gapY = (*value)->item[count].y - my;
      if (gapX >= -10 && gapX <= 10 && gapY >= -10 && gapY <=  10) {
        returnValue = TRUE;
      }
    }
    count++;
  }

  return returnValue;
}

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
BYTE pillsGetNumberOwnedByPlayer(pillboxes *value, BYTE playerNum) {
  BYTE returnValue; /* Value to return */
  BYTE count; /* Looping variable */

  returnValue = 0;
  count = 0;

  while (count < (*value)->numPills) {
    if ((*value)->item[count].owner == playerNum) {
      returnValue++;
    }
    count++;
  }

  return returnValue;
}
