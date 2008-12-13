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
*Filename:      input.h
*Author:        John Morrison
*Creation Date: 16/12/98
*Last Modified:   1/5/00
*Purpose:
*  Keyboard and mouse routines (ie Direct Input routines)
*********************************************************/

#ifndef _INPUT_H
#define _INPUT_H

#include <windows.h>
#include "..\bolo\global.h"
#include "..\bolo\backend.h"

/* Typestructure that holds the keys */
typedef struct {
  /* These are DIK_??? defines */
  int kiForward;    /* Tank accelerate */
  int kiBackward;   /* Tank decelerate */
  int kiLeft;       /* Tank left */
  int kiRight;      /* Tank right */
  int kiShoot;      /* Tank shooting */
  int kiLayMine;     /* Tank lay mine */
  /* The rest are all WM_KEYDOWN (virtual-key codes) */
  int kiGunIncrease; /* Increase gunsight length */
  int kiGunDecrease; /* Decrease gunsight length */
  int kiTankView;    /* Center on tank */
  int kiPillView;    /* Pill view */
  int kiScrollUp;    /* Scroll up */
  int kiScrollDown;  /* Scroll down */
  int kiScrollLeft;  /* Scroll left */
  int kiScrollRight; /* Scroll right */
} keyItems;


#define INPUT_SCROLL_WAIT_TIME 3
#define INPUT_GUNSIGHT_WAIT_TIME 6

/*********************************************************
*NAME:          inputSetup
*AUTHOR:        John Morrison
*CREATION DATE: 16/12/98
*LAST MODIFIED: 16/12/98
*PURPOSE:
*  Sets up input systems, direct draw structures etc.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
* appInst - Handle to the application
* appWnd  - Main Window Handle
*********************************************************/
bool inputSetup(HINSTANCE appInst, HWND appWnd);

/*********************************************************
*NAME:          inputCleanup
*AUTHOR:        John Morrison
*CREATION DATE: 16/12/98
*LAST MODIFIED: 16/12/98
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
tankButton inputGetKeys(HWND hWnd, keyItems *setKeys, bool isMenu);

/*********************************************************
*NAME:          inputActivate
*AUTHOR:        John Morrison
*CREATION DATE: 17/12/98
*LAST MODIFIED: 17/12/98
*PURPOSE:
*  Application has just got the focus Aquire the input
*
*ARGUMENTS:
* 
*********************************************************/
void inputActivate(void);

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
bool inputIsFireKeyPressed(HWND hWnd, keyItems *setKeys, bool isMenu);

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
void inputScroll(HWND hWnd, keyItems *setKeys, bool isMenu);

#endif
