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
*Name:          Sound
*Filename:      sound.h
*Author:        John Morrison
*Creation Date: 26/12/98
*Last Modified: 24/02/02
*Purpose:
*  System Specific Sound Playing routines 
*  (Uses Direct Sound)
*********************************************************/

#ifndef SOUND_H 
#define SOUND_H 

#include <windows.h>
#include <dsound.h>
#include "..\bolo\global.h"
#include "..\bolo\backend.h"
#include "winbolo.h"
#ifdef _WIN32
#include "resource.h"
#else
#include "linresource.h"
#endif

/*********************************************************
*NAME:          soundSetup
*AUTHOR:        John Morrison
*CREATION DATE: 26/10/98
*LAST MODIFIED: 26/10/98
*PURPOSE:
*  Sets up sound systems, direct sound structures etc.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
* appInst - Handle to the application (Required to 
*           load bitmaps from resources)
* appWnd  - Main Window Handle (Required for clipper)
*********************************************************/
bool soundSetup(HINSTANCE appInst, HWND appWnd);

/*********************************************************
*NAME:          drawCleanup
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Destroys and cleans up sound systems, direct sound
*  structures etc.
*
*ARGUMENTS:
*
*********************************************************/
void soundCleanup(void);

/*********************************************************
*NAME:          soundPlaySound
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 24/02/02
*PURPOSE:
*  Plays the specified Sound Buffer
*
*ARGUMENTS:
*  value           - Direct Sound Buffer to load into
*  lpzResourceName - Resource Name to load if buffer lost
*  lpzFileName     - File name to load if buffer is lost
*********************************************************/
void soundPlaySound(LPDIRECTSOUNDBUFFER *value, LPSTR lpzResourceName, LPSTR lpzFileName);

/*********************************************************
*NAME:          soundPlayEffect
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 28/12/98
*PURPOSE:
*  Plays the correct Sound file
*
*ARGUMENTS:
*  value       - The sound file number to play
*********************************************************/
void soundPlayEffect(sndEffects value);

/*********************************************************
*NAME:          soundISASoundCard
*AUTHOR:        John Morrison
*CREATION DATE: 29/12/98
*LAST MODIFIED: 29/12/98
*PURPOSE:
*  ISA Sound cards waste CPU cycles switching the sound
*  mixer on or off. This can be compensatated by constantly
*  plays silence on the primary buffer.
*
*ARGUMENTS:
*  value - TRUE to turn on FALSE to turn off.
*********************************************************/
void soundISASoundCard(bool value);

/*********************************************************
*NAME:          soundIsPlayable
*AUTHOR:        John Morrison
*CREATION DATE: 13/6/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  Returns whether the sound system is enabled or not. By
*  enabled I mean an error hasn't stopped us from starting
*  it
*
*ARGUMENTS:
*
*********************************************************/
bool soundIsPlayable();

/*********************************************************
*NAME:          soundReloadSoundBuffer
*AUTHOR:        John Morrison
*CREATION DATE: 24/02/02
*LAST MODIFIED: 24/02/02
*PURPOSE:
*  Loads and returns a sound buffer. Tries a file first
*  then the bolosounds file for wav data. Returns NULL
*  if unsuccessful
*
*ARGUMENTS:
*  value           - Direct Sound Buffer to load into
*  hModule         - Bolo Sounds Module
*  resourceName    - Resource Name to load if buffer lost
*  lpzFileName     - File name to load if buffer is lost
*********************************************************/
IDirectSoundBuffer *soundLoadSound(IDirectSound *pDS, HMODULE hModule, LPCTSTR lpResourceName, LPCTSTR lpFileName);

/*********************************************************
*NAME:          soundReloadSoundBuffer
*AUTHOR:        John Morrison
*CREATION DATE: 24/02/02
*LAST MODIFIED: 24/02/02
*PURPOSE:
*  Reloads the sound buffer if lost. Tries a file first
*  then the bolosounds file for wav data. Returns success
*
*ARGUMENTS:
*  value           - Direct Sound Buffer to load into
*  hModule         - Bolo Sounds Module
*  resourceName    - Resource Name to load if buffer lost
*  lpzFileName     - File name to load if buffer is lost
*********************************************************/
bool soundReloadSoundBuffer(LPDIRECTSOUNDBUFFER *value, HMODULE hModule, LPSTR resourceName, LPSTR lpzFileName);

#endif /* SOUND_H */
