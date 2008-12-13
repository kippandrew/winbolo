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
*Filename:      explosion.c
*Author:        John Morrison
*Creation Date: 01/01/99
*Last Modified: 04/10/03
*Purpose:
*  Responsable for Explosions
*********************************************************/

#include "global.h"
#include "screenbullet.h"
#include "screen.h"
#include "rubble.h"
#include "frontend.h"
#include "explosions.h"

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
void explosionsCreate(explosions *expl) {
	*expl = NULL;
}

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
void explosionsDestroy(explosions *expl) {
  explosions q;

  while (!IsEmpty(*expl)) {
    q = *expl;
    *expl = ExplosionsTail(q);
    Dispose(q);
  }
}

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
void explosionsAddItem(explosions *expl, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE startPos) {
  explosions q;

  New (q);
  q->mx = mx;
  q->my = my;
  q->px = px;
  q->py = py;
  q->length = startPos;
  q->next = *expl;
  q->prev = NULL;
  if (NonEmpty(*expl)) {
    (*expl)->prev = q;
  }
  
  *expl = q;
}

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
void explosionsUpdate(explosions *expl) {
  static BYTE update =0;  /* The update time */
  explosions position; /* Position throught the items */
  bool needUpdate;     /* Whether an update is needed or not */

  update++;
  if (update != EXPLOAD_UPDATE_TIME) {
    return;
  } else {
    update = 0;
  }

  position = *expl;

  while (NonEmpty(position)) {
    needUpdate = TRUE;
    if (position->length > EXPLODE_DEATH) {
        position->length--;
    } else {
      /* Remove from data structure */
      needUpdate = FALSE;
      explosionDeleteItem(expl, &position);
    }
    
    /* Get the next Item */
    if (*expl != NULL && needUpdate == TRUE) {
      position = ExplosionsTail(position);
    }
  }
}

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
void explosionDeleteItem(explosions *expl, explosions *value) {
  explosions del;  /* The item to delete */

  del = *value;
  (*value) = ExplosionsTail(del);
  if (del->prev != NULL) {
    del->prev->next = del->next;
  } else {
    /* Must be the first item - Move the master position along one */
    *expl = ExplosionsTail(*expl);
    if (NonEmpty(*expl)) {
      (*expl)->prev = NULL;
    } 
  }

  if (del->next != NULL) {
    del->next->prev = del->prev;
  }
  Dispose(del);
}

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
void explosionsCalcScreenBullets(explosions *expl, screenBullets *sBullets, BYTE leftPos, BYTE rightPos, BYTE topPos, BYTE bottomPos) {
  explosions q;   /* Temp Pointer */

  q = *expl;
  while (NonEmpty(q)) {
    if (q->mx >= leftPos && q->mx < rightPos && q->my >= topPos && q->my < bottomPos) {
      screenBulletsAddItem(sBullets, (BYTE) (q->mx-leftPos), (BYTE) (q->my-topPos), q->px, q->py, q->length);
    }
    q = ExplosionsTail(q);
  }
}
