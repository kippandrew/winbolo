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
 * KeySetupDialog.java
 *
 * Created on January 10, 2004, 8:27 PM
 */

package com.jbolo.gui.dialog;

import com.jbolo.bolo.GameClient;
import java.awt.*;
import java.awt.event.*;

/**
 * Provides the window to setup keys
 * @author John Morrison
 */
public class KeySetupDialog extends CentredDialog implements ActionListener {
    /** Empty string */
    private static String EMPTY_STRING = "";
    
    /** Our Game Client */
    private GameClient gc;
    
    private TextField txtAccel;
    private TextField txtDecel;
    private TextField txtAnticlockwise;
    private TextField txtClockwise;
    private TextField txtIncrease;
    private TextField txtDecrease;
    private TextField txtShoot;
    private TextField txtLayMine;
    private TextField txtTankView;
    private TextField txtPillView;
    private TextField txtScrollUp;
    private TextField txtScrollDown;
    private TextField txtScrollLeft;
    private TextField txtScrollRight;

    
    
    
    private Checkbox autoSlowdown;
    private Checkbox autoHide;
    /** Creates a new instance of KeySetupDialog */
    public KeySetupDialog(Frame f, GameClient gclient) {
	super(f, "Choose Key Settings", true);
	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e) {dispose() ;}
	});
	
	
	
	this.gc = gclient;
	setLayout(new GridLayout(0, 4, 5, 5));
	add(new Label("Drive Tank:"));
	add(new Label("Accelerate"));
	this.txtAccel = new TextField(3);
	add(txtAccel);
	
	Label l = new Label("Click on the option you wish to change then next key pressed will be assigned it");
	autoSlowdown = new Checkbox("Auto slowdown", gc.getAutoslowState());
	add(autoSlowdown);
	add(new Label(EMPTY_STRING));
	add(new Label("Decelerate"));
	txtDecel = new TextField(3);
	add(txtDecel);
	add(new Label(EMPTY_STRING));
	add(new Label("Rotate Tank"));
	add(new Label("Anti-clockwise"));
	txtClockwise = new TextField(3);
	add(txtClockwise);
	add(new Label("(left)"));
	add(new Label(EMPTY_STRING));
	add(new Label("Clockwise"));
	txtAnticlockwise = new TextField(3);
	add(txtAnticlockwise);
	add(new Label("(right)"));
	add(new Label("Gun range"));
	add(new Label("Increase"));
	txtIncrease = new TextField(3);
	add(txtIncrease);
	autoHide = new Checkbox("autohide gunsight", gc.getAutoShowHideGunsight()); // Enable Automatic\nshow and hide of\ngunsight
	add(autoHide);
	add(new Label(EMPTY_STRING));
	add(new Label("Decrease"));
	txtDecrease = new TextField(3);
	add(txtDecrease);
	add(new Label(EMPTY_STRING));
	add(new Label("Weapons"));
	add(new Label("Shoot"));
	txtShoot = new TextField(3);
	add(txtShoot);
	add(new Label(EMPTY_STRING));
	add(new Label(EMPTY_STRING));
	add(new Label("Lay mine"));
	txtLayMine = new TextField(3);
	add(txtLayMine);
	add(new Label(EMPTY_STRING));
	add(new Label("Change Views"));
	add(new Label("Tank"));
	txtTankView =new TextField(3);
	add(txtTankView);
	add(new Label(EMPTY_STRING));
	add(new Label(EMPTY_STRING));
	add(new Label("Pillbox"));
	txtPillView = new TextField(3);
	add(txtPillView);
	add(new Label(EMPTY_STRING));
	add(new Label("Scroll:"));
	add(new Label("Up"));
	txtScrollUp = new TextField(3);
	add(txtScrollUp);
	add(new Label(EMPTY_STRING));
	add(new Label(EMPTY_STRING));
	add(new Label("Down"));
	txtScrollDown = new TextField(3);
	add(txtScrollDown);
	add(new Label(EMPTY_STRING));
	add(new Label(EMPTY_STRING));
	add(new Label("Left"));
	txtScrollLeft = new TextField(3);
	add(txtScrollLeft);
	Button b = new Button("OK");
	b.addActionListener(this);
	add(b);
	
	add(new Label(EMPTY_STRING));
	add(new Label("Right"));
	txtScrollRight = new TextField(3);
	add(txtScrollRight);
	b = new Button("Cancel");
	b.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { dispose();}});
	add(b);
	setResizable(false);
	pack();
	centre();
	setVisible(true);
    }
    
    /**
     * The OK button has been pressed. Assign the keys
     * @param actionEvent The action event that is prefered. Ignored
     */
    public void actionPerformed(java.awt.event.ActionEvent actionEvent) {
	// Sets the checkbox
	gc.setAutoShowHideGunsight(autoHide.getState());
	gc.setAutoslowState(autoSlowdown.getState());
	
	// Sets the keys
	
	
	
	// Finally dispose the window
	dispose();
    }
    
    
}
