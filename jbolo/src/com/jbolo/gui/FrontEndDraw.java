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
 * FrontEndDraw.java
 *
 * Created on December 14, 2003, 11:44 PM
 */

package com.jbolo.gui;

import java.util.ArrayList;
import com.jbolo.bolo.ScreenItem;
import com.jbolo.bolo.Point;
import com.jbolo.bolo.ScreenTankItem;

/**
 * The different drawing methods that must be implemeted by a frontend
 * @author  John Morrison
 */
public interface FrontEndDraw {

    /**
     * Writes Messages writes the news ticker to the screen
     * @param topLine Top news ticker line
     * @param bottomLine Bottom news ticker line
     */
    public void feWriteMessages(String topLine, String bottomLine);
    
    /**
     * Writes the number of kills and deaths this player has had
     * @param kills Number of kills to write
     * @param deaths Number of deaths to write
     */
    public void feWriteKillsDeaths(int kills, int deaths);
    
    /**
     * Sets the pillbox status for a particular pillbox
     * @param pillNum The pill number to draw (0-15)
     * @param pb - The allience of the pillbox
     */
    public void feDrawStatusPillbox(int pillNum, int pb);
    
    /** Sets the base status for a particular base
     * @param baseNum The base number to draw
     * @param bs The base status for the base
     */
    public void feDrawStatusBase(int baseNum, int bs);

    /** Sets the tank status for a particular base
     * @param tankNum The tank number to draw
     * @param ts The Tank status for the base
     */
    public void feDrawStatusTank(int tankNum, int ts);
        
    /**
     * Used to draw the base stock amounts
     * @param shells Number of shells
     * @param mines Number of mines
     * @param armour Amount of armour
     * @param redraw Set to true if we want to redraw the last amounts
     */
    public void feDrawStatusBaseBars(int shells, int mines, int armour, boolean redraw);
   
    /**
     * Used to draw the tank stock amounts
     * @param shells Number of shells
     * @param mines Number of mines
     * @param armour Amount of armour
     * @param trees Amount of trees
     */
    public void feDrawStatusTankBars(int shells, int mines, int armour, int trees);
    
    /** Redraws the main graphics window
     * @param graphics Graphics drawing object
     * @param view multidensional array of tiles to draw
     * @param mines multidensional array of whether the tiles contain mines
     * @param tanks ArrayList of ScreenTankItems to draw
     * @param shells ArrayList of ScreenItems to draw for shells
     * @param lgms ArrayList of ScreenItems to draw for lgms
     * @param ScreenItem containing gunsight point or null if not on screen
     * @param cursor Cursor position on the screen or null if cursor is not on the screen
     * @param srtDelay Start delay in ticks -1 for none
     */
    public void feDrawMainView(int[][] view, boolean[][]mines, ArrayList<ScreenTankItem> tanks, ArrayList<ScreenItem> shells, ArrayList<ScreenTankItem> lgms, ScreenItem gunsight, Point cursor, long srtDelay);    

    /**
     * Draws the download line down the screen
     *
     * @param justBlack Just draw the screen black?
     * @param yPos Y Position of the map
     */
    public void feDrawDownloadScreen(boolean justBlack, int yPos);
    
    /**
     * Draws the man arrow status. If isDead is set to true then the angle is ignored.
     *
     * @param isDead Is the man dead
     * @param angle The angle the item is facing
     */
    public void feDrawManStatus(boolean isDead, float angle);
    
    /**
     * Clears the man arrow status.
     */
    public void feDrawClearManStatus();    
}
