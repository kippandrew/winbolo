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
 * BoloMapRun.java
 *
 * Created on 13 June 2004, 05:26
 */

package com.jbolo.bolo.map;

/**
 * MapRun used in writing map files
 * @author  John Morrison
 */
public class BoloMapRun {
    
    /** Data in the run */
    private byte[] data;
    /** Length of the data in the run */
    private int dataLen;
    /** Nibble flag for writing runs */
    private boolean nibbleFlag;
    /** X map co-ordinate we are up to after processing run */
    private int x;
    /** Y map co-ordinate we are up to after processing run */
    private int y;
    /** Start X map co-ordinate of this run */
    private int startX;
    
    /** Creates a new instance of BoloMapRun
     *
     * @param x X map co-ordinate at start
     * @param y Y map co-ordinate at start
     */
    public BoloMapRun(int x, int y) {
        data = new byte[255];
        dataLen = 0;
        nibbleFlag = false;
        this.x = x;
        this.y = y;
        startX = 0;
    }
    
    /**
     * Sets the X and Y Map co-ordinates
     * @param x X map co-ordinate to set
     * @param y Y map co-ordinate to set
     */
    public void set(int x, int y) {
        this.x = x;
        this.y = y;
    }
    
    /**
     * Sets the start x map co-ordinate of this run
     * @param value X map co-ordinate to set
     */    
    public void setStartX(int value) {
        startX = value;
    }
    
    /**
     * Gets the Starting X co-ordinate of this run
     * @return X Map Co-ordinate
     */    
    public int getStartX() {
        return startX;
    }
    
    /**
     * Gets the X map co-ordinate we are set to after finishing this run
     * @return X Map co-ordinate
     */    
    public int getX() {
        return x;
    }
    
    
    /**
     * Gets the X map co-ordinate we are set to after finishing this run
     * @return Y Map co-ordinate
     */    
    public int getY() {
        return y;
    }
    
    /**
     * Gets the 4 byte header to write to the map file. Format is:
     * Each run is described by a run header followed by the run data.
     * The run header structure is:
     * BYTE datalen;// length of the data for this run INCLUDING this 4 byte header
     * BYTE y;      // y co-ordinate of this run.
     * BYTE startx; // first square of the run
     * BYTE endx;   // last square of run + 1 (ie first deep sea square after run)
     * @return 4 byte run header
     */    
    public byte[] getHeader() {
        byte header[] = new byte[4];
        
        header[0] = (byte) (dataLen + 4);
        header[1] = (byte) y;
        header[2] = (byte) startX;
        header[3] = (byte) x;
        return header;
    }
    
    /**
     * Gets the length of the data in the run array
     * @return Data length
     */    
    public int getDataLen() {
        return dataLen;
    }
    
    /**
     * Adds a nibble to the map run.
     * @param value Nibble value to add
     */    
    public void putNibble(int value) {
        // #define put_nibble(X) (!nibble_flag ? (nibble_flag = TRUE,  *nibble_data = (X)<<4) : (nibble_flag = FALSE, *nibble_data++ |= (X) & 0xF))
        
        if (nibbleFlag == false) {
            nibbleFlag = true;
            data[dataLen] = (byte) (value << 4);
        } else {
            nibbleFlag = false;
            data[dataLen] |= value & 0xF;
            dataLen++;
        }
    }
    
    /**
     * Array of data that is the compressed map run.
     * Note the array length is not the size of the data. It is the size
     * of the buffer. You must call getDataLength() to determine it.
     * @return byte array
     */    
    public byte[] getData() {
        return data;
    }
    
    /**
     * Called at the end of the run to add a final nibble to
     * the compressed run if required
     */    
    public void finish() {
        if (nibbleFlag == true) {
            putNibble(0);
        }
    }
}
