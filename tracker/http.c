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
*Name:          http
*Filename:      http.c
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for handling of incoming HTTP game list
*  connections (web server)
*********************************************************/


#ifdef _WIN32
/* Windows winsock */
#include <pthread.h>
#include <winsock2.h>
#include <windows.h>
#include <memory.h>
#define HTTP_SEND_SIGNAL	0
#else
/* Liunux Winsock plus some useful macros */
/*#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
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



/* From Linux linux/socket.h: */
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL     0x4000  /* Do not generate SIGPIPE */
#endif

#define closesocket(X) close(X)
typedef struct hostent  HOSTENT;
#define SD_BOTH 2
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define HTTP_SEND_SIGNAL        MSG_NOSIGNAL

#endif

#define BIGSTRLEN 2048
#define	HTTP_GAMES_HEADER	"HTTP/1.1 200 OK\nContent-Type: text/html\n\n<HTML><BODY><meta http-equiv=\"pragma\" content=\"no-cache\"></meta>This is an html output of the game list<BR><BR>\r\n"
#define	HTTP_SIZEOFGAMES_HEADER	sizeof(HTTP_GAMES_HEADER)

/*#define MSG_NOSIGNAL    0*/

#include "main.h" /* for stats vars */
#include "global.h"
#include "util.h"
#include "currentgames.h"
#include "bans.h"
#include "http.h"
#include "currentgames.h"
#include "tcp.h"
#include "stats.h"
#include "udp.h" /* for udpNumStats */

#define	HTTP_MAX_THREADS	10
#define	BUFF_SIZE		4096

SOCKET httpSocket = SOCKET_ERROR;
httpThreadInfoStruct *httpThreadInfo[HTTP_MAX_THREADS];

/*********************************************************
*NAME:          httpCreate
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 03/03/15 Y/M/D
*PURPOSE:
* Creates the http server to show the games
*
*ARGUMENTS:
*  port     - Port to bind to
*  fileName - File name to load the files we can send
*             The format is: <web filename>:<path>
*********************************************************/
SOCKET httpCreate(unsigned short port, char *fileName) {
  httpSocket = utilMakeBoundSocket(TRUE, TRUE, FALSE, port);

  httpNumThreads = 0;

  /* FIXME: This doesn't load in the file yet. */
  return httpSocket;
}


/*********************************************************
*NAME:          httpAssignToThread
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/15/10 YMD
*LAST MODIFIED: 03/15/10 YMD
*PURPOSE:
* Assigns a connection to a thread and signals the thread
*
*ARGUMENTS:
*  cg - a pointer to the currentgames structure
*  info - The info packet
*  pack - Address the packet came from
*********************************************************/
void httpAssignToThread(currentGames *cg, int sock, struct in_addr *pack) {
        int search, ret;
        pthread_t threadId;      /* Thread ID -- ignored but required for pthread_create */

	fprintf(stderr, "HTTP: assign connection. [");

        for (search = 0; search < httpNumThreads; search++) {
                if (!(httpThreadInfo[search]->busy)) {
                        httpThreadInfo[search]->busy = TRUE;
                        httpThreadInfo[search]->cg = cg;
                        httpThreadInfo[search]->sock = sock;
                        httpThreadInfo[search]->pack = pack;
                        pthread_cond_signal(&(httpThreadInfo[search]->signal));
                        pthread_mutex_unlock(&(httpThreadInfo[search]->mutex));
                        fprintf(stderr, "%d reuse]\n", search);
                        return;
                } else
                        fprintf(stderr, "%d-busy,", search);
        }

        if (httpNumThreads < HTTP_MAX_THREADS) {
                /* start another thread... */
                httpThreadInfo[httpNumThreads] = (httpThreadInfoStruct *)malloc(sizeof(httpThreadInfoStruct));
                httpThreadInfo[httpNumThreads]->busy = TRUE;
                httpThreadInfo[httpNumThreads]->cg = cg;
                httpThreadInfo[httpNumThreads]->sock = sock;
                httpThreadInfo[httpNumThreads]->pack = pack;
                fprintf(stderr, "%d new]\n", httpNumThreads);

                pthread_cond_init(&(httpThreadInfo[httpNumThreads]->signal), NULL);
                pthread_mutex_init(&(httpThreadInfo[httpNumThreads]->mutex), NULL);

                ret = pthread_create(&threadId, NULL, httpThreadWaitForWork, (void *)httpNumThreads);
                httpNumThreads++;
        } else {
                /* wait until a thead is done */
        }


}

