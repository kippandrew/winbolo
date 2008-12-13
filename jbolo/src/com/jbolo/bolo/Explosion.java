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
 * Explosion.java
 *
 * Created on December 25, 2003, 10:47 PM
 */

package com.jbolo.bolo;

/**
 * 
 * @author John Morrison
 */
public class Explosion {

	/** Start Frame of an Explosion */
	public static final int START = 8;

	/** X Map position of the explosion */
	protected int mx;
	/** Y Map position of the explosion */
	protected int my;
	/** X Pixel position of the explosion */
	protected int px;
	/** Y Pixel position of the explosion */
	protected int py;
	/** Length the explosion has been alive */
	protected int length;

	/**
	 * Creates a new instance of Explosion
	 * 
	 * @param mx
	 *            X Map position of the explosion
	 * @param my
	 *            Y Map position of the explosion
	 * @param px
	 *            X Pixle position of the explosion
	 * @param py
	 *            Y Pixel position of the explosion
	 * @param startPos
	 *            How far through this explosion do we start
	 */
	public Explosion(int mx, int my, int px, int py, int startPos) {
		this.mx = mx;
		this.my = my;
		this.px = px;
		this.py = py;
		length = startPos;
	}

	public int getMX() {
		return mx;
	}

	public int getMY() {
		return my;
	}

	public int getPX() {
		return px;
	}

	public int getPY() {
		return py;
	}

	public void decrementLength() {
		length--;
	}

	/**
	 * Returns the length of this explosion
	 * 
	 * @return length of the explosion
	 */
	public int getLength() {
		return length;
	}

	/**
	 * Returns if this explosion can be seen on the screen
	 * 
	 * @return whether this explosion can be seen on the screen
	 * @param left
	 *            Left position of the screen
	 * @param right
	 *            Right position of the screen
	 * @param top
	 *            Top position of the screen
	 * @param bottom
	 *            Bottom position of the screen
	 */
	public boolean isInRect(int left, int right, int top, int bottom) {
		if (mx >= left && mx <= right && my >= top && my <= bottom) {
			return true;
		}
		return false;
	}
}
