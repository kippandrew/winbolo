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
*Name:          Players
*Filename:      players.c
*Author:        John Morrison
*Creation Date: 09/02/02
*Last Modified: 09/08/04
*Purpose:
*  Looks after players. Alliences between etc.
*********************************************************/

#include <string.h>

#include "allience.h"
#include "frontend.h"
#include "global.h"
#include "labels.h"
#include "lgm.h"
#include "log.h"
#include "messages.h"
#include "network.h"
#include "players.h"
#include "screen.h"
#include "screenlgm.h"
#include "screentank.h"
#include "tank.h"
#include "tilenum.h"
#include "util.h"

#include "../server/servercore.h"


char myLastPlayerName[PLAYER_NAME_LEN];

/*********************************************************
*NAME:          playersCreate
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Sets up the players structure.
*
*ARGUMENTS:
* plrs - Pointer to the players object to create
*********************************************************/
void playersCreate(players *plrs) {
  BYTE count; /* Looping variable */

  New(*plrs);
  memset(*plrs, 0, sizeof(**plrs));
  if (backendGetContext() == FALSE) {
    myLastPlayerName[0] = EMPTY_CHAR;
  }
    
  
  (*plrs)->myPlayerNum = NEUTRAL; /* Not set yet */
  for (count = 0;count<MAX_TANKS;count++) {
    (*plrs)->item[count].inUse = FALSE;
    (*plrs)->item[count].needUpdate = FALSE;
    (*plrs)->item[count].allie = allienceCreate();
    (*plrs)->item[count].isChecked = FALSE;
    (*plrs)->playerBrainNames[count][0] = '\0';
  }
}

/*********************************************************
*NAME:          playersDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Destroys the playesr structure
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
void playersDestroy(players *plrs) {
  BYTE count; /* Looping variable */

  if ((*plrs) != NULL) {
    for (count = 0;count<MAX_TANKS;count++) {
      (*plrs)->item[count].inUse = FALSE;
      allienceDestroy(&((*plrs)->item[count].allie));
    }
    if (*plrs != NULL) {
      Dispose(*plrs);
    }
    (*plrs) = NULL;
  }
}

/*********************************************************
*NAME:          playersGetSelf
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns your own player number
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
BYTE playersGetSelf(players *plrs) {
  return (*plrs)->myPlayerNum;
}

/*********************************************************
*NAME:          playersSetSelf
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Sets your own player number and player name. Returns 
* whether the operation succeeded.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* playerName - The player name to set
*********************************************************/
bool playersSetSelf(players *plrs, BYTE playerNum, char *playerName) {
  bool returnValue; /* Value to return */

  returnValue = FALSE;
  if ((*plrs)->item[playerNum].inUse == FALSE) {
    if (playersNameTaken(plrs, playerName) == FALSE) {
      if (backendGetContext() == FALSE) {
        strcpy(myLastPlayerName, playerName);
      }
      returnValue = TRUE;
      strcpy((*plrs)->item[playerNum].playerName, playerName);    
      (*plrs)->item[playerNum].inUse = TRUE;
      (*plrs)->myPlayerNum = playerNum;
      lgmSetPlayerNum(screenGetLgmFromPlayerNum(playerNum), playerNum);
      utilCtoPString(playerName, (char *) (*plrs)->playerBrainNames[playerNum]);
      if (threadsGetContext() == FALSE) {
        frontEndSetPlayer((playerNumbers) playerNum, playerName);
      }
      if (backendGetContext() == FALSE) {
        strcpy(myLastPlayerName, playerName);
      }
    }
  }
  return returnValue;
}

/*********************************************************
*NAME:          playersSetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 18/02/99
*LAST MODIFIED: 05/05/01
*PURPOSE:
* Sets/changes a player name. Returns whether the operation
* succeed or not. (Fails if name is already in use) If it
* sccueeds then it makes the appropriate anouncement
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* playerName - The player name to set
*********************************************************/
bool playersSetPlayerName(players *plrs, BYTE playerNum, char *playerName) {
  bool returnValue;           /* Value to return */
  char messageStr[FILENAME_MAX]; /* Newswire Message */
  char label[FILENAME_MAX];   /* Used to hold the string made by label */
  char temp[FILENAME_MAX];
  
  messageStr[0] = '\0';
  label[0] = '\0';
  returnValue = FALSE;
  if ((*plrs)->item[playerNum].inUse == TRUE) {
    if (playersNameTaken(plrs, playerName) == FALSE) {
      /* OK to change do so and then make the message */
      returnValue = TRUE;
      /* Make Message */
      strcat(messageStr, MESSAGE_QUOTES);
      if (playerNum == (*plrs)->myPlayerNum) {
        labelMakeMessage(label, (*plrs)->item[playerNum].playerName, langGetText(MESSAGE_THIS_COMPUTER));
      } else {
        labelMakeMessage(label, (*plrs)->item[playerNum].playerName, (*plrs)->item[playerNum].location);
      }
      strcat(messageStr, label);
      strcat(messageStr, MESSAGE_QUOTES);
      strcat(messageStr, langGetText(MESSAGE_CHANGENAME));
      strcat(messageStr, playerName);
      strcat(messageStr, MESSAGE_QUOTES);
      messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), messageStr);
      /* Update the name */
      strcpy((*plrs)->item[playerNum].playerName, playerName);
      utilCtoPString(playerName, (char *) (*plrs)->playerBrainNames[playerNum]);
      strcpy(temp, playerName);
      if (playerNum != (*plrs)->myPlayerNum) {
        strcat(temp, "@");
        strcat(temp, (*plrs)->item[playerNum].location);
      } else if (backendGetContext() == FALSE) {
        strcpy(myLastPlayerName, playerName);
      }
      if (threadsGetContext() == FALSE) {
        frontEndSetPlayer((playerNumbers) playerNum, temp);
      }
      /* Log it */
      logAddEvent(log_ChangeName, playerNum, 0, 0, 0, 0, (*plrs)->playerBrainNames[playerNum]); 
    }
  }
  return returnValue;
}


/*********************************************************
*NAME:          playersSetPlayersMenu
*AUTHOR:        John Morrison
*CREATION DATE: 09/02/02
*LAST MODIFIED: 09/02/02
*PURPOSE:
* Sets the entries in the players menu if they are in use
*
*ARGUMENTS:
* plrs - Pointer to the players object 
********************************************************/
void playersSetPlayersMenu(players *plrs) {
  BYTE count; /* Looping variable */
  char temp[FILENAME_MAX];

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE) {
      strcpy(temp, (*plrs)->item[count].playerName);
      if (count != (*plrs)->myPlayerNum) {
        strcat(temp, "@");
        strcat(temp, (*plrs)->item[count].location);
      }
      if (threadsGetContext() == FALSE) {
        frontEndSetPlayer((playerNumbers) count, temp);
      }
    }
    count++;
  }
}

/*********************************************************
*NAME:          playersSetPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Sets a player up.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* playerName - The player name to set
* location   - Location string of the player.
* mx         - Map X location
* my         - Map Y location
* px         - Pixel X location
* py         - Pixel Y location
* frame      - Tank animation frame.
* onBoat     - is the player on a boat?
* numAllies  - Number of Allies the player has
* allies     - BYTE buffer containing each allie
*********************************************************/
void playersSetPlayer(players *plrs, BYTE playerNum, char *playerName, char *location, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, bool onBoat, BYTE numAllies, BYTE *allies) {
  BYTE count; /* Looping variable */
  char str[512]; /* The player name */
  int iMyPlayerNum;
  int iPlayerNum;

  iMyPlayerNum = (int)(*plrs)->myPlayerNum;
  iPlayerNum = (int)playerNum;

  /* This code block is only executed for clients besides your own in the player list */
  if ((*plrs)->item[playerNum].inUse == FALSE) {
    (*plrs)->item[playerNum].inUse = TRUE;
    strcpy((*plrs)->item[playerNum].playerName, playerName);
    strcpy((*plrs)->item[playerNum].location, location);
    utilCtoPString(playerName, (char *) ((*plrs)->playerBrainNames[playerNum]));
    (*plrs)->item[playerNum].mapX = mx;
    (*plrs)->item[playerNum].mapY = my;
    (*plrs)->item[playerNum].pixelX = px;
    (*plrs)->item[playerNum].pixelY = py;
    (*plrs)->item[playerNum].frame = frame;
    (*plrs)->item[playerNum].onBoat = onBoat;
    (*plrs)->item[playerNum].allie = allienceCreate();
    count = 0;
    while (count < numAllies) {
      allienceAdd(&((*plrs)->item[playerNum].allie), allies[count]);
      count++;
    }
  }
  /* Processing our client, we want to store it's IP so that it can be snagged for the network info */
  else if (iMyPlayerNum == iPlayerNum)
  {
	  strcpy((*plrs)->item[playerNum].location, location);
  }
  
  /* Update front end if we are in a running game (ie not in the joining phase) */
  if (netGetStatus() != netFailed) {
    strcpy(str, (*plrs)->item[playerNum].playerName);
    if (playerNum != (*plrs)->myPlayerNum) {
      strcat(str, "@");
      strcat(str, (*plrs)->item[playerNum].location);
    }
    if (threadsGetContext() == FALSE) {
      frontEndSetPlayer((playerNumbers) playerNum, str);
      frontEndStatusTank((BYTE) (playerNum+1), playersScreenAllience(plrs, playerNum));
	  frontEndRedrawAll();
    }
  }
  
} 


