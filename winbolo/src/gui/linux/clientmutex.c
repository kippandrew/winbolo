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
*Name:          clientMutex
*Filename:      clientMutex.c
*Author:        John Morrison
*Creation Date: 27/5/00
*Last Modified: 27/5/00
*Purpose:
*  WinBolo Server Thread manager
*********************************************************/

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#else
#include "SDL.h"
typedef SDL_mutex *HANDLE;
#endif
#include <string.h>
#include <time.h>
#include "../../bolo/global.h"
#include "../clientmutex.h"

HANDLE hClientMutexHandle = NULL;


/*********************************************************
*NAME:          clientMutexCreate
*AUTHOR:        John Morrison
*CREATION DATE: 27/5/00
*LAST MODIFIED: 27/5/00
*PURPOSE:
*  Creates the client Mutex. Returns success
*
*ARGUMENTS:
*
*********************************************************/
bool clientMutexCreate(void) {
  char name[FILENAME_MAX];
  bool returnValue; /* Value to return */

  returnValue = TRUE;
#ifdef _WIN32
  sprintf(name, "%s%d", DIALOG_BOX_TITLE, GetTickCount());
  hClientMutexHandle = CreateMutex(NULL, FALSE, name);
  if (hClientMutexHandle == NULL) {
    DWORD d = GetLastError();
    if (d == ERROR_ALREADY_EXISTS) {
      d= 1;
    }
    returnValue = FALSE;
  }
#else
  hClientMutexHandle = SDL_CreateMutex();
  if (hClientMutexHandle == NULL) {
    returnValue = FALSE;
  }
#endif

  return returnValue;
}

/*********************************************************
*NAME:          clientMutexCreate
*AUTHOR:        John Morrison
*CREATION DATE: 27/5/00
*LAST MODIFIED: 27/5/00
*PURPOSE:
*  Destroys the client Mutex.
*
*ARGUMENTS:
*
*********************************************************/
void clientMutexDestroy(void) {
#ifdef _WIN32
  CloseHandle(hClientMutexHandle);
#else
  SDL_DestroyMutex(hClientMutexHandle);
#endif
  hClientMutexHandle = NULL;
}


/*********************************************************
*NAME:          clientMutexWaitFor
*AUTHOR:        John Morrison
*CREATION DATE: 27/5/00
*LAST MODIFIED: 27/5/00
*PURPOSE:
*  Destroys the client Mutex.
*
*ARGUMENTS:
*
*********************************************************/
void clientMutexWaitFor(void) {
#ifdef _WIN32
  WaitForSingleObject(hClientMutexHandle, INFINITE);
#else
  SDL_mutexP(hClientMutexHandle);
#endif
}

/*********************************************************
*NAME:          clientMutexRelease
*AUTHOR:        John Morrison
*CREATION DATE: 27/5/00
*LAST MODIFIED: 27/5/00
*PURPOSE:
*  Frees the lock on the client Mutex so other waiting
*  subsystems can aquire it.
*
*ARGUMENTS:
*
*********************************************************/
void clientMutexRelease(void) {
#ifdef _WIN32
  ReleaseMutex(hClientMutexHandle);
#else
  SDL_mutexV(hClientMutexHandle);
#endif
}
