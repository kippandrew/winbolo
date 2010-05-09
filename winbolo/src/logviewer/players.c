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
*Creation Date: 18/02/99
*Last Modified: 05/05/01
*Purpose:
*  Looks after players. Alliences between etc.
*********************************************************/

#include <string.h>
#include "global.h"
#include "allience.h"
#include "labels.h"
#include "screentank.h"
#include "screenlgm.h"
#include "backend.h"
#include "tilenum.h"
#include "util.h"
#include "players.h"
#include "bolo_map.h"


players plrs;
BYTE myPlayerNum = 0; /* Your own player number */

char testP[256];

void playersSetSelf(BYTE pn) {
  myPlayerNum = pn;
}

/*********************************************************
I*NAME:          playersCreate
*AUTHOR:        John Morrison
*CREATION DATE: 18/2/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Sets up the players structure.
*
*ARGUMENTS:
*
*********************************************************/
void playersCreate() {
  BYTE count; /* Looping variable */

  myPlayerNum = NEUTRAL; /* Not set yet */
  for (count = 0;count<MAX_TANKS;count++) {
    plrs.item[count].inUse = FALSE;
    plrs.item[count].needUpdate = FALSE;
    plrs.item[count].allie = allienceCreate();
    plrs.item[count].isChecked = FALSE;  
    plrs.item[count].team = NO_TEAM_SET;
  }
  testP[0] = '\0';
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
*
*********************************************************/
void playersDestroy() {
  BYTE count; /* Looping variable */

  for (count = 0;count<MAX_TANKS;count++) {
    plrs.item[count].inUse = FALSE;
    allienceDestroy(&(plrs.item[count].allie));
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
*
*********************************************************/
BYTE playersGetSelf() {
  return myPlayerNum;
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
*  playerNum  - The player number to set
*  playerName - The player name to set
*********************************************************/
bool playersSetPlayerName(BYTE playerNum, char *playerName) {
  bool returnValue;           /* Value to return */
  char messageStr[FILENAME_MAX]; /* Newswire Message */
  char label[FILENAME_MAX];   /* Used to hold the string made by label */
  
  messageStr[0] = '\0';
  label[0] = '\0';
  returnValue = FALSE;
  plrs.item[playerNum].inUse = TRUE;
  /* Make Message */
  strcat(messageStr, "\"");
  labelMakeMessage(label, plrs.item[playerNum].playerName, plrs.item[playerNum].location);
  strcat(messageStr, label);
  strcat(messageStr, MESSAGE_QUOTES);
  strcat(messageStr, MESSAGE_CHANGENAME);
  strcat(messageStr, playerName);
  strcat(messageStr, MESSAGE_QUOTES);
  windowAddEvent(0, messageStr);

  /* Update the name */
  strcpy(plrs.item[playerNum].playerName, playerName);
  return returnValue;
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
*  playerNum  - The player number to set
*  playerName - The player name to set
*  location   - Location string of the player.
*  mx         - Map X location
*  my         - Map Y location
*  px         - Pixel X location
*  py         - Pixel Y location
*  frame      - Tank animation frame.
*  onBoat     - is the player on a boat?
*  numAllies  - Number of Allies the player has
*  allies     - BYTE buffer containing each allie
*  announce   - Do we want to announce this fact?
*  override   - Override if the game is fastfowarding
*********************************************************/
void playersSetPlayer(BYTE playerNum, char *playerName, char *location, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, bool onBoat, BYTE numAllies, BYTE *allies, bool announce, bool override) {
  BYTE count; /* Looping variable */
  char str[512]; /* The player name */

  if (plrs.item[playerNum].inUse == FALSE || override == TRUE) {
    
    if (override == FALSE || plrs.item[playerNum].inUse == FALSE) {
      /* onlysetup team if not overridding for rewind */
      plrs.item[playerNum].team = playersGetUnusedTeam(playerNum);
      allienceDestroy(&(plrs.item[playerNum].allie));
    }
    plrs.item[playerNum].inUse = TRUE;
    strcpy(plrs.item[playerNum].playerName, playerName);
    strcpy(plrs.item[playerNum].location, location);
    plrs.item[playerNum].mapX = mx;
    plrs.item[playerNum].mapY = my;
    plrs.item[playerNum].pixelX = px;
    plrs.item[playerNum].pixelY = py;
    if (frame > 15) {
      frame -= 16;
    }
    plrs.item[playerNum].frame = frame;
    plrs.item[playerNum].onBoat = onBoat;
    
    plrs.item[playerNum].allie = allienceCreate();
    count = 0;
    while (count < numAllies) {
      allienceAdd(&(plrs.item[playerNum].allie), allies[count]);
      count++;
    }
  }

  if (announce == TRUE) {
    strcpy(str, plrs.item[playerNum].playerName);
    strcat(str, "@");
    strcat(str, plrs.item[playerNum].location);
    strcat(str, " has joined game");
    windowAddEvent(0, str);
  }

//  frontEndSetPlayer((playerNumbers) playerNum, str);
//  frontEndStatusTank((BYTE) (playerNum+1), playersScreenAllience(playerNum));
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
*  playerNum  - The player number to set
*  mx         - Map X location
*  my         - Map Y location
*  px         - Pixel X location
*  py         - Pixel Y location
*  frame      - Tank animation frame.
*  onBoat     - is the player on a boat?
*  lgmMX      - Lgm Map X Position
*  lgmMY      - Lgm Map Y Position
*  lgmPX      - Lgm Map X Position
*  lgmPY      - Lgm Map Y Position
*  lgmFrame   - Lgm Frame number
*********************************************************/
void playersUpdateTank(BYTE playerNum, BYTE mx, BYTE my, BYTE px, BYTE py, BYTE frame, bool onBoat) {
 if (plrs.item[playerNum].inUse == TRUE) {
    plrs.item[playerNum].mapX = mx;
    plrs.item[playerNum].mapY = my;
    plrs.item[playerNum].pixelX = px;
    plrs.item[playerNum].pixelY = py;
    plrs.item[playerNum].frame = frame;
    plrs.item[playerNum].onBoat = onBoat;
  }
}


void playersUpdateLgm(BYTE playerNum, BYTE lgmMX, BYTE lgmMY, BYTE lgmPX, BYTE lgmPY, BYTE lgmFrame) {
  if (plrs.item[playerNum].inUse == TRUE) {
    plrs.item[playerNum].lgmMapX = lgmMX;
    plrs.item[playerNum].lgmMapY = lgmMY;
    plrs.item[playerNum].lgmPixelX = lgmPX;
    plrs.item[playerNum].lgmPixelY = lgmPY;
    plrs.item[playerNum].lgmFrame = lgmFrame;
  }
}

void playersLgmZero() {
  BYTE count;
  for (count=0;count<MAX_TANKS;count++) {
    if (plrs.item[count].inUse == TRUE) {
 //     plrs.item[count].mapX = 0;
 //     plrs.item[count].mapY = 0;
      plrs.item[count].lgmMapX = 0;
      plrs.item[count].lgmMapY = 0;
    }
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
*  playerNum  - The player number to set
*  dest       - Destination string
*********************************************************/
void playersGetPlayerName(BYTE playerNum, char *dest) {
  if (plrs.item[playerNum].inUse == TRUE) {
    strcpy(dest, plrs.item[playerNum].playerName);
  } else {
    strcpy(dest, NO_TANK);
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
*  playerNum  - The player number to set
*  dest       - Destination string
*********************************************************/
void playersMakeMessageName(BYTE playerNum, char *dest) {
  char label[FILENAME_MAX];   /* Used to hold the string made by label */
  
  label[0] = '\0';
  labelMakeMessage(label, plrs.item[playerNum].playerName, plrs.item[playerNum].location);
  strcpy(dest, label);
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
*  playerNum  - The player number to set
*  dest       - Destination string
*********************************************************/
void playersMakeScreenName(BYTE playerNum, char *dest) {
  char label[FILENAME_MAX];   /* Used to hold the string made by label */
  
  label[0] = '\0';
  if (plrs.item[playerNum].inUse == TRUE) {
    labelMakeTankLabel(label, plrs.item[playerNum].playerName, plrs.item[playerNum].location, FALSE);
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
*  playerA - The player number to check
*  playerB  - The player number to check
*********************************************************/
bool playersIsAllie(BYTE playerA, BYTE playerB) {
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
    if (plrs.item[playerA].inUse == TRUE) {
      check = playerA;
      check2 = playerB;
    } else if (plrs.item[playerB].inUse == TRUE) {
      check = playerB;
      check2 = playerA;
    }
    
    if (check != NEUTRAL) {
      returnValue = allienceExist(&(plrs.item[check].allie), check2);
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
*  playerNum - The player number to check
*********************************************************/
BYTE playersGetNumAllie(BYTE playerNum) {
  BYTE returnValue; /* Value to return */

  returnValue = 0;
  if (plrs.item[playerNum].inUse == TRUE) {
    returnValue = allienceNumAllies(&(plrs.item[playerNum].allie)) + 1;
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
*  playerNum - Player number to check
*********************************************************/
tankAlliance playersScreenAllience(BYTE playerNum) {
  tankAlliance returnValue; /* Value to return */

  returnValue = tankNone;
  if (playerNum < MAX_TANKS) {
    if (plrs.item[playerNum].inUse == FALSE) {
      returnValue = tankNone;
    } else if (playerNum == myPlayerNum) {
      returnValue = tankSelf;
    } else if (allienceExist(&(plrs.item[myPlayerNum].allie), playerNum) == TRUE) {
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
*  value    - screen tanks data structure
*  leftPos  - Left bound
*  rightPos - Left bound
*  top      - top bound
*  bottom   - Bottom bound
*********************************************************/
void playersMakeScreenTanks(screenTanks *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  char playerName[FILENAME_MAX]; /* Holds playername/location info */
  WORLD conv;                    /* Used in conversion */
  WORLD conv2;
  BYTE frame;                    /* Holds frame info */
  BYTE count;                    /* Looping variable */
  BYTE mx;                       /* Tank map and pixel X and Y co-ordinates */
  BYTE my;
  BYTE px;
  BYTE py;
  //char str[256];

  
  for (count=0;count<MAX_TANKS;count++) {
    if (plrs.item[count].inUse == TRUE) {
      playerName[0] = '\0';
      /* Extract fixed map co-ordinates */

      conv = (plrs.item[count].mapX << 8) + (plrs.item[count].pixelX << 4);
      conv -= 128;
      mx = (conv >> 8);
      px = (conv & 0xF0) >> 4;

      conv2 = (plrs.item[count].mapY << 8) + (plrs.item[count].pixelY << 4);
      conv2 -= 128;
      my = (conv2 >> 8);
      py = (conv2 & 0xF0) >> 4;

      //      sprintf(str, "Player at (%d, %d)", mx, my);

      if (mx >= leftPos && mx <= rightPos && my >= top && my <= bottom) {
        /* Extract player screen name */
        playersMakeScreenName(count, playerName);
        frame = plrs.item[count].frame;
        if (plrs.item[count].onBoat == TRUE) {
          frame += TANK_BOAT_ADD;
        }
        if (myPlayerNum != count) {
          if (allienceExist(&(plrs.item[count].allie), myPlayerNum) == TRUE) {
            frame += TANK_GOOD_ADD;
          } else {
            frame += TANK_EVIL_ADD;
          }
        }

/*        if (count == 0) {
          sprintf(playerName, "mx=%d, my=%d, px=%d, py=%d", (BYTE) (mx-leftPos), (BYTE) (my-top), px, py);
            if (strcmp(playerName, testP) != 0) {
              windowAddEvent(0, playerName);
              strcpy(testP, playerName);
            }
        } */
        
        screenTanksAddItem(value,(BYTE) (mx - leftPos), (BYTE) (my - top), px, py, frame, plrs.item[count].team, plrs.item[count].frame, plrs.item[count].onBoat, playerName); 
      } else if (count == 0) {
/*        sprintf(playerName, "NIF: mx=%d, my=%d, l=%d, r=%d, t=%d, b=%d", mx, my, leftPos, rightPos, top, bottom);
            if (strcmp(playerName, testP) != 0) {
              windowAddEvent(0, playerName);
              strcpy(testP, playerName);
              finished();
            }
*/
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
*  value    - screen LGM data structure
*  leftPos  - Left bound
*  rightPos - Left bound
*  top      - top bound
*  bottom   - Bottom bound
*********************************************************/
void playersMakeScreenLgm(screenLgm *value, BYTE leftPos, BYTE rightPos, BYTE top, BYTE bottom) {
  WORLD wx;
  WORLD wy;
  BYTE count;                    /* Looping variable */

  for (count=0;count<MAX_TANKS;count++) {
    if (plrs.item[count].inUse == TRUE) {
      if (plrs.item[count].lgmMapX >= leftPos && plrs.item[count].lgmMapX <= rightPos && plrs.item[count].lgmMapY >= top && plrs.item[count].lgmMapY <= bottom) {
        wx = plrs.item[count].lgmMapX << TANK_SHIFT_MAPSIZE;
        wx += plrs.item[count].lgmPixelX << TANK_SHIFT_RIGHT2;
        wy = plrs.item[count].lgmMapY << TANK_SHIFT_MAPSIZE;
        wy += plrs.item[count].lgmPixelY << TANK_SHIFT_RIGHT2;
        
        screenLgmAddItem(value,(BYTE) (plrs.item[count].lgmMapX - leftPos), (BYTE) (plrs.item[count].lgmMapY - top), plrs.item[count].lgmPixelX, plrs.item[count].lgmPixelY, plrs.item[count].lgmFrame); 
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
*
*********************************************************/
BYTE playersGetNumPlayers() {
  BYTE returnValue; /* Value to return */
  BYTE count;       /* Looping variable */

  returnValue = 0;
  for (count=0;count<MAX_TANKS;count++) {
    if (plrs.item[count].inUse == TRUE) {
      returnValue++;
    }
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
*  playerNum - The number of the player that has left
*  announce  - Whether to announce this or not
*********************************************************/
void playersLeaveGame(BYTE playerNum, bool announce) {
  char name[FILENAME_MAX];   /* The Player Name */
  char output[FILENAME_MAX]; /* The message */
  BYTE count;                /* Looping variable */


  if (plrs.item[playerNum].inUse == TRUE) {
    allienceDestroy(&(plrs.item[playerNum].allie));
    count = 0;
    while (count < MAX_TANKS) {
      if (plrs.item[playerNum].inUse == TRUE && count != playerNum) {
        allienceRemove(&(plrs.item[count].allie), playerNum);
      }
      count++;
    }
    plrs.item[playerNum].inUse = FALSE;
    plrs.item[playerNum].needUpdate = FALSE;
    plrs.item[playerNum].isChecked = FALSE;
    plrs.item[playerNum].team = NO_TEAM_SET;

    if (announce == TRUE) {
      output[0] = '\0';
      name[0] = '\0';
      playersMakeMessageName(playerNum, name);
  //    frontEndClearPlayer((playerNumbers) playerNum);
  //    frontEndStatusTank((BYTE) (playerNum + 1), tankNone);
      /* Make a message about it */
      strcat(output, MESSAGE_QUOTES);
      strcat(output, name);
      strcat(output, MESSAGE_QUOTES);
      strcat(output, MESSAGE_QUIT_GAME);
      windowAddEvent(0, output);
    }
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
*  playerNum - The player num to check
*********************************************************/
bool playersIsInUse(BYTE playerNumber) {
  return plrs.item[playerNumber].inUse;
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
*  playerNum - The player num to check
*  mx        - LGM Map X Position
*  my        - LGM Map Y Position
*  px        - LGM Pixel X Position
*  py        - LGM Pixel Y Position
*  frame     - LGM Frame
*********************************************************/
void playersGetLgmDetails(BYTE playerNumber, BYTE *mx, BYTE *my , BYTE *px, BYTE *py, BYTE *frame){
  if (plrs.item[playerNumber].inUse == TRUE) {
    *mx = plrs.item[playerNumber].lgmMapX;
    *my = plrs.item[playerNumber].lgmMapY;
    *px = plrs.item[playerNumber].lgmPixelX;
    *py = plrs.item[playerNumber].lgmPixelY;
    *frame = plrs.item[playerNumber].lgmFrame;
  } else {
    *mx = 0;
    *my = 0;
    *px = 0;
    *py = 0;
    *frame = 0;
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
*  playerNum - Player number that is leaving the alliance
*********************************************************/
void playersLeaveAlliance(BYTE playerNum) {
  BYTE count; /* Looping variable */
  BYTE total; /* Amount of items to redraw */

  allienceDestroy(&(plrs.item[playerNum].allie));
  plrs.item[playerNum].allie = allienceCreate();
  plrs.item[playerNum].team = playersGetUnusedTeam(playerNum);
  count = 0;
  while (count < MAX_TANKS) {
    if (plrs.item[count].inUse == TRUE && count != playerNum) {
       allienceRemove(&(plrs.item[count].allie), playerNum);
    }
    count++;
  }
 
  
  /* Update the screen */
  total = screenNumBases();
  for (count=1;count<=total;count++) {
//    frontEndStatusBase(count, screenBaseAlliance(count));
  }
  total = screenNumPills();
  for (count=1;count<=total;count++) {
  //  frontEndStatusPillbox(count, screenPillAlliance(count));
  }
  total = playersGetNumPlayers();
  for (count=1;count<=total;count++) {
//    frontEndStatusTank(count, playersScreenAllience((BYTE) (count-1)));
  }
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
*  playerNum - Player number to get for
*********************************************************/
unsigned long playersGetAlliesBitMap(BYTE playerNum) {
  unsigned long returnValue; /* Value to return */
  BYTE count;                /* Looping variable */

  returnValue = 0;
  count = 0;

  while (count<MAX_TANKS) {
    if (plrs.item[count].inUse == TRUE) {
      if (count == playerNum || allienceExist(&(plrs.item[count].allie), playerNum) == TRUE) {
        returnValue |= 1 << count;
      }
    }
    count++;
  }

  return returnValue;
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
*  acceptedBy - Who accepted them in
*  newMember  - Who the new member is
*********************************************************/
void playersAcceptAlliance(BYTE acceptedBy, BYTE newMember) {
  BYTE count;   /* Looping variable */
  BYTE count2;   /* Looping variable */
  BYTE total;   /* Number of alliances acceptedBy has */
  unsigned long allyA; /* Alliances for a and b */
  unsigned long allyB;
  unsigned long test;
  unsigned long test2;


  allyA = playersGetAlliesBitMap(acceptedBy);
  allyB = playersGetAlliesBitMap(newMember);

  // plrs.item[acceptedBy].team = plrs.item[newMember].team;

  count = 0;
  // For all possible tanks..
  while (count < MAX_TANKS) {
		// If this player is in use..
    if (plrs.item[count].inUse == TRUE) {
			// A's allys shifted right 'count' bits..
      test = (allyA >>count);
			// AND this with right-most bit.
      test &= 1;
			// If 'count' is allied with A (I think)
      if (test) {
        count2 = 0;
				// For all tanks..
        while (count2 < MAX_TANKS) {
          test2 = (allyB >>count2);
          test2 &= 1;
					// If B is allied with 'count2' [AND A is allied with 'count']
          if (test2) {
						// Add an alliance between count and count2.
            allienceAdd(&(plrs.item[count].allie), count2);
						// So, put all allies of the 'accepter', on the team of the 'requester'.
						plrs.item[count].team = plrs.item[count2].team;
          }
          count2++;
        }
      }
      test = (allyB >>count);
      test &= 1;
			// If B is allied with 'count'..
      if (test) {
        count2 = 0;
        while (count2 < MAX_TANKS) {
          test2 = (allyA >>count2);
          test2 &= 1;
					// If A is allied with 'count2' [AND B is allied with 'count']
          if (test2) {
						// Add an alliance between count and count2.
            allienceAdd(&(plrs.item[count].allie), count2);
          }
          count2++;
        }
      }
    }
    count++;
  }
      
      
 
  /* Update the screen */
  total = screenNumBases();
  for (count=1;count<=total;count++) {
//    frontEndStatusBase(count, screenBaseAlliance(count));
  }
  total = screenNumPills();
  for (count=1;count<=total;count++) {
//    frontEndStatusPillbox(count, screenPillAlliance(count));
  }
  total = playersGetNumPlayers();
  for (count=1;count<=total;count++) {
//    frontEndStatusTank(count, playersScreenAllience((BYTE) (count-1)));
  }
}

bool playersChooseView(int x, int y) {
  BYTE count = 0;
  bool done = FALSE;
  while (count < MAX_TANKS && done == FALSE) {
    if (plrs.item[count].inUse == TRUE) {
      if (plrs.item[count].mapX == x && plrs.item[count].mapY == y) {
        done = TRUE;
        myPlayerNum = count;
        return TRUE;
      }
    }
    count++;
  }
  return FALSE;
}

BYTE playersGetCentredX() {
  if (plrs.item[myPlayerNum].inUse == TRUE && plrs.item[myPlayerNum].mapX >= MAP_MINE_EDGE_LEFT) {
      return ((plrs.item[myPlayerNum].mapX << 8) + (plrs.item[myPlayerNum].pixelX << 4)) >> 8;
  }
  return 0;
}


BYTE playersGetCentredY() {
  if (plrs.item[myPlayerNum].inUse == TRUE && plrs.item[myPlayerNum].mapY >= MAP_MINE_EDGE_TOP) {
      return ((plrs.item[myPlayerNum].mapY << 8) + (plrs.item[myPlayerNum].pixelY << 4)) >> 8;
  }
  return 0;
}

BYTE playersGetTeamId(BYTE playerNum) {
  return plrs.item[playerNum].team;
}


BYTE playersGetUnusedTeam(BYTE playerNum) {
  BYTE vals[MAX_TANKS];
  BYTE count = 0;
  BYTE returnValue = NO_TEAM_SET;
 
  memset(vals, FALSE, MAX_TANKS);
  while (count < MAX_TANKS) {
    if (plrs.item[count].team != NO_TEAM_SET && plrs.item[count].inUse == TRUE) {
      vals[plrs.item[count].team] = TRUE;
    }
    count++;
  }

  count = 0;
  while (returnValue == NO_TEAM_SET && count < MAX_TANKS) {
    if (vals[count] == FALSE) {
      returnValue = count;
    }
    count++;
  }
  return returnValue;
}

BYTE playersGetTeamForOwner(BYTE owner) {
  if (plrs.item[owner].inUse == TRUE) {
    return plrs.item[owner].team;
  }
  return NEUTRAL_TEAM;
}

void playersSetTeams(BYTE *pTeams) {
  BYTE count = 0;

  /* Reset the team ID's */
  while (count < MAX_TANKS) {
    plrs.item[count].team = pTeams[count];
    count++;
  }
}

void playersCopyPTeams(BYTE *dest) {
  BYTE count = 0;
  while (count < MAX_TANKS) {
    dest[count] = plrs.item[count].team;
    count++;
  }
}
