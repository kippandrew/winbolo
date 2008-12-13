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

package com.jbolo.bolo;

import java.io.ByteArrayOutputStream;

/** The Player Starts object
 * @author John Morrison
 */
public class Starts extends GameItems {

    /** Size of the starts memory structure */
    public static final int SIZEOF_STARTS = 49;
    
    /** Constructor
     * @param gw The game world that created this GameItems
     */
    public Starts(GameWorld gw) {
        super(gw);
        items = new Start[GameItems.MAX_ITEMS];
    }
    
    /** Sets the number of items in this game items
     * @param numItems Number of items to set
     */
    public void setNumItems(int numItems) throws Exception {
        super.setNumItems(numItems);
        byte count = 0;
        
        while (count < numItems) {
            items[count] = new Start(count);
            count++;
        }
    }
    
    /** Sets up a specific start
     * @param itemNum Item number to set
     * @param x X position of the start
     * @param y Y position of the start
     * @param dir direction of the start
     */
    public void set(int itemNum, int x, int y, byte dir) {
        Start ss;
        ss = (Start) items[itemNum];
        ss.set(x, y, dir);
    }
    
    /** Returns a random start
     * @return A Start
     */
    public Start getStart() {
        int count;
        int val;
        boolean okTank;
        boolean okPill;
        boolean found;
        int rnd;
        int bt;
        long secondChoice = -1;
        Start s = null;
        
        val = 0;
        if (numItems < 1) {
            return new Start((byte) 0);
        }
        
        found = false;
        count = 0;
        while (count < numItems && found == false) {
            okPill = true;
            okTank = true;
            rnd = (int) (Math.random()+1) % numItems;
            s = (Start) items[rnd];
            found = true;
            // okTank = screenCheckTankRange((*value)->item[rnd].x, (*value)->item[rnd].y, playerNum, 512.0);^M
            //okPill = screenCheckPillsRange((*value)->item[rnd].x, (*value)->item[rnd].y, playerNum, 2048.0);
        }
        return s;
    }
    
    public Start getStart(int playerNum) {
        int count;
        int val;
        boolean okTank;
        boolean okPill;
        boolean found;
        int rnd;
        int bt;
        long secondChoice = -1;
        Start s = null;
        Pillboxes pbs = gw.getPills();
        
        val = 0;
        if (numItems < 1) {
            return null;
        }
        
        found = false;
        count = 0;
        while (count < numItems && found == false) {
            okPill = true;
            okTank = true;
            rnd = (int) (Math.random()+1) % numItems;
            s = (Start) items[rnd];
            found = true;
            //FIXME	    okTank = gw.checkTankRanges(s.getX(), s.getY(), playerNum, 512.0);
            okPill = pbs.checkRanges(s.getX(), s.getY(), playerNum, 2048.0);
            
            if (okTank == true) {
                /* Not under Check for a bigger range */
                //FIXME		okTank = gw.checkTankRanges(s.getX(), s.getY(), playerNum, 2048.0);
            }
            if (okPill == true) {
                secondChoice = count;
            }
            if (okTank == true && okPill == true) {
                found = true;
                return s;
            }
        }
        if (secondChoice != -1) {
            return (Start) items[(int) secondChoice];
        }
        
        return (Start) items[(int) ((Math.random()+1) % numItems)];
    }
    
    /**
     * Checks to see if a game item at a certain position is capturable or not
     *
     * @return If the game item is capturable
     * @param xValue X position of the pill
     * @param yValue Y position of the pill
     */
    public boolean isCapturable(int xValue, int yValue) {
        return false;
    }
    
    public void updateFrontEndStatus(int itemNum) {
    }
    
    /**
     * Sets this game item from the array of bytes received from the network
     *
     * @param data byte array to create from
     */
    public void setFromNetData(byte[] data, int offset) {
        int numItems;
        Start s;
        int dataPos;
        int x;
        int y;
        byte dir;
        byte count = 0;
        
        numItems = data[offset+SIZEOF_STARTS-1];
        try {
            setNumItems(numItems);
        } catch (Exception e) {
            e.printStackTrace();
        }
        dataPos = offset;
        while (count < numItems) {
            s = new Start(count);
            x = Utils.ConvertByteToInt(data[dataPos]);
            dataPos++;
            y = Utils.ConvertByteToInt(data[dataPos]);
            dataPos++;
            dir = data[dataPos];
            dataPos++;
            
            s.set(x,  y, dir);
            items[count] = s;
            count++;
        }
        // get rid of old items
        while (count < MAX_ITEMS) {
            items[count] = null;
            count++;
        }
    }
    
    /**
     * Generates a network sendable data array for copying to other players
     *
     * @return byte array to send
     */
    public byte[] makeNetData() {
        ByteArrayOutputStream bos = new ByteArrayOutputStream(SIZEOF_STARTS);
        int count = 0;
        Start s;
        while (count < numItems) {
            s = (Start) items[count];
            bos.write(s.getX());
            bos.write(s.getY());
            bos.write(s.getDir());
            count++;
        }
        bos.write(numItems);
        return bos.toByteArray();
    }
    
}