/*********************************************************
*NAME:          playerSetLocation
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
* Sets the location of the player at ip to the hostname
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* ip   - Current IP address of the player
* host - Hostname of the player
*********************************************************/
void playerSetLocation(players *plrs, char *ip, char *host) {
  BYTE count; /* Looping variable */
  BYTE found = 0xFF;
  char str[512]; /* The player name */
  
  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && count != (*plrs)->myPlayerNum) {
      if (strcmp((*plrs)->item[count].location, ip) == 0) {
        strcpy((*plrs)->item[count].location, host);
        found = count;
      }
    }
    count++;
  }

  if (found != 0xFF) {
    /* Update front end if we are in a running game (ie not in the joining phase) */
    if (netGetStatus() != netFailed) {
      strcpy(str, (*plrs)->item[found].playerName);
      if (found != (*plrs)->myPlayerNum) {
        strcat(str, "@");
        strcat(str, (*plrs)->item[found].location);
      }
      if (threadsGetContext() == FALSE) {
        frontEndSetPlayer((playerNumbers) found, str);
        frontEndSetPlayerCheckState((playerNumbers) found, (*plrs)->item[found].isChecked);
      }
    }
  }
}

/*********************************************************
*NAME:          playersUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 28/2/99
*PURPOSE:
* Updates a player with specific location data.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* mx         - Map X location
* my         - Map Y location
* px         - Pixel X location
* py         - Pixel Y location
* frame      - Tank animation frame.
* onBoat     - is the player on a boat?
* lgmMX      - Lgm Map X Position
* lgmMY      - Lgm Map Y Position
* lgmPX      - Lgm Map X Position
* lgmPY      - Lgm Map Y Position
* lgmFrame   - Lgm Frame number
*********************************************************/
void playersUpdate(players *plrs, BYTE playerNum, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, bool onBoat, BYTE lgmMX, BYTE lgmMY, BYTE lgmPX, BYTE lgmPY, BYTE lgmFrame) {
 if ((*plrs)->item[playerNum].inUse == TRUE) {
    (*plrs)->item[playerNum].mapX = mx;
    (*plrs)->item[playerNum].mapY = my;
    (*plrs)->item[playerNum].pixelX = px;
    (*plrs)->item[playerNum].pixelY = py;
    (*plrs)->item[playerNum].frame = frame;
    (*plrs)->item[playerNum].onBoat = onBoat;
    (*plrs)->item[playerNum].lgmMapX = lgmMX;
    (*plrs)->item[playerNum].lgmMapY = lgmMY;
    (*plrs)->item[playerNum].lgmPixelX = lgmPX;
    (*plrs)->item[playerNum].lgmPixelY = lgmPY;
    (*plrs)->item[playerNum].lgmFrame = lgmFrame;
  }
}

/*********************************************************
*NAME:          playersGameTickUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 15/09/02
*LAST MODIFIED: 15/09/02
*PURPOSE:
* Updates a player with specific location data.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* mx         - Map X location
* my         - Map Y location
* px         - Pixel X location
* py         - Pixel Y location
* frame      - Tank animation frame.
* onBoat     - is the player on a boat?
* lgmMX      - Lgm Map X Position
* lgmMY      - Lgm Map Y Position
* lgmPX      - Lgm Map X Position
* lgmPY      - Lgm Map Y Position
* lgmFrame   - Lgm Frame number
*********************************************************/
void playersGameTickUpdate(players *plrs) {
  BYTE count; /* Looping variable */

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE) {
      if ((*plrs)->item[count].lgmMapX != 0 && (*plrs)->item[count].lgmMapY != 0 && (*plrs)->item[count].lgmFrame != LGM_HELICOPTER_FRAME) {
        (*plrs)->item[count].lgmFrame++;
        if ((*plrs)->item[count].lgmFrame > LGM_MAX_FRAMES ) {
          (*plrs)->item[count].lgmFrame = 0;
        }
      }
    }
    count++;
  }
}

/*********************************************************
*NAME:          playersGetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Gets a player name.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* dest       - Destination string
*********************************************************/
void playersGetPlayerName(players *plrs, BYTE playerNum, char *dest) {
  if (plrs != NULL) {
    if ((*plrs)->item[playerNum].inUse == TRUE) {
      strcpy(dest, (*plrs)->item[playerNum].playerName);
    } else {
      strcpy(dest, NO_TANK);
    }
  } else if (backendGetContext() == FALSE) {
    /* We are a client.. */
    strcpy(dest, myLastPlayerName);
  }
}

/*********************************************************
*NAME:          playersGetPlayerLocation
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Gets a player location.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* dest       - Destination string
*********************************************************/
void playersGetPlayerLocation(players *plrs, BYTE playerNum, char *dest) {
  if (plrs != NULL) {
    if ((*plrs)->item[playerNum].inUse == TRUE) {
      strcpy(dest, (*plrs)->item[playerNum].location);
    } else {
      strcpy(dest, NO_TANK);
    }
  } 
}

/*********************************************************
*NAME:          playersMakeMessageName
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Makes the message name for a specific player
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* dest       - Destination string
*********************************************************/
void playersMakeMessageName(players *plrs, BYTE playerNum, char *dest) {
  char label[FILENAME_MAX];   /* Used to hold the string made by label */
  
  label[0] = '\0';
  if (playerNum == (*plrs)->myPlayerNum) {
    labelMakeMessage(label, (*plrs)->item[playerNum].playerName, langGetText(MESSAGE_THIS_COMPUTER));
    strcpy(dest, label);
  } else if ((*plrs)->item[playerNum].inUse == FALSE) {
    strcpy(dest, NO_TANK);
  } else {
    labelMakeMessage(label, (*plrs)->item[playerNum].playerName, (*plrs)->item[playerNum].location);
    strcpy(dest, label);
  }
}

/*********************************************************
*NAME:          playersMakeScreenName
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Makes the message name for a specific player
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum  - The player number to set
* dest       - Destination string
*********************************************************/
void playersMakeScreenName(players *plrs, BYTE playerNum, char *dest) {
  char label[FILENAME_MAX];   /* Used to hold the string made by label */
  
  label[0] = '\0';
  if ((*plrs)->item[playerNum].inUse == TRUE) {
    if (playerNum == (*plrs)->myPlayerNum) {
      labelMakeTankLabel(label, (*plrs)->item[playerNum].playerName, langGetText(MESSAGE_THIS_COMPUTER), TRUE);
    } else {
      labelMakeTankLabel(label, (*plrs)->item[playerNum].playerName, (*plrs)->item[playerNum].location, FALSE);
    }
    strcpy(dest, label);
  }
}


