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


package com.jbolo.bolo.map;


public class MapNetItem {
    /** X position where the MapNetItem occurs */
    private int mx;
    
    /** Y position where the MapNetItem occurs */
    private int my;
    
    /** terrain the MapNetItem is to change to */
    private int terrain;
    
    /** Does this change need to be sent to the server or clients */
    private boolean needSend;
    
    /** Time we have been waiting for the change to be approved */
    private int length;
    
    /** terrain the MapNetItem was prior to the change */
    private int oldTerrain;
    
    /**
     * Operation
     *
     * @param mx
     * @param my
     * @param terrain
     * @param oldTerrain
     * @param needSend
     * @param length
     * @return
     */
    public MapNetItem(int mx, int my, int terrain, int oldTerrain, boolean needSend, int length) {
	this.mx = mx;
	this.my = my;
	this.terrain = terrain;
	this.oldTerrain = oldTerrain;
	this.needSend = needSend;
	this.length = length;
    }
    /**
     * Gets the X position of the change
     *
     * @return X position of the change
     */
    public int getX() {
	return mx;
    }
    
    /**
     * Gets the Y position of the change
     *
     * @return Y position of the change
     */
    public int getY() {
	return my;
    }
    
    /**
     * Gets the terrain the change is
     *
     * @return the terrain the change is
     */
    public int getTerrain() {
	return terrain;
    }
    
    /**
     * Gets the terrain prior to the change
     *
     * @return the terrain prior to the change
     */
    public int getOldTerrain() {
	return oldTerrain;
    }
    
    /**
     * Gets if the change needs to be sent to the client/server
     *
     * @return If the change needs to be sent to the client/server
     */
    public boolean getNeedSend() {
	return needSend;
    }
    
    /**
     * Sets the length of time a change has been waiting
     *
     * @param length Length of time the change has been waiting to set
     */
    public void setLength(int length) {
	this.length = length;
    }
    
    /**
     * Sets the terrain of this change
     *
     * @param terrain The terrain to set for this change
     */
    public void setTerrain(int terrain) {
	this.terrain = terrain;
    }
    
    /**
     * Sets if we need to send this change
     *
     * @param needSend Does this change need to be sent.
     */
    public void setNeedSend(boolean needSend) {
	this.needSend = needSend;
    }
    
    /**
     * Increments the length of time this change has been waiting
     */
    public void incrementLength() {
	length++;
    }
    
    /**
     * Gets the length of time this change has been waiting
     *
     * @return the length of time this change has been waiting
     */
    public int getLength() {
	return length;
    }
}
