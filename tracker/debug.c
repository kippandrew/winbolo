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
*Name:          debug
*Filename:      debug.c
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for handling of debug telnet connections
*********************************************************/


#include <stdio.h>
#ifdef _WIN32
/* Windows winsock */
#include <winsock2.h>
#include <windows.h>
#include <memory.h>

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
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define closesocket(X) close(X)
typedef struct hostent  HOSTENT;
#define SD_BOTH 2

#endif

#include "global.h"
#include "util.h"
#include "currentgames.h"
#include "bans.h"
#include "main.h"
#include "debug.h"


/* Used to stop socket blocking */
#define NO_BLOCK_SOCK 1


SOCKET debugSocket = SOCKET_ERROR;
char debugAllowFile[FILENAME_MAX];
debugClients dbc;

/*********************************************************
*NAME:          debugCreate
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Sets up the debug module and returns the socket handle
*
*ARGUMENTS:
*  port     - Port to bind to
*  fileName - The allow file for debug connections
*********************************************************/
SOCKET debugCreate(unsigned short port, char *fileName) {
  dbc = NULL;
  debugSocket = utilMakeBoundSocket(TRUE, TRUE, FALSE, port);
  if (fileName == NULL) {
    debugAllowFile[0] = EMPTY_CHAR;
  } else {
    strcpy(debugAllowFile, fileName);
  }
  return debugSocket;
}

/*********************************************************
*NAME:          debugDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Cleansup after the debug module
*
*ARGUMENTS:
*
*********************************************************/
void debugDestroy() {
  debugClients q; /* Temp pointer */

  if (debugSocket != SOCKET_ERROR) {
    shutdown(debugSocket, SD_BOTH);
    closesocket(debugSocket);
    debugSocket = SOCKET_ERROR;
  }

  while (NonEmpty(dbc)) {
    mainRemoveSocket(dbc->sock);
    shutdown(dbc->sock, SD_BOTH);
    closesocket(dbc->sock);
    q = DebugTail(dbc);
    Dispose(dbc);
    dbc = q;
  }
}

/*********************************************************
*NAME:          debugGetHighestSocket
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Returns the highest socket in se by the debug subsystem
*
*ARGUMENTS:
*
*********************************************************/
int debugGetHighestSocket() {
  int returnValue; /* Value to return */
  debugClients q;  /* Temp Pointer */

  returnValue = debugSocket;
  mainAddSocket(debugSocket);
  q = dbc;
  while (NonEmpty(q)) {
    if ((int) q->sock > returnValue) {
      returnValue = (int) q;
    }
    mainAddSocket(q->sock);
    q = DebugTail(q);
  }

  return returnValue;
}


/*********************************************************
*NAME:          debugRead
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Checks the socket for new connections and all its 
* subclients
*
*ARGUMENTS:
*
*********************************************************/
void debugRead() {
  int sock;                /* The new socket */
  struct sockaddr_in addr; /* Prev person packet came from  */
  int addrSize;            /* Size of the address structure */

  addrSize = sizeof(addr);
  sock = accept(debugSocket, (struct sockaddr *) &addr, (socklen_t *) &addrSize);
  while (sock > 0) {
    /* Process the connect here */
    /* FIXME: Debug line */
    fprintf(stderr, "DEBUG: Incoming debug connection request from: %s\n", inet_ntoa(addr.sin_addr));
    if (debugCheckAddress(&addr) == TRUE) {
      debugAddNewClient(sock, (struct in_addr *)  &addr);
    } else {
      shutdown(sock, SD_BOTH);
      closesocket(sock);
    }
    sock = accept(debugSocket, (struct sockaddr *) &addr, (socklen_t *) &addrSize);
  }

  debugCheckClients();
}

