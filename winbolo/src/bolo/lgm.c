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
*Name:          lgm
*Filename:      lgm.c
*Author:        John Morrison
*Creation Date: 17/01/99
*Last Modified: 01/02/03
*Purpose:
*  Operations on tanks LGM are handled by this module
*********************************************************/

#include <string.h>
#include "bases.h"
#include "bolo_map.h"
#include "building.h"
#include "debug_file_output.h"
#include "explosions.h"
#include "floodfill.h"
#include "frontend.h"
#include "global.h"
#include "grass.h"
#include "labels.h"
#include "lgm.h"
#include "log.h"
#include "messages.h"
#include "mines.h"
#include "minesexp.h"
#include "netmt.h"
#include "netpnb.h"
#include "network.h"
#include "players.h"
#include "pillbox.h"
#include "rubble.h"
#include "screen.h"
#include "sounddist.h"
#include "swamp.h"
#include "tank.h"
#include "types.h"
#include "util.h"
#include "../server/servernet.h"
#include "../winbolonet/winbolonet.h"



/* Prototypes */

/*********************************************************
*NAME:          lgmCreate 
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 3/12/00
*PURPOSE:
*  Sets up the LGM structure
*
*ARGUMENTS:
*
*********************************************************/
lgm lgmCreate(BYTE playerNum) {
  lgm lgman;

  New(lgman);
  lgman->playerNum = playerNum;
  lgman->x = 0;
  lgman->y = 0;
  lgman->inTank = TRUE;
  lgman->isDead = FALSE;
  lgman->nextAction = LGM_IDLE;
  lgman->action = LGM_IDLE;
  lgman->state = LGM_STATE_IDLE;
  lgman->frame = 0;
  lgman->numTrees = 0;
  lgman->numMines = 0;
  lgman->numPills = LGM_NO_PILL;
  lgman->waitTime = 0;
  lgman->blessX = 0;
  lgman->blessY = 0;
  lgman->onTop = 0;
  lgman->obstructed = 0;

  return lgman;
}

/*********************************************************
*NAME:          lgmDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
* Destroys the LGM structure
*
*ARGUMENTS:
*  value - Pointer to the lgm sturcture
*********************************************************/
void lgmDestroy(lgm *value) {
  if (*value != NULL) {
    Dispose(*value);
  }
}

/*********************************************************
*NAME:          lgmUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Game tick has passed. Update the lgm position
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the base structure
*  tnk    - Pointer to the tank structure
*********************************************************/
void lgmUpdate(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk) {
	BYTE pos = 0;
	BYTE mx = 0;
	BYTE my= 0;
	WORLD wx;
	WORLD wy;
	WORLD t;

	/* Single player or server instance */
	if (netGetType() == netSingle || threadsGetContext() == TRUE) {

		/* Man is parachuting in */
		if ((*lgman)->isDead == TRUE) {
			lgmParchutingIn(lgman);
			/* check to see if lgm will land ontop of a building/pillbox/or base */
			mx = lgmGetMX(lgman);
			my = lgmGetMY(lgman);
			pos = mapGetPos(mp, mx, my);
			if (pos == BUILDING || pos == HALFBUILDING || pillsExistPos(pb, mx, my) == TRUE || basesExistPos(bs, mx, my) == TRUE) {
				(*lgman)->onTop = TRUE;
				// if base is owned by player, then, we're not ontop.
				if(basesAmOwner(bs,(*lgman)->playerNum, mx, my)==TRUE){
					(*lgman)->onTop = FALSE;
				}
			} else {
				(*lgman)->onTop = FALSE;
			}
		} else if ((*lgman)->state != LGM_STATE_IDLE) {
			/* LGM is either going to a destination or returning to the tank */
			/* Update frame */
			(*lgman)->frame++;
			if ((*lgman)->frame > LGM_MAX_FRAMES) {
				(*lgman)->frame = 0;
			}

			/* Check his not waiting */
			(*lgman)->obstructed = LGM_BRAIN_FREE;


			/* The deathWait indicator is not reaching us properly here
			 * the tank's x and y will sometimes be around 65528 or 65533
			 * while it's tankDeath will be zero, which isn't right.
			 */
			
			sprintf(streamText,"Tank Coords: (%u,%u) ... ",(*tnk)->x,(*tnk)->y);
			writeDebugFile(streamText);
			streamText[0] = '\0';
			sprintf(streamText,"LGM State: %d ... ", (*lgman)->state);
			writeDebugFile(streamText);
			streamText[0] = '\0';
			sprintf(streamText,"DeathWait: %d ... \n", (*tnk)->deathWait);
			writeDebugFile(streamText);
			streamText[0] = '\0';


			if ((*lgman)->waitTime > 0) {
				/* LGM is currently building something (at a destination) */
				(*lgman)->waitTime--;
			} else if ((*lgman)->state == LGM_STATE_GOING) {
				/* LGM is going to a destination */
				lgmMoveAway(lgman, mp,pb,bs,tnk);
			} else if (((*lgman)->state == LGM_STATE_RETURN) && ((*tnk)->deathWait == 0)) {
				/* LGM is returning to a non-dead tank */
				lgmReturn(lgman, mp,pb,bs,tnk);
			} else if (((*lgman)->state == LGM_STATE_RETURN) && ((*tnk)->deathWait > 0)) {
				/* LGM should sit and wait until tank is alive again */
				(*lgman)->waitTime = 1;
			}
		} 
	} else {
		/* We do the following if we are a client only */
		if ((*lgman)->state != LGM_STATE_IDLE) {
			/* Man is out and about doing stuff */
			/* Update frame */
			(*lgman)->frame++;
			if ((*lgman)->frame > LGM_MAX_FRAMES) {
				(*lgman)->frame = 0;
			}
		}
		/* LGM isn't dead and he's not in the tank */
		if ((*lgman)->isDead == FALSE && (*lgman)->inTank == FALSE) {
			tankGetWorld(tnk, &wx, &wy);
			/* Multiplayer game but just a client */
			if (threadsGetContext() == FALSE) {
				frontEndManStatus(FALSE, utilCalcAngle((*lgman)->x, (*lgman)->y, wx, wy));
			}
		}
	}
}

bool lgmCheckNewRequest(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE mapX, BYTE mapY, BYTE *action, BYTE *pillNum, bool *isMine, BYTE *trees, BYTE *mines, bool perform);

/*********************************************************
*NAME:          lgmAddRequest
*AUTHOR:        John Morrison
*CREATION DATE: 17/01/99
*LAST MODIFIED: 01/02/03
*PURPOSE:
*  Adds a new request to the lgm structure
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  tnk    - Pointer to the tank structure
*  bs     - Pointer to the base structure
*  mapX   - X Co-ordinate of the new action
*  mapY   - Y Co-ordinate of the new action
*  action - What the new action is
*********************************************************/
void lgmAddRequest(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE mapX, BYTE mapY, BYTE action) {

  if ((*lgman)->isDead == TRUE && tankGetArmour(tnk) <= TANK_FULL_ARMOUR) {
	/* LGM is parachuting in and tank is alive */
    messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_MAN_DEAD));
  } else if ((*lgman)->action != LGM_IDLE) {
    /* Busy doing something else  Place in second request */
    (*lgman)->nextX = mapX;
    (*lgman)->nextY = mapY;
    (*lgman)->nextAction = action;
  } else if (netGetType() == netSingle || threadsGetContext() == TRUE) {
    /* Man isn't doing something  Check to make sure is possible first */
    lgmNewPrimaryRequest(lgman, mp, pb, bs, tnk, mapX, mapY, action);
  } else {
    /* Network game */
    BYTE pillNum;
    bool isMine;
    BYTE trees;
    BYTE minesAmount;
    bool ok;
    ok = lgmCheckNewRequest(lgman, mp, pb, bs, tnk, mapX, mapY, &action, &pillNum, &isMine, &trees, &minesAmount, FALSE);
    (*lgman)->numTrees = trees;
    (*lgman)->numMines = minesAmount;
    (*lgman)->numPills = pillNum;

    if (ok) {
      netMNTAdd(screenGetNetMnt(), NMNT_MANACTION, action, (*lgman)->playerNum, mapX, mapY);
      (*lgman)->action = action;
    } else {
      lgmBackInTank(lgman, mp, pb, bs, tnk, TRUE);
    }
  }
}  


/*********************************************************
*NAME:          lgmTankDied
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  The tank has died. Cancel all pending orders if any
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the base structure
*  tnk    - Pointer to the tank structure
*********************************************************/
void lgmTankDied(lgm *lgman) {
  (*lgman)->nextAction = LGM_IDLE;
}


