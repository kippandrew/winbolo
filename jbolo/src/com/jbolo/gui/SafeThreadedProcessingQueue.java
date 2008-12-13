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
 * SafeThreadedProcessingQueue.java
 *
 * Created on 21 May 2004, 23:04
 */

package com.jbolo.gui;


import java.util.ArrayList;
import java.util.ListIterator;


/**
 *
 * @author  John  Morrison
 */
public abstract class SafeThreadedProcessingQueue implements Runnable {
    
    /** Amount of time to sleep */
    private int sleepTime;    
    /** The thread that runs */
    private Thread thread;
    /** List of waiting events to send */
    private ArrayList listWaiting;
    /** List of events that are being processed */
    protected ArrayList listProcessing;
    
    /** Creates a new instance of SafeThreadedProcessingQueue */
    public SafeThreadedProcessingQueue(int sleepTime) {
        this.sleepTime = sleepTime;
        thread = new Thread(this);
        listWaiting = new ArrayList();
        listProcessing = new ArrayList();        
    }
    
    /**
     * Run method
     */
    public void run() {
        Thread currentThread = Thread.currentThread();
        
        while (thread == currentThread) {
            if (getLength() > 0) {
                prepareProcessingList();
                process();
            }
            try {
                currentThread.sleep(sleepTime);
            } catch (Exception e) {
                // Ok to ignore this exception
            }
        }
    }
   
    public void start() {
        try {
            thread.start();
        } catch (Exception e) {
            // Ignore the thread is already running exception
        }
    }
    
    protected abstract void process();
    
    /**
     * Adds an object to the waiting list of objects to be processed
     *
     * @param object Object add
     */
    public synchronized void addRequest(Object object) {
        listWaiting.add(object);
    }
    
    /**
     * Stops processing the current list of events
     */
    public void stop() {
        thread = null;
    }
    
    /**
     * Copies the waiting list to the processing list for processing and
     * resets the waiting list
     */
    private synchronized void prepareProcessingList() {
        listProcessing = listWaiting;
        listWaiting = new ArrayList();
    }
    
    /**
     * Gets the length of the waiting list to see if they need processing
     *
     * @return Number of events in the waiting list
     */
    private synchronized int getLength() {
        return listWaiting.size();
    }

    
}
