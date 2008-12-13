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
 * ScreenItem.java
 *
 * Created on October 5, 2003, 6:07 PM
 */

/** ScreenItem is used to show items on the screen
 * @author John Morrison
 */
public class ScreenItem {
    
    /** The objects x map position */    
    private int mx;
    
    /** The objects y map position */    
    private int my;
    
    /** The objects x pixel position */    
    private int px;
    
    /** The objects y pixel position */    
    private int py;
    
    /** The objects frame */    
    private int frame;
    
    /** The objects item number. */    
    private int itemNum;
    
    /** Creates a new instance of ScreenItem
     * @param mx The objects map X position
     * @param my The objects map Y position
     * @param px The objects pixel X position
     * @param py The objects pixel Y position
     * @param frame The objects frame
     * @param itemNum The objects itemNum
     */
    public ScreenItem(int mx, int my, int px, int py, int frame, int itemNum) {
	this.mx = mx;
	this.my = my;
	this.px = px;
	this.py = py;
	this.frame = frame;
	this.itemNum = itemNum;
    }
    
    /** Creates a new instance of ScreenItem
     * @param mx The objects map X position
     * @param my The objects map Y position
     * @param px The objects pixel X position
     * @param py The objects pixel Y position
     * @param frame The objects frame
     */
    public ScreenItem(int mx, int my, int px, int py, int frame) {
	this.mx = mx;
	this.my = my;
	this.px = px;
	this.py = py;
	this.frame = frame;
	this.itemNum = 0;
    }

    
    /** Returns the map x position
     * @return int
     */    
    public int getMX() {
	return mx;
    }
    
    /** Returns the map y position
     * @return int
     */    
    public int getMY() {
	return my;
    }
    
    /** Returns the pixel x position
     * @return int
     */    
    public int getPX() {
	return px;
    }
    
    /** Returns the pixel y position
     * @return int
     */    
    public int getPY() {
	return py;
    }
    
    /** Returns the objects drawing frame
     * @return int
     */    
    public int getFrame() {
	return frame;
    }
    
    /** Returns the objects item number
     * @return int
     */    
    public int getItemNum() {
	return itemNum;
    }
}
