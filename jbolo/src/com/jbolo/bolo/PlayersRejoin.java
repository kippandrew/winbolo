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
 * PlayersRejoin.java
 *
 * Created on 5 June 2004, 10:04
 */

package com.jbolo.bolo;

/**
 * Looks after players rejoin and ownerships.
 *
 * @author  John Morrison
 */
public class PlayersRejoin {
    
    /** Maximum rejoin time is 5 minutes (in ticks) */
    private static int MAX_REJOIN_TIME = 6000;
    
    /** Are the players in use */
    private boolean inUse[];
    /** Pills that were owned by a player */
    private int pills[];
    /** Bases that were owned by a player */
    private int bases[];
    /** Player names */
    private String name[];
    /** Timeout time length */
    private int timeOut[];
    
    
    /** Gameworld this players rejoin belongs to */
    private GameWorld gw;
    
    /** Creates a new instance of PlayersRejoin
     *
     * @param gw Gameworld this belongs to
     */
    public PlayersRejoin(GameWorld gw) {
        this.gw = gw;
        
        inUse = new boolean[Players.MAX_PLAYERS];
        pills = new int[Players.MAX_PLAYERS];
        bases = new int[Players.MAX_PLAYERS];
        name = new String[Players.MAX_PLAYERS];
        timeOut = new int[Players.MAX_PLAYERS];
        
        int count = 0;
        while (count < Players.MAX_PLAYERS) {
            inUse[count] = false;
            count++;
        }
    }
    
    /**
     * Called once a game tick to update the rejoin timeouts
     */
    public void update() {
        int count = 0;
        while (count < Players.MAX_PLAYERS) {
            if (inUse[count] == true) {
                timeOut[count]++;
                if (timeOut[count] > MAX_REJOIN_TIME) {
                    inUse[count] = false;
                }
            }
            count++;
        }
    }
    
    /**
     * Adds a player to the rejoin structure
     *
     * @param name Players name
     * @param pills Pills this player owned
     * @param bases Bases this player owned
     */
    public void addPlayer(String name, int pills, int bases) {
        int count = 0;    /* Looping variable */
        int best = 0 ;     /* Best position to add */
        int timeBest = MAX_REJOIN_TIME;  /* Best time to beat - If we can't find a free spot */
        boolean done = false;     /* Finished looping */
        
        
        while (count < Players.MAX_PLAYERS && done == false) {
            if (inUse[count] == false) {
                best = count;
                done = true;
            } else if (timeOut[count] < timeBest) {
                best = count;
                timeBest = timeOut[count];
            }
            count++;
        }
        
        /* Add it to best slot */
        inUse[best]  = true;
        this.name[best] = name;
        this.pills[best] = pills;
        this.bases[best] = bases;
        this.timeOut[best] = 0;
    }
    
    
    public void rejoinRequest(String name, int playerNum) {
        int count = 0;
        int num = 0;
        int testItem;
        boolean found = false;
        
        while (count < Players.MAX_PLAYERS && found == false) {
            if (inUse[count] == true) {
                if (name.equals(this.name[count]) == true) {
                    found = true;
                    num = count;
                }
            }
            count++;
        }
        
        // Found them
        if (found == true) {
            Pillboxes pbs = gw.getPills();
            Bases bss = gw.getBases();
            NetChangeMNT nc = gw.getMNT();

            count = 0;
            while (count < Players.MAX_PLAYERS) {
                // Pills
                testItem = pills[num] >> count;
                testItem &= 1;
                if (testItem == 1) {
                    if (pbs.getOwner(count) == GameItem.NEUTRAL) {
                        pbs.setOwner(count, playerNum, true);
                        nc.add(NetChangeMNT.NMNT_PILLMIGRATE, count, playerNum, 0, 0);     
                    }
                }
                                
                // Bases
                testItem = bases[num] >> count;
                testItem &= 1;
                if (testItem == 1) {
                    if (bss.getOwner(count) == GameItem.NEUTRAL) {
                        bss.setOwner(count, playerNum, true);
                        nc.add(NetChangeMNT.NMNT_BASEMIGRATE, count, playerNum, 0, 0);     
                    }
                }
                
                count++;
            }
            
        }
    }
}