/*********************************************************
*NAME:          playersIsAllie
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns whether playerA is allied to playerB
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerA - The player number to check
* playerB  - The player number to check
*********************************************************/
bool playersIsAllie(players *plrs, BYTE playerA, BYTE playerB) {
  bool returnValue; /* Value to return */
  BYTE check;       /* Which item to check - In case one player has left */
  BYTE check2;      /* Whick item to check for - In case one player has left */

  check = check2 = NEUTRAL;
  returnValue = FALSE;

  if (playerA == playerB) {
    returnValue = TRUE;
  } else if (playerA == NEUTRAL || playerB == NEUTRAL) {
    returnValue = FALSE;
  } else {
    /* Check for exist */
    if ((*plrs)->item[playerA].inUse == TRUE) {
      check = playerA;
      check2 = playerB;
    } else if ((*plrs)->item[playerB].inUse == TRUE) {
      check = playerB;
      check2 = playerA;
    }
    
    if (check != NEUTRAL) {
      returnValue = allienceExist(&((*plrs)->item[check].allie), check2);
    }
  }
  return returnValue;
}

/*********************************************************
*NAME:          playersGetNumAllie
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns the number allies a player has. (Atleast 1 as it
* includes themselves)
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The player number to check
*********************************************************/
BYTE playersGetNumAllie(players *plrs, BYTE playerNum) {
  BYTE returnValue; /* Value to return */

  returnValue = 0;
  if ((*plrs)->item[playerNum].inUse == TRUE) {
    returnValue = allienceNumAllies(&((*plrs)->item[playerNum].allie)) + 1;
  }
  return returnValue;
}

