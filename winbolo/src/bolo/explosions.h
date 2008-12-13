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
*Name:          Explosion
*Filename:      explosion.h
*Author:        John Morrison
*Creation Date:  1/1/99
*Last Modified: 15/1/98
*Purpose:
*  Responsable for Explosions
*********************************************************/

#ifndef EXPLOSIONS_H
#define EXPLOSIONS_H

#include "global.h"

#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define ExplosionsTail(list) ((list)->next);


#define EXPLOSION_START 8
#define EXPLODE_DEATH 1
/* Explosions should only be updated every 3 ticks */
#define EXPLOAD_UPDATE_TIME 3

/* Type structure */

typedef struct explosionsObj *explosions;
struct explosionsObj {
  explosions next;
  explosions prev;
  BYTE mx;
  BYTE my;
  BYTE px;
  BYTE py;
  BYTE length;
};

/* Prototypes */

/*********************************************************
*NAME:          explosionsCreate
*AUTHOR:        John Morrison
*CREATION DATE:  1/1/99
*LAST MODIFIED:  1/1/99
*PURPOSE:
*  Sets up the explosions data structure
*
*ARGUMENTS:
*  expl - Pointer to the explosions object
*********************************************************/
void explosionsCreate(explosions *expl);

/*********************************************************
*NAME:          explosionsDestroy
*AUTHOR:        John Morrison
*CREATION DATE:  1/1/99
*LAST MODIFIED:  1/1/99
*PURPOSE:
*  Destroys and frees memory for the explosions data 
*  structure
*
*ARGUMENTS:
*  expl - Pointer to the explosions object
*********************************************************/
void explosionsDestroy(explosions *expl);

/*********************************************************
*NAME:          explosionsAddItem
*AUTHOR:        John Morrison
*CREATION DATE:  1/1/99
*LAST MODIFIED: 15/1/99
*PURPOSE:
*  Adds an item to the explosions data structure. 
*
*ARGUMENTS:
*  expl   - Pointer to the explosions object
*  value  - Pointer to the shells data structure
*  mx     - Map X co-ord of the explosion
*  my     - Map Y co-ord of the explosion
*  px     - Pixel X co-ord of the explosion
*  py     - Pixel Y co-ord of the explosion
*  startPos  - How far through the explosion does it start
*********************************************************/
void explosionsAddItem(explosions *expl, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE startPos);

/*********************************************************
*NAME:          explosionsUpdate
*AUTHOR:        John Morrison
*CREATION DATE:  1/1/99
*LAST MODIFIED:  1/1/99
*PURPOSE:
*  Updates each explosion position
*
*ARGUMENTS:
*  expl - Pointer to the explosions object
*********************************************************/
void explosionsUpdate(explosions *expl);

/*********************************************************
*NAME:          explosionDeleteItem
*AUTHOR:        John Morrison
*CREATION DATE: 1/1/99
*LAST MODIFIED: 1/1/99
*PURPOSE:
*  Deletes the item for the given number
*
*ARGUMENTS:
*  expl  - Pointer to the explosions object
*  value - Position to destroy (places next in it)
*********************************************************/
void explosionDeleteItem(explosions *expl, explosions *value);

/*********************************************************
*NAME:          explosionCalcScreenBullets
*AUTHOR:        John Morrison
*CREATION DATE: 1/1/98
*LAST MODIFIED: 1/1/98
*PURPOSE:
*  Adds items to the sceenBullets data structure if they
*  are on screen
*
*ARGUMENTS:
*  expl       - Pointer to the explosions object
*  sBullet    - The screenBullets Data structure
*  leftPos    - X Map offset start
*  rightPos   - X Map offset end
*  topPos     - Y Map offset end
*  bottomPos  - Y Map offset end
*********************************************************/
void explosionsCalcScreenBullets(explosions *expl, screenBullets *sBullets, BYTE leftPos, BYTE rightPos, BYTE topPos, BYTE bottomPos);

#endif /* EXPLOSIONS_H */