/*********************************************************
*NAME:          lgmCheckNewRequest
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  ???
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the base structure
*  tnk    - Pointer to the tank structure
*  mapX   -
*  mapY   -
*  action - Pointer to
*  pillNum - Pointer to
*  isMine  - Pointer to
*  trees   - Pointer to
*  minesAmount - Pointer to
*  perform     -
*********************************************************/
bool lgmCheckNewRequest(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE mapX, BYTE mapY, BYTE *action, BYTE *pillNum, bool *isMine, BYTE *trees, BYTE *minesAmount, bool perform) {
  bool proceed;  /* Is it OK to proceed with the action */
  bool isBase;   /* Is the present co-ordinate a base */
  bool isPill;   /* Is the present co-ordinate a pill */
  BYTE tankX;    /* Tank co-ordinates */
  BYTE tankY;
  BYTE tankTrees;
  BYTE pos;      /* Map terrain at build request place */
  BYTE pillArmour=0;
  
  tankX = tankGetMX(tnk);
  tankY = tankGetMY(tnk);
  tankTrees = (*tnk)->trees;

  proceed = TRUE;
  *isMine = FALSE;
  *trees = 0;
  *minesAmount = 0;
  *pillNum = LGM_NO_PILL;

  /* Get the terrain of the map square that the user clicked on */
  pos = mapGetPos(mp,mapX,mapY);
  if (pos == MINE_FOREST) {
    pos = FOREST;
  }
  isPill = pillsExistPos(pb, mapX, mapY);
  isBase = basesExistPos(bs, mapX, mapY);

  switch (*action) {
  case LGM_TREE_REQUEST:
    if (pos != FOREST || isBase == TRUE || isPill == TRUE) {
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_NO_TREE));
      proceed = FALSE;
    }
    break;
  case LGM_ROAD_REQUEST:
    if (pos == FOREST && isPill == FALSE && isBase == FALSE) {
	  /* Clicked on a tree that doesn't contain a base or pill on it */
      *action = LGM_TREE_REQUEST;
    } else if (pos == BOAT || pos == DEEP_SEA || pos == BUILDING || pos == HALFBUILDING || isPill == TRUE || isBase == TRUE) {
	  /* Clicked one of the following a boat, deep sea, building, half building, pill, base  */
      proceed = FALSE;
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_NO_BUILD));
    } else if (pos == ROAD) {
	  /* Clicked on a road */
      proceed = FALSE;
    } else if (pos == RIVER && mapX == tankX && mapY == tankY && tankIsOnBoat(tnk)) {
	  /* Clicked on a square that has a tank on a boat on it */
	  proceed = FALSE;
	  messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_NO_BUILD_UNDER_BOAT));
    } else if (tankGetLgmTrees(tnk, LGM_COST_ROAD, perform) == FALSE) {
      proceed = FALSE;
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_INSUFFICIENT_TREES));
	} else {
      *trees = LGM_COST_ROAD;
    }
    break;
  case LGM_BUILDING_REQUEST:
    if (pos == FOREST && isPill == FALSE && isBase == FALSE) {
      *action = LGM_TREE_REQUEST;
    } else if (pos == BOAT || pos == DEEP_SEA || isPill == TRUE || isBase == TRUE) {
      proceed = FALSE;
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_NO_BUILD));
    } else if (pos == RIVER && mapX == tankX && mapY == tankY && tankIsOnBoat(tnk)) {
	  /* Clicked on a square that has a tank on a boat on it */
	  proceed = FALSE;
	  messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_NO_BUILD_UNDER_BOAT));
    } else if (pos == RIVER) {
	  /* Build a wall on a river, that means build a boat */
      *action = LGM_BOAT_REQUEST;
      if (tankGetLgmTrees(tnk, LGM_COST_BOAT, perform) == FALSE) {
        proceed = FALSE;
        messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_INSUFFICIENT_TREES));
      } else {
        *trees = LGM_COST_BOAT;
      }
        
    } else if (tankX == mapX && tankY == mapY) {
      proceed = FALSE;
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_BUILDTANK));
    } else if (pos == HALFBUILDING) {
      if (tankGetLgmTrees(tnk, LGM_COST_REPAIRBUILDING, perform) == FALSE) {
        proceed = FALSE;
        messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_INSUFFICIENT_TREES));
      } else {
        *trees = LGM_COST_REPAIRBUILDING;
      }
    } else if (pos == BUILDING) {
      proceed = FALSE;
    } else if (tankGetLgmTrees(tnk, LGM_COST_BUILDING, perform) == FALSE) {
        proceed = FALSE;
        messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_INSUFFICIENT_TREES));
    } else {
      *trees = LGM_COST_BUILDING;
    }

    break;
  case LGM_PILL_REQUEST:
    if (pos == BOAT || pos == DEEP_SEA || pos == BUILDING || pos == HALFBUILDING || pos == RIVER || isBase == TRUE) {
      proceed = FALSE;
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_NO_BUILD));
    } else if (pos == FOREST && isPill == FALSE) {
      *action = LGM_TREE_REQUEST;
    } else if (tankX == mapX && tankY == mapY) {
      proceed = FALSE;
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_BUILDTANK));
    } else if (isPill == TRUE) {
      if (pillsGetArmourPos(pb, mapX, mapY) == PILLS_MAX_ARMOUR) {
        proceed= FALSE;
        messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_PILL_NO_NEED_REPAIR));
      } else if (tankTrees<LGM_COST_PILLREPAIR) {
        proceed = FALSE;
        messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_INSUFFICIENT_TREES));
      } else {
		pillArmour = pillsGetArmourPos(pb, mapX, mapY);
		if(pillArmour>=11){
          *trees = LGM_COST_PILLREPAIR;
		}
		if(pillArmour>=7&&pillArmour<11){
		  *trees = LGM_COST_PILLREPAIR*2;
		}
		if(pillArmour>=3&&pillArmour<7){
		  *trees = LGM_COST_PILLREPAIR*3;
		}
		if(pillArmour<3){
		  *trees = LGM_COST_PILLREPAIR*4;
		}
		if(tankTrees<*trees)
		{
			*trees = LGM_COST_PILLREPAIR*tankTrees;
		}
		tankGetLgmTrees(tnk, *trees, perform);
      }
      *pillNum = LGM_NO_PILL;
    } else if ((tankGetCarriedPill(tnk, pillNum, perform)) == FALSE) {
      proceed = FALSE;
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_NO_PILLS));
    } else if (tankGetLgmTrees(tnk, LGM_COST_PILLNEW, perform) == FALSE) {
      proceed = FALSE;
      tankPutCarriedPill(tnk, *pillNum);
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_INSUFFICIENT_TREES));
    } else {
      *trees = LGM_COST_PILLNEW;
    }
    break;
  case LGM_BOAT_REQUEST:
    if (pos != RIVER || isPill == TRUE || isBase == TRUE) {
      proceed = FALSE;
    } else if (tankGetLgmTrees(tnk, LGM_COST_BOAT, perform) == FALSE) {
      proceed = FALSE;
    } else {
      *trees = LGM_COST_BOAT;
    }
    break;
  default:
    /* Case LGM_REQUEST_MINE */
    if (pos == DEEP_SEA || pos == RIVER || pos == BUILDING || pos == BOAT || pos == HALFBUILDING || isPill == TRUE || isBase == TRUE) {
      proceed = FALSE;
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_NO_BUILD));
    } else if (tankGetLgmMines(tnk, LGM_COST_MINE, perform) == FALSE) {
      proceed = FALSE;
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_INSUFFICIENT_MINES));
    } else {
      *minesAmount = LGM_COST_MINE;
    }
    break;
  }
  if (*action == LGM_TREE_REQUEST && pos != FOREST) {
    proceed = FALSE;
    messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_NO_TREE));
  }

  if (proceed == TRUE) {
    /* Check for a visible mine */
    if ((minesExistPos(screenGetMines(), mapX, mapY)) == TRUE) {
      *isMine = TRUE;
      proceed = FALSE;
      messageAdd(assistantMessage, langGetText(MESSAGE_ASSISTANT), langGetText2(LGM_PILL_NO_BUILD_ON_MINE));
      if ((*lgman)->nextAction == LGM_MINE_REQUEST && (*lgman)->nextX == mapX && (*lgman)->nextY == mapY) {
        (*lgman)->nextAction = LGM_IDLE;
      }
    }
  }

  return proceed;
}

/*********************************************************
*NAME:          lgmNewPrimaryRequest
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  A new primary request is wanted. Check if it is
*  possible. If so then make it so.
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  tnk    - Pointer to the tank structure
*  bs     - Pointer to the base structure
*  mapX   - X Co-ordinate of the new action
*  mapY   - Y Co-ordinate of the new action
*  action - What the new action is
*********************************************************/
void lgmNewPrimaryRequest(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE mapX, BYTE mapY, BYTE action) {
  BYTE pillNum;
  bool isMine;
  BYTE trees;
  BYTE minesAmount;

  pillNum = LGM_NO_PILL;

  /* If its OK to proceed then set it up */
  if (lgmCheckNewRequest(lgman, mp, pb, bs, tnk, mapX, mapY, &action, &pillNum, &isMine, &trees, &minesAmount, TRUE) == TRUE) {
    (*lgman)->numTrees = trees;
    (*lgman)->numMines = minesAmount;
    if (isMine == TRUE) {
      (*lgman)->state = LGM_STATE_IDLE;
      (*lgman)->inTank = TRUE;
      lgmBackInTank(lgman, mp, pb, bs, tnk, FALSE);
    } else {
      (*lgman)->destX = mapX;
      (*lgman)->destX <<= TANK_SHIFT_MAPSIZE;
      (*lgman)->destY = mapY;
      (*lgman)->destY <<= TANK_SHIFT_MAPSIZE;
      (*lgman)->destX += MAP_SQUARE_MIDDLE;
      (*lgman)->destY += MAP_SQUARE_MIDDLE;
      tankGetWorld(tnk, &((*lgman)->x), &((*lgman)->y));
      (*lgman)->state = LGM_STATE_GOING;
      (*lgman)->action = action;
      (*lgman)->inTank = FALSE;
      switch (action) {
      case LGM_ROAD_REQUEST:
        if (mapGetPos(mp, mapX, mapY) == RIVER) {
          (*lgman)->blessX = mapX;
          (*lgman)->blessY = mapY;
        }
        break;
       case LGM_BOAT_REQUEST:
        (*lgman)->blessX = mapX;
        (*lgman)->blessY = mapY;
        break;
      case LGM_BUILDING_REQUEST:
        (*lgman)->blessX = mapX;
        (*lgman)->blessY = mapY;
        break;
      case LGM_PILL_REQUEST:
        (*lgman)->numPills = pillNum;
        (*lgman)->blessX = mapX;
        (*lgman)->blessY = mapY;
        break;
      case LGM_MINE_REQUEST:
        break;
      default:
        /* Do nothing */
        break;
      }
    }
    if (netGetType() == netUdp || threadsGetContext() == TRUE) {
      serverNetSendManWorkingMessage(screenGetTankPlayer(tnk), mapX, mapY, minesAmount, trees, pillNum);
    }
  } else {
    (*lgman)->state = LGM_STATE_IDLE;
    tankGiveTrees(tnk, trees);
    tankGiveMines(tnk, minesAmount);
    if (pillNum != LGM_NO_PILL) {
      tankPutCarriedPill(tnk, pillNum);
    }
    if (threadsGetContext() == TRUE) {
      serverNetSendManReturnMessge(screenGetTankPlayer(tnk), trees, minesAmount, pillNum);
    }

  }
}