/*********************************************************
*NAME:          playersNameTaken
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns whether some player already is using that name
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* checkName - The player name to check
*********************************************************/
bool playersNameTaken(players *plrs, char *checkName) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */

  count = 0;
  returnValue = FALSE;

  while (count<MAX_TANKS && returnValue == FALSE) {
    if ((*plrs)->item[count].inUse == TRUE) {
      if (strcmp((*plrs)->item[count].playerName, checkName) == 0) {
        returnValue = TRUE;
      } else if ((*plrs)->item[count].playerName[0] == '*') {
        /* Try removing the star */
        if (strcmp((*plrs)->item[count].playerName+1, checkName) == 0) {
          returnValue = TRUE;
        }
      }

    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          playersScreenAllience
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns a player is screen allience type.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - Player number to check
*********************************************************/
tankAlliance playersScreenAllience(players *plrs, BYTE playerNum) {
  tankAlliance returnValue; /* Value to return */

  returnValue = tankNone;
  if (playerNum < MAX_TANKS) {
    if ((*plrs)->item[playerNum].inUse == FALSE) {
      returnValue = tankNone;
    } else if (playerNum == (*plrs)->myPlayerNum) {
      returnValue = tankSelf;
    } else if (allienceExist(&((*plrs)->item[(*plrs)->myPlayerNum].allie), playerNum) == TRUE) {
      returnValue = tankAllie;
    } else {
      returnValue = tankEvil;
    }
  }
  return returnValue;
}


/*********************************************************
*NAME:          playersMakeScreenTanks
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED:  8/1/00
*PURPOSE:
* Adds each player to the screen tanks structure
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* value    - screen tanks data structure
* leftPos  - Left bound
* rightPos - Left bound
* top      - top bound
* bottom   - Bottom bound
*********************************************************/
void playersMakeScreenTanks(players *plrs, screenTanks *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  char playerName[FILENAME_MAX]; /* Holds playername/location info */
  WORLD conv;                    /* Used in conversion */
  WORLD conv2;
  WORLD ourTankX;                /* Our tank X and Y co-ordinates */
  WORLD ourTankY;
  WORLD tx;                      /* Current tanks X and Y co-ordinates */
  WORLD ty;
  BYTE frame;                    /* Holds frame info */
  BYTE count;                    /* Looping variable */
  BYTE mx;                       /* Tank map and pixel X and Y co-ordinates */
  BYTE my;
  BYTE px;
  BYTE py;

/* FIXME: This function could use some optimisation I think */
  screenGetTankWorld(&ourTankX, &ourTankY);
  
  for (count=0;count<MAX_TANKS;count++) {
    if ((*plrs)->item[count].inUse == TRUE && count != (*plrs)->myPlayerNum) {
      playerName[0] = EMPTY_CHAR;
      /* Extract fixed map co-ordinates */
      conv = (*plrs)->item[count].mapX;
      conv <<= TANK_SHIFT_MAPSIZE;
      conv2 = (*plrs)->item[count].pixelX;
      conv2 <<= TANK_SHIFT_RIGHT2;
      conv += conv2;
      conv -= TANK_SUBTRACT;
      conv >>= TANK_SHIFT_MAPSIZE;
      mx = (BYTE) conv;
      conv = (*plrs)->item[count].mapY;
      conv <<= TANK_SHIFT_MAPSIZE;
      conv2 = (*plrs)->item[count].pixelY;
      conv2 <<= TANK_SHIFT_RIGHT2;
      conv += conv2;
      conv -= TANK_SUBTRACT;
      conv >>= TANK_SHIFT_MAPSIZE;
      my = (BYTE) conv;

      if (mx >= leftPos && mx <= rightPos && my >= top && my <= bottom) {
        tx = (WORLD) (((*plrs)->item[count].mapX << TANK_SHIFT_MAPSIZE) + ((*plrs)->item[count].pixelX<< TANK_SHIFT_RIGHT2));
        if (tx > ourTankX) {
          conv = tx - ourTankX;
        } else {
          conv = ourTankX - tx;
        }
        ty = (WORLD) ((((*plrs)->item[count].mapY << TANK_SHIFT_MAPSIZE)) + (((*plrs)->item[count].pixelY<< TANK_SHIFT_RIGHT2)));
        if (ty > ourTankY) {
          conv2 = ty - ourTankY;
        } else {
          conv2 = ourTankY - ty;
        }
        if ((screenIsItemInTrees(tx, ty) == FALSE) || (conv < MIN_TREEHIDE_DIST && conv2 < MIN_TREEHIDE_DIST)  ) {
          /* Extract fixed pixel co-ordinates */
          conv = (*plrs)->item[count].mapX;
          conv <<= TANK_SHIFT_MAPSIZE;
          conv2 = (*plrs)->item[count].pixelX;
          conv2 <<= TANK_SHIFT_RIGHT2;
          conv += conv2;
          conv -= TANK_SUBTRACT;
          conv <<= TANK_SHIFT_MAPSIZE;
          conv >>= TANK_SHIFT_PIXELSIZE;
          px = (BYTE) conv;

          conv = (*plrs)->item[count].mapY;
          conv <<= TANK_SHIFT_MAPSIZE;
          conv2 = (*plrs)->item[count].pixelY;
          conv2 <<= TANK_SHIFT_RIGHT2;
          conv += conv2;
          conv -= TANK_SUBTRACT;
          conv <<= TANK_SHIFT_MAPSIZE;
          conv >>= TANK_SHIFT_PIXELSIZE;
          py = (BYTE) conv;
          /* Extract player screen name */
          playersMakeScreenName(plrs, count, playerName);
          frame = (*plrs)->item[count].frame;
          if ((*plrs)->item[count].onBoat == TRUE) {
            frame += TANK_BOAT_ADD;
          }
          if (allienceExist(&((*plrs)->item[count].allie), (*plrs)->myPlayerNum) == TRUE) {
            frame += TANK_GOOD_ADD;
          } else {
            frame += TANK_EVIL_ADD;
          }
          screenTanksAddItem(value,(BYTE) (mx - leftPos), (BYTE) (my - top), px, py, frame, count, playerName); 
        }
      }
    }
  }
}

/*********************************************************
*NAME:          playersMakeScreenLgm
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED:  7/3/99
*PURPOSE:
* Adds each players lgm to the screen LGM structure
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* value    - screen LGM data structure
* leftPos  - Left bound
* rightPos - Left bound
* top      - top bound
* bottom   - Bottom bound
*********************************************************/
void playersMakeScreenLgm(players *plrs, screenLgm *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  WORLD wx;
  WORLD wy;
  WORLD conv;                    /* Used in conversion */
  WORLD conv2;
  WORLD ourTankX;                /* Our tank X and Y co-ordinates */
  WORLD ourTankY;
  BYTE count;                    /* Looping variable */

  for (count=0;count<MAX_TANKS;count++) {
    if ((*plrs)->item[count].inUse == TRUE && count != (*plrs)->myPlayerNum) {
      if ((*plrs)->item[count].lgmMapX >= leftPos && (*plrs)->item[count].lgmMapX <= rightPos && (*plrs)->item[count].lgmMapY >= top && (*plrs)->item[count].lgmMapY <= bottom) {
        wx = (*plrs)->item[count].lgmMapX << TANK_SHIFT_MAPSIZE;
        wx += (*plrs)->item[count].lgmPixelX << TANK_SHIFT_RIGHT2;
        wy = (*plrs)->item[count].lgmMapY << TANK_SHIFT_MAPSIZE;
        wy += (*plrs)->item[count].lgmPixelY << TANK_SHIFT_RIGHT2;
        screenGetTankWorld(&ourTankX, &ourTankY);
        if (wx > ourTankX) {
          conv = wx - ourTankX;
        } else {
          conv = ourTankX - wx;
        }
        if (wy > ourTankY) {
          conv2 = wy - ourTankY;
        } else {
          conv2 = ourTankY - wy;
        }
        
        if ((*plrs)->item[count].lgmFrame == LGM_HELICOPTER_FRAME || screenIsItemInTrees(wx, wy) == FALSE || (conv < MIN_TREEHIDE_DIST && conv2 < MIN_TREEHIDE_DIST)) {
          screenLgmAddItem(value,(BYTE) ((*plrs)->item[count].lgmMapX - leftPos), (BYTE) ((*plrs)->item[count].lgmMapY - top), (*plrs)->item[count].lgmPixelX, (*plrs)->item[count].lgmPixelY, (*plrs)->item[count].lgmFrame); 
        }
      }
    }
  }
}

/*********************************************************
*NAME:          playersGetNumPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 18/2/99
*PURPOSE:
* Returns the number of players in the game
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
BYTE playersGetNumPlayers(players *plrs) {
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping variable */


  returnValue = 0;
  // This is some code to attempt to debug a linbolods crash with plrs = 0x0
  if (*plrs != 0x0){
	  if (*plrs != NULL) {
		for (count=0;count<MAX_TANKS;count++) {
		  if ((*plrs)->item[count].inUse == TRUE) {
			returnValue++;
		  }
		}
	  }
  } else {
	// At this point we will print a message to the console, and then allow the function to return
	// hopefully this will allow a logfile to be generated rather than a seg fault, so that we can perhaps track this error better.
    screenServerConsoleMessage("Players is equal to zero, something has happened that shouldn't have. \r\n");
  }
  return returnValue;
}

/*********************************************************
*NAME:          playersIsTankCloser
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED: 19/2/99
*PURPOSE:
* Returns whether a tank not allied to the pillbox 
* is closer then this players tank.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* x          - X co-ordinate of the pillbox
* y          - Y co-ordinate of the pillbox
* pillOwner  - Who owns the pill
* tankAmount - My tanks distance from pill
*********************************************************/
bool playersIsTankCloser(players *plrs, WORLD x, WORLD y, BYTE pillOwner, double tankAmount) {
  bool returnValue; /* Value to return */
  WORLD tankX;      /* Tank X and Y co-ordinates */
  WORLD tankY;
  WORLD conv;       /* Used in conversions */
  WORLD diffX;      /* Used in calculating in trees */
  WORLD diffY;
  double test;      /* The amount we are testing */
  bool inRange;     /* Is the tank we are testing even in range */
  BYTE count;       /* Looping variable */

  returnValue = FALSE;
  test = 0;
  count = 0;

  while (count < MAX_TANKS && returnValue == FALSE) {
    /* Check to see player slot is being used */
    if ((*plrs)->item[count].inUse == TRUE && count != (*plrs)->myPlayerNum) {
      /* Check for non-allined tank */
      if (allienceExist(&((*plrs)->item[count].allie), pillOwner) == FALSE && count != pillOwner && ((*plrs)->item[count].mapX != 0 && (*plrs)->item[count].mapY != 0)) {
        /* Make tankX and tankY */
        tankX = (*plrs)->item[count].mapX;
        tankX <<= BRADIAN_ADD8;
        conv = (*plrs)->item[count].pixelX;
        conv <<= TANK_SHIFT_RIGHT2;
        tankX += conv;
        tankY = (*plrs)->item[count].mapY;
        tankY <<= BRADIAN_ADD8;
        conv = (*plrs)->item[count].pixelY;
        conv <<= TANK_SHIFT_RIGHT2;
        tankY += conv;
        if (tankX > x) {
          diffX = tankX - x;
        } else {
          diffX = x -tankX;
        }
        if (tankY > y) {
          diffY = tankY - y;
        } else {
          diffY = y - tankY;
        }
        /* Not in trees check */
        if (screenIsItemInTrees(tankX, tankY) == FALSE || (diffX < MIN_TREEHIDE_DIST && diffY < MIN_TREEHIDE_DIST)) {
          /* Distance check */
          inRange = utilIsItemInRange(x, y, tankX, tankY, PILLBOX_RANGE, &test);
          if (inRange == TRUE && test < tankAmount) {
            returnValue = TRUE;
          }
        }
      }
    }
    count++;
  }
  return returnValue;
}

/*********************************************************
*NAME:          playersIsTankHit
*AUTHOR:        John Morrison
*CREATION DATE: 19/2/99
*LAST MODIFIED: 31/7/00
*PURPOSE:
* Returns the player number if a player was hit otherwise
* returns NEUTRAL (255)
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* x     - X co-ordinate of the shell
* y     - Y co-ordinate of the shell
* angle - Angle the shell is traveling
* owner - Owner of the shell (players can not be hit
*         by there own shells
*********************************************************/
BYTE playersIsTankHit(players *plrs, WORLD x, WORLD y, TURNTYPE angle, BYTE owner) {
  BYTE returnValue;   /* Value to return */
  WORLD tankX;        /* Tank X and Y co-ordinates */
  WORLD tankY;
  WORLD conv;         /* Used in conversions */
  TURNTYPE tankAngle; /* Angle tank is traveling */
  BYTE count;         /* Looping variable */

  returnValue = NEUTRAL;
  count = 0;

  while (count < MAX_TANKS && returnValue == NEUTRAL) {
    /* Check to see player slot is being used */
    if ((*plrs)->item[count].inUse == TRUE && count != (*plrs)->myPlayerNum && count != owner) {
      /* Check for a collision */
       tankX = (*plrs)->item[count].mapX;
       tankX <<= TANK_SHIFT_MAPSIZE;
       conv = (*plrs)->item[count].pixelX;
       conv <<= TANK_SHIFT_RIGHT2;
       tankX += conv;
       tankY = (*plrs)->item[count].mapY;
       tankY <<= TANK_SHIFT_MAPSIZE;
       conv = (*plrs)->item[count].pixelY;
       conv <<= TANK_SHIFT_RIGHT2;
       tankY += conv;
       tankAngle = (TURNTYPE) (((*plrs)->item[count].frame) * TANK_FRAMES);
       if ((utilIsTankHit(tankX, tankY, tankAngle, x, y, angle)) == TRUE) {
         returnValue = count;
       }
    }
    count++;
  }
  return returnValue;
}

/*********************************************************
*NAME:          playersMakeNetAlliences
*AUTHOR:        John Morrison
*CREATION DATE: 25/2/99
*LAST MODIFIED: 29/2/99
*PURPOSE:
* Returns the number of alliences playerNum has. Also
* copies each into the array value
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - Player number to make for
* value     - Array to hold the alliences
*********************************************************/
BYTE playersMakeNetAlliences(players *plrs, BYTE playerNum, BYTE *value) {
  BYTE returnValue; /* Value to return */
  BYTE count; /* Looping variable */

  returnValue = allienceNumAllies(&((*plrs)->item[playerNum].allie));
  count = 1;
  while (count <= returnValue) {
    value[count-1] = allienceReturnNum(&((*plrs)->item[playerNum].allie), (BYTE) (count-1));
    count++;
  }
  return returnValue;
}

/*********************************************************
*NAME:          playersGetFirstNotUsed
*AUTHOR:        John Morrison
*CREATION DATE: 25/2/99
*LAST MODIFIED: 25/2/99
*PURPOSE:
* Returns the first not used player number.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* value - Array to hold the alliences
*********************************************************/
BYTE playersGetFirstNotUsed(players *plrs) {
  BYTE returnValue; /* Value to return */
  BYTE count; /* Looping variable */

  returnValue = 0;
  returnValue = NEUTRAL;
  count = 0;
  while (count < MAX_TANKS && returnValue == NEUTRAL) {
    if ((*plrs)->item[count].inUse == FALSE) {
      returnValue = count;
    }
    count++;
  }
  return returnValue;
}

/*********************************************************
*NAME:          playersLeaveGame
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED: 2/11/99
*PURPOSE:
* A player has left the game.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The number of the player that has left
*********************************************************/
void playersLeaveGame(players *plrs, BYTE playerNum) {
  char name[FILENAME_MAX];   /* The Player Name */
  char output[FILENAME_MAX]; /* The message */
  BYTE count;                /* Looping variable */


  if ((*plrs)->item[playerNum].inUse == TRUE) {
    allienceDestroy(&((*plrs)->item[playerNum].allie));

    count = 0;
    while (count < MAX_TANKS) {
      if ((*plrs)->item[playerNum].inUse == TRUE && count != playerNum) {
        allienceRemove(&((*plrs)->item[count].allie), playerNum);
      }
      count++;
    }

    output[0] = '\0';
    name[0] = '\0';
    playersMakeMessageName(plrs, playerNum, name);
    (*plrs)->item[playerNum].inUse = FALSE;
    (*plrs)->item[playerNum].needUpdate = FALSE;
    (*plrs)->item[playerNum].isChecked = FALSE;
    (*plrs)->playerBrainNames[playerNum][0] = '\0';
    if (threadsGetContext() == FALSE) {
      frontEndClearPlayer((playerNumbers) playerNum);
      frontEndStatusTank((BYTE) (playerNum + 1), tankNone);
      frontEndSetPlayerCheckState((playerNumbers) playerNum, FALSE);
    }
    /* Make a message about it */
    strcat(output, MESSAGE_QUOTES);
    strcat(output, name);
    strcat(output, MESSAGE_QUOTES);
    strcat(output, langGetText(MESSAGE_QUIT_GAME));
    messageAdd(newsWireMessage, langGetText(MESSAGE_NEWSWIRE), output);
  }
}

/*********************************************************
*NAME:          playersSetMenuItems
*AUTHOR:        John Morrison
*CREATION DATE: 23/3/99
*LAST MODIFIED: 23/3/99
*PURPOSE:
* Sets all the players in the players menu
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
void playersSetMenuItems(players *plrs) {
  BYTE count;                /* Looping variable */
  char str[512]; /* The player name */

  for (count=0;count<MAX_TANKS;count++) {
    if ((*plrs)->item[count].inUse == TRUE  ) {
      strcpy(str, (*plrs)->item[count].playerName);
      if (count != (*plrs)->myPlayerNum) {
        strcat(str, "@");
        strcat(str, (*plrs)->item[count].location);
      }
      if (threadsGetContext() == FALSE) {
        frontEndSetPlayer((playerNumbers) count, str);
      }
    }
  }
  if (threadsGetContext() == FALSE) {
    frontEndEnableRequestAllyMenu(FALSE);
    frontEndEnableLeaveAllyMenu(FALSE);
  }
}

