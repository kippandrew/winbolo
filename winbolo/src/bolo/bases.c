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
*Name:          Bases 
*Filename:      bases.c 
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 24/07/04
*Purpose:
*  Provides operations on bases 
*********************************************************/

/* Includes */
#include <math.h>
#include <memory.h>
#include "global.h"
#include "tank.h"
#include "frontend.h"
#include "messages.h"
#include "players.h"
#include "netpnb.h"
#include "netmt.h"
#include "network.h"
#include "screen.h"
#include "log.h"
#include "../winbolonet/winbolonet.h"
#include "bases.h"

int baseTimer[MAX_TANKS];

void basesUpdateTimer(int playerNumber){
	baseTimer[playerNumber]=BASE_TICKS_BETWEEN_REFUEL;
}


void basesRemoveTimer(int playerNumber){
	baseTimer[playerNumber]=30000; // the 30000 is a arbitrary large number
}
/*********************************************************
*NAME:         basesCreate 
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Creates and initilises the bases structure.
*  Sets number of bases to zero
*
*ARGUMENTS:
*  value - Pointer to the bases structure 
*********************************************************/
void basesCreate(bases *value) {
  BYTE count; /* Looping variable */

  New(*value);
  memset(*value, 0, sizeof(**value));
  (*value)->numBases = 0;

  for (count=0;count<MAX_BASES;count++) {
    (*value)->item[count].baseTime = 0;
    (*value)->item[count].justStopped = TRUE;
  }
  for (count=0;count<MAX_TANKS;count++){
	  baseTimer[count]=30000;
  }
  if(netGetType() == netSingle)
  {
	baseTimer[0]=BASE_TICKS_BETWEEN_REFUEL;
  }
}

/*********************************************************
*NAME:          basesDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Destroys the bases data structure. Also frees memory.
*
*ARGUMENTS:
*  value - Pointer to the bases structure
*********************************************************/
void basesDestroy(bases *value) {
  if (*value != NULL) {
    Dispose(*value);
  }
}

/*********************************************************
*NAME:          basesSetNumBases
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets the number of bases in the structure 
*
*ARGUMENTS:
*  value     - Pointer to the bases structure
*  numBases - The number of bases  
*********************************************************/
void basesSetNumBases(bases *value, BYTE numBases) {
  if (numBases <= MAX_BASES) {
    (*value)->numBases = numBases;
  }
}


/*********************************************************
*NAME:          basesGetNumBases 
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Gets the number of bases in the structure
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*********************************************************/
BYTE basesGetNumBases(bases *value) {
  if ((*value) != NULL) {
    return (*value)->numBases;
  }
  return 0;
}

/*********************************************************
*NAME:          basesSetBase
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED:  29/4/00
*PURPOSE:
*  Sets a specific base with its item data
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  item    - Pointer to a base item 
*  baseNum - The base number
*********************************************************/
void basesSetBase(bases *value, base *item, BYTE baseNum) {
  if (baseNum > 0 && baseNum <= (*value)->numBases) {
    baseNum--;
    (((*value)->item[baseNum]).x) = item->x;
    (((*value)->item[baseNum]).y) = item->y;
    if (item->owner > (MAX_TANKS-1) && item->owner != NEUTRAL) {
      item->owner = NEUTRAL;
    }
    logAddEvent(log_BaseSetOwner, baseNum, item->owner, TRUE, 0, 0, NULL);
    (((*value)->item[baseNum]).owner) = item->owner;
    (((*value)->item[baseNum]).armour) = item->armour;
    (((*value)->item[baseNum]).shells) = item->shells;
    (((*value)->item[baseNum]).mines) = item->mines;
    (*value)->item[baseNum].refuelTime = 0;
    (*value)->item[baseNum].baseTime = item->baseTime;
    (*value)->item[baseNum].justStopped = TRUE;
    logAddEvent(log_BaseSetStock, baseNum, item->shells, item->mines, item->armour, 0, NULL);
  }
}

/*********************************************************
*NAME:          basesGetBase
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/98
*LAST MODIFIED: 9/2/98
*PURPOSE:
*  Gets a specific base
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  item    - Pointer to a base item 
*  baseNum - The base number
*********************************************************/
void basesGetBase(bases *value, base *item, BYTE baseNum) {
  if (baseNum > 0 && baseNum <= (*value)->numBases) {
    baseNum--;
    item->x = ((*value)->item[baseNum]).x;
    item->y = ((*value)->item[baseNum]).y;
    item->owner = ((*value)->item[baseNum]).owner;
    item->armour = ((*value)->item[baseNum]).armour;
    item->shells = ((*value)->item[baseNum]).shells;
    item->mines = ((*value)->item[baseNum]).mines;
  }
}

