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
 * SendMessageDialog.java
 *
 * Created on January 5, 2004, 12:10 AM
 */

package com.jbolo.gui.dialog;

import com.jbolo.bolo.GameClient;
import java.awt.*;
import java.awt.event.*;

/**
 * Provides the send Message window
 * @author John Morrison
 */
public class SendMessageDialog extends CentredDialog implements ItemListener, ActionListener, KeyListener, Runnable {
    /** New line letter to react to for pressing enter */
    private static char NEW_LINE = '\n';
    
    /** Number of seconds to sleep for in milliseconds before allowing another message to be sent */
    private static int MESSAGE_DELAY = 2000;
    
    /** Game client to use */
    private GameClient gc;
    
    /** Label for number of players */
    private Label numPlayersToSend;
    
    /** The button to send messages with */
    private Button send;
    /** All players choice */
    private Checkbox cbAllPlayers;
    /** All allies choice */
    private Checkbox cbAllAllies;
    /** All nearby tanks */
    private Checkbox cbNearbyTanks;
    /** Selections on the players menu */
    private Checkbox cbPlayersMenu; 
    /** Text box to send */
    private TextField text;
    /** Thread for stopping people sending messages */
    private Thread waitMessageSend;
    
    /** Creates a new instance of SendMessageDialog */
    public SendMessageDialog(Frame owner, GameClient gc) {
	super(owner, "Compose Message", false);
	this.gc = gc;
	
	setLayout(new GridLayout(0, 2, 5, 5));
	
	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e) {e.getWindow().setVisible(false);}
	});
	
	CheckboxGroup cbg = new CheckboxGroup();
	send = new Button("Send");
	send.addActionListener(this);
	
	cbAllPlayers = new Checkbox("All players", cbg, true);
	cbAllPlayers.addItemListener(this);
	cbAllAllies = new Checkbox("All allies", cbg, false);
	cbAllAllies.addItemListener(this);
	cbNearbyTanks = new Checkbox("All nearby tanks", cbg, false);
	cbNearbyTanks.addItemListener(this);
	cbPlayersMenu = new Checkbox("Selection on the Players menu", cbg, false);
	cbPlayersMenu.addItemListener(this);
	numPlayersToSend = new Label(getNumPlayersToSendText());
	text = new TextField();
	text.addKeyListener(this);
	
	add(cbAllPlayers);
	add(new Label(""));
	add(cbAllAllies);
	add(new Label(""));
	add(cbNearbyTanks);
	add(new Label(""));
	add(cbPlayersMenu);
	add(new Label(""));
	add(text);
	add(send);
	add(numPlayersToSend);
	setResizable(false);
	pack();
	centre();
	text.requestFocus();
    }

    /**
     * Gets the send message type we are bby looking at which check box is checked
     * @return the message type
     */
    private int getCheckedItemMessageType() {
	int returnValue; // Value to return
	if (cbAllPlayers.getState() == true)  {
	    returnValue = GameClient.MESSAGE_ALL_PLAYERS;
	} else if (cbAllAllies.getState() == true) {
	    returnValue = GameClient.MESSAGE_ALL_ALLIES;
	} else if (cbNearbyTanks.getState() == true) {
	    returnValue = GameClient.MESSAGE_ALL_NEARBY;
	} else {
	    // cbPlayersMenu must be true
	    returnValue = GameClient.MESSAGE_SELECTIONS_PLAYERS_MENU;
	}
	return returnValue;
    }

    /**
     * Gets the number of players to send text
     * @return The string saying "Sending message to X players
     */
    private String getNumPlayersToSendText() {
	String returnValue;
	int numPlayers = gc.getNumberCheckedPlayers(getCheckedItemMessageType());
	
	returnValue =  "Sending message to " + Integer.toString(numPlayers) + " player";
	if (numPlayers != 1) {
	    returnValue += "s";
	}
	return returnValue;
    }
    
    private void sendMessage() {
	String textSend = text.getText();
	int len = textSend.length();
	if (len > 240) {
	    len = 240;
	}
	
	textSend = textSend.substring(0, len);
	
	gc.sendMessage(getCheckedItemMessageType(), textSend);
	text.selectAll();
	text.requestFocus();
	send.setEnabled(false);
	waitMessageSend = new Thread(this);
	waitMessageSend.start();
    }
    
    /**
     * Called when one of the checkbox item states changes to update the message label
     * @param itemEvent the event that changed
     */
    public void itemStateChanged(java.awt.event.ItemEvent itemEvent) {
	numPlayersToSend.setText(getNumPlayersToSendText());
	text.requestFocus();
    }

    public void actionPerformed(java.awt.event.ActionEvent actionEvent) {
	sendMessage();
    }
    
    public void keyPressed(java.awt.event.KeyEvent keyEvent) {
	if (keyEvent.getKeyChar() == '\n' && send.isEnabled() == true) {
	    sendMessage();
	}
    }
    
    public void keyReleased(java.awt.event.KeyEvent keyEvent) {
    }
    
    public void keyTyped(java.awt.event.KeyEvent keyEvent) {
    }
    
    public void run() {
	Thread thread = Thread.currentThread();
	try {
	    thread.sleep(MESSAGE_DELAY);
	} catch (Exception e) {
	    
	}
	send.setEnabled(true);
	waitMessageSend = null;
    }
    
    
}
