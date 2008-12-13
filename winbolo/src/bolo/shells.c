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
*Name:          Shells
*Filename:      shells.c
*Author:        John Morrison
*Creation Date: 25/12/98
*Last Modified: 04/04/02
*Purpose:
*  Responsable for Shells tracking/collision detect etc.
*********************************************************/

#include <memory.h>
#include "global.h"
#include "util.h"
#include "bolo_map.h"
#include "pillbox.h"
#include "tank.h"
#include "screenbullet.h"
#include "screen.h"
#include "rubble.h"
#include "building.h"
#include "explosions.h"
#include "sounddist.h"
#include "grass.h"
#include "swamp.h"
#include "tankexp.h"
#include "lgm.h"
#include "floodfill.h"
#include "frontend.h"
#include "minesexp.h"
#include "util.h"
#include "sounddist.h"
#include "players.h"
#include "netpnb.h"
#include "netmt.h"
#include "network.h"
#include "shells.h"
#include "screen.h"

bool c;
shellsNetHit snh;

#undef SHELL_START_ADD
#define SHELL_START_ADD 5


/*********************************************************
*NAME:          shellsCreate
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 25/12/98
*PURPOSE:
*  Sets up the shells data structure
*
*ARGUMENTS:
*
*********************************************************/
shells shellsCreate(void) {
	return NULL;
}


/*********************************************************
*NAME:          shellsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 25/12/98
*PURPOSE:
*  Destroys and frees memory for the shells data structure
*
*ARGUMENTS:
*  value - Pointer to the shells data structure
*********************************************************/
void shellsDestroy(shells *value) {
  shells q;

  while (!IsEmpty(*value)) {
    q = *value;
    *value = ShellsTail(q);
    Dispose(q);
  }
  c = TRUE;
}

/*********************************************************
*NAME:          shellsAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 6/3/99
*PURPOSE:
*  Adds an item to the shells data structure. Function
*  also calls the sound playing function
*
*ARGUMENTS:
*  value  - Pointer to the shells data structure
*  x      - X co-ord of the start position
*  y      - Y co-ord of the start position
*  angle  - angle of the shot
*  len    - Length in map units of the item
*  owner  - Who fired the shell
*  onBoat - Was the shell launched from a boat
*********************************************************/
void shellsAddItem(shells *value, WORLD x, WORLD y, TURNTYPE angle, TURNTYPE len, BYTE owner, bool onBoat) {
  shells q;
  int xAdd;
  int yAdd;

  utilCalcDistance(&xAdd, &yAdd, angle, SHELL_SPEED);
  x = (WORLD) (x + (SHELL_START_ADD) * xAdd);
  y = (WORLD) (y + (SHELL_START_ADD) * yAdd);
/*  if (xAdd >= 0) {
    x += 22;
  } else {
    x -= 22;
  }
  if (yAdd >= 0) {
    y += 22;
  } else {
    y -= 22;
  }  */
  New (q);
  q->x = x;
  q->y = y;
  q->angle = angle;
  q->length = (BYTE) (1 + (SHELL_LIFE * len) - (SHELL_START_ADD));
  q->onBoat = onBoat;
  q->creator = playersGetSelf(screenGetPlayers());
  q->owner = owner;
  q->packSent = FALSE;
  q->shellDead = FALSE;
  q->next = *value;
  q->prev = NULL;
  if (NonEmpty(*value)) {
    (*value)->prev = q;
  }
  *value = q;
}