/*********************************************************
*NAME:          basesExistPos
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Returns whether a base exist at a specific location
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
bool basesExistPos(bases *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping Variable */

  returnValue = FALSE;
  count = 0;
  while (returnValue == FALSE && count < ((*value)->numBases)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
      returnValue = TRUE;
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesGetAlliancePos
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Returns the type of a base (good, neutral, evil) 
*  depending on the map position. Returns base neutral if
*  base not found.
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
baseAlliance basesGetAlliancePos(bases *value, BYTE xValue, BYTE yValue) {
  baseAlliance returnValue; /* Value to return */
  bool done;                /* Finished looping */
  BYTE count;               /* Looping Variable */

  returnValue = baseNeutral;
  count = 0;
  done = FALSE;
  while (done == FALSE && count < ((*value)->numBases)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
     if ((*value)->item[count].armour <= MIN_ARMOUR_CAPTURE) {
        returnValue = baseDead;
      } else if ((*value)->item[count].owner == NEUTRAL) {
        returnValue = baseNeutral;
      } else if ((*value)->item[count].owner == playersGetSelf(screenGetPlayers())) {
        returnValue = baseOwnGood;
      } else if (playersIsAllie(screenGetPlayers(), (*value)->item[count].owner, playersGetSelf(screenGetPlayers())) == TRUE) {
        returnValue = baseAllieGood;
      } else {
        returnValue = baseEvil;
      }
      done = TRUE;
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesGetStatusNum
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 11/1/99
*PURPOSE:
*  Returns the type of a base (good, neutral, dead, evil) 
*  depending on the baseNum. Returns base neutral if
*  base not found.
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
baseAlliance basesGetStatusNum(bases *value, BYTE baseNum) {
  baseAlliance returnValue; /* Value to return */

  baseNum--;
  returnValue = baseNeutral;
  if (baseNum <= ((*value)->numBases)) {
    if ((*value)->item[baseNum].armour <= MIN_ARMOUR_CAPTURE) {
      returnValue = baseDead;
    } else if ((*value)->item[baseNum].owner == NEUTRAL) {
      returnValue = baseNeutral;
    } else if ((*value)->item[baseNum].owner == playersGetSelf(screenGetPlayers())) {
      returnValue = baseOwnGood;
    } else if (playersIsAllie(screenGetPlayers(), (*value)->item[baseNum].owner, playersGetSelf(screenGetPlayers())) == TRUE) {
      returnValue = baseAllieGood;
    } else {
      returnValue = baseEvil;
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 10/1/99
*LAST MODIFIED: 4/1/00
*PURPOSE:
*  Called once every Bolo tick to update stocks, give
*  stuff to tanks etc.
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  tnk    - Pointer to the tank structure
*********************************************************/
void basesUpdate(bases *value, tank *tnk) {
  WORLD twx;               /* Tank World Co-ordinates */
  WORLD twy;
  BYTE tx;                 /* Tank Map X and Y Co-ordinates */
  BYTE ty;
  BYTE baseNum;            /* The base number if the tank is on a base */
  BYTE tankArmour;         /* Amount of health the tank has */
  BYTE count;              /* Looping Variable */
  int secondCounter;       /* another looping variable */
  bool isServer;           /* Are we the server */
  double numPlayers;         /* Number of players */
  double maxTime;

  count = 0;
  secondCounter = 0;
  
  isServer = threadsGetContext();
  numPlayers = playersGetNumPlayers(screenGetPlayers());
  /* Old Algorithm */
/*  maxTime = (800.0 / numPlayers);
  while (count < (*value)->numBases) {
    (*value)->item[count].baseTime++;
    if ((*value)->item[count].baseTime >= maxTime) {
      if (isServer == TRUE || netGetType() == netSingle) {
        basesUpdateStock(value, (BYTE) (count+1));
      }
      (*value)->item[count].baseTime = 0;
    }
    if ((*value)->item[count].refuelTime > 0) {
      (*value)->item[count].refuelTime--;
    }
    count++;
  }
  */

  while (secondCounter < MAX_TANKS)
  {
	  if(baseTimer[secondCounter] != 30000)
	  {
		  baseTimer[secondCounter]--;
		  if(baseTimer[secondCounter]<=0)
		  {
			if (isServer == TRUE || netGetType() == netSingle) 
			{
				while (count < (*value)->numBases) 
				{
					basesUpdateStock(value, (BYTE) (count+1));
					count++;
				}
			}
			baseTimer[secondCounter]=BASE_TICKS_BETWEEN_REFUEL;
			//printf("ticks refuel %d \r\n",winboloTimer());
		  }
	  }
	
	secondCounter++;
  }

  count = 0;

  while (count < (*value)->numBases) 
  {
	  if ((*value)->item[count].refuelTime > 0) {
	      (*value)->item[count].refuelTime--;
	  }
	  count++;
  }

  if (isServer == FALSE) {
    /* Client - Check for refuelling */
    /* Get tanks location */
    tankGetWorld(tnk, &twx, &twy);
    twx >>= TANK_SHIFT_MAPSIZE;
    tx = (BYTE) twx;
    twy >>= TANK_SHIFT_MAPSIZE;
    ty = (BYTE) twy;
    tankArmour = tankGetArmour(tnk);
    baseNum = basesGetBaseNum(value,tx,ty);
    if (baseNum != BASE_NOT_FOUND && tankArmour <= TANK_FULL_ARMOUR) {
      /* On base */
      if ((*value)->item[baseNum-1].justStopped == FALSE) {
        basesRefueling(value, tnk, baseNum);
      } else {
        (*value)->item[baseNum-1].justStopped = FALSE;
        (*value)->item[baseNum-1].refuelTime = BASE_REFUEL_ARMOUR;
      }
    }
  } else {
    baseNum = BASE_NOT_FOUND;
  }

  count = 0;

  /* Set them back to empty  */
  while (count < (*value)->numBases) {
    if (count != (baseNum-1)) {
      (*value)->item[count].justStopped = TRUE;
    }
    count++;
  }

}

/*********************************************************
*NAME:          basesUpdateStock
*AUTHOR:        John Morrison
*CREATION DATE: 10/1/99
*LAST MODIFIED: 19/2/99
*PURPOSE:
*  Called when a stock update is needed
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  baseNum - The base to update
*********************************************************/
void basesUpdateStock(bases *value, BYTE baseNum) {
  BYTE oldArmour;          /* Used for checking base armour for updates */
  BYTE addAmount;

  baseNum--;
  addAmount = 1; /* (BYTE) playersGetNumPlayers(); - 1.09 was but halved rechar time (2*playersGetNumPlayers()); * FIXME: Constant rate */
  if (baseNum < (*value)->numBases) {
    if ((*value)->item[baseNum].armour < BASE_FULL_ARMOUR) {
      oldArmour = (*value)->item[baseNum].armour;
      (*value)->item[baseNum].armour += addAmount;
      if ((*value)->item[baseNum].armour > BASE_FULL_ARMOUR) {
        (*value)->item[baseNum].armour = BASE_FULL_ARMOUR;
      }
      /* Update the frontend status as required */
      if (oldArmour == BASE_DEAD && (*value)->item[baseNum].armour > BASE_DEAD) { /* FIXME: Changed to hardcoded value */
        if (threadsGetContext() == FALSE) {
          frontEndStatusBase((BYTE) (baseNum+1), (basesGetStatusNum(value, (BYTE) (baseNum+1))));
        }
      }
    }
    if ((*value)->item[baseNum].shells < BASE_FULL_SHELLS) {
      (*value)->item[baseNum].shells += addAmount;
      if ((*value)->item[baseNum].shells > BASE_FULL_SHELLS) {
        (*value)->item[baseNum].shells = BASE_FULL_SHELLS;
      }
    }

    if ((*value)->item[baseNum].mines < BASE_FULL_MINES) {
      (*value)->item[baseNum].mines += addAmount;
      if ((*value)->item[baseNum].mines > BASE_FULL_MINES) {
        (*value)->item[baseNum].mines = BASE_FULL_MINES;
      }
    }
    logAddEvent(log_BaseSetStock, baseNum, (*value)->item[baseNum].shells, (*value)->item[baseNum].mines, (*value)->item[baseNum].armour, 0, NULL);
  }
  if (addAmount > 0) {
    netMNTAdd(screenGetNetMnt(), NMNT_BASERELOAD, baseNum, addAmount, 0, 0);
  }
}

/*********************************************************
*NAME:          basesAmOwner
*AUTHOR:        John Morrison
*CREATION DATE: 10/1/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
*  Returns whether the bases at the particular location
*  is owned by the player
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  owner  - Owner to check with
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
bool basesAmOwner(bases *value, BYTE owner, BYTE xValue, BYTE yValue) {
  bool returnValue;         /* Value to return */
  BYTE self;                /* Our player number */
  bool done;                /* Finished looping */
  BYTE count;               /* Looping Variable */

  returnValue = FALSE;
  count = 0;
  done = FALSE;
  /* FIXME: This is redundent. */
  self = owner;
  while (done == FALSE && count < ((*value)->numBases)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
      if ((*value)->item[count].owner == self || (playersIsAllie(screenGetPlayers(), (*value)->item[count].owner, self) == TRUE)) {
        returnValue = TRUE;
      }
      done = TRUE;
    }
    count++;
  }

  return returnValue;
}


/*********************************************************
*NAME:          basesSetBaseOwner
*AUTHOR:        John Morrison
*CREATION DATE: 10/01/99
*LAST MODIFIED: 04/04/02
*PURPOSE:
* Sets the base to be owned by paremeter passed.
* Returns the previous owner. If it was not neutral we
* assume then it was "stolen" and subsequently remove
* all its possessions. If migrate is set to TRUE then
* it has migrated from a alliance when a player left 
* and we shouldn't make a message
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  baseNum - Base number to set
*  owner   - Who owns it
*  migrate - TRUE if it has migrated from an alliance
*********************************************************/
BYTE basesSetBaseOwner(bases *value, BYTE baseNum, BYTE owner, BYTE migrate) {
  BYTE returnValue;         /* Value to return */
  /* Message stuff */
  char oldOwner[FILENAME_MAX];
  char messageStr[FILENAME_MAX];

  oldOwner[0] = '\0';
  messageStr[0] = '\0';

  returnValue = FALSE;
  if (baseNum > 0 && baseNum <= (*value)->numBases) {
    baseNum--;
    returnValue = (*value)->item[baseNum].owner;
    if (migrate == TRUE) {
      (*value)->item[baseNum].owner = owner;
    } else if (owner == NEUTRAL) {
      (*value)->item[baseNum].owner = owner;
    } else if ((*value)->item[baseNum].owner != owner) {
      playersMakeMessageName(screenGetPlayers(), owner, messageStr);
      if (returnValue != NEUTRAL) {
        (*value)->item[baseNum].armour = 0;
        (*value)->item[baseNum].shells = 0;
        (*value)->item[baseNum].mines = 0;
        (*value)->item[baseNum].baseTime = 0;
        strcat(messageStr, langGetText(MESSAGE_STOLE_BASE));
        playersGetPlayerName(screenGetPlayers(), returnValue, oldOwner);
        strcat(messageStr, oldOwner);
        messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), messageStr);
      } else {
        /* Neutral */
        strcat(messageStr, langGetText(MESSAGE_CAPTURE_BASE));
        messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), messageStr);
      }
      (*value)->item[baseNum].owner = owner;
    }
    logAddEvent(log_BaseSetOwner, baseNum, owner, migrate, 0, 0, NULL);

    /* WinBolo.net Stuff */
    if (migrate == FALSE && owner != NEUTRAL) {
      winbolonetAddEvent(WINBOLO_NET_EVENT_BASE_CAPTURE, threadsGetContext(), owner, WINBOLO_NET_NO_PLAYER);
    }
  
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesSetOwner
*AUTHOR:        John Morrison
*CREATION DATE: 10/01/99
*LAST MODIFIED: 04/04/02
*PURPOSE:
* Sets the base to be owned by paremeter passed.
* Returns the previous owner. If it was not neutral we
* assume then it was "stolen" and subsequently remove
* all its possessions. If migrate is set to TRUE then
* it has migrated from a alliance when a player left 
* and we shouldn't make a message
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  xValue  - X Location
*  yValue  - Y Location
*  owner   - Who owns it
*  migrate - TRUE if it has migrated from an alliance
*********************************************************/
BYTE basesSetOwner(bases *value, BYTE xValue, BYTE yValue, BYTE owner, BYTE migrate) {
  BYTE returnValue;         /* Value to return */
  bool done;                /* Finished looping */
  BYTE count;               /* Looping Variable */
  /* Message stuff */
  char oldOwner[FILENAME_MAX];
  char messageStr[FILENAME_MAX];

  oldOwner[0] = '\0';
  messageStr[0] = '\0';

  returnValue = FALSE;
  count = 0;
  done = FALSE;
  while (done == FALSE && count < ((*value)->numBases)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
      returnValue = (*value)->item[count].owner;
      if (migrate == TRUE) {
        (*value)->item[count].owner = owner;
        done = TRUE;
      } else if (owner == NEUTRAL) {
        (*value)->item[count].owner = owner;
        done = TRUE;
      } else if ((*value)->item[count].owner != owner) {
        playersMakeMessageName(screenGetPlayers(), owner, messageStr);
        if (returnValue != NEUTRAL) {
          (*value)->item[count].armour = 0;
          (*value)->item[count].shells = 0;
          (*value)->item[count].mines = 0;
          (*value)->item[count].baseTime = 0;
          strcat(messageStr, langGetText(MESSAGE_STOLE_BASE));
          playersGetPlayerName(screenGetPlayers(), returnValue, oldOwner);
          strcat(messageStr, oldOwner);
          messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), messageStr);
        } else {
          /* Neutral */
          strcat(messageStr, langGetText(MESSAGE_CAPTURE_BASE));
          messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), messageStr);
        }
        (*value)->item[count].owner = owner;
        logAddEvent(log_BaseSetOwner, count, owner, migrate, 0, 0, NULL);
        done = TRUE;
        /* WinBolo.net Stuff */
        if (migrate == FALSE && owner != NEUTRAL) {
          if (returnValue == NEUTRAL) {
            winbolonetAddEvent(WINBOLO_NET_EVENT_BASE_CAPTURE, threadsGetContext(), owner, WINBOLO_NET_NO_PLAYER);
          } else {
            winbolonetAddEvent(WINBOLO_NET_EVENT_BASE_STEAL, threadsGetContext(), owner, returnValue);
          }
        }

      }
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesGetBaseNum
*AUTHOR:        John Morrison
*CREATION DATE: 10/1/99
*LAST MODIFIED: 10/1/99
*PURPOSE:
* Returns the bases number.
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
BYTE basesGetBaseNum(bases *value, BYTE xValue, BYTE yValue) {
  BYTE returnValue;         /* Value to return */
  bool done;                /* Finished looping */
  BYTE count;               /* Looping Variable */

  returnValue = BASE_NOT_FOUND-1;
  count = 0;
  done = FALSE;
  while (done == FALSE && count < ((*value)->numBases)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
      returnValue = count;
      done = TRUE;
    }
    count++;
  }

  return (returnValue+1);
}