/*********************************************************
*NAME:          debugCheckAddress
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Checks the address of a new debug client for allow 
* access to the remote debug telnet access
*
*ARGUMENTS:
*  addr - Address the socket came from  
*********************************************************/
bool debugCheckAddress(struct sockaddr_in *addr) {
  bool returnValue;        /* Value to return */
  char line[FILENAME_MAX]; /* Current line */
  FILE *fp;                /* File pointer */
  char *ip;                /* Ip address of the socket */

  returnValue = FALSE;
  fp = NULL;
  ip = inet_ntoa(addr->sin_addr);
  if (debugAllowFile[0] != EMPTY_CHAR) {
    fp = fopen(debugAllowFile, "r");
  }
  if (fp) {  
    while (!feof(fp) && returnValue == FALSE) {
      fgets(line, FILENAME_MAX, fp);
      if (line[strlen(line)-1] == '\n') {
        line[strlen(line)-1] = '\0';
      }
      if (strcmp(line, ip) == 0) {
        /* Found it */
        returnValue = TRUE;
      }
    }
    fclose(fp);
  }

  return returnValue;
}


/*********************************************************
*NAME:          debugAddNewClient
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Adds a new client to the list of active debug clients
*
*ARGUMENTS:
*  sock - New socket to add
*  addr - Address of the client
*********************************************************/
void debugAddNewClient(SOCKET sock, struct in_addr *addr) {
  debugClients q; /* New client */

  /* Set it to be non blocking */
#ifdef _WIN32
  long noBlock = NO_BLOCK_SOCK;
  ioctlsocket(sock, FIONBIO, &noBlock);
#else
   fcntl(sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL));
#endif
  mainAddSocket(sock);

  /* Add it */
  New(q);
  q->sock = sock;
  q->state = debugWelcome;
  q->paused = FALSE;
  utilReverseLookup(addr, q->address);
  strcat(q->address, " (");
  strcat(q->address, inet_ntoa(*addr));
  strcat(q->address, ") ");
  q->txt[0] = EMPTY_CHAR;
  q->prev = NULL;
  q->next = dbc;
  if (dbc != NULL) {
    dbc->prev = q;
  }
  dbc = q;

  /* Send the welcome */
  send(q->sock, DEBUG_WELCOME, sizeof(DEBUG_WELCOME), 0);
}

/*********************************************************
*NAME:          debugCheckClients
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Checks each of the connected debug clients for new data
*
*ARGUMENTS:
*
*********************************************************/
void debugCheckClients() {
  debugClients q, del; /* Temp Pointers */
  int oldLen;          /* Length of the current string */
  int dataLen;         /* Length of the read */
  bool remove;         /* Do we remove this item */

  q = dbc;
  while (NonEmpty(q)) {
    remove = FALSE;
    oldLen = strlen(q->txt);
    dataLen = recv(q->sock, (q->txt + oldLen), DEBUG_MAX_SIZE - oldLen, 0);
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
    } else {
      q->txt[oldLen + dataLen] = EMPTY_CHAR;
      q->charReads = dataLen;
      /* Process the new data */
      remove = debugProcessClientData(q);
    }

    /* Remove the item */
    if (remove == TRUE) {
      mainRemoveSocket(q->sock);
      shutdown(q->sock, SD_BOTH);
      closesocket(q->sock);      
      if (q->prev != NULL) {
        q->prev->next = q->next;
      } else {
        dbc = q->next;
      }
      if (q->next != NULL) {
        q->next->prev = q->prev;
      }
      del = q;
      q = DebugTail(q);
      Dispose(del);
    } else {
      q = DebugTail(q);
    }
  }
}


/*********************************************************
*NAME:          debugSendClientsMessage
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* Sends a message to all clients
*
*ARGUMENTS:
*  global - If TRUE send to all clients regardless
*           of state, else only clients in debugMonitor
*  prefix - Prefix to send
*  text   - Text to send
*  text2  - Text to send (OPTIONAL: NULL for none)
*********************************************************/
void debugSendClientsMessage(bool global, char *prefix, char *text, char *text2) {
  debugClients q; /* Temp Pointer */
  int preLen;     /* Prefix length */
  int strLen;     /* String length */
  int strLen2;    /* String length */

  q = dbc;
  preLen = strlen(prefix);
  strLen = strlen(text);
  if (text2 != NULL) {
    strLen2 = strlen(text2);
  }
  while (NonEmpty(q)) {
    if (global == TRUE || (q->state == debugMonitor && q->paused == FALSE)) {
      send(q->sock, "\r\n", 2 ,0);
      send(q->sock, prefix, preLen, 0);
      send(q->sock, text, strLen, 0);
      if (text2 != NULL) {
        send(q->sock, text2, strLen2, 0);
      }
      send(q->sock, "\r\n", 2 ,0);
    }
    q = DebugTail(q);
  }
}


