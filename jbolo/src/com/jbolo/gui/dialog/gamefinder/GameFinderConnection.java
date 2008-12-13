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
 * GameFinderConnection.java
 *
 * Created on 20 May 2004, 19:52
 */

package com.jbolo.gui.dialog.gamefinder;

import java.util.ArrayList;
import java.awt.Label;


/**
 *
 * @author  John Morrison
 */
public abstract class GameFinderConnection {
    /** The list of games to return */
    protected ArrayList<CurrentGame> games;

    /** Message of the day string */
    protected String motd;
    
    /** Whether the connection succeeded or not */
    protected boolean result;
    
    
    /** Creates a new instance of GameFinderConnection */
    public GameFinderConnection() {
        games = new ArrayList<CurrentGame>();
        result = false;
        motd = "";
    }
    
    /**
     * Gets whether the connection result was successful
     * @return success
     */
    public boolean getResult() {
        return result;
    }
    
    /**
     * Gets the MOTD string
     * @return String
     */
    public String getMOTD() {
        return motd;
    }

    /**
     * Returns the list of games this search found
     * @return ArrayList
     */
    public ArrayList<CurrentGame> getList() {
        return games;
    }
    
    /**
     * Searches for a list of games
     *
     * @param status Status field display messages
     * @return ArrayList of CurrentGame
     */
     public abstract void searchForGames(Label status);
}
