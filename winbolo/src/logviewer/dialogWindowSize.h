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
*Name:          Dialog - Window Size
*Filename:      dialogWindowSize.c
*Author:        John Morrison
*Creation Date: 07/05/01
*Last Modified: 07/05/01
*Purpose:
*  Looks after the Game setup dialog
*********************************************************/

#ifndef _DIALOG_WINDOW_SIZE_H
#define _DIALOG_WINDOW_SIZE_H

/*********************************************************
*NAME:          dialogWindowSizeCallback
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  The Opening Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogWindowSizeCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );


#endif /* _DIALOG_WINDOW_SIZE_H */