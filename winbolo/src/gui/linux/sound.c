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
*Last Modified:  13/6/00
*Purpose:
*  System Specific Sound Playing routines 
*  (Uses Direct Sound)
*********************************************************/

#include <stdio.h>
#include <unistd.h>
#include <glib.h>
#include "SDL.h"
#include "../../bolo/global.h"
#include "../../bolo/backend.h"
#include "bolosounds.h"
#include "messagebox.h"
#include "../lang.h"
#include "../linresource.h"

//#include "..\dsutil.h"
//#include "..\lang.h"
//#include "..\sound.h"

extern gchar *applicationPath;

#define MAX_CHANNELS 8

Mix_Chunk *lpDSPrimary = NULL;
Mix_Chunk *lpDSBigExplosionFar = NULL;
Mix_Chunk *lpDSBigExplosionNear = NULL;
Mix_Chunk *lpDSBubbles = NULL;
Mix_Chunk *lpDSFarmingTreeFar = NULL;
Mix_Chunk *lpDSFarmingTreeNear = NULL;
Mix_Chunk *lpDSHitTankFar = NULL;
Mix_Chunk *lpDSHitTankNear = NULL;
Mix_Chunk *lpDSHitTankSelf = NULL;
Mix_Chunk *lpDSManBuildingFar = NULL;
Mix_Chunk *lpDSManBuildingNear = NULL;
Mix_Chunk *lpDSManDyingFar = NULL;
Mix_Chunk *lpDSManDyingNear = NULL;
Mix_Chunk *lpDSManLayingMineNear = NULL;
Mix_Chunk *lpDSMineExplosionFar = NULL;
Mix_Chunk *lpDSMineExplosionNear = NULL;
Mix_Chunk *lpDSShootFar = NULL;
Mix_Chunk *lpDSShootNear = NULL;
Mix_Chunk *lpDSShootSelf = NULL;
Mix_Chunk *lpDSShotBuildingFar = NULL;
Mix_Chunk *lpDSShotBuildingNear = NULL;
Mix_Chunk *lpDSShotTreeFar = NULL;
Mix_Chunk *lpDSShotTreeNear = NULL;
Mix_Chunk *lpDSTankSinkingFar = NULL;
Mix_Chunk *lpDSTankSinkingNear = NULL;

bool isPlayable;


void windowDisableSound();