/*********************************************************
*NAME:          shellsUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 04/04/02
*PURPOSE:
*  Updates each shells position and checks for colisions
*
*ARGUMENTS:
*  value    - Pointer to the shells data structure
*  mp       - Pointer to the map Structure
*  pb       - Pointer to the pillbox Structure
*  tk       - Pointer to an array of tank structures
*  numTanks - Number of tanks in the array
*  isServer - TRUE if we are a server
*********************************************************/
void shellsUpdate(shells *value, map *mp, pillboxes *pb, bases *bs, tank *tk, BYTE numTanks, bool isServer) {
  WORLD newX;      /* X and Y positions of the new shell location */
  WORLD newY;
  shells position; /* The position in the stack of items */
  bool needUpdate; /* Does an update need to occur? */
  int xAdd;        /* Amounts to add */
  int yAdd;  
  BYTE bmx;        /* Shell X and Y map positions */
  BYTE bmy;
  BYTE sx;         /* Screen - TANK_SUBTRACT Map X and Y Positions */
  BYTE sy;
  BYTE spx;        /* Screen - TANK_SUBTRACT  Pixel X and Y Positions */
  BYTE spy;
  WORLD conv;      /* Used for bit shifting */
  BYTE testPos;    /* Test position for mine send */
  BYTE count;      /* Looping variable */
  
  position = *value;
  if (netGetType() == netSingle) {
    isServer = FALSE;
  } 

  while (NonEmpty(position)) {
    needUpdate = TRUE;
    if (position->shellDead == TRUE && (position->packSent == TRUE || netGetType() == netSingle)) {
      needUpdate = FALSE;
      shellsDeleteItem(value, &position);
    } else if (position->shellDead == TRUE && position->packSent == FALSE) {
      needUpdate = TRUE;
    } else if (position->length > SHELL_DEATH) {
      /* Move the shell */
      utilCalcDistance(&xAdd, &yAdd, position->angle, SHELL_SPEED);
      newX = (WORLD) (position->x + xAdd);
      newY = (WORLD) (position->y + yAdd);
      /* Check for colision */
      if ((shellsCalcCollision(mp, pb, tk, bs, &newX, &newY, position->angle, position->owner, position->onBoat, numTanks, isServer)) == TRUE) {
        /* Get X and Y map co-ords. */
        conv = newX;
        conv >>= TANK_SHIFT_MAPSIZE;
        bmx = (BYTE) conv;
        conv = newY;
        conv >>= TANK_SHIFT_MAPSIZE;
        bmy = (BYTE) conv;
        /* Get Screen - TANK_SUBTRACT co-ords */
        conv = newX - TANK_SUBTRACT;
        conv >>= TANK_SHIFT_MAPSIZE;
        sx = (BYTE) conv;
        conv = newY - TANK_SUBTRACT;
        conv >>= TANK_SHIFT_MAPSIZE;
        sy = (BYTE) conv;
        conv = newX - TANK_SUBTRACT;
        conv <<= TANK_SHIFT_MAPSIZE;
        conv >>= TANK_SHIFT_PIXELSIZE;
        spx = (BYTE) conv;
        conv = newY - TANK_SUBTRACT;
        conv <<= TANK_SHIFT_MAPSIZE;
        conv >>= TANK_SHIFT_PIXELSIZE;
        spy = (BYTE) conv;
        explosionsAddItem(screenGetExplosions(), sx,sy,spx,spy,EXPLOSION_START);
        minesExpAddItem(screenGetMinesExp(), mp, bmx, bmy);
        count = 0;
        while (count < numTanks) {
          lgmDeathCheck(screenGetLgmFromPlayerNum(screenGetTankPlayer(&tk[count])), mp, pb, bs, newX, newY, position->owner);
          count++;
        }        
        if (position->packSent == TRUE) { 
          needUpdate = FALSE;
          shellsDeleteItem(value, &position);
        } else {
          position->shellDead = TRUE;
        }
      } else {
        position->length--;
        position->x = newX;
        position->y = newY;
      }
      /* Update Position */
    } else {
      /* Add to explosion Data structure and remove from shells data structure */
      needUpdate = FALSE;
      /* Get X and Y map co-ords. */
      conv = position->x;
      conv >>= TANK_SHIFT_MAPSIZE;
      bmx = (BYTE) conv;
      conv = position->y;
      conv >>= TANK_SHIFT_MAPSIZE;
      bmy = (BYTE) conv;
      /* Get Screen - TANK_SUBTRACT co-ords */
      conv = position->x - TANK_SUBTRACT;
      conv >>= TANK_SHIFT_MAPSIZE;
      sx = (BYTE) conv;
      conv = position->y - TANK_SUBTRACT;
      conv >>= TANK_SHIFT_MAPSIZE;
      sy = (BYTE) conv;
      conv = position->x - TANK_SUBTRACT;
      conv <<= TANK_SHIFT_MAPSIZE;
      conv >>= TANK_SHIFT_PIXELSIZE;
      spx = (BYTE) conv;
      conv = position->y - TANK_SUBTRACT;
      conv <<= TANK_SHIFT_MAPSIZE;
      conv >>= TANK_SHIFT_PIXELSIZE;
      spy = (BYTE) conv;
      explosionsAddItem(screenGetExplosions(), sx,sy,spx,spy,EXPLOSION_START);
      minesExpAddItem(screenGetMinesExp(), mp, bmx, bmy);
      testPos = mapGetPos(mp, bmx, bmy);
      if (isServer == TRUE && testPos >= MINE_START && testPos <= MINE_END) {
        netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, screenGetTankPlayer(tk), bmx, bmy);
      }
      count = 0;
      while (count < numTanks) {
        lgmDeathCheck(screenGetLgmFromPlayerNum(screenGetTankPlayer(&tk[count])), mp, pb, bs, position->x, position->y, position->owner);
        count++;
      }
      if (position->packSent == TRUE) {
        shellsDeleteItem(value, &position);
      } else if (position->shellDead == FALSE) {
        position->shellDead = TRUE;
      }
      
    }

    /* Get the next Item */
    if (*value != NULL && needUpdate == TRUE) {
      position = ShellsTail(position);
    }
  }
}

