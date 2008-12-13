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
 * FrameTimer.java
 *
 * Created on November 2, 2003, 7:37 PM
 */

package com.jbolo.gui;

import com.jbolo.bolo.GameWorld;

/**
 * Implement the timer for redrawing the main view
 * 
 * @author John Morrison
 */
public class FrameTimer extends Timer {
	/** The frame rate for last second */
	private int lastFrameRate;
	/** The current number of frames drawn this second */
	private int frameRate;
	/** Drawing object */
//	private Draw draw;

	/**
	 * Creates a new instance of a Timer
	 * 
	 * @param gw
	 *            GameWorld this timer is associated with
	 * @param waitTime
	 *            Number of milliseconds to wait between calling the timer
	 */
	public FrameTimer(GameWorld gw, Draw draw, int waitTime) {
		super(gw, waitTime);
//		this.draw = draw;
		frameRate = 0;
		lastFrameRate = 0;
	}

	/**
	 * The threads process method
	 */
	void process() {
		gw.screenUpdate(0);
		frameRate++;
	}

	/**
	 * Gets the frame rate for the last second
	 * 
	 * @return frame rate for the last second
	 */
	public int getFrameRate() {
		return lastFrameRate;
	}

	/**
	 * Called once per second to update and counters required
	 */
	void second() {
		lastFrameRate = frameRate;
		frameRate = 0;
	}

}