/*********************************************************
*NAME:          basesRefueling
*AUTHOR:        John Morrison
*CREATION DATE: 10/1/99
*LAST MODIFIED: 22/12/99
*PURPOSE:
* The baseNum is being occupied by a tank. Refuel it
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
void basesRefueling(bases *value, tank *tnk, BYTE baseNum) {
  BYTE shellsAmount;  /* Tank Shells, mines and armour */
  BYTE mines;
  BYTE armour;
  BYTE trees;

  baseNum--;

  if ((*value)->item[baseNum].refuelTime == 0) {
    tankGetStats(tnk, &shellsAmount, &mines, &armour, &trees);
    if (playersIsAllie(screenGetPlayers(), (*value)->item[baseNum].owner, screenGetTankPlayer(tnk))) {
      if (armour < TANK_FULL_ARMOUR && ((*value)->item[baseNum].armour - BASE_ARMOUR_GIVE) >= BASE_MIN_ARMOUR) {
        (*value)->item[baseNum].armour -= BASE_ARMOUR_GIVE;
        tankAddArmour(tnk, BASE_ARMOUR_GIVE);
        (*value)->item[baseNum].refuelTime = BASE_REFUEL_ARMOUR;
        netPNBAdd(screenGetNetPnb(), NPNB_BASE_REFUEL_ARMOUR, baseNum, playersGetSelf(screenGetPlayers()), 0, 0);
        if (threadsGetContext() == FALSE) {
          frontEndUpdateBaseStatusBars(((*value)->item[baseNum].shells), ((*value)->item[baseNum].mines), ((*value)->item[baseNum].armour));
        }
      } else if (shellsAmount < TANK_FULL_SHELLS && ((*value)->item[baseNum].shells - BASE_SHELLS_GIVE) >= BASE_MIN_SHELLS) {
        (*value)->item[baseNum].shells -= BASE_SHELLS_GIVE;
        tankAddShells(tnk, BASE_SHELLS_GIVE);
        netPNBAdd(screenGetNetPnb(), NPNB_BASE_REFUEL_SHELLS, baseNum, playersGetSelf(screenGetPlayers()), 0, 0);
        (*value)->item[baseNum].refuelTime = BASE_REFUEL_SHELLS;
        if (threadsGetContext() == FALSE) {
          frontEndUpdateBaseStatusBars(((*value)->item[baseNum].shells), ((*value)->item[baseNum].mines), ((*value)->item[baseNum].armour));
        }
      } else if (mines < TANK_FULL_MINES && ((*value)->item[baseNum].mines - BASE_MINES_GIVE) >= BASE_MIN_MINES) {
        (*value)->item[baseNum].mines -= BASE_MINES_GIVE;
        tankAddMines(tnk, BASE_MINES_GIVE);
        (*value)->item[baseNum].refuelTime = BASE_REFUEL_MINES;
        netPNBAdd(screenGetNetPnb(), NPNB_BASE_REFUEL_MINES, baseNum, playersGetSelf(screenGetPlayers()), 0, 0);
        if (threadsGetContext() == FALSE) {
          frontEndUpdateBaseStatusBars(((*value)->item[baseNum].shells), ((*value)->item[baseNum].mines), ((*value)->item[baseNum].armour));
        }
      }
      logAddEvent(log_BaseSetStock, baseNum, (*value)->item[baseNum].shells, (*value)->item[baseNum].mines, (*value)->item[baseNum].armour, 0, NULL);
    }
  }
}

