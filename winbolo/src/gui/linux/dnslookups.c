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
*Name:          dnsLookups
*Filename:      dnsLookups.c
*Author:        John Morrison
*Creation Date: 10/04/01
*Last Modified: 10/04/01
*Purpose:
*  WinBolo Server Thread manager
*********************************************************/

#ifdef _WIN32
	#include <windows.h>
	#include <winbase.h>
#else
	#include "SDL.h"
	#include "SDL_thread.h"
	typedef SDL_mutex *HANDLE;
#endif
#include <string.h>
#include "../../bolo/global.h"
#include "../netclient.h"
#include "../dnsLookups.h"



HANDLE hDnsMutexHandle = NULL;
SDL_Thread *hDnsThread;
dnsList dnsProcessing;
dnsList dnsWaiting;
bool dnsShouldRun;
bool dnsFinished;

/*********************************************************
*NAME:          eCreate
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
*  Creates the DNS Lookups thread. Returns success
*
*ARGUMENTS:
*
*********************************************************/
bool dnsLookupsCreate(void) {
  bool returnValue;        /* Value to return */

  returnValue = TRUE;
  dnsProcessing = NULL;
  dnsWaiting = NULL;
  dnsShouldRun = TRUE;
  dnsFinished = FALSE;
  
  hDnsMutexHandle = SDL_CreateMutex();
  if (hDnsMutexHandle == NULL) {
    returnValue = FALSE;
  }

  /* create thread and run */
  if (returnValue == TRUE) {
    hDnsThread = SDL_CreateThread(dnsLookupsRun, NULL);
    if (hDnsThread  == NULL) {
      SDL_DestroyMutex(hDnsMutexHandle);
      hDnsMutexHandle = NULL;
      returnValue = FALSE;
    }

  }
  return returnValue;
}

/*********************************************************
*NAME:          dnsLookupsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
*  Destroys the DNS lookup Thread.
*
*ARGUMENTS:
*
*********************************************************/
void dnsLookupsDestroy(void) {
  dnsList del; /* Use to delete our queues */

  if (hDnsMutexHandle != NULL) { /* FIXME: Will be non null if we started it OK. Is there a better way? (threadid?) */
    /* Wait for current to finish */
    dnsShouldRun = FALSE;
    while (dnsFinished == FALSE) {
      /* Wait a bit for the last call to finish */
      sleep(DNS_SHUTDOWN_SLEEP_TIME_LINUX);
    }
    SDL_WaitThread(hDnsThread, NULL);

    /* Free our list queues */
    SDL_mutexP(hDnsMutexHandle);
    while (NonEmpty(dnsProcessing)) {
      del = dnsProcessing;
      dnsProcessing = dnsProcessing->next;
      Dispose(del);
    }
    while (NonEmpty(dnsWaiting)) {
      del = dnsWaiting;
      dnsWaiting = dnsWaiting->next;
      Dispose(del);
    } 
    SDL_mutexV(hDnsMutexHandle);

    /* Remove the locking mutex */
    SDL_DestroyMutex(hDnsMutexHandle);
    hDnsMutexHandle = NULL;
  }
}


/*********************************************************
*NAME:          dnsLookupsAddRequest
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
*  Adds a request to the DNS lookup queue
*
*ARGUMENTS:
*  ip   - The IP address or DNS name to lookup
*  func - The function to call back when the results have
*         have be determined
*********************************************************/
void dnsLookupsAddRequest(char *ip, void *func) {
  dnsList add; /* Used to add to the queue */

  if (dnsShouldRun == TRUE) {
    New(add);
    strcpy(add->ip, ip);
    add->func = func;
    SDL_mutexP(hDnsMutexHandle);
    add->next = dnsWaiting;
    dnsWaiting = add;
    SDL_mutexV(hDnsMutexHandle);
  }
}


/*********************************************************
*NAME:          dnsLookupsRun
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
*  The DNS Lookups thread run method
*
*ARGUMENTS:
*
*********************************************************/
int dnsLookupsRun() {
  char dest[512]; /* Destination address space */
  dnsList q;      /* Used to iterate through the list */
  void *func;   /* Function to call */

  while (dnsShouldRun == TRUE) {
    SDL_mutexP(hDnsMutexHandle);
    dnsProcessing = dnsWaiting;
    dnsWaiting = NULL;
    SDL_mutexV(hDnsMutexHandle);
    
    while (NonEmpty(dnsProcessing) && dnsShouldRun == TRUE) {
      q = dnsProcessing;
      netClientGetAddress(q->ip, dest);
/*      func = q->func;
      func(q->ip, dest); */
      /* FIXME: Hardcoded because I don't know how to call back in linux */
      netProcessedDnsLookup(q->ip, dest);
      dnsProcessing = q->next;
      Dispose(q);
    }
    sleep(DNS_THREAD_SLEEP_TIME_LINUX);
  }
  dnsFinished = TRUE;
  return 0;
}

