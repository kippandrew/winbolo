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
 * TankButtons.java
 *
 * Created on October 11, 2003, 9:21 PM
 */

package com.jbolo.bolo;

/**
 * TankButtons contains the different buttons that are pressed
 * 
 * @author John Morrison
 */
public class TankButtons {

	/** No Buttons being pressed */
	public static final int TNONE = 0;
	/** Left button is being pressed */
	public static final int TLEFT = 1;
	/** Right button is being pressed */
	public static final int TRIGHT = 2;
	/** Accelerate Button */
	public static final int TACCEL = 3;
	/** Decelerate Button is being pressed */
	public static final int TDECEL = 4;
	/** Left + Accelerate */
	public static final int TLEFTACCEL = 5;
	/** Right + Accelerate */
	public static final int TRIGHTACCEL = 6;
	/** Left + Decelerate */
	public static final int TLEFTDECEL = 7;
	/** Right + Decelerate */
	public static final int TRIGHTDECEL = 8;

	private int heldButtons;

	boolean kiForward; /* Tank accelerate */
	boolean kiBackward; /* Tank decelerate */
	boolean kiLeft; /* Tank left */
	boolean kiRight; /* Tank right */
	boolean kiShoot; /* Tank shooting */
	boolean kiLayMine; /* Tank lay mine */
	/* The rest are all WM_KEYDOWN (virtual-key codes) */
	boolean kiGunIncrease; /* Increase gun sight length */
	boolean kiGunDecrease; /* Decrease gun sight length */
	boolean kiTankView; /* Centre on tank */
	boolean kiPillView; /* Pill view */
	boolean kiScrollUp; /* Scroll up */
	boolean kiScrollDown; /* Scroll down */
	boolean kiScrollLeft; /* Scroll left */
	boolean kiScrollRight; /* Scroll right */

	/** Creates a new instance of TankButtons */
	public TankButtons() {
		heldButtons = TNONE;
		kiForward = false;
		kiBackward = false;
		kiLeft = false;
		kiRight = false;
		kiShoot = false;
		kiLayMine = false;
		kiGunIncrease = false;
		kiGunDecrease = false;
		kiTankView = false;
		kiPillView = false;
		kiScrollUp = false;
		kiScrollDown = false;
		kiScrollLeft = false;
		kiScrollRight = false;
		kiShoot = false;
	}

	public int getHeldButtonsuttons() {
		return heldButtons;
	}

	public void keyDown(int keyCode) {
		updateHeldKeys(keyCode, true);

	}

	public void keyUp(int keyCode) {
		updateHeldKeys(keyCode, false);
	}

	public boolean getShooting() {
		return kiShoot;
	}

	private void updateHeldKeys(int keyCode, boolean down) {
		// Q = 81
		if (keyCode == 81) {
			kiForward = down;
		}
		// A = 65
		if (keyCode == 65) {
			kiBackward = down;
		}
		// O = 79
		if (keyCode == 79) {
			kiLeft = down;
		}
		// P = 80
		if (keyCode == 80) {
			kiRight = down;
		}
		// space =
		if (keyCode == 32) {
			kiShoot = down;
		}

		heldButtons = TNONE;
		if (kiForward == true && kiRight == true) {
			heldButtons = TRIGHTACCEL;
		} else if (kiForward == true && kiLeft == true) {
			heldButtons = TLEFTACCEL;
		} else if (kiBackward == true && kiLeft == true) {
			heldButtons = TLEFTDECEL;
		} else if (kiBackward == true && kiRight == true) {
			heldButtons = TRIGHTDECEL;
		} else if (kiForward == true) {
			heldButtons = TACCEL;
		} else if (kiBackward == true) {
			heldButtons = TDECEL;
		} else if (kiLeft == true) {
			heldButtons = TLEFT;
		} else if (kiRight == true) {
			heldButtons = TRIGHT;
		} else {
			heldButtons = TNONE;
		}
	}
}
