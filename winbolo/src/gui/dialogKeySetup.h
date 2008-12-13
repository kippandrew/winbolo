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
*Name:          Dialog - Key Setup
*Filename:      dialogkeysetup.c
*Author:        John Morrison
*Creation Date: 31/1/99
*Last Modified: 28/4/00
*Purpose:
*  Looks after the keyboard setup dialog box
*********************************************************/

#ifndef DIALOG_KEY_SETUP_H
#define DIALOG_KEY_SETUP_H

#include <windows.h>
#include "..\bolo\global.h"

/* Get Key Window class and window caption */
#define WIND_KEYCLASSNAME "BoloGetKey"

/* Defines what the next key will modify if any */
typedef enum {
  nkNone,
  nkForward,
  nkBackward,
  nkTurnLeft,
  nkTurnRight,
  nkIncrease,
  nkDecrease,
  nkShoot,
  nkLayMine,
  nkTankView,
  nkPillView,
  nkUp,
  nkDown,
  nkLeft,
  nkRight
} nextKey;

/*********************************************************
*NAME:          dialogKeySetupCallback
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
*  The Key setup call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogKeySetupCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);

/*********************************************************
*NAME:          dialogKeySetupInit
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
*  Setups up the dialog box
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogKeySetupInit(HWND hWnd);

/*********************************************************
*NAME:          dialogKeySetupVirtKeyToString
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* Takes a windows virtual key and places a string 
* identifier into str of it.
*
*ARGUMENTS:
*  value - Virtual key.
*  str   - String value of it.
*********************************************************/
void dialogKeySetupVirtKeyToString(int value, char *str);

/*********************************************************
*NAME:          dialogKeySetupDIKeyToString
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* Takes a Direct Input and key and places a string 
* identifier into str of it.
*
*ARGUMENTS:
*  value - DI Identifier.
*  str   - String value of it.
*********************************************************/
void dialogKeySetupDIKeyToString(int value, char *str);

/*********************************************************
*NAME:          dialogKeySetupVirtKeyToDI
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* Returns the Direct Input equivilent of the windows 
* virtual key pressed.
*
*ARGUMENTS:
*  value - virtual key.
*********************************************************/
int dialogKeySetupVirtKeyToDI(int value);

/*********************************************************
*NAME:          dialogKeySetupDisplayString
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* Sets the given dialog item to the string given.
*
*ARGUMENTS:
*  hWnd  - Window Handle of the dialog 
*  value - The dialog item to set.
*  str   - The string to set it to.
*********************************************************/
void dialogKeySetupDisplayString(HWND hWnd, nextKey value, char *str);

/*********************************************************
*NAME:          dialogKeySetupUnCheck
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
* Get Key window has shut down. Uncheck things if they
* haven't been unchecked
*
*ARGUMENTS:
*  hWnd   - Handle to the dialog
*********************************************************/ 
void dialogKeySetupUnCheck(HWND hWnd);

/*********************************************************
*NAME:          dialogKeySetupProcess
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* One of the buttons has been clicked. Check it (uncheck
* others etc.)  and set up the get key window.
*
*ARGUMENTS:
*  hWnd    - Window Handle of the dialog 
*  newKey  - The new key to get
*********************************************************/
void dialogKeySetupClick(HWND hWnd, nextKey newKey);

/*********************************************************
*NAME:          dialogKeySetupProcess
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* A key has been pressed and we we are in the state to 
* capture the next key. Process it here and update the
* screen
*
*ARGUMENTS:
*  hWnd    - Window Handle of the dialog 
*  keyCode - KeyCode of the button pressed
*********************************************************/
void dialogKeySetupProcess(HWND hWnd, int keyCode);

/*********************************************************
*NAME:          GetKeyProc
*AUTHOR:        John Morrison
*CREATION DATE: 1/2/99
*LAST MODIFIED: 1/2/99
*PURPOSE:
*  The main window call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More message parameters
*********************************************************/
LRESULT CALLBACK GetKeyProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif /* DIALOG_KEY_SETUP_H */
