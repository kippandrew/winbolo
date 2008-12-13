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
 * GameWorldItem.java
 *
 * Created on October 9, 2003, 5:05 PM
 */

package com.jbolo.bolo;

/**
 * GameWorldItems represent items with world units as their basis
 * 
 * @author John Morrison
 * @see Tank
 * @see Shell
 */
public class GameWorldItem {

	/**
	 * Amount to subtract from the middle to get the left co-ordinate For
	 * drawing
	 */
	private static int MIDDLE = 128;

	/** The items world x position */
	protected int x;

	/** The items world y position */
	protected int y;

	/** Creates a new instance of GameWorldItem */
	public GameWorldItem() {
		x = 0;
		y = 0;
	}

	/**
	 * Creates a new instance of GameWorldItem
	 * 
	 * @param x
	 *            X position to set
	 * @param y
	 *            Y position to set
	 */
	public GameWorldItem(int x, int y) {
		this.x = x;
		this.y = y;
	}

	/**
	 * Returns the items X position in world units
	 * 
	 * @return X position in world units
	 */
	public int getWorldX() {
		return x;
	}

	/**
	 * Returns the items Y position in world units
	 * 
	 * @return Items Y position in world units
	 */
	public int getWorldY() {
		return y;
	}

	/**
	 * Returns the map X position of the item
	 * 
	 * @return The items map X position
	 */
	public int getMX() {
		return x >> 8;
	}

	/**
	 * Returns the map X position for the screen (takes off half a map square)
	 * 
	 * @return The items screen map X position
	 */
	public int getScreenMX() {
		return (x - MIDDLE) >> 8;
	}

	/**
	 * Returns the map Y position for the screen (takes off half a map square)
	 * 
	 * @return The items screen map Y position
	 */
	public int getScreenMY() {
		return (y - MIDDLE) >> 8;
	}

	/**
	 * Returns the map Y position of the item
	 * 
	 * @return The items map Y position
	 */
	public int getMY() {
		return y >> 8;
	}

	/**
	 * Returns the pixel X position of the item
	 * 
	 * @return The items pixel x position
	 */
	public int getPX() {
		return (x & 0x00F0) >> 4;
	}

	/**
	 * Returns the pixel Y position of the item
	 * 
	 * @return The items Y pixel position
	 */
	public int getPY() {
		return (y & 0x00F0) >> 4;
	}

	/**
	 * Returns the pixel X position for the screen (takes off half a map square)
	 * 
	 * @return The items screen pixel X position
	 */
	public int getScreenPX() {
		return ((x - MIDDLE) & 0x00F0) >> 4;
	}

	/**
	 * Returns the pixel Y position for the screen (takes off half a map square)
	 * 
	 * @return The items screen pixel Y position
	 */
	public int getScreenPY() {
		return ((y - MIDDLE) & 0x00F0) >> 4;
	}

	/**
	 * Returns the items X position in world units
	 * 
	 * @return X position in world units
	 */
	public int getX() {
		return getWorldX();
	}

	/**
	 * Returns the items Y position in world units
	 * 
	 * @return Y position in world units
	 */
	public int getY() {
		return getWorldY();
	}

	/**
	 * Sets the X position in world units
	 * 
	 * @param x
	 *            X position in world units
	 */
	public void setX(int x) {
		this.x = x;
	}

	/**
	 * Sets the Y position in world units
	 * 
	 * @param y
	 *            Y position in world units
	 */
	public void setY(int y) {
		this.y = y;
	}

	public String toString() {
		return "WorldX: " + x + ", WorldY: " + y + ", MX: " + getMX()
				+ ", MY: " + getMY() + ", PX: " + getPX() + ", PY: " + getPY();
	}
}
