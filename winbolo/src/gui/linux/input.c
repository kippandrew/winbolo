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
*Name:          Input
*Filename:      input.c
*Author:        John Morrison
*Creation Date: 16/12/98
*Last Modified:   1/5/00
*Purpose:
*  Keyboard and mouse routines (ie Direct Input routines)
*********************************************************/

#include "SDL.h"
#include "../../bolo/global.h"
#include "../../bolo/backend.h"
#include "input.h"

tankButton tb;
static BYTE scrollKeyCount = 0; /* Used for screen scrolling */
keyItems heldKeys;

/*********************************************************
*NAME:          inputSetup
*AUTHOR:        John Morrison
*CREATION DATE: 16/12/98
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Sets up input systems, direct draw structures etc.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
* appInst - Handle to the application
* appWnd  - Main Window Handle
*********************************************************/
bool inputSetup() {
  scrollKeyCount = 0;
  /* These are DIK_??? defines */
  heldKeys.kiForward = FALSE;    /* Tank accelerate */
  heldKeys.kiBackward = FALSE;   /* Tank decelerate */
  heldKeys.kiLeft = FALSE;       /* Tank left */
  heldKeys.kiRight = FALSE;      /* Tank right */
  heldKeys.kiShoot = FALSE;      /* Tank shooting */
  heldKeys.kiLayMine = FALSE;     /* Tank lay mine */
  heldKeys.kiGunIncrease = FALSE; /* Increase gunsight length */
  heldKeys.kiGunDecrease = FALSE; /* Decrease gunsight length */
  heldKeys.kiTankView = FALSE;    /* Center on tank */
  heldKeys.kiPillView = FALSE;    /* Pill view */
  heldKeys.kiScrollUp = FALSE;    /* Scroll up */
  heldKeys.kiScrollDown = FALSE;  /* Scroll down */
  heldKeys.kiScrollLeft = FALSE;  /* Scroll left */
  heldKeys.kiScrollRight = FALSE; /* Scroll right */
  return TRUE;
}

/*********************************************************
*NAME:          inputCleanup
*AUTHOR:        John Morrison
*CREATION DATE: 13/12/98
*LAST MODIFIED: 13/12/98
*PURPOSE:
*  Destroys and cleans up input systems, direct draw 
*  structures etc.
*
*ARGUMENTS:
*
*********************************************************/
void inputCleanup(void) {
  return;
}

/*********************************************************
*NAME:          inputGetKeys
*AUTHOR:        John Morrison
*CREATION DATE: 17/12/98
*LAST MODIFIED:   1/5/00
*PURPOSE:
*  Gets the current Buttons that are being pressed.
*  Returns tank buttons being pressed.
*
*ARGUMENTS:
*     hWnd - The main window
*  setKeys - Structure that holds the key settings
*  isMenu  - True if we are in a menu
*********************************************************/
tankButton inputGetKeys(bool isMenu) {
  static BYTE gunsightKeyCount = 0; /* Used for gunsight movement */

  /* FIXME: Check message window doesn't have focus *
  if (GetForegroundWindow() != hWnd || isMenu == TRUE) {
    return TNONE;
   SDL_PumpEvents();
    SDL_PumpEvents();
 } */
 tb = TNONE;
  /* Now set up the tank buttons */
  if (heldKeys.kiForward == TRUE &&  heldKeys.kiRight == TRUE) {
    tb = TRIGHTACCEL;
  } else if (heldKeys.kiForward == TRUE && heldKeys.kiLeft == TRUE) {
    tb = TLEFTACCEL;
  } else if (heldKeys.kiBackward == TRUE && heldKeys.kiLeft == TRUE) {
    tb = TLEFTDECEL;
  } else if (heldKeys.kiBackward == TRUE && heldKeys.kiRight == TRUE) {
    tb = TRIGHTDECEL;
  } else if (heldKeys.kiForward == TRUE) {
    tb = TACCEL;
  } else if (heldKeys.kiBackward == TRUE) {
    tb = TDECEL;
  } else if (heldKeys.kiLeft == TRUE) {
    tb = TLEFT;
  } else if (heldKeys.kiRight == TRUE) {
    tb = TRIGHT;
  } else {
    tb = TNONE;
  }
  /* Get whether the tank is lay a mine */
  if (heldKeys.kiLayMine == TRUE) {
      screenTankLayMine();
  }
  scrollKeyCount++;
  if (scrollKeyCount >= INPUT_SCROLL_WAIT_TIME && isMenu == FALSE) {
    scrollKeyCount = 0;
    /* Scroll Check */
    if (heldKeys.kiScrollUp == TRUE) {
      screenUpdate(up);
    }
    if (heldKeys.kiScrollDown == TRUE) {
      screenUpdate(down);
    }
    if (heldKeys.kiScrollLeft == TRUE) {
      screenUpdate(left);
    }
    if (heldKeys.kiScrollRight == TRUE) {
      screenUpdate(right);
    }
  }
  gunsightKeyCount++;
  if (gunsightKeyCount >= INPUT_GUNSIGHT_WAIT_TIME && isMenu == FALSE) {
    /* Gunsight Check */
    if (heldKeys.kiGunIncrease == TRUE) {
      screenGunsightRange(TRUE);
      gunsightKeyCount = 0;
    } else if (heldKeys.kiGunDecrease == TRUE) {
      screenGunsightRange(FALSE);
      gunsightKeyCount = 0;
    } else if (gunsightKeyCount > (INPUT_GUNSIGHT_WAIT_TIME + 1)) {
      gunsightKeyCount = INPUT_GUNSIGHT_WAIT_TIME;
    }
  }
  return tb;
}

