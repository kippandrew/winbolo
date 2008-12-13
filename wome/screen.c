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
*Filename:      screen.c
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified:  4/11/98
*Purpose:
*  Provides Interfaces with the front end
*********************************************************/


/* Includes */
#include "global.h"
#include "screen.h"
#include "bolo_map.h"
#include "pillbox.h"
#include "bases.h"
#include "starts.h"

/* Module Level Variables */

screen view;
screenMines mineView;
map mp;
bases bs;
pillboxes pb;
starts ss;

/* The offset from the top and left of the map */
BYTE xOffset;
BYTE yOffset;

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
void screenSetup() { 

  xOffset = 127;
  yOffset = 127;

  mapCreate(&mp);
  pillsCreate(&pb);
  startsCreate(&ss);
  basesCreate(&bs);
  New(view);
  New(mineView);
  screenUpdateView(redraw);
}

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
void screenDestroy() {
  mapDestroy(&mp);
  pillsDestroy(&pb);
  startsDestroy(&ss);
  basesDestroy(&bs);
  Dispose(view);
  Dispose(mineView);
}
/*********************************************************
*NAME:          screenUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
*  Updates the screen. Takes numerous directions
*
*ARGUMENTS:
*  value - Pointer to the starts structure
*********************************************************/
void screenUpdate(updateType value) {
  screenUpdateView(value);
  Ex_OnPaint(&view, &mineView);
}

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
void screenSetPos(BYTE xValue, BYTE yValue, BYTE terrain) {
  mapSetPos(&mp, xValue, yValue, terrain);
  basesDeleteBase(&bs, xValue, yValue);
  startsDeleteStart(&ss, xValue, yValue); 
  pillsDeletePill(&pb, xValue, yValue);
}

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
BYTE screenGetPos(screen *value,BYTE xValue, BYTE yValue) {
  BYTE returnValue = DEEP_SEA; /* Value to return */

  if (xValue < SCREEN_SIZE_X && yValue < SCREEN_SIZE_Y) {
    returnValue = (*value)->screenItem[xValue][yValue];
  }
  return returnValue;
}

