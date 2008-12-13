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
*Filename:      frameMutex.c
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
#include "framemutex.h"

HANDLE hFrameMutexHandle = NULL;


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
bool frameMutexCreate(void) {
  char name[FILENAME_MAX];
  bool returnValue; /* Value to return */

  returnValue = TRUE;
#ifdef _WIN32
  sprintf(name, "%s%d", DIALOG_BOX_TITLE, GetTickCount());
  hFrameMutexHandle = CreateMutex(NULL, FALSE, name);
  if (hFrameMutexHandle == NULL) {
    DWORD d = GetLastError();
    if (d == ERROR_ALREADY_EXISTS) {
      d= 1;
    }
    returnValue = FALSE;
  }
#else
  hFrameMutexHandle = SDL_CreateMutex();
  if (hFrameMutexHandle == NULL) {
    returnValue = FALSE;
  }
#endif

  return returnValue;
}

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
void frameMutexDestroy(void) {
#ifdef _WIN32
  CloseHandle(hFrameMutexHandle);
#else
  SDL_DestroyMutex(hFrameMutexHandle);
#endif
  hFrameMutexHandle = NULL;
}


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
void frameMutexWaitFor(void) {
#ifdef _WIN32
  WaitForSingleObject(hFrameMutexHandle, INFINITE);
#else
  SDL_mutexP(hFrameMutexHandle);
#endif
}

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
void frameMutexRelease(void) {
#ifdef _WIN32
  ReleaseMutex(hFrameMutexHandle);
#else
  SDL_mutexV(hFrameMutexHandle);
#endif
}