/*********************************************************
*NAME:          basesGetClosest
*AUTHOR:        John Morrison
*CREATION DATE: 11/1/99
*LAST MODIFIED: 27/5/00
*PURPOSE:
* Returns the base Number of the base closest and is 
* either neutral or allied to the tank and inside the 
* range. If no base is inside the range it returns 
* BASE_NOT_FOUND.
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Map Location of the tank
*  yValue - Y Map Location of the tank
*********************************************************/
BYTE basesGetClosest(bases *value, WORLD tankX, WORLD tankY) {
  bool returnValue; /* Value to return */
  WORLD x;
  WORLD y;
  WORLD gapX;       /* Gap from base to tank */
  WORLD gapY;
  double distance;
  double oldDistance;
  BYTE count; /* Looping Variable */
  BYTE self;  /* Yourselfs player number */

  oldDistance = BASE_STATUS_RANGE; /* The range of the gunsight is the distance for a base to be shown */
  returnValue = BASE_NOT_FOUND-1;
  count = 0;
  self = playersGetSelf(screenGetPlayers());

  while (count < (*value)->numBases) {
    /* Check for neutral or allied */
    if ((*value)->item[count].owner == NEUTRAL || (playersIsAllie(screenGetPlayers(), self, (*value)->item[count].owner) == TRUE)) {
      x = (*value)->item[count].x;
      y = (*value)->item[count].y;
      x <<= 8;
      x += MAP_SQUARE_MIDDLE;
      y <<= 8;
      y += MAP_SQUARE_MIDDLE;
      if (tankX - x < 0) {
        gapX = x - tankX;
      } else {
        gapX = tankX - x;
      }
      if (tankY - y < 0) {
        gapY = y - tankY;
      } else {
        gapY = tankY - y;
      }

      distance = sqrt((double) ((gapX * gapX) + (gapY * gapY)));
      if (distance >=0 && distance < oldDistance) {
        oldDistance = distance;
        returnValue = count;
      }
    }
    count++;
  }
  return (returnValue+1);
}

