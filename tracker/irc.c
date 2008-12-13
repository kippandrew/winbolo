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
*Name:          irc
*Filename:      irc.c
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for handling of irc telnet connections
*********************************************************/


#include <stdio.h>
#ifdef _WIN32
	/* Windows winsock */
	#include <winsock2.h>
	#include <windows.h>
	#include <memory.h>

#else
	/* Liunux Winsock plus some useful macros */
	#include <pthread.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#include <netdb.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>
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

	#define IRC_SEND_SIGNAL        MSG_NOSIGNAL
#endif

#include "global.h"
#include "util.h"
#include "currentgames.h"
#include "bans.h"
#include "main.h"
#include "irc.h"

/* Used to stop socket blocking */
#define NO_BLOCK_SOCK 1


SOCKET ircSocket = SOCKET_ERROR;
ircClients ircc;
pthread_mutex_t ircMutex; /* protects access to the irc game list */

/*********************************************************
*NAME:          ircCreate
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/29 YMD
*LAST MODIFIED: 03/03/29
*PURPOSE:
* Sets up the irc listener port module and returns the
* socket handle
*
*ARGUMENTS:
*  port     - Port to bind to
*********************************************************/
SOCKET ircCreate(unsigned short port, char *fileName) {
  ircc = NULL;
  ircSocket = utilMakeBoundSocket(TRUE, TRUE, FALSE, port);
  pthread_mutex_init(&ircMutex, NULL);

  return ircSocket;
}

/*********************************************************
*NAME:          ircDestroy
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/29 YMD
*LAST MODIFIED: 03/03/29
*PURPOSE:
* Cleansup after the irc module
*
*ARGUMENTS:
*
*********************************************************/
void ircDestroy() {
  ircClients q; /* Temp pointer */

  if (ircSocket != SOCKET_ERROR) {
    shutdown(ircSocket, SD_BOTH);
    closesocket(ircSocket);
    ircSocket = SOCKET_ERROR;
  }

  pthread_mutex_lock(&ircMutex);

  while (NonEmpty(ircc)) {
    mainRemoveSocket(ircc->sock);
    shutdown(ircc->sock, SD_BOTH);
    closesocket(ircc->sock);
    q = IrcTail(ircc);
    Dispose(ircc);
    ircc = q;
  }

  pthread_mutex_unlock(&ircMutex);
  pthread_mutex_destroy(&ircMutex);
}

/*********************************************************
*NAME:          ircGetHighestSocket
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Returns the highest socket in se by the debug subsystem
*
*ARGUMENTS:
*
*********************************************************/
int ircGetHighestSocket() {
  int returnValue; /* Value to return */
  ircClients q;  /* Temp Pointer */

  pthread_mutex_lock(&ircMutex);
    
  returnValue = ircSocket;
  mainAddSocket(ircSocket);
  q = ircc;
  while (NonEmpty(q)) {
    if ((int) q->sock > returnValue) {
      returnValue = (int) q;
    }
    mainAddSocket(q->sock);
    q = IrcTail(q);
  }

  pthread_mutex_unlock(&ircMutex);
    
  return returnValue;
}


/*********************************************************
*NAME:          ircAddNewClient
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/29
*LAST MODIFIED: 03/03/29
*PURPOSE:
* Adds a new client to the list of active irc clients
*
*ARGUMENTS:
*  sock - New socket to add
*  addr - Address of the client
*********************************************************/
void ircAddNewClient(SOCKET sock, struct in_addr *addr) {
  ircClients q; /* New client */
  ircClients val;
  
  /* Set it to be non blocking */
#ifdef _WIN32
  long noBlock = NO_BLOCK_SOCK;
  ioctlsocket(sock, FIONBIO, &noBlock);
#else
   fcntl(sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL));
#endif
/*  mainAddSocket(sock);*/

  /* Add it at the beginning*/
  pthread_mutex_lock(&ircMutex);
  val = ircc;
  New(q);
  q->sock = sock;
  q->next = val;
  q->prev = NULL;
  if (val != NULL) {
	  val->prev = q;
  }
  ircc = q;
  pthread_mutex_unlock(&ircMutex);
  
  /*send(q->sock, IRC_SEND_TXT, strlen(IRC_SEND_TXT), 0);*/
}