/*********************************************************
*NAME:          shellsDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 25/12/98
*PURPOSE:
*  Deletes the value from the master list
*
*ARGUMENTS:
*  master  - The master list of all shells
*  value   - Pointer to the shells to delete. Also puts
*            next shell its position
*********************************************************/
void shellsDeleteItem(shells *master, shells *value) {
  shells del;  /* The item to delete */

  del = *value;
  (*value) = ShellsTail(del);
  if (del->prev != NULL) {
    del->prev->next = del->next;
  } else {
    /* Must be the first item - Move the master position along one */
    (*master) = ShellsTail(*master);
    if (NonEmpty(*master)) {
      (*master)->prev = NULL;
    }
  }

  if (del->next != NULL) {
    del->next->prev = del->prev;
  }
  Dispose(del);
  c= TRUE;
}

/*********************************************************
*NAME:          shellsCalcScreenBullets
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Adds items to the sceenBullets data structure if they
*  are on screen
*
*ARGUMENTS:
*  value   - Pointer to the shells data structure
*  sBullet  - The screenBullets Data structure
*  leftPos  - X Map offset start
*  rightPos - X Map offset end
*  top      - Y Map offset end
*  bottom   - Y Map offset end
*********************************************************/
void shellsCalcScreenBullets(shells *value, screenBullets *sBullets, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  shells q;   /* Temp Pointer */
  WORLD conv; /* Used for Bit shifting */
  BYTE x;     /* Map X and Y Positions (relative to screen) */
  BYTE y;
  BYTE px;    /* Pixel X and Y Positions */
  BYTE py;
  BYTE frame; /* Animation Frame to draw */


  q = *value;
  c = FALSE;
  while (NonEmpty(q) && c == FALSE) {
    conv = q->x;
    conv >>= TANK_SHIFT_MAPSIZE;
    x = (BYTE) conv;
    conv = q->y;
    conv >>= TANK_SHIFT_MAPSIZE;
    y = (BYTE) conv;
    if (x >= leftPos && x < rightPos && y >= top && y < bottom) {
      frame = utilGetDir(q->angle);
      x -= (BYTE) leftPos;
      y -= (BYTE) top;
      conv = q->x;
      conv <<= TANK_SHIFT_MAPSIZE;
      conv >>= TANK_SHIFT_PIXELSIZE;
      px = (BYTE) conv;
      conv = q->y;
      conv <<= TANK_SHIFT_MAPSIZE;
      conv >>= TANK_SHIFT_PIXELSIZE;
      py = (BYTE) conv;
      screenBulletsAddItem(sBullets, x, y, px, py, (BYTE) (frame + SHELL_START_EXPLODE+1)); 
    }
    q = ShellsTail(q);
  }
}

