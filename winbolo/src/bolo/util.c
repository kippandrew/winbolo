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
*Name:          Util
*Filename:      util.c
*Author:        John Morrison
*Creation Date: 25/12/98
*Last Modified: 22/04/01
*Purpose:
*  Provides misc functions
*********************************************************/

/* Inludes */
#include <math.h>
#include <string.h>
#include "global.h"
#include "tank.h"
#include "pillbox.h"
#include "bolo_map.h"
#include "bases.h"
#include "util.h"
#include "mathWinbolo.h"

/*********************************************************
*NAME:          utilCalcDistance
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 25/12/98
*PURPOSE:
* Calculates the X and Y distance an object 
* should move from a given speed and angle
*
*ARGUMENTS:
*  xAmount - The amount to add in the X direction
*  yAmount - The amount to add in the Y direction
*  angle   - The angle the tank is facing
*  speed   - The speed of the tank
*********************************************************/
void utilCalcDistance(int *xAmount, int *yAmount, TURNTYPE angle, int speed) {
  double dbAngle; /* Floating piont number for calculations */

  /* Take away 64 bradians to make angle correct for sin/cos calculations */
  angle -= BRADIANS_EAST;
  if (angle < 0) {
    angle += BRADIANS_MAX;
  }
  /* Convert bradians to degrees */
  dbAngle = (DEGREES_MAX / BRADIANS_MAX) * angle;
  /* Convert degrees to radians */
  dbAngle = (dbAngle / DEGREES_MAX) * RADIANS_MAX;
  /* Perform calculation */
  *xAmount = (int) (speed * cos(dbAngle));
  *yAmount = (int) (speed * sin(dbAngle));
}


/*********************************************************
*NAME:          utilCalcTankSlide
*AUTHOR:        Chris Lesnieski
*CREATION DATE: 2009-01-04
*LAST MODIFIED: 2009-01-04
*PURPOSE:
*  Calculates the X and Y distance a tank should move 
*  after being hit by a shell.  We do not decrement the 
*  tank slide timer here as that is handled by code in the
*  tankUpdate() method.
*
*ARGUMENTS:
*  tankSlideTimer - The number of ticks left to slide
*  angle - The angle at which the shell was travelling
*  xAmount - The amount to add in the X direction
*  yAmount - The amount to add in the Y direction
*  speed   - The speed of the tank
*********************************************************/
void utilCalcTankSlide(BYTE tankSlideTimer, TURNTYPE angle, int *xAmount, int *yAmount, int speed)
{
	float angleRad;
	float ratio;

	angleRad = mathConvertBradianToRadian(angle);

	/* This ratio is not right as it is but it sure is fun to play with */
	ratio = (float)tankSlideTimer / (float)speed;

	/* Calculate the change in both x and y directions */
	*xAmount = (int)(ratio * cos(angleRad));
	*yAmount = (int)(ratio * sin(angleRad)) * -1;

	return;
}


/*********************************************************
*NAME:          utilGetDir
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/98
*LAST MODIFIED: 26/11/98
*PURPOSE:
*  Converts BRadians to 16 direction (0-16)
*
*ARGUMENTS:
*  value - The angle in brandians
*********************************************************/
BYTE utilGetDir(TURNTYPE value) {
  BYTE returnValue; /* Value to return */

  if (value <= 8) {
    returnValue = 0;
  } else if (value <= 24) {
    returnValue = 1;
  } else if (value <= 40) {
    returnValue = 2;
  } else if (value <= 56) {
    returnValue = 3;
  } else if (value <= 72) {
    returnValue = 4;
  } else if (value <= 88) {
    returnValue = 5;
  } else if (value <= 104) {
    returnValue = 6;
  } else if (value <= 120) {
    returnValue = 7;
  } else if (value <= 136) {
    returnValue = 8;
  } else if (value <= 152) {
    returnValue = 9;
  } else if (value <= 168) {
    returnValue = 10;
  } else if (value <= 184) {
    returnValue = 11;
  } else if (value <= 200) {
    returnValue = 12;
  } else if (value <= 216) {
    returnValue = 13;
  } else if (value <= 232) {
    returnValue = 14;
  } else if (value <= 248) {
    returnValue = 15;
  } else {
    returnValue = 0;
  } 

  return returnValue;
}

