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
*Name:          Dialog - UDP Setup
*Filename:      dialogudpsetup.h
*Author:        John Morrison
*Creation Date: 20/2/99
*Last Modified: 21/11/99
*Purpose:
*  Looks after the UDP (Interet) setup dialog box
*********************************************************/

#ifndef DIALOG_UDPSETUP_H
#define DIALOG_UDPSETUP_H

#include <windows.h>
#include "..\bolo\global.h"

/* The default UDP port */
#define DEFAULT_UDP_PORT 27500
#define DEFAULT_UDP_PORT_STR "27500\0"
/* Base 10 for itoa */
#define BASE_10 10

/*********************************************************
*NAME:          dialogUdpSetupCallback
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 20/2/99
*PURPOSE:
*  The UDP Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogUdpSetupCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogUdpSetupIpCallback
*AUTHOR:        John Morrison
*CREATION DATE: 21/11/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Callback for the target IP address control window to 
* handle pasting of "ip:port" text to auto split it up
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogUdpSetupIpCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

/*********************************************************
*NAME:          dialogUdpSetupInit
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Called on dialog startup to set up.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogUdpSetupInit(HWND hWnd);

/*********************************************************
*NAME:          dialogUdpSetupDone
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 24/2/99
*PURPOSE:
* Called on dialog finishing to pass the game options
* back to the gameFront module. Returns whether all 
* parameters are valid
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  isJoin - True if we are joing a game else false
*********************************************************/
bool dialogUdpSetupDone(HWND hWnd, bool isJoin);

#endif /* DIALOG_UDPSETUP_H */