/*********************************************************
*NAME:          ircRead
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/29 YMD
*LAST MODIFIED: 03/03/29
*PURPOSE:
* Checks the socket for new connections and all its
* subclients
*
*ARGUMENTS:
*
*********************************************************/
void ircRead() {
  int sock;                /* The new socket */
  struct sockaddr_in addr; /* Prev person packet came from  */
  int addrSize;            /* Size of the address structure */

  addrSize = sizeof(addr);
  sock = accept(ircSocket, (struct sockaddr *) &addr, (socklen_t *) &addrSize);
  while (sock > 0) {
    /* Process the connect here */
    /* FIXME: Debug line */
    fprintf(stderr, "IRC: Incoming irc listener connection request from: %s\n", inet_ntoa(addr.sin_addr));
/*    if (debugCheckAddress(&addr) == TRUE) {*/
      ircAddNewClient(sock, (struct in_addr *)  &addr);
/*    } else {
      shutdown(sock, SD_BOTH);
      closesocket(sock);
      }*/
    sock = accept(ircSocket, (struct sockaddr *) &addr, (socklen_t *) &addrSize);
  }

  /*ircCheckClients();*/
}

/*********************************************************
*NAME:          ircCheckClients
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Checks each of the connected irc clients for new data
*
*ARGUMENTS:
*
*********************************************************/
void ircCheckClients() {
  ircClients q;      /* Temp Pointers */
  int dataLen;            /* Length of the read */
  bool remove;            /* Do we remove this item */
  char txt[IRC_READ_LEN]; /* Um, yeah. */

  pthread_mutex_lock(&ircMutex);
    
  q = ircc;
  while (NonEmpty(q)) {
    remove = FALSE;
    dataLen = recv(q->sock, txt, IRC_READ_LEN, IRC_SEND_SIGNAL);
/*    fprintf(stderr, "IRC: checking client read %d err %d EAGAIN %d\n", dataLen, SOCKET_ERROR, EAGAIN)*/;
    if (dataLen == SOCKET_ERROR) {
      /* Error */
#ifdef _WIN32
      if (WSAGetLastError() != WSAEWOULDBLOCK) {
#else
      if (errno != EAGAIN) { 
#endif 
        remove = TRUE;
      }
    } else if (dataLen == 0) {
      /* Socket close gracefully */
      remove = TRUE;
    }

    /* Remove the item */
    if (remove == TRUE) {
	    ircRemoveClient(&q);
    } else {
	    q = IrcTail(q);
    }
  }
  
  pthread_mutex_unlock(&ircMutex);
}

/*********************************************************
 *NAME:          ircRemoveClient
 *AUTHOR:        Andrew Roth
 *CREATION DATE: 03/03/29 YMD
 *LAST MODIFIED: 03/03/29
 *PURPOSE:
 * Removes a client from client linked list
 *
 *ARGUMENTS:
 *  text   - Text to send
 *********************************************************/
void ircRemoveClient(ircClients *q2) {
	  ircClients q = *q2, del;
	  
	  fprintf(stderr, "IRC: removed client\n");
	  mainRemoveSocket(q->sock);
	  shutdown(q->sock, SD_BOTH);
	  closesocket(q->sock);      
	  if (q->prev != NULL) {
		  q->prev->next = q->next;
		  /*fprintf(stderr, "IRC: REM: removed nonfirst");*/
	  } else {
		  ircc = q->next;
		  /*fprintf(stderr, "IRC: REM: removed first one, ircc now %p", ircc);*/
	  }
	  if (q->next != NULL) {
		  q->next->prev = q->prev;
	  } /*else fprintf(stderr, "IRC: REM: next is null");*/
	  del = q;
	  *q2 = IrcTail(q);
	  Dispose(del);
  } 


/*********************************************************
 *NAME:          ircSendClientsMessage
 *AUTHOR:        Andrew Roth
 *CREATION DATE: 00/10/02
 *LAST MODIFIED: 00/10/02
 *PURPOSE:
 * Sends a message to all clients
 *
 *ARGUMENTS:
 *  text   - Text to send
 *********************************************************/
void ircSendClientsMessage(char *text) {
	ircClients q; /* Temp Pointer */
	int strLen;     /* String length */
	int r;

	/*ircCheckClients();*/
	pthread_mutex_lock(&ircMutex);

	q = ircc;
	strLen = strlen(text);
	fprintf(stderr, "IRC: send game to client [");
	while (NonEmpty(q)) {
		fprintf(stderr, "1");
		r = send(q->sock, text, strLen, IRC_SEND_SIGNAL);
		fprintf(stderr, "2");
		r = send(q->sock, "\r\n", 2 , IRC_SEND_SIGNAL);
		fprintf(stderr, "3");

		if (r == -1) {
			/*fprintf(stderr, "q before %d\n", q);*/
			ircRemoveClient(&q);
			fprintf(stderr, "<");
			/*fprintf(stderr, "q after %d\n", q);*/
		} else {
			q = IrcTail(q);
		}

		fprintf(stderr, "4|");
	}
	fprintf(stderr, "]");

	pthread_mutex_unlock(&ircMutex);
}