/*********************************************************
*NAME:          playersGetNumAllies
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Returns the number of allies to your player. (Includes
* self
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
int playersGetNumAllies(players *plrs) {
  BYTE returnValue; /* Value to return */

  returnValue = allienceNumAllies(&((*plrs)->item[(*plrs)->myPlayerNum].allie)) + 1;
  return returnValue;
}


/*********************************************************
*NAME:          playersGetNumChecked
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Gets the number of players with the checked bytes set to
* TRUE
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
int playersGetNumChecked(players *plrs) {
  BYTE returnValue; /* Value to return */
  BYTE count; /* Looping variable */

  returnValue = 0;
  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && (*plrs)->item[count].isChecked == TRUE) {
      returnValue++;
    }
    count++;
  }
  return returnValue;
}

/*********************************************************
*NAME:          playersCheckAllies
*AUTHOR:        John Morrison
*CREATION DATE:  6/4/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Checks all your players allies bytes and sends frontend
* messages
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
void playersCheckAllies(players *plrs) {
  BYTE count; /* Looping variable */

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && (allienceExist(&((*plrs)->item[(*plrs)->myPlayerNum].allie), count) == TRUE || count == (*plrs)->myPlayerNum)) {
      (*plrs)->item[count].isChecked = TRUE;
    } else {
      (*plrs)->item[count].isChecked = FALSE;
    }
    if (threadsGetContext() == FALSE) {
      frontEndSetPlayerCheckState((playerNumbers) count, (*plrs)->item[count].isChecked);
    }
    count++;
  }
  playersSetAllieMenu(plrs);
}

/*********************************************************
*NAME:          playersCheckAllNone
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Checks/Unchecks all players allies bytes and sends 
* frontend messages
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* isChecked - TRUE if check all
*********************************************************/
void playersCheckAllNone(players *plrs, bool isChecked) {
  BYTE count;         /* Looping variable */

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE) {
      (*plrs)->item[count].isChecked = isChecked;
      if (threadsGetContext() == FALSE) {
        frontEndSetPlayerCheckState((playerNumbers) count, isChecked);
      }
    }
    count++;
  }
  playersSetAllieMenu(plrs);
}

/*********************************************************
*NAME:          playersToggleCheckedState
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Toggles the checked state of a player.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The number of the player to check
*********************************************************/
void playersToggleCheckedState(players *plrs, BYTE playerNum) {
  if (playerNum < MAX_TANKS) {
    if ((*plrs)->item[playerNum].inUse == TRUE  ) {
      if ((*plrs)->item[playerNum].isChecked == TRUE) {
        (*plrs)->item[playerNum].isChecked = FALSE;
      } else {
        (*plrs)->item[playerNum].isChecked = TRUE;
      }
      if (threadsGetContext() == FALSE) {
        frontEndSetPlayerCheckState((playerNumbers) playerNum, (*plrs)->item[playerNum].isChecked);
      }
    }
  }
  playersSetAllieMenu(plrs);
}

/*********************************************************
*NAME:          playersCheckNearbyPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Checks nearby players
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* xValue - Your tanks X Map position
* yValue - Your tanks Y Map position
*********************************************************/
void playersCheckNearbyPlayers(players *plrs, BYTE xValue, BYTE yValue) {
  int xDiff;  /* X and Y differences in location */
  int yDiff;
  BYTE count; /* Looping variable */

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE) {
      xDiff = xValue - (*plrs)->item[count].mapX;
      yDiff = yValue - (*plrs)->item[count].mapY;
      if (xDiff >= PLAYER_MAX_SELECT_LEFT && xDiff <= PLAYER_MAX_SELECT_RIGHT && yDiff >= PLAYER_MAX_SELECT_TOP && yDiff <= PLAYER_MAX_SELECT_BOTTOM) {
        (*plrs)->item[count].isChecked = TRUE;
      } else {
        (*plrs)->item[count].isChecked = FALSE;
      }
      if (threadsGetContext() == FALSE) {
        frontEndSetPlayerCheckState((playerNumbers) count, (*plrs)->item[count].isChecked);
      }
    }
    count++;
  }
  playersSetAllieMenu(plrs);
}

/*********************************************************
*NAME:          playersNumNearbyPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 6/4/99
*LAST MODIFIED: 6/4/99
*PURPOSE:
* Returns number of nearby players. (Includes self)
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* xValue - Your tanks X Map position
* yValue - Your tanks Y Map position
*********************************************************/
int playersNumNearbyPlayers(players *plrs, BYTE xValue, BYTE yValue) {
  int returnValue; /* Value to return */
  int xDiff;       /* X and Y differences in location */
  int yDiff;
  BYTE count;      /* Looping variable */

  count = 0;
  returnValue = 1;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE) {
      xDiff = xValue - (*plrs)->item[count].mapX;
      yDiff = yValue - (*plrs)->item[count].mapY;
      if (xDiff >= PLAYER_MAX_SELECT_LEFT && xDiff <= PLAYER_MAX_SELECT_RIGHT && yDiff >= PLAYER_MAX_SELECT_TOP && yDiff <= PLAYER_MAX_SELECT_BOTTOM) {
        returnValue++;
      }
    }
    count++;
  }
  
  return returnValue;
}

