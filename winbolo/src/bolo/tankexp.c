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
*Name:          TankExplosions
*Filename:      tankexp.c
*Author:        John Morrison
*Creation Date: 15/1/99
*Last Modified: 04/04/02
*Purpose:
*  Responsable for moving dead tank explosions
*********************************************************/


#include "global.h"
#include "tank.h"
#include "screen.h"
#include "explosions.h"
#include "util.h"
#include "messages.h"
#include "frontend.h"
#include "building.h"
#include "grass.h"
#include "rubble.h"
#include "swamp.h"
#include "lgm.h"
#include "floodfill.h"
#include "sounddist.h"
#include "players.h"
#include "tankexp.h"

/*********************************************************
*NAME:          tkExplosionCreate
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Sets up the tkExplosion data structure
*
*ARGUMENTS:
*  tke - Pointer to the tank explosions object
*********************************************************/
void tkExplosionCreate(tkExplosion *tke) {
  *tke = NULL;
}

/*********************************************************
*NAME:          tkExplosionDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Destroys and frees memory for the tkExplosion data 
*  structure
*
*ARGUMENTS:
*  tke - Pointer to the tank explosions object
*********************************************************/
void tkExplosionDestroy(tkExplosion *tke) {
  tkExplosion q;

  while (!IsEmpty(*tke)) {
    q = *tke;
    *tke = TkExplosionTail(q);
    Dispose(q);
  }
}

/*********************************************************
*NAME:          tkExplosionAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
*  Adds an item to the tkExplosion data structure. 
*
*ARGUMENTS:
*  tke         - Pointer to the tank explosions object
*  x           - World X Co-orindate
*  y           - World X Co-orindate
*  angle       - Angle of travel
*  length      - Length of travel 
*  explodeType - Type of explosions (big - small)
*********************************************************/
void tkExplosionAddItem(tkExplosion *tke, WORLD x, WORLD y, TURNTYPE angle, BYTE length, BYTE explodeType) {
  tkExplosion q;
  
  New (q);
  q->own = TRUE;
  q->x = x;
  q->y = y;
  q->angle = angle;
  q->length = length;
  q->next = *tke;
  q->explodeType = explodeType;
  q->creator = playersGetSelf(screenGetPlayers());
  q->packSent = FALSE;
  q->prev = NULL;
  if (NonEmpty(*tke)) {
    (*tke)->prev = q;
  }

  *tke = q;
}

