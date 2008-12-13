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
*Name:          Dialog - Game Setup
*Filename:      dialoggamesetup.h
*Author:        John Morrison
*Creation Date: 27/1/99
*Last Modified:  2/7/00
*Purpose:
*  Looks after the Game setup dialog
*********************************************************/

#ifndef DIALOG_GAMESETUP_H
#define DIALOG_GAMESETUP_H

#include <windows.h>
#include "..\bolo\global.h"

/* There are 60 seconds in a minute */
#define NUM_SECONDS 60 

/*********************************************************
*NAME:          dialogGameSetupCallback
*AUTHOR:        John Morrison
*CREATION DATE: 27/1/99
*LAST MODIFIED: 27/1/99
*PURPOSE:
*  The Opening Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogGameSetupCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogGameFinderDlgNotify
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  The open dialog file chooser notify event
*
*ARGUMENTS:
*  hDlg - Handle to the dialog
*  pofn - Notify message
*********************************************************/
BOOL NEAR PASCAL dialogGameFinderDlgNotify(HWND hDlg, LPOFNOTIFY pofn);

/*********************************************************
*NAME:          dialogGameFinderOpeningProc
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  The open dialog file chooser notify event
*
*ARGUMENTS:
*  hDlg   - Handle to the window
*  uMsg   - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogGameFinderOpeningProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*********************************************************
*NAME:          dialogGameSetupChooseMap
*AUTHOR:        John Morrison
*CREATION DATE: 28/1/99
*LAST MODIFIED: 28/1/99
*PURPOSE:
*  The Choose Map button has been pushed. Operate the
*  dialog box.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameSetupChooseMap(HWND hWnd);

/*********************************************************
*NAME:          dialogGameSetupMapCheck
*AUTHOR:        John Morrison
*CREATION DATE: 28/1/99
*LAST MODIFIED: 28/1/99
*PURPOSE:
* Checks to see if the map is valid set the Map name 
* to be it and enable the dialog OK button - 
* else disable the OK button
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  fileName - filename to check
*********************************************************/
void dialogGameSetupMapCheck(HWND hWnd, char *fileName);

/*********************************************************
*NAME:          dialogGameSetupInit
*AUTHOR:        John Morrison
*CREATION DATE: 28/1/99
*LAST MODIFIED: 28/1/99
*PURPOSE:
* Called on dialog startup to handle command line
* arguments 
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameSetupInit(HWND hWnd);

/*********************************************************
*NAME:          dialogGameSetupDone
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 29/1/99
*PURPOSE:
* Called on dialog finishing to pass the game options
* back to the gameFront module
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameSetupDone(HWND hWnd);

#endif /* DIALOG_GAMESETUP_H */
