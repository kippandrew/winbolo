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
 * MessageBox.java
 *
 * Created on January 4, 2004, 7:57 PM
 */

package com.jbolo.gui.dialog;

import java.awt.*;
import java.awt.event.*;

/**
 * Provides a message box
 * @author  John Morrison
 */
public class MessageBox extends CentredDialog implements ActionListener {
    
    /** Default Message box title string */
    public static String TITLE = "JBolo";
    
    /** Creates a new instance of MessageBox 
     * @param message The message to display in the message box
     * @param title The title of the message box     
     */
    public MessageBox(String message, String title) {
	super(new Frame(), title, true);
	setup(message);
    }

    /** Creates a new instance of MessageBox using the default title
     * @param message The message to display in the message box
     */
    public MessageBox(String message) {
	super(new Frame(), TITLE, true);
	setup(message);
    }
    
    
    /** Creates a new instance of MessageBox
     * @param owner The owner frame of this message box
     * @param message The message to display in the message box
     * @param title The title of the message box     
     */
    public MessageBox(Frame owner, String message, String title) {
	super(owner, title, true);
	setup(message);
    }
    
    /** Creates a new instance of MessageBox using the default title
     * @param owner The owner frame of this message box
     * @param message The message to display in the message box
     */
    public MessageBox(Frame owner, String message) {
	super(owner, TITLE, true);
	setup(message);
    }

    /** Creates a new instance of MessageBox
     * @param owner The owner dialolg of this message box
     * @param message The message to display in the message box
     * @param title The title of the message box     
     */
    public MessageBox(Dialog owner, String message, String title) {
	super(owner, title, true);
	setup(message);
    }
    
    /** Creates a new instance of MessageBox using the default title
     * @param owner The owner dialolg of this message box
     * @param message The message to display in the message box
     */
    public MessageBox(Dialog owner, String message) {
	super(owner, TITLE, true);
	setup(message);
    }

    /**
     * Sets up this message box for display and displays it
     * @param message The message to display in the message box
     */
    private void setup(String message) {
	Button b;
	Label l = new Label(message);
	
	l.setAlignment(Label.CENTER);
	setLayout(new BorderLayout());
	add("Center", l);
	b = new Button("OK");
	add("South", b);
	b.addActionListener(this);
	addWindowListener(new WindowAdapter() { public void windowClosing(WindowEvent e) {System.exit(0);}});
	setResizable(true);
	pack();
	centre();
	setVisible(true);
    }    
    
    /**
     * Called when the OK button is pressed
     * @param actionEvent The event that occurs
     */
    public void actionPerformed(java.awt.event.ActionEvent actionEvent) {
	setVisible(false);
	dispose();
    }
    
}
