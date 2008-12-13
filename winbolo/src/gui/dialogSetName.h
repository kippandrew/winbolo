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
*Name:          Dialog - Set Player Name
*Filename:      dialogSetname.h
*Author:        John Morrison
*Creation Date: 1/2/99
*Last Modified: 2/6/00
*Purpose:
*  Looks after the set player name dialog box.
*********************************************************/

#ifndef _DIALOG_SET_PLAYER_NAME_H
#define _DIALOG_SET_PLAYER_NAME_H

#include <windows.h>

/*********************************************************
*NAME:          dialogSetNameCallback
*AUTHOR:        John Morrison
*CREATION DATE: 1/2/99
*LAST MODIFIED: 1/2/99
*PURPOSE:
*  The Set Player name dialog box call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogSetNameCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);

/*********************************************************
*NAME:          dialogSetNameInit
*AUTHOR:        John Morrison
*CREATION DATE: 1/2/99
*LAST MODIFIED: 1/2/99
*PURPOSE:
* Sets up the set player name dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogSetNameInit(HWND hWnd);

/*********************************************************
*NAME:          dialogSetNameDone
*AUTHOR:        John Morrison
*CREATION DATE: 1/2/99
*LAST MODIFIED: 1/2/99
*PURPOSE:
* Called when the OK button is pressed to update the player
* name
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogSetNameDone(HWND hWnd);

/*********************************************************
*NAME:          dialogSetNameInGame
*AUTHOR:        John Morrison
*CREATION DATE: 2/6/00
*LAST MODIFIED: 2/6/00
*PURPOSE:
* Sets whether we are inside a game or not
*
*ARGUMENTS:
*  inGame - Sets if we are inside a game or not
*********************************************************/
void dialogSetNameInGame(bool inGame);

#endif /* _DIALOG_SET_PLAYER_NAME_H */
