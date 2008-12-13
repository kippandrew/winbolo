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
*Filename:      tcp.c
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for handling of incoming TCP game list
*  connections
*********************************************************/


#ifdef _WIN32
/* Windows winsock */
#include <winsock2.h>
#include <windows.h>
#include <memory.h>

#define TCP_SEND_SIGNAL	0
#else
/* Liunux Winsock plus some useful macros */
/*#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
*/

#include        <stdio.h>  
#include        <stdlib.h>  
#include        <netdb.h>  
#include        <sys/types.h>  
#include        <sys/time.h>  
#include        <sys/socket.h>  
#include        <stdio.h>  
#include        <time.h>  
#include        <signal.h>  
#include        <pthread.h>  
#include        <string.h>  
#include        <errno.h> 
#include        <netinet/in.h>
#include <fcntl.h>
#define closesocket(X) close(X)
typedef struct hostent  HOSTENT;
#define SD_BOTH 2
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

/* From Linux linux/socket.h: */
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL     0x4000  /* Do not generate SIGPIPE */
#endif


#define TCP_SEND_SIGNAL	MSG_NOSIGNAL

#endif

#include "global.h"
#include "util.h"
#include "currentgames.h"
#include "bans.h"
#include "tcp.h"
#include "stats.h"

extern long statsHttp;
extern long statsGames;
extern long statsTcp;
extern long statsInteresting;

SOCKET tcpSocket = SOCKET_ERROR;

#define	NUM_THREADS	500

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
SOCKET tcpCreate(unsigned short port) {
  tcpSocket = utilMakeBoundSocket(TRUE, TRUE, FALSE, port);
  return tcpSocket;
}

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
void tcpDestroy() {
  if (tcpSocket != SOCKET_ERROR) {
    shutdown(tcpSocket, SD_BOTH);
    closesocket(tcpSocket);
    tcpSocket = SOCKET_ERROR;
  }
}

/*********************************************************
*NAME:          tcpRead
*AUTHOR:        John Morrison & Andrew Roth
*CREATION DATE: 00/10/01
*LAST MODIFIED: 01/05/01
*PURPOSE:
* Checks the socket for new connections
*
*ARGUMENTS:
*  cg - Pointer to current games structure
*********************************************************/
void tcpRead(currentGames *cg) {
  int sock;                /* The new socket */
  struct sockaddr_in addr; /* Prev person packet came from  */
  int addrSize;            /* Size of the address structure */
    
  addrSize = sizeof(addr);
  sock = accept(tcpSocket, (struct sockaddr *) &addr, (socklen_t *) &addrSize);
  while (sock > 0) {
    /* Process the connect here */
    /* FIXME: Debug line */
    fprintf(stderr, "TCP: Incoming tcp connection request from: %s\n", inet_ntoa(addr.sin_addr));
    statsTcp++;
    tcpSendGameData(cg, sock, &(addr.sin_addr), TCP_ALL);

    statsLogConnection(STATS_LOG_TELNET_LIST, (BYTE *) &(addr.sin_addr));
    sock = accept(tcpSocket, (struct sockaddr *) &addr, (socklen_t *) &addrSize);
  }
}


#define TCP_GAMES_HEADER  "TVERSION=1\r\n"
#define TCP_SIZEOFGAMES_HEADER strlen(TCP_GAMES_HEADER ) 

