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
*Name:          brainsHandler
*Filename:      brainsHandler.h
*Author:        John Morrison
*Creation Date: 23/11/99
*Last Modified: 26/11/99
*Purpose:
*  Handles front end brain operations. Menu setting etc.
*********************************************************/

#ifndef BRAINSHANDLER_H
#define BRAINSHANDLER_H

#include <windows.h>
#include "..\bolo\global.h"

/* Strings */
#define SLASH_STRING "\\"
#define SLASH_CHAR '\\'
#define BRAINS_DIR_STRING "Brains"
#define BRAINS_FILTER_STRING "*.brn"
#define BRAINS_EXTENSION ".brn"

/* Resource ID Offset */
#define BRAINS_RESOURCE_OFFSET 41000
#define IDR_BRAIN    2000
#define BRAIN_MAINMENU_OFFSET 5



/*********************************************************
*NAME:          brainsHandlerLoadBrains
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  Loads the brain entries into the brains menu.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
*  appInst - Handle to the application.
*  hWnd  - Main Window Handle.
*********************************************************/
bool brainsHandlerLoadBrains(HWND hWnd);

/*********************************************************
*NAME:          brainsHandlerLoadBrainMenuItems
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 24/11/99
*PURPOSE:
*  Loads the brain file name into the menu.
*  Returns success
*
*ARGUMENTS:
*  brainsList - The list of brains to add on to.
*********************************************************/
bool brainsHandlerLoadBrainMenuItems(HMENU brainsList);

/*********************************************************
*NAME:          brainsHandlerSetDisabled
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED:  3/12/99
*PURPOSE:
*  Sets each item in the menu to be disabled.
*
*ARGUMENTS:
*  hWnd    - Handle to the main window
*  enabled - TRUE if we should enable the items else
*            disable them
*********************************************************/
void brainsHandlerSet(HWND hWnd, bool enabled);

/*********************************************************
*NAME:          brainsHandlerGetNum
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  Returns the number of brains loaded.
*
*ARGUMENTS:
*
*********************************************************/
int brainsHandlerGetNum();

/*********************************************************
*NAME:          brainsHandlerManual
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  The manual brain button has been pressed
*
*ARGUMENTS:
*  hWnd - Window Handle
*********************************************************/
void brainsHandlerManual(HWND hWnd);

/*********************************************************
*NAME:          brainsHandlerStart
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  Try to launch a new brain. Returns success.
*
*ARGUMENTS:
*  hWnd - Window Handle
*  str  - Filename and path of brain to load
*  name - Name of the brain
*********************************************************/
bool brainsHandlerStart(HWND hWnd, char *str, char *name);

/*********************************************************
*NAME:          brainsHandlerItem
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  The brain selection has been made.
*
*ARGUMENTS:
*  hWnd - Window Handle
*  id   - Window parameter offset pressed.
*********************************************************/
void brainsHandlerItem(HWND hWnd, UINT id);

/*********************************************************
*NAME:          brainsHandlerBrainsItem
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  A brains menu item has been selected.
*
*ARGUMENTS:
*  hWnd - Window Handle
*  id   - Window parameter offset pressed.
*********************************************************/
void brainsHandlerBrainsItem(HWND hWnd, UINT id);

/*********************************************************
*NAME:          brainsHandlerShutdown
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 24/11/99
*PURPOSE:
*  Shuts down the brains subsystem.
*
*ARGUMENTS:
*  hWnd - Pointer to the main window
*********************************************************/
void brainsHandlerShutdown(HWND hWnd);

/*********************************************************
*NAME:          brainHandlerIsBrainRunning
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 25/11/99
*PURPOSE:
*  Returns whether a brain is running or not
*
*ARGUMENTS:
*
*********************************************************/
bool brainHandlerIsBrainRunning();

/*********************************************************
*NAME:          brainHandlerRun
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Called to execute the brains "THINK" call if it is
*  running
*
*ARGUMENTS:
*  hWnd - Main Window handler
*********************************************************/
void brainHandlerRun(HWND hWnd);

#endif /* BRAINSHANDLER_H */
