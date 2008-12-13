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
*Name:          Dialog - Tracker Setup
*Filename:      dialogTrackerSetup.h
*Author:        John Morrison
*Creation Date: 13/11/99
*Last Modified: 13/11/99
*Purpose:
*  Looks after the tracker setup dialog box
*********************************************************/

#ifndef TRACKERSETUP_H
#define TRACKERSETUP_H

#include <windows.h>
#include "..\bolo\global.h"

/*********************************************************
*NAME:          dialogTrackerSetupCallback
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
*  The Tracker Setup Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogTrackerSetupCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogTrackerSetupInit
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
* Called on dialog startup to set up.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogTrackerSetupInit(HWND hWnd);

/*********************************************************
*NAME:          dialogTrackerSetupDone
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
* Called on dialog finishing to pass the game options
* back to the gameFront module. Returns whether all 
* parameters are valid
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
bool dialogTrackerSetupDone(HWND hWnd);

#endif /* TRACKERSETUP_H */