/*********************************************************
*NAME:          shellsCalcCollision
*AUTHOR:        John Morrison
*CREATION DATE: 29/12/98
*LAST MODIFIED: 29/07/00
*PURPOSE:
*  Returns whether the collision has occured.
*  
*ARGUMENTS:
*  map      - Pointer to the Map Structure
*  pb       - Pointer to the pillboxes structure
*  tk       - Pointer to an array of tank structures
*  bs       - Pointer to the bases structure
*  xValue   - X position
*  yValue   - Y position
*  angle    - The angle the shell is travelling
*  owner    - Who fired the shell
*  onBoat   - Was the shell launched from a boat
*  numTanks - Number of tanks in the array
*  isServer - TRUE if we are a server
*********************************************************/
bool shellsCalcCollision(map *mp, pillboxes *pb, tank *tk, bases *bs, WORLD *xValue, WORLD *yValue, TURNTYPE angle, BYTE owner, bool onBoat, BYTE numTanks, bool isServer) {
  bool returnValue; /* Value to return */
  tankHit th;       /* Used to store whether the tank has been hit */
  WORLD conv;       /* Used in the conversion */
  BYTE mapX;        /* Map Co-ordinates of where the item is going to hit */
  BYTE mapY;
  BYTE terrain;     /* The Terrain Type hit */
  BYTE newTerrain;  /* The new terrain type to replace it with */
  BYTE playerHit;   /* The player number of the player that got hit */
  bool isMine;      /* Is the terrain a mine */
  BYTE count;       /* Looping variable */
  bool baseExist;   /* Does a base exist here */
  netType gameT;    /* Type of network game */


  returnValue =  FALSE;
  isMine = FALSE;
  gameT = netGetType();
  baseExist = FALSE;
  /* Convert the position to Map co-ords */
  conv = *xValue;
  conv >>= TANK_SHIFT_MAPSIZE;
  mapX = (BYTE) conv; 
  conv = *yValue;
  conv >>= TANK_SHIFT_MAPSIZE;
  mapY = (BYTE) conv;

  if ((pillsIsPillHit(pb, mapX, mapY)) == TRUE) {
    returnValue = TRUE;
    *xValue = mapX;
    *xValue <<= TANK_SHIFT_MAPSIZE;
    *xValue += MAP_SQUARE_MIDDLE;
    *yValue = mapY;
    *yValue <<= TANK_SHIFT_MAPSIZE;
    *yValue += MAP_SQUARE_MIDDLE;
    /* Play sound here */
    if (isServer == TRUE || gameT == netSingle) {
      if (pillsDamagePos(pb, mapX, mapY, TRUE, TRUE) == TRUE) {
        count = pillsGetPillNum(pb, mapX, mapY, FALSE, FALSE);
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_DEAD, (BYTE) (count-1), pillsGetPillOwner(pb, count), mapX, mapY);
      } else {
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_HIT, 0, owner, mapX, mapY);
      }
    } else if (owner == screenGetTankPlayer(tk)) {
      pillsDamagePos(pb, mapX, mapY, FALSE, TRUE);
    }
    soundDist(shotBuildingNear, mapX, mapY);
  }
  if (returnValue == FALSE) {
    count = 0;
    while (count < numTanks && returnValue == FALSE) {
      if (screenGetTankPlayer(&tk[count]) != owner) {
        th = tankIsTankHit(&(tk[count]), mp, pb, bs, *xValue, *yValue, angle, owner);
        switch (th) {
        case TH_HIT:
          returnValue = TRUE;
          if (threadsGetContext() == FALSE) {
            frontEndPlaySound(hitTankSelf);
          }
          if (isServer == FALSE) {
            if ((mapGetPos(mp, tankGetMX(&(tk[count])), tankGetMY(&(tk[count]))) == DEEP_SEA)) {
            /* We have drowned - They have killed us */
//              if (owner != NEUTRAL) {
                netMNTAdd(screenGetNetMnt(), NMNT_KILLME, playersGetSelf(screenGetPlayers()), screenGetTankPlayer(&(tk[count])), owner, 0xFF);
  //            } else {
                /* Was player get self for first owner parameter all the time */
    //            netMNTAdd(NMNT_KILLME, owner, screenGetTankPlayer(&(tk[count])), 0xFF, 0xFF);
//              }

            }
          }
          break;
        case TH_KILL_SMALL:
          returnValue = TRUE;
          if (isServer == FALSE) {
            tkExplosionAddItem(screenGetTankExplosions(), *xValue, *yValue, angle, TK_EXPLODE_LENGTH, TK_SMALL_EXPLOSION);
  //            if (owner != NEUTRAL) {
                netMNTAdd(screenGetNetMnt(), NMNT_KILLME, playersGetSelf(screenGetPlayers()), screenGetTankPlayer(&(tk[count])), owner, 0xFF);
//              } else {
 //               netMNTAdd(NMNT_KILLME, owner, screenGetTankPlayer(&(tk[count])), 0xFF, 0xFF);
//              }

          }
          if (threadsGetContext() == FALSE) {
            frontEndPlaySound(hitTankSelf);
          }
          break;
        case TH_KILL_BIG:
          returnValue = TRUE;
          if (isServer == FALSE) {
            tkExplosionAddItem(screenGetTankExplosions(), *xValue, *yValue, angle, TK_EXPLODE_LENGTH, TK_LARGE_EXPLOSION);
//              if (owner != NEUTRAL) {
  //              netMNTAdd(NMNT_KILLME, owner, screenGetTankPlayer(&(tk[count])), playersGetSelf(), 0);
    //          } else {
                netMNTAdd(screenGetNetMnt(), NMNT_KILLME, playersGetSelf(screenGetPlayers()), screenGetTankPlayer(&(tk[count])), owner, 0xFF);
     //         }

          }
          if (threadsGetContext() == FALSE) {
            frontEndPlaySound(hitTankSelf);
          }
          break;
         case TH_MISSED:
         default:
           break;
        }
      }
      count++;
    }
  }
  /* Check for player hit */
  if (returnValue == FALSE && isServer == FALSE) {
    playerHit = playersIsTankHit(screenGetPlayers(), *xValue, *yValue, angle, owner);
    if (playerHit != NEUTRAL) {
      returnValue = TRUE;
      soundDist(hitTankNear, mapX, mapY);
    }
  }
  
  if (returnValue == FALSE) {
    baseExist = basesExistPos(bs, mapX, mapY); 
    /* Check for base */
    if (baseExist == TRUE) {
      if (onBoat == TRUE) {
        returnValue = TRUE;
        *xValue = mapX;
        *xValue <<= TANK_SHIFT_MAPSIZE;
        *xValue += MAP_SQUARE_MIDDLE;
        *yValue = mapY;
        *yValue <<= TANK_SHIFT_MAPSIZE;
        *yValue += MAP_SQUARE_MIDDLE;
        /* Play sound */
        if ((basesCanHit(bs, mapX, mapY, owner)) == TRUE) {
          if (isServer == TRUE || gameT == netSingle) {
            basesDamagePos(bs, mapX, mapY);
            netPNBAdd(screenGetNetPnb(), NPNB_BASE_HIT, 0, screenGetTankPlayer(tk), mapX, mapY);
          }
          pillsBaseHit(pb, mapX, mapY, (basesGetOwnerPos(bs, mapX, mapY)));
        }
        soundDist(shotBuildingNear, mapX, mapY);
      } else if ((basesCanHit(bs, mapX, mapY, owner)) == TRUE) { /* Huh? */
        returnValue = TRUE;
        *xValue = mapX;
        *xValue <<= TANK_SHIFT_MAPSIZE;
        *xValue += MAP_SQUARE_MIDDLE;
        *yValue = mapY;
        *yValue <<= TANK_SHIFT_MAPSIZE;
        *yValue += MAP_SQUARE_MIDDLE;
        /* Do damage to base */
        if (isServer == TRUE || gameT == netSingle) {
          basesDamagePos(bs, mapX, mapY);
          netPNBAdd(screenGetNetPnb(), NPNB_BASE_HIT, 0, screenGetTankPlayer(tk), mapX, mapY);
        }
        pillsBaseHit(pb, mapX, mapY, (basesGetOwnerPos(bs, mapX, mapY)));
        /* Play sound */
        soundDist(shotBuildingNear, mapX, mapY);
      } /*end huh?*/
    }
  }
  if (returnValue == FALSE) {
    if ((mapIsPassable(mp, mapX, mapY, onBoat)) == FALSE && baseExist == FALSE) {
      returnValue = TRUE;
      *xValue = mapX;
      *xValue <<= TANK_SHIFT_MAPSIZE;
      *xValue += MAP_SQUARE_MIDDLE;
      conv = *xValue;
      conv >>= TANK_SHIFT_MAPSIZE;
      mapX = (BYTE) conv;      
      *yValue = mapY;
      *yValue <<= TANK_SHIFT_MAPSIZE;
      *yValue += MAP_SQUARE_MIDDLE;
      conv = *yValue;
      conv >>= TANK_SHIFT_MAPSIZE;
      mapY = (BYTE) conv;
    
      terrain = mapGetPos(mp, mapX, mapY);
      if (terrain >= MINE_START && terrain <= MINE_END) {
        terrain -= MINE_SUBTRACT;
        isMine = TRUE;
      }
      /* Update the map  & Play the sound */
      switch (terrain) {
      case BUILDING:
        mapSetPos(mp, mapX, mapY, (buildingAddItem(screenGetBuildings(), mapX, mapY)), FALSE, FALSE);
        soundDist(shotBuildingNear, mapX, mapY);
        break;
      case FOREST:
        if (isMine == TRUE) {
          mapSetPos(mp, mapX, mapY, GRASS+MINE_SUBTRACT, FALSE, FALSE);
          if (isServer == TRUE) {
            netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, screenGetTankPlayer(tk), mapX, mapY);
          }
        } else {
          mapSetPos(mp, mapX, mapY, GRASS, FALSE, FALSE);
        }
        soundDist(shotTreeNear, mapX, mapY);
        break;
      case HALFBUILDING:
        mapSetPos(mp, mapX, mapY, (buildingAddItem(screenGetBuildings(), mapX, mapY)), FALSE, FALSE);
        soundDist(shotBuildingNear, mapX, mapY);
        break;
      case BOAT:
        mapSetPos(mp, mapX, mapY, RIVER, FALSE, FALSE);
        soundDist(shotBuildingNear, mapX, mapY);
        break;
      case GRASS:
        if (isMine == TRUE) {
          mapSetPos(mp, mapX, mapY, GRASS+MINE_SUBTRACT, FALSE, FALSE);
          if (isServer == TRUE) {
            netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, screenGetTankPlayer(tk), mapX, mapY);
          }
        } else {
          newTerrain = grassAddItem(screenGetGrass(), mapX, mapY);
          mapSetPos(mp, mapX, mapY, newTerrain, FALSE, FALSE);
          if (newTerrain == RIVER) {
            floodAddItem(screenGetFloodFill(), mapX, mapY);
          }
        }
        break;
      case SWAMP:
        if (isMine == TRUE) {
          mapSetPos(mp, mapX, mapY, SWAMP+MINE_SUBTRACT, FALSE, FALSE);
          if (isServer == TRUE) {
            netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, screenGetTankPlayer(tk), mapX, mapY);
          }
        } else {
          newTerrain = swampAddItem(screenGetSwamp(), mapX, mapY);
          mapSetPos(mp, mapX, mapY, newTerrain, FALSE, FALSE);
          if (newTerrain == RIVER) {
            floodAddItem(screenGetFloodFill(), mapX, mapY);
          }
        }
        break;
      case RUBBLE:
        newTerrain = rubbleAddItem(screenGetRubble(), mapX, mapY);
        mapSetPos(mp, mapX, mapY, newTerrain, FALSE, FALSE);
        if (newTerrain == RIVER) {
          floodAddItem(screenGetFloodFill(), mapX, mapY);
        }
        break;
      case ROAD:
        if (isMine == TRUE) {
          mapSetPos(mp, mapX, mapY, ROAD+MINE_SUBTRACT, FALSE, FALSE);
          if (isServer == TRUE) {
            netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, screenGetTankPlayer(tk), mapX, mapY);
          }
        } else {
          mapSetPos(mp, mapX, mapY, (shellsCheckRoad(mp, pb, bs, mapX, mapY, angle)), FALSE, FALSE);         
        }
      }
    }
  }
  if (returnValue == TRUE) {
    screenReCalc();
  }
  return returnValue;
}


