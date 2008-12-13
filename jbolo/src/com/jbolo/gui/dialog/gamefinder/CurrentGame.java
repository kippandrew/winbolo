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
 * CurrentGame.java
 *
 * Created on 20 May 2004, 19:30
 */

package com.jbolo.gui.dialog.gamefinder;

/**
 * A current game that has been found on the tracker or by broadcast search
 * @author  John Morrison
 */
public class CurrentGame {
    /** Colon to put between the address and port */
    private static String COLON = ":";
    
    /** Server address of the game */
    private String address;
    /** Name of map being played */
    private String mapName;
    /** Version number */
    private String version;
    /** Port of the game */
    private int port;
    /** Number of players */
    private int numPlayers;
    private int numBases;
    private int numPills;
    private boolean mines;
    private int gameType;
    private int ai;
    private boolean password;
    
    
    
    
    /**
     * Creates a new instance of CurrentGame
     *
     * @throw Exception if any string values are null
     * @param address Server address
     * @param mapName Map being played
     * @param version Version being played
     * @param port Game port
     * @param numPlayers Number of players in the game
     * @param numBases Number of bases in the game
     * @param numPills Number of pillboxes in the game
     * @param mines Are mines allowed in the game
     * @param gameType Game type of the game
     * @param ai AI players allowed
     * @param password Does the game have a password
     */
    public CurrentGame(String address, String mapName, String version, int port, int numPlayers, int numBases, int numPills,     boolean mines, int gameType, int ai, boolean password) throws Exception {
        this.address = address;
        this.mapName = mapName;
        this.version = version;
        this.port = port;
        this.numPlayers = numPlayers;
        this.numBases = numBases;
        this.numPills = numPills;
        this.mines = mines;
        this.gameType = gameType;
        this.ai = ai;
        this.password = password;
        
        if (address == null || mapName == null || version == null) {
            throw new Exception("parameters are not allowed to be null!");
        }
    }
    
    /**
     * Gets this addresses host:port string
     * @return String
     */
    public String getAddressPort() {
        return address + COLON + port;
    }
    
    /**
     * Returns if this CurrentGame matches the host:port string to test
     * @return boolean
     * @param check Address in host:port format
     */
    public boolean matches(String check) {
        return check.equals(getAddressPort());
    }
    
    
    /**
     * Gets if mines are allowed in this game
     * @return boolean
     */    
    public boolean allowMines() {
        return mines;
    }
    
    /**
     * The game type of this game
     * @return int
     */    
    public int getGameType() {
        return gameType;
    }
    
    /**
     * Geta the AI type of the game
     * @return int
     */    
    public int getAI() {
        return ai;
    }
    
    /**
     * Returns if this game password protected
     * @return boolean
     */    
    public boolean hasPassword() {
        return password;
    }
    
    /**
     * Gets the server address
     * @return String
     */    
    public String getAddress() {
        return address;
    }
    
    /**
     * Gets the map being played
     * @return String
     */    
    public String getMapName() {
        return mapName;
    }
    
    /**
     * Gets the game version number
     * @return String
     */    
    public String getVersion() {
        return version;
    }
    
    /**
     * Gets the game port
     * @return int
     */    
    public int getPort() {
        return port;
    }
    
    /**
     * Gets the number of players in the game
     * @return int
     */    
    public int getNumPlayers() {
        return numPlayers;
    }
    
    /**
     * Gets the number of bases in the game
     * @return int
     */    
    public int getNumBases() {
        return numBases;
    }
    
    /**
     * Gets the number of pillboxes in the game
     * @return int
     */    
    public int getNumPills() {
        return numPills;
    }
    
}
