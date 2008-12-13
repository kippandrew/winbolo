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
*Name:          Dialog - Alliance
*Filename:      dialogAlliance.h
*Author:        John Morrison
*Creation Date: 11/6/00
*Last Modified: 11/6/00
*Purpose:
*  Looks after the Alliance dialog
*********************************************************/

#ifndef _DIALOG_ALLIANCE_H
#define _DIALOG_ALLIANCE_H

#include <windows.h>

/*********************************************************
*NAME:          dialogAllianceCallback
*AUTHOR:        John Morrison
*CREATION DATE: 11/6/00
*LAST MODIFIED: 11/6/00
*PURPOSE:
*  The Alliance Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogAllianceCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogAllianceInit
*AUTHOR:        John Morrison
*CREATION DATE: 11/6/00
*LAST MODIFIED: 11/6/00
*PURPOSE:
*  The Alliance Dialog init function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogAllianceInit(HWND hWnd);

/*********************************************************
*NAME:          dialogAllianceInit
*AUTHOR:        John Morrison
*CREATION DATE: 11/6/00
*LAST MODIFIED: 11/6/00
*PURPOSE:
*  The Alliance Dialog init function
*
*ARGUMENTS:
*  hWnd       - Handle to the window
*  hParentWnd - The parent window handle
*  playerName - The player name to display
*  playerNum  - The player number that is requesting you 
*               alliance
*********************************************************/
void dialogAllianceSetName(HWND hWnd, HWND hParentWnd, char *playerName, BYTE playerNum);

#endif /* _DIALOG_ALLIANCE_H */

