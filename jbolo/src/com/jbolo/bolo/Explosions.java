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
 * Explosions.java
 *
 * Created on December 25, 2003, 10:57 PM
 */

package com.jbolo.bolo;

import java.util.ArrayList;
import java.util.Iterator;

/**
 * Explosions contains the list of all explosions in the game. Explosions are
 * graphics only and do not cause any other game events to occur
 * 
 * @author John Morrison
 */
public class Explosions {

	/** Time when explosions expire */
	private static int EXPLODE_DEATH = 1;

	/** Explosions should only be updated every 3 ticks */
	private static int UPDATE_TIME = 3;

	/** ArrayList of Explosion objects */
	private ArrayList<Explosion> items;

	/** Time between explosion frame updates to prevent it going too quickly */
	private int updateTime;

	/** Creates a new instance of Explosions */
	public Explosions() {
		items = new ArrayList<Explosion>(64);
		updateTime = 0;
	}

	/**
	 * Called once every game tick to update the explosions frame and to remove
	 * the expired ones
	 */
	public void update() {
		Explosion exp;
		Iterator<Explosion> li;

		updateTime++;
		if (updateTime != UPDATE_TIME) {
			return;
		}
		updateTime = 0;

		
		li = items.iterator();
		while (li.hasNext()) {
			exp = (Explosion) li.next();
			if (exp.getLength() > EXPLODE_DEATH) {
				exp.decrementLength();
			} else {
				li.remove();
			}
		}
	}

	/**
	 * Adds an explosion to the explosions in our game world
	 * 
	 * @param mx
	 *            X Map position of the explosion
	 * @param my
	 *            Y Map position of the explosion
	 * @param px
	 *            X Pixel position of the explosion
	 * @param py
	 *            Y Pixel position of the explosion
	 * @param startPos
	 *            How far through this explosion do we start
	 */
	public void add(int mx, int my, int px, int py, int startPos) {
		Explosion add;
		add = new Explosion(mx, my, px, py, startPos);
		items.add(add);
	}

	/**
	 * Creates the list of explosions that can be seen on the screen
	 * 
	 * @return ArrayList of ScreenItems
	 * @param leftPos
	 *            Left position of the screen
	 * @param rightPos
	 *            Right position of the screen
	 * @param topPos
	 *            Top position of the screen
	 * @param bottomtPos
	 *            Bottom position of the screen
	 */
	public ArrayList<ScreenItem> getScreenItems(int left, int right, int top, int bottom) {
		ArrayList<ScreenItem> returnValue = new ArrayList<ScreenItem>();
		ScreenItem add;
		
		for (Explosion exp : items) {
			if (exp.isInRect(left, right, top, bottom) == true) {
				add = new ScreenItem(exp.getMX() - left, exp.getMY() - top, exp
						.getPX(), exp.getPY(), exp.getLength());
				returnValue.add(add);
			}
		}
		return returnValue;
	}
}
