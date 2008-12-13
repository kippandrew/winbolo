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
*Name:          interesting
*Filename:      interesting.c
*Author:        Andrew Roth based on tcp
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for handling of incoming interesting game list
*  connections
*********************************************************/


#ifdef _WIN32
	/* Windows winsock */
	#include <winsock2.h>
	#include <windows.h>
	#include <memory.h>
	#define INTERESTING_SEND_SIGNAL	0
#else
	/* Liunux Winsock plus some useful macros */
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>

	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#include <netdb.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>

	#define closesocket(X) close(X)
	typedef struct hostent  HOSTENT;
	#define SD_BOTH 2
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define interesting_SEND_SIGNAL	MSG_NOSIGNAL
#endif

#include "global.h"
#include "util.h"
#include "currentgames.h"
#include "bans.h"
#include "interesting.h"
#include "stats.h"
#include "tcp.h"
#include "main.h"

extern long statsHttp;
extern long statsGames;
extern long statsinteresting;
SOCKET interestingSocket = SOCKET_ERROR;

#define	NUM_THREADS	500

/*********************************************************
*NAME:          interestingCreate
*AUTHOR:        Andrew Roth based on tcp
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Sets up the interesting module and returns the socket handle
*
*ARGUMENTS:
*  port - Port to bind to
*********************************************************/
SOCKET interestingCreate(unsigned short port) {
  interestingSocket = utilMakeBoundSocket(TRUE, TRUE, FALSE, port);
  return interestingSocket;
}

/*********************************************************
*NAME:          interestingDestroy
*AUTHOR:        Andrew Roth based on tcp
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Cleansup after the interesting module
*
*ARGUMENTS:
*
*********************************************************/
void interestingDestroy() {
  if (interestingSocket != SOCKET_ERROR) {
    shutdown(interestingSocket, SD_BOTH);
    closesocket(interestingSocket);
    interestingSocket = SOCKET_ERROR;
  }
}

/*********************************************************
*NAME:          interestingRead
*AUTHOR:        Andrew Roth based on tcp & Andrew Roth
*CREATION DATE: 00/10/01
*LAST MODIFIED: 01/05/01
*PURPOSE:
* Checks the socket for new connections
*
*ARGUMENTS:
*  cg - Pointer to current games structure
*********************************************************/
void interestingRead(currentGames *cg) {
  int sock;                /* The new socket */
  struct sockaddr_in addr; /* Prev person packet came from  */
  int addrSize;            /* Size of the address structure */
    
  addrSize = sizeof(addr);
  sock = accept(interestingSocket, (struct sockaddr *) &addr, (socklen_t *) &addrSize);
  while (sock > 0) {
    /* Process the connect here */
    /* FIXME: Debug line */
    fprintf(stderr, "interesting: Incoming interesting connection request from: %s\n", inet_ntoa(addr.sin_addr));
    statsInteresting++;
    tcpSendGameData(cg, sock, &(addr.sin_addr), TCP_INTERESTING);

    statsLogConnection(STATS_LOG_TELNET_LIST, (BYTE *) &(addr.sin_addr));
    sock = accept(interestingSocket, (struct sockaddr *) &addr, (socklen_t *) &addrSize);
  }
}

