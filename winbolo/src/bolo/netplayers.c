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
*Filename:      Net Players.c
*Author:        John Morrison
*Creation Date: 26/02/99
*Last Modified: 10/12/03
*Purpose:
*  Handles keeping track of all players address 
*  in the game. Completely rewritten on 29/8/99 for second
*  networking attempt
*  Added udp packet support stuff on 24/02/02
*********************************************************/

#include <string.h>
#include "global.h"
#include "udppackets.h"
#include "netplayers.h"
#include "backend.h"
#include "players.h"

/*********************************************************
*NAME:          netPlayersCreate
*AUTHOR:        John Morrison
*CREATION DATE: 26/02/99
*LAST MODIFIED: 16/12/08
*PURPOSE:
* Creates an netPlayers struncture
*
*ARGUMENTS:
*
*********************************************************/
void netPlayersCreate(netPlayers *np) {
  BYTE count;

  for (count=0;count<MAX_TANKS;count++) {
    (*np).inUse[count] = FALSE;
    (*np).inGame[count] = FALSE;
    (*np).locked[count] = FALSE ;
    (*np).passed[count] = FALSE;
	(*np).passedrsa[count] = FALSE;
    (*np).udpp[count] = udpPacketsCreate();
    (*np).cheatCount[count] = 0;
    /* Incoming must be incrememnent */
//    udpPacketsGetNextInSequenceNumber(&(*np).udpp[count]);
/* Fixme want to allocate on join    (*np).udpp = NULL; */
  }
}

