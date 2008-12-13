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
import java.util.ListIterator;
import com.jbolo.bolo.map.BoloMap;

/*
 * TimedTerrain.java
 *
 * Created on October 5, 2003, 5:11 PM
 */

/**
 * TimedTerrain is the super class for all terrain triggers, floodfill, MinesExp
 * 
 * @author John Morrison
 */
public abstract class TimedTerrain {

	/** List of items in this timed terrain */
	protected ArrayList<ChangeableTerrainItem> items;

	/** Life of the timed item before it triggers */
	protected int length;

	/** GameWorld that owns this TimedTerrain */
	protected GameWorld gw;

	/** List of items in this timed terrain which were added during an update */
	private ArrayList<ChangeableTerrainItem> itemsWaiting;

	/**
	 * Creates a new instance of TimedTerrain
	 * 
	 * @param gw
	 *            GameWorld that owns this TimedTerrain
	 * @param length
	 *            Life of the timed item before it triggers
	 */
	public TimedTerrain(GameWorld gw, int length) {
		items = new ArrayList<ChangeableTerrainItem>();
		itemsWaiting = new ArrayList<ChangeableTerrainItem>();
		this.length = length;
		this.gw = gw;
	}

	/**
	 * add adds an event at position X, Y to this timed event if it doesn't exist
	 * already.
	 * 
	 * @param x
	 *            X map position of the event.
	 * @param y
	 *            Y map position of the event.
	 */
	public void add(int x, int y) {
		Iterator<ChangeableTerrainItem> li = items.iterator();
		boolean found = false;
		ChangeableTerrainItem cti;

		while (found == false && li.hasNext()) {
			cti = li.next();
			if (x == cti.getX() && cti.getY() == y) {
				found = true;
			}
		}

		if (found == false) {
			cti = new ChangeableTerrainItem(x, y);
			items.add(cti);
		}
	}

	/**
	 * add adds an event at position X, Y to this timed event if it doesn't exist
	 * already. Called when events are added via the process abstract method to
	 * prevent concurrent exceptions
	 * 
	 * @param x
	 *            X map position of the event.
	 * @param y
	 *            Y map position of the event.
	 */
	protected void addWaiting(int x, int y) {
		ListIterator<ChangeableTerrainItem> li = items.listIterator();
		boolean found = false;
		ChangeableTerrainItem cti;

		while (found == false && li.hasNext()) {
			cti = li.next();
			if (x == cti.getX() && cti.getY() == y) {
				found = true;
			}
		}

		if (found == false) {
			cti = new ChangeableTerrainItem(x, y);
			itemsWaiting.add(cti);
		}
	}

	/**
	 * update is called every game tick to update the items in this TimedTerrain
	 * 
	 * @param mp
	 *            The Bolo map these items are on
	 * @param pbs
	 *            Pillboxes on this map
	 * @param bss
	 *            Bases on this map
	 */
	public void update(BoloMap mp, Pillboxes pbs, Bases bss) {
		ListIterator<ChangeableTerrainItem> li = items.listIterator();
		ChangeableTerrainItem cti;

		while (li.hasNext()) {
			cti = li.next();
			cti.increment();
			if (cti.getLength() >= length) {
				/* Remove it */
				checkSurroundings(mp, pbs, bss, cti.getX(), cti.getY());
				li.remove();
			}
		}

		/* Add any waiting */
		items.addAll(itemsWaiting);
		itemsWaiting.clear();
	}

	/**
	 * getConvertedTerrain converts squares containing bases and pillboxes to
	 * Terrain.ROAD as they dont effect timedterrain events
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
	 */
	private int getConvertedTerrain(BoloMap mp, Pillboxes pbs, Bases bss,
			int x, int y) {
		if (bss.existPos(x, y) == true || pbs.existPos(x, y) == true) {
			return Terrain.ROAD;
		}
		return mp.getPos(x, y);
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
	abstract void processCheckSurroundings(BoloMap mp, Pillboxes pbs,
			Bases bss, int x, int y, int above, int below, int left, int right);

	/**
	 * checkSurroundings checks and surrounding squares to see if they should be
	 * processed
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
	 */
	private void checkSurroundings(BoloMap mp, Pillboxes pbs, Bases bss, int x,
			int y) {
		int above;
		int below;
		int left;
		int right;

		above = getConvertedTerrain(mp, pbs, bss, x, y - 1);
		below = getConvertedTerrain(mp, pbs, bss, x, y + 1);
		left = getConvertedTerrain(mp, pbs, bss, x - 1, y);
		right = getConvertedTerrain(mp, pbs, bss, x + 1, y);

		processCheckSurroundings(mp, pbs, bss, x, y, above, below, left, right);
	}
}
