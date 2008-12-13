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
*Name:          Dialog - Opening
*Filename:      dialogopening.h
*Author:        John Morrison
*Creation Date: 27/1/99
*Last Modified: 27/1/99
*Purpose:
*  Looks after the Opening/Welcoming dialog
*********************************************************/

#ifndef DIALOG_OPENING_H
#define DIALOG_OPENING_H

#include <windows.h>
#include "..\bolo\global.h"

/*********************************************************
*NAME:          dialogOpeningCallback
*AUTHOR:        John Morrison
*CREATION DATE: 27/1/99
*LAST MODIFIED: 27/1/99
*PURPOSE:
*  The Opening Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogOpeningCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogOpeningSetup
*AUTHOR:        John Morrison
*LAST MODIFIED: 28/4/00
*LAST MODIFIED: 28/4/00
*PURPOSE:
*  The Opening Dialog setup function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
void dialogOpeningSetup(HWND hWnd);

#endif /* DIALOG_OPENING_H */
