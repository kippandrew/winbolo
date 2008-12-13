/*
 * $Id$
 *
 * Copyright (c) 2000-2008 John Morrison.
 * Copyright (c) 2000-2008 Andrew Roth.
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
*Name:          tcp
*Filename:      tcp.h
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for handling of incoming TCP game list
*  connections
*********************************************************/

#ifndef _TCP_H
#define _TCP_H

#include "global.h"
#include "currentgames.h"
#include "stats.h"

/* Converts a TRUE | FALSE into "Yes" or "No" */
#define TCP_TRUEFALSE_TO_YESNO(X) (X == TRUE ? "Yes" : "No")

#define	TCP_ALL		0
#define	TCP_INTERESTING	1

/*********************************************************
*NAME:          tcpCreate
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Sets up the tcp module and returns the socket handle
*
*ARGUMENTS:
*  port - Port to bind to
*********************************************************/
SOCKET tcpCreate(unsigned short port);

/*********************************************************
*NAME:          tcpDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Cleansup after the tcp module
*
*ARGUMENTS:
*
*********************************************************/
void tcpDestroy();

/*********************************************************
*NAME:          tcpRead
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Checks the socket for new connections
*
*ARGUMENTS:
*  cg - Pointer to current games structure
*********************************************************/
void tcpRead(currentGames *cg);

/*********************************************************
*NAME:          tcpSendGameData
*AUTHOR:        John Morrison, Andrew Roth
*CREATION DATE: 00/10/01
*LAST MODIFIED: 03/04/03 YMD
*PURPOSE:
* Sends the game data
*
*ARGUMENTS:
*  cg   - Pointer to current games structure
*  sock - The socket to send to
*  pack - Address the packet came from
*  filter - If TCP_INTERESTING then sends only
*  interesting games, TCP_ALL sends all games
*********************************************************/
void tcpSendGameData(currentGames *cg, int sock, struct in_addr *pack, int filter);

#endif /* _TCP_H */

