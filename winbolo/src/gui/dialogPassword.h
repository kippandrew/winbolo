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
*Name:          Dialog - Password
*Filename:      dialogpassword.h
*Author:        John Morrison
*Creation Date: 24/2/99
*Last Modified: 24/2/99
*Purpose:
*  Looks after the Get password dialog box
*********************************************************/

#ifndef DIALOG_PASSWORD_H
#define DIALOG_PASSWORD_H

#include <windows.h>
#include "..\bolo\global.h"

/*********************************************************
*NAME:          dialogPasswordCallback
*AUTHOR:        John Morrison
*CREATION DATE: 24/2/99
*LAST MODIFIED: 24/2/99
*PURPOSE:
*  The Opening Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogPasswordCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

#endif /* DIALOG_PASSWORD_H */