/*********************************************************
*NAME:          debugProcessClientData
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* The client has received some more data. Process it here
* based on our current state. Returns TRUE if the socket
* should be closed and removed.
* NOTE: can modify clients state and txt items
*
*ARGUMENTS:
*  client - The client being worked on
*********************************************************/
bool debugProcessClientData(debugClients client) {
  bool returnValue;

  returnValue = FALSE;
  switch (client->state) {
  case debugMonitor:
    returnValue = debugProcessMonitor(client);
    break;
  default: 
    /* debugWelcome: */
    returnValue = debugProcessWelcome(client);
  }
  return returnValue;
}

/*********************************************************
*NAME:          debugProcessMonitor
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* The client has received some more data. We are in the
* monitor state. Returns TRUE if the socket should be 
* closed and removed.
*
* NOTE: can modify clients state and txt items
* Valid options from this menu are:
* First char is ESC, goto main menu
* If there is a line break send the message to all 
* people in debug mode.
*
*ARGUMENTS:
*  client - The client being worked on
*********************************************************/
bool debugProcessMonitor(debugClients client) {
  if (client->txt[0] == DEBUG_ESC_KEY) {
    /* Goto main menu */
    client->state = debugWelcome;
    client->txt[0] = EMPTY_CHAR;
    send(client->sock, "\r\n", 2, 0);
    send(client->sock, DEBUG_WELCOME, sizeof(DEBUG_WELCOME), 0);    
  } else if (client->txt[0] == DEBUG_TILDE_KEY) {
    /* Pause */
    client->txt[0] = EMPTY_CHAR;
    client->paused = TRUE;
  } else if (client->txt[strlen(client->txt)-1] == '\n') {
    /* Send out debug message */
    debugSendClientsMessage(FALSE, DEBUG_MSG_PREFIX, client->address, client->txt);
    client->txt[0] = EMPTY_CHAR;
  } else {
    /* Echo it back to ourseleves */
    client->paused = FALSE;
    send(client->sock, client->txt + (strlen(client->txt) - client->charReads), client->charReads, 0);
  }
  return FALSE;
}

/*********************************************************
*NAME:          debugProcessClientData
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/02
*LAST MODIFIED: 00/10/02
*PURPOSE:
* The client has received some more data. We are in the
* welcome menu state. Returns TRUE if the socket should
* be closed and removed.
*
* NOTE: can modify clients state and txt items
* Valid options from this menu are:
* 1 - Monitor Log & Debug
* 2 - Flush Game List
* 3 - Reload Tracker
* 4 - Close Tracker
* 5 - Quit
*
*ARGUMENTS:
*  client - The client being worked on
*********************************************************/
bool debugProcessWelcome(debugClients client) {
  bool returnValue; /* Value to return */
  returnValue = FALSE;

  if (client->txt[0] == '5') {
    /* Quit */
    returnValue = TRUE;
    client->txt[0] = EMPTY_CHAR;
  } else if (client->txt[0] == '4') {
    mainTellQuit();
    client->txt[0] = EMPTY_CHAR;
    returnValue = TRUE;
  } else if (client->txt[0] == '3') {
    /* FIXME: Todo */
    client->txt[0] = EMPTY_CHAR;
    returnValue = TRUE;
  } else if (client->txt[0] == '2') {
    client->txt[0] = EMPTY_CHAR;
    mainFlushGamesList();
  } else if (client->txt[0] == '1') {
    /* FIXME: Todo */
    client->txt[0] = EMPTY_CHAR;
    client->state = debugMonitor;
    send(client->sock, DEBUG_MONITOR, sizeof(DEBUG_MONITOR), 0);    
  } else {
    /* Invalid option */
    client->txt[0] = EMPTY_CHAR;
    send(client->sock, "\r\n", 2, 0);
    send(client->sock, DEBUG_WELCOME, sizeof(DEBUG_WELCOME), 0);    
  }
  return returnValue;
}

