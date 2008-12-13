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
 * FloodFillTimedTerrain.java
 *
 * Created on October 6, 2003, 7:45 AM
 */

package com.jbolo.bolo;

import com.jbolo.bolo.map.BoloMap;

/**
 * FloodFillTimedTerrain is the class for all flood filling of craters
 * 
 * @author John Morrison
 */
public class FloodFillTimedTerrain extends TimedTerrain {

	/** Time to wait between flood filling a crater */
	private static int WAIT_TIME = 16;

	/**
	 * Creates a new instance of FloodFillTimedTerrain
	 * 
	 * @param gw
	 *            GameWorld this belongs to
	 */
	public FloodFillTimedTerrain(GameWorld gw) {
		super(gw, WAIT_TIME);
	}

	/**
	 * checkAddItem checks the terrain at position X, Y to see if it should be
	 * added to the queue
	 * 
	 * @param terrain
	 *            Terrain of the square
	 * @param x
	 *            X map position of the event
	 * @param y
	 *            Y map position of the event
	 */
	private void checkAddItem(int terrain, int x, int y) {
		if (terrain == Terrain.CRATER || terrain == Terrain.MINE_CRATER) {
			this.addWaiting(x, y);
		}
	}

	/**
	 * processCheckSurroundings processes the surrounding squares to see if they
	 * should be processed
	 * 
	 * @param mp
	 *            The Bolo map these items are on
	 * @param pbs
	 *            Pillboxes on this map
	 * @param bss
	 *            Bases on this map
	 * @param x
	 *            X map position of this event
	 * @param y
	 *            Y map position of this event
	 * @param above
	 *            Above of the event
	 * @param below
	 *            Below of the event
	 * @param left
	 *            Left of the event
	 * @param right
	 *            Right of the event
	 */
	void processCheckSurroundings(BoloMap mp, Pillboxes pbs, Bases bss, int x,
			int y, int above, int below, int left, int right) {
		if (left == Terrain.DEEP_SEA || left == Terrain.BOAT
				|| left == Terrain.RIVER || right == Terrain.DEEP_SEA
				|| right == Terrain.BOAT || right == Terrain.RIVER
				|| above == Terrain.DEEP_SEA || above == Terrain.RIVER
				|| above == Terrain.BOAT || below == Terrain.DEEP_SEA
				|| below == Terrain.BOAT || below == Terrain.RIVER) {
			mp.setPos(x, y, Terrain.RIVER, false, false);
			gw.getMines().remove(x, y);

			checkAddItem(left, x - 1, y);
			checkAddItem(right, x + 1, y);
			checkAddItem(above, x, y - 1);
			checkAddItem(below, x, y + 1);
			gw.reCalc();
		}
	}
}
