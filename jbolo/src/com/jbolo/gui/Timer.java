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
 * Timer.java
 *
 * Created on November 2, 2003, 6:59 PM
 */

package com.jbolo.gui;


import com.jbolo.bolo.GameWorld;

/**
 * Base class for a timing thread.
 *
 * @see FrameTimer
 * @see GameTimer
 * @author  John Morrison
 */
public abstract class Timer implements Runnable {
    /** The game world associated with this timer */
    protected GameWorld gw;
    /** The number of milliseconds to wait between calling the timer */
    protected int waitTime;
    /** Is this thread running */
    private boolean running;
    /** This thread */
    private Thread thread;
    /** Number of milliSeconds passed used to determin when second is called */
    private long milliSeconds;
    /** Amount of time spent processing this second */
    private int processingTime;
    /** Amount of time spent processing last second */
    private int lastProcessingTime;
    
    /** Creates a new instance of a Timer 
     * @param gw GameWorld this timer is associated with 
     * @param waitTime Number of milliseconds to wait between calling the timer
     */
    public Timer(GameWorld gw, int waitTime) {
	this.gw = gw;
	this.waitTime = waitTime;
	running = false;
    }
    
    /**
     * Tells this thread to start
     */
    public void start() {
	thread = new Thread(this);
	thread.start();
	processingTime = 0;
	lastProcessingTime = 0;	
	milliSeconds = System.currentTimeMillis();
    }
    
    /**
     * The timers run method
     */
    public void run() {
	Thread myThread = Thread.currentThread();
	long time = System.currentTimeMillis();
        long startProcessTime;
        long sleepTime;
        
        
	while (thread == myThread) {
	    try {
		if (System.currentTimeMillis() - milliSeconds >= 1000) {
		    second();
		    lastProcessingTime = processingTime;
		    processingTime = 0;
		    milliSeconds = System.currentTimeMillis();
		}
		    
                sleepTime = (System.currentTimeMillis() - time);
                if (sleepTime > waitTime) {
                    sleepTime = waitTime;
                } else {
                    sleepTime = waitTime - sleepTime;
                }
		Thread.sleep(sleepTime);
		time = System.currentTimeMillis();
		process();
		processingTime += (System.currentTimeMillis() - time);
	    } catch (Exception e) {
		e.printStackTrace();
	    }
	}
    }
    
    /**
     * Tells this thread to stop
     */
    public void stop() {
	thread = null;
    }

    /** Gets the time taken to process the timers process() method last second
     * @return the time in milliseconds
     */
    public int getProcessingTime() {
	return lastProcessingTime;
    }
    
    /**
     * Process is called every time the timer reaches waittime. Must be implemented
     * by timer classes
     */
    abstract void process();
    
    /**
     * Called once per second to update and counters required
     */
    abstract void second();
    
}