/*********************************************************
*NAME:          shellsCheckRoad
*AUTHOR:        John Morrison
*CREATION DATE: 6/1/99
*LAST MODIFIED: 6/1/99
*PURPOSE:
*  A shell has been fired from a boat and has hit a road.
*  This function returns whether the road should be kept
*  as road or destroyed and replaced as river
*  
*ARGUMENTS:
*  mp    - Pointer to the Map Data structure
*  pb    - Pointer to the pillbox structure
*  bs    - Pointer to the bases structure
*  mapX  - X position of the hit
*  mapY  - Y position of the hit
*  angle - The angle the shell is travelling
*********************************************************/
BYTE shellsCheckRoad(map *mp, pillboxes *pb, bases *bs, BYTE mapX, BYTE mapY, TURNTYPE dir) {
  BYTE returnValue; /* Value to return */
  BYTE dir16;       /* Angle converted to 16 direction */
  
  returnValue = ROAD;
  dir16 = utilGet16Dir(dir);

  if (dir16 < BRADIANS_NEAST || dir16 >= BRADIANS_NWEST) {
    if (mapIsLand(mp, pb, bs, mapX, (BYTE) (mapY-1)) == FALSE) {
      returnValue = RIVER;
    }
  } else if (dir16 >= BRADIANS_NEAST && dir16 < BRADIANS_SEAST) {
    if (mapIsLand(mp, pb, bs, (BYTE) (mapX+1), mapY) == FALSE) {
      returnValue = RIVER;
    }
  } else if (dir16 >= BRADIANS_SEAST && dir16 < BRADIANS_SWEST) {
    if (mapIsLand(mp, pb, bs, mapX, (BYTE) (mapY+1)) == FALSE) {
      returnValue = RIVER;
    }
  } else {
    if (mapIsLand(mp, pb, bs, (BYTE) (mapX-1), mapY) == FALSE) {
      returnValue = RIVER;
    }
  }
  
  /* Play the sound if required */
  if (returnValue == RIVER) {
    soundDist(shotBuildingNear, mapX, mapY);
  }

   return returnValue;
}

