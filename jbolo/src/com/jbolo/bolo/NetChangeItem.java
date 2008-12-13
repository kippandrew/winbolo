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
 * NetItem.java
 *
 * Created on November 29, 2003, 10:05 PM
 */

package com.jbolo.bolo;

/** A NetItem is transmitted between servers and clients about changes
 * that occur in the GameWorld
 * @author John Morrison
 */
public class NetChangeItem {
    
    /** The item to change */
    private int item;
    
    /** Owner of the change */
    private int owner;
    
    /** Id of the item */
    private int id;
    
    /** X position of the item */
    private int x;
    
    /** Y position of the item */
    private int y;
    
    /** Creates a new instance of NetItem
     * @param x X position of the item
     * @param y Y position of the item
     * @param item item
     * @param id Id of the item
     * @param owner owner of the item
     */
    public NetChangeItem(int x, int y, int item, int id, int owner) {
	this.x = x;
	this.y = y;
	this.item = item;
	this.id = id;
	this.owner = owner;
    }
    
    /** Gets the X position of the item
     * @return X position of the item
     */    
    public int getX() {
	return x;
    }
    
    /** Gets the Y position of this item
     * @return Y position of the item
     */    
    public int getY() {
	return y;
    }
    
    /** Gets this item
     * @return the item
     */    
    public int getItem() {
	return item;
    }
    
    /** Gets the owner of this NetItem
     * @return owner of this NetItem
     */    
    public int getOwner() {
	return owner;
    }
    
    /** Gets the item of this NetItem
     * @return id of the item
     */    
    public int getId() {
	return id;
    }
    
}
