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
 * Player.java
 *
 * Created on October 8, 2003, 11:24 PM
 */

package com.jbolo.bolo;

/** Represents a player in the game
 * @author  John Morrison
 */
public class Player {
    
    /** The player name */    
    private String name;
    
    /** The player location */    
    private String location;

    /** Player map x position */    
    private int mapX;
    
    /** Player map y position */    
    private int mapY;
    
    /** Player pixel x position */    
    private int pixelX;
    
    /** Player pixel y position */    
    private int pixelY;
    
    /** Player frame */    
    private int frame;
    
    /** Is the player on a boat */    
    private boolean onBoat;
    
    /** Player's LGM map x position */    
    private int lgmMapX;
    
    /** Player's LGM map y position */    
    private int lgmMapY;
    
    /** Player's LGM pixel x position */    
    private int lgmPixelX;
    
    /** Player's LGM pixel y position */    
    private int lgmPixelY;
    
    /** Player's LGM pixel frame */    
    private int lgmFrame;
    
    /** Is this player ticked? */    
    private boolean isTicked;
    
    /** Does this player need updating? */    
    private boolean needUpdate;
    
    /** This players alliances */    
    private Alliance allies;
    
    /**
     * Creates a new instance of Player
     * @param location Location of the player
     * @param name Player name
     */
    public Player(String name, String location) {
        allies = new Alliance();
        needUpdate = false;
        isTicked = false;
        lgmFrame = 0;
        lgmPixelX = 0;
        lgmPixelY = 0;
        mapX = 0;
        mapY = 0;
        pixelX = 0;
        pixelY = 0;
        frame = 0;
        this.name = name;
	this.location = location;
    }
    
    /** Returns the player name
     * @return String
     */    
    public String getName() {
	return name;
    }
    
    /** Returns the player location
     * @return String
     */
    public String getLocation() {
	return location;
    }
    
    /**
     * Returns the players full name and location in name@location form
     * @return String
     */
    public String getNameLocation() {
        return name + "@" + location;
    }

    /**
     * Sets the Map X co-ordinate of the player
     *
     * @param mx Value to set
     */
    public void setMX(int mx) {
        this.mapX = mx;
    }
    
    /**
     * Sets the Map Y co-ordinate of the player
     *
     * @param my Value to set
     */
    public void setMY(int my) {
        this.mapY = my;
    }
    
    /**
     * Sets if we need an update
     *
     * @param value value to set
     */
    public void setNeedUpdate(boolean value) {
        needUpdate = value;
    }
    
    /**
     * Returns the players world x position
     * @return int
     */
    public int getWX() {
        return Utils.GetWorldFromMapPixel(mapX, pixelX);
    }
    
    /**
     * Returns the players world x position
     * @return int
     */
    public int getWY() {
        return Utils.GetWorldFromMapPixel(mapY, pixelY);
    }    

    /**
     * Returns the players lgm world x position
     * @return int
     */
    public int getLgmWX() {
        return Utils.GetWorldFromMapPixel(lgmMapX, lgmPixelX);
    }
    
    /**
     * Returns the players lgm world y position
     * @return int
     */
    public int getLgmWY() {
        return Utils.GetWorldFromMapPixel(lgmMapY, lgmPixelY);
    }
    
    /** Returns the player map x position
     * @return int 
     */    
    public int getMX() {
        return mapX;
    }
    
    /** Returns the player map y position
     * @return y
     */    
    public int getMY() {
        return mapY;
    }
    
    /** Returns the player pixel x position
     * @return int
     */    
    public int getPX() {
        return pixelX;
    }
    
    /** Returns the player pixel y position
     * @return int
     */    
    public int getPY() {
        return pixelY;
    }
    
    /** Returns the player frame
     * @return int
     */    
    public int getFrame() {
        return frame;
    }
    
    /** Returns the players lgm x map position
     * @return int
     */    
    public int getLgmMX() {
        return lgmMapX;
    }
    
    /** Returns the players lgm y map position
     * @return int
     */    
    public int getLgmMY() {
        return lgmMapY;
    }
    
