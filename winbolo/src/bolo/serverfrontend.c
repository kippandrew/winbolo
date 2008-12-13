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
*Name:          Server frontend
*Filename:      serverfrontend.c
*Author:        John Morrison
*Creation Date: 3/10/00
*Last Modified: 3/10/00
*Purpose:
* Dummy wrapper for the server functions not required in
* clients
*********************************************************/

/* Inludes */
#include "global.h"
#include "../server/servercore.h"

/*********************************************************
*NAME:          serverCoreLgmOperation
*AUTHOR:        John Morrison
*CREATION DATE: 3/10/00
*LAST MODIFIED: 3/10/00
*PURPOSE:
* A client has requested a lgm operataion
* Always your own for a client.
*
*ARGUMENTS:
*  playerNum - the player numbers lgm to get
*  destX     - X Destination
*  destY     - Y Destination
*  operation - Operation to perform
*********************************************************/
void serverCoreLgmOperation(BYTE playerNum, BYTE destX, BYTE destY, BYTE operation) {
  return;
}


/*********************************************************
*NAME:          serverCoreServerConsoleMessage
*AUTHOR:        John Morrison
*CREATION DATE: 8/7/00
*LAST MODIFIED: 8/7/00
*PURPOSE:
* Displays a server message on the console
*
*ARGUMENTS:
*  mode - Server mode
********************************************************/
void serverCoreServerConsoleMessage(char *msg) {
  return;
}

void serverNetSendManWorkingMessage(BYTE playerNum, BYTE mapX, BYTE mapY, BYTE numMines, BYTE numTrees, BYTE pillNum) {
  return;
	
}

/*********************************************************
*NAME:          serverCoreTankInView
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED:  8/9/00
*PURPOSE:
* Returns if the item at checkX, checkY is in viewing
* range of playre playerNum and any of its pillboxes
*
*ARGUMENTS:
* playerNum - PlayerNum to check
* checkX    - X Position to check
* checkY    - Y Position to check
*********************************************************/
bool serverCoreTankInView(BYTE playerNum, BYTE checkX, BYTE checkY) {
  return TRUE;
}