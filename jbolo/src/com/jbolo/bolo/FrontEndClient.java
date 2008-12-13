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
 * FrontEndMisc.java
 *
 * Created on January 4, 2004, 10:44 PM
 */

package com.jbolo.bolo;

/**
 * The other interface that must be implemented by a client
 * 
 * @author John Morrison
 */
public interface FrontEndClient {

	/**
	 * Clears a players entry from the players menu
	 * 
	 * @param playerNum
	 *            player to clear (0-15)
	 */
	void feClearPlayer(int playerNum);

	/**
	 * Sets a players menu entry in the players menu
	 * 
	 * @param playerNum
	 *            player to set (0-15)
	 * @param name
	 *            Name to set
	 */
	void feSetPlayer(int playerNum, String name);

	/**
	 * Sets the check state of a player
	 * 
	 * @param playerNum
	 *            Player to set state for (0-15)
	 * @param checkState
	 *            State to set
	 */
	public void feSetPlayerCheckState(int playerNum, boolean checkState);

	/**
	 * Sets whether the gunsight menu item is checked or not
	 * 
	 * @param isShown
	 *            is the gunsight shown
	 */
	public void feShowGunsight(boolean isShown);

	/**
	 * Enables/Disables the request alliance menu ite
	 * 
	 * @param enable
	 *            Whether to enable it or not
	 */
	public void feEnableRequestAllyMenu(boolean enable);

	/**
	 * Enables/Disables the leave alliance menu ite
	 * 
	 * @param enable
	 *            Whether to enable it or not
	 */
	public void feEnableLeaveAllyMenu(boolean enable);
}
