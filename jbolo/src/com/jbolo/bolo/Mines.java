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



package com.jbolo.bolo;

import com.jbolo.bolo.map.BoloMap;

/*
 * Mines.java
 *
 * Created on October 7, 2003, 8:08 AM
 */

/** Object stores whether our tank knows whether mines are on the map
 * @author John Morrison
 */
public class Mines {
    
    /** Mine positions as known by our tank */    
    private boolean[][] pos;
    /** Does this game have hidden mines */    
    private boolean allowHiddenMines;
    
    /** GameWorld that owns this */
    private GameWorld gw;
    
    /** Creates a new instance of Mines
     *
     * @param gw GameWorld that owns this
     * @param allowHiddenMines Does this game allow hidden mines
     */
    public Mines(GameWorld gw, boolean allowHiddenMines) {
        int count;
        int count2;
        
        this.allowHiddenMines = allowHiddenMines;
	this.gw = gw;
        pos = new boolean[256][256];
        for (count=0;count<256;count++) {
            for (count2=0;count2<256;count2++) {
                pos[count][count2] = false;
            }
        }
    }
    
    /** Returns if this game allows hidden mines
     * @return boolean
     */    
    public boolean getAllowHiddenMines() {
        return allowHiddenMines;
    }

    /** Sets if this game allows hidden mines
     * @param value Value to set
     */    
    public void setAllowHiddenMines(boolean value) {
        allowHiddenMines = value;
    }
    
    
    /** Adds a mine to this tanks known mines
     * @param x X map co-ordinate to add a mine at
     * @param y Y map co-ordinate to add a mine at
     * @return If a mine existed in this position already
     */    
    public boolean add(int x, int y) {
        boolean returnValue; // Value to return
        if (x <= BoloMap.MAP_MINE_EDGE_LEFT || x >= BoloMap.MAP_MINE_EDGE_RIGHT || y <= BoloMap.MAP_MINE_EDGE_TOP || y >= BoloMap.MAP_MINE_EDGE_BOTTOM) {
            return true;
        }
        returnValue = pos[x][y];
        pos[x][y] = true;
        return returnValue;
    }
    
    /** Removes a mine from the tanks known mines
     * @param x X map co-ordinate to remove mine from
     * @param y Y map co-ordinate to remove mine from
     */    
    public void remove(int x, int y) {
       pos[x][y] = false;   
    }
    
    /** existPos checks if a mine is known to the tank at map position x,y
     * @param x X map co-ordinate to check
     * @param y Y map co-ordinate to check
     * @return boolean
     */    
    public boolean existPos(int x, int y) {
        if (x <= BoloMap.MAP_MINE_EDGE_LEFT || x >= BoloMap.MAP_MINE_EDGE_RIGHT || y <= BoloMap.MAP_MINE_EDGE_TOP || y >= BoloMap.MAP_MINE_EDGE_BOTTOM) {
            return true;
        } else if (allowHiddenMines == true) {
            return pos[x][y];
        }
    
	return gw.getBoloMap().isMine(x, y);

    }
}