bool lgmCheckBlessedSquare(BYTE xValue, BYTE yValue, BYTE action, map *mp, pillboxes *pb, bases *bs, tank *tnk) {
  bool returnValue; /* Value to return */
  BYTE pos;
  bool isBase;
  bool isPill;

  returnValue = TRUE;
  pos = mapGetPos(mp, xValue, yValue);
  isBase = basesExistPos(bs, xValue, yValue);
  isPill = pillsExistPos(pb, xValue, yValue);

  switch (action) {
  case LGM_TREE_REQUEST:
    if (pos == BUILDING || pos == HALFBUILDING || pos == RIVER || pos == BOAT || pos == DEEP_SEA || isPill == TRUE ) {
      returnValue = FALSE;
    }
    break;
  case LGM_ROAD_REQUEST:
    if (pos == BUILDING || pos == HALFBUILDING || pos == BOAT || pos == DEEP_SEA || isPill == TRUE ) {
      returnValue = FALSE;
    }
    break;
  case LGM_BUILDING_REQUEST:
    if (pos == BUILDING || pos == RIVER || pos == BOAT || pos == DEEP_SEA || isPill == TRUE) {
      returnValue = FALSE;
    } 

    break;
  case LGM_PILL_REQUEST:
    if (pos == BUILDING || pos == HALFBUILDING || pos == RIVER || pos == BOAT || pos == DEEP_SEA) {
      returnValue = FALSE;
    }
    break;
  case LGM_BOAT_REQUEST:
    if (pos != RIVER || isPill == TRUE) {
      returnValue = FALSE;
    }
    break;
  default:
    /* Case LGM_REQUEST_MINE */
    if (pos == DEEP_SEA || pos == RIVER || pos == BUILDING || pos == BOAT || pos == HALFBUILDING || isPill == TRUE || isBase == TRUE) {
      returnValue = FALSE;
    }
    break;
  }
  return returnValue;
}

/*********************************************************
*NAME:          lgmMoveAway
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Man is moving towrads his destination.
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the base structure
*  tnk    - Pointer to the tank structure
*********************************************************/
void lgmMoveAway(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk) {
  WORLD conv;
  BYTE speed;     /* speed of terrain currently on */
  TURNTYPE angle; /* Angle of travel */
  TURNTYPE frontAngle; /* Front end angle of travel */
  BYTE bmx;       /* Man Map co-ords */
  BYTE bmy;
  WORLD newmx;
  WORLD newmy;
  BYTE newbmx;
  BYTE newbmy;
  int xAdd;       /* Add amounts */
  int yAdd;
  bool noGo;      /* Set to true if one drection couldn't move */
  bool onBoat;    /* Is the tank on a boat */

  /* Deal with the tank being on a boat */
  onBoat = lgmCheckTankBoat(lgman, tnk, LGM_TANKBOAT_LEAVE);

  noGo = FALSE;
  tankGetWorld(tnk, &newmx, &newmy);
  angle = utilCalcAngle((*lgman)->x, (*lgman)->y, (*lgman)->destX, (*lgman)->destY);
  frontAngle = utilCalcAngle((*lgman)->x, (*lgman)->y, newmx, newmy);
  if (threadsGetContext() == FALSE) {
    frontEndManStatus(FALSE, frontAngle);
  }
  conv = (*lgman)->x;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmx = (BYTE) conv;
  conv = (*lgman)->y;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmy = (BYTE) conv;
  
  if ((bmx == (*lgman)->blessX && bmy == (*lgman)->blessY) || onBoat == TRUE) {
    speed = MAP_MANSPEED_TREFBASE;
  } else {
    speed = mapGetManSpeed(mp, pb, bs, bmx, bmy, (*lgman)->playerNum);
  }
  
  utilCalcDistance(&xAdd, &yAdd, angle, speed);
  newmx = (WORLD) ((*lgman)->x + xAdd);
  newmy = (WORLD) ((*lgman)->y + yAdd);

  newmx >>= TANK_SHIFT_MAPSIZE;
  newmy >>= TANK_SHIFT_MAPSIZE;
  newbmx = (BYTE) newmx;
  newbmy = (BYTE) newmy;

  /* Huh? */
  if ((mapGetManSpeed(mp, pb, bs, bmx, newbmy, (*lgman)->playerNum)) > 0 || onBoat == TRUE) {

  }

  
/*  utilCalcDistance(&xAdd, &yAdd, angle, speed);
  newmx = (WORLD) ((*lgman)->x + xAdd);
  newmy = (WORLD) ((*lgman)->y + yAdd);

  newmx >>= TANK_SHIFT_MAPSIZE;
  newmy >>= TANK_SHIFT_MAPSIZE;
  newbmx = (BYTE) newmx;
  newbmy = (BYTE) newmy; */

  if ((mapGetManSpeed(mp, pb, bs, bmx, newbmy, (*lgman)->playerNum)) > 0 || onBoat == TRUE) {
    (*lgman)->y = (WORLD) ((*lgman)->y  + yAdd);
  } else if (bmx == (*lgman)->blessX && newbmy == (*lgman)->blessY && lgmCheckBlessedSquare(bmx, newbmy, (*lgman)->action, mp, pb, bs, tnk) == TRUE) {
    (*lgman)->y = (WORLD) ((*lgman)->y + yAdd);
    
//  } else if (bmx == (*lgman)->blessX && newbmy == (*lgman)->blessY) {
//    (*lgman)->y = (WORLD) ((*lgman)->y + yAdd);
  } else {
    (*lgman)->obstructed = LGM_BRAIN_PARTIAL;
    noGo = TRUE;
    newbmy = bmy;
  }
  if (((mapGetManSpeed(mp, pb, bs, newbmx, newbmy,(*lgman)->playerNum) )) > 0 && xAdd != 0 || onBoat == TRUE) {
    (*lgman)->x = (WORLD) ((*lgman)->x + xAdd);
  } else if (newbmx == (*lgman)->blessX && bmy == (*lgman)->blessY && lgmCheckBlessedSquare(newbmx, newbmy, (*lgman)->action, mp, pb, bs, tnk) == TRUE) {
    (*lgman)->x = (WORLD) ((*lgman)->x + xAdd);
//  } else if (newbmx == (*lgman)->blessX && bmy == (*lgman)->blessY) {
//    (*lgman)->x = (WORLD) ((*lgman)->x + xAdd);
  } else if (noGo == TRUE || yAdd == 0) {
    (*lgman)->state = LGM_STATE_RETURN;
  } 

  /* Check for achieved goal */
  if (((*lgman)->x - (*lgman)->destX) >= LGM_MIN_GOAL  && ((*lgman)->x - (*lgman)->destX) <= LGM_MAX_GOAL && ((*lgman)->y - (*lgman)->destY) >= LGM_MIN_GOAL && ((*lgman)->y - (*lgman)->destY) <= LGM_MAX_GOAL) {
    /* Arrived */
    (*lgman)->waitTime = LGM_BUILD_TIME;
    (*lgman)->state = LGM_STATE_RETURN;
    lgmDoWork(lgman, mp, pb, bs);
  }

}


