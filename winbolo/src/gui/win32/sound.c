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
*Filename:      sound.c
*Author:        John Morrison
*Creation Date: 26/12/98
*Last Modified: 24/02/02
*Purpose:
*  System Specific Sound Playing routines 
*  (Uses Direct Sound)
*********************************************************/

#include <windows.h>
#include <dsound.h>
#include "..\..\bolo\global.h"
#include "..\..\bolo\backend.h"
#include "..\winbolo.h"
#include "..\resource.h"
#include "..\..\bsd\bolosounds.h"
#include "..\dsutil.h"
#include "..\lang.h"
#include "..\sound.h"
#include "..\skins.h"
#include "..\winutil.h"

#define BOLO_SOUNDS_DLL "BoloSounds.bsd"

LPDIRECTSOUND lpDS = NULL;
LPDIRECTSOUNDBUFFER lpDSPrimary = NULL;
LPDIRECTSOUNDBUFFER lpDSBigExplosionFar = NULL;
LPDIRECTSOUNDBUFFER lpDSBigExplosionNear = NULL;
LPDIRECTSOUNDBUFFER lpDSBubbles = NULL;
LPDIRECTSOUNDBUFFER lpDSFarmingTreeFar = NULL;
LPDIRECTSOUNDBUFFER lpDSFarmingTreeNear = NULL;
LPDIRECTSOUNDBUFFER lpDSHitTankFar = NULL;
LPDIRECTSOUNDBUFFER lpDSHitTankNear = NULL;
LPDIRECTSOUNDBUFFER lpDSHitTankSelf = NULL;
LPDIRECTSOUNDBUFFER lpDSManBuildingFar = NULL;
LPDIRECTSOUNDBUFFER lpDSManBuildingNear = NULL;
LPDIRECTSOUNDBUFFER lpDSManDyingFar = NULL;
LPDIRECTSOUNDBUFFER lpDSManDyingNear = NULL;
LPDIRECTSOUNDBUFFER lpDSManLayingMineNear = NULL;
LPDIRECTSOUNDBUFFER lpDSMineExplosionFar = NULL;
LPDIRECTSOUNDBUFFER lpDSMineExplosionNear = NULL;
LPDIRECTSOUNDBUFFER lpDSShootFar = NULL;
LPDIRECTSOUNDBUFFER lpDSShootNear = NULL;
LPDIRECTSOUNDBUFFER lpDSShootSelf = NULL;
LPDIRECTSOUNDBUFFER lpDSShotBuildingFar = NULL;
LPDIRECTSOUNDBUFFER lpDSShotBuildingNear = NULL;
LPDIRECTSOUNDBUFFER lpDSShotTreeFar = NULL;
LPDIRECTSOUNDBUFFER lpDSShotTreeNear = NULL;
LPDIRECTSOUNDBUFFER lpDSTankSinkingFar = NULL;
LPDIRECTSOUNDBUFFER lpDSTankSinkingNear = NULL;

bool isPlayable;
/* Module Handle to the Bolo Sounds DLL */
HMODULE boloSounds = NULL;

