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
*Name:          frameMutex
*Filename:      frameMutex.h
*Author:        John Morrison
*Creation Date: 27/5/00
*Last Modified: 27/5/00
*Purpose:
*  WinBolo Server Thread manager
*********************************************************/

#ifndef _FRAME_MUTEX_H
#define _FRAME_MUTEX_H

#ifdef _WIN32
/* Windows */
#include <windows.h>
#else
/* Linux */
#endif

#include "../../bolo/global.h"


/*********************************************************
*NAME:          frameMutexCreate
*AUTHOR:        John Morrison
*CREATION DATE: 27/5/00
*LAST MODIFIED: 27/5/00
*PURPOSE:
*  Creates the frame Mutex. Returns success
*
*ARGUMENTS:
*
*********************************************************/
bool frameMutexCreate(void);

/*********************************************************
*NAME:          frameMutexCreate
*AUTHOR:        John Morrison
*CREATION DATE: 27/5/00
*LAST MODIFIED: 27/5/00
*PURPOSE:
*  Destroys the frame Mutex.
*
*ARGUMENTS:
*
*********************************************************/
void frameMutexDestroy(void);

/*********************************************************
*NAME:          frameMutexWaitFor
*AUTHOR:        John Morrison
*CREATION DATE: 27/5/00
*LAST MODIFIED: 27/5/00
*PURPOSE:
*  Destroys the frame Mutex.
*
*ARGUMENTS:
*
*********************************************************/
void frameMutexWaitFor(void);

/*********************************************************
*NAME:          frameMutexRelease
*AUTHOR:        John Morrison
*CREATION DATE: 27/5/00
*LAST MODIFIED: 27/5/00
*PURPOSE:
*  Frees the lock on the frame Mutex so other waiting
*  subsystems can aquire it.
*
*ARGUMENTS:
*
*********************************************************/
void frameMutexRelease(void);

#endif /* _CLIENT_MUTEX_H */
