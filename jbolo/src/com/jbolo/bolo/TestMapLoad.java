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
 * TestMapLoad.java
 *
 * Created on January 11, 2004, 10:53 PM
 */

package com.jbolo.bolo;

import com.jbolo.bolo.map.BoloMap;

/**
 * Test class to attempt loading a map
 * 
 * @author John Morrison
 */
public class TestMapLoad {

	private Pillboxes pbs;

	private Bases bss;

	private BoloMap map;

	private Starts sss;

	private String mapName;

	/** Creates a new instance of TestMapLoad */
	public TestMapLoad() {
		mapName = "";
	}

	/**
	 * Attempts to read in a bolo map from fileName
	 * 
	 * @return if the map can be successfully read
	 * @param fileName
	 *            the filename and path of the map to load
	 */
	public boolean loadMap(String fileName) {
		boolean returnValue; // Value to return
		GameWorld gw = new GameWorld(0, false, null, null, null);
		pbs = new Pillboxes(gw);
		bss = new Bases(gw);
		sss = new Starts(gw);
		map = new BoloMap(gw);

		returnValue = map.read(fileName, sss, bss, pbs);
		if (returnValue == true) {
			mapName = Utils.GetMapNameFromFileName(fileName);
		}

		return returnValue;
	}

	/**
	 * Gets the map name of the loaded map
	 * 
	 * @return String containing the map name without extension
	 */
	public String getMapName() {
		return mapName;
	}

	public static void main(String args[]) {
		TestMapLoad load = new TestMapLoad();

		System.err.println("Loading... " + load.loadMap("c:/jbolo/map.map"));
	}
}