/*********************************************************
*NAME:          playersSendMessageAllAllies
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends message to all allies
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* message - Message to send
*********************************************************/
void playersSendMessageAllAllies(players *plrs, char *messageStr) {
  BYTE count; /* Looping variable */

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && allienceExist(&((*plrs)->item[(*plrs)->myPlayerNum].allie), count) == TRUE) {
      netMessageSendPlayer((*plrs)->myPlayerNum, count, messageStr);
    }
    count++;
  }
}

/*********************************************************
*NAME:          playersSendMessageAllSelected
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends message to all selected players
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* message - Message to send
*********************************************************/
void playersSendMessageAllSelected(players *plrs, char *messageStr) {
  char topLine[FILENAME_MAX]; /* The message topline */
  BYTE count;                 /* Looping variable */

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && (*plrs)->item[count].isChecked == TRUE) {
      if ((*plrs)->myPlayerNum == count) {
        /* Send self */
        topLine[0] = '\0';
        playersMakeMessageName(plrs, playersGetSelf(plrs), topLine);
        messageAdd((messageType) playersGetSelf(plrs), topLine, messageStr);
      } else {
        netMessageSendPlayer((*plrs)->myPlayerNum, count, messageStr);
      }

    }
    count++;
  }
}

/*********************************************************
*NAME:          playersSendMessageAllNearby
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 7/4/99
*PURPOSE:
* Sends message to all nearby players
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* xValue - Your tanks X Map position
* yValue - Your tanks Y Map position
* message - Message to send
*********************************************************/
void playersSendMessageAllNearby(players *plrs, BYTE xValue, BYTE yValue, char *messageStr) {
  int xDiff;       /* X and Y differences in location */
  int yDiff;
  BYTE count;      /* Looping variable */

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE) {
      xDiff = xValue - (*plrs)->item[count].mapX;
      yDiff = yValue - (*plrs)->item[count].mapY;
      if (xDiff >= PLAYER_MAX_SELECT_LEFT && xDiff <= PLAYER_MAX_SELECT_RIGHT && yDiff >= PLAYER_MAX_SELECT_TOP && yDiff <= PLAYER_MAX_SELECT_BOTTOM) {
        netMessageSendPlayer((*plrs)->myPlayerNum, count, messageStr);
      }
    }
    count++;
  }
}

/*********************************************************
*NAME:          playersIsInUse
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
* Returns whether a player Number is in use
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The player num to check
*********************************************************/
bool playersIsInUse(players *plrs, BYTE playerNumber) {
  return (*plrs)->item[playerNumber].inUse;
}

/*********************************************************
*NAME:          playersGetLgmDetails
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
* Gets the LGM details for a player
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The player num to check
* mx        - LGM Map X Position
* my        - LGM Map Y Position
* px        - LGM Pixel X Position
* py        - LGM Pixel Y Position
* frame     - LGM Frame
*********************************************************/
void playersGetLgmDetails(players *plrs, BYTE playerNumber, BYTE *mx, BYTE *my , BYTE *px, BYTE *py, BYTE *frame){
  if ((*plrs)->item[playerNumber].inUse == TRUE) {
    *mx = (*plrs)->item[playerNumber].lgmMapX;
    *my = (*plrs)->item[playerNumber].lgmMapY;
    *px = (*plrs)->item[playerNumber].lgmPixelX;
    *py = (*plrs)->item[playerNumber].lgmPixelY;
    *frame = (*plrs)->item[playerNumber].lgmFrame;
  } else {
    *mx = 0;
    *my = 0;
    *px = 0;
    *py = 0;
    *frame = 0;
  }
}

/*********************************************************
*NAME:          playersCheckCollision
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED:  4/11/99
*PURPOSE:
* Checks for a collision between our tank (given as 
* variables & the tanks in the players structure)
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The player num to check
* xValue  - Tank X World co-ordinate
* yValue  - Tank Y World co-ordinate
* leftPos - If left < 0 then hit from left > 0 from right
* downPos - If up < 0 then hit from above > 0 from below
*********************************************************/
bool playersCheckCollision(players *plrs, BYTE playerNum, WORLD xValue, WORLD yValue, int *leftPos, int *downPos) {
  bool returnValue; /* Value to return */
  bool count;       /* Looping Variable */
  WORLD conv;   /* Used for conversions */
  WORLD mx;     /* Tank map offsets */
  WORLD my;
  WORLD testX;
  WORLD testY;

  count = 0;
  *leftPos = 0;
  *downPos = 0;
  returnValue = FALSE;

  while (returnValue == FALSE && count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && count != playerNum) {
      /* Test for collision */
      conv = (*plrs)->item[count].mapX;
      conv <<= TANK_SHIFT_MAPSIZE;
      mx = conv;
      conv = (*plrs)->item[count].pixelX;
      conv <<= TANK_SHIFT_RIGHT2;
      mx += conv;
 
      conv = (*plrs)->item[count].mapY;
      conv <<= TANK_SHIFT_MAPSIZE;
      my = conv;
      conv = (*plrs)->item[count].pixelY;
      conv <<= TANK_SHIFT_RIGHT2;
      my += conv;
 
      if (mx > xValue) {
        testX = mx - xValue;
        *leftPos = -1;
      } else {
        testX = xValue - mx;
        *leftPos = 1;
      }
      if (testX < 128) {
        *leftPos = 0;
      }


      if (my > yValue) {
        testY = my - yValue;
        *downPos = 1;
      } else {
        testY = yValue - my;
        *downPos = -1;
      }
      if (testY < 128) {
        *downPos = 0;
      }

      if (testX < (256) && testY < (256)) {
        returnValue = TRUE;
      }    
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          playersSetAllieMenu
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/99
*LAST MODIFIED:  1/11/99
*PURPOSE:
* Determines whether the request and leave alliance menu
* items should be checked or not and passes it onto the
* frontend
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
void playersSetAllieMenu(players *plrs) {
  bool req;   /* Enable the request menu */
  bool leave; /* Enable the leave menu   */
  BYTE count; /* Looping variable */

  count = 0;
  req = FALSE;
  leave = FALSE;
  while (count < MAX_TANKS && leave == FALSE) {
    if ((*plrs)->item[count].inUse == TRUE && count != (*plrs)->myPlayerNum) {
      if (allienceExist(&((*plrs)->item[(*plrs)->myPlayerNum].allie), count) == TRUE) {
        leave = TRUE;
        req = FALSE;
      } else if ((*plrs)->item[count].isChecked == TRUE){
        req = TRUE;
      }
    }
    count++;
  }
  if (threadsGetContext() == FALSE) {
    frontEndEnableRequestAllyMenu(req);
    frontEndEnableLeaveAllyMenu(leave);
  }
}

/*********************************************************
*NAME:          playersRequestAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Process a request alliance request.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
void playersRequestAlliance(players *plrs) {
  BYTE count; /* Looping variable */

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && count != (*plrs)->myPlayerNum && (*plrs)->item[count].isChecked == TRUE) {
      if (allienceExist(&((*plrs)->item[(*plrs)->myPlayerNum].allie), count) == FALSE) {
        /* Place request */
        netRequestAlliance((*plrs)->myPlayerNum, count);
      }
    }
    count++;
  }
}

/*********************************************************
*NAME:          playersLeaveAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Process a leave alliance request.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - Player number that is leaving the alliance
*********************************************************/
void playersLeaveAlliance(players *plrs, BYTE playerNum) {
  BYTE count; /* Looping variable */
  BYTE total; /* Amount of items to redraw */
  bool found;
  count = 0;
  found = FALSE;
  while (count < MAX_TANKS && found == FALSE) {
    if (playerNum != count) {
      if (playersIsAllie(plrs, count, playerNum) == TRUE) {
        found = TRUE;
	  }
	}
    count++;
  }
  count--;

  screenBasesMigrate(playerNum,count);
  screenPillsMigratePlanted(playerNum,count);

  allienceDestroy(&((*plrs)->item[playerNum].allie));
  (*plrs)->item[playerNum].allie = allienceCreate();
  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && count != playerNum) {
       allienceRemove(&((*plrs)->item[count].allie), playerNum);
    }
    count++;
  }
 
  
  /* Update the screen */
  if (threadsGetContext() == FALSE) {
    total = screenNumBases();
    for (count=1;count<=total;count++) {
      frontEndStatusBase(count, screenBaseAlliance(count));
    }
    total = screenNumPills();
    for (count=1;count<=total;count++) {
      frontEndStatusPillbox(count, screenPillAlliance(count));
    }
    total = screenGetNumPlayers();
    for (count=1;count<=total;count++) {
      frontEndStatusTank(count, playersScreenAllience(plrs, (BYTE) (count-1)));
    }
    playersSetAllieMenu(plrs);
  }
}

