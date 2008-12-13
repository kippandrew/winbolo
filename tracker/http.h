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
*Filename:      http.h
*Author:        John Morrison
*Creation Date: 00/10/01
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for handling of incoming HTTP game list
*  connections (web server)
*********************************************************/


#ifndef _HTTP_H
#define _HTTP_H

#define max(A,B)        (A > B) ? (A) : (B)

#define BUFF_SIZE               4096

/* this structure stores mutex/signal and connection info for one thread */
typedef struct {
        int busy;
        currentGames *cg;
	int sock;
        struct in_addr *pack;
        pthread_cond_t signal;
        pthread_mutex_t mutex;
	char httpGet[BUFF_SIZE];
} httpThreadInfoStruct;
int httpNumThreads;

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
void httpAssignToThread(currentGames *cg, int sock, struct in_addr *pack);

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
void *httpThreadWaitForWork(void *httpThreadIndex);

/*********************************************************
*NAME:          httpCreate
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Creates the http server to show the games
*
*ARGUMENTS:
*  port     - Port to bind to
*  fileName - File name to load the files we can send
*             The format is: <web filename>:<path>
*********************************************************/
SOCKET httpCreate(unsigned short port, char *fileName);

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
void httpDestroy();

/*********************************************************
*NAME:          httpRead
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Checks the socket for new connections
*
*ARGUMENTS:
*  cg - Pointer to current games structure
*********************************************************/
void httpRead(currentGames *cg);

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
int httpReceiveGet(int sock, char *buff);

/*********************************************************
*NAME:          httpProcessNewRequest
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Processes the request
*
*ARGUMENTS:
*  threadDI - thread ID
*  cg   - Pointer to current games structure
*  sock - The socket to send to
*  pack - Address the packet came from
*********************************************************/
void httpProcessNewRequest(int id, currentGames *cg, int sock, struct in_addr *pack);

/*********************************************************
*NAME:          httpProcessNewRequestThread
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Processes the request
*
*ARGUMENTS:
*
*********************************************************/
void *httpProcessNewRequestThread(void *connectionInfo);

/*********************************************************
*NAME:          udpSendGameData
*AUTHOR:        Andrew Roth
*CREATION DATE: 03/02/26 YMD
*LAST MODIFIED: 03/02/26
*PURPOSE:
* Sends the game list to an http socket
*
*ARGUMENTS:
*  threadID - current thread id
*  cg   - Pointer to current games structure
*  sock - The socket to send to
*  pack - Address the packet came from
*********************************************************/
void httpSendGameData(int threadID, currentGames *cg, int sock, struct in_addr *pack);

#endif /* _HTTP_H */

