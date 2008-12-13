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
*Name:          Dialog - WinBolonet
*Filename:      dialogwinbolonet.h
*Author:        John Morrison
*Creation Date: 24/06/02
*Last Modified: 24/06/02
*Purpose:
*  Our Winbolo.net information
*********************************************************/

#ifndef __WINBOLO_NET_DIALOG_H
#define __WINBOLO_NET_DIALOG_H

#include <windows.h>
#include "..\..\bolo\global.h"


/*********************************************************
*NAME:          dialogWinbolonetSetup
*AUTHOR:        John Morrison
*CREATION DATE: 24/06/02
*LAST MODIFIED: 24/06/02
*PURPOSE:
*  Sets up the dialog window
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogWinbolonetSetup(HWND hWnd);


/*********************************************************
*NAME:          dialogWinbolonetSetup
*AUTHOR:        John Morrison
*CREATION DATE: 24/06/02
*LAST MODIFIED: 24/06/02
*PURPOSE:
* Called on dialog exit. Returns if the dialog can close
* or not because of configuration errors
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
bool dialogWinbolonetOK(HWND hWnd);

/*********************************************************
*NAME:          dialogWinbolonetCallback
*AUTHOR:        John Morrison
*CREATION DATE: 24/06/02
*LAST MODIFIED: 24/06/02
*PURPOSE:
*  The Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogWinbolonetCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

#endif /* __WINBOLO_NET_DIALOG_H */

