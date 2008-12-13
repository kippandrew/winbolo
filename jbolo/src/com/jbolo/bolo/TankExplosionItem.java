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
 * TankExplosionItem.java
 *
 * Created on December 29, 2003, 9:08 PM
 */

package com.jbolo.bolo;

/** 
 * A tank explosion
 * @author John Morrison
 */
public class TankExplosionItem extends GameWorldItem {
    /** The angle this explosion is travelling at */
    private float angle;
    /** Length of the tank explosion */
    private int length;
    /** The type of the explosion type */
    private byte explodeType;
    /** Has this tkexplosion been included in a network packet */
    private boolean packSent;
    /** Did we create this explosions or the network? */
    private boolean own;
    /** Creators player number */
    private int creator;
    
    /** Creates a new instance of TankExplosionItem
     * @param length Length of time this tank explosion will move for
     * @param packSent Has this tank explosion been sent across the network
     * @param x X world co-ordinate of the tank explosion
     * @param y Y world co-ordinate of the tank explosion
     * @param angle Angle the explosion is travelling at
     * @param explodeType The type of explosion is
     * @param own Did we create this explosion or the network
     * @param creator The creator player number of the tank explosion
     */
    public TankExplosionItem(int x, int y, float angle, int length, byte explodeType, boolean own, boolean packSent, int creator) {
	this.x = x;
	this.y = y;
	this.angle = angle;
	this.length = length;
	this.explodeType = explodeType;
	this.own = own;
	this.packSent = packSent;
	this.creator = creator;
    }
    
    /**
     * Gets the angle this tank explosion is travelling at
     * @return the explosion travelling angle
     */
    public float getAngle() {
	return angle;
    }
    
    /**
     * Removes one from this tank explosions length remaining
     */
    public void decrementLength() {
	if (length > 0) {
	    length--;
	}
    }
    
    /**
     * Gets the length of this explosion
     * @return the explosion length
     */
    public int getLength() {
	return length;
    }
    
    /**
     * Gets the type of explosion big or small
     * @return the explosion type
     */
    public byte getExplodeType() {
	return explodeType;
    }
    
    /**
     * Get if this player created the tank explosion
     * @return if this player created the tank explosion
     */
    public boolean getOwn() {
	return own;
    }
    
    /**
     * Gets if this tank explosion has been sent to the network
     * @return If the tank explosion has been sent in a network packet
     */
    public boolean getPacketSent() {
	return packSent;
    }
    
    /**
     * Returns the creator of the tank explosion
     * @return player number that created the tank explosion
     *
     */
     public int getCreator() {
	return creator;
    }
     
     /**
      * Sets whether this item has been sent over the network
      *
      * @param value Value to set
      */
    public void setPackSent(boolean value) {
        packSent = value;
    }
}