/*********************************************************
*NAME:          lgmReturn
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 2/12/00
*PURPOSE:
*  Man is moving towards the tank
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillbox structure
*  bs     - Pointer to the base structure
*  tnk    - Pointer to the tank structure
*********************************************************/
void lgmReturn(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk) {
  WORLD conv;
  BYTE speed;     /* speed of terrain currently on */
  TURNTYPE angle; /* Angle of travel */
  BYTE bmx;       /* Man Map co-ords */
  BYTE bmy;
  WORLD newmx;
  WORLD newmy;
  BYTE newbmx;
  BYTE newbmy;
  BYTE newTerrainX=0,newTerrainY=0,currentTerrain=0;
  BYTE onTopEdgeX=0,onTopEdgeY=0;
  bool sameTerrainTypeX = FALSE,sameTerrainTypeY = FALSE;
  int xAdd;       /* Add amounts */
  int yAdd;
  bool onBoat;    /* Is the tank on a boat */

  tankGetWorld(tnk, &newmx, &newmy);

  /* Deal with the tank being on a boat */
  onBoat = lgmCheckTankBoat(lgman, tnk, LGM_TANKBOAT_RETURN);
  
  angle = utilCalcAngle((*lgman)->x, (*lgman)->y, newmx, newmy);

  if (threadsGetContext() == FALSE) {
    frontEndManStatus(FALSE, angle);
  }
  
  conv = (*lgman)->x;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmx = (BYTE) conv;
  conv = (*lgman)->y;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmy = (BYTE) conv;
  
  if (tankGetArmour(tnk) > TANK_FULL_ARMOUR || newmx == 0 || newmy == 0) {
    speed = 0;
    return;
  } else if ((bmx == (*lgman)->blessX && bmy == (*lgman)->blessY) || onBoat == TRUE) {
    speed = MAP_MANSPEED_TREFBASE;
  } else {
    speed = mapGetManSpeed(mp, pb, bs, bmx, bmy, (*lgman)->playerNum);
  }
  if (speed == 0) {
    /* Do Nothing */

  }
  utilCalcDistance(&xAdd, &yAdd, angle, speed);
  newmx = (WORLD) ((*lgman)->x + xAdd);
  newmy = (WORLD) ((*lgman)->y + yAdd);

  newmx >>= TANK_SHIFT_MAPSIZE;
  newmy >>= TANK_SHIFT_MAPSIZE;
  newbmx = (BYTE) newmx;
  newbmy = (BYTE) newmy;

  if ((mapGetManSpeed(mp,pb,bs,bmx,newbmy, (*lgman)->playerNum)) > 0 || onBoat == TRUE) {
    (*lgman)->y = (WORLD) ((*lgman)->y + yAdd);
  } else if (bmx == (*lgman)->blessX && newbmy == (*lgman)->blessY) {
    (*lgman)->y = (WORLD) ((*lgman)->y + yAdd);
  } else {
    (*lgman)->obstructed = LGM_BRAIN_PARTIAL;
    newbmy = bmy;
  }
  if ((mapGetManSpeed(mp,pb,bs,newbmx,newbmy, (*lgman)->playerNum)) > 0 || onBoat == TRUE) {
    (*lgman)->x = (WORLD) ((*lgman)->x + xAdd);
  } else if (newbmx == (*lgman)->blessX && newbmy == (*lgman)->blessY) {
    (*lgman)->x = (WORLD) ((*lgman)->x + xAdd);
  } else {
    /* Totally obstructed */
    (*lgman)->obstructed = LGM_BRAIN_TOTAL;
  }

  /* If we have moved off the blesssed build square its no longer blessed */
  if (((*lgman)->x >> TANK_SHIFT_MAPSIZE) != (*lgman)->blessX || ((*lgman)->y >> TANK_SHIFT_MAPSIZE) != (*lgman)->blessY) {
    (*lgman)->blessX = 0;
    (*lgman)->blessY = 0;
  }

  if((*lgman)->onTop == TRUE){
	/*
	  ok we landed ontop of a obstruction, so, we're going to keep moving get speed, calculate movement.
	*/
    speed = 6; 
	/* speed is a magic number currently, once we detemine what the ontop speed for the various types of 
	   terrain are, this will be adjusted
	*/ 
	utilCalcDistance(&xAdd, &yAdd, angle, speed);
	/* once this algorithm is completed, the below edge numbers need to be defined instead of magic */
	onTopEdgeX = 0;
	onTopEdgeY = 0;
    newmx = (WORLD) ((*lgman)->x + xAdd + onTopEdgeX); /* ontopedge x and y are to give an 'edge' to the obstruction */
    newmy = (WORLD) ((*lgman)->y + yAdd + onTopEdgeY); 

    newmx >>= TANK_SHIFT_MAPSIZE;
    newmy >>= TANK_SHIFT_MAPSIZE;
    newbmx = (BYTE) newmx;
    newbmy = (BYTE) newmy;
    /*
	  ok here we're going to have to check for edges, if we see the edge of the obstruction, we have to stop
	  this is so we don't go outside of the obstruction

      we have to check for edges on the x sides, and the y sides. edges will be determined by weither the terrain next to us
	  is a obstructed object also, if it is, we will run over onto it also, this will allow us to run around ontop
	  of blocks of pills, or bases, so we have to check for any pillbox, enemy bases, and buildings or half buildings.
	
	  so, we will need to know first how close we are to the edge, if we're within a certain number of world coordinates to the 
	  edge of the tile we're on, do a check for a tile we can run onto in the neighbouring area.
	*/
	/* detect the terrain types, don't check diagonal squares. becuase the lgm can't run ontop of diagonal squares */
	currentTerrain = mapGetPos(mp,bmx,bmy);
	newTerrainX = mapGetPos(mp,newbmx,bmy);
   	newTerrainY = mapGetPos(mp,bmx,newbmy);

    if (currentTerrain == BUILDING||currentTerrain == HALFBUILDING){
		if (newTerrainX == BUILDING||newTerrainX == HALFBUILDING){
			sameTerrainTypeX = TRUE;
		}
		if (newTerrainY == BUILDING||newTerrainY == HALFBUILDING){
			sameTerrainTypeY = TRUE;
		}
	} else {
		/* do the check to see if pills/bases are beside each other, to allow the lgm to run around ontop of pills or bases.
		   if pills exist in the current location, then check to see if the new location is also a pill, if it is, you can go onto it.
		*/
		if (pillsExistPos(pb, bmx, bmy) == TRUE){
			if (pillsExistPos(pb, newbmx, bmy) == TRUE){
				sameTerrainTypeX = TRUE;
			}
			if (pillsExistPos(pb, bmx, newbmy) == TRUE){
				sameTerrainTypeY = TRUE;
			}
		} else {
			/* bases will be a bit more complicated, I will have to check for base ownership.
			   can't have tbe lgm running over onto a based owned by the player.
			*/
			if (basesExistPos(bs, bmx, bmy) == TRUE)
			{
				if (basesExistPos(bs, newbmx, bmy) == TRUE){
					if(basesAmOwner(bs,(*lgman)->playerNum, newbmx, bmy)==TRUE){
					  sameTerrainTypeX = FALSE;
					} else {
  					  sameTerrainTypeX = TRUE;
					}
				}
				if (basesExistPos(bs, bmx, newbmy) == TRUE){
					if(basesAmOwner(bs,(*lgman)->playerNum, bmx, newbmy)==TRUE){
					  sameTerrainTypeY = FALSE;
					} else {
					  sameTerrainTypeY = TRUE;
					}
				}
			} else {
			  /* must not be ontop of a building anymore, since its failed all the checks, onTop = false*/
			  (*lgman)->onTop = FALSE;
			}
		} 
	}
	// if the lgm's old mapx coordinate is still the same, then we can add xAdd and yAdd to the lgm's coordinates)
	// ok so the below works, now we need to modify it, to allow us to run onto tiles that are next to us.
	if (bmx == newbmx||sameTerrainTypeX == TRUE){
	  (*lgman)->x = (WORLD) ((*lgman)->x + xAdd);
	}
	if (bmy == newbmy||sameTerrainTypeY == TRUE){
	  (*lgman)->y = (WORLD) ((*lgman)->y + yAdd);
	}
	speed = 0;
  }

  /* Check for achieved goal */
  tankGetWorld(tnk, &newmx, &newmy);
  if (((*lgman)->x - newmx) >= LGM_RETURN_MIN_GOAL && ((*lgman)->x - newmx) <= LGM_RETURN_MAX_GOAL && ((*lgman)->y - newmy) >= LGM_RETURN_MIN_GOAL && ((*lgman)->y - newmy) <= LGM_RETURN_MAX_GOAL ) {
    /* Arrived back at tank */
    (*lgman)->state = LGM_STATE_IDLE;
    (*lgman)->inTank = TRUE;
    (*lgman)->action = LGM_IDLE;
    (*lgman)->blessX = 0;
    (*lgman)->blessY = 0;
/*    if (threadsGetContext() == TRUE) {
      serverNetSendManReturnMessge(screenGetTankPlayer(tnk), (*lgman)->numTrees, (*lgman)->numMines, (*lgman)->numPills);
    } */
    lgmBackInTank(lgman, mp, pb, bs, tnk, TRUE);
    if (threadsGetContext() == FALSE) {
      frontEndManClear();
    }
  }
}



