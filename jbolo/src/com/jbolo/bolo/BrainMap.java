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
 * BrainMap.java
 *
 * Created on November 29, 2003, 9:38 PM
 */

package com.jbolo.bolo;

import com.jbolo.bolo.map.BoloMap;

/**
 * Brain Map is used to hold a copy of the games map for AI players
 * 
 * @author John Morrison
 */
public class BrainMap {
	/** TERRAIN_UNKOWN is what a brain sees unless the terrain is known */
	private static final int TERRAIN_UNKNOWN = 20;

	/** BDEEPSEA is what a brain sees unless the terrain is deep sea */
	private static final int BDEEPSEA = 18;

	/** TERRAIN_MINE is a terrain with a mine on it */
	private static final int TERRAIN_MINE = 0x80;

	/** sbm conatins the the map for brain */
	private int[][] sbm;

	/** Creates a new instance of BrainMap */
	public BrainMap() {
		int count;
		int count2;

		sbm = new int[BoloMap.MAP_ARRAY_SIZE][BoloMap.MAP_ARRAY_SIZE];
		for (count = 0; count < BoloMap.MAP_ARRAY_SIZE; count++) {
			for (count2 = 0; count2 < BoloMap.MAP_ARRAY_SIZE; count2++) {
				sbm[count][count2] = TERRAIN_UNKNOWN;
			}
		}
	}

	/**
	 * Sets a position in the data structure
	 * 
	 * @param xValue
	 *            The X Value of the position to set
	 * @param yValue
	 *            The Y Value of the position to set
	 * @param terrain
	 *            Terrain to set it to
	 * @param isMine
	 *            Is this square mined
	 */
	public void setPos(int xValue, int yValue, int terrain, boolean isMine) {
		sbm[yValue][xValue] = Terrain.TerrainWithoutMine(terrain);
		if (terrain == Terrain.DEEP_SEA) {
			sbm[yValue][xValue] = BDEEPSEA;
		}

		if (isMine == true) {
			sbm[yValue][xValue] |= TERRAIN_MINE;
		}
	}
}