/*********************************************************
*NAME:          utilGet16Dir
*AUTHOR:        John Morrison
*CREATION DATE: 27/11/98
*LAST MODIFIED: 27/11/98
*PURPOSE:
*  Converts BRadians to nearest 16 direction
*
*ARGUMENTS:
*  value - The angle in brandians
*********************************************************/
BYTE utilGet16Dir(TURNTYPE value) {
  BYTE returnValue; /* Value to return */

  if (value <= BRADIAN_ADD8) {
    returnValue = BRADIANS_NORTH;
  } else if (value <= BRADIANS_NNEAST+ BRADIAN_ADD8) {
    returnValue = BRADIANS_NNEAST;
  } else if (value <= BRADIANS_NEAST + BRADIAN_ADD8) {
    returnValue = BRADIANS_NEAST;
  } else if (value <= BRADIANS_NEASTE + BRADIAN_ADD8) {
    returnValue = BRADIANS_NEASTE ;
  } else if (value <= BRADIANS_EAST + BRADIAN_ADD8) {
    returnValue = BRADIANS_EAST;
  } else if (value <= BRADIANS_SEASTE + BRADIAN_ADD8) {
    returnValue = BRADIANS_SEASTE;
  } else if (value <= BRADIANS_SEAST + BRADIAN_ADD8) {
    returnValue = BRADIANS_SEAST;
  } else if (value <= BRADIANS_SSEAST + BRADIAN_ADD8) {
    returnValue = BRADIANS_SSEAST;
  } else if (value <= BRADIANS_SOUTH + BRADIAN_ADD8) {
    returnValue = BRADIANS_SOUTH;
  } else if (value <= BRADIANS_SSWEST + BRADIAN_ADD8) {
    returnValue = BRADIANS_SSWEST;
  } else if (value <= BRADIANS_SWEST + BRADIAN_ADD8) {
    returnValue = BRADIANS_SWEST;
  } else if (value <= BRADIANS_SWESTW + BRADIAN_ADD8) {
    returnValue = BRADIANS_SWESTW;
  } else if (value <= BRADIANS_WEST + BRADIAN_ADD8) {
    returnValue = BRADIANS_WEST;
  } else if (value <= BRADIANS_NWESTW + BRADIAN_ADD8) {
    returnValue = BRADIANS_NWESTW;
  } else if (value <= BRADIANS_NWEST + BRADIAN_ADD8) {
    returnValue = BRADIANS_NWEST;
  } else if (value <= BRADIANS_NNWEST + BRADIAN_ADD8) {
    returnValue = BRADIANS_NNWEST;
  } else {
    returnValue = 0;
  } 

  return returnValue;
}

/*********************************************************
*NAME:          tankGetWorld
*AUTHOR:        John Morrison
*CREATION DATE:  2/1/99
*LAST MODIFIED:  2/1/99
*PURPOSE:
*  Returns whether the tank has been hit or not. If it 
*
*ARGUMENTS:
*  xTank     - X Co-ordinate of the tank
*  yTank     - Y Co-ordinate of the tank
*  tankAngle - Angle of the tank
*  x         - X co-ord of shell
*  y         - Y co-ord of shell
*  angle     - The direction the shell came from
*********************************************************/
bool utilIsTankHit(WORLD xTank, WORLD yTank, TURNTYPE tankAngle, WORLD x, WORLD y, TURNTYPE angle) {
  bool returnValue; /* Value to return */

  returnValue = FALSE;
  if (abs(xTank - x) < 128 && abs(yTank - y) < 128) {
    returnValue = TRUE;
  }

  return returnValue;
}

