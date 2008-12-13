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
 * DNSLookups.java
 *
 * Created on 21 May 2004, 22:59
 */

package com.jbolo.gui;


import java.util.ListIterator;
import java.net.InetAddress;
import com.jbolo.bolo.GameClient;



/**
 *
 * @author  John Morrison
 */
public class DNSLookups extends SafeThreadedProcessingQueue {
    /** Amount of time to sleep between processing entries - 5 seconds */
    private static int DNS_THREAD_SLEEP_TIME = 1000 * 5;
    
    /** The Game Client we are using */
    private GameClient gc;
    
    /** Creates a new instance of DNSLookups */
    public DNSLookups(GameClient gc) {
        super(DNS_THREAD_SLEEP_TIME);
        this.gc = gc;
    }
    
    /**
     * Process the list request
     */
    @SuppressWarnings("unchecked")
	protected void process() {
        ListIterator li = listProcessing.listIterator();
        DNSLookupItem dli;
        InetAddress addr;
        
        
        
        
        while (li.hasNext()) {
            dli = (DNSLookupItem) li.next();
            
            try {
                addr = InetAddress.getByName(dli.getIP());
                gc.setLocation(dli.getIP(), addr.getHostName());
            } catch (Exception e) {
                // Ignore unknown host exception as IP address will already be set in client
            }
            
        }
    }
}
