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

#include "currentgames.h"
#include "global.h"

#define	STATS_LOG_NEW_GAME	0
#define	STATS_LOG_GAME_UPDATE	1
#define	STATS_LOG_TELNET_LIST	2

#define	STATS_IP_DNS_SIZE	50
#define	STAT_DNS_SIZE		50
#define	STAT_LOG_FILE_SIZE	100

typedef struct IPstatsStruct IPstats;

typedef struct {
  /*struct in_addr *pack;*/
  unsigned char ip[IP_SIZE];
  char ipDNS[50];
  unsigned long stats[15];
} IPstatsStruct;
typedef struct {
  /*struct in_addr *pack;*/
  unsigned char ip[IP_SIZE];
  char ipDNS[50];
  time_t date;
  unsigned long stat;
} IPconnectStruct;

typedef struct {
  time_t date;
  unsigned char padding[1];
  unsigned short int numPlayers, maxPlayers;
  unsigned short int freePills, freeBases;
  unsigned short int minPills, minBases;
  unsigned short int spare1, spare2;
  bool timedOut;
} statsSaveEnd;

typedef struct {
  char mapName[MAP_STR_SIZE];
  char version[6];
  unsigned short port;
  BYTE numPlayers;
  BYTE numBases;
  BYTE numPills;
  bool mines;
  gameType game;
  aiType ai;
  bool password;
  unsigned long starttime;
  long startdelay;
  long timelimit;
} statsGameDetails;

typedef struct {
  time_t date;
  char ipdns[STATS_IP_DNS_SIZE];
  unsigned char ip[IP_SIZE];
  statsGameDetails det;
  statsSaveEnd end;
  char newLine[2];
} statsGameSave;

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