/*********************************************************
*NAME:          tkExplosionUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 15/01/99
*LAST MODIFIED: 04/04/02
*PURPOSE:
*  Updates each tkExplosion position.
*
*ARGUMENTS:
*  tke    - Pointer to the tank explosions object
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the bases structure
*  lgms   - Array of lgms
*  numLgm - Number of lgms in the array
*  tank   - Pointer to the tank object
*********************************************************/
void tkExplosionUpdate(tkExplosion *tke, map *mp, pillboxes *pb, bases *bs, lgm **lgms, BYTE numLgm, tank *tank) {
  static BYTE updateTime=0; /* Time between updates */
  tkExplosion position;     /* Position throught the items */
  bool needUpdate;          /* Whether an update is needed or not */
  int moveX;                /* Amount to move */
  int moveY;
  WORLD conv;               /* Used in world-> map conversions */
  WORLD newX;               /* New positions */
  WORLD newY;
  BYTE mx;                  /* When a item is destoyed this is its map squares */
  BYTE my;
  BYTE px;                  /* Pixel x & y co-ordinates */
  BYTE py;
  BYTE newmx;
  BYTE newmy;
  BYTE currentPos;          /* Current map square terrain */
  bool isServer;            /* Are we server */
  BYTE playerNum;           /* Our player number */
  BYTE testX;               /* Screen Move Position Checks */
  BYTE testY;
  BYTE count;               /* Looping variable */



  /* Update only so often - Not every game tick */
  updateTime++;
  if (updateTime < TK_UPDATE_TIME) {
    return;
  }

  testX= 0;
  testY = 0;
  updateTime = 0;
  playerNum = playersGetSelf(screenGetPlayers());
  isServer = threadsGetContext();
  position = *tke;

  while (NonEmpty(position)) {
    needUpdate = TRUE;
    utilCalcDistance(&moveX, &moveY, position->angle, TK_MOVE_AMOUNT);
    if (position->length > TK_EXPLODE_DEATH) {
      /* Add the "flame trail" */
      conv = position->x;
      conv >>= TANK_SHIFT_MAPSIZE;
      mx = (BYTE) conv;
      conv = position->y;
      conv >>= TANK_SHIFT_MAPSIZE;
      my = (BYTE) conv;
      conv = position->x;
      conv <<= TANK_SHIFT_MAPSIZE;
      conv >>= TANK_SHIFT_PIXELSIZE;
      px = (BYTE) conv;
      conv = position->y;
      conv <<= TANK_SHIFT_MAPSIZE;
      conv >>= TANK_SHIFT_PIXELSIZE;
      py = (BYTE) conv;
      explosionsAddItem(screenGetExplosions(), mx, my, px, py,EXPLOSION_START);
      /* Check for colisions then update position */
      newX = (WORLD) (position->x + moveX);
      newY = (WORLD) (position->y + moveY);

      /*  Moving screen position check */
      if (isServer == FALSE) {
        conv = newX;
        conv >>= TANK_SHIFT_MAPSIZE;
        testX = (BYTE) conv;
        conv = newY;
        conv >>= TANK_SHIFT_MAPSIZE;
        testY = (BYTE) conv;
      }
      
      /* Collision Test */
      if (newX > 0) {
        newX += TK_WIDTH_CHECK;
      } else {
        newX -= TK_WIDTH_CHECK;
      }
      if (newY > 0) {
        newY += TK_HEIGHT_CHECK;
      } else {
        newY -= TK_HEIGHT_CHECK;
      }

      conv = newX;
      conv >>= TANK_SHIFT_MAPSIZE;
      newmx = (BYTE) conv;
      conv = newY;
      conv >>= TANK_SHIFT_MAPSIZE;
      newmy = (BYTE) conv;

      if ((mapGetSpeed(mp,pb,bs,mx,newmy, TRUE, NEUTRAL)) > 0) {
        position->y = (WORLD) (position->y + moveY);
        if (isServer == FALSE && position->creator == playerNum) {
          if (testY > my) {
            screenMoveViewOffsetUp(FALSE);
          } else if (testY < my) {
            screenMoveViewOffsetUp(TRUE);
          } else {
            my = my;
          }
        }
        my = newmy;
      } 
      if ((mapGetSpeed(mp,pb,bs,newmx,my, TRUE, NEUTRAL)) > 0) {
        position->x = (WORLD) (position->x + moveX);
        if (isServer == FALSE && position->creator == playerNum) {
          if (testX > mx) {
            screenMoveViewOffsetLeft(FALSE);
          } else if (testX < mx) {
            screenMoveViewOffsetLeft(TRUE);
          }
        }
        mx = newmx;
      } 
      /* Update the length */
      position->length--;
      /* Check for damage to stuff etc. */
      currentPos = mapGetPos(mp,mx,my);
      if (currentPos == DEEP_SEA) {
        /* Check for deep sea death */
        needUpdate = FALSE;
        if (position->own == TRUE) {
		  if (isServer == FALSE) {
            soundDist(tankSinkNear, mx, my);
		    tankSetLastTankDeath(tank,LAST_DEATH_BY_DEEPSEA); /* Override LAST_DEATH_BY_SHELL */
            messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(MESSAGE_TANKSUNK));
		  }
        }
        tkExplosionDeleteItem(tke, &position);
      } else if (currentPos == FOREST) {
        /* Check for destroy trees */
        mapSetPos(mp, mx, my, GRASS, FALSE, FALSE);
        soundDist(shotTreeNear, mx, my);
      } else if (currentPos == BOAT) {
        /* Check for destroy boat */
        mapSetPos(mp, mx, my, RIVER, FALSE, FALSE);
        soundDist(shotBuildingNear, mx, my);
      }
      screenReCalc();
    } else {
      /* Remove from data structure */
      needUpdate = FALSE;
      conv = position->x;
      conv >>= TANK_SHIFT_MAPSIZE;
      mx = (BYTE) conv;
      conv = position->y;
      conv >>= TANK_SHIFT_MAPSIZE;
      my = (BYTE) conv;
      if (position->explodeType == TK_SMALL_EXPLOSION) {
        explosionsAddItem(screenGetExplosions(), mx, my, 0, 0 ,EXPLOSION_START);
        currentPos = mapGetPos(mp, mx, my);
        if (currentPos != RIVER && currentPos != DEEP_SEA) {
//          if (position->own == TRUE) {
            mapSetPos(mp, mx, my, CRATER, FALSE, FALSE);
            floodAddItem(screenGetFloodFill(), mx, my);
            screenReCalc();
  //        }
        }
        count = 1;
        while (count < numLgm) {
          lgmDeathCheck(lgms[count-1], mp, pb, bs, position->x, position->y, NEUTRAL);
          count++;
        }
        soundDist(mineExplosionNear, mx, my);
      } else {
        tkExplosionBigExplosion(tke, mp, pb, bs, mx, my, moveX, moveY, position->own, lgms, numLgm);
      }
      tkExplosionDeleteItem(tke, &position);
    }    
    /* Get the next Item */
    if (*tke != NULL && needUpdate == TRUE) {
      position = TkExplosionTail(position);
    }
  }
}


