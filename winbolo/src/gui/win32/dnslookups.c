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

#include <windows.h>
#include <winbase.h>
#include <string.h>
#include "../../bolo/global.h"
#include "../netclient.h"
#include "../dnslookups.h"


HANDLE hDnsMutexHandle = NULL;
HANDLE hDnsThread;
DWORD dnsThreadID;
dnsList dnsProcessing;
dnsList dnsWaiting;
bool dnsShouldRun;
bool dnsFinished;

/*********************************************************
*NAME:          dnsLookupsCreate
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
  char name[FILENAME_MAX]; /* Used in Mutex creation */
  bool returnValue;        /* Value to return */

  returnValue = TRUE;
  dnsProcessing = NULL;
  dnsWaiting = NULL;
  dnsShouldRun = TRUE;
  dnsFinished = FALSE;
    
  sprintf(name, "%s%d", "DNSLOOKUPS", winbolotimer());
  hDnsMutexHandle = CreateMutex(NULL, FALSE, name);
  if (hDnsMutexHandle == NULL) {
    returnValue = FALSE;
  }

  /* create thread and run */
  if (returnValue == TRUE) {
    hDnsThread = CreateThread((LPSECURITY_ATTRIBUTES) NULL, 0, (LPTHREAD_START_ROUTINE) dnsLookupsRun, 0, 0, &dnsThreadID);
    if (hDnsThread  == NULL) {
      CloseHandle(hDnsMutexHandle);
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
  DWORD val;   /* Thread Exit Value */

  if (hDnsMutexHandle != NULL) { /* FIXME: Will be non null if we started it OK. Is there a better way? (threadid?) */
    /* Wait for current to finish */
    dnsShouldRun = FALSE;
    while (dnsFinished == FALSE) {
      /* Wait a bit for the last call to finish */
      Sleep(DNS_SHUTDOWN_SLEEP_TIME);
    }
    WaitForSingleObject(hDnsThread, DNS_WAIT_THREAD_EXIT);
    GetExitCodeThread(hDnsThread, &val);
    if (val == STILL_ACTIVE) {
      TerminateThread(hDnsThread, 0);
    }
    
    CloseHandle(hDnsThread);

    /* Free our list queues */
    WaitForSingleObject(hDnsMutexHandle, INFINITE);
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
    ReleaseMutex(hDnsMutexHandle);

    /* Remove the locking mutex */
    CloseHandle(hDnsMutexHandle);
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
    WaitForSingleObject(hDnsMutexHandle, INFINITE);
    add->next = dnsWaiting;
    dnsWaiting = add;
    ReleaseMutex(hDnsMutexHandle);
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
int dnsLookupsRun(void) {
  char dest[512]; /* Destination address space */
  dnsList q;      /* Used to iterate through the list */
  FARPROC func;   /* Function to call */

  while (dnsShouldRun == TRUE) {
    WaitForSingleObject(hDnsMutexHandle, INFINITE);
    dnsProcessing = dnsWaiting;
    dnsWaiting = NULL;
    ReleaseMutex(hDnsMutexHandle);
    
    while (NonEmpty(dnsProcessing) && dnsShouldRun == TRUE) {
      q = dnsProcessing;
      netClientGetAddress(q->ip, dest);
      func = q->func;
      func(q->ip, dest);
      dnsProcessing = q->next;
      Dispose(q);
    }
    Sleep(DNS_THREAD_SLEEP_TIME);
  }
  dnsFinished = TRUE;
  ExitThread(0);
  return 0;
}
