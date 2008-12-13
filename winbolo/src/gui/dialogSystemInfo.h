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
*Name:          Dialog - System Info
*Filename:      dialogsysteminfo.h
*Author:        John Morrison
*Creation Date: 23/1/99
*Last Modified: 29/1/99
*Purpose:
*  Looks after the system info dialog box.
*********************************************************/

#ifndef DIALOG_SYSTEM_INFO_H
#define DIALOG_SYSTEM_INFO_H

/* There are 1000 miliseconds in a second */ 
#define SECOND 1000.0

/*********************************************************
*NAME:          dialogSysInfoCallback
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
*  The System Info call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogSysInfoCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogSysInfoSetup
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
* Sets up the gameinfo dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogSysInfoSetup(HWND hWnd);

/*********************************************************
*NAME:          dialogSysInfoSetup
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 29/1/99
*PURPOSE:
* Updates the system info dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogSysInfoUpdate(HWND hWnd);

#endif /* DIALOG_SYSTEM_INFO_H */
