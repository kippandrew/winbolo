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
*Name:          udp
*Filename:      udp.h
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for handling incoming UDP game update 
*  packets
*********************************************************/


#ifndef _UDP_H
#define _UDP_H

#ifdef _WIN32
/* Windows winsock */
#include <winsock2.h>
#include <windows.h>
#include <memory.h>

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
/* typedef struct hostent  HOSTENT; */
#define SD_BOTH 2

#endif


int udpMaxThreads;

/* Packet description */
#define BOLOPACKET_INFORESPONSE  14
/* "Bolo" string and size of */
#define BOLO_SIGNITURE "Bolo"
#define BOLO_SIGNITURE_SIZE 4
#define BOLOPACKET_PACKET_TYPEPOS 7

/* Bolo header packets */
typedef struct {
  BYTE signature[4];    /* 'Bolo'                */
  BYTE versionMajor;    /* BOLO_VERSION_MAJOR    */
  BYTE versionMinor;    /* BOLO_VERSION_MINOR    */
  BYTE versionRevision; /* BOLO_VERSION_REVISION */
  BYTE type;            /* Packet Type           */
} BOLOHEADER;

typedef struct {
  struct in_addr serveraddress;
  unsigned short serverport;
  unsigned long start_time;
} GAMEID;

typedef struct {
  BOLOHEADER h;

  char mapname[MAP_STR_SIZE]; /* Pascal string (first byte is length)         */
  GAMEID gameid;       /* 8 byte unique ID for game (combination       */
                       /* of starting machine address & timestamp)     */
  BYTE gametype;       /* Game type (1, 2 or 3: open, tourn. & strict) */
  BYTE allow_mines;    /* 0x80 for normal hidden mines                 */
                       /* 0xC0 for all mines visible                   */
  BYTE allow_AI;       /* 0 for no AI tanks, 1 for AI tanks allowed    */
  BYTE spare1;         /* 0                                            */
  long start_delay;    /* if non zero, time until game starts, (50ths) */
  long time_limit;     /* if non zero, time until game ends, (50ths)   */

  WORD num_players;    /* number of players                            */
  WORD free_pills;     /* number of free (neutral) pillboxes           */
  WORD free_bases;     /* number of free (neutral) refuelling bases    */
  BYTE has_password;   /* non-zero if game has password set            */
  BYTE spare2;         /* 0                                            */
} INFO_PACKET;


/* this structure stores mutex/signal and connection info for one thread */ 
typedef struct {
	int busy;
	currentGames *cg;
	INFO_PACKET info;
	struct in_addr pack;
	pthread_cond_t signal;
	pthread_mutex_t mutex;
} udpThreadInfoStruct;
int udpNumThreads;

/*********************************************************
*NAME:          udpCreate
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Sets up the udp module and returns the socket handle
*
*ARGUMENTS:
*  port - Port to bind to
*********************************************************/
SOCKET udpCreate(unsigned short port);

/*********************************************************
*NAME:          udpDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Cleansup after the udp module
*
*ARGUMENTS:
*
*********************************************************/
void udpDestroy();


/*********************************************************
*NAME:          udpRead
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Checks for incoming game data packets
*
*ARGUMENTS:
*  cg - Pointer to current games structure
*********************************************************/
void udpRead(currentGames *cg);

/*********************************************************
*NAME:          udpProcessInfoPacket
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Processes a game packet
*
*ARGUMENTS:
*  cg   - Pointer to current games structure
*  info - The info packet
*  pack - Address the packet came from
*********************************************************/
void udpProcessInfoPacket(currentGames *cg, INFO_PACKET *info, struct in_addr *pack);

/*********************************************************
*NAME:          udpAssignToThread
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/10 YMD
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Assigns a connection to a thread and signals the thread
*
*ARGUMENTS:
*  info - The info packet
*  pack - Address the packet came from
*********************************************************/
void udpAssignToThread(currentGames *cg, INFO_PACKET *info, struct in_addr *pack);

/*********************************************************
*NAME:          udpThreadWaitForWork
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/10 YMD
*LAST MODIFIED:
*PURPOSE:
* Processes a game packet
*
*ARGUMENTS:
* connectionInfoVoid - pointer to connInfo structure which
*  holds everything necessary to call udpProcessInfoPacket
*********************************************************/
void *udpThreadWaitForWork(void *connectionInfo);

/*********************************************************
*NAME:          udpThreadWaitForWork
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/10 YMD
*LAST MODIFIED:
*PURPOSE:
* Processes a game packet
*
*ARGUMENTS:
* connectionInfoVoid - pointer to connInfo structure which
*  holds everything necessary to call udpProcessInfoPacket
**********************************************************/
void *udpThreadSend(void *connectionInfo);

#endif /* _UDP_H */

