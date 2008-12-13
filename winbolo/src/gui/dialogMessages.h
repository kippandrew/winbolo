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
*Name:          Dialog - Messages
*Filename:      dialogmessages.h
*Author:        John Morrison
*Creation Date: 5/2/99
*Last Modified: 5/2/99
*Purpose:
*  Looks after the send messages dialog box.
*********************************************************/

#ifndef DIALOG_MESSAGES_H
#define DIALOG_MESSAGES_H

#include <windows.h>

/*********************************************************
*NAME:          dialogMessagesCallback
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 5/1/99
*PURPOSE:
*  The Messages dialogbox call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogMessagesCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogMessagesSetup
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 5/1/99
*PURPOSE:
* Sets up the send message dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogMessagesSetup(HWND hWnd);

/*********************************************************
*NAME:          dialogMessagesSend
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 5/1/99
*PURPOSE:
* Send message button has been pressed. Process it here
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogMessagesSend(HWND hWnd);

/*********************************************************
*NAME:          dialogMessagesChange
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Changes the "Sending to X players" message 
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogMessagesChange(HWND hWnd);

#endif /* DIALOG_MESSAGES_H */