/*********************************************************
*NAME:          shellsNetMake
*AUTHOR:        John Morrison
*CREATION DATE: 6/3/99
*LAST MODIFIED: 8/9/00
*PURPOSE:
*  When we have the token we inform all the players of
*  shells we have fired since last time we had the token.
*  Returns the length of the data created
*  
*ARGUMENTS:
*  value       - Pointer to shells structure
*  buff        - Pointer to a buffer to hold the shells 
*                net data
*  noPlayerNum - If the shells ->creator equals this
*                do not send it
*  sentState   - What to set the send state to
*********************************************************/
BYTE shellsNetMake(shells *value, BYTE *buff, BYTE noPlayerNum, bool sentState) {
  BYTE returnValue;  /* Value to return */
  unsigned int ttsz; /* Size of turntype type */
  unsigned int wsz;  /* Size of world type */
  BYTE *pnt;         /* Pointer to offset in the buffer */
  shells q;          /* Temp pointer to the shells structure */

  ttsz = sizeof(TURNTYPE);
  wsz = sizeof(WORLD);
  returnValue = 0;
  pnt = buff;
  q = *value;

  while (NonEmpty(q)) {
    if (q->packSent == FALSE && q->creator != noPlayerNum) {
      /* Need to add */
      /* Check range from things */
      if (screenTankInView(noPlayerNum, (BYTE) (q->x >> TANK_SHIFT_MAPSIZE), (BYTE) (q->y >> TANK_SHIFT_MAPSIZE)) == TRUE) {
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
        *pnt = q->owner; /* Owner */
        pnt++;
        returnValue++;
        *pnt = q->onBoat; /* On Boat */
        pnt++;
        returnValue++;
        *pnt = q->creator; /* Creator */
        pnt++;
        returnValue++;
      }
      /* We have no sent it */
      q->packSent = sentState;
    }
    q = ShellsTail(q);
  }
  return returnValue;
}