/*********************************************************
*NAME:          playersAcceptAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  4/7/00
*PURPOSE:
* A player has been accepted into an alliance
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* acceptedBy - Who accepted them in
* newMember  - Who the new member is
*********************************************************/
void playersAcceptAlliance(players *plrs, BYTE acceptedBy, BYTE newMember) {
  BYTE count;   /* Looping variable */
  BYTE count2;   /* Looping variable */
  BYTE total;   /* Number of alliances acceptedBy has */
  unsigned long allyA; /* Alliances for a and b */
  unsigned long allyB;
  unsigned long test;
  unsigned long test2;


  allyA = playersGetAlliesBitMap(plrs, acceptedBy);
  allyB = playersGetAlliesBitMap(plrs, newMember);

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE) {
      test = (allyA >>count);
      test &= 1;
      if (test) {
        count2 = 0;
        while (count2 < MAX_TANKS) {
          test2 = (allyB >>count2);
          test2 &= 1;
          if (test2) {
            allienceAdd(&((*plrs)->item[count].allie), count2);
          }
          count2++;
        }
      }
      test = (allyB >>count);
      test &= 1;
      if (test) {
        count2 = 0;
        while (count2 < MAX_TANKS) {
          test2 = (allyA >>count2);
          test2 &= 1;
          if (test2) {
            allienceAdd(&((*plrs)->item[count].allie), count2);
          }
          count2++;
        }
      }
    }
    count++;
  }
      
      
 
  /* Update the screen */
  if (threadsGetContext() == FALSE) {
    total = screenNumBases();
    for (count=1;count<=total;count++) {
      frontEndStatusBase(count, screenBaseAlliance(count));
    }
    total = screenNumPills();
    for (count=1;count<=total;count++) {
      frontEndStatusPillbox(count, screenPillAlliance(count));
    }
    total = screenGetNumPlayers();
    for (count=1;count<=total;count++) {
      frontEndStatusTank(count, playersScreenAllience(plrs, (BYTE) (count-1)));
    }
    playersSetAllieMenu(plrs);
  }
}


/*********************************************************
*NAME:          playersConnectionLost
*AUTHOR:        John Morrison
*CREATION DATE: 2/11/99
*LAST MODIFIED: 2/11/99
*PURPOSE:
* Called if your connection is lost to a network game.
* Drops all other players except yourself.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The number of the player that has left
*********************************************************/
void playersConnectionLost(players *plrs) {
  BYTE count; /* Looping variable */
  BYTE total;   /* Number of alliances acceptedBy has */
  BYTE current; /* Current Allie we are working on  */


  /* Move allies stuff to us */
  count = 0;
  total = allienceNumAllies(&((*plrs)->item[(*plrs)->myPlayerNum].allie));
  while (count < total) {
    current = allienceReturnNum(&((*plrs)->item[(*plrs)->myPlayerNum].allie), count);
    screenChangeOwnership(current);
    count++;
  }

  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && count != (*plrs)->myPlayerNum) {
      playersLeaveGame(plrs, count);
    }
    count++;
  }
}


/*********************************************************
*NAME:          playersGetBrainTanksInRect
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 9/1/00
*PURPOSE:
*  Makes the brain tank info for each tank inside the
*  rectangle formed by the function parameters.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* leftPos   - Left position of rectangle
* rightPos  - Right position of rectangle
* top    - Top position of rectangle
* bottom - Bottom position of rectangle
* tankX  - Our tanks X position
* tankY  - Our tanks Y position
*********************************************************/
void playersGetBrainTanksInRect(players *plrs, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom, WORLD tankX, WORLD tankY) {
  BYTE count;      /* Looping variable */
  WORLD conv;      /* Used in converting items world co-ordinates */
  WORLD wx;        /* Items X and Y positions */
  WORLD wy;
  WORLD diffX;     /* Tanks differences in position */
  WORLD diffY;
  BYTE owner;      /* Owner of the tank */

  count = 0;

/* typedef struct
	{
	OBJECT object; = 0
	WORLD_X x;
	WORLD_Y y;
	WORD idnum;
	BYTE direction;
	BYTE info;
	} ObjectInfo;
*/

  while (count<MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && count != (*plrs)->myPlayerNum) {   
      /* X Position */
      wx = (*plrs)->item[count].mapX;
      wx <<= TANK_SHIFT_MAPSIZE;
      conv = (*plrs)->item[count].pixelX;
      conv <<= TANK_SHIFT_RIGHT2;
      wx += conv;
      /* Y Position */
      wy = (*plrs)->item[count].mapY;
      wy <<= TANK_SHIFT_MAPSIZE;
      conv = (*plrs)->item[count].pixelY;
      conv <<= TANK_SHIFT_RIGHT2;
      wy += conv;
      /* Difference for tree check */
      if (wx > tankX) {
        diffX = wx - tankX;
      } else {
        diffX = tankX - wx;
      }
      if (wy > tankY) {
        diffY = wy - tankY;
      } else {
        diffY = tankY - wy;
      }
      
      
      if ((*plrs)->item[count].mapX >= leftPos && (*plrs)->item[count].mapX <= rightPos && (*plrs)->item[count].mapY >= top && (*plrs)->item[count].mapY <= bottom && (screenIsItemInTrees(wx, wy) == FALSE || (diffX < MIN_TREEHIDE_DIST && diffY < MIN_TREEHIDE_DIST))) {
        /* In the rectangle */  
        /* wx and wy already set */
        /* Info */
        if (allienceExist(&((*plrs)->item[count].allie), (*plrs)->myPlayerNum) == TRUE) {
          owner = PLAYERS_BRAIN_FRIENDLY;
        } else {
          owner = PLAYERS_BRAIN_HOSTILE;
        }
        screenAddBrainObject(PLAYERS_BRAIN_OBJECT_TYPE_TANK, wx, wy, count, (*plrs)->item[count].frame, owner);
      }
    }
    count++;
  }
}


/*********************************************************
*NAME:          playersGetBrainLgmsInRect
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 09/08/04
*PURPOSE:
*  Makes the brain lgm info for each lgm inside the
*  rectangle formed by the function parameters.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* left   - Left position of rectangle
* rightPos  - Right position of rectangle
* topPos    - Top position of rectangle
* bottom - Bottom position of rectangle
*********************************************************/
void playersGetBrainLgmsInRect(players *plrs, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  BYTE count;      /* Looping variable */
  WORLD conv;      /* Used in converting items world co-ordinates */
  WORLD wx;        /* Items world positions */
  WORLD wy;
  WORLD conv2;
  WORLD ourTankX;
  WORLD ourTankY;
  BYTE lgmType;    /* Type of lgm this is */
  BYTE owner;      /* Owner of the lgm    */

  count = 0;

/* typedef struct
	{
	OBJECT object; = 0
	WORLD_X x;
	WORLD_Y y;
	WORD idnum;
	BYTE direction;
	BYTE info;
	} ObjectInfo;
*/

  while (count<MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && count != (*plrs)->myPlayerNum) {
      if ((*plrs)->item[count].lgmMapX >= leftPos && (*plrs)->item[count].lgmMapX <= rightPos && (*plrs)->item[count].lgmMapY >= top && (*plrs)->item[count].lgmMapY <= bottom) {
        /* In trees check */
        wx = (*plrs)->item[count].lgmMapX << TANK_SHIFT_MAPSIZE;
        wx += (*plrs)->item[count].lgmPixelX << TANK_SHIFT_RIGHT2;
        wy = (*plrs)->item[count].lgmMapY << TANK_SHIFT_MAPSIZE;
        wy += (*plrs)->item[count].lgmPixelY << TANK_SHIFT_RIGHT2;
        screenGetTankWorld(&ourTankX, &ourTankY);
        if (wx > ourTankX) {
          conv = wx - ourTankX;
        } else {
          conv = ourTankX - wx;
        }
        if (wy > ourTankY) {
          conv2 = wy - ourTankY;
        } else {
          conv2 = ourTankY - wy;
        }
        
        if ((*plrs)->item[count].lgmFrame == LGM_HELICOPTER_FRAME || (screenIsItemInTrees(wx, wy) == FALSE || (conv < MIN_TREEHIDE_DIST && conv2 < MIN_TREEHIDE_DIST))) {
          /* In the rectangle */  
          /* Object Type */
          if ((*plrs)->item[count].lgmFrame == LGM_HELICOPTER_FRAME) {
            lgmType = PLAYERS_BRAIN_OBJECT_TYPE_PARA;
          } else {
            lgmType = PLAYERS_BRAIN_OBJECT_TYPE_LGM;
          }
          /* X Position */
          wx = (*plrs)->item[count].lgmMapX;
          wx <<= TANK_SHIFT_MAPSIZE;
          conv = (*plrs)->item[count].lgmPixelX;
          conv <<= TANK_SHIFT_RIGHT2;
          wx += conv;
          /* Y Position */
          wy = (*plrs)->item[count].lgmMapY;
          wy <<= TANK_SHIFT_MAPSIZE;
          conv = (*plrs)->item[count].lgmPixelY;
          conv <<= TANK_SHIFT_RIGHT2;
          wy += conv;
          /* Info */
          if (allienceExist(&((*plrs)->item[count].allie), (*plrs)->myPlayerNum) == TRUE) {
            owner = PLAYERS_BRAIN_FRIENDLY;
          } else {
            owner = PLAYERS_BRAIN_HOSTILE;
          }
          screenAddBrainObject(lgmType, wx, wy, count, 0, owner);
        }
      }
    }
    count++;
  }
}

