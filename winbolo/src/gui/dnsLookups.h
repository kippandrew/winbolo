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
*Filename:      dnsLookups.h
*Author:        John Morrison
*Creation Date: 10/04/01
*Last Modified: 10/04/01
*Purpose:
*  WinBolo Server Thread manager
*********************************************************/

#ifndef _DNSLOOKUPS_H
#define _DNSLOOKUPS_H

#include "../bolo/global.h"

#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))

/* Time to sleep between checks (MS) */
#define DNS_THREAD_SLEEP_TIME 500
#define DNS_THREAD_SLEEP_TIME_LINUX 1

/* Time to sleep between checks (MS) */
#define DNS_SHUTDOWN_SLEEP_TIME 200
#define DNS_SHUTDOWN_SLEEP_TIME_LINUX 1

/* Amount of time to wait for the thread to shutdown (MS) */
#define DNS_WAIT_THREAD_EXIT 1000


typedef struct dnsListObj *dnsList;
struct dnsListObj {
  dnsList next;  /* Next item */ 
  char ip[512];  /* IP Size */
  void *func;    /* Address to call */
};


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
bool dnsLookupsCreate(void);

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
void dnsLookupsDestroy(void);

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
void dnsLookupsAddRequest(char *ip, void *func);

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
int dnsLookupsRun();

#endif /* _DNSLOOKUPS_H */