/*********************************************************
*NAME:          lgmDoWork
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Man has arrived at objective. Do whatever it is he
*  came to do.
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  mp - Pointer to the map structure
*  pb - Pointer to the pillbox structure 
*  bs - Pointer to the bases structure 
*********************************************************/
void lgmDoWork(lgm *lgman, map *mp, pillboxes *pb, bases *bs) {
  WORLD conv;      /* Used for conversion */
  BYTE bmx;        /* Dest Map co-ords */
  BYTE bmy;
  pillbox addPill; /* Pill to add if required */
  bool isMine;     /* Is the item a mine */
  bool isPill;
  bool isBase;
  BYTE terrain;

  conv = (*lgman)->destX;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmx = (BYTE) conv;
  conv = (*lgman)->destY;
  conv >>= TANK_SHIFT_MAPSIZE;
  bmy = (BYTE) conv;
  isPill = pillsExistPos(pb, bmx, bmy);
  isBase = basesExistPos(bs, bmx, bmy);

  isMine = FALSE;
  terrain = mapGetPos(mp, bmx, bmy);
  if (terrain >= MINE_START && terrain <= MINE_END) {
    terrain -= MINE_SUBTRACT;
    isMine = TRUE;
  }


  switch ((*lgman)->action) {
  case LGM_TREE_REQUEST:
    minesExpAddItem(screenGetMinesExp(), mp, bmx, bmy);
    if (terrain == FOREST && isBase == FALSE && isPill == FALSE) {
      if (isMine == TRUE) {
        mapSetPos(mp, bmx, bmy, (BYTE) (GRASS+MINE_SUBTRACT), TRUE, FALSE);
        netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, 0, bmx, bmy);
      } else {
        mapSetPos(mp, bmx, bmy, GRASS, TRUE, FALSE);
      }
      (*lgman)->numTrees = LGM_GATHER_TREE;
      netPNBAdd(screenGetNetPnb(), NPNB_LGM_FARMTREE, 0, (*lgman)->playerNum, bmx, bmy, 0);
      soundDist(farmingTreeNear, bmx, bmy);
    }
    screenReCalc();
    break;
  case LGM_ROAD_REQUEST:
/* HUH?    minesExpAddItem(mp, bmx, bmy); */
    if (terrain != BUILDING && terrain != HALFBUILDING && terrain != BOAT && terrain != DEEP_SEA && isPill == FALSE && isBase == FALSE) {
      if (isMine == TRUE) {
        mapSetPos(mp, bmx, bmy, (BYTE) (ROAD+MINE_SUBTRACT), TRUE, FALSE);
        netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, 0, bmx, bmy);
      } else {
        mapSetPos(mp, bmx, bmy, ROAD, TRUE, FALSE);
      }
      (*lgman)->numTrees = 0;
      netPNBAdd(screenGetNetPnb(), NPNB_LGM_BUILDROAD, 0, (*lgman)->playerNum, bmx, bmy, 0);
      soundDist(manBuildingNear, bmx, bmy);
    
      lgmCheckRemove(terrain, bmx, bmy);
      screenReCalc();
    }
    break; 
  case LGM_BUILDING_REQUEST:
    if (terrain != BUILDING && terrain != RIVER && terrain != BOAT && terrain != DEEP_SEA && isPill == FALSE && isBase == FALSE) {
      if (isMine != TRUE) {
        mapSetPos(mp, bmx, bmy, BUILDING, TRUE, FALSE);
      } else {
        netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, 0, bmx, bmy);
        minesExpAddItem(screenGetMinesExp(), mp, bmx, bmy);
      }
      (*lgman)->numTrees = 0;
      soundDist(manBuildingNear, bmx, bmy);
      netMNTAdd(screenGetNetMnt(), NMNT_BUILDBUILDING, 0, (*lgman)->playerNum, bmx, bmy);
      lgmCheckRemove(terrain, bmx, bmy);
      screenReCalc();
    }
    break;
  case LGM_BOAT_REQUEST:
    if (terrain == RIVER) {
      mapSetPos(mp, bmx, bmy, BOAT, TRUE, FALSE);
      netMNTAdd(screenGetNetMnt(), NMNT_BUILDBOAT, 0, (*lgman)->playerNum, bmx, bmy);
      (*lgman)->numTrees = 0;
      screenReCalc();
    }
    break;
  case LGM_MINE_REQUEST:
    if ((isPill == FALSE && isBase == FALSE) && (terrain == SWAMP || terrain == CRATER || terrain == ROAD || terrain == FOREST || terrain == RUBBLE || terrain == GRASS)) {
      if (isMine == TRUE) {
        minesExpAddItem(screenGetMinesExp(), mp, bmx, bmy);
        netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, 0, bmx, bmy);
      } else {
        netMNTAdd(screenGetNetMnt(), NMNT_MINEPLACE, 0, (*lgman)->playerNum, bmx, bmy);
        mapSetPos(mp, bmx, bmy, (BYTE) (terrain + MINE_SUBTRACT), FALSE, FALSE);
        minesAddItem(screenGetMines(), bmx, bmy);
        (*lgman)->numMines = 0;
        soundDist(manLayingMineNear, bmx, bmy);
      }
      screenReCalc();
    }
    break;
  case LGM_PILL_REQUEST:
    addPill.x = bmx;
    addPill.y = bmy;
    if ((*lgman)->numPills == LGM_NO_PILL) {
      /* Repair pill */
      if (isPill == TRUE) {
        pillsRepairPos(pb, bmx, bmy,(*lgman)->numTrees);
        netPNBAdd(screenGetNetPnb(), NPNB_PILL_REPAIR, 0, (*lgman)->playerNum, bmx, bmy,(*lgman)->numTrees);
		(*lgman)->numTrees = 0;
        soundDist(manBuildingNear, bmx, bmy);
      }
    } else {
      if (isPill == FALSE && isBase == FALSE && minesExistPos(screenGetMines(), bmx, bmy) == FALSE && terrain != BUILDING && terrain != HALFBUILDING && terrain != RIVER && terrain != BOAT && terrain != DEEP_SEA) {
        if (isMine == TRUE) {
          netMNTAdd(screenGetNetMnt(), NMNT_MINEEXPLOSION, 0, 0, bmx, bmy);
          minesExpAddItem(screenGetMinesExp(), mp, bmx, bmy);
          floodAddItem(screenGetFloodFill(), bmx, bmy);
          lgmDeathCheck(lgman, mp, pb, bs, (WORLD) ((bmx << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), (WORLD) ((bmy<< M_W_SHIFT_SIZE)+MAP_SQUARE_MIDDLE), NEUTRAL);
          soundDist(mineExplosionNear, bmx, bmy);
          mapSetPos(mp, bmx, bmy, CRATER, TRUE, FALSE);
        } else {
          (*lgman)->numTrees = 0;
          addPill.owner = (*lgman)->playerNum;
          addPill.armour = PILLS_MAX_ARMOUR;
          addPill.speed = pillsGetAttackSpeed(pb, (*lgman)->numPills);
          addPill.coolDown = 0;
          addPill.inTank = FALSE;
          addPill.justSeen = FALSE;
          pillsSetPill(pb, &addPill, (*lgman)->numPills);
          netPNBAdd(screenGetNetPnb(), NPNB_PILL_DROP, (BYTE) ((*lgman)->numPills-1), (*lgman)->playerNum, addPill.x, addPill.y, 0);
          soundDist(manBuildingNear, bmx, bmy);
          if (threadsGetContext() == FALSE) {
            frontEndStatusPillbox((*lgman)->numPills, (pillsGetAllianceNum(pb, (*lgman)->numPills)));
          }
          (*lgman)->numPills = LGM_NO_PILL;
        }
      }
    }
    /*     if (isMine == TRUE) {
      netMNTAdd(NMNT_MINEEXPLOSION, 0, 0, bmx, bmy);
      minesExpAddItem(mp, bmx, bmy);
      floodAddItem(bmx, bmy);
      lgmDeathCheck(mp, pb, bs, (WORLD) ((bmx << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), (WORLD) ((bmy<< M_W_SHIFT_SIZE)+MAP_SQUARE_MIDDLE));
      soundDist(mineExplosionNear, bmx, bmy);
      mapSetPos(mp, bmx, bmy, CRATER, TRUE, FALSE);
    } else {
*/
    lgmCheckRemove(terrain, bmx, bmy);
    screenReCalc();
    break;
  default:
    /* do nothing */
    break;
  }
}


/*********************************************************
*NAME:          lgmBackInTank
*AUTHOR:        John Morrison
*CREATION DATE: 18/01/99
*LAST MODIFIED: 01/02/03
*PURPOSE:
*  Man has arrived back in tank. Dump stuff off
*
*ARGUMENTS:
*  lgman     - Pointer to the lgm sturcture
*  mp        - Pointer to the map structure
*  pb        - Pointer to the pillbox structure
*  bs        - Pointer to the base structure
*  tnk       - Pointer to the tank structure
*  sendItems - If TRUE, send the carriend items back
*              to the client
*********************************************************/
void lgmBackInTank(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk, bool sendItems) {
  BYTE trees;
  BYTE minesAmount;
  BYTE pillNum;

  minesAmount = trees = 0;
  pillNum = LGM_NO_PILL;
  if ((*lgman)->numTrees > 0) {
    tankGiveTrees(tnk, (*lgman)->numTrees);
    trees = (*lgman)->numTrees;
    (*lgman)->numTrees = 0;
  }
  if ((*lgman)->numPills != LGM_NO_PILL) {
    tankPutCarriedPill(tnk, (*lgman)->numPills);
    pillNum = (*lgman)->numPills;
    (*lgman)->numPills = LGM_NO_PILL;
  }
  if ((*lgman)->numMines > 0) {
    minesAmount = (*lgman)->numMines;
    tankAddMines(tnk, (*lgman)->numMines);
    minesAmount = (*lgman)->numMines;
    (*lgman)->numMines = 0;
  }

  if (sendItems == FALSE) {
    trees = 0;
    minesAmount = 0;
    pillNum = LGM_NO_PILL;
  }
  // Added
  if (threadsGetContext() == TRUE) {
    serverNetSendManReturnMessge(screenGetTankPlayer(tnk), trees, minesAmount, pillNum);
  } else {
    if ((*lgman)->nextAction != LGM_IDLE) {  
      if (/* FIXME - Removed for lgm problem threadsGetContext() == TRUE || */ netGetType() == netSingle) {
        lgmNewPrimaryRequest(lgman, mp, pb, bs, tnk, (*lgman)->nextX, (*lgman)->nextY, (*lgman)->nextAction);
      } else {
        /* Network game */
        BYTE pillNum;
        bool isMine;
        BYTE trees;
        BYTE minesAmount;
        bool ok;
        BYTE action;
        action = (*lgman)->nextAction;
        ok = lgmCheckNewRequest(lgman, mp, pb, bs, tnk, (*lgman)->nextX, (*lgman)->nextY, &action, &pillNum, &isMine, &trees, &minesAmount, FALSE);
        (*lgman)->numTrees = trees;
        (*lgman)->numMines = minesAmount;

        if (ok) {
          netMNTAdd(screenGetNetMnt(), NMNT_MANACTION, action, (*lgman)->playerNum, (*lgman)->nextX, (*lgman)->nextY);
          (*lgman)->action = action;
        }
      }   
      (*lgman)->nextAction = LGM_IDLE;
    }
  }
}