/*********************************************************
*NAME:          inputScroll
*AUTHOR:        John Morrison
*CREATION DATE: 1/5/00
*LAST MODIFIED: 1/5/00
*PURPOSE:
*  Checks and does scrolling of the window
*
*ARGUMENTS:
*     hWnd - The main window
*  setKeys - Structure that holds the key settings
*  isMenu  - True if we are in a menu
*********************************************************/
void inputScroll(bool isMenu) {
  bool proceed; /* Ok to proceed */

  scrollKeyCount++;
  if (scrollKeyCount >= INPUT_SCROLL_WAIT_TIME && isMenu == FALSE) {
    scrollKeyCount = 0;
    /* Scroll Check */
    if (heldKeys.kiScrollUp == TRUE) {
      screenUpdate(up);
    }
    if (heldKeys.kiScrollDown == TRUE) {
      screenUpdate(down);
    }
    if (heldKeys.kiScrollLeft == TRUE) {
      screenUpdate(left);
    }
    if (heldKeys.kiScrollRight == TRUE) {
       screenUpdate(right);
    }
  }
}

/*********************************************************
*NAME:          inputIsFireKeyPressed
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 14/11/99
*PURPOSE:
*  Returns whether the fire key is pressed
*  Old Comment dates before hWnd was added:
*      *CREATION DATE: 25/12/98
*      *LAST MODIFIED: 31/12/98
*
*ARGUMENTS:
*     hWnd - The main window
*  setKeys - Structure that holds the key settings
*   isMenu - TRUE if we are in a menu
*********************************************************/
bool inputIsFireKeyPressed(bool isMenu) {
  bool returnValue;       /* Value to return */

  returnValue = FALSE;
 
  /* Get the fire button state here if it is OK to proceed */
  if (heldKeys.kiShoot == TRUE) {
    returnValue = TRUE;
  }
  return returnValue;
}

void inputButtonInput(keyItems *setKeys, int key, bool newState) {
  if ((setKeys->kiForward) == key) {
    heldKeys.kiForward = newState;
  }
  if ((setKeys->kiBackward) == key) {
    heldKeys.kiBackward = newState;
  }
  if ((setKeys->kiLeft) == key) {
    heldKeys.kiLeft = newState;
  }
  if ((setKeys->kiRight) == key) {
    heldKeys.kiRight = newState;
  }
  if ((setKeys->kiShoot) == key) {
    heldKeys.kiShoot = newState;
  }
  if ((setKeys->kiLayMine) == key) {
    heldKeys.kiLayMine = newState;
  }
  if ((setKeys->kiGunIncrease) == key) {
    heldKeys.kiGunIncrease = newState; /* Increase gunsight length */
  }
  if ((setKeys->kiGunDecrease) == key) {
    heldKeys.kiGunDecrease = newState; /* Decrease gunsight length */
  }
  if ((setKeys->kiScrollUp) == key) {
    heldKeys.kiScrollUp = newState;    /* Scroll up */
  }
  if ((setKeys->kiScrollDown) == key) {
    heldKeys.kiScrollDown = newState;  /* Scroll down */
  }
  if ((setKeys->kiScrollLeft) == key) {
    heldKeys.kiScrollLeft = newState;  /* Scroll left */
  }
  if ((setKeys->kiScrollRight) == key) {
    heldKeys.kiScrollRight = newState; /* Scroll right */
  }
}


