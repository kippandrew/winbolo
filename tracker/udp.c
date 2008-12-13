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
*Filename:      udp.c
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for handling incoming UDP game update 
*  packets
*********************************************************/


#ifdef _WIN32
/* Windows winsock */
#include <winsock2.h>
#include <windows.h>
#include <memory.h>

#else
/* Liunux Winsock plus some useful macros */
#include        <string.h>  
#include        <netinet/in.h>
#define closesocket(X) close(X)
typedef struct hostent  HOSTENT;
#define SD_BOTH 2
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif

#include <stdio.h>
#include "global.h"
#include "util.h"
#include "currentgames.h"
#include "bans.h"
#include "udp.h"
#include "stats.h" /* for udpMaxThreads */

#define	UDP_MAX_THREADS	10

SOCKET udpSocket = SOCKET_ERROR;
udpThreadInfoStruct *udpThreadInfo[UDP_MAX_THREADS];
FILE *INFOPACKET_LOG;

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
SOCKET udpCreate(unsigned short port) {
  udpSocket = utilMakeBoundSocket(FALSE, FALSE, FALSE, port);
  udpNumThreads = 0;
  INFOPACKET_LOG = fopen("games.log", "w+");

  return udpSocket;
}

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
void udpDestroy() {
	int i;
  if (udpSocket != SOCKET_ERROR) {
    shutdown(udpSocket, SD_BOTH);
    closesocket(udpSocket);
    udpSocket = SOCKET_ERROR;

    for (i = 0; i < udpNumThreads; i++) {
	    pthread_mutex_destroy(&(udpThreadInfo[udpNumThreads]->mutex));
	    pthread_cond_destroy(&(udpThreadInfo[udpNumThreads]->signal));
	    free(&(udpThreadInfo[udpNumThreads]->signal));
    }
  }
}

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
void udpRead(currentGames *cg) {
  int len;
  char buff[FILENAME_MAX];
  int szlast;
  struct sockaddr_in last;

  szlast = sizeof(last);

  /*printf("Reading from udp socket\n");*/
  len = recvfrom(udpSocket, buff, sizeof(buff), 0, (struct sockaddr *) &last, (socklen_t *) &szlast);
  while (len > 0) {
    fprintf(stderr,"UDP: Reading from udp socket ");
    if (len == sizeof(INFO_PACKET)) {
      fprintf(stderr, "[ip,");
      if (strncmp(buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0 && buff[BOLOPACKET_PACKET_TYPEPOS] == BOLOPACKET_INFORESPONSE) {
	      /* Process response */
	      fprintf(stderr, "assign,");
	      
	      /* print a line in the infopacket log file */
	      {
		       char mapName[MAP_STR_SIZE]; /* Name of the map */
		       char timeStr[MAP_STR_SIZE]; /* time str */
		       BYTE ip[4];
		       time_t currTime;
		       
		       memcpy(ip, & (last.sin_addr.s_addr), 4);
		       time(&currTime);
		       ctime_r(&currTime, timeStr);
		       timeStr[strlen(timeStr)-1] = 0;

		       utilPtoCString(((INFO_PACKET *)buff)->mapname, mapName);
		       fprintf(INFOPACKET_LOG, "[%s] %d.%d.%d.%d port=%d starttime=%ld map=%s\n", ctime(&currTime),
				       ip[0], ip[1], ip[2], ip[3],
				       ((INFO_PACKET *)buff)->gameid.serverport,
				       ((INFO_PACKET *)buff)->gameid.start_time,
				       mapName
			      );
		       fflush(INFOPACKET_LOG);
		       /*udpProcessInfoPacket(cg, (INFO_PACKET *) buff, &(last.sin_addr));*/
	      }

	      if (((INFO_PACKET *)buff)->gameid.serveraddress.s_addr == 0)
		      udpAssignToThread(cg, (INFO_PACKET *) buff, &(last.sin_addr));
	      else
		      fprintf(stderr, "not a server]\n");

	      /*fprintf(stderr, "start]\n");
	      pthread_create(&threadId, NULL, udpProcessInfoPacketThread, (void *)connInfo);*/
      }
    } else {
	    fprintf(stderr, "len %d wanted %d]\n", len, sizeof(INFO_PACKET));
    }
    len = recvfrom(udpSocket, buff, sizeof(buff), 0, (struct sockaddr *) &last, (socklen_t *) &szlast);
  }

  fprintf(stderr, "UDP: done\n");
}

/*********************************************************
*NAME:          udpAssignToThread
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/25 YMD
*LAST MODIFIED: 00/10/01
*PURPOSE:
*  Copies game info to a thread's info so it doesn't get
*  overwritten by another thread
*
*ARGUMENTS:
*  cg - a pointer to the currentgames structure
*  info - The info packet
*  pack - Address the packet came from
*  index - The thread index to copy the values to
*********************************************************/
void copyGameToThread(currentGames *cg, INFO_PACKET *info, struct in_addr *pack, int index) {
	if (udpThreadInfo[index] != NULL) {
		fprintf(stderr, "copyinfo,");
		udpThreadInfo[index]->cg = cg;
		memcpy(&(udpThreadInfo[index]->info), info, sizeof(udpThreadInfo[index]->info));
		memcpy(&(udpThreadInfo[index]->pack), pack, sizeof(udpThreadInfo[index]->pack));
	} else {
		fprintf(stderr, "UDP WARNING: udpThreadInfo[%d] is null while copying to it!", index);
	}
}

/*********************************************************
*NAME:          udpAssignToThread
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/10 YMD
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Assigns a connection to a thread and signals the thread
*
*ARGUMENTS:
*  cg - a pointer to the currentgames structure
*  info - The info packet
*  pack - Address the packet came from
*********************************************************/
void udpAssignToThread(currentGames *cg, INFO_PACKET *info, struct in_addr *pack) {
	int search, ret;
	pthread_t threadId;      /* Thread ID -- ignored but required for pthread_create */
	  
	for (search = 0; search < udpNumThreads; search++) {
		if (!(udpThreadInfo[search]->busy)) {
			udpThreadInfo[search]->busy = TRUE;
			copyGameToThread(cg, info, pack, search);
			pthread_cond_signal(&(udpThreadInfo[search]->signal));
			pthread_mutex_unlock(&(udpThreadInfo[search]->mutex));
			fprintf(stderr, "%d reuse]\n", search);
			return;
		} else
			fprintf(stderr, "%d-busy,", search);
	}
	
	if (udpNumThreads < UDP_MAX_THREADS) {
		/* start another thread... */
		udpThreadInfo[udpNumThreads] = (udpThreadInfoStruct *)malloc(sizeof(udpThreadInfoStruct));
		udpThreadInfo[udpNumThreads]->busy = TRUE;
		copyGameToThread(cg, info, pack, udpNumThreads);
		fprintf(stderr, "%d new]\n", udpNumThreads);
				
		pthread_cond_init(&(udpThreadInfo[udpNumThreads]->signal), NULL);
		pthread_mutex_init(&(udpThreadInfo[udpNumThreads]->mutex), NULL);

		ret = pthread_create(&threadId, NULL, udpThreadWaitForWork, (void *)udpNumThreads);
		udpNumThreads++;
	} else {
		/* wait until a thead is done */
	}

	
}

/*********************************************************
*NAME:          udpThreadWaitForWork
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/10 YMD
*LAST MODIFIED: 
*PURPOSE:
* Thread waits for a connection to be assigned to it
*
*ARGUMENTS:
* udpThreadIndex - the thread index as integer, >= 0
*********************************************************/
void *udpThreadWaitForWork(void *udpThreadIndex) {
	int myIndex = (int)udpThreadIndex;
	fprintf(stderr, "\tUDP: thread %d, ", myIndex);

	if (udpThreadInfo[myIndex]->busy) {
		fprintf(stderr, "startup\n");
		/* this must be initial startup, have work to do already! */
		udpProcessInfoPacket(udpThreadInfo[myIndex]->cg,
				&(udpThreadInfo[myIndex]->info),
				&(udpThreadInfo[myIndex]->pack));
	}

	while (TRUE) {
		/* wait for signal */
                fprintf(stderr, "\tUDP: thread %d sleep\n", myIndex);
		
		pthread_mutex_lock(&(udpThreadInfo[myIndex]->mutex));
		udpThreadInfo[myIndex]->busy = FALSE;
		pthread_cond_wait(&(udpThreadInfo[myIndex]->signal), &(udpThreadInfo[myIndex]->mutex));
		pthread_mutex_unlock(&(udpThreadInfo[myIndex]->mutex));

		fprintf(stderr, "\tUDP: thread %d wakeup\n", myIndex);

		/* reaches here means there is a signal to process 
		 * NOTE: udpAssignToThread will set busy to TRUE */
		udpProcessInfoPacket(udpThreadInfo[myIndex]->cg,
				&(udpThreadInfo[myIndex]->info),
				&(udpThreadInfo[myIndex]->pack));
	}
}

/*********************************************************
*NAME:          udpProcessInfoPacket
*AUTHOR:        John Morrison and Andrew
*CREATION DATE: 00/10/01
*LAST MODIFIED: 01/03/21
*PURPOSE:
* Processes a game packet
*
*ARGUMENTS:
*  cg   - Pointer to current games structure
*  info - The info packet
*  pack - Address the packet came from
*********************************************************/
void udpProcessInfoPacket(currentGames *cg, INFO_PACKET *info, struct in_addr *pack) {
  char mapName[MAP_STR_SIZE]; /* Name of the map */
  char version[MAP_STR_SIZE]; /* WinBolo Version */
  bool password;              /* Has Password */
  bool mines;                 /* Has mines */
  char address[FILENAME_MAX]; /* Address */

  fprintf(stderr, "\tUDP: [processinfo,\n");
  if (info->gameid.serveraddress.s_addr == 0) {
    /* Only servers */
    utilReverseLookup(pack, address);
    fprintf(stderr, "\t      ");
    if (bansExist(address) == FALSE) {
	    fprintf(stderr, "noban,");
	    utilPtoCString(info->mapname, mapName);
	    password = TRUE;
	    if (info->has_password == 0) {
		    password = FALSE;
	    }
	    mines = FALSE;
	    if (info->allow_mines == 0x80) {
		    mines = TRUE;
	    }
	    sprintf(version, "%d.%d%d", info->h.versionMajor, info->h.versionMinor, info->h.versionRevision);
	    /*FIXME: Debug line
	    fprintf(stderr,"\tUDP: Updating game  timelimit %ld  startdelay %ld\n", info->time_limit, info->start_delay);*/
	    if (info->h.versionMajor == 1 && info->h.versionMinor == 1  &&
			    (info->h.versionRevision == 1  || info->h.versionRevision == 2 || 
			    info->h.versionRevision == 3)) {
		    info->gameid.serverport = ntohs(info->gameid.serverport);
	    }
	    fprintf(stderr, "update,\n");
	    currentGamesUpdate(cg, address, info->gameid.serverport, mapName, version, (BYTE) info->num_players, (BYTE) info->free_bases, (BYTE) info->free_pills, mines, info->gametype, info->allow_AI, password, htonl(info->gameid.start_time), info->start_delay, info->time_limit, (unsigned char *)&(pack->s_addr));
	    fprintf(stderr, "\n\t      ");
	    fprintf(stderr, "done]\n");
    } else {
	    fprintf(stderr, "banned]\n");
    }
  } else {
	  fprintf(stderr, "not a server2 *** SHOULD NEVER REACH HERE]\n");
  }
}