/*********************************************************
*NAME:          playersGetBrainsNamesArray
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Gets the players brain name array.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
*********************************************************/
u_char36 **playersGetBrainsNamesArray(players *plrs) {
  return (u_char36 **) &((*plrs)->playerBrainNames);
}

/*********************************************************
*NAME:          playersGetAlliesBitMap
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED:   4/7/00
*PURPOSE:
*  Returns the playerBitMap of all players that are allied
*  to us.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - Player number to get for
*********************************************************/
unsigned long playersGetAlliesBitMap(players *plrs, BYTE playerNum) {
  unsigned long returnValue; /* Value to return */
  BYTE count;                /* Looping variable */

  returnValue = 0;
  count = 0;

  while (count<MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE) {
      if (count == playerNum || allienceExist(&((*plrs)->item[count].allie), playerNum) == TRUE) {
        returnValue |= 1 << count;
      }
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          playersSendAiMessage
*AUTHOR:        John Morrison
*CREATION DATE: 12/12/99
*LAST MODIFIED: 12/12/99
*PURPOSE:
*  Called when a brain wishes to send a message to players
*  in the game.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* bitMap     - The checked and unchecked player bitMap
* messageStr - The message to be sent.
*********************************************************/
void playersSendAiMessage(players *plrs, unsigned long bitMap, char *messageStr) {
  char topLine[FILENAME_MAX]; /* The message topline */
  BYTE count; /* Looping variable */
  unsigned long test;

  count = 0;
  while (count < MAX_TANKS) {
    test = (bitMap >>count);
    test &= 1;
    if ((*plrs)->item[count].inUse == TRUE && test) {
      if (count == (*plrs)->myPlayerNum) {
        topLine[0] = '\0';
        playersMakeMessageName(plrs, (*plrs)->myPlayerNum, topLine);
        messageAdd((messageType) (*plrs)->myPlayerNum, topLine, messageStr);
      } else {
        netMessageSendPlayer((*plrs)->myPlayerNum, count, messageStr);
      }
    }
    count++;
  }
}

/*********************************************************
*NAME:          playersPrepareLogSnapshotForPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 25/07/04
*LAST MODIFIED: 25/07/04
*PURPOSE:
* Prepares a single player log snapshot. Returns if the
* player spot is in use
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - Player to get snapshot for
* buff      - Destination buffer
* len       - Length of the buffer
*********************************************************/
bool playersPrepareLogSnapshotForPlayer(players *value, BYTE playerNum, BYTE *buff, BYTE *len) {
  bool returnValue;
  tank *tnk;
  lgm *lgm;

  returnValue = (*value)->item[playerNum].inUse;
  buff[0] = playerNum;
  buff[1] = returnValue;
  *len = 2;
  tnk = serverCoreGetTankFromPlayer(playerNum);
  lgm = serverCoreGetLgmFromPlayerNum(playerNum);
  if (returnValue == TRUE && tnk != NULL && lgm != NULL) {
    /* Set MX/MY/PX/PY/frame/onBoat/lgmMX/lgmMY/lgmPX/lgmPX/frame/Name/Location */
    buff[2] = tankGetMX(tnk);
    buff[3] = tankGetMY(tnk);
    buff[4] = utilPutNibble(tankGetPX(tnk), tankGetPY(tnk));
    buff[5] = tankGetFrame(tnk);
    buff[6] = tankIsOnBoat(tnk);
    buff[7] = lgmGetMX(lgm);
    buff[8] = lgmGetMY(lgm);
    buff[9] = utilPutNibble(lgmGetPX(lgm), lgmGetPY(lgm));
    buff[10] = lgmGetFrame(lgm);
    *len = 11;
    utilCtoPString((*value)->item[playerNum].playerName, buff+11);
    *len += buff[11]+1; /* Add 1 for len prefix */
    utilCtoPString((*value)->item[playerNum].location, buff+*len);
    *len += *(buff+*len) + 1;
    *len += allianceMakeLogAlliance(&((*value)->item[playerNum].allie), buff+*len);
  }
  return returnValue;
}

void playersCheckUpdate(players *plrs, BYTE playerNum) {
  if ((*plrs)->item[playerNum].inUse == TRUE) {
    (*plrs)->item[playerNum].mapX = 0;
    (*plrs)->item[playerNum].mapY = 0;
    (*plrs)->item[playerNum].needUpdate = TRUE;
  }
}

bool playersNeedUpdate(players *plrs, BYTE playerNum) {
  if ((*plrs)->item[playerNum].inUse == TRUE && (*plrs)->item[playerNum].needUpdate == TRUE) {
    (*plrs)->item[playerNum].needUpdate = FALSE;
    return TRUE;
  }
  return FALSE;
}

void playerNeedUpdateDone(players *plrs) {
  BYTE count;
  count = 0;
  while (count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE) {
      (*plrs)->item[count].needUpdate = FALSE;
    }
    count++;
  }
}

/*********************************************************
*NAME:          playersCheckSameSquare
*AUTHOR:        Minhiriath
*CREATION DATE: 13/3/2009
*LAST MODIFIED: 13/3/2009
*PURPOSE:
* Checks to see if our tank is in the same square as any other tank.
*
*ARGUMENTS:
* plrs - Pointer to the players object 
* playerNum - The player num to check
* xValue  - Tank X Map co-ordinate
* yValue  - Tank Y Map co-ordinate
*********************************************************/
bool playersCheckSameSquare(players *plrs, BYTE playerNum, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */
  bool count;       /* Looping Variable */
  WORLD conv;   /* Used for conversions */
  WORLD mx;     /* Tank map offsets */
  WORLD my;
  WORLD testX;
  WORLD testY;

  count = 0;
  returnValue = FALSE;

  while (returnValue == FALSE && count < MAX_TANKS) {
    if ((*plrs)->item[count].inUse == TRUE && count != playerNum) {
      /* Test for collision */
      conv = (*plrs)->item[count].mapX;
      conv <<= TANK_SHIFT_MAPSIZE;
      mx = conv;
	  
      conv = (*plrs)->item[count].mapY;
      conv <<= TANK_SHIFT_MAPSIZE;
      my = conv;

	  if(xValue == mx && yValue == my)
	  {
		  returnValue = TRUE;
	  }
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          playersSetMyLastPlayerName
*AUTHOR:        Chris Lesnieski
*CREATION DATE: 14/02/09
*LAST MODIFIED: 14/02/09
*PURPOSE:
* Will copy a string to the myLastPlayerName variable.
* This should be done on initial start-up of WinBolo when
* it loads the winbolo.ini file.
*
*ARGUMENTS:
* dest       - Name to be set as player's previous name 
*********************************************************/
void playersSetMyLastPlayerName(char *dest)
{
  strcpy(myLastPlayerName, dest);
}
