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
*Name:          Dialog - Skins
*Filename:      dialogSkins.h
*Author:        John Morrison
*Creation Date: 28/09/03
*Last Modified: 28/09/03
*Purpose:
*  Looks after the skins dialog
*********************************************************/

#include <windows.h>
#include "..\..\bolo\global.h"


/*********************************************************
*NAME:          dialogSkinsCallback
*AUTHOR:        John Morrison
*CREATION DATE: 28/09/03
*LAST MODIFIED: 28/09/03
*PURPOSE:
*  The Skins Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogSkinsCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogSkinsSetInit
*AUTHOR:        John Morrison
*CREATION DATE: 28/09/03
*LAST MODIFIED: 28/09/03
*PURPOSE:
*  The Skins Dialog init function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogSkinsInit(HWND hWnd);

/*********************************************************
*NAME:          dialogSkinsLoadList
*AUTHOR:        John Morrison
*CREATION DATE: 28/09/03
*LAST MODIFIED: 28/09/03
*PURPOSE:
*  Loads the list of available skins into the 
*  dialog box
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogSkinsLoadList(HWND hWnd);

/*********************************************************
*NAME:          dialogSkinsSetText
*AUTHOR:        John Morrison
*CREATION DATE: 28/09/03
*LAST MODIFIED: 28/09/03
*PURPOSE:
*  Sets the text in the dialog box
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogSkinsSetText(HWND hWnd);