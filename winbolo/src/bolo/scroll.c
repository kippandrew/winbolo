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
*Name:          scroll
*Filename:      scroll.c
*Author:        John Morrison
*Creation Date: 16/01/99
*Last Modified: 10/06/01
*Purpose:
*  Handles scrolling on the screen. Auto scrolling and
*  keeping the object in the centre of the screen
*********************************************************/


#include "global.h"
#include "backend.h"
#include "screen.h"
#include "scroll.h"

/* Is it auto scrolling or not */
bool autoScroll = FALSE;
BYTE scrollX = 0;
BYTE scrollY = 0;
BYTE xPositive = TRUE;
BYTE yPositive = TRUE;
bool autoScrollOverRide = FALSE;
bool mods = FALSE;


/*********************************************************
*NAME:          scrollSetScrollType
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Modifies state of autoscrolling
*
*ARGUMENTS:
*  isAuto - Set to on or off?
*********************************************************/
void scrollSetScrollType(bool isAuto) {
  autoScroll = isAuto;
  autoScrollOverRide = FALSE;
  if (isAuto == FALSE) {
    scrollX = 0;
    scrollY = 0;
    mods = FALSE;
  } 
}

/*********************************************************
*NAME:          scrollCenterObject
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Centres the screen on the object
*
*ARGUMENTS:
*  xValue - Pointer to hold new X co-ordinate
*  yValue - Pointer to hold new Y co-ordinate
*  objectX - Object to centre on X co-ordinate 
*  objectY - Object to centre on Y co-ordinate 
*********************************************************/
void scrollCenterObject(BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY) {
  *xValue = objectX - SCROLL_CENTER;
  *yValue = objectY - SCROLL_CENTER;
  autoScrollOverRide = FALSE;
}

/*********************************************************
*NAME:          scrollUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 19/11/99
*PURPOSE:
*  Called every game tick. Checks to see if the screen 
*  is required to be moved because the tank has moved
*  etc.
*  If the object is not a tank the last 3 parameters
*  are ignored. It returns if a recalculation is needed
*
*ARGUMENTS:
*  pb        - Pointer to the pillboxes structure
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Object to centre on X co-ordinate 
*  objectY   - Object to centre on Y co-ordinate 
*  isTank    - Is the object a tank
*  gunsightX - The gunsights X position
*  gunsightY - The gunishgts Y position
*  speed     - The speed of the tank
*  armour    - Amount of armour on the tank
*  angle     - Tank travelling angle
*  manual    - Is it manual move (ie by keys, not tank)
*********************************************************/
bool scrollUpdate(pillboxes *pb, BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, bool isTank, BYTE gunsightX, BYTE gunsightY, BYTE speed, BYTE armour, TURNTYPE angle, bool manual) {
  bool returnValue; /* Value to return */
  
  returnValue = TRUE;
  if (screenTankIsDead() == TRUE) {
    returnValue = FALSE;
  } else if (manual == TRUE) {
    returnValue = scrollManual(xValue, yValue, objectX, objectY, angle);
  } else if (autoScroll == TRUE && isTank == TRUE && armour <= TANK_FULL_ARMOUR && autoScrollOverRide == FALSE) {
    /* Calculate using the autoscroll function */
    returnValue = scrollAutoScroll(pb, xValue, yValue, objectX, objectY, gunsightX, gunsightY, speed, angle);
  } else {
   /* calculate not using auto scroll functions */
    returnValue = scrollNoAutoScroll(xValue, yValue, objectX, objectY, angle);
  }

  return returnValue;

}

