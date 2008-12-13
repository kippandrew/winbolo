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
 * InfoDialog.java
 *
 * Created on January 2, 2004, 11:43 AM
 */

package com.jbolo.gui.dialog;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;

/** Info dialog is the base class for all information dialog boxes
 * @author John Morrison
 * @see GameInfoDialog
 * @see SystemInfoDialog
 * @see NetworkInfoDialog
 */
public abstract class InfoDialog extends Dialog implements Runnable {
    /** Number of milliseconds in a second */
    protected static double SECOND = 1000.0;
    
    /** Our thread for processing */
    protected Thread thread;

    /** Create a new instance of InfoDialog
     * @param owner The owner of this Dialog
     * @param name The dialog title
     */
    public InfoDialog(Frame owner, String name) {
	super(owner, name);
	setup();
    }
    
    /** Create a new instance of InfoDialog
     * @param owner The owner of this Dialog
     */
    public InfoDialog(Frame owner) {
	super(owner);
	setup();
    }
        
    /**
     * Run method of the thread. Calls update every second
     */
    public void run() {
	Thread currentThread = Thread.currentThread();
	while (currentThread == thread) {
	    update();
	    try {
		thread.sleep((int) SECOND);
	    } catch (Exception e) {
		// Ignore the wakeup exception
	    }
	}
    }
    
    /**
     * Overrides the setVisible method to make the window visible
     * Starts and stops the thread as part of it
     * @param visible Is the dialog visible or not
     */
    public void setVisible(boolean visible) {	
	if (visible == true) {
	    thread = new Thread(this);
	    thread.start();
	} else {
	    thread = null;
	}
	super.setVisible(visible);
    }
    
    /**
     * Sets up the dialog
     */
    private void setup() {
	setLayout(new GridLayout(0, 2, 5, 5));
	setResizable(false);
	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e) {e.getWindow().setVisible(false);}
	});
	
    }
    
    /**
     * Called once a second to update the dialog contents
     */
    abstract void update();
}
