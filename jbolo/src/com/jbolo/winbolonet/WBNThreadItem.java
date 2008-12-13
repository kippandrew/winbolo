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
 * WBNThreadItem.java
 *
 * Created on 18 May 2004, 22:56
 */

package com.jbolo.winbolonet;

/**
 * WBN formatted data messages waiting to be sent by the thread
 * @author  John Morrison
 */
public class WBNThreadItem {
    
    /** WBN formatted data */
    private byte data[];
    
    /** Creates a new instance of WBNThreadItem 
     * @param data Data in this item
     */
    public WBNThreadItem(byte data[]) {
        this.data = data.clone();
    }
    
    /**
     * Gets the data in this ThreadItem
     * @return byte array
     */
    public byte[] getData() {
        return data;
    }
    
}
