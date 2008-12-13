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
*Name:          Net Players
*Filename:      Net Players.h
*Author:        John Morrison
*Creation Date: 26/02/99
*Last Modified: 12/02/03
*Purpose:
*  Handles keeping track of all players address 
*  in the game. Completely rewritten on 29/8/99 for second
*  networking attemp
*  Added udp packet support stuff on 24/02/02
*********************************************************/

#ifndef NET_PLAYERS_H
#define NET_PLAYERS_H


#include <time.h>
#ifdef _WIN32
  #include <winsock.h>
#else 
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

#include "global.h"
#include "udppackets.h"


/* Defines */
/* Check for removing every 30 seconds */
#define NET_PLAYERS_INACTIVEREMOVE_TIME (30) 

/* The player number is not found */
#define NET_PLAYERS_NOT_FOUND (0xAF) 

/* typedef struct netPlayersObj *netPlayers; */
typedef struct { /* Obj */
  bool inUse[MAX_TANKS]; 		/* Is this in use */
  bool inGame[MAX_TANKS]; 		/* Have they entered the game or are they still joining? */
  struct sockaddr_in addr[MAX_TANKS]; 	/* Last packet from address */
  bool locked[MAX_TANKS];		/* Are they locked */
  bool passed[MAX_TANKS];		/* Have they entered the password (if required by the server) */
  udpPackets udpp[MAX_TANKS];		/* udppackets ADT reference */
  time_t lastHeard[MAX_TANKS]; 		/* When we last heard from them */
  time_t lastServerTime[MAX_TANKS];
  time_t lastClientTime[MAX_TANKS];
  BYTE cheatCount[MAX_TANKS];
} netPlayers;



/* Prototypes */

/*********************************************************
*NAME:          netPlayersSetPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/00
*PURPOSE:
* Sets up the netPlayers item for playerNum. Sets the last
* heard time to now
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*  addr      - Address structure of client
*********************************************************/
void netPlayersSetPlayer(netPlayers *value, BYTE playerNum, struct sockaddr_in *addr);


/*********************************************************
*NAME:          netPlayersCreate
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 29/8/99
*PURPOSE:
* Creates an netPlayers struncture
*
*ARGUMENTS:
*
*********************************************************/
void netPlayersCreate(netPlayers *np);

/*********************************************************
*NAME:          netPlayersDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 29/8/99
*PURPOSE:
* Destroys a netPlayers structure
*
*ARGUMENTS:
*  value - The netPlayers structure to destroy
*********************************************************/
void netPlayersDestroy(netPlayers *value);

/*********************************************************
*NAME:          netPlayersHasPassed
*AUTHOR:        John Morrison
*CREATION DATE: 11/6/00
*LAST MODIFIED: 11/6/00
*PURPOSE:
* Returns whether this socket has been past the password
*
*ARGUMENTS:
*  value     - The netPlayers structure to check
*  playerNum - The player number
*********************************************************/
bool netPlayersHasPassed(netPlayers *value, BYTE playerNum);

/*********************************************************
*NAME:          netPlayersDonePassword
*AUTHOR:        John Morrison
*CREATION DATE: 11/6/00
*LAST MODIFIED: 11/6/00
*PURPOSE:
* This player has passed the password check. Let them 
* proceed
*
*ARGUMENTS:
*  value     - The netPlayers structure to set
*  playerNum - The player number to set
*********************************************************/
void netPlayersDonePassword(netPlayers *value, BYTE playerNum);

/*********************************************************
*NAME:          netPlayersRemovePlayerNum
*AUTHOR:        John Morrison
*CREATION DATE: 29/8/99
*LAST MODIFIED: 29/8/99
*PURPOSE:
* Removes the player Number for a specific socket (Sets
* it to available)
*
*ARGUMENTS:
*  value     - The netPlayers structure to add to
*  playerNum - The socket Number to set
*********************************************************/
BYTE netPlayersRemovePlayerNum(netPlayers *value, BYTE playerNum);

/*********************************************************
*NAME:          netPlayersIsLocked
*AUTHOR:        John Morrison
*CREATION DATE: 1/6/00
*LAST MODIFIED: 1/6/00
*PURPOSE:
* Returns if the clients have locked the game or not
*
*ARGUMENTS:
*  value     - The netPlayers structure to check
*********************************************************/
bool netPlayersIsLocked(netPlayers *value);

