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
 * WBNEvent.java
 *
 * Created on 16 May 2004, 22:54
 */

package com.jbolo.winbolonet;

/**
 * WBNEvent represents a single event to send to winbolo.net
 * @author  John Morrison
 */
public class WBNEvent {
    
    /** Player A's WBN key or null for non-participant */
    private byte[] keyA;
    /** Player B's WBN key or null for non-participant */
    private byte[] keyB;
    /** WBN Event type */    
    private byte event;
    
    /** Creates a new instance of WBNEvent 
     *
     * @param eventType type of this event
     * @param keyA Key of player A
     * @param keyB Key of player B
     */
    public WBNEvent(byte eventType, byte keyA[], byte keyB[]) {
        int count = 0;
        
        this.keyA = new byte[WBN.WINBOLONET_KEY_LEN];
        this.keyB = new byte[WBN.WINBOLONET_KEY_LEN];
        event = eventType;
        while (count < WBN.WINBOLONET_KEY_LEN) {
            this.keyA[count] = keyA[count];
            this.keyB[count] = keyB[count];
            count++;
        }
    }
   
    /**
     * Gets the event type
     * @return byte containing the event type
     */
    public byte getEventType() {
        return event;
    }
    
    /**
     * Returns the Player A winbolo.net key
     * @return byte array containing the winbolo.net key
     */
    public byte[] getKeyA() {
        return keyA;
    }
    
    /**
     * Returns the Player B winbolo.net key
     * @return Byte array containing the winbolo.net key
     */
    public byte[] getKeyB() {
        return keyB;
    }
}