/*********************************************************
*NAME:          screenUpdateView
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED: 29/10/98
*PURPOSE:
*  Updates the values in the view area
*
*ARGUMENTS:
* value - The update type (Helps in optimisations)
*********************************************************/
void screenUpdateView(updateType value) {
  BYTE count;   /* Looping Variables */
  BYTE count2;

  for (count=0;count<SCREEN_SIZE_X;count++) {
    for (count2=0;count2<SCREEN_SIZE_Y;count2++) {
      (*view).screenItem[count][count2] = screenCalcSquare((BYTE) (count+xOffset),(BYTE) (count2+yOffset), count, count2);
    }
  }
}

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
BYTE screenCalcSquare(BYTE xValue, BYTE yValue, BYTE scrX, BYTE scrY) {
  BYTE returnValue; /* Value to return */
  BYTE currentPos;
  BYTE aboveLeft;
  BYTE above;
  BYTE aboveRight;
  BYTE left;
  BYTE right;
  BYTE belowLeft;
  BYTE below;
  BYTE belowRight;

  /* Set up Items */
  if ((pillsExistPos(&pb,xValue,yValue)) == TRUE) {
    currentPos = PILL_EVIL;
  } else if ((basesExistPos(&bs,xValue,yValue)) == TRUE) {
    currentPos = BASE_GOOD;
  } else if ((startsExistPos(&ss, xValue, yValue)) == TRUE) {
    currentPos = START;
  }  else {
    currentPos = mapGetPos(&mp,xValue,yValue);
  }

  if (currentPos >= MINE_START && currentPos <= MINE_END) {
    (*mineView).mineItem[scrX][scrY] = TRUE;
    currentPos = currentPos - MINE_SUBTRACT;
  } else {
    (*mineView).mineItem[scrX][scrY] = FALSE;
  }

  if ((pillsExistPos(&pb,(BYTE) (xValue-1),(BYTE) (yValue-1))) == TRUE) {
    aboveLeft = PILL_EVIL;
  } else if ((basesExistPos(&bs,(BYTE) (xValue-1),(BYTE) (yValue-1))) == TRUE) {
    aboveLeft = ROAD;
  } else {
    aboveLeft = mapGetPos(&mp,(BYTE) (xValue-1),(BYTE) (yValue-1));
  }

  if (aboveLeft >= MINE_START && aboveLeft <= MINE_END) {
    aboveLeft = aboveLeft - MINE_SUBTRACT;
  }


  if ((pillsExistPos(&pb,xValue,(BYTE) (yValue-1))) == TRUE) {
    above = PILL_EVIL;
  } else if ((basesExistPos(&bs,xValue,(BYTE) (yValue-1))) == TRUE) {
    above = ROAD;
  } else {
    above = mapGetPos(&mp,xValue,(BYTE) (yValue-1));
  }

  if (above >= MINE_START && above <= MINE_END) {
    above = above - MINE_SUBTRACT;
  }


  if ((pillsExistPos(&pb,(BYTE) (xValue+1),(BYTE) (yValue-1))) == TRUE) {
    aboveRight = PILL_EVIL;
  } else if ((basesExistPos(&bs,(BYTE) (xValue+1),(BYTE) (yValue-1))) == TRUE) {
    aboveRight = ROAD;
  } else {
    aboveRight = mapGetPos(&mp,(BYTE) (xValue+1),(BYTE) (yValue-1));
  }

  if (aboveRight >= MINE_START && aboveRight <= MINE_END) {
    aboveRight = aboveRight - MINE_SUBTRACT;
  }


  if ((pillsExistPos(&pb,(BYTE) (xValue-1),yValue)) == TRUE) {
    left = PILL_EVIL;
  } else if ((basesExistPos(&bs,(BYTE) (xValue-1),yValue)) == TRUE) {
    left = ROAD;
  } else {
    left = mapGetPos(&mp,(BYTE) (xValue-1),yValue);
  }

  if (left >= MINE_START && left <= MINE_END) {
    left = left - MINE_SUBTRACT;
  }


  if ((pillsExistPos(&pb,(BYTE) (xValue+1),yValue)) == TRUE) {
    right = PILL_EVIL;
  } else if ((basesExistPos(&bs,(BYTE) (xValue+1),yValue)) == TRUE) {
    right = ROAD;
  } else {
    right = mapGetPos(&mp,(BYTE) (xValue+1),yValue);
  }

  if (right >= MINE_START && right <= MINE_END) {
    right = right - MINE_SUBTRACT;
  }


  if ((pillsExistPos(&pb,(BYTE) (xValue-1),(BYTE) (yValue+1))) == TRUE) {
    belowLeft = PILL_EVIL;
  } else if ((basesExistPos(&bs,(BYTE) (xValue-1),(BYTE) (yValue+1))) == TRUE) {
    belowLeft = ROAD;
  } else {
    belowLeft = mapGetPos(&mp,(BYTE) (xValue-1),(BYTE) (yValue+1));
  }

  if (belowLeft >= MINE_START && belowLeft <= MINE_END) {
    belowLeft = belowLeft - MINE_SUBTRACT;
  }


  if ((pillsExistPos(&pb,xValue,(BYTE) (yValue+1))) == TRUE) {
    below = PILL_EVIL;
  } else if ((basesExistPos(&bs,xValue,(BYTE) (yValue+1))) == TRUE) {
    below = ROAD;
  } else {
    below = mapGetPos(&mp,xValue,(BYTE) (yValue+1));
  }

  if (below >= MINE_START && below <= MINE_END) {
    below = below - MINE_SUBTRACT;
  }

  if ((pillsExistPos(&pb,(BYTE) (xValue+1),(BYTE) (yValue+1))) == TRUE) {
    belowRight = PILL_EVIL;
  } else if ((basesExistPos(&bs,(BYTE) (xValue+1),(BYTE) (yValue+1))) == TRUE) {
    belowRight = ROAD;
  } else {
    belowRight = mapGetPos(&mp,(BYTE) (xValue+1),(BYTE) (yValue+1));
  }

  if (belowRight >= MINE_START && belowRight <= MINE_END) {
    belowRight = belowRight - MINE_SUBTRACT;
  }


  switch (currentPos) {
  case ROAD:
    returnValue = screenCalcRoad(aboveLeft, above, aboveRight, left, right, belowLeft, below, belowRight);
    break;
  case BUILDING:
    returnValue = screenCalcBuilding(aboveLeft, above, aboveRight, left, right, belowLeft, below, belowRight);
    break;
  case RIVER:
    returnValue = screenCalcRiver(aboveLeft, above, aboveRight, left, right, belowLeft, below, belowRight);
    break;
  case DEEP_SEA:
    returnValue = screenCalcDeepSea(aboveLeft, above, aboveRight, left, right, belowLeft, below, belowRight);
    break;
  default:
    returnValue = currentPos;
    break;
  }



  
  return returnValue;
}

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
BYTE screenCalcRoad(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight) {
  BYTE returnValue; /* Value to return */


  /* Turn Boats and deep sea into rivers */
  if (above == DEEP_SEA || above == BOAT) {
    above = RIVER;
  }
  if (aboveLeft == DEEP_SEA || aboveLeft == BOAT) {
    aboveLeft = RIVER;
  }
  if (aboveRight == DEEP_SEA || aboveRight == BOAT) {
    aboveRight = RIVER;
  }
  if (left == DEEP_SEA || left == BOAT) {
    left = RIVER;
  }
  if (right == DEEP_SEA || right == BOAT) {
    right = RIVER;
  }
  if (below == DEEP_SEA || below == BOAT) {
    below = RIVER;
  }
  if (belowLeft == DEEP_SEA || belowLeft == BOAT) {
    belowLeft = RIVER;
  }
  if (belowRight == DEEP_SEA || belowRight == BOAT) {
    belowRight = RIVER;
  }


  if (aboveLeft != ROAD && above == ROAD && aboveRight != ROAD && left == ROAD && right == ROAD && belowLeft != ROAD && below == ROAD && belowRight != ROAD) {
    returnValue = ROAD_CROSS;
  } else if (above == ROAD && left == ROAD && right == ROAD && below == ROAD) {
    returnValue = ROAD_SOLID;
  } else if (left == RIVER && right == RIVER && above == RIVER && below == RIVER) {
    returnValue = ROAD_WATER11;
  } else if (right == ROAD && below == ROAD && left == RIVER && above == RIVER) {
    returnValue = ROAD_WATER5;
  } else if (left == ROAD && below == ROAD && right == RIVER && above == RIVER) {
    returnValue = ROAD_WATER6;
  } else if (above == ROAD && left == ROAD && below == RIVER && right == RIVER) {
    returnValue = ROAD_WATER7;
  } else if (right == ROAD && above == ROAD && left == RIVER && below == RIVER) {
    returnValue = ROAD_WATER8;
  } else if((left == ROAD || right == ROAD) && above == RIVER && below == RIVER) {
    returnValue = ROAD_WATER9;
  } else if((above == ROAD || below == ROAD) && left == RIVER && right == RIVER) {
    returnValue = ROAD_WATER10;
  } else if (above == RIVER && below == ROAD) {
    returnValue = ROAD_WATER1;
  } else if (right == RIVER && left == ROAD) {
    returnValue = ROAD_WATER2;
  } else if (below == RIVER && above == ROAD) {
    returnValue = ROAD_WATER3;
  } else if (left == RIVER && right == ROAD) {
    returnValue = ROAD_WATER4;

  } else if (right == ROAD && below == ROAD && above == ROAD && (aboveRight == ROAD || belowRight == ROAD)) {
    returnValue = ROAD_SIDE1;
  } else if (left == ROAD && right == ROAD && below == ROAD && (belowLeft == ROAD || belowRight == ROAD)) {
    returnValue = ROAD_SIDE2;
  } else if (left == ROAD && above == ROAD && below == ROAD && (belowLeft == ROAD || aboveLeft == ROAD)) {
    returnValue = ROAD_SIDE3;
  } else if (left == ROAD && right == ROAD && above == ROAD && (aboveRight == ROAD || aboveLeft == ROAD)) {
    returnValue = ROAD_SIDE4;  
  
  } else if (left == ROAD && right == ROAD && below == ROAD) {
    returnValue = ROAD_T1;
  } else if (left == ROAD && above == ROAD && below == ROAD) {
    returnValue = ROAD_T2;
  } else if (left == ROAD && right == ROAD && above == ROAD) {
    returnValue = ROAD_T3;
  } else if (right == ROAD && above == ROAD && below == ROAD) {
    returnValue = ROAD_T4;  
  } else if (below == ROAD && right == ROAD && belowRight == ROAD) {
    returnValue = ROAD_CORNER5;
  } else if (below == ROAD && left == ROAD && belowLeft == ROAD) {
    returnValue = ROAD_CORNER6;
  } else if (above == ROAD && left == ROAD && aboveLeft == ROAD) {
    returnValue = ROAD_CORNER7;
  } else if (above == ROAD && right == ROAD && aboveRight == ROAD) {
    returnValue = ROAD_CORNER8;
  } else if (below == ROAD && right == ROAD) {
    returnValue = ROAD_CORNER1;
  } else if (below == ROAD && left == ROAD) {
    returnValue = ROAD_CORNER2;
  } else if (above == ROAD && left == ROAD) {
    returnValue = ROAD_CORNER3;
  } else if (above == ROAD && right == ROAD) {
    returnValue = ROAD_CORNER4;
  } else if (right == ROAD || left == ROAD) {
    returnValue = ROAD_HORZ;
  } else if (above == ROAD || below == ROAD) {
    returnValue = ROAD_VERT;
  } else {
    returnValue = ROAD_SOLID;
  }

  return returnValue;
}

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
BYTE screenCalcBuilding(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight) {
  BYTE returnValue; /* Value to return */

  if (aboveLeft == BUILDING && above == BUILDING && aboveRight == BUILDING && left == BUILDING && right == BUILDING && belowLeft == BUILDING && below == BUILDING && belowRight == BUILDING) {
    returnValue = BUILD_SOLID;
  } else if (right == BUILDING && above == BUILDING && below == BUILDING && left == BUILDING && aboveRight != BUILDING && aboveLeft != BUILDING && belowRight != BUILDING && belowLeft != BUILDING) {
    returnValue = BUILD_CROSS;
  } else if (right == BUILDING && above == BUILDING && below == BUILDING && left == BUILDING && aboveRight != BUILDING && aboveLeft != BUILDING && belowRight != BUILDING && belowLeft == BUILDING) {
    returnValue = BUILD_MOST1;
  } else if (right == BUILDING && above == BUILDING && below == BUILDING && left == BUILDING && aboveRight != BUILDING && aboveLeft == BUILDING && belowRight != BUILDING && belowLeft != BUILDING) {
    returnValue = BUILD_MOST2;
  } else if (right == BUILDING && above == BUILDING && below == BUILDING && left == BUILDING && aboveRight != BUILDING && aboveLeft != BUILDING && belowRight == BUILDING && belowLeft != BUILDING) {
    returnValue = BUILD_MOST3;
  } else if (right == BUILDING && above == BUILDING && below == BUILDING && left == BUILDING && aboveRight == BUILDING && aboveLeft != BUILDING && belowRight != BUILDING && belowLeft != BUILDING) {
    returnValue = BUILD_MOST4;

  } else if (aboveLeft == BUILDING && above == BUILDING && left == BUILDING && right == BUILDING && belowLeft == BUILDING && below == BUILDING && belowRight == BUILDING) {
    returnValue = BUILD_SIDECORN13;
  } else if (above == BUILDING && aboveRight == BUILDING && left == BUILDING && right == BUILDING && belowLeft == BUILDING && below == BUILDING && belowRight == BUILDING) {
    returnValue = BUILD_SIDECORN14;
  } else if (aboveLeft == BUILDING && above == BUILDING && aboveRight == BUILDING && left == BUILDING && right == BUILDING && belowLeft == BUILDING && below == BUILDING) {
    returnValue = BUILD_SIDECORN15;
  } else if (aboveLeft == BUILDING && above == BUILDING && aboveRight == BUILDING && left == BUILDING && right == BUILDING && below == BUILDING && belowRight == BUILDING) {
    returnValue = BUILD_SIDECORN16;

  } else if (left == BUILDING && right == BUILDING && above == BUILDING && below == BUILDING && aboveRight == BUILDING && belowLeft == BUILDING && aboveLeft != BUILDING && belowRight != BUILDING) {
    returnValue = BUILD_TWIST1;
  } else if (left == BUILDING && right == BUILDING && above == BUILDING && below == BUILDING && belowRight == BUILDING && aboveLeft == BUILDING && aboveRight != BUILDING && belowLeft != BUILDING) {
    returnValue = BUILD_TWIST2;

  } else if (above == BUILDING && left == BUILDING && right == BUILDING && below == BUILDING && belowRight == BUILDING && aboveRight == BUILDING) {
    returnValue = BUILD_SIDECORN5;
  } else if (above == BUILDING && left == BUILDING && right == BUILDING && below == BUILDING && belowLeft == BUILDING && aboveLeft == BUILDING) {
    returnValue = BUILD_SIDECORN6;
  } else if (above == BUILDING && left == BUILDING && right == BUILDING && below == BUILDING && belowLeft == BUILDING && belowRight == BUILDING) {
    returnValue = BUILD_SIDECORN7;
  } else if (above == BUILDING && left == BUILDING && right == BUILDING && below == BUILDING && aboveLeft == BUILDING && aboveRight == BUILDING) {
    returnValue = BUILD_SIDECORN8;
  } else if (above == BUILDING && below == BUILDING && left == BUILDING && right != BUILDING && belowLeft == BUILDING && aboveLeft != BUILDING) {
    returnValue = BUILD_SIDECORN9;
  } else if (above == BUILDING && below == BUILDING && right == BUILDING && belowRight == BUILDING && left != BUILDING && aboveRight != BUILDING) {
    returnValue = BUILD_SIDECORN10;
  } else if (above == BUILDING && below == BUILDING && right == BUILDING && aboveRight == BUILDING &&
    belowRight != BUILDING) {
    returnValue = BUILD_SIDECORN11;
  } else if (above == BUILDING && below == BUILDING && left == BUILDING && aboveLeft == BUILDING && belowLeft != BUILDING) {
    returnValue = BUILD_SIDECORN12;

  } else if (right == BUILDING && above == BUILDING && left == BUILDING && below != BUILDING && aboveLeft != BUILDING && aboveRight != BUILDING) {
    returnValue = BUILD_T1;
  } else if (right == BUILDING && below == BUILDING && left == BUILDING && belowLeft != BUILDING && belowRight != BUILDING) {
    returnValue = BUILD_T2;
  } else if (right == BUILDING && above == BUILDING && below == BUILDING && aboveRight != BUILDING && belowRight != BUILDING) {
    returnValue = BUILD_T3;
  } else if (below == BUILDING && above == BUILDING && left == BUILDING && aboveLeft != BUILDING && belowLeft != BUILDING) {
    returnValue = BUILD_T4;  

  } else if (left == BUILDING && right == BUILDING && above == BUILDING && aboveRight == BUILDING && aboveLeft != BUILDING) {
    returnValue = BUILD_SIDECORN1;
  } else if (left == BUILDING && right == BUILDING && above == BUILDING && aboveLeft == BUILDING && aboveRight != BUILDING) {
    returnValue = BUILD_SIDECORN2;
  } else if (left == BUILDING && right == BUILDING && below == BUILDING && belowLeft == BUILDING && belowRight != BUILDING) {
    returnValue = BUILD_SIDECORN3;
  } else if (left == BUILDING && right == BUILDING && below == BUILDING && above != BUILDING && belowRight == BUILDING && belowLeft != BUILDING) { //aboveRight != BUILDING && 
    returnValue = BUILD_SIDECORN4;


  } else if (right == BUILDING && above == BUILDING && below == BUILDING) {
    returnValue = BUILD_SIDE1;
  } else if (left == BUILDING && above == BUILDING && below == BUILDING) {
    returnValue = BUILD_SIDE2;
  } else if (right == BUILDING && left == BUILDING && below == BUILDING) {
    returnValue = BUILD_SIDE3;
  } else if (right == BUILDING && above == BUILDING && left == BUILDING) {
    returnValue = BUILD_SIDE4;

  } else if (right == BUILDING && below == BUILDING && belowRight == BUILDING) {
    returnValue = BUILD_CORNER1;
  } else if (left ==BUILDING && below == BUILDING && belowLeft == BUILDING) {
    returnValue = BUILD_CORNER2;
  } else if (right ==BUILDING && above == BUILDING && aboveRight == BUILDING) {
    returnValue = BUILD_CORNER3;
  } else if (left ==BUILDING && above == BUILDING && aboveLeft == BUILDING) {
    returnValue = BUILD_CORNER4;

  } else if (right == BUILDING && below == BUILDING) {
    returnValue = BUILD_L1;
  } else if (left == BUILDING && below == BUILDING) {
    returnValue = BUILD_L2;
  } else if (right == BUILDING && above == BUILDING) {
    returnValue = BUILD_L3;
  } else if (left == BUILDING && above == BUILDING) {
    returnValue = BUILD_L4;
  } else if (left == BUILDING && right == BUILDING) {
    returnValue = BUILD_HORZ;
  } else if (above == BUILDING && below == BUILDING) {
    returnValue = BUILD_VERT;
  } else if (right == BUILDING) {
    returnValue = BUILD_HORZEND1;
  } else if (left == BUILDING) {
    returnValue = BUILD_HORZEND2;
  } else if (below == BUILDING) {
    returnValue = BUILD_VERTEND1;
  } else if (above == BUILDING) {
    returnValue = BUILD_VERTEND2;
  } else {
    returnValue = BUILD_SINGLE;
  }

  return returnValue;
}

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
BYTE screenCalcRiver(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight) {
  BYTE returnValue; /* Value to return */

  /* Turn Deep Sea and Boats into River */

  if (above == DEEP_SEA || above == BOAT) {
    above = RIVER;
  }
  if (aboveLeft == DEEP_SEA || aboveLeft == BOAT) {
    aboveLeft = RIVER;
  }
  if (aboveRight == DEEP_SEA || aboveRight == BOAT) {
    aboveRight = RIVER;
  }
  if (left == DEEP_SEA || left == BOAT) {
    left = RIVER;
  }
  if (right == DEEP_SEA || right == BOAT) {
    right = RIVER;
  }
  if (below == DEEP_SEA || below == BOAT) {
    below = RIVER;
  }
  if (belowLeft == DEEP_SEA || belowLeft == BOAT) {
    belowLeft = RIVER;
  }
  if (belowRight == DEEP_SEA || belowRight == BOAT) {
    belowRight = RIVER;
  }

  
  if ((above != RIVER && above != ROAD) && (below != RIVER && below != ROAD) && (right != RIVER && right != ROAD) && (left != RIVER && left != ROAD)) {
    returnValue = RIVER_SURROUND;
  } else if ((above != RIVER && above != ROAD) && (below != RIVER && below != ROAD) && right == RIVER && (left != RIVER && left != ROAD)) {  
    returnValue = RIVER_END1;
  } else if ((above != RIVER && above != ROAD) && (below != RIVER && below != ROAD) && (right != RIVER && right != ROAD) && left == RIVER) {
    returnValue = RIVER_END2;
  } else if ((above != RIVER && above != ROAD) && below == RIVER && (right != RIVER && right != ROAD) && (left != RIVER && left != ROAD)) {
    returnValue = RIVER_END3;
  } else if (above == RIVER && (below != RIVER && below != ROAD) &&  (right != RIVER && right != ROAD) && (left != RIVER && left != ROAD)) {
    returnValue = RIVER_END4;

  } else if ((above != RIVER && above != ROAD) && (left != RIVER && left != ROAD)) {
    returnValue = RIVER_CORN1;
  } else if ((above != RIVER && above != ROAD) && (right != RIVER && right != ROAD)) {
    returnValue = RIVER_CORN2;
  } else if ((below != RIVER && below != ROAD) && (left != RIVER && left != ROAD)) {
    returnValue = RIVER_CORN3;
  } else if ((below != RIVER && below != ROAD) && (right != RIVER && right != ROAD)) {
    returnValue = RIVER_CORN4;
  } else if ((below != RIVER && below != ROAD) && (above != RIVER && above != ROAD) && (below != RIVER && below != ROAD)) {
    returnValue = RIVER_SIDE1;
  } else if ((left != RIVER && left  != ROAD) &&  (right != RIVER && right != ROAD)) {
    returnValue = RIVER_SIDE2;
  } else if ((left != RIVER && left != ROAD)) {
    returnValue = RIVER_ONESIDE1;
  } else if ((below != RIVER && below != ROAD)) {
    returnValue = RIVER_ONESIDE2;
  } else if ((right != RIVER && right != ROAD)) {
    returnValue = RIVER_ONESIDE3;
  } else if ((above != RIVER && above != ROAD)) {
    returnValue = RIVER_ONESIDE4;


  } else {
    returnValue = RIVER_SOLID;
  } 
  return returnValue;
}

