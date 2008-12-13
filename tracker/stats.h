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
*Name:          stats
*Filename:      stats.h
*Author:        Andrew Roth
*Creation Date: 00/11/06
*LAST MODIFIED: 00/11/06
*Purpose:
*  Responsable for saving stats to file, and generating
*  a stats summary (which can use html tags)
*********************************************************/

#ifndef _STATS_H
#define _STATS_H

#define STATS_LOG_NEW_GAME      0
#define STATS_LOG_GAME_UPDATE   1
#define STATS_LOG_TELNET_LIST   2

int udpMaxThreads;

/*********************************************************
*NAME:          statsLogConnection
*AUTHOR:        Andrew Roth
*CREATION DATE: 00/11/06
*LAST MODIFIED: 00/11/06
*PURPOSE:
* Logs a connection.  Files consist of a *.* for example
* home.com
*
*ARGUMENTS:
* 
*********************************************************/
void statsLogConnection(int connectionType, unsigned char ip[3]);

/*********************************************************
*NAME:          statsSaveGameStart
*AUTHOR:        Andrew Roth
*CREATION DATE: 00/11/06
*LAST MODIFIED: 00/11/06
*PURPOSE:
*  Saves the game info at "value" to the games.log, and returns
*  the file record index where it saved it
*
*ARGUMENTS:
*  gameIndex - The game's index to save
*********************************************************/
unsigned long statsSaveGameStart(currentGames value);


/*********************************************************
*NAME:          statsSaveGame
*AUTHOR:        Andrew Roth
*CREATION DATE: 00/11/06
*LAST MODIFIED: 00/11/06
*PURPOSE:
*  Saves game the game info at "value" to the games.log at
*  
*  info.
*
*ARGUMENTS:
*  currentGames - The game info address to save
*********************************************************/
void statsSaveGame(unsigned long origFileIndex, currentGames value);

/*********************************************************
*NAME:          statsSaveGame
*AUTHOR:        Andrew Roth
*CREATION DATE: 01/04/15
*LAST MODIFIED: 01/04/15
*PURPOSE:
*  Saves ending info for the game at "value" to offset offset
*
*ARGUMENTS:
*  value - The game info address to save
*********************************************************/
void statsSaveGameEnd(unsigned long offset, currentGames value);

#endif /* _STATS_H */