/*********************************************************
*NAME:          utilCalcAngle
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  Returns the angle from object 1 to object 2
*
*ARGUMENTS:
*  object1X - X Location of object1
*  object1Y - Y Location of object1
*  object2X - X Location of object2
*  object2Y - Y Location of object1
*********************************************************/
TURNTYPE utilCalcAngle(WORLD object1X, WORLD object1Y, WORLD object2X, WORLD object2Y) {
  TURNTYPE returnValue; /* Value to return */
  double angle;
  double gapX;
  double gapY;

  if (object2X - object1X < 0) {
    gapX = object1X - object2X;
  } else {
    gapX = object2X - object1X;
  }
  if (object2Y - object1Y < 0) {
    gapY = object1Y - object2Y;
  } else {
    gapY = object2Y - object1Y;
  }
 
  angle = atan((gapX/gapY));
  angle = (angle / RADIANS_MAX) * DEGREES_MAX;
  
  returnValue = (TURNTYPE) ((BRADIANS_MAX/ DEGREES_MAX) * angle);
  
  if (object2X - object1X <= 0 && object2Y - object1Y <= 0) {
    returnValue = (TURNTYPE) (BRADIANS_MAX - returnValue);
  } else if (object2X - object1X > 0 && object2Y - object1Y >= 0) {
    returnValue = (TURNTYPE) (BRADIANS_SOUTH - returnValue);
  } else if (object2X - object1X <= 0 && object2Y - object1Y >= 0) {
    returnValue += BRADIANS_SOUTH;
  }

  return returnValue;
}


/*********************************************************
*NAME:          utilIsItemInRange
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED:  23/9/00
*PURPOSE:
* Returns whether an item is in range of range.
* (Remained from utilIsTankInRange) 
*
*ARGUMENTS:
*  x      - Pillbox world X co-ord
*  y      - Pillbox world X co-ord
*  tankX  - Tank world X co-ord
*  tankY  - Tank world Y co-ord
*  range  - Range to check in range
*  amount - Stores copy of the distance if in range
*********************************************************/
bool utilIsItemInRange(WORLD x, WORLD y, WORLD tankX, WORLD tankY, WORLD range, double *amount) {
  bool returnValue; /* Value to return */
  WORLD gapX;       /* Gap from pillbox to tank */
  WORLD gapY;
  double distance;  /* Hold distance */

  /* FIXME: optimise me by using unsigned types. neg*neg = positive */
  returnValue = FALSE;
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

  distance = (double) gapX * gapX;
  distance += (double) gapY * gapY;
  distance = sqrt(distance);
  if (distance >=0 && distance <= range) {
    returnValue = TRUE;
    *amount = distance;
  } else {
    *amount = WORLD_MAX;
  }
  return returnValue;
}