/*********************************************************
*NAME:          screenCalcDeepSea
*AUTHOR:        John Morrison
*CREATION DATE: 29/10/98
*LAST MODIFIED:  4/11/98
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
BYTE screenCalcDeepSea(BYTE aboveLeft, BYTE above, BYTE aboveRight, BYTE left, BYTE right, BYTE belowLeft, BYTE below, BYTE belowRight) {
  BYTE returnValue; /* Value to return */

  /* Turn Boats into River */

  if (above == BOAT) {
    above = RIVER;
  }
  if (aboveLeft == BOAT) {
    aboveLeft = RIVER;
  }
  if (aboveRight == BOAT) {
    aboveRight = RIVER;
  }
  if (left == BOAT) {
    left = RIVER;
  }
  if (right == BOAT) {
    right = RIVER;
  }
  if (below == BOAT) {
    below = RIVER;
  }
  if (belowLeft == BOAT) {
    belowLeft = RIVER;
  }
  if (belowRight == BOAT) {
    belowRight = RIVER;
  }


  if (aboveLeft != DEEP_SEA && above != DEEP_SEA && left != DEEP_SEA && right == DEEP_SEA && below == DEEP_SEA) {
    returnValue = DEEP_SEA_CORN1;
  } else if (aboveRight != DEEP_SEA && above != DEEP_SEA && right != DEEP_SEA && left == DEEP_SEA && below == DEEP_SEA) {
    returnValue = DEEP_SEA_CORN2;
  } else if (belowRight != DEEP_SEA && below != DEEP_SEA && right != DEEP_SEA && left == DEEP_SEA && above == DEEP_SEA) {
    returnValue = DEEP_SEA_CORN4;
  } else if (belowLeft != DEEP_SEA && below != DEEP_SEA && left != DEEP_SEA && right == DEEP_SEA && above == DEEP_SEA) {
    returnValue = DEEP_SEA_CORN3;
  } else if (left == RIVER && right == DEEP_SEA) {
    returnValue = DEEP_SEA_SIDE1;
  } else if (below == RIVER && above == DEEP_SEA) {
    returnValue = DEEP_SEA_SIDE2;
  } else if (above == RIVER && below == DEEP_SEA) {
    returnValue = DEEP_SEA_SIDE3;
  } else if (right == RIVER && left == DEEP_SEA) {
    returnValue = DEEP_SEA_SIDE4;
  } else {
    returnValue = DEEP_SEA_SOLID;
  }
  return returnValue;
}

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
bool screenLoadMap(BYTE *buff, long length) {
  bool returnValue; /* Value to return */
  
  returnValue = FALSE;
  screenDestroy();
  screenSetup();
  returnValue = mapRead(&buff, &mp, &pb, &bs, &ss, length);
  screenUpdateView(redraw);
  return returnValue;
}


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
bool screenWriteMap(BYTE *buff, long *length) {
  return mapWrite(buff, length, &mp, &pb, &bs, &ss);
}

