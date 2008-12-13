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
*Creation Date: 05/05/01
*Last Modified: 05/05/01
*Purpose:
*  System Specific Sound Playing routines 
*  (Uses Direct Sound)
*********************************************************/

#ifndef _SOUND_H 
#define _SOUND_H 

#include <windows.h>
#include <dsound.h>
#include "global.h"
#include "backend.h"

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
*LAST MODIFIED: 28/12/98
*PURPOSE:
*  Plays the specified Sound Buffer
*
*ARGUMENTS:
*  value       - Direct Sound Buffer to load into
*  lpzFileName - File name to load if buffer is lost
*********************************************************/
void soundPlaySound(LPDIRECTSOUNDBUFFER *value, LPSTR lpzFileName);

/*********************************************************
*NAME:          soundLoadWave
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

#endif /* _SOUND_H  */
