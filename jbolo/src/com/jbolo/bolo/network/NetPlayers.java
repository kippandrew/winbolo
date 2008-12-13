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
 * NetPlayers.java
 *
 * Created on 5 June 2004, 08:54
 */

package com.jbolo.bolo.network;


import java.net.SocketAddress;
import com.jbolo.bolo.Players;


/**
 * Handles keeping track of all players address in the game.
 * 
 * @provides bolo/netplayer.c
 * @author  John Morrison
 */
public class NetPlayers {
    
    /** The player number is not found from this address */
    public static int NOT_FOUND = (0xAF);
    
    /** Check for removing every 30 seconds  (in ms) */
    private static int INACTIVEREMOVE_TIME = 30 * 1000;
    
    
    /** Players in the game */
    private NetPlayer np[];
    
    /** Creates a new instance of NetPlayers */
    public NetPlayers() {
        int count = 0;
        np = new NetPlayer[Players.MAX_PLAYERS];
        while (count < Players.MAX_PLAYERS) {
            np[count] = null;
            count++;
        }
    }
    
    /**
     * Returns if this player is in use or not
     *
     * @erturn boolean
     * @param playerNum Player number to check
     */
    public boolean getInUse(int playerNum) {
        if (np[playerNum] == null) {
            return false;
        }
        return true;
    }
    
    /**
     * Gets the address of a player
     * @param playerNum Player to get the address for
     */
    public SocketAddress getAddress(int playerNum) {
        return np[playerNum].getAddress();
    }
    
    /**
     * Sets the address of a player
     * @param playerNum Player to set the address for
     * @param address address to set
     */
    public void setAddress(int playerNum, SocketAddress address) {
        np[playerNum].setAddress(address);
    }
    
    /**
     * Sets up the netPlayers item for playerNum. Sets the last heard time to now
     *
     * @param playerNum Player to set
     * @param address Players address
     */
    public void setPlayer(int playerNum, SocketAddress address) {
        np[playerNum] = new NetPlayer(address);
    }
    
    /**
     * Gets the player number that matches the address. If an address matches
     * updates their last heard time
     *
     * @return player number or NOT_FOUND if player is not found
     * @param address Address to get for
     */
    public int getPlayerNumber(SocketAddress address) {
        int returnValue = NOT_FOUND;
        int count = 0;
        
        while (count < Players.MAX_PLAYERS && returnValue == NOT_FOUND) {
            if (np[count] != null) {
                if (np[count].isAtAddress(address) == true) {
                    returnValue = count;
                }
            }
            count++;
        }
        
        return returnValue;
    }
    
    /**
     * Sets this player to be in game or not
     *
     * @param playerNum player to set
     * @param value Value to set to
     */
    public void setInGame(int playerNum, boolean value) {
        np[playerNum].setInGame(value);
    }
    
    /**
     * Gets if this player is in game or just joining
     *
     * @return boolean
     * @param playerNum player to set
     */
    public boolean getInGame(int playerNum) {
        if (np[playerNum] != null) {
            return np[playerNum].isInGame();
        }
        return false;
    }
    
    /**
     * Gets if this player to has done the password check
     *
     * @return boolean
     * @param playerNum player to get
     */
    public boolean hasPassed(int playerNum) {
        if (np[playerNum] != null) {
            return np[playerNum].getHasPassed();
        }
        return false;
    }
    
    /**
     * Sets this player to has done the password check
     *
     * @param playerNum player to set
     */
    public void donePassword(int playerNum) {
        np[playerNum].setHasPassed(true);
    }
    
    /**
     * Removes this player from NetPlayers
     *
     * @param playerNum player to remove
     */
    public int removePlayerNum(int playerNum) {
        np[playerNum] = null;
        return playerNum;
    }
    
    /**
     * Gets whether all players in this game have locked or not
     * @return boolean
     */
    public boolean getIsLocked() {
        boolean returnValue = true;
        int count = 0;
        
        while (returnValue == true && count < Players.MAX_PLAYERS) {
            if (np[count] != null) {
                returnValue = np[count].isLocked();
            }
            count++;
        }
        return returnValue;
    }
    
    /**
     * Sets whether this player has locked or not
     */
    public void setLock(int playerNum, boolean lockState) {
        if (np[playerNum] != null) {
            np[playerNum].setLocked(lockState);
        }
    }
    
    /**
     * Gets the UDP packets for this player
     *
     * @return UDPPackets for the player
     * @playerNum Player to get for
     */
    UDPPackets getUDPPackets(int playerNum) {
        return np[playerNum].getUDPPackets();
    }
    
    /**
     * Players address for playerNum is required
     *
     * @return players address or null if no player for that playerNum
     * @param playerNum player to get address for
     */
    public SocketAddress GetPlayerDataReq(int playerNum) {
        if (np[playerNum] != null) {
            return np[playerNum].getAddress();
        }
        return null;
    }
    
    
    /**
     * Returns if thie player should be disconnected due to inactivity
     *
     * @param playerNum player number to remove
     */
    public boolean shouldDisconnect(int playerNum) {
        boolean returnValue = false; // Value to return
        
        if (np[playerNum] != null) {
            if (System.currentTimeMillis() - np[playerNum].getLastHeard() > INACTIVEREMOVE_TIME) {
                returnValue = true;
            }
        }
        return returnValue;
    }
    
    
    /**
     * Checks if this player should be removed for cheating
     *
     * @return if the player should be removed for cheating
     * @param playerNum player number to check
     * @param time client timestamp
     * @param ourTime Server timestamp
     */
    public boolean check(int playerNum, long time, long ourTime) {
        if (np[playerNum] != null) {
            return np[playerNum].check(time, ourTime);
        }
        return false;
    }
    
    /**
     * Sets this player to be a cheater and to be removed
     *
     * @param playerNum Player to set
     */
    public void setCheater(int playerNum) {
        if (np[playerNum] != null) {
            np[playerNum].setCheat(150);
        }
    }
}
