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
*Name:          Dialog - Game Info
*Filename:      dialoggameinfo.h
*Author:        John Morrison
*Creation Date: 26/1/99
*Last Modified: 26/1/99
*Purpose:
*  Looks after the game info dialog box.
*********************************************************/

#ifndef DIALOG_GAME_INFO_H
#define DIALOG_GAME_INFO_H

/* There are 60 seconds in a minute */
#define NUM_SECONDS_MINUTE 60

/*********************************************************
*NAME:          dialogGameInfoCallback
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
*  The Game Info call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogGameInfoCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogGameInfoSetup
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
* Sets up the gameinfo dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogGameInfoSetup(HWND hWnd);

/*********************************************************
*NAME:          dialogGameInfoUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
* Called each time the dialog is supposed to be updated.
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogGameInfoUpdate(HWND hWnd);

#endif /* DIALOG_GAME_INFO_H */
