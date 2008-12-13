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
*Name:          Screen
*Filename:      screen.h 
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 11/11/00
*Purpose:
*  Provides Interfaces with the front end
*********************************************************/

#ifndef SCREEN_H
#define SCREEN_H


/* Includes */
#include "global.h"

/* Defines */
#define SCREEN_SIZE_X 30 /*16 is bolo window size */
#define SCREEN_SIZE_Y 30 /*18 is bolo window size */

#define ROAD_HORZ 17
#define ROAD_VERT 18
#define ROAD_CORNER1 19 /* The road corners */
#define ROAD_CORNER2 20
#define ROAD_CORNER3 21
#define ROAD_CORNER4 22
#define ROAD_CORNER5 23 /* The 4 solid road corners */
#define ROAD_CORNER6 24
#define ROAD_CORNER7 25
#define ROAD_CORNER8 26
#define ROAD_SOLID 27 /* Solid road */
#define ROAD_CROSS 28
#define ROAD_T1 29    /* T Junctions */
#define ROAD_T2 30
#define ROAD_T3 31
#define ROAD_T4 32
#define ROAD_WATER1 33 /* 1 side over other normal */
#define ROAD_WATER2 34
#define ROAD_WATER3 35
#define ROAD_WATER4 36
#define ROAD_WATER5 37 /* Corners on water */
#define ROAD_WATER6 38
#define ROAD_WATER7 39
#define ROAD_WATER8 40
#define ROAD_WATER9 41 /* Horizontal water peice */
#define ROAD_WATER10 42 /* Vertical water peice */
#define ROAD_WATER11 43 /* Lone water peice */

#define ROAD_SIDE1 44 /* Single Side peices */
#define ROAD_SIDE2 45
#define ROAD_SIDE3 46
#define ROAD_SIDE4 47 

#define PILL_EVIL 48
#define BASE_GOOD 49
#define START 122

#define BUILD_SINGLE 50
#define BUILD_HORZ 51
#define BUILD_VERT 52
#define BUILD_HORZEND1 53 /* End peices */
#define BUILD_HORZEND2 54
#define BUILD_VERTEND1 55
#define BUILD_VERTEND2 56
#define BUILD_SOLID 57 /* Builing peices on all side */
#define BUILD_CORNER1 58 /* 4 corners */
#define BUILD_CORNER2 59
#define BUILD_CORNER3 60
#define BUILD_CORNER4 61
#define BUILD_L1 62 /* L - Shaped Peices */
#define BUILD_L2 63
#define BUILD_L3 64
#define BUILD_L4 65
#define BUILD_T1 66 /* T Shaped Peices  */
#define BUILD_T2 67
#define BUILD_T3 68
#define BUILD_T4 69
#define BUILD_CROSS 70
#define BUILD_SIDE1 71 /* Sides */
#define BUILD_SIDE2 72
#define BUILD_SIDE3 73
#define BUILD_SIDE4 74
#define BUILD_SIDECORN1 75 /* Sides and Corners */
#define BUILD_SIDECORN2 76
#define BUILD_SIDECORN3 77
#define BUILD_SIDECORN4 78
#define BUILD_SIDECORN5 79
#define BUILD_SIDECORN6 80
#define BUILD_SIDECORN7 81
#define BUILD_SIDECORN8 82
#define BUILD_SIDECORN9 83
#define BUILD_SIDECORN10 84
#define BUILD_SIDECORN11 85
#define BUILD_SIDECORN12 86
#define BUILD_SIDECORN13 87
#define BUILD_SIDECORN14 88
#define BUILD_SIDECORN15 89
#define BUILD_SIDECORN16 90
#define BUILD_TWIST1 91 /* Strange half peices */
#define BUILD_TWIST2 92
#define BUILD_MOST1 93 /* Most covered Also last 4 peices */
#define BUILD_MOST2 94
#define BUILD_MOST3 95
#define BUILD_MOST4 96


#define RIVER_END1 97  /* The four end U peices */
#define RIVER_END2 98
#define RIVER_END3 99
#define RIVER_END4 100
#define RIVER_SOLID 101 /* Solid River Peice */
#define RIVER_SURROUND 102 /* River surrounded on all sides by buildings */
#define RIVER_SIDE1 103 /* River Side Peices eg - | | */
#define RIVER_SIDE2 104 
#define RIVER_ONESIDE1 105 /* One side land others water */
#define RIVER_ONESIDE2 106
#define RIVER_ONESIDE3 107
#define RIVER_ONESIDE4 108
#define RIVER_CORN1 109 /* Four corner L shaped peices */
#define RIVER_CORN2 110
#define RIVER_CORN3 111
#define RIVER_CORN4 112
/* Deep Sea Peices */
#define DEEP_SEA_SOLID 113 
#define DEEP_SEA_CORN1 114 /* Corner peices L Shaped */
#define DEEP_SEA_CORN2 115
#define DEEP_SEA_CORN3 116
#define DEEP_SEA_CORN4 117
#define DEEP_SEA_SIDE1 118 /* Four Sides */
#define DEEP_SEA_SIDE2 119
#define DEEP_SEA_SIDE3 120
#define DEEP_SEA_SIDE4 121