/*********************************************************
*NAME:          scrollCheck
*AUTHOR:        John Morrison
*CREATION DATE: 19/11/99
*LAST MODIFIED: 19/11/99
*PURPOSE:
*  Returns whether an item is on screen or not
*
*ARGUMENTS:
*  xValue    - Current X co-ordinate
*  yValue    - Current Y co-ordinate
*  objectX   - Objects X co-ordinate 
*  objectY   - Objects Y co-ordinate 
*********************************************************/
bool scrollCheck(BYTE xValue, BYTE yValue, BYTE objectX, BYTE objectY) {
  bool returnValue; /* Used internally */
    
  returnValue = TRUE;
  /* Check to see if moving towards screen edge */
  if ((objectX - xValue) > MAIN_SCREEN_SIZE_X) {
    xValue++;
    returnValue = FALSE;
  }
  if ((objectX-1) < xValue) {
    xValue--;
    returnValue = FALSE;
  }
  if ((objectY - yValue) > MAIN_SCREEN_SIZE_Y) {
    returnValue = FALSE;
  }
  if (objectY <= yValue) {
   returnValue = FALSE;
  }

  return returnValue;
}


/*********************************************************
*NAME:          scrollManual
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 10/06/01
*PURPOSE:
*  Movement scroll keys have been pressed. Returns if a 
*  movement occurs
*
*ARGUMENTS:
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Objects X co-ordinate 
*  objectY   - Objects Y co-ordinate 
*  angle     - Items Angle
*********************************************************/
bool scrollManual(BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, TURNTYPE angle) {
  bool returnValue; /* Used internally */
  bool leftPos;
  bool rightPos;
  bool upPos;
  bool downPos;

  leftPos = FALSE;
  rightPos = FALSE;
  upPos = FALSE;
  downPos = FALSE;
  returnValue = FALSE;
  autoScrollOverRide = TRUE;

  if (angle >= BRADIANS_SSWEST && angle <= BRADIANS_NNWEST) {
    rightPos = TRUE;
  }
  if (angle >= BRADIANS_NNEAST && angle <= BRADIANS_SSEAST) {
    leftPos = TRUE;
  }
  if (angle >= BRADIANS_SEASTE  && angle <= BRADIANS_SWESTW) {
    downPos = TRUE;
  }
  if (angle <= BRADIANS_NEASTE || angle >= BRADIANS_NWESTW) {
    upPos = TRUE;
  }

  
  /* Check to see if moving towards screen edge */
  if ((objectX - (*xValue)) > MAIN_SCREEN_SIZE_X && leftPos == TRUE) {
    (*xValue)++;
    returnValue = TRUE;
  }
  if ((objectX-2) < (*xValue) && rightPos == FALSE) {
    (*xValue)--;
    returnValue = TRUE;
  }
  if ((objectY - (*yValue)) > MAIN_SCREEN_SIZE_Y && downPos == TRUE) {
    (*yValue)++;
    returnValue = TRUE;
  }
  if (objectY <= (*yValue) && upPos == TRUE) {
   (*yValue)--;
   returnValue = TRUE;
  }
  if (mods == TRUE && returnValue == TRUE) {
    mods = FALSE;
  }

  return returnValue;
}

