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
 * Alliance.java
 *
 * Created on October 5, 2003, 10:46 AM
 */

package com.jbolo.bolo;

import java.util.HashSet;
import java.io.ByteArrayOutputStream;

/**
 * Represents Alliance between tanks.
 * 
 * @author John Morrison
 */
public class Alliance {

	/** Internal hashset of current allies. */
	HashSet<Integer> allies;

	/** Creates a new instance of Alliance. */
	public Alliance() {
		allies = new HashSet<Integer>();
	}

	/**
	 * Adds a player to the list of known alliance members
	 * 
	 * @param player
	 *            Player number to add
	 */
	public void add(int player) {
		Integer in = Integer.valueOf(player);
		allies.add(in);
	}

	/**
	 * Removes player from the list of alliances.
	 * 
	 * @param player
	 *            Player to remove.
	 */
	public void remove(int player) {
		Integer in = Integer.valueOf(player);
		allies.remove(in);
	}

	/**
	 * Checks to see if the player exists in the alliance.
	 * 
	 * @param player
	 *            Player to check if they are in the alliance.
	 * @return If the player exists in the player
	 */
	public boolean exist(int player) {
		Integer test = Integer.valueOf(player);

		return allies.contains(test);
	}

	/**
	 * Gets the number of allies this player has
	 * 
	 * @return number of allies this player has
	 */
	public int getCount() {
		return allies.size();
	}

	/**
	 * Makes this net alliances as a byte array
	 * 
	 * @return a byte array containing the list of allies. The array size is the
	 *         number of allies
	 */
	public byte[] makeNetAlliances() {
		ByteArrayOutputStream bos = new ByteArrayOutputStream(15);

		for (Integer i : allies) {
			bos.write(i.byteValue());
		}
		return bos.toByteArray();
	}

	/**
	 * Removes all alliances we have
	 */
	public void clearAllAlliances() {
		allies.clear();
	}
}
