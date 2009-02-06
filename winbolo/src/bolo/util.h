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
*Filename:      util.h 
*Author:        John Morrison
*Creation Date: 25/12/98
*Last Modified: 30/01/02
*Purpose:
*  Provides misc functions
*********************************************************/

#ifndef UTILS_H
#define UTILS_H

#define BRADIAN_ADD8 8
#define NIBBLE_SHIFT_SIZE 4



/* Used in extracting the map name from the filename and path */
#define STRTOK_SEPS "\\/"
#define END3 4
#define END2 3
#define END1 2
#define END0 1
#define END3CHRA '.'
#define END2CHRA 'm'
#define END1CHRA 'a'
#define END0CHRA 'p'
#define END2CHRB 'M'
#define END1CHRB 'A'
#define END0CHRB 'P'


/* We allow big file names */
#define BIG_FILENAME 1024



/* Includes */
#include "global.h"
#include "types.h"

/*********************************************************
*NAME:          tankCalcDistance
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
void utilCalcDistance(int *xAmount, int *yAmount, TURNTYPE angle, int speed);


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
void utilCalcTankSlide(BYTE tankSlideTimer, TURNTYPE angle, int *xAmount, int *yAmount, int speed);


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
BYTE utilGetDir(TURNTYPE value);

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
BYTE utilGet16Dir(TURNTYPE value);

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
bool utilIsTankHit(WORLD xTank, WORLD yTank, TURNTYPE tankAngle, WORLD x, WORLD y, TURNTYPE angle);

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
TURNTYPE utilCalcAngle(WORLD object1X, WORLD object1Y, WORLD object2X, WORLD object2Y);

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
bool utilIsItemInRange(WORLD x, WORLD y, WORLD tankX, WORLD tankY, WORLD range, double *amount);

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
bool utilIsTankInTrees(map *mp, pillboxes *pb, bases *bs, WORLD wx, WORLD wy);

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
void utilPtoCString(char *src, char *dest);

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
void utilCtoPString(char *src, char *dest);

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
void utilGetNibbles(BYTE value, BYTE *high, BYTE *low);

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
BYTE utilPutNibble(BYTE high, BYTE low);

/*********************************************************
*NAME:          utilExtractMapName
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
* Extracts the map name from a file name and path.
*
*ARGUMENTS:
*  
*  fileName - Map File name and path.
*  mapName  - Stores the Map Name.
*********************************************************/
void utilExtractMapName(char *fileName, char *mapName);

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
void utilStripNameReplace(char *name);

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
void utilStripName(char *name);

#endif /* UTILS_H */
