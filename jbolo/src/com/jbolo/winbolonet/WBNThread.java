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
 * WBNThread.java
 *
 * Created on 18 May 2004, 20:44
 */

package com.jbolo.winbolonet;

import java.util.ListIterator;
import com.jbolo.gui.SafeThreadedProcessingQueue;

/**
 * The thread class that sends off winbolo.net events for processing
 * @author  John Morrison
 */
public class WBNThread extends SafeThreadedProcessingQueue {
    
    /** Amount of milliseconds to sleep between processing requests */
    private static int WBN_THREAD_SLEEP_TIME = 500;
    
    /** HTTP object used for sending data */
    private HTTP http;
    
    /** Creates a new instance of WBNThread
     * @param http HTTP object used to transmit data
     */
    public WBNThread(HTTP http) {
        super(WBN_THREAD_SLEEP_TIME);
        this.http = http;
    }

    @SuppressWarnings("unchecked")
	protected void process() {
        ListIterator li;
        WBNThreadItem item;
        
        li = listProcessing.listIterator();
        while (li.hasNext()) {
            item = (WBNThreadItem) li.next();
            http.sendMessage(item.getData(), item.getData().length);
        }                
    }    
}
