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


package com.jbolo.bolo;

import java.util.ArrayList;
import java.util.Iterator;

/*
 * ChangeableTerrain.java
 *
 * Created on October 5, 2003, 9:57 AM
 */

/**
 * ChanganbleTerrain looks after terrain that has a hit counter, such as grass
 * turning into swamp, swamp into river, buildings into half buildings half
 * buildings into rubble
 * 
 * @provides bolo/buildings.c bolo/swamp.c bolo/grass.c
 * @author John Morrison
 * @see ChangeableTerrainItem.java
 */
public class ChangeableTerrain {
	/** length of the event */
	private int length;
	/** What terrain to change this square into when length is reached */
	private int change;
	/** original terrain of this item */
	private int original;
	/** The list of items in this ChangeableTerrain */
	private ArrayList<ChangeableTerrainItem> items;

	/**
	 * Creates a new instance of ChangeableTerrain
	 * 
	 * @param length
	 *            Length of the item before it changes
	 * @param change
	 *            The terrain to change into
	 * @param original
	 *            Original terrain of this item
	 */
	public ChangeableTerrain(int length, int change, int original) {
		items = new ArrayList<ChangeableTerrainItem>();
		this.length = length;
		this.change = change;
		this.original = original;
	}

	/**
	 * addItem adds a item to this ChangeableTerrainItem. If the item exists the
	 * items length counter is increased. If it reaches the maximum length it is
	 * removed and the terrain is changed
	 * 
	 * @param x
	 *            X position of the item
	 * @param y
	 *            Y position of the item
	 * @return the terrain of the position
	 */
	public int addItem(int x, int y) {
		ChangeableTerrainItem cti;
		int returnValue = original;
		boolean found = false;
		Iterator<ChangeableTerrainItem> li = items.listIterator();

		if (length == 1) {
			returnValue = change;
		} else {
			while (found == false && li.hasNext()) {
				cti = li.next();
				if (cti.exists(x, y) == true) {
					found = true;
					cti.increment();
					if (cti.getLength() >= length) {
						returnValue = change;
						li.remove();
					}
				}
			}

			if (found == false) {
				cti = new ChangeableTerrainItem(x, y);
				items.add(cti);
			}
		}
		return returnValue;
	}

	/**
	 * Removes the counter on this terrain position
	 * 
	 * @param x
	 *            X position
	 * @param y
	 *            Y position
	 */
	public void removePos(int x, int y) {
		ChangeableTerrainItem cti;
		boolean found = false;
		Iterator<ChangeableTerrainItem> li = items.iterator();

		while (found == false && li.hasNext()) {
			cti = li.next();
			if (cti.exists(x, y) == true) {
				li.remove();
				found = true;
			}
		}
	}
}
