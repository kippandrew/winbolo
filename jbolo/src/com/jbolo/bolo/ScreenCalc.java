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


/*
 * ScreenCalc.java
 *
 * Created on October 5, 2003, 7:33 PM
 */

package com.jbolo.bolo;

/** The ScreenCalc is used to convert a Terrain into a TileNum for drawing
 * @author John Morrison
 */
public class ScreenCalc extends TileNum {
    
    /** Creates a new instance of ScreenCalc */
    public ScreenCalc() {
    }
    
    
    /** Returns the road type of a square
     * @param aboveLeft The square above left
     * @param above The square above
     * @param aboveRight The square above right
     * @param left The square left
     * @param right The square above right
     * @param belowLeft The square below left
     * @param below The square above below
     * @param belowRight The square below right
     * @return Returns the road type of a square
     */
    public static int ScreenCalcRoad(int aboveLeft, int above, int aboveRight, int left, int right, int belowLeft, int below, int belowRight) {
	int returnValue; /* Value to return */
	
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
	} else if (right == ROAD && above == ROAD  && left == RIVER && below == RIVER) {
	    returnValue = ROAD_WATER8;
	} else if(above == RIVER && below == RIVER) { //(left == ROAD || right == ROAD) && above == RIVER && below == RIVER
	    returnValue = ROAD_WATER9;
	} else if(left == RIVER && right == RIVER) { //(above == ROAD || below == ROAD) &&
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
    
    /** Returns the boat type of a square
     * @param aboveLeft The square above left
     * @param above The square above
     * @param aboveRight The square above right
     * @param left The square left
     * @param right The square above right
     * @param belowLeft The square below left
     * @param below The square above below
     * @param belowRight The square below right
     * @return Returns the boat type of a square
     */
    public static int ScreenCalcBoat(int aboveLeft, int above, int aboveRight, int left, int right, int belowLeft, int below, int belowRight) {
	int returnValue; /* Value to return */
	
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
	
	if (above != RIVER && left != RIVER) {
	    returnValue = BOAT_5;
	} else if (above != RIVER && right != RIVER) {
	    returnValue = BOAT_6;
	} else if (below != RIVER && right != RIVER) {
	    returnValue = BOAT_7;
	} else if (below != RIVER && left != RIVER) {
	    returnValue = BOAT_4;
	} else if (left != RIVER) {
	    returnValue = BOAT_2;
	} else if (right != RIVER) {
	    returnValue = BOAT_3;
	} else if (below != RIVER) {
	    returnValue = BOAT_0;
	} else {
	    returnValue = BOAT_1;
	}
	return returnValue;
    }
    
    /** Returns the building type
     * @param aboveLeft The square above left
     * @param above The square above
     * @param aboveRight The square above right
     * @param left The square left
     * @param right The square above right
     * @param belowLeft The square below left
     * @param below The square above below
     * @param belowRight The square below right
     * @return Returns the building type
     */
    public static int ScreenCalcBuilding(int aboveLeft, int above, int aboveRight, int left, int right, int belowLeft, int below, int belowRight) {
	int returnValue; /* Value to return */
	
	/* Turn HalfBuilding into building */
	if (above == HALFBUILDING) {
	    above = BUILDING;
	}
	if (aboveLeft == HALFBUILDING) {
	    aboveLeft = BUILDING;
	}
	if (aboveRight == HALFBUILDING) {
	    aboveRight = BUILDING;
	}
	if (left == HALFBUILDING) {
	    left = BUILDING;
	}
	if (right == HALFBUILDING) {
	    right = BUILDING;
	}
	if (below == HALFBUILDING) {
	    below = BUILDING;
	}
	if (belowLeft == HALFBUILDING) {
	    belowLeft = BUILDING;
	}
	if (belowRight == HALFBUILDING) {
	    belowRight = BUILDING;
	}
	
	
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
	} else if (left == BUILDING && right == BUILDING && below == BUILDING && above != BUILDING && belowRight == BUILDING && belowLeft != BUILDING) {
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
    
    /** Returns the sea type of a square
     * @param aboveLeft The square above left
     * @param above The square above
     * @param aboveRight The square above right
     * @param left The square left
     * @param right The square above right
     * @param belowLeft The square below left
     * @param below The square above below
     * @param belowRight The square below right
     * @return Returns the river type of a square
     */
    public static int ScreenCalcDeepSea(int aboveLeft, int above, int aboveRight, int left, int right, int belowLeft, int below, int belowRight) {
	int returnValue; /* Value to return */
	
	
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
    
    /** Returns the river type of a square
     * @param aboveLeft The square above left
     * @param above The square above
     * @param aboveRight The square above right
     * @param left The square left
     * @param right The square above right
     * @param belowLeft The square below left
     * @param below The square above below
     * @param belowRight The square below right
     * @return Returns the river type of a square
     */
    public static int ScreenCalcRiver(int aboveLeft, int above, int aboveRight, int left, int right, int belowLeft, int below, int belowRight) {
	int returnValue; /* Value to return */
	
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
	
	if ((above != RIVER && above != ROAD) && (below != RIVER && below != ROAD) && (right != RIVER && right != ROAD) && (left != RIVER
	&& left != ROAD)) {
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
    
    /** Returns the forest type of a square
     * @param aboveLeft The square above left
     * @param above The square above
     * @param aboveRight The square above right
     * @param left The square left
     * @param right The square above right
     * @param belowLeft The square below left
     * @param below The square above below
     * @param belowRight The square below right
     * @return Returns the forest type of a square
     */
    public static int ScreenCalcForest(int aboveLeft, int above, int aboveRight, int left, int right, int belowLeft, int below, int belowRight) {
	/* NOTE: belowLeft, belowRight, aboveLeft, aboveRight UNUSED */
	int returnValue; /* Value to return */
	
	returnValue = FOREST;
	if (above != FOREST && left != FOREST && right == FOREST && below == FOREST) {
	    returnValue = FOREST_BR;
	} else if (above != FOREST && left == FOREST && right != FOREST && below == FOREST) {
	    returnValue = FOREST_BL;
	} else if (above == FOREST && left == FOREST && right != FOREST && below != FOREST) {
	    returnValue = FOREST_AL;
	} else if (above == FOREST && left != FOREST && right == FOREST && below != FOREST) {
	    returnValue = FOREST_AR;
	} else if (above == FOREST && left != FOREST && right != FOREST && below != FOREST) {
	    returnValue = FOREST_ABOVE;
	} else if (above != FOREST && left != FOREST && right != FOREST && below == FOREST) {
	    returnValue = FOREST_BELOW;
	} else if (above != FOREST && left == FOREST && right != FOREST && below != FOREST) {
	    returnValue = FOREST_LEFT;
	} else if (above != FOREST && left != FOREST && right == FOREST && below != FOREST) {
	    returnValue = FOREST_RIGHT;
	} else if (above != FOREST && below != FOREST && left != FOREST && right != FOREST) {
	    returnValue = FOREST_SINGLE;
	}
	return returnValue;
    }
    
    /** Returns the crater type of a square
     * @param aboveLeft The square above left
     * @param above The square above
     * @param aboveRight The square above right
     * @param left The square left
     * @param right The square above right
     * @param belowLeft The square below left
     * @param below The square above below
     * @param belowRight The square below right
     * @return Returns the crater type of a square
     */
    public static int ScreenCalcCrater(int aboveLeft, int above, int aboveRight, int left, int right, int belowLeft, int below, int belowRight) {
	/* NOTE: belowLeft, belowRight, aboveLeft, aboveRight UNUSED */
	int returnValue; /* Value to return */
	
	returnValue = CRATER;
	
/*	if (above != CRATER && left != CRATER && right == CRATER && below == CRATER) {
	    returnValue = CRATER_BR;
	} else if (above != CRATER && left == CRATER && right != CRATER && below == CRATER) {
	    returnValue = CRATER_BL;
	} else if (above == CRATER && left == CRATER && right != CRATER && below != CRATER) {
	    returnValue = CRATER_AL;
	} else if (above == CRATER && left != CRATER && right == CRATER && below != CRATER) {
	    returnValue = CRATER_AR;
	} else if (above == CRATER && left != CRATER && right != CRATER && below != CRATER) {
	    returnValue = CRATER_ABOVE;
	} else if (above != CRATER && left != CRATER && right != CRATER && below == CRATER) {
	    returnValue = CRATER_BELOW;
	} else if (above != CRATER && left == CRATER && right != CRATER && below != CRATER) {
	    returnValue = CRATER_LEFT;
	} else if (above != CRATER && left != CRATER && right == CRATER && below != CRATER) {
	    returnValue = CRATER_RIGHT;
	} else if (above != CRATER && below != CRATER && left != CRATER && right != CRATER) {
	    returnValue = CRATER_SINGLE;
	} */
	return returnValue;
    }
}