/*********************************************************
*NAME:          shellsNetExtract
*AUTHOR:        John Morrison
*CREATION DATE:  6/3/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
* Network shells data have arrived. Add them to our 
* shells structure here.
*  
*ARGUMENTS:
*  value    - Pointer to shells structure
*  pb       - Pointer to the pillboxes structure
*  buff     - Pointer to a buffer to hold the shells 
*             net data
*  dataLen  - Length of the data
*  isServer - TRUE if we are the game server.
*********************************************************/
void shellsNetExtract(shells *value, pillboxes *pb, BYTE *buff, BYTE dataLen, bool isServer) {
  BYTE pos;   /* Position through the data we are */
  BYTE *pnt;  /* Pointer to offset in the buffer */
  shells q;   /* Temp pointer to hold additions to the shells structure */
  WORLD conv; /* Used in the conversion */
  BYTE mx;    /* Map X position */
  BYTE my;    /* Map Y position */
  BYTE self;  /* Player Number */
  WORLD wx;   /* An Item */
  WORLD wy;
  WORLD twx, twy;
  TURNTYPE tt;
  BYTE length;
  BYTE owner;
  bool onBoat;
  BYTE creator;
  bool shouldAdd;
  BYTE amount;
  tank *tnk;
  double dummy;
  int xAdd;
  int yAdd;

  self = playersGetSelf(screenGetPlayers());
  pos = 0;
  pnt = buff;
  q = NULL;

  while (pos < dataLen) {
    shouldAdd = FALSE;
    /* Get each Data item out */
    memcpy(&wx, pnt, sizeof(WORLD)); /* X */
    pnt += sizeof(WORLD);
    pos += sizeof(WORLD);
    memcpy(&wy, pnt, sizeof(WORLD)); /* Y */
    pnt += sizeof(WORLD);
    pos += sizeof(WORLD);
    memcpy(&tt, pnt, sizeof(TURNTYPE)); /* Angle */
    pnt += sizeof(TURNTYPE);
    pos += sizeof(TURNTYPE);
    length = *pnt; /* Length */ 
    pnt++;
    pos++;
    owner = *pnt; /* Owner */ 
    pnt++;
    pos++;
    onBoat = *pnt; /* On boat */
    pnt++;
    pos++;
    creator = *pnt;
    pnt++;
    pos++;

    /* Check to see if we should add to it */
    if (isServer == TRUE) {
      if (owner != NEUTRAL) {
        tnk = screenGetTankFromPlayer(creator);
        tankGetWorld(tnk, &twx, &twy);
        amount = tankGetShells(tnk);
        if (amount > 0) {
          if (utilIsItemInRange(twx, twy, wx, wy, 512, &dummy) == TRUE) {
            amount--;
            tankSetShells(tnk, amount);
            shouldAdd = TRUE;
          }
        }
      } else {
        /* Fired from a pill - Check locality */
        utilCalcDistance(&xAdd, &yAdd, tt, SHELL_SPEED);
        xAdd *=2;
        yAdd *=2;
        if (pillsExistPos(pb, (BYTE) ((WORLD) (wx-xAdd) >> M_W_SHIFT_SIZE), (BYTE) ((WORLD) (wy-yAdd) >> M_W_SHIFT_SIZE)) == TRUE && pillsDeadPos(pb, (BYTE) ((WORLD) (wx-xAdd) >> M_W_SHIFT_SIZE), (BYTE) ((WORLD) (wy-yAdd) >> M_W_SHIFT_SIZE)) == FALSE) {
          shouldAdd = TRUE;
        }
      }
    } else if (creator != self) {
      shouldAdd = TRUE;
    }

    if (length > 68 && owner == NEUTRAL) { /* Added length check to stop cheating */
      shouldAdd = FALSE;
    } else if (length > 52 && owner != NEUTRAL) {
		/* FIXME: Check shells fired from a tank are near the tank that fired them, they have sufficent shells etc */
      /* Tank max length */
      shouldAdd = FALSE;
    }


    /* Add it if required */
    if (shouldAdd == TRUE) {
      New(q);
      if (isServer == TRUE) {
        q->packSent = FALSE;
      } else {
        q->packSent = TRUE;
      }
      q->x = wx;
      q->y = wy;
      q->angle = tt;
      q->shellDead = FALSE;
      q->length = length;
      q->owner = owner;
      q->onBoat = onBoat;
      q->creator = creator;
      /* Add it to the structure */
      q->next = *value;
      q->prev = NULL;
      if (NonEmpty(*value)) {
        (*value)->prev = q;
      }
      *value = q;
    }
  }
  
  /* Play the last sound event if exist */
  if (q != NULL) {
    conv = q->x;
    conv >>= TANK_SHIFT_MAPSIZE;
    mx = (BYTE) conv;
    conv = q->y;
    conv >>= TANK_SHIFT_MAPSIZE;
    my = (BYTE) conv;
    soundDist(shootNear, mx, my);
  }
}