/*********************************************************
*NAME:          tkExplosionDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Deletes the item for the given number
*
*ARGUMENTS:
*  tke     - Pointer to the tank explosions object
*  itemNum - The item number to get
*********************************************************/
void tkExplosionDeleteItem(tkExplosion *tke, tkExplosion *value) {
  tkExplosion del;  /* The item to delete */
  
  del = *value;
  (*value) = TkExplosionTail(del);
  if (del->prev != NULL) {
    del->prev->next = del->next;
  } else {
    /* Must be the first item - Move the master position along one */
    *tke = TkExplosionTail(*tke);
    if (NonEmpty(*tke)) {
      (*tke)->prev = NULL;
    }
  }

  if (del->next != NULL) {
    del->next->prev = del->prev;
  }
  Dispose(del);

}

/*********************************************************
*NAME:          tkExplosionCalcScreenBullets
*AUTHOR:        John Morrison
*CREATION DATE: 15/1/98
*LAST MODIFIED: 15/1/98
*PURPOSE:
*  Adds items to the sceenBullets data structure if they
*  are on screen
*
*ARGUMENTS:
*  tke       - Pointer to the tank explosions object
*  sBullet   - The screenBullets Data structure
*  leftPos   - X Map offset start
*  rightPos  - X Map offset end
*  topPos    - Y Map offset end
*  bottomPos - Y Map offset end
*********************************************************/
void tkExplosionCalcScreenBullets(tkExplosion *tke, screenBullets *sBullets, BYTE leftPos, BYTE rightPos, BYTE topPos, BYTE bottomPos) {
  tkExplosion q; /* Temp Pointer */
  WORLD conv;    /* Used for conversions */ 
  BYTE mx;       /* Map Positions */
  BYTE my; 
  BYTE px;       /* Pixel Positions */
  BYTE py; 

  q = *tke;
  while (NonEmpty(q)) {
    conv = q->x;
    conv >>= TANK_SHIFT_MAPSIZE;
    mx = (BYTE) conv;
    conv = q->y;
    conv >>= TANK_SHIFT_MAPSIZE;
    my = (BYTE) conv;
    if (mx >= leftPos && mx < rightPos && my >= topPos && my < bottomPos) {
      conv = q->x;
      conv <<= TANK_SHIFT_MAPSIZE;
      conv >>= TANK_SHIFT_PIXELSIZE;  
      px = (BYTE) conv;
      conv = q->y;
      conv <<= TANK_SHIFT_MAPSIZE;
      conv >>= TANK_SHIFT_PIXELSIZE;
      py = (BYTE) conv;

      screenBulletsAddItem(sBullets, (BYTE) (mx-leftPos), (BYTE) (my-topPos), px, py, TANK_EXPLOSION_FRAME);
    }
    q = TkExplosionTail(q);
  }
}


