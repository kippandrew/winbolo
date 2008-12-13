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
*Name:          Threads
*Filename:      threads.c
*Author:        John Morrison
*Creation Date: 12/08/99
*Last Modified: 21/09/03
*Purpose:
*  WinBolo Server Thread manager
*********************************************************/

#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "SDL.h"
typedef SDL_mutex *HANDLE;
#endif
#include "../bolo/global.h"
#include "servertransport.h"
#include "servercore.h"
#include "threads.h"

HANDLE hMutexHandle = NULL;
bool threadServerContext = FALSE;
bool threadStarted = FALSE;

/*********************************************************
*NAME:          threadsCreate
*AUTHOR:        John Morrison
*CREATION DATE: 12/08/99
*LAST MODIFIED: 21/09/03
*PURPOSE:
*  Creates the thread manager for the server and sets up
*  TCP connection request and UDP listen thread. Returns
*  success
*
*ARGUMENTS:
*  context - TRUE if to start in server context
*********************************************************/
bool threadsCreate(bool context) {
  bool returnValue = TRUE; /* Value to return */

  if (threadStarted == TRUE) {
    return TRUE;
  }
  threadServerContext = context;
  screenServerConsoleMessage((char *)"Thread Manager Startup");
#ifdef _WIN32
  hMutexHandle = CreateMutex(NULL, FALSE, "WinBoloDS");
#else 
  hMutexHandle = SDL_CreateMutex();
#endif
  if (hMutexHandle == NULL) {
    returnValue = FALSE;
    fprintf(stderr, "Error Creating Mutex\n");
  }
 
  threadStarted = returnValue;
  return returnValue;
}

/*********************************************************
*NAME:          threadsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 12/8/99
*LAST MODIFIED: 24/8/99
*PURPOSE:
*  Terminate and shuts down the threads.
*
*ARGUMENTS:
*
*********************************************************/
void threadsDestroy(void) {
 
  threadStarted = FALSE;
  screenServerConsoleMessage((char *) "Thread Manager Shutdown");
  /* TCP Listener */
#ifdef _WIN32
  CloseHandle(hMutexHandle);
#else
  SDL_DestroyMutex(hMutexHandle);
#endif
  hMutexHandle = NULL;
   
}

/*********************************************************
*NAME:          threadsWaitFor
*AUTHOR:        John Morrison
*CREATION DATE: 12/8/99
*LAST MODIFIED: 24/8/99
*PURPOSE:
*  Waits till the mutex is acquired.
*
*ARGUMENTS:
*
*********************************************************/
void threadsWaitForMutex(void) {
#ifdef _WIN32
  WaitForSingleObject(hMutexHandle, INFINITE);
#else
  SDL_mutexP(hMutexHandle);
#endif
}

/*********************************************************
*NAME:          threadsRelease
*AUTHOR:        John Morrison
*CREATION DATE: 12/8/99
*LAST MODIFIED: 24/8/99
*PURPOSE:
*  Releases the mutex when we are done with it.
*
*ARGUMENTS:
*
*********************************************************/
void threadsReleaseMutex(void) {
#ifdef _WIN32
  ReleaseMutex(hMutexHandle);
#else
  SDL_mutexV(hMutexHandle);
#endif
}

/*********************************************************
*NAME:          threadsGetContext
*AUTHOR:        John Morrison
*CREATION DATE: 27/07/03
*LAST MODIFIED: 27/07/03
*PURPOSE:
*  Returns the current thread context. TRUE if we are in
*  a server context, false if we are in a client
*
*ARGUMENTS:
*
*********************************************************/
bool threadsGetContext() {
  return threadServerContext;
}

/*********************************************************
*NAME:          threadsSetContext
*AUTHOR:        John Morrison
*CREATION DATE: 27/07/03
*LAST MODIFIED: 27/07/03
*PURPOSE:
*  Sets the current thread context. TRUE if we are in
*  a server context, false if we are in a client
*
*ARGUMENTS:
*  serverContext - Context to set to
*********************************************************/
void threadsSetContext(bool serverContext) {
  threadServerContext = serverContext;
}
