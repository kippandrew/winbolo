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
 * GameType.java
 *
 * Created on October 12, 2003, 10:28 AM
 */

package com.jbolo.bolo;

/**
 * GameType is responsible for storing the game type (open, tournament strict
 * tournament) as well as calculating the number of supplies that are given to a
 * new tank
 * 
 * @author John Morrison
 */
public class GameType {

	/** An open game */
	public static final int OPEN_GAME_TYPE = 1;

	/** A tournament game */
	public static final int STRICT_GAME_TYPE = 3;

	/** A strict tournament game */
	public static final int TOURNAMENT_GAME_TYPE = 2;

	/** A full load of armour on the tank */
	private static int FULL_ARMOUR = 40;

	/** A full load of shells on the tank */
	private static int FULL_SHELLS = 40;

	/** A full load of mines on the tank */
	private static int FULL_MINES = 40;

	/** A full load of trees on the tank */
	private static int FULL_TREES = 40;

	/** The game type of this game */
	private int gameType;

	/** Game World this belongs to */
	private GameWorld gw;

	/**
	 * Creates a new instance of GameType
	 * 
	 * @param gw
	 *            GameWorld this belongs to
	 * @param gameType
	 *            The game type of this game
	 */
	public GameType(GameWorld gw, int gameType) {
		this.gameType = gameType;
		this.gw = gw;
	}

	/**
	 * Creates an instance of game type
	 * 
	 * @param gameType
	 *            The game type of the game
	 */
	void set(int gameType) {
		this.gameType = gameType;
	}

	/**
	 * getType returns the game type of the game
	 * 
	 * @return the game type of the game One of OPEN_GAME, STRICT_GAME,
	 *         TOURNAMENT_GAME
	 */
	public int getType() {
		return gameType;
	}

	/**
	 * Returns the number of shells a new tank gets
	 * 
	 * @return The amount of shells
	 */
	public int getShells() {
		int numBases;
		double percent;
		Bases bss;

		if (gameType == OPEN_GAME_TYPE) {
			return FULL_SHELLS;
		} else if (gameType == STRICT_GAME_TYPE) {
			return 0;
		} else {
			bss = gw.getBases();
			numBases = bss.getNumItems();
			if (numBases == 0) {
				numBases = 1;
			}
			percent = bss.getNumNeutral();
			percent /= numBases;
			return (int) (2.0 * percent);
		}
	}

	/**
	 * Returns the number of mines a new tank gets
	 * 
	 * @return The amount of mines
	 */
	public int getMines() {
		if (gameType == OPEN_GAME_TYPE) {
			return FULL_MINES;
		}
		return 0;
	}

	/**
	 * Returns the number of armour a new tank gets
	 * 
	 * @return The amount of armour
	 */
	public int getArmour() {
		return FULL_ARMOUR;
	}

	/**
	 * Returns the number of trees a new tank gets
	 * 
	 * @return The number of trees
	 */
	public int getTrees() {
		if (gameType == OPEN_GAME_TYPE) {
			return FULL_TREES;
		}
		return 0;
	}
}
