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
 * SetPlayerNameDialog.java
 *
 * Created on January 4, 2004, 4:04 PM
 */

package com.jbolo.gui.dialog;
import java.awt.*;
import java.awt.event.*;
import com.jbolo.bolo.GameClient;

/**
 * Provides the change player name dialog
 * @author John Morrison
 */
public class SetPlayerNameDialog extends CentredDialog implements ActionListener, KeyListener {
    /** Are we in the game or in the pregame dialog */
    private boolean inGame;
    /** The text field the new name is typed in */
    private TextField field;
    /** Our game client for setting the player name */
    private GameClient gc;
    /** The current player name we are going to change */
    private String currentName;
    
    /** Has the name been changed */
    private boolean changed;
    
    /** Creates a new instance of SetPlayerNameDialog 
     * @param owner Owner of the dialog
     * @param inGame are we in game or the pregame dialog
     * @param Name The current player name
     */
    public SetPlayerNameDialog(Frame owner, boolean inGame, GameClient client, String name) {
	super(owner, "Enter Player Name", true);
        setup(inGame, client, name);
    }
    

    public SetPlayerNameDialog(Dialog owner, boolean inGame, GameClient client, String name) {
        super(owner, "Enter Player Name", true);
        setup(inGame, client, name);
    }

    private void setup(boolean inGame, GameClient client, String name) {
	this.inGame = inGame;
	this.gc = client;
	currentName = name;
        changed = false;

        // Setup the dialog
	setLayout(new BorderLayout(15, 15));
	add("North", new Label("Enter the new player name for your tank"));
	field = new TextField(name, GameClient.MAX_NAME_LENGTH);
	add("Center", field);
	Button b = new Button("OK");
	b.addActionListener(this);
	add("South", b);
	setResizable(false);
	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e) {e.getWindow().dispose();}
	});
	
	field.addKeyListener(this);

	pack();
	b.setSize(25, b.getHeight());
	centre();
	setVisible(true);
	field.selectAll();
	field.requestFocus();
    }

    private void changeName() {
	String newName;
	boolean changeOK = true;
	boolean closeDialog = false;
	
	newName = field.getText();
	
        if (newName.length() > 0) {
            if (inGame == false) {
                //FIXME: Need to implement what to do here
                currentName = field.getText();
                closeDialog = true;
                changed = true;
            } else {
                if (currentName.equals(newName) == true) {
                    // Names are the same
                    closeDialog = true;
                } else {
                    if (gc.setPlayerName(newName) == false) {
                        // Someone must already have our name
                        MessageBox mb = new MessageBox(this, "This player name is in use, please try another");
                    } else {
                        closeDialog = true;
                        changed = true;
                        currentName = field.getText();

                    }
                }
            }
        } else {
            MessageBox mb = new MessageBox(this, "You must enter in a name");
        }
	if (closeDialog == true) {
	    hide();
	    dispose();
	}
	
    }
    
    /**
     * The OK button has been pressed. Process the name change. Attempt to change the name
     * and close the dialog if ok.
     * @param actionEvent the java.awt.ActionEvent performed
     */
    public void actionPerformed(java.awt.event.ActionEvent actionEvent) {
	changeName();
    }
    
    public void keyPressed(java.awt.event.KeyEvent keyEvent) {
	/// Do nothing
    }
    
    public void keyReleased(java.awt.event.KeyEvent keyEvent) {
	// Do nothing
    }
    
    public void keyTyped(java.awt.event.KeyEvent keyEvent) {
	if (keyEvent.getKeyChar() == '\n') {
	    changeName();
	}
    }
    
    public boolean isNameChanged() {
        return changed;
    }
    
    public String getName() {
        return currentName;
    }
    
}
