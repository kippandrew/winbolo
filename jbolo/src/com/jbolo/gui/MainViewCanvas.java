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
 * MainViewPanel.java
 *
 * Created on January 2, 2004, 8:29 PM
 */

package com.jbolo.gui;

import com.jbolo.bolo.GameClient;
import java.awt.Graphics2D;

/** Provides the main window. Required as only canvas awt objects intercept mouse
 * movement events
 * @author John Morrison
 */
public class MainViewCanvas extends java.awt.Canvas {
    /** The GameClient used in drawing */
    private GameClient gc;
    /** The drawing class */
    private Draw draw;
    /** The bolo cursor used to get the current selects */
    private BoloCursor bc;
    
    /** Creates a new instance of MainViewPanel
     * @param gc GameClient to call
     * @param draw Drawing class to use to draw
     * @param bc Bolo Cursor used to get the current build select
     */
    public MainViewCanvas(GameClient gc, Draw draw, BoloCursor bc) {
	super();
	this.gc = gc;
	this.draw = draw;
	this.bc = bc;
    }
    
    /** Overridden paint method to draw our window
     * @param graphics Graphics Object to draw with
     */    
    public void paint(java.awt.Graphics graphics) {
	draw.redrawAll((Graphics2D) graphics, gc);
	draw.drawSelectIndentsOn(bc.getCurrentSelect());
    }
    
}
