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
 * GameInfoDialog.java
 *
 * Created on January 2, 2004, 5:56 PM
 */

package com.jbolo.gui.dialog;

import java.awt.*;
import com.jbolo.bolo.GameClient;
import com.jbolo.bolo.GameType;

/**
 * Provides the Game information dialog window
 * @author  John Morrison
 */
public class GameInfoDialog extends InfoDialog {
    /** Time limit label */
    private Label timeLimit;
    /** Number of players in this game */
    private Label numPlayers;
    /** The game client */
    private GameClient gc;
    /** Does the time limit need to be updated */
    private boolean needTimeUpdate;
    /** Last number of Player used to tell if we need to update the window */
    private int lastNumberPlayers;
    
    /** Creates a new instance of GameInfoDialog
     * @param owner The owner of this dialog
     * @param client The game client interface
     */
    public GameInfoDialog(Frame owner , GameClient client) {
	super(owner, "JBolo Game Info");
	gc = client;
	timeLimit = new Label();
	
	needTimeUpdate = true;
	add(new Label("Map Name:"));
	add(new Label(gc.getMapName()));
	add(new Label("Number of Players:"));
	lastNumberPlayers = gc.getNumberPlayers();
	numPlayers = new Label(Integer.toString(lastNumberPlayers));
	add(numPlayers);
	add(new Label("Game Type:"));
	add(new Label(convertGameType(gc.getGameType())));
	add(new Label("Hidden Mines:"));
	add(new Label(convertHiddenMines(gc.isHiddenMinesAllowed())));
	add(new Label("Computer Tanks Allowed:"));
	add(new Label(convertComputerTanks(gc.getComputerTanksAllowed())));
	add(new Label("Game Time Limit:"));	
	
	timeLimit = new Label(getTimeLeft(gc.getGameTimeLeft()));
	add(timeLimit);
	pack();
    }
    
    /**
     * Called once a second to update the game window
     */
    void update() {
	if (lastNumberPlayers != gc.getNumberPlayers()) {
	    lastNumberPlayers = gc.getNumberPlayers();
	    numPlayers.setText(Integer.toString(lastNumberPlayers));
	}
	if (needTimeUpdate == true) {
	    timeLimit.setText(getTimeLeft(gc.getGameTimeLeft()));
	}
    }
    
    private String getTimeLeft(long time) {
	if (time == -1) {
	    needTimeUpdate = false;
	    return "Unlimited";
	}
	time /= 60;
	time /= 5;
	return "About " + Long.toString(time) + " minute(s) left";
    }
	
    
    /**
     * Converts the compTanka variable to a string
     * @param compTanks Value to convert
     * @return A string containing if computers tanks are allowed
     */
    private String convertComputerTanks(int compTanks) {
	// FIXME
	return "No";
    }
    
    /**
     * Converts the boolean hiddent mines value into a string
     * @param hidden Are hidden mines allowed
     * @return if hidden mines are allowed
     */
    private String convertHiddenMines(boolean hidden) {
	if (hidden == true) {
	    return "Yes";
	}
	return "No";
    }
    
    /**
     * Converts the game type to the string representing it
     * @param gameType Type of this game
     * @return String containing the game type
     * @see com.jbolo.bolo.GameType
     */
    private String convertGameType(int gameType) {
	switch (gameType) {
	    case GameType.OPEN_GAME_TYPE:
		return "Open Game";
	    case GameType.TOURNAMENT_GAME_TYPE:
		return "Tournament Game";
	    case GameType.STRICT_GAME_TYPE:
		return "Strict Tournament";
	}
	return "Strict Tournament";
    }
}
