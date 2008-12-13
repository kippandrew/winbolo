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
*Name:          Dialog - Game Finder
*Filename:      dialogGameFinder.h
*Author:        John Morrison
*Creation Date: 18/1/00
*Last Modified:  2/6/00
*Purpose:
*  Looks after the Game finder dialog
*********************************************************/

#ifndef _DIALOG_GAMEFINDER_H
#define _DIALOG_GAMEFINDER_H

#include <windows.h>
#include "..\bolo\global.h"


/*********************************************************
*NAME:          dialogGameFinderSetMethod
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 18/1/00
*PURPOSE:
*  Sets the lookup method for the finding games and the
*  title of the dialog box.
*
*ARGUMENTS:
*  title            - Title of the dialog box
*  useTrackerMethod - TRUE if we are to use the tracker
*                     for game lookups. Else we broadcast
*********************************************************/
void dialogGameFinderSetMethod(char *title, bool useTrackerMethod);

/*********************************************************
*NAME:          dialogGameFinderCallback
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 18/1/00
*PURPOSE:
*  The GameFinder Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogGameFinderCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);

/*********************************************************
*NAME:          dialogGameFinderInit
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 18/1/00
*PURPOSE:
* Called on dialog startup.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameFinderInit(HWND hWnd);

/*********************************************************
*NAME:          dialogGameFinderClear
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 18/1/00
*PURPOSE:
* Clears the dialog of game information and empties the
* list box.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameFinderClear(HWND hWnd);

/*********************************************************
*NAME:          dialogGameFinderJoin
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 27/5/00
*PURPOSE:
* Called on attampt to join the currently selected game.
* Returns whether we can join the game or not. ie version
* numbers the same
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
bool dialogGameFinderJoin(HWND hWnd);

/*********************************************************
*NAME:          dialogGameFinderRefresh
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 18/1/00
*PURPOSE:
* Refreshs the list of active games.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameFinderRefresh(HWND hWnd);

/*********************************************************
*NAME:          dialogGameFinderBuildList
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 18/1/00
*PURPOSE:
* Fills the list box with the current games
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameFinderBuildList(HWND hWnd);

/*********************************************************
*NAME:          dialogGameFinderSearch
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 18/1/00
*PURPOSE:
* Does the search for games.
*
*ARGUMENTS:
* hWnd - Handle to the dialog
**********************************************************/
void dialogGameFinderSearch(HWND hWnd);

/*********************************************************
*NAME:          dialogGameFinderSetItem
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 18/1/00
*PURPOSE:
* Sets the details up about a particular item
*
*ARGUMENTS:
*  hWnd    - Handle to the window
*  itemNum - The item number pressed 
*********************************************************/
void dialogGameFinderSetItem(HWND hWnd, int itemNum);

#endif /* _DIALOG_GAMEFINDER_H */
