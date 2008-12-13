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
 * DNSLookupItem.java
 *
 * Created on 21 May 2004, 23:13
 */

package com.jbolo.gui;

/**
 * DNSLookupItem is a IP address to lookup into a hostname for 
 * a player
 * @author  John Morrison
 */
public class DNSLookupItem {

    /** IP Address to lookup */
    private String ip;
    /** Player number we are looking up */
    private int playerNum;
    
    /** Creates a new instance of DNSLookupItem 
     * @param ip IP Address to lookup
     * @player playerNum Player number we are looking for
     */
    public DNSLookupItem(String ip, int playerNum) {
        this.ip = ip;
        this.playerNum = playerNum;
    }
    
    /**
     * Gets the IP to lookup
     * @return String
     */
    public String getIP() {
        return ip;
    }
    
    /**
     * Gets the player number we are looking up
     * @return int
     */
    public int getPlayerNumber() {
        return playerNum;
    }
    
}
