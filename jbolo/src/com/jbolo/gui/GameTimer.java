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
 * GameTimer.java
 *
 * Created on November 2, 2003, 10:09 PM
 */

package com.jbolo.gui;

import com.jbolo.bolo.GameWorld;
import com.jbolo.bolo.TankButtons;

/**
 * The GameTimer used to call back for updating the game world every tick
 * 
 * @author John Morrison
 */
public class GameTimer extends Timer {
	/** Tank Buttons to pass to game client on update */
	private TankButtons tb;

	/**
	 * Creates a new instance of a GameTimer
	 * 
	 * @param gw
	 *            GameWorld this timer is associated with
	 * @param waitTime
	 *            Number of milliseconds to wait between calling the timer
	 * @param tb
	 *            The tank buttons
	 */
	public GameTimer(GameWorld gw, int waitTime, TankButtons tb) {
		super(gw, waitTime);
		this.tb = tb;

	}

	/**
	 * The process method used to call back for updating the game world every
	 * tick
	 */
	void process() {
		gw.update(tb);
	}

	/**
	 * Called once per second to update and counters required
	 */
	void second() {
		// Nothing required as handled by super class
		gw.getNetwork().second();
	}
}
