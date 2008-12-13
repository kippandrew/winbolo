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
*Name:          NetClient
*Filename:      NetClient.h
*Author:        John Morrison
*Creation Date: 20/2/99
*Last Modified:  4/6/00
*Purpose:
*  netClient TCP/UDP packet driver.
*********************************************************/

#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include "../bolo/global.h"
#ifdef _WIN32
#include <winsock.h>
#endif
#include "../bolo/network.h"
#include "currentgames.h"

/* Used to stop socket blocking */
#define NO_BLOCK_SOCK 1

/* Array identifiers to get out IP from function */
#define IP_ARRAY0 0
#define IP_ARRAY1 1
#define IP_ARRAY2 2
#define IP_ARRAY3 3

/* Want ping events to time out after 60 seconds */
#define TIME_OUT 6000

/* Prototypes */

/* FIXME */
bool netClientUdpPingServer(BYTE *buff, int *len, bool wantCrc, bool addNonReliable);

/*********************************************************
*NAME:          netClientCreate
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Creates the netClient. Returns whether the operation
* was sucessful or not
*
*ARGUMENTS:
*  port - The port to create on (0== dont care)
*********************************************************/
bool netClientCreate(unsigned short port);

/*********************************************************
*NAME:          netClientDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 30/8/99
*PURPOSE:
* Destroys the netClient.
*
*ARGUMENTS:
*
*********************************************************/
void netClientDestroy(void);

/*********************************************************
*NAME:          netClientCheck
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Checks for incoming packets. Process if exists
*
*ARGUMENTS:
*
*********************************************************/
void netClientCheck(void);

/*********************************************************
*NAME:          netClientSendUdpLast
*AUTHOR:        John Morrison
*CREATION DATE: 28/8/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sends a udp packet to the last address.
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*********************************************************/
void netClientSendUdpLast(BYTE *buff, int len);

/*********************************************************
*NAME:          netClientSendUdpServer
*AUTHOR:        John Morrison
*CREATION DATE: 28/8/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sends a udp packet to the server address.
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*********************************************************/
void netClientSendUdpServer(BYTE *buff, int len);

/*********************************************************
*NAME:          netClientGetServerAddress
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Copys the servers address and port
*
*ARGUMENTS:
*  dest - Destination address
*  port - Port of that corresponds to the address
*********************************************************/
void netClientGetServerAddress(struct in_addr *dest, unsigned short *port);

/*********************************************************
*NAME:          netClientGetServerAddressString
*AUTHOR:        John Morrison
*CREATION DATE: 1/9/99
*LAST MODIFIED: 1/9/99
*PURPOSE:
* Copys the servers address and port as a string.
*
*ARGUMENTS:
*  dest - Destination string
*********************************************************/
void netClientGetServerAddressString(char *dest);

/*********************************************************
*NAME:          netClientSetServerAddress
*AUTHOR:        John Morrison
*CREATION DATE: 24/2/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sets the servers address and port.
*
*ARGUMENTS:
*  src  - src address
*  port - Port of that corresponds to the address
*********************************************************/
void netClientSetServerAddress(struct in_addr *src, unsigned short port);

/*********************************************************
*NAME:          netClientSetServerPort
*AUTHOR:        John Morrison
*CREATION DATE: 12/1/00
*LAST MODIFIED: 12/1/00
*PURPOSE:
* Sets the servers port.
*
*ARGUMENTS:
*  port - Port of that corresponds to the address
*********************************************************/
void netClientSetServerPort(unsigned short port);

/*********************************************************
*NAME:          netClientSetUs
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sets up our IP address.
*
*ARGUMENTS:
*
*********************************************************/
void netClientSetUs(void);

/*********************************************************
*NAME:          netClientGetAddress
*AUTHOR:        John Morrison
*CREATION DATE: 11/4/01
*LAST MODIFIED: 11/4/01
*PURPOSE:
* Reverse looks up an ip address
*
*ARGUMENTS:
*  ip   - IP Address to look up
*  dest - Buffer to hold returnm
*********************************************************/
void netClientGetAddress(char  *ip, char *dest);

/*********************************************************
*NAME:          netClientGetUs
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 28/8/9921/2/99
*PURPOSE:
* Gets our IP address and port.
*
*ARGUMENTS:
*  dest - Destination address
*  port - Destination port
*********************************************************/
void netClientGetUs(struct in_addr *dest, unsigned short *port);

/*********************************************************
*NAME:          netClientGetUsStr
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 28/8/993/3/99
*PURPOSE:
* Gets our IP address and port as a string
*
*ARGUMENTS:
*  dest - Destination string
*********************************************************/
void netClientGetUsStr(char *dest);