/*********************************************************
*NAME:          shellsGetBrainShellsInRect
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 28/11/99
*PURPOSE:
*  Makes the brain shell info for each shell inside the
*  rectangle formed by the function parameters.
*
*ARGUMENTS:
*  value     - Pointer to the shells structure
*  leftPos   - Left position of rectangle
*  rightPos  - Right position of rectangle
*  topPos    - Top position of rectangle
*  bottomPos - Bottom position of rectangle
*********************************************************/
void shellsGetBrainShellsInRect(shells *value, BYTE leftPos, BYTE rightPos, BYTE topPos, BYTE bottomPos) {
  shells position; /* The position in the stack of items */
  BYTE owner;      /* Owner of the item */
  WORLD conv;      /* Used in converting items world co-ordinates */
  BYTE mx;         /* Shell X and Y Positions */
  BYTE my; 
  BYTE playerNum;  /* Our player number       */

  playerNum = playersGetSelf(screenGetPlayers());
  position = *value;

/* typedef struct
	{
	OBJECT object; = 3
	WORLD_X x;
	WORLD_Y y;
	WORD idnum;
	BYTE direction;
	BYTE info;
	} ObjectInfo;
*/

  while (NonEmpty(position)) {
    conv = position->x;
    conv >>= TANK_SHIFT_MAPSIZE;
    mx = (BYTE) conv;
    conv = position->y;
    conv >>= TANK_SHIFT_MAPSIZE;
    my = (BYTE) conv;
    if (mx >= leftPos && mx <= rightPos && my >= topPos && my <= bottomPos) {
      /* In the rectangle */
      if (position->owner == NEUTRAL) {
        owner = SHELLS_BRAIN_NEUTRAL;
      } else if (playersIsAllie(screenGetPlayers(), playerNum, position->owner) == TRUE) {
        owner = SHELLS_BRAIN_FRIENDLY;
      } else {
        owner = SHELLS_BRAIN_HOSTILE;
      }
      screenAddBrainObject(SHELLS_BRAIN_OBJECT_TYPE, position->x, position->y, 0, utilGet16Dir(position->angle), owner);
    }
    position = ShellsTail(position);
  }
}
