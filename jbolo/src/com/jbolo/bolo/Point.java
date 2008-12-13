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
 * Point.java
 *
 * Created on October 9, 2003, 7:20 AM
 */

package com.jbolo.bolo;

/** Point represents an X, Y position
 *
 * @author  John Morrison
 */
public class Point {
    /** The X position of the point */
    private int x;
    /** The Y position of the point */
    private int y;
    
    /** Creates a new instance of Point 
     * @param x X position of the point
     * @param y Y position of the point
     */
    public Point(int x, int y) {
	this.x = x;
	this.y = y;
    }
    
    /** getX returns the X position of the point
     * @return X position of the point
     */
    public int getX() {
	return x;
    }
    
    /** getY returns the Y position of the point
     * @return Y position of the point
     */
    public int getY() {
	return y;
    }
    
    /** setX sets the X position of the point
     * @param x X position to set
     */
    public void setX(int x) {
	this.x = x;
    }
    
    /** setY sets the Y position of the point
     * @param y Y position to set
     */    
    public void setY(int y) {
	this.y = y;
    }
    
    /** set sets the X and Y position of the point
     * @param x X position to set
     * @param y Y position to set
     */
    public void set(int x, int y) {
	this.x = x;
	this.y = y;
    }
    
    public String toString() {
	return "X: " + x + ", Y: " + y;
    }
}
