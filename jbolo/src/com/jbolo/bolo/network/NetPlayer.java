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
 * NetPlayer.java
 *
 * Created on 5 June 2004, 08:48
 */

package com.jbolo.bolo.network;

import java.net.SocketAddress;

/**
 * Net player represents a player in the game
 * @provides bolo/netplayer.c
 * @author  John Morrison
 */
public class NetPlayer {
    
    
    /** Is this player in the game */
    private boolean inGame;
    /** Has this player client locked */
    private boolean locked;
    /** Has this player passed the password check */
    private boolean hasPassed;
    /** Packets player has sent */
    private UDPPackets packets;
    /** Last time we heard from this player */
    private long lastHeard;
    /** Server time of this player */
    private long serverTime;
    /** Client time of this player */
    private long clientTime;
    /** Players cheat count */
    private int cheatCount;
    /** Address of this player */
    private SocketAddress address;
    
    /** Creates a new instance of NetPlayer */
    public NetPlayer(SocketAddress address) {
        this.address = address;
        locked = false;
        inGame = false;
        hasPassed = false;
        cheatCount = 0;
        packets = new UDPPackets();
        lastHeard = System.currentTimeMillis();
    }
    
    /**
     * Returns if thie NetPlayers address matches the address
     * If it does the last heard time is updated to now
     *
     * @return boolean
     * @param address Address to check
     */
    public boolean isAtAddress(SocketAddress address) {
        if (this.address.equals(address) == true) {
            if (cheatCount > 0) {
                cheatCount--;
            }
            lastHeard = System.currentTimeMillis();
            return true;
        }
        return false;
    }
    
    
    public boolean isInGame() {
        return inGame;
    }
    
    public void setInGame(boolean inGame) {
        this.inGame = inGame;
    }
    
    public void setLocked(boolean locked) {
        this.locked = locked;
    }
    
    public boolean isLocked() {
        return locked;
    }
    
    public boolean getHasPassed() {
        return hasPassed;
    }
    
    public void setHasPassed(boolean value) {
        hasPassed = value;
    }
    
    public UDPPackets getUDPPackets() {
        return packets;
    }
    
    public long getLastHeard() {
        return lastHeard;
    }
    
    public SocketAddress getAddress() {
        return address;
    }
    
    public void setAddress(SocketAddress address) {
        this.address = address;
    }
    
    public void setCheat(int value) {
        cheatCount = value;
    }
    
    public boolean check(long time, long serverTime) {
        long clientDiff;
        long serverDiff;
        long diff;
        
        serverDiff = serverTime - this.serverTime;
        this.serverTime = serverTime;
        clientDiff = time - this.clientTime;
        clientTime = time;
        diff = serverDiff - clientDiff;
        
        if (diff < -1 || cheatCount > 100) {
            //     (*value).cheatCount[playerNum] += 10;
            //      printf("incrementing... %d\n", (*value).cheatCount[playerNum]);
            if (cheatCount > 100) {
                //        printf("Threshold too high...\n");
                return true;
                
            }
        }
        
        return false;
    }    
}
