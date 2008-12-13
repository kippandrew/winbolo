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
*Filename:      debug.h
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/02
*Purpose:
*  Responsable for handling of debug telnet connections
*********************************************************/

#ifndef _DEBUG_H
#define _DEBUG_H

#include "global.h"


/* Max string size */
#define DEBUG_MAX_SIZE 1024

/* 27 is the escape key in ASCII */
#define DEBUG_ESC_KEY 27
/* The tilde key */
#define DEBUG_TILDE_KEY '`'

#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define DebugTail(list) ((list)->next);

/* Our state */
typedef enum {
  debugWelcome,
  debugMonitor
} debugStates;

/* Each debug client consists of a socket identifier
 * and a text buffer for inputing */
typedef struct debugClientsObj *debugClients;
struct debugClientsObj {
  debugClients next;          /* Next item */
  debugClients prev;          /* Prev item */
  SOCKET sock;                /* socket    */
  BYTE state;                 /* Our state */
  char address[FILENAME_MAX]; /* Clients DNS and IP address */
  char txt[DEBUG_MAX_SIZE];   /* Text buffer */
  bool paused;                /* TRUE if paused, (no debug messages */
  int charReads;              /* Number of chars read this read */
};


/* The messages to print out */
#define DEBUG_WELCOME \
"==========================================\r\n\
Game Tracker Version 0.01 - Copyright 2000\r\n\
==========================================\r\n\
1 - Monitor Log & Debug\r\n\
2 - Flush Game List\r\n\
3 - Reload Tracker\r\n\
4 - Close Tracker\r\n\
5 - Quit\r\n\
==========================================\r\n"


#define DEBUG_MONITOR \
"\r\nMonitoring.  ` toggles pause.  New: Chat! (type slowly! :P)\r\n"


/* Message Prefixes */
#define DEBUG_MSG_PREFIX "DEBUG|Chat"


/* Prototypes */

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
SOCKET debugCreate(unsigned short port, char *fileName);

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
void debugDestroy();

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
void debugRead();

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
bool debugCheckAddress(struct sockaddr_in *addr);

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
void debugAddNewClient(SOCKET sock, struct in_addr *addr);

/*********************************************************
*NAME:          debugCheckClients
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Checks each of the connected debug clients for new data
*
*ARGUMENTS:
*
*********************************************************/
void debugCheckClients();

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
void debugSendClientsMessage(bool global, char *prefix, char *text, char *text2);

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
bool debugProcessClientData(debugClients client);

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
bool debugProcessMonitor(debugClients client);

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
bool debugProcessWelcome(debugClients client);

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
int debugGetHighestSocket();

#endif /* _DEBUG_H */