Mix_Chunk *soundLoad(char *tempFile, FILE *fp, BYTE *buff, int len) {
  Mix_Chunk *returnValue;
  FILE *out;
  
  fread(buff, len, 1, fp);
  out = fopen(tempFile, "wb");
  fwrite(buff, len, 1, out);
  fclose(out); 
  returnValue = Mix_LoadWAV(tempFile);
  return returnValue;
}

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
bool soundSetup() {
  bool returnValue;       /* Value to return */
  bool dsLoadOK;
  gchar *tmpPath;
  char fileName[FILENAME_MAX];
  char inFile[FILENAME_MAX];
  char *tf;
  FILE *in;
  BYTE *buff;

  buff = malloc(1024 * 100);
  /* Get tmp file */
  strcpy(fileName, g_get_tmp_dir());
  tf = tempnam(fileName, "lb");
  strcpy(fileName, tf);

  /* Get in path */
  strcpy(inFile, applicationPath);
  strcat(inFile, "/");
  strcat(inFile, "bolosounds.bsd");


  returnValue = TRUE;
  dsLoadOK = TRUE;
  isPlayable = TRUE;

  in = fopen(inFile, "rb");
  if (!in) {
//    windowDisableSound();
    isPlayable = FALSE;
  //  MessageBox(langGetText(STR_SOUNDERR_FILENOTFOUND), DIALOG_BOX_TITLE);
  }
	
  
  if (returnValue == TRUE && isPlayable == TRUE) {
    if (Mix_OpenAudio(22050, AUDIO_S16, 2, 512) < 0) {
      MessageBox("Couldn't start sound. Check to make sure\nthe device is not busy", DIALOG_BOX_TITLE);
      isPlayable = FALSE;
      returnValue = TRUE;
    }
  }
  
  if (returnValue == TRUE && isPlayable == TRUE) {
  /* Load Sound Files In */
    lpDSBigExplosionFar = soundLoad(fileName, in, buff, 31332);
    lpDSBigExplosionNear = soundLoad(fileName, in, buff, 31332);
    lpDSBubbles = soundLoad(fileName, in, buff, 7248);
    lpDSFarmingTreeFar = soundLoad(fileName, in, buff, 14480);
    lpDSFarmingTreeNear = soundLoad(fileName, in, buff, 14480);
    lpDSHitTankFar = soundLoad(fileName, in, buff, 6090);
    lpDSHitTankNear =soundLoad(fileName, in, buff, 6048);
    lpDSHitTankSelf = soundLoad(fileName, in, buff, 15632);
    lpDSManBuildingFar = soundLoad(fileName, in, buff, 13534);
    lpDSManBuildingNear = soundLoad(fileName, in, buff, 13534);
    lpDSManDyingFar = soundLoad(fileName, in, buff, 29424);
    lpDSManDyingNear = soundLoad(fileName, in, buff, 29424);
    lpDSManLayingMineNear = soundLoad(fileName, in, buff, 10672);
    lpDSMineExplosionFar = soundLoad(fileName, in, buff, 16336);
    lpDSMineExplosionNear = soundLoad(fileName, in, buff, 16336);
    lpDSShootFar = soundLoad(fileName, in, buff, 9512);
    lpDSShootNear = soundLoad(fileName, in, buff, 14556); 
    lpDSShootSelf = soundLoad(fileName, in, buff, 44464);
    lpDSShotBuildingFar = soundLoad(fileName, in, buff, 13936);
    lpDSShotBuildingNear = soundLoad(fileName, in, buff, 13936);
    lpDSShotTreeFar = soundLoad(fileName, in, buff, 18544);
    lpDSShotTreeNear = soundLoad(fileName, in, buff, 18544);
    lpDSTankSinkingFar = soundLoad(fileName, in, buff, 72252);
    lpDSTankSinkingNear = soundLoad(fileName, in, buff, 72252);
    if (lpDSBigExplosionFar == NULL || lpDSBigExplosionNear == NULL || lpDSBubbles == NULL || lpDSFarmingTreeFar == NULL || lpDSFarmingTreeNear == NULL || lpDSHitTankFar == NULL || lpDSHitTankNear == NULL || lpDSHitTankSelf == NULL || lpDSManBuildingFar == NULL || lpDSManBuildingNear == NULL || lpDSManDyingFar == NULL || lpDSManDyingNear == NULL || lpDSManLayingMineNear == NULL || lpDSMineExplosionFar == NULL || lpDSMineExplosionNear == NULL || lpDSShootFar == NULL || lpDSShootNear == NULL || lpDSShootSelf == NULL || lpDSShotBuildingFar == NULL || lpDSShotBuildingNear == NULL || lpDSShotTreeFar == NULL || lpDSShotTreeNear == NULL || lpDSTankSinkingFar == NULL || lpDSTankSinkingNear == NULL) {
      returnValue = FALSE;
      MessageBox(langGetText(STR_SOUNDERR_LOADSOUNDFAILED), DIALOG_BOX_TITLE);
    }

  }

  if (dsLoadOK == FALSE) {
    returnValue = TRUE;
  }
  if (returnValue == FALSE) {
    isPlayable = FALSE;
//    windowDisableSound();
  }
  unlink(fileName);
  free(buff);
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
    Mix_FreeChunk(lpDSBigExplosionFar);
    lpDSBigExplosionFar = NULL;
  }
  if (lpDSBigExplosionNear != NULL) {
    Mix_FreeChunk(lpDSBigExplosionNear);
    lpDSBigExplosionNear = NULL;
  }
  if (lpDSBubbles != NULL) {
    Mix_FreeChunk(lpDSBubbles);
    lpDSBubbles = NULL;
  }
  if (lpDSFarmingTreeFar != NULL) {
    Mix_FreeChunk(lpDSFarmingTreeFar);
    lpDSFarmingTreeFar = NULL;
  }
  if (lpDSFarmingTreeNear != NULL) {
    Mix_FreeChunk(lpDSFarmingTreeNear);
    lpDSFarmingTreeNear = NULL;
  }
  if (lpDSHitTankFar != NULL) {
    Mix_FreeChunk(lpDSHitTankFar);
    lpDSHitTankFar = NULL;
  }
  if (lpDSHitTankNear != NULL) {
    Mix_FreeChunk(lpDSHitTankNear);
    lpDSHitTankNear = NULL;
  }
  if (lpDSHitTankSelf != NULL) {
    Mix_FreeChunk(lpDSHitTankSelf);
    lpDSHitTankSelf = NULL;
  }
  if (lpDSManBuildingFar != NULL) {
    Mix_FreeChunk(lpDSManBuildingFar);
    lpDSManBuildingFar = NULL;
  }
  if (lpDSManBuildingNear != NULL) {
    Mix_FreeChunk(lpDSManBuildingNear);
    lpDSManBuildingNear = NULL;
  }
  if (lpDSManDyingFar != NULL) {
    Mix_FreeChunk(lpDSManDyingFar);
    lpDSManDyingFar = NULL;
  }
  if (lpDSManDyingNear != NULL) {
    Mix_FreeChunk(lpDSManDyingNear);
    lpDSManDyingNear = NULL;
  }

  if (lpDSManLayingMineNear != NULL) {
    Mix_FreeChunk(lpDSManLayingMineNear);
    lpDSManLayingMineNear = NULL;
  }
  if (lpDSMineExplosionFar != NULL) {
    Mix_FreeChunk(lpDSMineExplosionFar);
    lpDSMineExplosionFar = NULL;
  }
  if (lpDSMineExplosionNear != NULL) {
    Mix_FreeChunk(lpDSMineExplosionNear);
    lpDSMineExplosionNear = NULL;
  }
  if (lpDSShotBuildingFar != NULL) {
    Mix_FreeChunk(lpDSShotBuildingFar);
    lpDSShotBuildingFar = NULL;
  }
  if (lpDSShotBuildingNear != NULL) {
    Mix_FreeChunk(lpDSShotBuildingNear);
    lpDSShotBuildingNear = NULL;
  }
  if (lpDSShotTreeFar != NULL) {
    Mix_FreeChunk(lpDSShotTreeFar);
    lpDSShotTreeFar = NULL;
  }
  if (lpDSShotTreeNear != NULL) {
    Mix_FreeChunk(lpDSShotTreeNear);
    lpDSShotTreeNear = NULL;
  }
  if (lpDSTankSinkingFar != NULL) {
    Mix_FreeChunk(lpDSTankSinkingFar);
    lpDSTankSinkingFar = NULL;
  }
  if (lpDSTankSinkingNear != NULL) {
    Mix_FreeChunk(lpDSTankSinkingNear);
    lpDSTankSinkingNear = NULL;
  }
  if (lpDSShootFar != NULL) {
    Mix_FreeChunk(lpDSShootFar);
    lpDSShootFar = NULL;
  }  
  if (lpDSShootNear != NULL) {
    Mix_FreeChunk(lpDSShootNear);
    lpDSShootNear = NULL;
  }
  if (lpDSShootSelf != NULL) {
    Mix_FreeChunk(lpDSShootSelf);
    lpDSShootSelf = NULL;
  }

  if (isPlayable == TRUE) {
    Mix_CloseAudio();
    isPlayable = FALSE;
  }
}


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
void soundPlaySound(Mix_Chunk *value, char *name, int channel) {
  static int useChannel = 0;
  if (channel == -1) {
    Mix_PlayChannel(useChannel, value, 0);
    useChannel++;
    if (useChannel == 4 /* MAX_CHANNELS */) {
      useChannel = 0;
    }
  } else {
    Mix_PlayChannel(channel, value, 0);
  }
}

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
void soundPlayEffect(sndEffects value) {  
  switch (value) {
  case shootSelf:
    soundPlaySound(lpDSShootSelf, IDW_SHOOTING_SELF, -1);
    break;
  case shootNear:
    soundPlaySound(lpDSShootNear, IDW_SHOOTING_NEAR, -1);
    break;
  case shotTreeNear:
    soundPlaySound(lpDSShotTreeNear, IDW_SHOT_TREE_NEAR, -1);
    break;
  case shotTreeFar:
    soundPlaySound(lpDSShotTreeFar, IDW_SHOT_TREE_FAR, -1);
    break;
  case shotBuildingNear:
    soundPlaySound(lpDSShotBuildingNear, IDW_SHOT_BUILDING_NEAR, -1);
    break;
  case shotBuildingFar:
    soundPlaySound(lpDSShotBuildingFar, IDW_SHOT_BUILDING_FAR, -1);
    break;
  case hitTankFar:
    soundPlaySound(lpDSHitTankFar, IDW_HIT_TANK_FAR, -1);
    break;
  case hitTankNear:
    soundPlaySound(lpDSHitTankNear, IDW_HIT_TANK_NEAR, -1 );
    break;
  case hitTankSelf:
    soundPlaySound(lpDSHitTankSelf, IDW_HIT_TANK_SELF, -1);
    break;
  case bubbles:
    soundPlaySound(lpDSBubbles, IDW_BUBBLES, -1);
    break;
  case tankSinkNear:
    soundPlaySound(lpDSTankSinkingNear, IDW_TANK_SINKING_NEAR, -1);
    break;
  case tankSinkFar:
    soundPlaySound(lpDSTankSinkingFar, IDW_TANK_SINKING_FAR, -1);
    break;
  case bigExplosionNear:
    soundPlaySound(lpDSBigExplosionNear, IDW_BIG_EXPLOSION_NEAR, -1);
    break;
  case bigExplosionFar:
    soundPlaySound(lpDSBigExplosionFar, IDW_BIG_EXPLOSION_FAR, -1);
    break;
  case farmingTreeNear:
    soundPlaySound(lpDSFarmingTreeNear, IDW_FARMING_TREE_NEAR, -1);
    break;
  case farmingTreeFar:
    soundPlaySound(lpDSFarmingTreeFar, IDW_FARMING_TREE_FAR, -1);
    break;
  case manBuildingNear:
    soundPlaySound(lpDSManBuildingNear, IDW_MAN_BUILDING_NEAR, -1);
    break;
  case manBuildingFar:
    soundPlaySound(lpDSManBuildingFar, IDW_MAN_BUILDING_FAR, -1);
    break;
  case manDyingNear:
    soundPlaySound(lpDSManDyingNear, IDW_MAN_DYING_NEAR, -1);
    break;
  case manDyingFar:
    soundPlaySound(lpDSManDyingFar, IDW_MAN_DYING_FAR, -1);
    break;
  case manLayingMineNear:
    soundPlaySound(lpDSManLayingMineNear, IDW_MAN_LAYING_MINE_NEAR, -1);
    break;
  case mineExplosionNear:
    soundPlaySound(lpDSMineExplosionNear, IDW_MINE_EXPLOSION_NEAR, -1);
    break;
  case mineExplosionFar:
    soundPlaySound(lpDSMineExplosionFar, IDW_MINE_EXPLOSION_FAR, -1);
    break;
  default:
    /* shootFar */
    soundPlaySound(lpDSShootFar, IDW_SHOOTING_FAR, -1);
    break;
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