/*********************************************************
*NAME:          lgmOnScreen
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns wehther the man is on screen
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  leftPos   - Left bounds of screen
*  rightPos  - Right bounds of screen
*  top    - Top bounds of screen
*  bottom - Bottom bounds of screen
*********************************************************/
bool lgmOnScreen(lgm *lgman, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  bool returnValue; /* Value to return */
  BYTE x;     /* Map X and Y Positions (relative to screen) */
  BYTE y;
 
  returnValue = FALSE;
  x = (BYTE) (( (unsigned int)(*lgman)->x -1) >> TANK_SHIFT_MAPSIZE);
  y = (BYTE) (((unsigned int) (*lgman)->y -2) >> TANK_SHIFT_MAPSIZE);
  if (x >= leftPos && x < rightPos && y >= top && y < bottom && (*lgman)->state != LGM_STATE_IDLE && (*lgman)->inTank == FALSE) {
    returnValue = TRUE; 
  }
  return returnValue;
}

/*********************************************************
*NAME:          lgmGetScreenCoords
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns wehther the man is on screen
*
*ARGUMENTS:
*  lgman   - Pointer to the lgm sturcture
*  leftPos - Left bounds of screen
*  topPos  - Top bounds of screen
*  mx      - X Map co ord
*  my      - Y Map co ord
*  px      - X pixel co ord
*  py      - Y pixel co ord
*********************************************************/
void lgmGetScreenCoords(lgm *lgman, BYTE leftPos, BYTE topPos, BYTE *mx, BYTE *my, BYTE *px, BYTE *py, BYTE *frame) {
  WORLD conv; /* Used for Bit shifting */

  if ((*lgman)->isDead == TRUE) {
    *frame = LGM_HELICOPTER_FRAME;
    conv = (*lgman)->x-MAP_SQUARE_MIDDLE;
    conv >>= TANK_SHIFT_MAPSIZE;
    *mx = (BYTE) conv - leftPos;
    conv = (*lgman)->y-MAP_SQUARE_MIDDLE;
    conv >>= TANK_SHIFT_MAPSIZE;
    *my = (BYTE) conv - topPos;
    conv = (*lgman)->x-MAP_SQUARE_MIDDLE;
    conv <<= TANK_SHIFT_MAPSIZE;
    conv >>= TANK_SHIFT_PIXELSIZE;
    *px = (BYTE) conv;
    conv = (*lgman)->y-MAP_SQUARE_MIDDLE;
    conv <<= TANK_SHIFT_MAPSIZE;
    conv >>= TANK_SHIFT_PIXELSIZE;
    *py = (BYTE) conv;
  } else {
    *frame = (*lgman)->frame;
    conv = (*lgman)->x - 1;
    conv >>= TANK_SHIFT_MAPSIZE;
    *mx = (BYTE) conv - leftPos;
    conv = (*lgman)->y - 2;
    conv >>= TANK_SHIFT_MAPSIZE;
    *my = (BYTE) conv - topPos;
    conv = (*lgman)->x -1 ;
    conv <<= TANK_SHIFT_MAPSIZE;
    conv >>= TANK_SHIFT_PIXELSIZE;
    *px = (BYTE) conv;
    conv = (*lgman)->y -2;
    conv <<= TANK_SHIFT_MAPSIZE;
    conv >>= TANK_SHIFT_PIXELSIZE;
    *py = (BYTE) conv;
  }
}


/*********************************************************
*NAME:          lgmDeathCheck
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 02/02/04
*PURPOSE:
*  Called when an item explodes to check to see if the
*  lgm should be killed because he is on the screen.
*
*ARGUMENTS:
*  mp     - Pointer to the map structure
*  pb     - Pointer to the pillboxes structure
*  bs     - Pointer to the bases structure
*  wx     - X World co ord
*  wy     - Y World co ord
*  owner  - Who owned the firing shell (NEUTRAL for mines)
*********************************************************/
void lgmDeathCheck(lgm *lgman, map *mp, pillboxes *pb, bases *bs, WORLD wx, WORLD wy, BYTE owner) {
  char messageStr[FILENAME_MAX];       /* Message to output */
  char playerName[PLAYER_NAME_LEN]; /* Player name */
  BYTE lgmX;                        /* LGM X Map co-ordinate */
  BYTE lgmY;                        /* LGM Y Map co-ordinate */
  bool solid;                       /* Is the map square the lgm on solid or not */
  bool dead;                        /* Are we dead */
  TURNTYPE dummy;                   /* Dummy variable used for paremeter passing */
  pillbox item;   /* Item to add to the pillbox */
  double distance;
  BYTE pos;
  BYTE mx, my;
  bool finishedPillPlace; /* Used to place pills properly */
  BYTE pillPlaceX;
  BYTE pillPlaceY;
  BYTE count;
  

  if ((threadsGetContext() == TRUE || netGetType() == netSingle) && (*lgman)->isDead == FALSE && (*lgman)->inTank == FALSE) {
    WORLD conv;
    dead = FALSE;
    conv = (*lgman)->x - 1;
    conv >>= 8;
    lgmX = (BYTE) conv; 
    lgmY = (BYTE) ((unsigned int) ((*lgman)->y - 2) >> 8);
    messageStr[0] = '\0';
    playerName[0] = '\0';

    mx = (BYTE) (wx >> 8);
    my = (BYTE) (wy >> 8);

    utilIsItemInRange((*lgman)->x, (*lgman)->y, wx, wy, PILLBOX_RANGE, &distance);
    pos = mapGetPos(mp, mx, my);
    solid = FALSE;
    if (pos == BUILDING || pos == HALFBUILDING || pillsExistPos(pb, mx, my) == TRUE || basesExistPos(bs, mx, my) == TRUE) {
      solid = TRUE;
    }
    if (solid == FALSE && distance <= MAP_SQUARE_MIDDLE) {
      dead = TRUE;
    } else if (solid == TRUE && lgmX == mx && lgmY == my) {
      dead = TRUE;
    }

    if (dead == TRUE) {
      netPNBAdd(screenGetNetPnb(), NPNB_LGM_DEAD, FALSE, (*lgman)->playerNum, lgmX, lgmY, 0);
      soundDist(manDyingNear, lgmX, lgmY);
      (*lgman)->isDead = TRUE;
      (*lgman)->frame = LGM_HELICOPTER_FRAME;
      (*lgman)->numTrees = 0;
      (*lgman)->numMines = 0;
      (*lgman)->nextAction = LGM_IDLE;
      if ((*lgman)->numPills != LGM_NO_PILL) {
        /* Drop Pill */
        if (netGetType() == netSingle || threadsGetContext() == TRUE) {
          finishedPillPlace = FALSE;
          count = 0;
          pillPlaceX = lgmX;
          pillPlaceY = lgmY;
          while (finishedPillPlace == FALSE) {
            item.x = pillPlaceX;
            item.y = pillPlaceY+count;
            if (item.x > MAP_MINE_EDGE_LEFT && item.x < MAP_MINE_EDGE_RIGHT && item.y > MAP_MINE_EDGE_TOP && item.y < MAP_MINE_EDGE_BOTTOM) {
              pos = mapGetPos(mp, item.x, item.y);
              if (pillsExistPos(pb, item.x, item.y) == FALSE && basesExistPos(bs, item.x, item.y) == FALSE && pos != BUILDING && pos != HALFBUILDING && pos != BOAT) {
                finishedPillPlace = TRUE;
              }
            }
            count++;
            if (count == 10 && finishedPillPlace == FALSE) {
              count = 0;
              item.y = pillPlaceY;
              pillPlaceX++;
            }
          }
          item.armour = 0;
          item.owner = (*lgman)->playerNum;
          item.speed = PILLBOX_ATTACK_NORMAL;
          item.reload = PILLBOX_ATTACK_NORMAL;
          item.coolDown = 0;
          item.inTank = FALSE;
          item.justSeen = FALSE;
          pillsSetPill(pb,&item,(*lgman)->numPills);
          netPNBAdd(screenGetNetPnb(), NPNB_PILL_DEAD, (BYTE) ((*lgman)->numPills-1), (*lgman)->playerNum, item.x, item.y, 0);
          if (threadsGetContext() == FALSE) {
            frontEndStatusPillbox((*lgman)->numPills, (pillsGetAllianceNum(pb, (*lgman)->numPills)));
          }
        }
        (*lgman)->numPills = LGM_NO_PILL;
        screenReCalc();
      }
      screenGetTankWorldFromLgm(lgman, &((*lgman)->destX), &((*lgman)->destY));
      
      /* Check for tank in mines (i.e. dead) send builder back to spoke it died*/
      if ((*lgman)->destX <= ((MAP_MINE_EDGE_LEFT+1) << 8) || (*lgman)->destX >= ((MAP_MINE_EDGE_RIGHT-1) << 8) || (*lgman)->destY <= ((MAP_MINE_EDGE_TOP+1) << 8) || (*lgman)->destY >= ((MAP_MINE_EDGE_BOTTOM-1) << 8)) {
        (*lgman)->destX = (*lgman)->x;
        (*lgman)->destY = (*lgman)->y;
      }
	  
      screenGetRandStart(&lgmX, &lgmY, &dummy);
      (*lgman)->x = lgmX;
      (*lgman)->x <<= TANK_SHIFT_MAPSIZE;
      (*lgman)->x += MAP_SQUARE_MIDDLE;
      (*lgman)->y = lgmY;
      (*lgman)->y <<= TANK_SHIFT_MAPSIZE;
      (*lgman)->y += MAP_SQUARE_MIDDLE;
      if (threadsGetContext() == FALSE) {
        frontEndManStatus(TRUE, 0.0f);
      }

      /* Log it */
      logAddEvent(log_LostMan, (*lgman)->playerNum, 0, 0, 0, 0, NULL);
      /* WinBolo.net it */
      winbolonetAddEvent(WINBOLO_NET_EVENT_LGM_LOST, TRUE, (*lgman)->playerNum, WINBOLO_NET_NO_PLAYER);
      if (owner != NEUTRAL) {
        winbolonetAddEvent(WINBOLO_NET_EVENT_LGM_KILL, TRUE, owner, (*lgman)->playerNum);
      }
      /* Process message */
      playersGetPlayerName(screenGetPlayers(), (*lgman)->playerNum, playerName);
      labelMakeMessage(messageStr, playerName, langGetText(MESSAGE_THIS_COMPUTER));
      strcat(messageStr, langGetText(MESSAGE_LGM_DEAD));
      messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), messageStr);
    }
  }
}

