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
*Creation Date: 27/02/05
*Last Modified: 27/02/05
*Purpose:
*  The Fullscreen setup Dialog
*********************************************************/

#ifndef __DIALOGFULLSCREENSETUP_H
#define __DIALOGFULLSCREENSETUP_H

#include <windows.h>
#include <ddraw.h>
#include "global.h"


/*********************************************************
*NAME:          dialogFullscreenSetupCallback
*AUTHOR:        John Morrison
*Creation Date: 27/02/05
*Last Modified: 27/02/05
*PURPOSE:
*  The Fullscreen setup Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogFullscreenSetupCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

BOOL WINAPI dialogFullScreenEnumCallback(LPDDSURFACEDESC lpDDSurfaceDesc,  LPVOID lpContext);

#endif /* __DIALOGFULLSCREENSETUP_H */
