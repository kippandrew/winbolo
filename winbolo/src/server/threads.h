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
*Filename:      threads.h
*Author:        John Morrison
*Creation Date: 12/08/99
*Last Modified: 21/09/03
*Purpose:
*  WinBolo Server Thread manager
*********************************************************/

#ifndef _THREADS_H
#define _THREADS_H

#include "../bolo/global.h"

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
bool threadsCreate(bool context);

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
void threadsDestroy(void);

/*********************************************************
 * *NAME:          threadsWaitFor
 * *AUTHOR:        John Morrison
 * *CREATION DATE: 12/8/99
 * *LAST MODIFIED: 24/8/99
 * *PURPOSE:
 * *  Waits till the mutex is acquired.
 * *
 * *ARGUMENTS:
 * *
 * *********************************************************/
void threadsWaitForMutex(void);

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
void threadsReleaseMutex(void);

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
bool threadsGetContext();

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
void threadsSetContext(bool serverContext);

#endif /* _THREADS_H */