/*********************************************************
*NAME:          netPlayersDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 26/02/99
*LAST MODIFIED: 24/02/02
*PURPOSE:
* Destroys a netPlayers structure
*
*ARGUMENTS:
*  value - The netPlayers structure to destroy
*********************************************************/
void netPlayersDestroy(netPlayers *value) {
  BYTE count;

  for (count=0;count<MAX_TANKS;count++) {
    if ((*value).udpp != NULL) {
      udpPacketsDestroy(&(*value).udpp[count]);
    }
    (*value).udpp[count] = NULL;
  }
  
}

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
bool netPlayersGetInUse(netPlayers *value, BYTE playerNum) {
  return (*value).inUse[playerNum];
}


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
struct sockaddr_in *netPlayersGetAddr(netPlayers *value, BYTE playerNum) {
  return &((*value).addr[playerNum]);
}

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
void netPlayersSetAddr(netPlayers *value, BYTE playerNum, struct sockaddr_in *addr) {
  if ((*value).inUse[playerNum] == TRUE) {
    memcpy(&((*value).addr[playerNum]), addr, sizeof(*addr));
  }
}

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
void netPlayersSetPlayer(netPlayers *value, BYTE playerNum, struct sockaddr_in *addr) {
  if ((*value).inUse[playerNum] == FALSE) {
    memcpy(&((*value).addr[playerNum]), addr, sizeof(*addr));
    (*value).addr[playerNum].sin_family = AF_INET;
    (*value).inUse[playerNum] = TRUE;
    (*value).inGame[playerNum] = FALSE;
    if ((*value).udpp[playerNum] != NULL) {
      udpPacketsDestroy(&(*value).udpp[playerNum]);
    }
    (*value).lastHeard[playerNum] = time(NULL);
    (*value).udpp[playerNum] = udpPacketsCreate();
    (*value).cheatCount[playerNum] = 0;
  }
}

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
BYTE netPlayersGetPlayerNumber(netPlayers *value, unsigned long addr, unsigned short port) {
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  
  returnValue = NET_PLAYERS_NOT_FOUND;
  count = 0;
  while (count < MAX_TANKS && returnValue == NET_PLAYERS_NOT_FOUND) {
    if ((*value).inUse[count] == TRUE) {
#ifdef _WIN32
      if ((*value).addr[count].sin_addr.S_un.S_addr == addr && (*value).addr[count].sin_port == port) {
#else
      if ((*value).addr[count].sin_addr.s_addr == addr && (*value).addr[count].sin_port == port) {
#endif
        returnValue = count;
        (*value).lastHeard[count] = time(NULL);
        if ((*value).cheatCount[count] > 1) {
          (*value).cheatCount[count] = (*value).cheatCount[count] - 1;
        }

      }
    }
    count++;
  }

  return returnValue;
}

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
void netPlayersSetInGame(netPlayers *value, BYTE playerNum, bool set) {
  (*value).inGame[playerNum] = set;
}

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
bool netPlayersGetInGame(netPlayers *value, BYTE playerNum) {
  bool returnValue; /* value to return */

  returnValue = FALSE;
  if ((*value).inUse[playerNum] == TRUE) {
    returnValue = (*value).inGame[playerNum];
  }
  return returnValue;
}


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
bool netPlayersHasPassed(netPlayers *value, BYTE playerNum) {
  return (*value).passed[playerNum];
}

/*********************************************************
*NAME:          netPlayersHasPassedRsa
*AUTHOR:        Min
*CREATION DATE: 16/12/08
*LAST MODIFIED: 16/12/08
*PURPOSE:
* Returns whether this socket has been past the RSA
*
*ARGUMENTS:
*  value     - The netPlayers structure to check
*  playerNum - The player number
*********************************************************/
bool netPlayersHasPassedRsa(netPlayers *value, BYTE playerNum) {
  return (*value).passedrsa[playerNum];
}

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
void netPlayersDonePassword(netPlayers *value, BYTE playerNum) {
  (*value).passed[playerNum] = TRUE;
}

/*********************************************************
*NAME:          netPlayersDoneRsa
*AUTHOR:        Min
*CREATION DATE: 16/12/08
*LAST MODIFIED: 16/12/08
*PURPOSE:
* This player has passed the Rsa check. Let them 
* proceed
*
*ARGUMENTS:
*  value     - The netPlayers structure to set
*  playerNum - The player number to set
*********************************************************/
void netPlayersDoneRsa(netPlayers *value, BYTE playerNum) {
  (*value).passedrsa[playerNum] = TRUE;
}

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
BYTE netPlayersRemovePlayerNum(netPlayers *value, BYTE playerNum) {
  (*value).inUse[playerNum] = FALSE;
  (*value).inGame[playerNum] = FALSE;
  (*value).locked[playerNum] = FALSE ;
  (*value).passed[playerNum] = FALSE;
  (*value).passedrsa[playerNum] = FALSE;
  udpPacketsDestroy(&((*value).udpp[playerNum]));
  (*value).udpp[playerNum] = NULL;
  return playerNum;
}

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
bool netPlayersIsLocked(netPlayers *value) {
  bool returnValue; /* Value to return */
  BYTE count; /* Looping variable */

  returnValue = TRUE;
  count = 0;
  while (count < MAX_TANKS && returnValue == TRUE) {
    if ((*value).inUse[count] == TRUE) {
      returnValue = (*value).locked[count];
    }
    count++;
  }
  return returnValue;
}

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
void netPlayersSetLock(netPlayers *value, BYTE playerNum, bool locked) {
  if ((*value).inUse[playerNum] == TRUE) {
    (*value).locked[playerNum] = locked;
  }
}

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
udpPackets netPlayersGetUdpPackets(netPlayers *value, BYTE playerNum) {
  udpPackets u;
  
  u = (*value).udpp[playerNum];
  return u;
}

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
void netPlayersGetPlayerDataReq(netPlayers *value, BYTE playerNum, struct in_addr *addr, unsigned short *port) {
  if ((*value).inUse[playerNum] == TRUE) {
    memcpy(addr, &((*value).addr[playerNum].sin_addr), sizeof(*addr));
    *port = (*value).addr[playerNum].sin_port;
  } else {
    memset(addr, 0, sizeof(*addr));
    *port = 0;
  }
}

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
bool netPlayersShouldDisconnectPlayer(netPlayers *value, BYTE playerNum) {
  bool returnValue = FALSE; /* Value to return */

  if ((*value).inUse[playerNum] == TRUE) {
    if (time(NULL) - (*value).lastHeard[playerNum] >= NET_PLAYERS_INACTIVEREMOVE_TIME) {
      returnValue = TRUE;
    }
  }

  return returnValue;
}


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
bool netPlayersCheck(netPlayers *value, BYTE playerNum, time_t t, time_t ourTime) {
  int clientDiff;
  int ourDiff;
  int diff;

  if ((*value).inUse[playerNum] == TRUE) {
    ourDiff = (int) ((ourTime) - (*value).lastServerTime[playerNum]);
    (*value).lastServerTime[playerNum] = ourTime;
    clientDiff = (int) (t - (*value).lastClientTime[playerNum]);
    (*value).lastClientTime[playerNum] = t;
    diff = ourDiff - clientDiff;
    
    if (diff < -1 || (*value).cheatCount[playerNum] > 100) {
 //     (*value).cheatCount[playerNum] += 10;
//      printf("incrementing... %d\n", (*value).cheatCount[playerNum]);
      if ((*value).cheatCount[playerNum] > 100) {
//        printf("Threshold too high...\n");
        return TRUE;
      }
    }
  }
  return FALSE;
}

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
void netPlayersSetCheater(netPlayers *value, BYTE playerNum) {
  if ((*value).inUse[playerNum] == TRUE) {
    (*value).cheatCount[playerNum] = 150;
  }
}