/*********************************************************
*NAME:          basesGetStats
*AUTHOR:        John Morrison
*CREATION DATE: 12/1/99
*LAST MODIFIED: 12/1/99
*PURPOSE:
* Gets the base statistics (armour, shells, mines) for
* a given base number
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  baseNum - The base number to get
*  shellsAmount  - Pointer to hold the shells amount
*  mines   - Pointer to hold the mines amount
*  armour  - Pointer to hold the armour amount
*********************************************************/
void basesGetStats(bases *value, BYTE baseNum, BYTE *shellsAmount, BYTE *mines, BYTE *armour) {
  if (baseNum <= (*value)->numBases) {
    baseNum--;
    *shellsAmount = (*value)->item[baseNum].shells;
    *mines = (*value)->item[baseNum].mines;
    *armour = (*value)->item[baseNum].armour;
  } else {
    *shellsAmount = 0;
    *mines = 0;
    *armour = 0;
  }
}

/*********************************************************
*NAME:          basesDamagePos
*AUTHOR:        John Morrison
*CREATION DATE: 16/2/99
*LAST MODIFIED: 28/2/99
*PURPOSE:
* This base has been hit. Do some damage to it.
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
void basesDamagePos(bases *value, BYTE xValue, BYTE yValue) {
  bool done;                /* Are we finished searching for the base */
  BYTE count;               /* Looping Variable */

  count = 0;
  done = FALSE;
  while (done == FALSE && count < ((*value)->numBases)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue && (*value)->item[count].armour > 0) { 
      (*value)->item[count].armour -= DAMAGE;
      if ((*value)->item[count].armour > BASE_FULL_ARMOUR) {
        (*value)->item[count].armour = 0;
      }
      if ((*value)->item[count].armour <= BASE_DISPLAY_X) {
        if (threadsGetContext() == FALSE) {
          frontEndStatusBase((BYTE) (count+1), baseDead);
        }
      }
      done = TRUE;
    }
    count++;
  }
}

