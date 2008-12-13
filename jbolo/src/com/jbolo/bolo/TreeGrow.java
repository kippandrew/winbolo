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
 * TreeGrow.java
 *
 * Created on November 30, 2003, 10:29 PM
 */

package com.jbolo.bolo;

import java.util.Random;
import com.jbolo.bolo.map.BoloMap;

/**
 * Provides growing trees.
 * 
 * @author John Morrison
 */
public class TreeGrow {

	/** The time a tree exist before growing unless we find a better spot */
	private static final int TREEGROW_TIME = 3000;
	/**
	 * Initial time delay before we build a tree. This will be overwritten
	 * before it gets called
	 */
	;
	private static final int TREEGROW_INITIAL_TIME = 30000;
	/** Initial scores to beat */
	;
	private static final int TREEGROW_INITIAL_SCORE = -10000;

	/** Weights of a map square */
	;
	private static final int TREE_GROW_FOREST = 100 /* 67 100 */;
	/** Weights of a map square */
	;
	private static final int TREE_GROW_GRASS = 25 /* 10 17 25 */;
	/** Weights of a map square */
	;
	private static final int TREE_GROW_RIVER = 2 /* 7 10 */;
	/** Weights of a map square */
	;
	private static final int TREE_GROW_BOAT = 1 /* 3 5 */;
	/** Weights of a map square */
	;
	private static final int TREE_GROW_DEEP_SEA = 0 /* 1 */;
	/** Weights of a map square */
	;
	private static final int TREE_GROW_DEEP_SWAMP = 2 /* 1 */;
	/** Weights of a map square */
	;
	private static final int TREE_GROW_DEEP_RUBBLE = -2 /* -2 -1 */;
	/** Weights of a map square */
	;
	private static final int TREE_GROW_BUILDING = -20 /* -7 -5 */;
	private static final int TREE_GROW_HALF_BUILDING = -15 /* -7 -5 */;
	private static final int TREE_GROW_CRATER = -2 /* -2 -1 */;
	private static final int TREE_GROW_ROAD = -100 /* -18 * -14 -10 */;
	private static final int TREE_GROW_MINE = -7 /* -7 -5 */;

	/* Score must be greater then 0 for it to grow */
	private static final int TREE_NO_GROW = 0;

	/** Random Number generator. */
	private Random random;

	/** GameWorld that owns this TreeGrow */
	private GameWorld gw;

	/** X map position to draw tree at */
	private int treeGrowX;

	/** Y map position to draw tree at */
	private int treeGrowY;

	/** Time until tree at treeGrowX, treeGrowY is created */
	private int treeGrowTime;

	/** Score of best tree to draw */
	private int treeGrowScore;

	/**
	 * Creates a new instance of TreeGrow
	 * 
	 * @param gw
	 *            GameWorld that owns this tree grow
	 */
	public TreeGrow(GameWorld gw) {
		this.random = new Random();
		this.gw = gw;
		treeGrowTime = TREEGROW_INITIAL_TIME;
		treeGrowScore = TREEGROW_INITIAL_SCORE;
		treeGrowX = 0;
		treeGrowY = 0;
	}

	/**
	 * Game tick has happened. Calculate the growth scores Build new trees etc.
	 */
	public void update() {
		int x; /* The X and Y co-ordinates of the item to add */
		int y;

		/* Get a random map square to calculate for */
		x = random.nextInt(BoloMap.MAP_ARRAY_SIZE + 1);
		y = random.nextInt(BoloMap.MAP_ARRAY_SIZE + 1);

		if (gw.getPills().existPos(x, y) == false
				&& gw.getBases().existPos(x, y) == false
				&& gw.getBoloMap().isMine(x, y) == false
				&& gw.getBoloMap().getPos(x, y) != Terrain.DEEP_SEA) {
			add(x, y, calcScore(x, y));
		}

		/* Update the times and grow trees etc */
		checkGrowTree();
	}

	/**
	 * Called once every tick to see if a tree should be grown and removed from
	 * the data structure.
	 */
	private void checkGrowTree() {
		int pos;
		int newPos;

		treeGrowTime--;
		if (treeGrowTime <= 0) {
			treeGrowScore = TREEGROW_INITIAL_SCORE;
			treeGrowTime = TREEGROW_INITIAL_TIME;
			pos = gw.getBoloMap().getPos(treeGrowX, treeGrowY);
			if (pos != Terrain.RIVER && pos != Terrain.BUILDING
					&& pos != Terrain.HALFBUILDING
					&& gw.getPills().existPos(treeGrowX, treeGrowY) == false
					&& gw.getBases().existPos(treeGrowX, treeGrowY) == false) {
				newPos = Terrain.FOREST;
				if (Terrain.IsMinedTerrain(pos)) {
					newPos += Terrain.MINE_SUBTRACT;
				}
				gw.getBoloMap().setPos(treeGrowX, treeGrowY, newPos, true, false);
				/* Remove Items from grass/swamp/rubble data structures */
				switch (Terrain.TerrainWithoutMine(pos)) {
				case Terrain.GRASS:
					gw.getGrass().removePos(treeGrowX, treeGrowY);
					break;
				case Terrain.SWAMP:
					gw.getSwamp().removePos(treeGrowX, treeGrowY);
					break;
				case Terrain.RUBBLE:
					gw.getRubble().removePos(treeGrowX, treeGrowY);
					break;
				case Terrain.HALFBUILDING:
					gw.getBuildings().removePos(treeGrowX, treeGrowY);
				default:
					/* Do nothing */
					break;
				}
			}
		}

	}

