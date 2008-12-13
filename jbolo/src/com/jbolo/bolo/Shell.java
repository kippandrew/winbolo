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
 * Shell.java
 *
 * Created on October 9, 2003, 7:09 AM
 */

package com.jbolo.bolo;

/**
 * Shell represents a single shell in the game
 * 
 * @author John Morrison
 */
public class Shell extends GameWorldItem {

	/** The default life of a shell */
	public static final int LIFE = 8;

	/** What length a shell dies at */
	public static final int DEATH = 0;

	/** Speed a shell travels at */
	public static final int SPEED = 32;

	/** Angle the shell is travelling at */
	private float angle;

	/** How long a shell has been alive. Decrements to DEATH */
	private int length;

	/** Owner of the shell */
	private int owner;

	/** Was this shell fired from a boat */
	private boolean onBoat;

	/** Creator (ie player id) which created the shell */
	private int creator;

	/** Indicate if the shell is dead */
	private boolean shellDead;

	/** Has this shell been transmitted to all the players */
	private boolean packSent;

	/**
	 * Creates a new instance of Shell
	 * 
	 * @param x
	 *            X position of the shell
	 * @param y
	 *            Y position of the shell
	 * @param angle
	 *            Angle the shell is travelling at
	 * @param owner
	 *            Owner of the shell
	 * @param length
	 *            length of the shell
	 * @param onBoat
	 *            Was this shell fired from a boat
	 * @param creator
	 *            Player that created this shell
	 * @param shellDead
	 *            Is this shell dead
	 * @param packSent
	 *            Has this shell been sent to other players
	 */
	public Shell(int x, int y, float angle, int owner, int length,
			boolean onBoat, int creator, boolean shellDead, boolean packSent) {
		this.x = x;
		this.y = y;
		this.angle = angle;
		this.owner = owner;
		this.length = length;
		this.onBoat = onBoat;
		this.creator = creator;
		this.shellDead = shellDead;
		this.packSent = packSent;
	}

	/**
	 * getAngle returns the angle this shell is travelling at
	 * 
	 * @return angle
	 */
	public float getAngle() {
		return angle;
	}

	/**
	 * getOwner returns the owner of this shell
	 * 
	 * @return int
	 */
	public int getOwner() {
		return owner;
	}

	/**
	 * getOnBoat returns if the shell was fired from a boat
	 * 
	 * @return boolean
	 */
	public boolean getOnBoat() {
		return onBoat;
	}

	/**
	 * getCreator returns the creator of the shell
	 * 
	 * @return int creator
	 */
	public int getCreator() {
		return creator;
	}

	/**
	 * getShellDead returns if the shell is dead
	 * 
	 * @return boolean
	 */
	public boolean getShellDead() {
		return shellDead;
	}

	/**
	 * Sets if this shell is dead or not
	 * 
	 * @param dead
	 *            Is the shell dead
	 */
	public void setShellDead(boolean dead) {
		shellDead = dead;
	}

	/**
	 * getLength returns the length of this shell before it expires
	 * 
	 * @return int
	 */
	public int getLength() {
		return length;
	}

	/**
	 * decrementLength decrements the shells time before it expires. When the
	 * shells length == DEATH it is dead
	 */
	public void decrementLength() {
		length--;
	}

	/**
	 * getPackSent returns if this shell has been transmitted to other players
	 * 
	 * @return boolean
	 */
	public boolean getPackSent() {
		return packSent;
	}

	/**
	 * setPackSent sets whether this shell has been sent to other players
	 * 
	 * @param sent
	 *            Sets if this has been sent
	 */
	public void setPackSent(boolean sent) {
		this.packSent = sent;
	}

}
