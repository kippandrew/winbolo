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
 * SysTimers.java
 *
 * Created on January 2, 2004, 2:45 PM
 */

package com.jbolo.gui;

/**
 * Interface that defines all the calls to get system timing information
 * @author  John Morrison
 */
public interface SysTimers {
    
    /** 
     * Gets the current frame rate of the game
     * @return frame rate of the game
     */
    public int getFrameRate();
    
    /** 
     * Gets the current Simulation modelling time of the game
     * @return time spent in milliseconds
     */
    public int getSimulationModellingTime();
    
    /** 
     * Gets the current network time of the game
     * @return time spent in milliseconds
     */
    public int getNetworkTime();
    
    /** 
     * Gets the current time spent in a brain 
     * @return time spent in milliseconds
     */
    public int getAITime();
    
    /** 
     * Gets the current time spent drawing in the game
     * @return time spent in milliseconds
     */
    public int getDrawingTime();
        
}