	/**
	 * Adds or updates an item in the treeGrow data structure. Replaces the
	 * score if the score is greater then the one that is already there and
	 * resets the timer to zero
	 * 
	 * @param x
	 *            X position to grow a tree at
	 * @param y
	 *            Y position to grow a tree at
	 * @param score
	 *            Score of this position
	 */
	private void add(int x, int y, int score) {
		if (score > treeGrowScore && score > TREE_NO_GROW) {
			treeGrowScore = score;
			treeGrowX = x;
			treeGrowY = y;
			treeGrowTime = TREEGROW_TIME;
		}
	}

	/**
	 * Calculates the tree growth score for the a specific map location.
	 * 
	 * @return the tree grow score
	 * @param mx
	 *            X position
	 * @param my
	 *            Y position
	 */
	private int calcScore(int mx, int my) {
		int returnValue = 0; // Value to return
		int pos;

		pos = Terrain.TerrainWithoutMine(gw.getBoloMap().getPos(mx, my));
		if (pos != Terrain.FOREST && pos != Terrain.RIVER
				&& pos != Terrain.BUILDING && pos != Terrain.HALFBUILDING
				&& gw.getPills().existPos(mx, my) == false
				&& gw.getBases().existPos(mx, my) == false) {
			/* Ok to do calculation */
			returnValue += calcSquare(mx, my);
			returnValue += calcSquare((mx - 1), (my - 1));
			returnValue += calcSquare((mx - 1), my);
			returnValue += calcSquare((mx - 1), (my + 1));
			returnValue += calcSquare(mx, (my - 1));
			returnValue += calcSquare(mx, (my + 1));
			returnValue += calcSquare((mx + 1), (my - 1));
			returnValue += calcSquare((mx + 1), my);
			returnValue += calcSquare((mx + 1), (my - 1));
			if (returnValue < 0) {
				returnValue = 0;
			}
		}

		return returnValue;
	}

	/**
	 * Calculates the tree growth score for a specific square
	 * 
	 * @return the grow score
	 * @param x
	 *            X map position
	 * @param y
	 *            Y map position
	 */
	private int calcSquare(int x, int y) {
		int returnValue = 0; // Value to return
		int terrain;

		if (gw.getPills().existPos(x, y) == true) {
			returnValue = TREE_GROW_ROAD;
		} else if (gw.getBases().existPos(x, y) == true) {
			returnValue = TREE_GROW_ROAD;
		} else {
			terrain = gw.getBoloMap().getPos(x, y);
			if (Terrain.IsMinedTerrain(terrain)) {
				returnValue = TREE_GROW_MINE;
				terrain -= Terrain.MINE_START;
			}

			switch (terrain) {
			case Terrain.FOREST:
				returnValue += TREE_GROW_FOREST;
				break;
			case Terrain.GRASS:
				returnValue += TREE_GROW_GRASS;
				break;
			case Terrain.RIVER:
				returnValue += TREE_GROW_RIVER;
				break;
			case Terrain.BOAT:
				returnValue += TREE_GROW_BOAT;
				break;
			case Terrain.DEEP_SEA:
				returnValue += TREE_GROW_DEEP_SEA;
				break;
			case Terrain.SWAMP:
				returnValue += TREE_GROW_DEEP_SWAMP;
				break;
			case Terrain.RUBBLE:
				returnValue += TREE_GROW_DEEP_RUBBLE;
				break;
			case Terrain.BUILDING:
				returnValue += TREE_GROW_BUILDING;
				break;
			case Terrain.HALFBUILDING:
				returnValue += TREE_GROW_HALF_BUILDING;
				break;
			case Terrain.CRATER:
				returnValue += TREE_GROW_CRATER;
				break;
			case Terrain.ROAD:
				returnValue += TREE_GROW_ROAD;
				break;
			default:
				returnValue += TREE_GROW_ROAD;
				break;
			}
		}

		return returnValue;
	}
}
