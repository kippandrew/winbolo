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
*Name:          Server Transport
*Filename:      serverTransport.h
*Author:        John Morrison
*Creation Date: 11/8/99
*Last Modified: 12/1/00
*Purpose:
*  WinBolo Server Transport layer.
*********************************************************/

#ifndef SERVER_TRANSPORT_H
#define SERVER_TRANSPORT_H

#include "../bolo/global.h"

#ifdef _WIN32
#else 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


/* Array identifiers to get out IP from function */
#define IP_ARRAY0 0
#define IP_ARRAY1 1
#define IP_ARRAY2 2
#define IP_ARRAY3 3

/* Used to stop socket blocking */
#define NO_BLOCK_SOCK 1

// FIXME: Prototype
void serverTransportSendUDP(BYTE *buff, int len, struct sockaddr_in *addr);

/*********************************************************
*NAME:          serverTransportCreate
*AUTHOR:        John Morrison
*CREATION DATE: 11/8/99
*LAST MODIFIED: 10/9/00
*PURPOSE:
*  Creates the new servers transport layer. Returns FALSE 
*  if an error occured
*
*ARGUMENTS:
* port      - Port to run server on.
* addrToUse - The address to use if not NULL
*********************************************************/
bool serverTransportCreate(unsigned short port, char *addrToUse);

/*********************************************************
*NAME:          serverTransportDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 11/8/99
*LAST MODIFIED: 11/8/99
*PURPOSE:
*  Shuts down the server transport subsystem.
*
*ARGUMENTS:
*
*********************************************************/
void serverTransportDestroy();

/*********************************************************
*NAME:          serverTransportListenUDP
*AUTHOR:        John Morrison
*CREATION DATE: 13/8/99
*LAST MODIFIED: 13/8/99
*PURPOSE:
*  Listens for incoming UDP packets and processes them. 
*  Function exits upon an error caused by the sockUdp 
*  being closed.
*
*ARGUMENTS:
*
*********************************************************/
void serverTransportListenUDP(void);

/*********************************************************
*NAME:          serverTransportSetUs
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Sets up our IP address.
*
*ARGUMENTS:
*
*********************************************************/
void serverTransportSetUs(void);

/*********************************************************
*NAME:          serverTransportGetUs
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Gets our IP address and port.
*
*ARGUMENTS:
*  dest - Destination address
*  port - Destination port
*********************************************************/
void serverTransportGetUs(struct in_addr *dest, unsigned short *port);

/*********************************************************
*NAME:          serverTransportSendUDPLast
*AUTHOR:        John Morrison
*CREATION DATE: 15/08/99
*LAST MODIFIED: 19/08/02
*PURPOSE:
* Sends a packet to the last UDP connection made.
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*  wantCrc - Should we add a CRC to this?
*********************************************************/
void serverTransportSendUDPLast(BYTE *buff, int len, bool wantCrc);

/*********************************************************
*NAME:          serverTransportSetTracker
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Sets the trackers details up. Returns success.
*
*ARGUMENTS:
*  address - Address of the tracker
*  port    - Port of the tracker
*********************************************************/
bool serverTransportSetTracker(char *address, unsigned short port);

/*********************************************************
*NAME:          serverTransportSendUdpTracker
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
* Sends a udp packet to the tracker
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*********************************************************/
void serverTransportSendUdpTracker(BYTE *buff, int len);

#endif /* SERVER_TRANSPORT_H */
