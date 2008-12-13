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

/*
 * ChangeableTerrainItem.java
 *
 * Created on October 5, 2003, 10:00 AM
 */

/** ChangeableTerrainItem defines an item in queue to be processed by a changeable event
 *
 * @provides bolo/buildings.c bolo/swamp.c bolo/grass.c
 * @see ChangeableTerrain.java
 * @author  John Morrison
 */
public class ChangeableTerrainItem {
    
    /** X position of the event */
    private int x;
    /** Y position of the event */
    private int y;
    /** Length of time this event has been active */
    private int length;
    
    /** Creates a new instance of ChangeableTerrainItem 
     *
     * @param x X position of the event
     * @param y Y position of the event
     */
    public ChangeableTerrainItem(int x, int y) {
	this.x = x;
	this.y = y;
	length = 1; /* When we are created we must of been hit once */
    }
    
    /** exists checks if this event is a position x, y
     *
     * @param x X position of the event
     * @param y Y position of the event
     * @return true if this event is at position x,y
     */    
    public boolean exists(int x, int y) {
	if (this.x == x && this.y == y) {
	    return true;
	}
	return false;
    }

    /** getX returns the X position of the item
     *
     * @return the X position of the item
     */        
    public int getX() {
	return x;
    }
    
    /** getY returns the Y position of the item
     *
     * @return the Y position of the item
     */        
    public int getY() {
	return y;
    }

    /** getLength returns the length of the item
     *
     * @return the item length
     */            
    public int getLength() {
	return length;
    }
    
    /** increment adds one to the length of this item
     */
    public void increment() {
	length++;
    }
}
