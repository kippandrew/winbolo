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


package com.jbolo.bolo.map;

/*
 * GameItemRect.java
 *
 * Created on October 5, 2003, 12:25 AM
 */

/**
 *
 * @author  John Morrison
 */
public class GameItemRect {
    
    /** Top position of this rect */    
    private int top;
    /** Bottom position of this rect */    
    private int bottom;
    /** Left position of this rect */    
    private int left;
    /** Right position of this rect */    
    private int right;
    
    
    /** Creates a new instance of GameItemRect
     * @param top Top position to set
     * @param bottom Bottom position to set
     * @param left Left position to set
     * @param right Right position to set
     */
    public GameItemRect(int top, int bottom, int left, int right) {
	this.top = top;
	this.bottom = bottom;
	this.left = left;
	this.right = right;
    }
    
    /** Gets the top position
     * @return top position
     */    
    public int getTop() {
	return top;
    }
    
    /** Gets the bottom position
     * @return bottom position
     */        
    public int getBottom() {
	return bottom;
    }

    /** Gets the left position
     * @return left position
     */    
    public int getLeft() {
	return left;
    }
    
    /** Gets the right position
     * @return right position
     */    
    public int getRight() {
	return right;
    }
    
    /** Compares this GameItemRect with test and sets all the extends to the
     * larger region.
     * @param test The GameItemRect to test
     */    
    public void setLargerExtends(GameItemRect test) {
	if (test.getTop() > top) {
	    top = test.getTop();
	}
	if (test.getBottom() < bottom) {
	    bottom = test.getBottom();
	}
	
	if (test.getLeft() < left) {
	    left = test.getLeft();
	}
	
	if (test.getRight() > right) {
	    right = test.getRight();
	}
    }
}