/*********************************************************
*NAME:          scrollNoAutoScroll
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 19/11/99
*PURPOSE:
*  Checks to see if the screen is required to be moved 
*  because the object is moving off screen. Doesn't 
*  use autoscrolling features. Returns if a recalculation
*  of the screen is needed
*
*ARGUMENTS:
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Object to centre on X co-ordinate 
*  objectY   - Object to centre on Y co-ordinate 
*  angle     - Turntype angle
*********************************************************/
bool scrollNoAutoScroll(BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, TURNTYPE angle) {
  bool returnValue; /* Value to return */
  bool leftPos;
  bool rightPos;
  bool upPos;
  bool downPos;

  leftPos = FALSE;
  rightPos = FALSE;
  upPos = FALSE;
  downPos = FALSE;
  returnValue = FALSE;

  if (angle >= BRADIANS_SSWEST && angle <= BRADIANS_NNWEST) {
    rightPos = TRUE;
  }
  if (angle >= BRADIANS_NNEAST && angle <= BRADIANS_SSEAST) {
    leftPos = TRUE;
  }
  if (angle >= BRADIANS_SEASTE  && angle <= BRADIANS_SWESTW) {
    downPos = TRUE;
  }
  if (angle <= BRADIANS_NEASTE || angle >= BRADIANS_NWESTW) {
    upPos = TRUE;
  }

  /* Check to see if moving towards screen edge */
  if ((objectX - (*xValue)) >= (MAIN_SCREEN_SIZE_X-NO_SCROLL_EDGE) && leftPos == TRUE) {
    (*xValue)++;
    returnValue = TRUE;
  }
  if ((objectX-1) < (*xValue)+NO_SCROLL_EDGE && rightPos == TRUE) {
    (*xValue)--;
    returnValue = TRUE;
  }
  if ((objectY - (*yValue)) >= (MAIN_SCREEN_SIZE_Y-NO_SCROLL_EDGE) && downPos == TRUE) {
    (*yValue)++;
    returnValue = TRUE;
  }
  if (objectY <= (*yValue)+NO_SCROLL_EDGE && upPos == TRUE) {
   (*yValue)--;
   returnValue = TRUE;
  }
  if (mods == TRUE && returnValue == TRUE) {
    mods = FALSE;
  }
  if (returnValue == TRUE) {
    autoScrollOverRide = FALSE;
  }

  return returnValue;
}

/*********************************************************
*NAME:          scrollAutoScroll
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Checks to see if the screen is required to be moved 
*  because the object is moving off screen. Uses the
*  autoscrolling features. Returns if a recalculation
*  of the screen is needed
*
*ARGUMENTS:
*  pb        - Pointer to the pillboxes structure
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Object to centre on X co-ordinate 
*  objectY   - Object to centre on Y co-ordinate 
*  gunsightX - The gunsights X position
*  gunsightY - The gunishgts Y position
*  speed     - The speed of the tank
*  angle     - Angle of the tank
*********************************************************/
bool scrollAutoScroll(pillboxes *pb, BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, BYTE gunsightX, BYTE gunsightY, BYTE speed, TURNTYPE angle) {
  bool returnValue;        /* Value to return */

  returnValue = FALSE;


  if (scrollX == 0 && scrollY == 0) {
    /* Check to see if moving towards screen edge */
    if (((gunsightX-1) - (*xValue)) >= (MAIN_SCREEN_SIZE_X)) {
      scrollX = (BYTE) (speed / SCROLL_DIVIDE);
      if (scrollX == 0) {
        scrollX = 1;
      }
      xPositive = TRUE;
      mods = TRUE;
    }
    if ((gunsightX) < (*xValue)) {
      scrollX = (BYTE) (speed / SCROLL_DIVIDE);
      if (scrollX == 0) {
        scrollX = 1;
      }
      xPositive = FALSE;
      mods = TRUE;
    }
    if (((gunsightY-1)- (*yValue)) >= (MAIN_SCREEN_SIZE_Y)) {
      scrollY = (BYTE) (speed / SCROLL_DIVIDE);
      if (scrollY == 0) {
        scrollY = 1;
      }

      yPositive = TRUE;
      mods = TRUE;
    }
    if (gunsightY < (*yValue)) {
      scrollY = (BYTE) (speed / SCROLL_DIVIDE);
      if (scrollY == 0) {
        scrollY = 1;
      }

      yPositive = FALSE;
      mods = TRUE;
    } 
  }


  if (scrollX > 0) {
    returnValue = TRUE;
    scrollX--;
    if (xPositive == TRUE) {
      (*xValue)++;
    } else {
      (*xValue)--;
    }
  }
  if (scrollY > 0) {
    scrollY--;
    returnValue = TRUE;
    if (yPositive == TRUE) {
      (*yValue)++;
    } else {
      (*yValue)--;
    }
  }
  if (scrollX == 0 && scrollY == 0) {
    mods = FALSE;
  }

  return returnValue;
}
