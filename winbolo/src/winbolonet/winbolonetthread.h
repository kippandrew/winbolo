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
*Name:          winbolonetThread
*Filename:      winbolonetThread.c
*Author:        John Morrison
*Creation Date: 16/02/03
*Last Modified: 16/02/03
*Purpose:
*  WinBolo.net Thread manager - Used to stop updates
*  causing game problems
*********************************************************/

#ifndef __WINBOLONET_THREAD_H
#define __WINBOLONET_THREAD_H

#include <string.h>
#include "../bolo/global.h"

#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))

/* Time to sleep between checks (MS) */
#define WBN_THREAD_SLEEP_TIME 500
#define WBN_THREAD_SLEEP_TIME_LINUX 1

/* Time to sleep between checks (MS) */
#define WBN_SHUTDOWN_SLEEP_TIME 200
#define WBN_SHUTDOWN_SLEEP_TIME_LINUX 1

/* Amount of time to wait for the thread to shutdown (MS) */
#define WBN_WAIT_THREAD_EXIT 1000


typedef struct wbnListObj *wbnList;
struct wbnListObj {
  wbnList next;  /* Next item */
  BYTE data[2048];  /* Data to send */
  int len;    /* Data length */
};




/*********************************************************
*NAME:          winbolonetThreadCreate 
*AUTHOR:        John Morrison
*CREATION DATE: 16/02/03
*LAST MODIFIED: 16/02/03
*PURPOSE:
*  Creates the winbolonet update thread. Returns success
*
*ARGUMENTS:
*
*********************************************************/
bool winbolonetThreadCreate(void);

/*********************************************************
*NAME:          winbolonetThreadDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 16/02/03
*LAST MODIFIED: 16/02/03
*PURPOSE:
*  Destroys the DNS lookup Thread.
*
*ARGUMENTS:
*
*********************************************************/
void winbolonetThreadDestroy(void);

/*********************************************************
*NAME:          winbolonetThreadAddRequest
*AUTHOR:        John Morrison
*CREATION DATE: 16/02/03
*LAST MODIFIED: 16/02/03
*PURPOSE:
*  Adds a request to the DNS lookup queue
*
*ARGUMENTS:
* data - Data to send 
* len  - Length of the data 
*********************************************************/
void winbolonetThreadAddRequest(BYTE *data, int len);

/*********************************************************
*NAME:          winbolonetThreadRun
*AUTHOR:        John Morrison
*CREATION DATE: 16/02/03
*LAST MODIFIED: 16/02/03
*PURPOSE:
*  The DNS Lookups thread run method
*
*ARGUMENTS:
*
*********************************************************/
int winbolonetThreadRun();

#endif /* __WINBOLONET_THREAD_H */

