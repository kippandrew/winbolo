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

#ifndef _INPUT_H
#define _INPUT_H

#include "../../bolo/global.h"
#include "../../bolo/backend.h"

#define INPUT_SCROLL_WAIT_TIME 3
#define INPUT_GUNSIGHT_WAIT_TIME 6

typedef struct {
  /* These are DIK_??? defines */
  uint kiForward;    /* Tank accelerate */
  uint kiBackward;   /* Tank decelerate */
  uint kiLeft;       /* Tank left */
  uint kiRight;      /* Tank right */
  uint kiShoot;      /* Tank shooting */
  uint kiLayMine;     /* Tank lay mine */
/* The rest are all WM_KEYDOWN (virtual-key codes) */
  uint kiGunIncrease; /* Increase gunsight length */
  uint kiGunDecrease; /* Decrease gunsight length */
  uint kiTankView;    /* Center on tank */
  uint kiPillView;    /* Pill view */
  uint kiScrollUp;    /* Scroll up */
  uint kiScrollDown;  /* Scroll down */
  uint kiScrollLeft;  /* Scroll left */
  uint kiScrollRight; /* Scroll right */
} keyItems;



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
bool inputSetup();

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
void inputCleanup(void); 

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
tankButton inputGetKeys(bool isMenu); 

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
void inputScroll(bool isMenu);

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
bool inputIsFireKeyPressed(bool isMenu);

void inputButtonInput(keyItems *setKeys, int key, bool newState);

#endif /* _INPUT_H */