/*********************************************************
*NAME:          httpThreadWaitForWork
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/03/10 YMD
*LAST MODIFIED:
*PURPOSE:
* Thread waits for a connection to be assigned to it
*
*ARGUMENTS:
* httpThreadIndex - the thread index as integer, >= 0
*********************************************************/
void *httpThreadWaitForWork(void *httpThreadIndex) {
        int myIndex = (int)httpThreadIndex;
        fprintf(stderr, "\tHTTP[%d]: thread ", myIndex);

        if (httpThreadInfo[myIndex]->busy) {
                fprintf(stderr, "startup\n");
                /* this must be initial startup, have work to do already! */
                httpProcessNewRequest(myIndex,
				httpThreadInfo[myIndex]->cg,
                                httpThreadInfo[myIndex]->sock,
                                httpThreadInfo[myIndex]->pack);
        }

        while (TRUE) {
                /* wait for signal */
                fprintf(stderr, "\tHTTP[%d]: thread sleep\n", myIndex);

                pthread_mutex_lock(&(httpThreadInfo[myIndex]->mutex));
                httpThreadInfo[myIndex]->busy = FALSE;
                pthread_cond_wait(&(httpThreadInfo[myIndex]->signal), &(httpThreadInfo[myIndex]->mutex));
                pthread_mutex_unlock(&(httpThreadInfo[myIndex]->mutex));

                fprintf(stderr, "\tHTTP[%d]: thread wakeup\n", myIndex);

                /* reaches here means there is a signal to process
                 * NOTE: httpAssignToThread will set busy to TRUE */
                httpProcessNewRequest(myIndex,
				httpThreadInfo[myIndex]->cg,
                                httpThreadInfo[myIndex]->sock,
                                httpThreadInfo[myIndex]->pack);
        }
}

/*********************************************************
*NAME:          httpDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Cleansup after the http module
*
*ARGUMENTS:
*
*********************************************************/
void httpDestroy() {
	if (httpSocket != SOCKET_ERROR) {
		shutdown(httpSocket, SD_BOTH);
		closesocket(httpSocket);
		httpSocket = SOCKET_ERROR;
	}
}

/*********************************************************
*NAME:          httpRead
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 11/1/00 by Andrew Roth
*PURPOSE:
* Checks the socket for new connections
*
*ARGUMENTS:
*  cg - Pointer to current games structure
*********************************************************/
void httpRead(currentGames *cg) {
  int sock;                /* The new socket */
  struct sockaddr_in addr; /* Prev person packet came from  */
  int addrSize;            /* Size of the address structure */
  
  addrSize = sizeof(addr);
  sock = accept(httpSocket, (struct sockaddr *) &addr, (socklen_t *) &addrSize); /* third argument was wrong */
  while (sock > 0) {
    /* Process the connect here */
    /* FIXME: Debug line */
    fprintf(stderr, "HTTP: Incoming http connection request from: %s\n", inet_ntoa(addr.sin_addr));
    statsHttp++;

    httpAssignToThread(cg, sock, (struct in_addr *)&addr);

    sock = accept(httpSocket, (struct sockaddr *) &addr, (socklen_t *) &addrSize);
  }
}

/*********************************************************
*NAME:          httpProcessNewRequest
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Processes the request
*
*ARGUMENTS:
*  threadID - thread index
*  cg   - Pointer to current games structure
*  sock - The socket to send to
*  pack - Address the packet came from
*********************************************************/
void httpProcessNewRequest(int threadID, currentGames *cg, int sock, struct in_addr *pack) {
  char address[FILENAME_MAX];

  fprintf(stderr, "\tHTTP[%d] process new request\n", threadID);

  httpReceiveGet(sock, httpThreadInfo[threadID]->httpGet);

  /*utilReverseLookup(pack, address);*/
  if (bansExist(address) == FALSE) {
/* FIXME: 2. Switching on type
    2.1 If the filename is in the list httpSendFile
    2.2 If it is /stats send call httpStats
    2.3 If it is /history send call httpHistory
    tpSend
    3. Calling stats function
*/
	  httpSendGameData(threadID, cg, sock, pack);
  }

  shutdown(sock, SD_BOTH);
  fprintf(stderr, ".");fflush(stderr);
  closesocket(sock);

}

/*********************************************************
*NAME:          httpReceiveGet
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/02/26 YMD
*LAST MODIFIED: 03/02/26
*PURPOSE:
* Receives data from the socket for GET / command in http
*
*ARGUMENTS:
*  sock - the socket to read from
*  buff - pointer to the buffer to read to
*********************************************************/
int httpReceiveGet(int sock, char *buff) {
	int len;

	while (TRUE) {
		len = recv(sock, buff, BUFF_SIZE-1, 0);
		if (len == 0)
			return FALSE;
		if (strstr(buff, "get") != NULL || strstr(buff, "GET") != NULL)
			return TRUE;
	}
}

