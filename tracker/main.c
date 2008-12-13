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
*Name:          main
*Filename:      main.c
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/02
*Purpose:
*  Provides main glue
*********************************************************/

#ifdef _WIN32
/* Windows winsock */
#include <winsock2.h>
#include <windows.h>
#include <memory.h>
#else
/* Liunux Winsock plus some useful macros */
/*#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>*/
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
/*#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
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

#define closesocket(X) close(X)
typedef struct hostent  HOSTENT;
#define SD_BOTH 2

#endif




#include "global.h"
#include "currentgames.h"
#include "udp.h"
#include "tcp.h"
#include "http.h"
#include "debug.h"
#include "bans.h"
#include "main.h"
#include "irc.h"
#include "interesting.h"


currentGames cg; /* Current Games */
bool mainQuit;   /* Should the program quit */
SOCKET mainUdp, mainTcp, mainHttp, mainDebug, mainIrc, mainInteresting;


long statsGames, statsTcp, statsUdp, statsHttp, statsInteresting;

#ifdef _WIN32
FD_SET reads;
FD_SET errors;
#else
fd_set reads;
fd_set errors;
#endif


/*********************************************************
*NAME:          mainRemoveSocket
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Removes the socket from the read set
*
*ARGUMENTS:
*  sock - Socket to add
*********************************************************/
void mainRemoveSocket(SOCKET sock) {
  if (FD_ISSET(sock, &reads) != FALSE) {
    FD_CLR(sock, &reads);
    FD_CLR(sock, &errors);
  }
}


/*********************************************************
*NAME:          mainAddSocket
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Adds the socket from the read set
*
*ARGUMENTS:
*  sock - Socket to add
*********************************************************/
void mainAddSocket(SOCKET sock) {
  if (FD_ISSET(sock, &reads) == FALSE) {
    FD_SET(sock, &reads);
    FD_SET(sock, &errors);
  }
}

/*********************************************************
*NAME:          mainTellQuit
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Tells the main program to quit
*
*ARGUMENTS:
* 
*********************************************************/
void mainTellQuit() {
  mainQuit = TRUE;
}

/*********************************************************
*NAME:          mainFlushGamesList
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Flushes the current game list
*
*ARGUMENTS:
* 
*********************************************************/
void mainFlushGamesList() {
  currentGamesDestroy(&cg);
  cg = currentGamesCreate();
}

/*********************************************************
*NAME:          setup
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Main program setup. Sets up each subsystem and returns
* overall success
*
*ARGUMENTS:
* 
*********************************************************/
bool setup() {
  bool returnValue = TRUE; /* value to return */

#ifdef _WIN32
  /* For winsock */
  WSADATA wsaData;  /* Winsock create data */
  if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
    fprintf(stderr, "Error Starting Winsock\n");
    returnValue = FALSE;
  }
#endif

  statsGames = 0;
  statsTcp = 0;
  statsUdp = 0;
  statsHttp = 0;
  statsInteresting = 0;
  mainQuit = FALSE;
  FD_ZERO(&reads);
  FD_ZERO(&errors);
  
  cg = currentGamesCreate();
  bansCreate(NULL); /* FIXME: Load a file? */

  /* Udp */
  mainUdp = udpCreate(50000);
  if (mainUdp == INVALID_SOCKET) {
	  printf("Can't create udp socket\n");
    returnValue = FALSE;
  } else { 
    FD_SET(mainUdp, &reads);
  }

  /* Tcp */
  mainTcp = tcpCreate(50000);
  if (mainTcp == INVALID_SOCKET) {
	  printf("Can't create tcp socket\n");
    returnValue = FALSE;
  } else {
    mainAddSocket(mainTcp);
  }

  /* http */
  mainHttp = httpCreate(50005, NULL); /* FIXME: Filename */
  if (mainHttp == INVALID_SOCKET) {
	  printf("Can't create http socket\n");
    returnValue = FALSE;
  } else {
    mainAddSocket(mainHttp);
  }

  /* debug */
  mainDebug = debugCreate(1231, "debug.allow"); /* FIXME: Filename */
  if (mainDebug == INVALID_SOCKET) {
    returnValue = FALSE;
  } else {
    mainAddSocket(mainDebug);
  }
  /* debug */
  mainIrc = ircCreate(6670, "irc.allow"); /* FIXME: Filename */
  if (mainIrc == INVALID_SOCKET) {
	  returnValue = FALSE;
  } else {
	  mainAddSocket(mainIrc);
  }

  /* interesting games port */
  mainInteresting = interestingCreate(50001);
  if (mainInteresting == INVALID_SOCKET) {
	  returnValue = FALSE;
  } else {
	  mainAddSocket(mainInteresting);
  }
  
  return returnValue;
}


/*********************************************************
*NAME:          mainGetHighestSocket
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Returns highest socket identifier. Also resets the 
* fs sets
*
*ARGUMENTS:
* 
*********************************************************/
int mainGetHighestSocket() {
  int returnValue; /* Value to return */
  int test;        /* Test item */

  FD_ZERO(&reads);
  FD_ZERO(&errors);
  mainAddSocket(mainHttp);  
  mainAddSocket(mainUdp);  
  mainAddSocket(mainTcp);  
/*  mainAddSocket(mainIrc); */
  mainAddSocket(mainInteresting);

  returnValue = mainUdp;
  if ((int) mainTcp > returnValue) {
    returnValue = mainTcp;
  }
  if ((int) mainHttp > returnValue) {
    returnValue = mainHttp;
  }
/*  if ((int) mainIrc > returnValue) {
    returnValue = mainIrc;
  } */
  if ((int) mainInteresting > returnValue) {
    returnValue = mainInteresting;
  }

  test = debugGetHighestSocket();
  if (test > returnValue) {
    returnValue = test;
  }

  return returnValue;
}

/*********************************************************
*NAME:          destroy
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Main program destroy. Cleans up each subsystem 
*
*ARGUMENTS:
* 
*********************************************************/
void destroy() {
  udpDestroy();
  tcpDestroy();
  httpDestroy();
  debugDestroy();
  ircDestroy();

  currentGamesDestroy(&cg);
  bansDestroy(NULL); /* FIXME: Save to a file */
#ifdef _WIN32
  WSACleanup();
#endif

}

/*********************************************************
*NAME:          main
*AUTHOR:        John Morrison and Andrew Roth
*CREATION DATE: 00/10/01
*LAST MODIFIED: 01/05/01
*PURPOSE:
* Main program loop
*
*ARGUMENTS:
* 
*********************************************************/
int main(int argc, char **argv) {
  if (setup() == FALSE) {
    fprintf(stderr, "Error starting up\n");
    exit(0);
  }

  while (mainQuit == FALSE) {
    select(mainGetHighestSocket()+1, &reads, NULL, &errors, NULL);
/*    usleep(10); */
    fprintf(stderr, "MAIN: Broke from select\n");
    udpRead(&cg);
    tcpRead(&cg);
    interestingRead(&cg);
    httpRead(&cg);
    debugRead();
    ircRead();
    currentGamesPurge(&cg);
    fprintf(stderr, "MAIN: And back to sleep\n\n");
  }

  destroy();

  return 0;
}