/*********************************************************
*NAME:          lgmParchutingIn
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Man is parachuting back in.
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*********************************************************/
void lgmParchutingIn(lgm *lgman) {
  TURNTYPE angle; /* Angle of travel */
  int xAdd;       /* Add amounts */
  int yAdd;


  angle = utilCalcAngle((*lgman)->x, (*lgman)->y, (*lgman)->destX, (*lgman)->destY);
  utilCalcDistance(&xAdd, &yAdd, angle, LGM_HELICOPTER_SPEED);
  (*lgman)->x = (WORLD) ((*lgman)->x + xAdd);
  (*lgman)->y = (WORLD) ((*lgman)->y + yAdd);

  /* Check for achieved goal */
  if (((*lgman)->x - (*lgman)->destX) >= -16 && ((*lgman)->x - (*lgman)->destX) <=16 && ((*lgman)->y - (*lgman)->destY) >= -16 && ((*lgman)->y - (*lgman)->destY) <=16) {
    /* Arrived at drop off spot. Begin trek back to tank */
    if (threadsGetContext() == TRUE) {
      netPNBAdd(screenGetNetPnb(), NPNB_LGM_DEAD, TRUE, (*lgman)->playerNum, 0, 0, 0);
    }
   
    (*lgman)->isDead = FALSE;
    (*lgman)->state = LGM_STATE_RETURN;
    (*lgman)->frame = 0;
  }
}