/*********************************************************
*NAME:          utilIsTankInTrees
*AUTHOR:        John Morrison
*CREATION DATE: 18/01/99
*LAST MODIFIED: 22/04/01
*PURPOSE:
* Returns whether a tank is hidden (ie surrounded) by
* trees
*
*ARGUMENTS:
*  mp  - Pointer to the map structure
*  pb  - Pointer to the pillbox structure
*  bs  - Pointer to the bases structure
*  wx  - Tank X world position
*  wy  - Tank Y world position
*********************************************************/
bool utilIsTankInTrees(map *mp, pillboxes *pb, bases *bs, WORLD wx, WORLD wy) {
  bool returnValue; /* Value to return */
  BYTE terrain;     /* Terrain at the current position */
  BYTE terrain2;    /* Terrain at next position */
  BYTE bmx;         /* Tank X & Y Co-ordinates */
  BYTE bmy;

  bmx = wx >> TANK_SHIFT_MAPSIZE;
  bmy = wy >> TANK_SHIFT_MAPSIZE;
  returnValue = TRUE;
  wx <<= TANK_SHIFT_MAPSIZE;
  wx >>= TANK_SHIFT_MAPSIZE;
  wy <<= TANK_SHIFT_MAPSIZE;
  wy >>= TANK_SHIFT_MAPSIZE;

  terrain = mapGetPos(mp, bmx, bmy);
  if (terrain != FOREST && terrain != MINE_FOREST) {
    returnValue = FALSE;
  } else if (pillsExistPos(pb, bmx, bmy) == TRUE || basesExistPos(bs, bmx, bmy) == TRUE) {
    returnValue = FALSE;
  }

  if (returnValue == TRUE) {
    if (wx >= MAP_SQUARE_MIDDLE) {
      /* wx > 128 */
      terrain = mapGetPos(mp, (BYTE) (bmx+1), bmy);
      if ((terrain != FOREST && terrain != MINE_FOREST) || pillsExistPos(pb, (BYTE) (bmx+1), bmy) == TRUE || basesExistPos(bs, (BYTE) (bmx+1), bmy) == TRUE) {
          returnValue = FALSE;
       }
      if (returnValue == TRUE) {
        if (wy >= MAP_SQUARE_MIDDLE) {
          /* wx > 128 && wy > 128 */
          terrain = mapGetPos(mp, (BYTE) (bmx+1), (BYTE) (bmy+1));
          terrain2 = mapGetPos(mp, bmx, (BYTE) (bmy+1));
          if (pillsExistPos(pb, (BYTE) (bmx+1), (BYTE) (bmy+1)) == TRUE || basesExistPos(bs, (BYTE) (bmx+1), (BYTE) (bmy+1)) == TRUE) {
            returnValue = FALSE;
          }
          if (pillsExistPos(pb, bmx, (BYTE) (bmy+1)) == TRUE || basesExistPos(bs, bmx, (BYTE) (bmy+1)) == TRUE) {
            returnValue = FALSE;
          }

        } else {
          /* wx > 128 && wy < 128 */
          terrain = mapGetPos(mp, (BYTE) (bmx+1), (BYTE) (bmy-1));
          terrain2 = mapGetPos(mp, bmx, (BYTE) (bmy-1));
        }
        if (returnValue == TRUE && (terrain != FOREST && terrain != MINE_FOREST) || (terrain2 != FOREST && terrain2 != MINE_FOREST)) {
          returnValue = FALSE;
        }
      }
    } else {
      /* wx < 128 */
      terrain = mapGetPos(mp, (BYTE) (bmx-1), bmy);
      if (terrain != FOREST && terrain != MINE_FOREST) {
          returnValue = FALSE;
       }
      if (returnValue == TRUE) {
        if (wy >= MAP_SQUARE_MIDDLE) {
          /* wx < 128 && wy > 128 */
          terrain2 = mapGetPos(mp, bmx, (BYTE) (bmy+1));
          terrain = mapGetPos(mp, (BYTE) (bmx-1), (BYTE) (bmy+1));
        } else {
          /* wx < 128 && wy < 128 */
          terrain2 = mapGetPos(mp, bmx, (BYTE) (bmy-1));
          terrain = mapGetPos(mp, (BYTE) (bmx-1), (BYTE) (bmy-1));
        }
        if ((terrain != FOREST && terrain != MINE_FOREST) || (terrain2 != FOREST && terrain2 != MINE_FOREST)) {
          returnValue = FALSE;
        }
      }
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          utilPtoCString
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Convert Bolo's network pascal string to C strings
*
*ARGUMENTS:
*  src  - Source string
*  dest - Destination string 
*********************************************************/
void utilPtoCString(char *src, char *dest) {
	int count; /* Looping variable */
  int len;   /* Length of the string */
  len = src[0];
  for (count=0; count<len; count++) {
    dest[count] = src[count+1];
	}
  dest[count] = '\0';
}

/*********************************************************
*NAME:          utilPtoCString
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Convert a C string to a Bolo's network pascal string 
*
*ARGUMENTS:
*  src  - Source string
*  dest - Destination string 
*********************************************************/
void utilCtoPString(char *src, char *dest) {
	int count; /* Looping variable */
  int len;   /* Length of the string */
  
  len = (int) strlen(src);
  for (count=1; count<=len; count++) {
    dest[count] = src[count-1];
	}
  dest[0] = (char) len;
}

/*********************************************************
*NAME:          utilGetNibbles
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 27/2/99
*PURPOSE:
* Extacts the high and low nibbles out of a byte
*
*ARGUMENTS:
*  value - The byte the nibbles come from
*  high - Pointer to hold high nibble
*  low  - Pointer to hold low nibble
*********************************************************/
void utilGetNibbles(BYTE value, BYTE *high, BYTE *low) {
  *high = *low = value;
  *high >>= NIBBLE_SHIFT_SIZE;
  *low <<= NIBBLE_SHIFT_SIZE;
  *low >>= NIBBLE_SHIFT_SIZE;
}

/*********************************************************
*NAME:          utilPutNibble
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 27/2/99
*PURPOSE:
* Returns the high and low nibbles as a combined byte
*
*ARGUMENTS:
*  high - High nibble
*  low  - Low nibble
*********************************************************/
BYTE utilPutNibble(BYTE high, BYTE low) {
  BYTE returnValue; /* Value to return */

  returnValue = high;
  returnValue <<= NIBBLE_SHIFT_SIZE;
  returnValue += low;
  return returnValue;
}

/*********************************************************
*NAME:          utilExtractMapName
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 27/5/00
*PURPOSE:
* Extracts the map name from a file name and path.
*
*ARGUMENTS:
*  fileName - Map File name and path.
*  mapName  - Stores the Map Name.
*********************************************************/
void utilExtractMapName(char *fileName, char *mapName) {
  char *tok;                /* String Tok return value */
  char dummy[BIG_FILENAME]; /* Dummy copy of filename */
  int len;                  /* Length of the string */

  dummy[0] = '\0';
  strcpy(dummy, fileName);
    
  tok = strtok(dummy, STRTOK_SEPS);
  if (tok) {
    strcpy(mapName, tok);
    tok = strtok(NULL, STRTOK_SEPS);
    while (tok != NULL) {
      strcpy(mapName, tok);
      tok = strtok(NULL, STRTOK_SEPS);
    }
    len = (int) strlen(mapName);
    if (mapName[len-END3] == END3CHRA && (mapName[len-END2] == END2CHRA || mapName[len-END2] == END2CHRB) && (mapName[len-END1] == END1CHRA || mapName[len-END1] == END1CHRB) && (mapName[len-END0] == END0CHRA || mapName[len-END0] == END0CHRB)) {
      mapName[len-END3] = '\0';
    }
  }
}


/*********************************************************
*NAME:          utilStripNameReplace
*AUTHOR:        John Morrison
*CREATION DATE: 30/01/02
*LAST MODIFIED: 30/01/02
*PURPOSE:
* Strips tab and space charectors from the start/end of
* the string name. If resulting string is empty copies
* in "Me"
*
*ARGUMENTS:
*  name - String to remove from
*********************************************************/
void utilStripNameReplace(char *name) {
  utilStripName(name);
  if (name[0] == '\0') {
    strcpy(name, "Me");
  }
}

/*********************************************************
*NAME:          utilStripName
*AUTHOR:        John Morrison
*CREATION DATE: 30/01/02
*LAST MODIFIED: 30/01/02
*PURPOSE:
* Strips tab and space charectors from the start/end of
* the string name
*
*ARGUMENTS:
*  name - String to remove from
*********************************************************/
void utilStripName(char *name) {
  char *ptr;
  int len;

  len = (int) strlen(name);
  /* Strip charectors at the end */
  if (len > 2) {
    ptr = name + strlen(name) - 1;
    while (ptr != name && (*ptr == ' ' || *ptr == '\t')) {
      *ptr = '\0';
      ptr--;
    }
  }

  /* Strip charectors at the start */
  ptr = name;
  len = 0;
  while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t')) {
    len++;
    ptr++;
  }
  if (len != 0) {
    ptr = name + len;
    while (*ptr != '\0') {
      *(ptr - len) = *ptr;
      ptr++;
    }
    *(ptr - len) = *ptr;
  }
}