/* Type definitions */

/* The screen object */
typedef struct screenObj *screen;
struct screenObj {
  BYTE screenItem[SCREEN_SIZE_X][SCREEN_SIZE_Y];
};

/* Screen Mines */
typedef struct screenMineObj *screenMines;
struct screenMineObj {
  bool mineItem[SCREEN_SIZE_X][SCREEN_SIZE_Y];
};


/* Button Pressed */
typedef enum {
  left,
  right,
  up,
  down,
  pageUp,
  pageDown,
  pageLeft,
  pageRight,
  redraw
}updateType;



/* Prototypes */

/*********************************************************
*NAME:          screenSetup
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets up all the variables - Should be run when the
*  program starts.
*
*ARGUMENTS:
*
*********************************************************/
void screenSetup();

/*********************************************************
*NAME:          screenDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Destroys the structures Should be called on
*  program exit
*
*ARGUMENTS:
*
*********************************************************/
void screenDestroy();

/*********************************************************
*NAME:          screenUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Updates the screen. Takes numerous directions
*
*ARGUMENTS:
*  value - Pointer to the screen structure
*********************************************************/
void screenUpdate(updateType value);

/*********************************************************
*NAME:          screenGetPos
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Gets the value of a square in the structure
*  Return DEEP_SEA if out of range
*
*ARGUMENTS:
*  value  - Pointer to the screen structure
*  xValue - The X co-ordinate
*  yValue - The Y co-ordinate
*********************************************************/
BYTE screenGetPos(screen *value,BYTE xValue, BYTE yValue);


/*********************************************************
*NAME:          screenUpdateView
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED:  4/11/98
*PURPOSE:
*  Updates the values in the view area
*
*ARGUMENTS:
* value - The update type (Helps in optimisations)
*********************************************************/
void screenUpdateView(updateType value);

/*********************************************************
*NAME:          screenCalcSquare
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Calculates the terrain type for a given location
*
*ARGUMENTS:
*  xValue - The x co-ordinate
*  yValue - The y co-ordinate
*********************************************************/
BYTE screenCalcSquare(BYTE xValue, BYTE yValue, BYTE scrX, BYTE scrY);


/*********************************************************
*NAME:          screenCalcRoad
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Returns the road type of a square
*
*ARGUMENTS:
*  aboveLeft  - The square above left 
*  above      - The square above 
*  aboveRight - The square above right
*  left       - The square left
*  right      - The square right
*  belowLeft  - The square below left
*  belowRight - The square below right
*********************************************************/
BYTE screenCalcRoad(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight);

/*********************************************************
*NAME:          screenCalcBuilding
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Returns the building type
*
*ARGUMENTS:
*  aboveLeft  - The square above left 
*  above      - The square above 
*  aboveRight - The square above right
*  left       - The square left
*  right      - The square right
*  belowLeft  - The square below left
*  belowRight - The square below right
*********************************************************/
BYTE screenCalcBuilding(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight);

/*********************************************************
*NAME:          screenCalcRiver
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Returns the River type
*
*ARGUMENTS:
*  aboveLeft  - The square above left 
*  above      - The square above 
*  aboveRight - The square above right
*  left       - The square left
*  right      - The square right
*  belowLeft  - The square below left
*  belowRight - The square below right
*********************************************************/
BYTE screenCalcRiver(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight);

/*********************************************************
*NAME:          screenCalcDeepSea
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Returns the Deep Sea type
*
*ARGUMENTS:
*  aboveLeft  - The square above left 
*  above      - The square above 
*  aboveRight - The square above right
*  left       - The square left
*  right      - The square right
*  belowLeft  - The square below left
*  belowRight - The square below right
*********************************************************/
BYTE screenCalcDeepSea(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight);

/*********************************************************
*NAME:          screenLoadMap
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 11/11/00
*PURPOSE:
*  Loads a map. Returns if it was sucessful reading the
*  map or not.
*
*ARGUMENTS:
* buff   - Pointer to the memory buffer
* length - Length of the memory buffer
*********************************************************/
bool screenLoadMap(BYTE *buff, long length);

/*********************************************************
*NAME:          screenWriteMap
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Loads a map. Returns if it was sucessful reading the
*  map or not.
*
*ARGUMENTS:
* buff   - Pointer to the memory buffer
* length - Pointer to length of the memory buffer
*********************************************************/
bool screenWriteMap(BYTE *buff, long *length);

void screenSetOffset(BYTE x, BYTE y);
BYTE screenGetOffsetX();
BYTE screenGetOffsetY();

/*********************************************************
*NAME:          screenSetPos
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Sets the value of a square in the structure
*
*ARGUMENTS:
*  xValue - The X co-ordinate
*  yValue - The Y co-ordinate
*  terrain - Terraint to set to
*********************************************************/
void screenSetPos(BYTE xValue, BYTE yValue, BYTE terrain) ;


BYTE screenGetNumPills();
BYTE screenGetNumBases();
BYTE screenGetNumStarts();

bool screenSetStart(BYTE x, BYTE y);
bool screenSetPill(BYTE x, BYTE y);
bool screenSetBase(BYTE x, BYTE y);
#endif /* SCREEN_H */