/*********************************************************
*NAME:          soundSetup
*AUTHOR:        John Morrison
*CREATION DATE: 26/10/98
*LAST MODIFIED:  13/6/00
*PURPOSE:
*  Sets up sound systems, direct sound structures etc.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
* appInst - Handle to the application (Required to 
*           load bitmaps from resources)
* appWnd  - Main Window Handle (Required for clipper)
*********************************************************/
bool soundSetup(HINSTANCE appInst, HWND appWnd) {
  bool returnValue;       /* Value to return */
  bool dsLoadOK;
  HRESULT res;            /* Direct Sound Function returns */
  DSBUFFERDESC desc;      /* Direct Sound buffer description */
  WAVEFORMATEX wfm;       /* Wave Format */

  returnValue = TRUE;
  dsLoadOK = TRUE;
  isPlayable = TRUE;
  boloSounds = LoadLibrary(BOLO_SOUNDS_DLL);
  if (boloSounds == NULL) {
    MessageBoxA(NULL, langGetText(STR_SOUNDERR_FILENOTFOUND), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
    windowDisableSound();
    isPlayable = FALSE;
  }
  /* Create DS object */
  if (boloSounds != NULL) {
    res = DirectSoundCreate(NULL, &lpDS,NULL);
    if (res == DSERR_ALLOCATED) {
      MessageBoxA(NULL, langGetText(STR_SOUNDERR_HARDWAREINUSE), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      FreeLibrary(boloSounds);
      boloSounds = NULL;
      windowDisableSound();
      isPlayable = FALSE;
    } else if (FAILED(res)) {
      dsLoadOK = FALSE;
      returnValue = FALSE;
      isPlayable = FALSE;
      MessageBoxA(NULL, langGetText(STR_SOUNDERR_CREATEFAILED), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    } 
  }
  /* Set Co-op level */
  if (returnValue == TRUE && boloSounds != NULL) {
    res = lpDS->lpVtbl->SetCooperativeLevel(lpDS, appWnd, DSSCL_NORMAL);
    if (FAILED(res)) {
      MessageBoxA(NULL, langGetText(STR_SOUNDERR_COOPFAILED), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }
  
  /* Create Primary Buffer */
  if (returnValue == TRUE && boloSounds != NULL) {
    memset(&desc, 0, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    desc.dwBufferBytes = 0;
    desc.lpwfxFormat = NULL;
    res = lpDS->lpVtbl->CreateSoundBuffer(lpDS, &desc, &lpDSPrimary, NULL);
    if (FAILED(res)) {
      MessageBoxA(NULL, langGetText(STR_SOUNDERR_PRIMARYBUFFFAIL), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Set the format. Don't care if it fails because then it uses the default anyway */
      memset(&wfm,0,sizeof(WAVEFORMATEX));
      wfm.wFormatTag = WAVE_FORMAT_PCM;
      wfm.nChannels = 2;
      wfm.nSamplesPerSec = 22050;
      wfm.wBitsPerSample = 16;
      wfm.nBlockAlign = wfm.wBitsPerSample / 8 * wfm.nChannels;
      wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nBlockAlign;

      lpDSPrimary->lpVtbl->SetFormat(lpDSPrimary, &wfm);
    }
  }

  /* Load Sound Files In */
  if (returnValue == TRUE && boloSounds != NULL) {
    lpDSTankSinkingNear = soundLoadSound(lpDS,boloSounds, IDW_TANK_SINKING_NEAR, "tank_sink_near.wav");
    lpDSShootSelf = soundLoadSound(lpDS,boloSounds,IDW_SHOOTING_SELF, "tank_shoot_self.wav");
    lpDSShootNear = soundLoadSound(lpDS,boloSounds, IDW_SHOOTING_NEAR, "tank_shoot_near.wav");
    lpDSShootFar = soundLoadSound(lpDS,boloSounds,IDW_SHOOTING_FAR, "tank_shoot_far.wav");
    lpDSBigExplosionFar = soundLoadSound(lpDS,boloSounds, IDW_BIG_EXPLOSION_FAR, "explosion_far.wav");
    lpDSBigExplosionNear = soundLoadSound(lpDS,boloSounds,IDW_BIG_EXPLOSION_NEAR, "explosion_near.wav");
    lpDSBubbles = soundLoadSound(lpDS,boloSounds,IDW_BUBBLES, "bubbles.wav");
    lpDSFarmingTreeFar = soundLoadSound(lpDS,boloSounds,IDW_FARMING_TREE_FAR, "farm_far.wav");
    lpDSFarmingTreeNear = soundLoadSound(lpDS,boloSounds,IDW_FARMING_TREE_NEAR, "farm_near.wav");
    lpDSHitTankFar = soundLoadSound(lpDS,boloSounds,IDW_HIT_TANK_FAR, "tank_hit_far.wav");
    lpDSHitTankNear = soundLoadSound(lpDS,boloSounds, IDW_HIT_TANK_NEAR, "tank_hit_near.wav");
    lpDSHitTankSelf = soundLoadSound(lpDS,boloSounds, IDW_HIT_TANK_SELF, "tank_hit_self.wav");
    lpDSManBuildingFar = soundLoadSound(lpDS,boloSounds, IDW_MAN_BUILDING_FAR, "build_far.wav");
    lpDSManBuildingNear = soundLoadSound(lpDS,boloSounds, IDW_MAN_BUILDING_NEAR, "build_near.wav");
    lpDSManDyingFar = soundLoadSound(lpDS,boloSounds, IDW_MAN_DYING_FAR, "man_dying_far.wav");
    lpDSManDyingNear = soundLoadSound(lpDS,boloSounds, IDW_MAN_DYING_NEAR, "man_dying_near.wav");
    lpDSManLayingMineNear = soundLoadSound(lpDS,boloSounds, IDW_MAN_LAYING_MINE_NEAR, "mine_near.wav");
    lpDSMineExplosionFar = soundLoadSound(lpDS,boloSounds, IDW_MINE_EXPLOSION_FAR, "mine_explosion_far.wav");
    lpDSMineExplosionNear = soundLoadSound(lpDS,boloSounds, IDW_MINE_EXPLOSION_NEAR, "mine_explosion_near.wav");
    lpDSShotBuildingFar = soundLoadSound(lpDS,boloSounds, IDW_SHOT_BUILDING_FAR, "shoot_wall_far.wav");
    lpDSShotBuildingNear = soundLoadSound(lpDS,boloSounds, IDW_SHOT_BUILDING_NEAR, "shoot_wall_near.wav");
    lpDSShotTreeFar = soundLoadSound(lpDS,boloSounds,IDW_SHOT_TREE_FAR, "shoot_tree_far.wav");
    lpDSShotTreeNear = soundLoadSound(lpDS,boloSounds, IDW_SHOT_TREE_NEAR, "shoot_tree_near.wav");
    lpDSTankSinkingFar = soundLoadSound(lpDS,boloSounds, IDW_TANK_SINKING_FAR, "tank_sink_far.wav");
    
    if (lpDSBigExplosionFar == NULL || lpDSBigExplosionNear == NULL || lpDSBubbles == NULL || lpDSFarmingTreeFar == NULL || lpDSFarmingTreeNear == NULL || lpDSHitTankFar == NULL || lpDSHitTankNear == NULL || lpDSHitTankSelf == NULL || lpDSManBuildingFar == NULL || lpDSManBuildingNear == NULL || lpDSManDyingFar == NULL || lpDSManDyingNear == NULL || lpDSMineExplosionFar == NULL || lpDSMineExplosionNear == NULL || lpDSShootFar == NULL || lpDSShootNear == NULL || lpDSShootSelf == NULL || lpDSShotBuildingFar == NULL || lpDSShotBuildingNear == NULL || lpDSShotTreeFar == NULL || lpDSShotTreeNear == NULL || lpDSTankSinkingFar == NULL || lpDSManLayingMineNear == NULL || lpDSTankSinkingNear == NULL) {
      returnValue = FALSE;
      MessageBoxA(NULL, langGetText(STR_SOUNDERR_LOADSOUNDFAILED), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    }

  }
  if (dsLoadOK == FALSE) {
    returnValue = TRUE;
  }
  if (returnValue == FALSE) {
    isPlayable = FALSE;
    windowDisableSound();
  }
  return returnValue;
}

/*********************************************************
*NAME:          soundCleanup
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 28/12/98
*PURPOSE:
*  Destroys and cleans up sound systems, direct sound
*  structures etc.
*
*ARGUMENTS:
*
*********************************************************/
void soundCleanup(void) {
  /* Unload the Direct Sound Buffers and Object */
  if (lpDSBigExplosionFar != NULL) {
    lpDSBigExplosionFar->lpVtbl->Release(lpDSBigExplosionFar);
    lpDSBigExplosionFar = NULL;
  }
  if (lpDSBigExplosionNear != NULL) {
    lpDSBigExplosionNear->lpVtbl->Release(lpDSBigExplosionNear);
    lpDSBigExplosionNear = NULL;
  }
  if (lpDSBubbles != NULL) {
    lpDSBubbles->lpVtbl->Release(lpDSBubbles);
    lpDSBubbles = NULL;
  }
  if (lpDSFarmingTreeFar != NULL) {
    lpDSFarmingTreeFar->lpVtbl->Release(lpDSFarmingTreeFar);
    lpDSFarmingTreeFar = NULL;
  }
  if (lpDSFarmingTreeNear != NULL) {
    lpDSFarmingTreeNear->lpVtbl->Release(lpDSFarmingTreeNear);
    lpDSFarmingTreeNear = NULL;
  }
  if (lpDSHitTankFar != NULL) {
    lpDSHitTankFar->lpVtbl->Release(lpDSHitTankFar);
    lpDSHitTankFar = NULL;
  }
  if (lpDSHitTankNear != NULL) {
    lpDSHitTankNear->lpVtbl->Release(lpDSHitTankNear);
    lpDSHitTankNear = NULL;
  }
  if (lpDSHitTankSelf != NULL) {
    lpDSHitTankSelf->lpVtbl->Release(lpDSHitTankSelf);
    lpDSHitTankSelf = NULL;
  }
  if (lpDSManBuildingFar != NULL) {
    lpDSManBuildingFar->lpVtbl->Release(lpDSManBuildingFar);
    lpDSManBuildingFar = NULL;
  }
  if (lpDSManBuildingNear != NULL) {
    lpDSManBuildingNear->lpVtbl->Release(lpDSManBuildingNear);
    lpDSManBuildingNear = NULL;
  }
  if (lpDSManDyingFar != NULL) {
    lpDSManDyingFar->lpVtbl->Release(lpDSManDyingFar);
    lpDSManDyingFar = NULL;
  }
  if (lpDSManDyingNear != NULL) {
    lpDSManDyingNear->lpVtbl->Release(lpDSManDyingNear);
    lpDSManDyingNear = NULL;
  }

  if (lpDSManLayingMineNear != NULL) {
    lpDSManLayingMineNear->lpVtbl->Release(lpDSManLayingMineNear);
    lpDSManLayingMineNear = NULL;
  }
  if (lpDSMineExplosionFar != NULL) {
    lpDSMineExplosionFar->lpVtbl->Release(lpDSMineExplosionFar );
    lpDSMineExplosionFar = NULL;
  }
  if (lpDSMineExplosionNear != NULL) {
    lpDSMineExplosionNear->lpVtbl->Release(lpDSMineExplosionNear);
    lpDSMineExplosionNear = NULL;
  }
  if (lpDSShotBuildingFar != NULL) {
    lpDSShotBuildingFar->lpVtbl->Release(lpDSShotBuildingFar);
    lpDSShotBuildingFar = NULL;
  }
  if (lpDSShotBuildingNear != NULL) {
    lpDSShotBuildingNear->lpVtbl->Release(lpDSShotBuildingNear);
    lpDSShotBuildingNear = NULL;
  }
  if (lpDSShotTreeFar != NULL) {
    lpDSShotTreeFar->lpVtbl->Release(lpDSShotTreeFar);
    lpDSShotTreeFar = NULL;
  }
  if (lpDSShotTreeNear != NULL) {
    lpDSShotTreeNear->lpVtbl->Release(lpDSShotTreeNear);
    lpDSShotTreeNear = NULL;
  }
  if (lpDSTankSinkingFar != NULL) {
    lpDSTankSinkingFar->lpVtbl->Release(lpDSTankSinkingFar);
    lpDSTankSinkingFar = NULL;
  }
  if (lpDSTankSinkingNear != NULL) {
    lpDSTankSinkingNear->lpVtbl->Release(lpDSTankSinkingNear);
    lpDSTankSinkingNear = NULL;
  }
  if (lpDSShootFar != NULL) {
    lpDSShootFar->lpVtbl->Release(lpDSShootFar);
    lpDSShootFar = NULL;
  }  
  if (lpDSShootNear != NULL) {
    lpDSShootNear->lpVtbl->Release(lpDSShootNear);
    lpDSShootNear = NULL;
  }
  if (lpDSShootSelf != NULL) {
    lpDSShootSelf->lpVtbl->Release(lpDSShootSelf);
    lpDSShootSelf = NULL;
  }
  if (lpDSPrimary != NULL) {
    lpDSPrimary->lpVtbl->Stop(lpDSPrimary);
    lpDSPrimary->lpVtbl->Release(lpDSPrimary);
    lpDSPrimary = NULL;
  } 
  if (lpDS != NULL) {
    lpDS->lpVtbl->Release(lpDS);
    lpDS = NULL;
  }
  /* Unload the DLL */
  if (boloSounds != NULL) {
    FreeLibrary(boloSounds);
    boloSounds = NULL;
  }
}


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
void soundPlaySound(LPDIRECTSOUNDBUFFER *value, LPSTR lpzResourceName, LPSTR lpzFileName) {
  HRESULT hr; /* Function Return Value */  
  
  hr = (*value)->lpVtbl->SetCurrentPosition( (*value),0 );
  hr = (*value)->lpVtbl->Play( (*value), 0, 0, 0 );
  
  if (hr == DSERR_BUFFERLOST) {
    /* Try reloading the buffer if the buffer is lost */
    if ( SUCCEEDED((*value)->lpVtbl->Restore((*value)))) {
      if (soundReloadSoundBuffer(value, boloSounds, lpzResourceName, lpzFileName) == TRUE) {
        (*value)->lpVtbl->Play((*value), 0, 0, 0);
      }
    }
  }
}

/*********************************************************
*NAME:          soundPlayEffect
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 24/02/02
*PURPOSE:
*  Plays the correct Sound file
*
*ARGUMENTS:
*  value       - The sound file number to play
*********************************************************/
void soundPlayEffect(sndEffects value) {  
  switch (value) {
  case shootSelf:
    soundPlaySound(&lpDSShootSelf, IDW_SHOOTING_SELF, "tank_shoot_self.wav");
    break;
  case shootNear:
    soundPlaySound(&lpDSShootNear, IDW_SHOOTING_NEAR, "tank_shoot_near.wav");
    break;
  case shotTreeNear:
    soundPlaySound(&lpDSShotTreeNear, IDW_SHOT_TREE_NEAR, "shoot_tree_near.wav");
    break;
  case shotTreeFar:
    soundPlaySound(&lpDSShotTreeFar, IDW_SHOT_TREE_FAR, "shoot_tree_far.wav");
    break;
  case shotBuildingNear:
    soundPlaySound(&lpDSShotBuildingNear, IDW_SHOT_BUILDING_NEAR, "shoot_wall_near.wav");
    break;
  case shotBuildingFar:
    soundPlaySound(&lpDSShotBuildingFar, IDW_SHOT_BUILDING_FAR, "shoot_wall_far.wav");
    break;
  case hitTankFar:
    soundPlaySound(&lpDSHitTankFar, IDW_HIT_TANK_FAR, "tank_hit_far.wav");
    break;
  case hitTankNear:
    soundPlaySound(&lpDSHitTankNear, IDW_HIT_TANK_NEAR, "tank_hit_near.wav");
    break;
  case hitTankSelf:
    soundPlaySound(&lpDSHitTankSelf, IDW_HIT_TANK_SELF, "tank_hit_near.wav");
    break;
  case bubbles:
    soundPlaySound(&lpDSBubbles, IDW_BUBBLES, "bubbles.wav");
    break;
  case tankSinkNear:
    soundPlaySound(&lpDSTankSinkingNear, IDW_TANK_SINKING_NEAR, "tank_sink_near.wav");
    break;
  case tankSinkFar:
    soundPlaySound(&lpDSTankSinkingFar, IDW_TANK_SINKING_FAR, "tank_sink_far.wav");
    break;
  case bigExplosionNear:
    soundPlaySound(&lpDSBigExplosionNear, IDW_BIG_EXPLOSION_NEAR, "explosion_near.wav");
    break;
  case bigExplosionFar:
    soundPlaySound(&lpDSBigExplosionFar, IDW_BIG_EXPLOSION_FAR, "explosion_far.wav");
    break;
  case farmingTreeNear:
    soundPlaySound(&lpDSFarmingTreeNear, IDW_FARMING_TREE_NEAR, "farm_near.wav");
    break;
  case farmingTreeFar:
    soundPlaySound(&lpDSFarmingTreeFar, IDW_FARMING_TREE_FAR, "farm_far.wav");
    break;
  case manBuildingNear:
    soundPlaySound(&lpDSManBuildingNear, IDW_MAN_BUILDING_NEAR, "build_near.wav");
    break;
  case manBuildingFar:
    soundPlaySound(&lpDSManBuildingFar, IDW_MAN_BUILDING_FAR, "build_far.wav");
    break;
  case manDyingNear:
    soundPlaySound(&lpDSManDyingNear, IDW_MAN_DYING_NEAR, "build_far.wav");
    break;
  case manDyingFar:
    soundPlaySound(&lpDSManDyingFar, IDW_MAN_DYING_FAR, "build_near.wav");
    break;
  case manLayingMineNear:
    soundPlaySound(&lpDSManLayingMineNear, IDW_MAN_LAYING_MINE_NEAR, "mine_near.wav");
    break;
  case mineExplosionNear:
    soundPlaySound(&lpDSMineExplosionNear, IDW_MINE_EXPLOSION_NEAR, "mine_explosion_near.wav");
    break;
  case mineExplosionFar:
    soundPlaySound(&lpDSMineExplosionFar, IDW_MINE_EXPLOSION_FAR, "mine_explosion_far.wav");
    break;
  default:
    /* shootFar */
    soundPlaySound(&lpDSShootFar, IDW_SHOOTING_FAR, "tank_shoot_far.wav");
    break;
  }

}

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
void soundISASoundCard(bool value) {
  if (value == TRUE) {
    lpDSPrimary->lpVtbl->Play(lpDSPrimary, 0, 0, DSBPLAY_LOOPING);
  } else {
    lpDSPrimary->lpVtbl->Stop(lpDSPrimary);
  }
}


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
bool soundIsPlayable() {
  return isPlayable;
}

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
IDirectSoundBuffer *soundLoadSound(IDirectSound *pDS, HMODULE hModule, LPCTSTR lpResourceName, LPCTSTR lpFileName) {
  IDirectSoundBuffer *returnValue; /* Sound created to return. */
  char fileName[MAX_PATH];         /* Filename to try opening */

  returnValue = NULL;

  /* Firstly try the skins zip */
  if (skinsIsLoaded() == TRUE) {
    skinsGetSkinDirectory(fileName);
    if (fileName[strlen(fileName)-1] != '\\') {
      strcat(fileName, "\\");
    }
    strcat(fileName, lpFileName);
    returnValue = DSLoadSoundBufferFile(pDS, fileName);
  }

  /* Nextly try the sounds directory */
  if (returnValue == NULL) {
    strcpy(fileName, "sounds");
    if (winUtilWBSubDirExist(fileName) == TRUE) {
      if (fileName[strlen(fileName)-1] != '\\') {
        strcat(fileName, "\\");
      }
      if (strlen(fileName) + strlen(lpFileName) < MAX_PATH) {
        strcat(fileName, lpFileName);
        returnValue = DSLoadSoundBufferFile(pDS, fileName);
      }
    }
  }

  /* Try loading internal if we failed everything else */
  if (returnValue == NULL) {
    returnValue =  DSLoadSoundBufferResource(pDS, hModule, lpResourceName);
  }

  return returnValue;
}

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
bool soundReloadSoundBuffer(LPDIRECTSOUNDBUFFER *value, HMODULE hModule, LPSTR resourceName, LPSTR lpzFileName) {
  char fileName[MAX_PATH];
  bool returnValue = FALSE;

  /* Firstly try the skins zip */
  if (skinsIsLoaded() == TRUE) {
    skinsGetSkinDirectory(fileName);
    if (fileName[strlen(fileName)-1] != '\\') {
      strcat(fileName, "\\");
    }
    strcat(fileName, lpzFileName);
    returnValue = DSReLoadSoundBufferFile(*value, lpzFileName);
  }

  strcpy(fileName, "sounds");
  if (winUtilWBSubDirExist(fileName) == TRUE) {
    if (fileName[strlen(fileName)-1] != '\\') {
      strcat(fileName, "\\");
    }
    if (strlen(fileName) + strlen(lpzFileName) < MAX_PATH) {
      strcat(fileName, lpzFileName);
    }
    returnValue = DSReLoadSoundBufferFile(*value, lpzFileName);
  }

  if (returnValue == FALSE) {
    returnValue = DSReloadSoundBuffer(*value, hModule, resourceName);
  }

  return returnValue;
}