    /** Returns the players lgm pixel x position
     * @return int
     */    
    public int getLgmPX() {
        return lgmPixelX;
    }
    
    /** Returns the players lgm pixel y position
     * @return int
     */    
    public int getLgmPY() {
        return lgmPixelY;
    }
    
    /** Returns the players lgm frame
     * @return int
     */    
    public int getLgmFrame() {
        return lgmFrame;
    }
    
    /** Returns if this player is on a boat
     * @return boolean
     */    
    public boolean getOnBoat() {
        return onBoat;
    }
    
    /**
     * Returns if this player is checked
     * @return If this player is checked in the player menu
     */    
    public boolean getChecked() {
        return this.isTicked;
    }
    
    /** Returns if this player needs updating
     * @return boolean
     */    
    public boolean getNeedUpdate() {
        return needUpdate;
    }
    
    /** Sets if this player is checked
     * @param isChecked value to set to
     */    
    public void setChecked(boolean isChecked) {
        isTicked = isChecked;
    }
    /** Sets this players name
     * @param name name to set
     */    
    public void setName(String name) {
        this.name = name;
    }
    
    /**
     * Sets the players location
     * @param location Location to set
     */
    public void setLocation(String location) {
        this.location = location;
    }
    
    /** Sets this players position
     * @param mx map x position
     * @param my map y position
     * @param px pixel x position
     * @param py pixel y position
     * @param frame player frame
     * @param onBoat player on boat
     * @param lgmMX lgm map x position
     * @param lgmMY lgm map y position
     * @param lgmPX lgm pixel x position
     * @param lgmPY lgm pixel y position
     * @param lgmFrame lgm frame
     */    
    public void update(int mx, int my, int px, int py, int frame, boolean onBoat, int lgmMX, int lgmMY, int lgmPX, int lgmPY, int lgmFrame) {
        mapX = mx;
        mapY = my;
        pixelX = px;
        pixelY = py;
        this.frame = frame;
        this.onBoat = onBoat;
        lgmMapX = lgmMX;
        lgmMapY = lgmMY;
        lgmPixelX = lgmPX;
        lgmPixelY = lgmPY;
        this.lgmFrame = lgmFrame;
    }
 
    /** Test to see is a name is the same as this player name (Case insensitive)
     *
     * @return if the name is the same
     * @param name Name to test
     */
    public boolean testNameSame(String name) {
	return name.toLowerCase().equals(this.name.toLowerCase());
    }
    
    /** Test is thi splayer is in an alliance with playerNum
     * @return If this player is an in an alliance with playerNum
     * @param playerNum Player to test if they are in an alliance with
     */
    public boolean allianceExist(int playerNum) {
	return allies.exist(playerNum);
    }
    
    /**
     * Adds an alliance between this player and another
     *
     * @param value Player number to add alliance with
     */
    public void addAlliance(int value) {
        allies.add(value);
    }
    
    /**
     * Removes an alliance between this player and another
     *
     * @param value Player number to remove alliance with
     */
    public void removeAlliance(int value) {
        allies.remove(value);
    }
    
    /**
     * Updates the LGM frame for this player if the LGM is out to give the illusion
     * of movement
     */
    public void lgmFrameUpdate() {
        if (lgmMapX != 0 && lgmMapY != 0 && lgmFrame != LGM.LGM_HELICOPTER_FRAME) {
            lgmFrame++;
            if (lgmFrame > LGM.LGM_MAX_FRAMES) {
                lgmFrame = 0;
            }
        }
    }
    
    /**
     * Returns the number of allies this player has
     *
     * @return number of allies this player has
     */
    public int getNumAllies() {
        return allies.getCount();
    }

    /**
     * Returns this net alliances as a byte array
     *
     * @return a byte array containing the list of allies. The array size is the number of allies
     */
    public byte[] makeNetAllies() {
        return allies.makeNetAlliances();
    }
    
    /**
     * Removes all alliances we have
     */
    public void clearAllAlliances() {
        allies.clearAllAlliances();
    }
    
}