/*********************************************************
*NAME:          tkExplosionCheckRemove
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/98
*LAST MODIFIED: 18/1/98
*PURPOSE:
*  An explosion has happened. Check to see if it should
*  remove from grass/building data structures etc.
*
*ARGUMENTS:
*  tke     - Pointer to the tank explosions object
*  terrain - Terrain type of the sqaure
*  mx      - Map X position 
*  my      - Map Y position
*********************************************************/
void tkExplosionCheckRemove(tkExplosion *tke, BYTE terrain, BYTE mx, BYTE my) {
  switch (terrain) {
  case BUILDING:
    buildingRemovePos(screenGetBuildings(), mx,my);
    break;
  case GRASS:
    grassRemovePos(screenGetGrass(), mx, my);
    break;
  case RUBBLE:
    rubbleRemovePos(screenGetRubble(), mx, my);
    break;
  case SWAMP:
    swampRemovePos(screenGetSwamp(), mx, my);
    break;
  default:
    /* Do nothing */
    break;
  }
}

/*********************************************************
*NAME:          tkExplosionBigExplosion
*AUTHOR:        John Morrison
*CREATION DATE: 18/01/98
*LAST MODIFIED: 04/04/02
*PURPOSE:
*  An explosion has happened. do all the work involved 
*  to do it.
*
*ARGUMENTS:
*  tke     - Pointer to the tank explosions object
*  mp      - Pointer to map structure 
*  pb      - Pointer to pillboxes structure
*  bs      - Pointer to bases strucutre
*  mx      - Map X position 
*  my      - Map Y position
*  moveX   - Moving X direction (positive/Negative)
*  moveY   - Moving Y direction (positive/Negative)
*  own     - Do we own this tkExplosion?
*  lgms   - Array of lgms
*  numLgm - Number of lgms in the array
*********************************************************/
void tkExplosionBigExplosion(tkExplosion *tke, map *mp, pillboxes *pb, bases *bs, BYTE mx, BYTE my, int moveX, int moveY, bool own, lgm **lgms, BYTE numLgm) {
  BYTE currentPos; /* Current position */
  BYTE count;      /* Looping variable */

  if (moveX > 0) {
    moveX = TK_MOVE_RIGHT;
  } else {
    moveX = TK_MOVE_LEFT;
  }
  if (moveY > 0) {
    moveY = TK_MOVE_RIGHT;
  } else {
    moveY = TK_MOVE_LEFT;
  }
  explosionsAddItem(screenGetExplosions(), (BYTE) (mx+moveX), (BYTE) (my+moveY), 0, 0,EXPLOSION_START);
  currentPos = mapGetPos(mp, (BYTE) (mx+moveX), (BYTE) (my+moveY));
  tkExplosionCheckRemove(tke, currentPos, (BYTE) (mx + moveX), (BYTE) (my +moveY));
  if (pillsExistPos(pb, (BYTE) (mx+moveX), (BYTE) (my + moveY))) {
    pillsGetDamagePos(pb, (BYTE) (mx+moveX), (BYTE) (my+moveY), TK_DAMAGE);
  } else if (currentPos != BOAT && currentPos != RIVER && currentPos != DEEP_SEA) {
//    if (own == TRUE) {
      mapSetPos(mp,(BYTE) (mx+moveX), (BYTE) (my+moveY), CRATER, FALSE, FALSE);
      floodAddItem(screenGetFloodFill(), (BYTE) (mx+moveX), (BYTE) (my+moveY));
//    }
  }

  count = 1;
  while (count <= numLgm) {
    lgmDeathCheck(lgms[count-1], mp, pb, bs, (WORLD) (((WORLD) (mx+moveX) << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), (WORLD) (((WORLD) (my + moveY)<< M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), NEUTRAL);
    count++;
  }
    
  explosionsAddItem(screenGetExplosions(), (BYTE) (mx+moveX), my, 0, 0,EXPLOSION_START);
  currentPos = mapGetPos(mp, (BYTE) (mx+moveX), my);
  tkExplosionCheckRemove(tke, currentPos, (BYTE) (mx + moveX), my);
  if (pillsExistPos(pb, (BYTE) (mx+moveX), my)) {
    pillsGetDamagePos(pb, (BYTE) (mx+moveX), my, TK_DAMAGE);
  } else if (currentPos != BOAT && currentPos != RIVER && currentPos != DEEP_SEA) {
  //  if (own == TRUE) {
      mapSetPos(mp,(BYTE) (mx+moveX), my, CRATER, FALSE, FALSE);
//    }
    floodAddItem(screenGetFloodFill(), (BYTE) (mx+moveX), my);
  }
  count = 1;
  while (count <= numLgm) {
    lgmDeathCheck(lgms[count-1], mp, pb, bs, (WORLD) (((WORLD) (mx+moveX) << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), (WORLD) ((my<< M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), NEUTRAL);
    count++;
  }
  
  explosionsAddItem(screenGetExplosions(), mx, (BYTE) (my+moveY), 0, 0,EXPLOSION_START);
  currentPos = mapGetPos(mp, mx, (BYTE) (my+moveY));
  tkExplosionCheckRemove(tke, currentPos, mx, (BYTE) (my +moveY));
  if (pillsExistPos(pb, mx, (BYTE) (my + moveY))) {
    pillsGetDamagePos(pb, mx, (BYTE) (my + moveY), TK_DAMAGE);
  } else if (currentPos != BOAT && currentPos != RIVER && currentPos != DEEP_SEA) {
//    if (own == TRUE) {
      mapSetPos(mp, mx, (BYTE) (my+moveY), CRATER, FALSE, FALSE);
  //  }
    floodAddItem(screenGetFloodFill(), mx, (BYTE) (my+moveY));
  }
  
  count = 1;
  while (count <= numLgm) {
    lgmDeathCheck(lgms[count-1], mp, pb, bs, (WORLD) ((mx<< M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), (WORLD) (((my+moveY)<< M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), NEUTRAL);
    count++;
  }
    
  explosionsAddItem(screenGetExplosions(), mx, my, 0, 0,EXPLOSION_START);
  count = 1;
  while (count <= numLgm) {
    lgmDeathCheck(lgms[count-1], mp, pb, bs, (WORLD) ((WORLD) (mx << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), (WORLD) ((WORLD) (my << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), NEUTRAL);
    count++;
  }
  
  currentPos = mapGetPos(mp, mx, my);
  tkExplosionCheckRemove(tke, currentPos, mx, my);
  if (pillsExistPos(pb, mx, my)) {
    pillsGetDamagePos(pb, mx, my, TK_DAMAGE);
  } else if (currentPos != BOAT && currentPos != RIVER && currentPos != DEEP_SEA) {
//    if (own == TRUE) {
      mapSetPos(mp, mx, my, CRATER, FALSE, FALSE);
  //  }
    floodAddItem(screenGetFloodFill(), mx, my);
  }
  soundDist(bigExplosionNear, mx, my);
  screenReCalc();
}

/*********************************************************
*NAME:          tkExplosionNetMake
*AUTHOR:        John Morrison
*CREATION DATE: 11/3/99
*LAST MODIFIED: 11/3/99
*PURPOSE:
*  When we have the token we inform all the players of
*  tke we have made since last time we had the token.
*  Returns the length of the data created
*  
*ARGUMENTS:
*  tke   - Pointer to the tank explosions object
*  buff  - Pointer to a buffer to hold the shells 
*          net data
*********************************************************/
BYTE tkExplosionNetMake(tkExplosion *tke, BYTE *buff) {
  BYTE returnValue;  /* Value to return */
  unsigned int ttsz; /* Size of turntype type */
  unsigned int wsz;  /* Size of world type */
  BYTE *pnt;         /* Pointer to offset in the buffer */
  tkExplosion q;     /* Temp pointer to the shells structure */

  ttsz = sizeof(TURNTYPE);
  wsz = sizeof(WORLD);
  returnValue = 0;
  pnt = buff;
  q = *tke;

  while (NonEmpty(q)) {
    if (q->packSent == FALSE) {
      /* Need to add */
      memcpy(pnt, &(q->x), wsz); /* X */
      pnt += wsz;
      returnValue = (BYTE) (returnValue + wsz);
      memcpy(pnt, &(q->y), wsz); /* Y */
      pnt += wsz;
      returnValue = (BYTE) (returnValue + wsz);
      memcpy(pnt, &(q->angle), ttsz); /* Angle */
      pnt += ttsz;
      returnValue = (BYTE) (returnValue + ttsz);
      *pnt = q->length; /* Length */
      pnt++;
      returnValue++;
      *pnt = q->explodeType; /* Explode Type */
      pnt++;
      returnValue++;
      *pnt = q->creator;
      pnt++;
      returnValue++;
      /* We have now sent it */
      q->packSent = TRUE;
    }
    q = TkExplosionTail(q);
  }

  return returnValue;
}

/*********************************************************
*NAME:          tkExplosionNetExtract
*AUTHOR:        John Morrison
*CREATION DATE: 11/3/99
*LAST MODIFIED: 14/9/00
*PURPOSE:
* Network tke data have arrived. Add them to our 
* tke structure here.
*  
*ARGUMENTS:
*  tke       - Pointer to the tank explosions object
*  buff      - Pointer to a buffer to hold the shells 
*              net data
*  dataLen   - Length of the data
*  playerNum - This players number 
*********************************************************/
void tkExplosionNetExtract(tkExplosion *tke, BYTE *buff, BYTE dataLen, BYTE playerNum) {
  BYTE pos;          /* Position through the data we are */
  unsigned int ttsz; /* Size of turntype type */
  unsigned int wsz;  /* Size of world type */
  BYTE *pnt;         /* Pointer to offset in the buffer */
  tkExplosion q;     /* Temp pointer to hold additions to the tkExplosion structure */
  bool isServer;     /* Are we server */
  bool isSent;
  /* An Item */
  WORLD wx;
  WORLD wy;
  TURNTYPE tt;
  BYTE length;
  BYTE expType;
  BYTE creator;


  isServer = threadsGetContext();
  if (isServer == TRUE) {
    isSent = FALSE;
  } else {
    isSent = TRUE;
  }

  ttsz = sizeof(TURNTYPE);
  wsz = sizeof(WORLD);
  pos = 0;
  pnt = buff;
  q = NULL;
  while (pos < dataLen) {
    /* Get each Data item out */
    memcpy(&wx, pnt, wsz); /* X */
    pnt += wsz;
    pos = (BYTE) (pos + wsz);
    memcpy(&wy, pnt, wsz); /* Y */
    pnt += wsz;
    pos = (BYTE) (pos + wsz);
    memcpy(&tt, pnt, ttsz); /* Angle */
    pnt += ttsz;
    pos += (BYTE) (pos + ttsz);
    length = *pnt; /* Length */ 
    pnt++;
    pos++;
    expType = *pnt; /* Length */ 
    pnt++;
    pos++;
    creator = *pnt; /* Length */ 
    pnt++;
    pos++;
    if (creator != playerNum) {
      /* Add it */
      if (isServer == FALSE) {
        BYTE lgmMX;
        BYTE lgmMY;
        BYTE lgmPX;
        BYTE lgmPY;
        BYTE lgmFrame;
        playersGetLgmDetails(screenGetPlayers(), creator, &lgmMX, &lgmMY, &lgmPX, &lgmPY, &lgmFrame);
        playersUpdate(screenGetPlayers(), creator, 0, 0, 0, 0, 0, 0, lgmMX, lgmMY, lgmPX, lgmPY, lgmFrame);
      }


      New(q);
      q->packSent = isSent;
      q->own = isServer;
      q->x= wx;
      q->y= wy;
      q->angle = tt;
      q->length = length; 
      q->explodeType = expType;
      q->creator = creator;
      /* Add it to the structure */
      q->next = *tke;
      q->prev = NULL;
      if (NonEmpty(*tke)) {
        (*tke)->prev = q;
      }
      *tke = q;
    }
  } 
}