/*********************************************************
*NAME:          lgmCheckRemove
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  An building operation has happened. Check to see if 
*  it should remove from grass/building data structures.
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  terrain - Terrain type of the sqaure
*  mx      - Map X position 
*  my      - Map Y position
*********************************************************/
void lgmCheckRemove(BYTE terrain, BYTE mx, BYTE my) {
  switch (terrain) {
  case HALFBUILDING:
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
*NAME:          lgmCheckTankBoat
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  This is to check the builder is close to the tank. If 
*  he is then it is considered "blessed" and can freely
*  travel to it.
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  tnk  - Pointer to the tank structure
*  dist - Distance to check
*********************************************************/
bool lgmCheckTankBoat(lgm *lgman, tank *tnk, WORLD dist) {
  WORLD newmx;      /* Used for gap calculations */
  WORLD newmy;
  bool returnValue; /* Value to Return */

  returnValue = FALSE;
  tankGetWorld(tnk, &newmx, &newmy);
  if (tankIsOnBoat(tnk) == TRUE) {
    if ((newmx - (*lgman)->x) < 0) {
      newmx = (*lgman)->x - newmx;
    } else {
      newmx = newmx - (*lgman)->x;
    }
    if ((newmy - (*lgman)->y) < 0) {
      newmy = (*lgman)->y - newmy;
    } else {
      newmy = newmy - (*lgman)->y;
    }
    if (newmx < dist && newmy < dist) {
      returnValue = TRUE;
    }
  }
  return returnValue;
}

/*********************************************************
*NAME:          lgmPutWorld
*AUTHOR:        John Morrison
*CREATION DATE: 23/9/00
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm map X position
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  x      - World X Co-ordinate
*  y      - World Y Co-ordinate
*********************************************************/
void lgmPutWorld(lgm *lgmman, WORLD x, WORLD y, BYTE frame) {
  if ((*lgmman)->inTank == FALSE) {
    (*lgmman)->x = x;
    (*lgmman)->y = y;
    /* We don't set the frame from the server because its all handled locally */
  /*  (*lgmman)->frame = frame; */
/*    (*lgmman)->inTank = FALSE; */
    (*lgmman)->state = LGM_STATE_GOING;
  }
}

/*********************************************************
*NAME:          lgmGetMX
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm map X position
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*********************************************************/
BYTE lgmGetMX(lgm *lgman) {
  BYTE returnValue; /* Value to return */
  WORLD conv;       /* Used in the conversion */

  if ((*lgman)->state == LGM_STATE_IDLE) {
    returnValue = 0;
  } else {
    if ((*lgman)->frame == LGM_HELICOPTER_FRAME) {
      conv = (*lgman)->x - MAP_SQUARE_MIDDLE;
    } else {
      conv = (*lgman)->x - 1;
    }
    conv >>= TANK_SHIFT_MAPSIZE;
    returnValue = (BYTE) conv;
  }
  return returnValue;
}

/*********************************************************
*NAME:          lgmGetMY
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm map Y position
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*********************************************************/
BYTE lgmGetMY(lgm *lgman) {
  BYTE returnValue; /* Value to return */
  WORLD conv;       /* Used in the conversion */

  if ((*lgman)->state == LGM_STATE_IDLE) {
    returnValue = 0;
  } else {
    if ((*lgman)->frame == LGM_HELICOPTER_FRAME) {
      conv = (*lgman)->y - MAP_SQUARE_MIDDLE;
    } else {
      conv = (*lgman)->y - 2;
    }
    conv >>= TANK_SHIFT_MAPSIZE;
    returnValue = (BYTE) conv;
  }

  return returnValue;
}

/*********************************************************
*NAME:          lgmGetPX
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm pixel X position
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*********************************************************/
BYTE lgmGetPX(lgm *lgman) {
  BYTE returnValue; /* Value to return */
  WORLD conv;       /* Used in the conversion */

  if ((*lgman)->state == LGM_STATE_IDLE) {
    returnValue = 0;
  } else {
    if ((*lgman)->frame == LGM_HELICOPTER_FRAME) {
      conv = (*lgman)->x - MAP_SQUARE_MIDDLE;
    } else {
      conv = (*lgman)->x - 1;
    }
    conv <<= TANK_SHIFT_MAPSIZE;
    conv >>= TANK_SHIFT_PIXELSIZE;
    returnValue = (BYTE) conv;

  }
  return returnValue;
}

/*********************************************************
*NAME:          lgmGetPY
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm pixel Y position
*
*ARGUMENTS:
*
*********************************************************/
BYTE lgmGetPY(lgm *lgman) {
  BYTE returnValue; /* Value to return */
  WORLD conv;       /* Used in the conversion */

  if ((*lgman)->state == LGM_STATE_IDLE) {
    returnValue = 0;
  } else {
    if ((*lgman)->frame == LGM_HELICOPTER_FRAME) {
      conv = (*lgman)->y - MAP_SQUARE_MIDDLE;
    } else {
      conv = (*lgman)->y - 2;
    }
    conv <<= TANK_SHIFT_MAPSIZE;
    conv >>= TANK_SHIFT_PIXELSIZE;
    returnValue = (BYTE) conv;
  }
  return returnValue;
}

/*********************************************************
*NAME:          lgmGetFrame
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns the lgm animation frame
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*********************************************************/
BYTE lgmGetFrame(lgm *lgman) {
  return (*lgman)->frame;
}

bool lgmIsOut(lgm *lgman) {
  return !((*lgman)->inTank); 
}

/*********************************************************
*NAME:          lgmGetStatus
*AUTHOR:        John Morrison
*CREATION DATE: 14/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Gets the man status for drawing on the status bars
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  tnk    - Pointer to this LGM's tank.
*  isOut  - TRUE if man is out of tank
*  isDead - TRUE if man is dead
*  angle  - Angle man is travelling on
*********************************************************/
void lgmGetStatus(lgm *lgman, tank *tnk, bool *isOut, bool *isDead, TURNTYPE *angle) {
  WORLD wx; /* Tanks world co-ordinates */
  WORLD wy;

  *isOut = TRUE;
  *isDead = FALSE;
  if ((*lgman)->inTank == TRUE) {
    *isOut = FALSE;
  } else if ((*lgman)->isDead == TRUE) {
    *isDead = TRUE;
  } else if ((*lgman)->state == LGM_STATE_GOING) {
    tankGetWorld(tnk, &wx, &wy);
    *angle = utilCalcAngle((*lgman)->x, (*lgman)->y, wx, wy);
  } else {
    /* Returning to tank */
     tankGetWorld(tnk, &wx, &wy);
     *angle = utilCalcAngle((*lgman)->x, (*lgman)->y, wx, wy);
  }
}

/*********************************************************
*NAME:          lgmGetWX
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns lgm World X co-ord
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*********************************************************/
WORLD lgmGetWX(lgm *lgman) {
  return (*lgman)->x;
}

/*********************************************************
*NAME:          lgmGetWY
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns lgm World Y co-ord
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*********************************************************/
WORLD lgmGetWY(lgm *lgman) {
  return (*lgman)->y;
}

/*********************************************************
*NAME:          lgmGetDir
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns lgm direction
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  tnk - Pointer to the tank object
*********************************************************/
BYTE lgmGetDir(lgm *lgman, tank *tnk) {
  TURNTYPE angle;
  if ((*lgman)->state == LGM_STATE_GOING) {
    angle = utilCalcAngle((*lgman)->x, (*lgman)->y, (*lgman)->destX, (*lgman)->destY);
  } else {
    angle = utilCalcAngle((*lgman)->x, (*lgman)->y, tankGetMX(tnk), tankGetMY(tnk));
  }
  return utilGetDir(angle);
}

/*********************************************************
*NAME:          lgmGetBrainState
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns lgm's state required for brain.
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*********************************************************/
BYTE lgmGetBrainState(lgm *lgman) {
  BYTE returnValue; /* Value to return */

  returnValue = LGM_BRAIN_MOVING;
  if ((*lgman)->isDead == TRUE) {
    returnValue = LGM_BRAIN_DEAD;
  } else if ((*lgman)->inTank == TRUE) {
    returnValue = LGM_BRAIN_INTANK;
  }
  return returnValue;
}

/*********************************************************
*NAME:          lgmGetBrainObstructed
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 23/9/00
*PURPOSE:
*  Returns lgm's obstructed state required for brain
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*********************************************************/
BYTE lgmGetBrainObstructed(lgm *lgman) {
  return (*lgman)->obstructed;
}

/*********************************************************
*NAME:          lgmSetBrainObstructed
*AUTHOR:        John Morrison
*CREATION DATE: 16/4/01
*LAST MODIFIED: 16/4/01
*PURPOSE:
*  Stores lgm's obstructed state required for brain
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  obstructed - The obstructed state to set
*********************************************************/
void lgmSetBrainObstructed(lgm *lgman, BYTE obstructed) {
  (*lgman)->obstructed = obstructed;
}

/*********************************************************
*NAME:          lgmNetBackInTank
*AUTHOR:        John Morrison
*CREATION DATE: 2/12/00
*LAST MODIFIED: 2/12/00
*PURPOSE:
*  Network message. LGM Back in tank
*
*ARGUMENTS:
*  lgman    - Pointer to the lgm sturcture
*  mp       - Pointer to the map structure
*  pb       - Pointer to the pillbox structure
*  bs       - Pointer to the base structure
*  tnk      - Pointer to the tank structure
*  numTrees - Amount of trees lgm is carrying
*  numMines - Amount of mines the lgm is carrying
*  pillNum  - Pillbox being carries
*********************************************************/
void lgmNetBackInTank(lgm *lgman, map *mp, pillboxes *pb, bases *bs, tank *tnk, BYTE numTrees, BYTE numMines, BYTE pillNum) {
  (*lgman)->isDead = FALSE;
  (*lgman)->state = LGM_STATE_IDLE;
  (*lgman)->inTank = TRUE;
  (*lgman)->action = LGM_IDLE;
  (*lgman)->blessX = 0;
  (*lgman)->blessY = 0;
  (*lgman)->onTop = 0;
  (*lgman)->numPills = pillNum;
  (*lgman)->numTrees = numTrees;
  (*lgman)->numMines = numMines;
  lgmBackInTank(lgman, mp, pb, bs, tnk, TRUE);

  if (threadsGetContext() == FALSE) {
    frontEndManClear();
  }
}

/*********************************************************
*NAME:          lgmNetManWorking
*AUTHOR:        John Morrison
*CREATION DATE: 01/02/03
*LAST MODIFIED: 01/02/03
*PURPOSE:
*  Network message. LGM out doing work 
*
*ARGUMENTS:
*  lgman    - Pointer to the lgm sturcture
*  tnk      - Pointer to the tank structure
*  mapX     - Bless X map position 
*  mapY     - Bless Y map position 
*  numTrees - Amount of trees lgm is carrying
*  numMines - Amount of mines the lgm is carrying
*  pillNum  - Pillbox being carries
*********************************************************/
void lgmNetManWorking(lgm *lgman, tank *tnk, BYTE mapX, BYTE mapY, BYTE numTrees, BYTE numMines, BYTE pillNum) {
  (*lgman)->isDead = FALSE;
  (*lgman)->state = LGM_STATE_GOING;
  (*lgman)->inTank = FALSE;
  (*lgman)->blessX = mapX;
  (*lgman)->blessY = mapY;
  (*lgman)->numPills = pillNum;
  (*lgman)->numTrees = numTrees;
  (*lgman)->numMines = numMines;
  /* Update the statuses */
  if (numTrees > 0) {
    tankGetLgmTrees(tnk, numTrees, TRUE);
  }
  if (numMines > 0) {
    tankGetLgmMines(tnk, numMines, TRUE);
  }
  if (pillNum != LGM_NO_PILL) {
    tankGetCarriedPillNum(tnk, pillNum);
  }
  tankGetWorld(tnk, &(*lgman)->x, &(*lgman)->y);   
}

/*********************************************************
*NAME:          lgmSetPlayerNum
*AUTHOR:        John Morrison
*CREATION DATE: 3/12/00
*LAST MODIFIED: 3/12/00
*PURPOSE:
*  Sets the player number
*
*ARGUMENTS:
*  lgman     - Pointer to the lgm sturcture
*  playerNum - Player number to set to
*********************************************************/
void lgmSetPlayerNum(lgm *lgman, BYTE playerNum) {
  if ((*lgman) != NULL) {
    (*lgman)->playerNum = playerNum;
  }
}

/*********************************************************
*NAME:          lgmSetIsDead
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/00
*LAST MODIFIED: 28/12/00
*PURPOSE:
*  Sets if the lgm is dead of not
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  isDead - Flag whether the LGM is dead or not
*********************************************************/
void lgmSetIsDead(lgm *lgman, bool isDead) {
  (*lgman)->isDead = isDead;
  (*lgman)->nextAction = LGM_IDLE;
  if (isDead == TRUE) {
    (*lgman)->x = 0;
    (*lgman)->y = 0;
    if (threadsGetContext() == FALSE) {
      frontEndManStatus(TRUE, 0.0f);
    }
  } else {
    if (threadsGetContext() == FALSE) {
      frontEndManClear();
    }
  }
}

/*********************************************************
*NAME:          lgmConnectionLost
*AUTHOR:        John Morrison
*CREATION DATE: 24/02/03
*LAST MODIFIED: 24/02/03
*PURPOSE:
* Called if our connection is lost from the server 
*
*ARGUMENTS:
*  lgman  - Pointer to the lgm sturcture
*  tnk    - Pointer to our tank structure
*********************************************************/
void lgmConnectionLost(lgm *lgman, tank *tnk) {
  BYTE lgmX;  /* Used to get start position */
  BYTE lgmY;
  TURNTYPE dummy;
  if ((*lgman)->isDead == TRUE) {
    screenGetTankWorldFromLgm(lgman, &((*lgman)->destX), &((*lgman)->destY));
    if ((*lgman)->x == 0 || (*lgman)->y == 0) {
      screenGetRandStart(&lgmX, &lgmY, &dummy);
      (*lgman)->x = (lgmX << TANK_SHIFT_MAPSIZE) + MAP_SQUARE_MIDDLE;
      (*lgman)->y = (lgmY << TANK_SHIFT_MAPSIZE) + MAP_SQUARE_MIDDLE;
    }
  } else if ((*lgman)->state != LGM_STATE_IDLE) {
    (*lgman)->state = LGM_STATE_RETURN;
  }

}

