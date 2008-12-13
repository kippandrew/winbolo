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
*Name:          Dialog - Network Info
*Filename:      dialognetworkinfo.h
*Author:        John Morrison
*Creation Date: 3/3/99
*Last Modified: 1/9/99
*Purpose:
*  Looks after the network info dialog box.
*********************************************************/

#ifndef DIALOG_NETWORK_INFO_H
#define DIALOG_NETWORK_INFO_H

#include <windows.h>

/*********************************************************
*NAME:          dialogNetInfoCallback
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 3/3/99
*PURPOSE:
*  The Network Info call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogNetInfoCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogNetInfoSetup
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 3/3/99
*PURPOSE:
* Sets up the dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogNetInfoSetup(HWND hWnd);

/*********************************************************
*NAME:          dialogNetInfoUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 1/9/99
*PURPOSE:
* Updates the network info dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogNetInfoUpdate(HWND hWnd);

#endif /* DIALOG_NETWORK_INFO_H */
