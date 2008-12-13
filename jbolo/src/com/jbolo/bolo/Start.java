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

import com.jbolo.winbolonet.WBN;

/** Start is used to represent a specific game start
 * @author John Morrison
 */
public class Start extends GameItem {
    /** Start position 0 */
    private final static byte START0 = 0;
    /** Start position 1 */
    private final static byte START1 = 1;
    /** Start position 2 */
    private final static byte START2 = 2;
    /** Start position 3 */
    private final static byte START3 = 3;
    /** Start position 4 */
    private final static byte START4 = 4;
    /** Start position 5 */
    private final static byte START5 = 5;
    /** Start position 6 */
    private final static byte START6 = 6;
    /** Start position 7 */
    private final static byte START7 = 7;
    /** Start position 8 */
    private final static byte START8 = 8;
    /** Start position 9 */
    private final static byte START9 = 9;
    /** Start position 10 */
    private final static byte START10 = 10;
    /** Start position 11 */
    private final static byte START11 = 11;
    /** Start position 12 */
    private final static byte START12 = 12;
    /** Start position 13 */
    private final static byte START13 = 13;
    /** Start position 14 */
    private final static byte START14 = 14;
    /** Start position 15 */
    private final static byte START15 = 15;
    
    /** Amount to times the starts by  to convert to bradians */
    private final static float START_TIMES_16 = (float) 16.0;
    
    /** The game item name used in sending messages. Eg Pillbox */
    public static String NAME = "Start";
    
    /** The direction of this start */
    private byte dir;
    
    /** Constructor
     * @param itemNum The number of this item 0-15
     */
    public Start(byte itemNum) {
	super(itemNum);
    }
    
    /** Returns the direction of this start
     * @return direction of this start
     */
    public byte getDir() {
	return dir;
    }
    
    public float getStartDir() {
	return convertDir(getDir()) * START_TIMES_16;
    }
    
    /** Sets this start
     * @param x X position
     * @param y Y position
     * @param dir Start direction
     */
    public void set(int x, int y, byte dir) {
	this.x = x;
	this.y = y;
	this.dir = dir;
    }
    
    public int getArmour() {
	return 0;
    }
    
    /**
     * Gets the Log typeo for setting the owner for this GameItem type
     * @return byte containing the log owner type
     */
    public byte getLogOwnerType() {
	return 0x50; // Error
    }
    
    public void updateFrontEndStatus(GameWorld gw, int itemNum) {
	// Dont need to do anything for a start
    }
    
    /**
     * Converts a Bolo Map start (0 =East, 4 = North) to my starts 0 = North, 4 = East etc.
     * @return the start direction
     * @param dir Direction it is facing
     */
    private byte convertDir(byte dir) {
	byte returnValue;
	
	switch (dir) {
	    case START0:
		returnValue = START4;
		break;
	    case START1:
		returnValue = START3;
		break;
	    case START2:
		returnValue = START2;
		break;
	    case START3:
		returnValue = START1;
		break;
	    case START4:
		returnValue = START0;
		break;
	    case START5:
		returnValue = START15;
		break;
	    case START6:
		returnValue = START14;
		break;
	    case START7:
		returnValue = START13;
		break;
	    case START8:
		returnValue = START12;
		break;
	    case START9:
		returnValue = START11;
		break;
	    case START10:
		returnValue = START10;
		break;
	    case START11:
		returnValue = START9;
		break;
	    case START12:
		returnValue = START8;
		break;
	    case START13:
		returnValue = START7;
		break;
	    case START14:
		returnValue = START6;
		break;
	    default:
		/* START15 */
		returnValue = START5;
		break;
	}
	
	return returnValue;
    }
    
    /**
     * Gets the name of the object 
     * #return The object name
     */
    public String getName() {
	return Messages.EMPTY_STRING;
    }
    
    /**
     * Gets the WBN event type for stealing this item
     *
     * @return byte containing the WBN event type
     */
    public byte getWBNEventSteal() {
        return WBN.WINBOLO_NET_EVENT_NO_EVENT;
    }

    /**
     * Gets the WBN event type for capturing this item
     *
     * @return byte containing the WBN event type
     */
    public byte getWBNEventCapture() {
        return WBN.WINBOLO_NET_EVENT_NO_EVENT;
    }
    
}
