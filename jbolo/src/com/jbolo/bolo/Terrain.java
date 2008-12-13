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


package com.jbolo.bolo;

/*
 * Terrain.java
 *
 * Created on October 5, 2003, 9:51 AM
 */

/**
 * Static class that defines the Bolo Terrain types.
 * 
 * @author John Morrison
 */
public class Terrain {

	/** DEEP_SEA terrain */
	public static final int DEEP_SEA = 255;
	/** BUILDING terrain */
	public static final byte BUILDING = 0;
	/** RIVER terrain */
	public static final byte RIVER = 1;
	/** SWAMP terrain */
	public static final byte SWAMP = 2;
	/** CRATER terrain */
	public static final byte CRATER = 3;
	/** ROAD terrain */
	public static final byte ROAD = 4;
	/** FOREST terrain */
	public static final byte FOREST = 5;
	/** RUBBLE terrain */
	public static final byte RUBBLE = 6;
	/** GRASS terrain */
	public static final byte GRASS = 7;
	/** HALFBUILDING terrain */
	public static final byte HALFBUILDING = 8;
	/** BOAT terrain */
	public static final byte BOAT = 9;
	/** SWAMP with a mine on it terrain */
	public static final byte MINE_SWAMP = 10;
	/** CRATER with a mine on it terrain */
	public static final byte MINE_CRATER = 11;
	/** ROAD with a mine on it terrain */
	public static final byte MINE_ROAD = 12;
	/** FOREST with a mine on it terrain */
	public static final byte MINE_FOREST = 13;
	/** RUBBLE with a mine on it terrain */
	public static final byte MINE_RUBBLE = 14;
	/** GRASS with a mine on it terrain */
	public static final byte MINE_GRASS = 15;
	/** Where the mines terrain start */
	public static final byte MINE_START = 10;
	/** Where the mines end terrain */
	public static final byte MINE_END = 15;
	/** Amount to subtracted from a mined terrain to make it the normal terrain */
	public static final byte MINE_SUBTRACT = 8;

	/**
	 * Checks if this terrain contains a mine
	 * 
	 * @return if this terrain has a mine on it
	 * @param terrain
	 *            Terrain type to check
	 */
	public static boolean IsMinedTerrain(int terrain) {
		if (terrain >= MINE_START && terrain <= MINE_END) {
			return true;
		}
		return false;
	}

	/**
	 * Gets the terrain without a mine on it
	 * 
	 * @return the terrain without a mine on it
	 * @param terrain
	 *            Terrain to get without a mine
	 */
	public static int TerrainWithoutMine(int terrain) {
		if (terrain >= MINE_START && terrain <= MINE_END) {
			return (terrain - MINE_SUBTRACT);
		}
		return terrain;
	}

	/** Creates a new instance of Terrain */
	public Terrain() {
	}

}