#define BIGSTRLEN 2048

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
*  interesting games, TCP_ALL sends all games (currently
*  only these two are allowed)
*********************************************************/
void tcpSendGameData(currentGames *cg, int sock, struct in_addr *pack, int filter) {
  int numGamesTotal, numGames;   /* number of registerd games, numGames contains
				    numGamesTotal if filter is TCP_ALL, otherwise
				    if filter is TCP_INTERESTING it contains the
				    number of interesting games */
  int count, sendCount;   /* Looping variables */
  char str[BIGSTRLEN];    /* Temp string */
  /* Game info */
  char address[FILENAME_MAX];
  unsigned short port;
  char mapName[FILENAME_MAX];
  char version[FILENAME_MAX];
  BYTE numPlayers;
  BYTE numBases;
  BYTE numPills;
  bool mines;
  gameType game;
  aiType ai;
  bool password;
  unsigned long starttime;
  BYTE startTimeBytes[4]; /* startime -> startTimeBytes converted later on -A */
  long startdelay;
  long timelimit;
  
  /* Strings to send */
  char gameStr[FILENAME_MAX];
  char aiStr[FILENAME_MAX];
  address[0] = '\0';

  /*utilReverseLookup(pack, address);*/
  if (bansExist(address) == FALSE) {
    count = 1;   /* hmm, I don't like switching to low-end of 1 all of a sudden  --Andrew */
    sendCount = 0;
    
    fprintf(stderr, "TCP: [.");

    /* Make header */
    send(sock, TCP_GAMES_HEADER, TCP_SIZEOFGAMES_HEADER, TCP_SEND_SIGNAL);
    fprintf(stderr, ".");fflush(stderr);
    
    /* FIXME: MOTD */
    if (filter == TCP_INTERESTING) {
	    strcpy(aiStr, "MOTDL=10\r\n");
	    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
    } else {
	    strcpy(aiStr, "MOTDL=6\r\n");
	    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
    }
    strcpy(aiStr, "MOTD=This is a simple tracker\r\n");
    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
    if (filter == TCP_INTERESTING) {
	    strcpy(aiStr, "MOTD=\r\n");
	    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
	    strcpy(aiStr, "MOTD=This is the INTERESTING games list, where interesting means\r\n");
	    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
	    strcpy(aiStr, "MOTD=non-PW, < 6 players, and unclaimed bases (or open game).\r\n");
	    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
	    strcpy(aiStr, "MOTD=\r\n");
	    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
    }
    sprintf(aiStr, "MOTD=Tracker Statistics\r\n");
    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
    sprintf(aiStr, "MOTD=Number of Tracked Games: %li\r\n", statsGames);
    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
    sprintf(aiStr, "MOTD=Number of Tracker Game List Requests: %li\r\n", statsTcp);
    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
    sprintf(aiStr, "MOTD=Number of Tracker Interesting Game List Requests: %li\r\n", statsInteresting);
    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);
    sprintf(aiStr, "MOTD=Number of Tracker Web Requests: %li\r\n", statsHttp);
    send(sock, aiStr, strlen(aiStr), TCP_SEND_SIGNAL);

    fprintf(stderr, ".");fflush(stderr);

    /* from here on we need access to the currentGames structure */
    currentGamesLock();

    /* Number of games */
    numGamesTotal = currentGamesItemCount(cg);
    if (filter == TCP_INTERESTING) {
	    /* ok this is inefficient.. but whatever, shouldn't be noticeable
	     * I have to do this cause of the stupid "send num games before
	     * game data".. so i gotta count the interesting games first
	    */
	    numGames = 0;
	    count = 1;
	    while (count <= numGamesTotal) {
		    currentGamesGetItem(cg, numGamesTotal-count+1, (char *) address, &port, (char *) mapName, (char *) version, &numPlayers, &numBases, &numPills, &mines, &game, &ai, &password, &starttime, &startdelay, &timelimit);
		    if ( password == FALSE &&
				    ((numPlayers < 6 &&
				    numBases > 0) ||
			 game == gameOpen))
			    numGames++;
		    count++;
	    }
    } else
	    numGames = numGamesTotal;

    sprintf(str, "NGAMES=%d\r\n", numGames);
    send(sock, str, strlen(str), TCP_SEND_SIGNAL);
    fprintf(stderr, ".");fflush(stderr);

    /* make each game */
    count = 1;
    while (count <= numGamesTotal) {   /* if filter == TCP_INTERESTING, even though i know the 
					  number of interesting games, i still have to loop
					  through all the games to in effect find them again! */
      currentGamesGetItem(cg, numGamesTotal-count+1, (char *) address, &port, (char *) mapName, (char *) version, &numPlayers, &numBases, &numPills, &mines, &game, &ai, &password, &starttime, &startdelay, &timelimit);
      /*printf("GAME INFO %d pass %d npl %d nbase %d\n",  numGamesTotal-count, password, numPlayers, numBases);*/
      
      switch (game) {
      case gameOpen:
        strcpy(gameStr, "Open");
        break;
      case gameTournament:
        strcpy(gameStr, "Tournament");
        break;
      default:
        strcpy(gameStr, "Strict");
      }
      switch (ai) {
      case aiNone:
        strcpy(aiStr, "no");
        break;
      case aiYes:
        strcpy(aiStr, "yes");
        break;
      default:
	strcpy(aiStr, "yesAdv");
      }
      /* convert startime to startTimeBytes -A */ 
      memcpy(startTimeBytes, &starttime, 4);

      if ( (filter == TCP_ALL) ||
		      ( filter == TCP_INTERESTING && 
			password == FALSE && 
			((numPlayers < 6 &&
			numBases > 0 ) || 
		      game == gameOpen))
	 ) {
	      sprintf(str, "GAME%.03d=%s:%d\r\nVERSION=%s\r\nMAP=%s\r\nTYPE=%s\r\nPLAYERS=%d\r\nBASES=%d\r\nPILLS=%d\r\nHIDMINES=%s\r\nPASSWORD=%s\r\nBRAINS=%s\r\nDELAY=%ld\r\nLIMIT=%ld\r\nSTARTTIME=%ld\r\n", sendCount, address, port, version, mapName, gameStr, numPlayers, numBases, numPills, TCP_TRUEFALSE_TO_YESNO(mines), TCP_TRUEFALSE_TO_YESNO(password), aiStr, startdelay, timelimit, starttime);
	      /* sprintf(str, "GAME%.03d=%s:%d\r\nVERSION=%s\r\nMAP=%s\r\nTYPE=%s\r\nPLAYERS=%d\r\nBASES=%d\r\nPILLS=%d\r\nHIDMINES=%s\r\nPASSWORD=%s\r\nBRAINS=%s\r\nDELAY=%ld\r\nLIMIT=%ld\r\nSTARTTIME=%c%c%c%c\r\n", count-1, address, port, version, mapName, gameStr, numPlayers, numBases, numPills, TCP_TRUEFALSE_TO_YESNO(mines), TCP_TRUEFALSE_TO_YESNO(password), aiStr, startdelay, timelimit, startTimeBytes[0], startTimeBytes[1], startTimeBytes[2], startTimeBytes[3]);*/
	      send(sock, str, strlen(str), TCP_SEND_SIGNAL); 
	      sendCount++;
	      fprintf(stderr, "#");fflush(stderr);
      }
      count++;
    }
  }

  /* we are finished with currentGames structure */
  currentGamesUnlock();
  
  /* Close the socket */
  shutdown(sock, SD_BOTH);
  fprintf(stderr, ".");fflush(stderr);
  closesocket(sock);

  fprintf(stderr, "] TCP: Send game data.  tcpSendGameData is finished.\n");
}