/*********************************************************
*NAME:          httpSendGameData
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/02/26 YMD
*LAST MODIFIED: 03/02/26
*PURPOSE:
* Sends the game list to an http socket
*
*ARGUMENTS:
*  threadID - thread index
*  cg   - Pointer to current games structure
*  sock - The socket to send to
*  pack - Address the packet came from
*********************************************************/
void httpSendGameData(int threadID, currentGames *cg, int sock, struct in_addr *pack) {
  int numGames;           /* number of registerd games */
  int count;              /* Looping variable */
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

    fprintf(stderr, "\tHTTP[%d]: [.", threadID);

    /* Make header */
    send(sock, HTTP_GAMES_HEADER, HTTP_SIZEOFGAMES_HEADER, HTTP_SEND_SIGNAL);
    fprintf(stderr, ".");fflush(stderr);

    /* FIXME: MOTD */
    sprintf(aiStr, "Tracker Statistics<BR>\r\n");
    send(sock, aiStr, strlen(aiStr), HTTP_SEND_SIGNAL);
    sprintf(aiStr, "Number of Tracked Games: %li<BR>\r\n", statsGames);
    send(sock, aiStr, strlen(aiStr), HTTP_SEND_SIGNAL);
    sprintf(aiStr, "Number of Tracker Game List Requests: %li<BR>\r\n", statsTcp);
    send(sock, aiStr, strlen(aiStr), HTTP_SEND_SIGNAL);
    sprintf(aiStr, "Number of Interesting Game List Requests: %li<BR><BR>\r\n", statsInteresting);
    send(sock, aiStr, strlen(aiStr), HTTP_SEND_SIGNAL);
    sprintf(aiStr, "Number of Tracker Web Requests: %li<BR><BR>\r\n", statsHttp);
    send(sock, aiStr, strlen(aiStr), HTTP_SEND_SIGNAL);
    sprintf(aiStr, "Most http requests at one time: %d<BR>\r\n", httpNumThreads);
    send(sock, aiStr, strlen(aiStr), HTTP_SEND_SIGNAL);
    sprintf(aiStr, "Most udp requests at one time: %d<BR><BR>\r\n", udpNumThreads);
    send(sock, aiStr, strlen(aiStr), HTTP_SEND_SIGNAL);

    fprintf(stderr, ".");fflush(stderr);

    /* from here on we need access to the currentGames structure */
    currentGamesLock();

    /* Number of games */
    numGames = currentGamesItemCount(cg);
    if (numGames == 0)
	    sprintf(str, "There are no games at the moment - maybe you could start one.\r\n");
    else if (numGames == 1) {
	    sprintf(str, "There is currently %d game.<BR><BR><PRE>\r\n\n", numGames);
    } else {
	    sprintf(str, "There are currently %d games.<BR><BR><PRE>\r\n\n", numGames);
    }
    send(sock, str, strlen(str), HTTP_SEND_SIGNAL);
    fprintf(stderr, ".");fflush(stderr);


    /* make each game */
    while (count <= numGames) {
	    /*printf("GAME INFO %d\n",  numGames-count);*/
      currentGamesGetItem(cg, numGames-count+1, (char *) address, &port, (char *) mapName, (char *) version, &numPlayers, &numBases, &numPills, &mines, &game, &ai, &password, &starttime, &startdelay, &timelimit);
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

/*      sprintf(str, "GAME%.03d=%s:%d\r\nVERSION=%s\r\nMAP=%s\r\nTYPE=%s\r\nPLAYERS=%d\r\nBASES=%d\r\nPILLS=%d\r\nHIDMINES=%s\r\nPASSWORD=%s\r\nBRAINS=%s\r\nDELAY=%ld\r\nLIMIT=%ld\r\nSTARTTIME=%ld\r\n", count-1, address, port, version, mapName, gameStr, numPlayers, numBases, numPills, http_TRUEFALSE_TO_YESNO(mines), TCP_TRUEFALSE_TO_YESNO(password), aiStr, startdelay, timelimit, starttime);*/
        sprintf(str, "Host: <B><A HREF=winbolo://%s:%d>%s:%d</A></B>  Version: <B>%s</B>  Players: <B>%d</B>  "
                "Bases: <B>%d</B>  Pills: <B>%d</B>\nMap: <B>%s</B>  Game: <B>%s</B>  "
                "Hidden Mines: <I>%s</I>  Bots: <I>%s</I>  PW: <I>%s</I>\n\n",
                address, port, address, port, version, numPlayers, numBases, numPills, mapName, gameStr,
                TCP_TRUEFALSE_TO_YESNO(mines), aiStr, TCP_TRUEFALSE_TO_YESNO(password));

     /* sprintf(str, "GAME%.03d=%s:%d\r\nVERSION=%s\r\nMAP=%s\r\nTYPE=%s\r\nPLAYERS=%d\r\nBASES=%d\r\nPILLS=%d\r\nHIDMINES=%s\r\nPASSWORD=%s\r\nBRAINS=%s\r\nDELAY=%ld\r\nLIMIT=%ld\r\nSTARTTIME=%c%c%c%c\r\n", count-1, address, port, version, mapName, gameStr, numPlayers, numBases, numPills, http_TRUEFALSE_TO_YESNO(mines), TCP_TRUEFALSE_TO_YESNO(password), aiStr, startdelay, timelimit, startTimeBytes[0], startTimeBytes[1], startTimeBytes[2], startTimeBytes[3]);*/
      send(sock, str, strlen(str), HTTP_SEND_SIGNAL);
      count++;
      fprintf(stderr, "#");fflush(stderr);
    }
  }

  /* we are finished with currentGames structure */
  currentGamesUnlock();

  sprintf(str, "</PRE></BODY></HTML>\r\n");
  send(sock, str, strlen(str), HTTP_SEND_SIGNAL);

  fprintf(stderr, "]  httpSendGameData is finished.\n");
}

