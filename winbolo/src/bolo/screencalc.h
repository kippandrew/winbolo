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
*Name:          ScreenCalc
*Filename:      screenCalc.h 
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 30/01/02
*Purpose:
*  Translates a map square into a tile. 
*  eg road -> T-Junction
*********************************************************/

#ifndef SCREEN_CALC_H
#define SCREEN_CALC_H


/* Includes */
#include "global.h"
#include "tilenum.h"

/* Prototypes */

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
*NAME:          screenCalcBoat
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Returns the boat type of a square
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
BYTE screenCalcBoat(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight);

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
*NAME:          screenCalcForest
*AUTHOR:        John Morrison
*CREATION DATE: 26/3/98
*LAST MODIFIED: 26/3/98
*PURPOSE:
*  Returns the forest type of a square
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
BYTE screenCalcForest(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight);

/*********************************************************
*NAME:          screenCalcCrater
*AUTHOR:        John Morrison
*CREATION DATE: 30/01/02
*LAST MODIFIED: 30/01/02
*PURPOSE:
*  Returns the forest type of a square
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
BYTE screenCalcCrater(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight);

#endif /* SCREEN_CALC_H */
