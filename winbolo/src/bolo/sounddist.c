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
*Name:          Sound Distancing
*Filename:      sounddist.c
*Author:        John Morrison
*Creation Date: 19/01/99
*Last Modified: 05/05/01
*Purpose:
*  Responsible for differentiating between playing soft
*  sound effects and loud sound effects.
*********************************************************/

#include "global.h"
#include "screen.h"
#include "frontend.h"
#include "sounddist.h"
#include "log.h"

/*********************************************************
*NAME:          clientSoundDist
*AUTHOR:        John Morrison
*CREATION DATE: 19/01/99
*LAST MODIFIED: 05/05/01
*PURPOSE:
*  Calculates whether a soft sound of a loud sound should 
*  be played and passes paremeters to frontend
*
*ARGUMENTS:
*  value - Sound effect to be played
*  mx    - Map X co-ordinatate for the sound origin
*  my    - Map Y co-ordinatate for the sound origin
*********************************************************/
void clientSoundDist(sndEffects value, BYTE mx, BYTE my) {
  BYTE tankX; /* Tank X Map Co-ordinate */
  BYTE tankY; /* Tank Y Map Co-ordinate */
  BYTE gapX;  /* Distance from tank to sound */
  BYTE gapY;

  if (logIsRecording() == TRUE) {
    soundDistLog(value, mx, my);
  }
  if (mineExplosionNear == value) {
    gapY = 0;
  }
  screenGetTankMapCoord(&tankX, &tankY);
  /* Get gap */
  if ((tankX - mx) < 0) {
    gapX = mx - tankX;
  } else {
    gapX = tankX - mx;
  }

  if ((tankY - my) < 0) {
    gapY = my - tankY;
  } else {
    gapY = tankY - my;
  }

  if (gapY < SDIST_NONE && gapX < SDIST_NONE) {
  /* Determine whether loud/soft sound should be played */
    switch (value) {
    case shootNear:
      if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
        frontEndPlaySound(shootFar);
      } else { 
        frontEndPlaySound(shootNear);
      }
      break;
    case shotTreeNear:
      if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
        frontEndPlaySound(shotTreeFar);
      } else { 
        frontEndPlaySound(shotTreeNear);
      }
      break;
    case shotBuildingNear:
      if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
        frontEndPlaySound(shotBuildingFar);
      } else { 
        frontEndPlaySound(shotBuildingNear);
      }
      break;
    case hitTankNear:
      if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
        frontEndPlaySound(hitTankFar);
      } else { 
        frontEndPlaySound(hitTankNear);
      }
      break;
    case tankSinkNear:
      if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
        frontEndPlaySound(tankSinkFar);
      } else { 
        frontEndPlaySound(tankSinkNear);
      }
      break;
    case bigExplosionNear:
      if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
        frontEndPlaySound(bigExplosionFar);
      } else { 
        frontEndPlaySound(bigExplosionNear);
      }
      break;
    case farmingTreeNear:
      if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
        frontEndPlaySound(farmingTreeFar);
      } else { 
        frontEndPlaySound(farmingTreeNear);
      }
      break;
    case manBuildingNear:
      if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
        frontEndPlaySound(manBuildingFar);
      } else { 
        frontEndPlaySound(manBuildingNear);
      }
      break;
    case manDyingNear:
      if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
        frontEndPlaySound(manDyingFar);
      } else { 
        frontEndPlaySound(manDyingNear);
      }
      break;
    case mineExplosionNear:
      if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
        frontEndPlaySound(mineExplosionFar);
      } else { 
        frontEndPlaySound(mineExplosionNear);
      }
      break;
    case manLayingMineNear:
      if (gapX <= SDIST_SOFT || gapY <= SDIST_SOFT) {
        frontEndPlaySound(manLayingMineNear);
      }
      break;
    case shootSelf:
    case shotTreeFar:
    case shotBuildingFar:
    case hitTankFar:
    case hitTankSelf:
    case manDyingFar:
    case mineExplosionFar:
    case bubbles:
    case tankSinkFar:
    case bigExplosionFar:
    case farmingTreeFar:
    case manBuildingFar:
    case shootFar:
    default:
      break;
    }
  }
}

/*********************************************************
*NAME:          soundDistLog
*AUTHOR:        John Morrison
*CREATION DATE: 05/05/01
*LAST MODIFIED: 05/05/01
*PURPOSE:
*  Calculates the item to be logged
*
*ARGUMENTS:
*  value - Sound effect to be played
*  mx    - Map X co-ordinatate for the sound origin
*  my    - Map Y co-ordinatate for the sound origin
*********************************************************/
void soundDistLog(sndEffects value, BYTE mx, BYTE my) {
  BYTE logMessageType; /* Log item type */
  bool wantLog;        /* Do we want to log this? */
  
  wantLog = TRUE;
  switch (value) {
  case shootSelf:
  case shootNear:
  case shootFar:
    logMessageType = log_SoundShoot;
    break;

  case shotTreeNear:
  case shotTreeFar:
    logMessageType = log_SoundFarm;
    break;

  case shotBuildingNear:
  case shotBuildingFar:
//    logMessageType = log_SoundHit;
    break;

  case hitTankNear:
  case hitTankFar:
  case hitTankSelf:
    wantLog = FALSE;
    break;
  case bubbles:
  case tankSinkNear:
  case tankSinkFar:
    wantLog = FALSE;
    break;
  case bigExplosionNear:
    logMessageType = log_SoundExplosion;
    break;
  case bigExplosionFar:
    logMessageType = log_SoundExplosion;
    break;
  case farmingTreeNear:
  case farmingTreeFar:
    logMessageType = log_SoundFarm;
    break;
  case manBuildingNear:
  case manBuildingFar:
    logMessageType = log_SoundBuild;
    break;
  case manDyingNear:
  case manDyingFar:
    logMessageType = log_SoundManDie;
    break;
  
  case manLayingMineNear:
    logMessageType = log_SoundMineLay;
    break;

  case mineExplosionNear:
  case mineExplosionFar:
    logMessageType = log_SoundMineExplode;
    break;
  }

  if (wantLog == TRUE) {
    logAddEvent(logMessageType, mx, my, 0, 0, 0, NULL);
  }

}
