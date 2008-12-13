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
 * ScreenTankItem.java
 *
 * Created on October 5, 2003, 6:38 PM
 */

/** ScreenTankItems is used to represent a tank to draw onto the screen.
 * @author John Morrison
 */
public class ScreenTankItem extends ScreenItem {
    
    /** The player name */    
    private String name;
    
    /** Creates a new instance of ScreenTankItem
     * @param mx The objects map X positoin
     * @param my The objects map Y positoin
     * @param px The objects pixel X positoin
     * @param py The objects pixel Y positoin
     * @param frame The objects frame
     * @param itemNum The objects itemNum
     * @param name The tanks player name
     */
    public ScreenTankItem(int mx, int my, int px, int py, int frame, int itemNum, String name) {
	super(mx,  my,  px,  py,  frame, itemNum);
	this.name = name;
    }
    
    /** Returns the player name of this ScreenTankItem
     * @return String
     */    
    public String getName() {
	return name;
    }
}