/*********************************************************
*NAME:          netClientUdpPing
*AUTHOR:        John Morrison
*CREATION DATE: 24/02/99
*LAST MODIFIED: 19/08/02
*PURPOSE:
* Sends a packet and waits till a repsonse packet arrives
* Returns FALSE if it times out.
*
*ARGUMENTS:
*  buff - Packet to send (also returned)
*  len  - Length of the packet (also returned)
*  dest - Destination address
*  port - Destination port
*  wantCrc - TRUE if we should add a CRC to this packet
*  addNoReliable - If true adds the non-reliable packet
*                  marker to the packet
*********************************************************/
bool netClientUdpPing(BYTE *buff, int *len, char *dest, unsigned short port, bool wantCrc, bool addNonReliable);

/*********************************************************
*NAME:          netClientUdpSendNoWait
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sends a packet to the string address given
*
*ARGUMENTS:
*  buff - Packet to send
*  len  - Length of the packet
*  dest - Destination address
*  port - Destination port
*********************************************************/
void netClientSendUdpNoWait(BYTE *buff, int len, char *dest, unsigned short port);

/*********************************************************
*NAME:          netClientGetLastStr
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Copies the location of the last packet into a string
*
*ARGUMENTS:
*  dest - Destination string
*********************************************************/
void netClientGetLastStr(char *dest);

/*********************************************************
*NAME:          netClientGetLast
*AUTHOR:        John Morrison
*CREATION DATE: 23/12/99
*LAST MODIFIED: 23/12/99
*PURPOSE:
* Copies the location of the last packet.
*
*ARGUMENTS:
*  dest - Destination.
*  port - Destination port.
*********************************************************/
void netClientGetLast(struct in_addr  *dest, unsigned short *port);

/*********************************************************
*NAME:          netClientSetUseEvents
*AUTHOR:        John Morrison
*CREATION DATE: 5/6/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sets our netClient socket to use windows notificaion on 
* incoming packets. Returns sucess.
*
*ARGUMENTS:
*
*********************************************************/
bool netClientSetUseEvents(void);

/*********************************************************
*NAME:          netClientUdpCheck
*AUTHOR:        John Morrison
*CREATION DATE: 28/5/00
*LAST MODIFIED: 28/5/00
*PURPOSE:
* Thread function responsible for checking for UDP Data.
*
*ARGUMENTS:
*
*********************************************************/
void netClientUdpCheck(void);

/*********************************************************
*NAME:          netClientSetUdpAsync
*AUTHOR:        John Morrison
*CREATION DATE: 30/8/99
*LAST MODIFIED: 30/8/99
*PURPOSE:
* Sets the netClients UDP socket to be blocking/nonblocking
* Returns success.
*
*ARGUMENTS:
*  on - TRUE for non-block, FALSE for blocking
*********************************************************/
bool netClientSetUdpAsync(bool on);

/*********************************************************
*NAME:          netClientSetTracker
*AUTHOR:        John Morrison
*CREATION DATE: 30/8/99
*LAST MODIFIED: 30/8/99
*PURPOSE:
* Sets the trackers details up. Returns success.
*
*ARGUMENTS:
*  address - Address of the tracker
*  port    - Port of the tracker
*********************************************************/
bool netClientSetTracker(char *address, unsigned short port);

/*********************************************************
*NAME:          netClientSendUdpTracker
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
void netClientSendUdpTracker(BYTE *buff, int len);

#ifdef _WIN32

/*********************************************************
*NAME:          netClientFindTrackedGames
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/00
*LAST MODIFIED: 23/1/00
*PURPOSE:
* Looks up the tracker address and ports and downloads a
* list of current games. Returns success
*
*ARGUMENTS:
*  hWnd           - Window handle of the calling dialog
*  cg             - Pointer to the currentGames structure
*  trackerAddress - Address of the tracker to use
*  port           - Port of the tracker
*  motd           - Buffer to hold the message of the day
*********************************************************/
bool netClientFindTrackedGames(HWND hWnd, currentGames *cg, char *trackerAddress, unsigned short port, char *motd);

/*********************************************************
*NAME:          netClientFindBroadcastGames
*AUTHOR:        John Morrison
*CREATION DATE: 4/6/00
*LAST MODIFIED: 4/6/00
*PURPOSE:
* Gets a list of current games on you by using your 
* broadcast address.
*
*ARGUMENTS:
*  hWnd           - Window handle of the calling dialog
*  cg             - Pointer to the currentGames structure
*********************************************************/
bool netClientFindBroadcastGames(HWND hWnd, currentGames *cg);

#endif

#endif /* NET_CLIENT_H */