/*********************************************************
*NAME:          netPlayersSetLock
*AUTHOR:        John Morrison
*CREATION DATE: 1/6/00
*LAST MODIFIED: 1/6/00
*PURPOSE:
* Sets the game lock flag for a particular socket.
*
*ARGUMENTS:
*  value     - The netPlayers structure to add to
*  sockNum   - The socket Number to set
*  locked    - The lock state to set
*********************************************************/
void netPlayersSetLock(netPlayers *value, BYTE playerNum, bool locked);

/*********************************************************
*NAME:          netPlayersGetPlayerNumber
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/00
*PURPOSE:
* Returns the player matching this IP address and port
* Returns NET_PLAYERS_NOT_FOUND if it doesn't match any
* Updates the last heard time to now if it matches a
* player
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*  addr      - IP Address as long
*  port      - Port of connect (in network byte order)
*********************************************************/
BYTE netPlayersGetPlayerNumber(netPlayers *value, unsigned long addr, unsigned short port);

/*********************************************************
*NAME:          netPlayersGetInUse
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/00
*PURPOSE:
* Returns if this player is in use or not
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*********************************************************/
bool netPlayersGetInUse(netPlayers *value, BYTE playerNum);


/*********************************************************
*NAME:          netPlayersGetAddr
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/00
*PURPOSE:
* Returns the socket_addr_in structure for playerNum
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*********************************************************/
struct sockaddr_in *netPlayersGetAddr(netPlayers *value, BYTE playerNum);

/*********************************************************
*NAME:          netPlayersSetAddr
*AUTHOR:        John Morrison
*CREATION DATE: 12/02/03
*LAST MODIFIED: 12/02/03
*PURPOSE:
* Sets the socket_addr_in structure for playerNum
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*  addr      - Address structure of client
*********************************************************/
void netPlayersSetAddr(netPlayers *value, BYTE playerNum, struct sockaddr_in *addr);

/*********************************************************
*NAME:          netPlayersSetInGame
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/00
*PURPOSE:
* Sets the in game flag for this player
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*  set       - Value to set to
*********************************************************/
void netPlayersSetInGame(netPlayers *value, BYTE playerNum, bool set);

/*********************************************************
*NAME:          netPlayersGetInGame
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/00
*PURPOSE:
* Returns if a player is in game or not
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*********************************************************/
bool netPlayersGetInGame(netPlayers *value, BYTE playerNum);

/*********************************************************
*NAME:          netPlayersGetUdpPackets
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/00
*PURPOSE:
* Returns the UDP Packets for player playerNum
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*********************************************************/
udpPackets netPlayersGetUdpPackets(netPlayers *value, BYTE playerNum);

/*********************************************************
*NAME:          netPlayersGetPlayerDataReq
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/00
*PURPOSE:
* Gets the address and port for player playerNum
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*  addr      - Address structure
*  port      - Port
*********************************************************/
void netPlayersGetPlayerDataReq(netPlayers *value, BYTE playerNum, struct in_addr *addr, unsigned short *port);

/*********************************************************
*NAME:          netPlayersShouldDisconnectPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/00
*PURPOSE:
* Returns if we should disconnect playerNum after they
* have been idle for the required time
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*********************************************************/
bool netPlayersShouldDisconnectPlayer(netPlayers *value, BYTE playerNum);

/*********************************************************
*NAME:          netPlayersCheck
*AUTHOR:        John Morrison
*CREATION DATE: 09/02/04
*LAST MODIFIED: 09/02/04
*PURPOSE:
* Returns if we should disconnect a player for cheating
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*  playerNum - The player number
*  t         - Time from packet
*  ourTime   - Time from the server
*********************************************************/
bool netPlayersCheck(netPlayers *value, BYTE playerNum, time_t t, time_t ourTime);

/*********************************************************
*NAME:          netPlayersSetCheater
*AUTHOR:        John Morrison
*CREATION DATE: 09/02/04
*LAST MODIFIED: 09/02/04
*PURPOSE:
* Sets this player to be a cheater so they can be removed
*
*ARGUMENTS:
*  value     - The netPlayers structure 
*********************************************************/
void netPlayersSetCheater(netPlayers *value, BYTE playerNum);

#endif /* NET_PLAYERS_H */