/*********************************************************
*NAME:          basesCanHit
*AUTHOR:        John Morrison
*CREATION DATE: 16/2/99
*LAST MODIFIED: 16/2/99
*PURPOSE:
*  Returns whether the bases at the particular location
*  can be hit by a shell from a particular owner
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*  hitBy  - Person who fired the shell
*********************************************************/
bool basesCanHit(bases *value, BYTE xValue, BYTE yValue, BYTE hitBy) {
  bool returnValue;         /* Value to return */
  bool done;                /* Finished looping */
  BYTE count;               /* Looping Variable */

  returnValue = FALSE;
  if (hitBy != NEUTRAL) {
    count = 0;
    done = FALSE;
    while (done == FALSE && count < ((*value)->numBases)) {
      if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
        if ((playersIsAllie(screenGetPlayers(), ((*value)->item[count].owner), hitBy) == FALSE) && (*value)->item[count].owner != NEUTRAL && (*value)->item[count].armour > BASE_MIN_CAN_HIT) {
          returnValue = TRUE;
        }
        done = TRUE;
      }
      count++;
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesCantDrive
*AUTHOR:        Minhiriath
*CREATION DATE: 29/12/2008
*LAST MODIFIED: 29/12/2008
*PURPOSE:
*  Returns whether the bases at the particular location
*  can be driven over by an object owned by the player lgm, or tank.
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*  hitBy  - Person who fired the shell
*********************************************************/
bool basesCantDrive(bases *value, BYTE xValue, BYTE yValue, BYTE hitBy) {
  bool returnValue;         /* Value to return */
  bool done;                /* Finished looping */
  BYTE count;               /* Looping Variable */

  returnValue = FALSE;
  if (hitBy != NEUTRAL) {
    count = 0;
    done = FALSE;
    while (done == FALSE && count < ((*value)->numBases)) {
      if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
        if ((playersIsAllie(screenGetPlayers(), ((*value)->item[count].owner), hitBy) == FALSE) && (*value)->item[count].owner != NEUTRAL && (*value)->item[count].armour > MIN_ARMOUR_CAPTURE) {
          returnValue = TRUE;
        }
        done = TRUE;
      }
      count++;
    }
  }

  return returnValue;
}
/*********************************************************
*NAME:          basesGetBaseOwner
*AUTHOR:        John Morrison
*CREATION DATE: 16/2/99
*LAST MODIFIED: 16/2/99
*PURPOSE:
* Returns the owner of a base 
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  baseNum - The base number to check
*********************************************************/
BYTE basesGetBaseOwner(bases *value, BYTE baseNum) {
  BYTE returnValue;         /* Value to return */

  returnValue = BASE_NOT_FOUND;
  if (baseNum > 0 && baseNum <= (*value)->numBases) {
    baseNum--;
    returnValue = (*value)->item[baseNum].owner;
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesGetOwnerPos
*AUTHOR:        John Morrison
*CREATION DATE: 16/2/99
*LAST MODIFIED: 16/2/99
*PURPOSE:
* Returns the owner of a base at the position.
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
BYTE basesGetOwnerPos(bases *value, BYTE xValue, BYTE yValue) {
  BYTE returnValue;         /* Value to return */
  bool done;                /* Finished looping */
  BYTE count;               /* Looping Variable */

  returnValue = NEUTRAL;
  count = 0;
  done = FALSE;
  while (done == FALSE && count < ((*value)->numBases)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
      returnValue = (*value)->item[count].owner;
      done = TRUE;
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesGetNumNeutral
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Returns the number of neutral bases
*
*ARGUMENTS:
*  value - Pointer to the bases structure
*********************************************************/
BYTE basesGetNumNeutral(bases *value) {
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  
  returnValue = 0;
  for (count=0;count<(*value)->numBases;count++) {
    if ((*value)->item[count].owner == NEUTRAL) {
      returnValue++;
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesSetBaseNetData
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 27/2/99
*PURPOSE:
* Sets the base data to buff.
*
*ARGUMENTS:
*  value - Pointer to the bases structure
*  buff  - Buffer of data to set base structure to
*  len   - Length of the data
*********************************************************/
void basesSetBaseNetData(bases *value, BYTE *buff, int len)  {
  BYTE returnValue = 1;
  BYTE count = 0;
  unsigned short us;

  (*value)->numBases = buff[0];
  while (count < (*value)->numBases) {
    (*value)->item[count].x = buff[returnValue];
    returnValue++;
    (*value)->item[count].y = buff[returnValue];
    returnValue++;
    (*value)->item[count].owner = buff[returnValue];
    returnValue++;
    (*value)->item[count].armour = buff[returnValue];
    returnValue++;
    (*value)->item[count].shells = buff[returnValue];
    returnValue++;
    (*value)->item[count].mines = buff[returnValue];
    returnValue++;
    (*value)->item[count].refuelTime = buff[returnValue];
    returnValue++;
    us = buff[returnValue];
    us += (buff[returnValue+1] << 8);
    (*value)->item[count].baseTime = ntohs(us);
    returnValue += 2;
    (*value)->item[count].justStopped = buff[returnValue];
    returnValue++;
    count++;
  }
}


void basesSetBaseCompressData(bases *value, BYTE *buff, int dataLen) {
  memcpy(&(**value), buff, SIZEOF_BASES);
}

/*********************************************************
*NAME:          basesSetBaseNetData
*AUTHOR:        John Morrison
*CREATION DATE: 27/02/99
*LAST MODIFIED: 24/07/04
*PURPOSE:
* Gets a copy of the base data and copies it to buff.
* Returns the data length
*
*ARGUMENTS:
*  value - Pointer to the bases structure
*  buff  - Buffer to hold copy of bases data
*********************************************************/
BYTE basesGetBaseNetData(bases *value, BYTE *buff) {
  int returnValue = 1; /* Value to return - Data length  */
  BYTE count = 0;
  unsigned short us;

  buff[0] = (*value)->numBases;
  while (count < (*value)->numBases) {
    buff[returnValue] = (*value)->item[count].x;
    returnValue++;
    buff[returnValue] = (*value)->item[count].y;
    returnValue++;
    buff[returnValue] = (*value)->item[count].owner;
    returnValue++;
    buff[returnValue] = (*value)->item[count].armour;
    returnValue++;
    buff[returnValue] = (*value)->item[count].shells;
    returnValue++;
    buff[returnValue] = (*value)->item[count].mines;
    returnValue++;
    buff[returnValue] = (*value)->item[count].refuelTime;
    returnValue++;
    us = (*value)->item[count].baseTime;
    us = htons(us);
    buff[returnValue] = (BYTE) (us >> 8);
    returnValue++;
    buff[returnValue] = (BYTE) (us & 0xFF);
    returnValue++;
    buff[returnValue] = (*value)->item[count].justStopped;
    returnValue++;
    count++;
  }

  return returnValue;

  /* Old code *
	memcpy(buff, &(**value), SIZEOF_BASES);
  return SIZEOF_BASES; */
}

/*********************************************************
*NAME:          basesNetGiveArmour
*AUTHOR:        John Morrison
*CREATION DATE: 9/3/99
*LAST MODIFIED: 9/3/99
*PURPOSE:
* A network call has been made that some one is refueling
* armour from a base. Remove it and update the screen here
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  baseNum - Basenum it is happening to
*********************************************************/
void basesNetGiveArmour(bases *value, BYTE baseNum) {
  if (((*value)->item[baseNum].armour - BASE_ARMOUR_GIVE) >= BASE_MIN_ARMOUR) {
    (*value)->item[baseNum].armour -= BASE_ARMOUR_GIVE;
    (*value)->item[baseNum].refuelTime = BASE_REFUEL_ARMOUR;
    logAddEvent(log_BaseSetStock, baseNum, (*value)->item[baseNum].shells, (*value)->item[baseNum].mines, (*value)->item[baseNum].armour, 0, NULL);
  }
}

/*********************************************************
*NAME:          basesNetGiveShells
*AUTHOR:        John Morrison
*CREATION DATE: 9/3/99
*LAST MODIFIED: 9/3/99
*PURPOSE:
* A network call has been made that some one is refueling
* shells from a base. Remove it and update the screen here
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  baseNum - Basenum it is happening to
*********************************************************/
void basesNetGiveShells(bases *value, BYTE baseNum) {
  if (((*value)->item[baseNum].shells - BASE_SHELLS_GIVE) >= BASE_MIN_SHELLS) {
    (*value)->item[baseNum].shells -= BASE_SHELLS_GIVE;
    (*value)->item[baseNum].refuelTime = BASE_REFUEL_SHELLS;
    logAddEvent(log_BaseSetStock, baseNum, (*value)->item[baseNum].shells, (*value)->item[baseNum].mines, (*value)->item[baseNum].armour, 0, NULL);
  }
}

/*********************************************************
*NAME:          basesNetGiveMines
*AUTHOR:        John Morrison
*CREATION DATE: 9/3/99
*LAST MODIFIED: 9/3/99
*PURPOSE:
* A network call has been made that some one is refueling
* mines from a base. Remove it and update the screen here
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  baseNum - Basenum it is happening to
*********************************************************/
void basesNetGiveMines(bases *value, BYTE baseNum) {
  if (((*value)->item[baseNum].mines - BASE_MINES_GIVE) >= BASE_MIN_MINES) {
    (*value)->item[baseNum].mines -= BASE_MINES_GIVE;
    (*value)->item[baseNum].refuelTime = BASE_REFUEL_MINES;
    logAddEvent(log_BaseSetStock, baseNum, (*value)->item[baseNum].shells, (*value)->item[baseNum].mines, (*value)->item[baseNum].armour, 0, NULL);
  }
}

/*********************************************************
*NAME:          basesSetNeutralOwner
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
*  Changes all bases of owner owner back to neutral
*
*ARGUMENTS:
*  value - Pointer to the bases structure
*  owner - Owner to set back to neutral
*********************************************************/
void basesSetNeutralOwner(bases *value, BYTE owner) {
  BYTE count;       /* Looping Variable */

  count = 0;
  while (count < ((*value)->numBases)) {
    if (((*value)->item[count].owner) == owner) {
      (*value)->item[count].owner = NEUTRAL;
      logAddEvent(log_BaseSetOwner, count, NEUTRAL, FALSE, 0, 0, NULL);
      netPNBAdd(screenGetNetPnb(), NPNB_BASE_CAPTURE, count, NEUTRAL, (*value)->item[count].x, (*value)->item[count].y);
    }
    count++;
  }
}

/*********************************************************
*NAME:          basesMigrate
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
*  Causes all bases owned by owner  to migrate to a new 
* owner because its old owner left the game.
*
*ARGUMENTS:
*  value    - Pointer to the bases structure
*  oldOwner - Old Owner to remove
*  newOwner - Owner to replace with
*********************************************************/
void basesMigrate(bases *value, BYTE oldOwner, BYTE newOwner) {
  BYTE count;       /* Looping Variable */

  count = 0;
  while (count < ((*value)->numBases)) {
    if (((*value)->item[count].owner) == oldOwner) {
      (*value)->item[count].owner = newOwner;
      logAddEvent(log_BaseSetOwner, newOwner, NEUTRAL, FALSE, 0, 0, NULL);
      netMNTAdd(screenGetNetMnt(), NMNT_BASEMIGRATE, count, newOwner, (*value)->item[count].x, (*value)->item[count].y);
    }
    count++;
  }
}

/*********************************************************
*NAME:          basesServerRefuel
*AUTHOR:        John Morrison
*CREATION DATE: 9/11/99
*LAST MODIFIED: 9/11/99
*PURPOSE:
*  The server has issued a refuel command to a base
*
*ARGUMENTS:
*  value     - Pointer to the bases structure
*  baseNum   - The base number to refuel
*  addAmount - Amount of items to add
*********************************************************/
void basesServerRefuel(bases *value, BYTE baseNum, BYTE addAmount) {
  BYTE oldArmour; /* Amount of old armour base had */

  if (baseNum < (*value)->numBases) {
    if ((*value)->item[baseNum].armour < BASE_FULL_ARMOUR) {
      oldArmour = (*value)->item[baseNum].armour;
      (*value)->item[baseNum].armour += addAmount;
      if ((*value)->item[baseNum].armour > BASE_FULL_ARMOUR) {
        (*value)->item[baseNum].armour = BASE_FULL_ARMOUR;
      }
      /* Update the frontend status as required */
      if (oldArmour == BASE_DEAD && (*value)->item[baseNum].armour > BASE_DEAD) {
        if (threadsGetContext() == FALSE) {
          frontEndStatusBase((BYTE) (baseNum+1), (basesGetStatusNum(value, (BYTE) (baseNum+1))));
        }
      }
    }
    if ((*value)->item[baseNum].shells < BASE_FULL_SHELLS) {
      (*value)->item[baseNum].shells += addAmount;
      if ((*value)->item[baseNum].shells > BASE_FULL_SHELLS) {
        (*value)->item[baseNum].shells = BASE_FULL_SHELLS;
      }
    }

    if ((*value)->item[baseNum].mines < BASE_FULL_MINES) {
      (*value)->item[baseNum].mines += addAmount;
      if ((*value)->item[baseNum].mines > BASE_FULL_MINES) {
        (*value)->item[baseNum].mines = BASE_FULL_MINES;
      }
    }
  }
}

/*********************************************************
*NAME:          baseIsCapturable
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Returns whether a base can be captured at a specific
*  location. Returns FALSE if it doesn't exist at location
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  xValue - X Location
*  yValue - Y Location
*********************************************************/
bool baseIsCapturable(bases *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */
  bool done;        /* Finised looping */
  BYTE count;       /* Looping Variable */  

  returnValue = FALSE;
  done = FALSE;
  count = 0;
  while (done == FALSE && count < ((*value)->numBases)) {
    if (((*value)->item[count].x) == xValue && ((*value)->item[count].y) == yValue) {
      done = TRUE;
      if ((*value)->item[count].owner == NEUTRAL || (*value)->item[count].armour <= MIN_ARMOUR_CAPTURE) {
        returnValue = TRUE;
      }
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesGetBrainBaseItem
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 25/11/99
*PURPOSE:
*  Returns the base info required by a brain for a 
*  specific base.
*
*ARGUMENTS:
*  value   - Pointer to the bases structure
*  baseNum - Base Number to get info for
*  wx      - Bases world X Location
*  wy      - Bases world Y Location
*  info    - Bases alliance
*  shells  - Amount of shells in the base
*  mines   - Amount of mines in the base
*  armour  - Amount of armour in the base
*********************************************************/
void basesGetBrainBaseItem(bases *value, BYTE baseNum, WORLD *wx, WORLD *wy, BYTE *info, BYTE *shell, BYTE *mines, BYTE *armour) {
  if (baseNum > 0 && baseNum <= (*value)->numBases) {
    baseNum--;
    *wx = (*value)->item[baseNum].x;
    *wx <<= TANK_SHIFT_MAPSIZE;
    *wy = (*value)->item[baseNum].y;
    *wy <<= TANK_SHIFT_MAPSIZE;
    if ((*value)->item[baseNum].owner == NEUTRAL) {
      *info = BASES_BRAIN_NEUTRAL;
    } else if (playersIsAllie(screenGetPlayers(), playersGetSelf(screenGetPlayers()), (*value)->item[baseNum].owner) == TRUE) {
      *info = BASES_BRAIN_FRIENDLY;
    } else {
      *info = BASES_BRAIN_HOSTILE;
    }

    *shell = (*value)->item[baseNum].shells;
    *mines = (*value)->item[baseNum].mines;
    *armour = (*value)->item[baseNum].armour /5;
  }
}

/*********************************************************
*NAME:          basesGetBrainBaseInRect
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 09/06/01
*PURPOSE:
*  Makes the brain base info for each base inside the
*  rectangle formed by the function parameters
*
*ARGUMENTS:
*  value     - Pointer to the bases structure
*  leftPos   - Left position of rectangle
*  rightPos  - Right position of rectangle
*  topPos    - Top position of rectangle
*  bottomPos - Bottom position of rectangle
*********************************************************/
void basesGetBrainBaseInRect(bases *value, BYTE leftPos, BYTE rightPos, BYTE topPos, BYTE bottomPos) {
  BYTE count;     /* Looping variable */
  WORLD wx;       /* X and Y Positions of the object */
  WORLD wy;
  BYTE owner;     /* Owner of the item */
  BYTE playerNum; /* Our player Number */
  bool isAllie;   /* Is this item our allie */
  BYTE armour;


  count = 0;
  playerNum = playersGetSelf(screenGetPlayers());

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

  while (count < ((*value)->numBases)) {
    isAllie = FALSE;
    if ((*value)->item[count].owner != NEUTRAL) {
      isAllie = playersIsAllie(screenGetPlayers(), playerNum, (*value)->item[count].owner);
    }
    if ((((*value)->item[count].x) >= leftPos && ((*value)->item[count].x) <= rightPos && ((*value)->item[count].y) >= topPos && ((*value)->item[count].y) <= bottomPos) || isAllie == TRUE) {
      /* In the rectangle */
      wx = (*value)->item[count].x;
      wx <<= TANK_SHIFT_MAPSIZE;
      wy = (*value)->item[count].y;
      wy <<= TANK_SHIFT_MAPSIZE;
      wx += MAP_SQUARE_MIDDLE;
      wy += MAP_SQUARE_MIDDLE;
      if ((*value)->item[count].owner == NEUTRAL) {
        owner = BASES_BRAIN_NEUTRAL;
      } else if (isAllie == TRUE) {
        owner = BASES_BRAIN_FRIENDLY;
      } else {
        owner = BASES_BRAIN_HOSTILE;
      }
      if ((*value)->item[count].armour > 0 && isAllie == FALSE) {
        armour = 1;
      } else {
        armour = (BYTE) ((*value)->item[count].armour / 5);
      }
      screenAddBrainObject(BASES_BRAIN_OBJECT_TYPE, wx, wy, count, armour, owner);
    }
    count++;
  }
}

/*********************************************************
*NAME:          basesGetMaxs
*AUTHOR:        John Morrison
*CREATION DATE: 13/6/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  Gets the maximum positions values of all the bases.
*  eg left most, right most etc.
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  leftPos   - Pointer to hold the left most value
*  rightPos  - Pointer to hold the right most value
*  topPos    - Pointer to hold the top most value
*  bottomPos - Pointer to hold the bottom most value
*********************************************************/
void basesGetMaxs(bases *value, int *leftPos, int *rightPos, int *topPos, int *bottomPos) {
  BYTE count; /* Looping Variable */

  *topPos = MAP_ARRAY_SIZE;
  *bottomPos = -1;
  *leftPos = MAP_ARRAY_SIZE;
  *rightPos = -1;

  count = 0;
  while (count < ((*value)->numBases)) {
    if ((*value)->item[count].x < *leftPos) {
      *leftPos = (*value)->item[count].x;
    }
    if ((*value)->item[count].x > *rightPos) {
      *rightPos = (*value)->item[count].x;
    }
    if ((*value)->item[count].y < *topPos) {
      *topPos = (*value)->item[count].y;
    }
    if ((*value)->item[count].y > *bottomPos) {
      *bottomPos = (*value)->item[count].y;
    }  
    count++;
  }
}

/*********************************************************
*NAME:          basesMoveAll
*AUTHOR:        John Morrison
*CREATION DATE: 13/6/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  Repositions the bases by moveX, moveY
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  moveX  - The X move amount 
*  moveY  - The Y move amount 
*********************************************************/
void basesMoveAll(bases *value, int moveX, int moveY) {
  BYTE count; /* Looping Variable */

  count = 0;
  while (count < ((*value)->numBases)) {
    (*value)->item[count].x = (BYTE) ((*value)->item[count].x + moveX);
    (*value)->item[count].y = (BYTE) ((*value)->item[count].y + moveY);
    count++;
  }
}

/*********************************************************
*NAME:          basesGetOwnerBitMask
*AUTHOR:        John Morrison
*CREATION DATE: 22/6/00
*LAST MODIFIED: 22/6/00
*PURPOSE:
* Returns the owner bitmask for all the bases own by a 
* player.
*
*ARGUMENTS:
*  value  - Pointer to the bases  structure
*  owner  - Owner to return bases owned for
*  moveY  - The Y move amount 
*********************************************************/
PlayerBitMap basesGetOwnerBitMask(bases *value, BYTE owner) {
  PlayerBitMap returnValue; /* Value to return */
  BYTE count;               /* Looping variable */

  count = 0;
  returnValue = 0;
  while (count < (*value)->numBases) {
    if ((*value)->item[count].owner == owner) {
      returnValue |= 1 << count;
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          basesClearMines
*AUTHOR:        John Morrison
*CREATION DATE: 31/7/00
*LAST MODIFIED: 31/7/00
*PURPOSE:
* Removes mines from under bases.
*
*ARGUMENTS:
*  value  - Pointer to the bases structure
*  mp     - Pointer to the map structure
*********************************************************/
void basesClearMines(bases *value, map *mp) {
  BYTE count;       /* Looping Variable */  
  BYTE terrain;     /* Terrain Value    */

  count = 0;
  while (count < ((*value)->numBases)) {
    terrain = mapGetPos(mp, (*value)->item[count].x, (*value)->item[count].y);
    if (terrain >= MINE_START && terrain <= MINE_END) {
      mapSetPos(mp, (*value)->item[count].x, (*value)->item[count].y, (BYTE) (terrain - MINE_SUBTRACT), FALSE, TRUE);
    }
    count++;
  }
}

/*********************************************************
*NAME:          basesGetNumberOwnedByPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 19/11/03
*LAST MODIFIED: 19/11/03
*PURPOSE:
* Returns the number of bases owned by this player
*
*ARGUMENTS:
*  value     - Pointer to the bases structure
*  playerNum - Player number for bases
*********************************************************/
BYTE basesGetNumberOwnedByPlayer(bases *value, BYTE playerNum) {
  BYTE returnValue; /* Value to return */
  BYTE count; /* Looping variable */

  count = 0;
  returnValue = 0;

  while (count < (*value)->numBases) {
    if ((*value)->item[count].owner == playerNum) {
      returnValue++;
    }
    count++;
  }

  return returnValue;
}