void screenSetOffset(BYTE x, BYTE y) {
  xOffset = x;
  yOffset = y;
}
BYTE screenGetOffsetX() {
  return xOffset;
}

BYTE screenGetOffsetY() {
  return yOffset;
}

BYTE screenGetNumPills() {
  return pillsGetNumPills(&pb);
}

BYTE screenGetNumBases() {
  return basesGetNumBases(&bs);
}

BYTE screenGetNumStarts() {
  return startsGetNumStarts(&ss);
}


bool screenSetStart(BYTE x, BYTE y) {
  BYTE num;
  start s;
  num = startsGetNumStarts(&ss);
  if (num >= MAX_STARTS) {
    return FALSE;
  }
  basesDeleteBase(&bs, x, y);
  startsDeleteStart(&ss, x, y);
  pillsDeletePill(&pb, x, y);

  mapSetPos(&mp, x, y, DEEP_SEA);
  s.x = x;
  s.y = y;
  s.dir = 0;
  num = startsGetNumStarts(&ss);
  startsSetNumStarts(&ss, (BYTE) (num+1));
  startsSetStart(&ss, &s, (BYTE) (num+1));
  return TRUE;
}

bool screenSetPill(BYTE x, BYTE y) {
  BYTE num;
  pillbox s;
  num = pillsGetNumPills(&pb);
  if (num >= MAX_PILLS) {
    return FALSE;
  }
  basesDeleteBase(&bs, x, y);
  startsDeleteStart(&ss, x, y);
  pillsDeletePill(&pb, x, y);

  mapSetPos(&mp, x, y, ROAD);
  s.x = x;
  s.y = y;
  s.owner = 0xFF;
  s.armour = 15;
  s.speed = 0;
  s.inTank = FALSE;
  num = pillsGetNumPills(&pb);
  pillsSetNumPills(&pb, (BYTE) (num+1));
  pillsSetPill(&pb, &s, (BYTE) (num+1));
  return TRUE;

}

bool screenSetBase(BYTE x, BYTE y) {
  BYTE num;
  base s;

  num = basesGetNumBases(&bs);
  if (num >= MAX_BASES) {
    return FALSE;
  }
  basesDeleteBase(&bs, x, y);
  startsDeleteStart(&ss, x, y);
  pillsDeletePill(&pb, x, y);

  mapSetPos(&mp, x, y, ROAD);
  s.x = x;
  s.y = y;
  s.owner = 0xFF;
  s.armour = 90;
  s.mines = 90;
  s.shells = 90;

  
  num = basesGetNumBases(&bs);
  basesSetNumBases(&bs, (BYTE) (num+1));
  basesSetBase(&bs, &s, (BYTE) (num+1));
  return TRUE;
}

