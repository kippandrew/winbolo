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

import java.util.ListIterator;
import com.jbolo.bolo.map.BoloMap;

/*
 * MinesExplosionTimedTerrain.java
 *
 * Created on October 6, 2003, 8:03 AM
 */

/**
 * MinesExplostionTimedTerrain is the class for all exploding mines
 * @author John Morrison
 */
public class MinesExplosionTimedTerrain extends TimedTerrain {
   
    /** Time to wait between chain reacting mines */
    private static int WAIT_TIME = 10;
    
    /** Creates a new instance of MinesExplosionTimedTerrain 
     *
     * @param gw GameWorld that owns this TimedTerrain
     */
    public MinesExplosionTimedTerrain(GameWorld gw) {
	super(gw, WAIT_TIME);
    }
   
    /** processCheckSurroundings processes the mine explosion
     *
     * @param existingLi The existing ListIterator used
     * @param mp The Bolo map these items are on
     * @param pbs Pillboxes on this map
     * @param bss Bases on this map
     * @param x X map position of this event
     * @param y Y map position of this event
     * @param above Above of the event
     * @param below  Below of the event
     * @param left Left of the event
     * @param right Right of the event
     */    
    void processCheckSurroundings(BoloMap mp, Pillboxes pbs, Bases bss, int x, int y, int above, int below, int left, int right) {
	int pos;
	
	pos = mp.getPos(x, y);
	gw.getMines().remove(x,y);
	if (Terrain.IsMinedTerrain(pos) == true) {
	    mp.setPos(x, y, Terrain.CRATER, false, false);
	    gw.getSoundDist().soundDist(SoundDist.MINE_EXPLOSION_NEAR, x, y);
	    /* FIXME
	     count = 1;    
	     while (count <= numLgm) {      
	     lgmDeathCheck(lgms[count-1], mp, pb, bs, (WORLD) ((mx << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), (WORLD) ((my << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), NEUTRAL);      
	     count++;    }    
	     screenCheckTankMineDamage(mx, my);
	     */
	    gw.checkTankMineDamage(x, y);
	    gw.getExplosions().add(x, y, 0, 0, Explosion.START);
	    gw.getFloodFill().add(x, y);
	    switch (pos - Terrain.MINE_SUBTRACT) {
		case Terrain.GRASS:
		    gw.getGrass().removePos(x, y);
		    break;
		case Terrain.SWAMP:
		    gw.getSwamp().removePos(x, y);
		    break;
		case Terrain.RUBBLE:
		    gw.getRubble().removePos(x, y);
		    break;
	    }
	    if (Terrain.IsMinedTerrain(above) == true) {
		addWaiting(x, y-1);
	    }
	    if (Terrain.IsMinedTerrain(below) == true) {
		addWaiting(x, y+1);
	    }
	    if (Terrain.IsMinedTerrain(left) == true) {
		addWaiting(x-1, y);
	    }
	    if (Terrain.IsMinedTerrain(right) == true) {
		addWaiting(x+1, y);
	    }
	}
	gw.reCalc();
    }
    
}
