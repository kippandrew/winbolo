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
 * MapNet.java
 *
 * Created on November 25, 2003, 10:54 PM
 */

package com.jbolo.bolo.map;

import java.util.ArrayList;
import java.util.ListIterator;
import com.jbolo.bolo.GameWorld;
import com.jbolo.bolo.Pillboxes;
import com.jbolo.bolo.Bases;
import com.jbolo.bolo.Terrain;
import com.jbolo.bolo.Messages;
import com.jbolo.bolo.SoundDist;

/**
 * Looks after the series of map events that need to be sent to server/clients
 * @author  John Morrison
 */
public class MapNet {
    
    /** Maximum time we will wait for a server authorise before undoing */    
    private static int MAX_SERVER_WAIT = 3;
    
    /** List of items waiting to be transmitted */
    private ArrayList items;
    
    /** List ofincoming MapNetItems */
    private ArrayList itemsInc;
    
    /** BoloMap that this MapNet belongs to */
    private BoloMap map;
    
    /** GameWorld that created this MapNet */
    private GameWorld gw;
    
    /** Creates a new instance of MapNet
     * @param gw GameWorld that created this MapNet
     * @param map The bolo map that this MapNet belonds to
     */
    public MapNet(GameWorld gw, BoloMap map) {
	items = new ArrayList();
	itemsInc = new ArrayList();
	this.gw = gw;
	this.map = map;
    }
    
    /**
     * Adds a item to the mapNet structure. If an item already
     * exists at that position it repaces it with the new
     * terrain.
     *
     * @param x X Co-ordinate
     * @param y Y Co-ordinate
     * @param terrain Terrain to set
     * @param needSend sed in network. True if we should request the server make this change False indicates the server should make it
     */
    void add(int x, int y, int terrain, boolean needSend) {
	MapNetItem current; /* Current MapNetItem to work on */
	ListIterator li;
	boolean done = false; /* Have we completed what we are to do? */
	
	/* Check to see if it is in the incoming buffer */
	if (gw.isServer() == false) {
	    li = itemsInc.listIterator();
	    while (done == false && li.hasNext()) {
		current = (MapNetItem) li.next();
		if (current.getX() == x && current.getY() == y && current.getTerrain() == terrain) {
		    /* Exists */
		    map.setPosFromNet(x, y, terrain);
                    gw.getBrainMap().setPos(x, y, terrain, gw.getMines().existPos(x, y));
		    li.remove();
		    done = true;
		}
	    }
	}
	if (done == false && map.getTerrainRaw(x, y) == Terrain.RIVER && (terrain == Terrain.CRATER || terrain == Terrain.MINE_CRATER)) {
	    return;
	}
	
	if (map.getTerrainRaw(x, y) != terrain) {
	    li = items.listIterator();
	    while (done == false && li.hasNext()) {
		current = (MapNetItem) li.next();
		if (current.getX() == x && current.getY() == y) {
		    /* Found */
		    current.setTerrain(terrain);
		    current.setLength(0);
		    current.setNeedSend(needSend);
		    done = true;
		}
	    }
	    
	    /* If not found then add it */
	    if (done == false) {
		current = new MapNetItem(x, y, terrain, map.getTerrainRaw(x, y), needSend, 0);
		items.add(current);
	    }
	    map.setPosFromNet(x, y, terrain);
            gw.getBrainMap().setPos(x, y, terrain, gw.getMines().existPos(x, y));	    
	}
	
    }
    
    
    /** Updates the time the items have been waiting for the
     & server to authenticate them. If it reaches the expiry
     & date then it is restored.
     *
     * @param pbs Pillboxes on this map
     * @param bss Bases on this map
     */
    void update(Pillboxes pbs, Bases bss) {
	boolean needRedraw = false;
	ListIterator li;
	MapNetItem current;
	int oldTerrain;
	
	li = items.listIterator();
	while (li.hasNext()) {
	    current = (MapNetItem) li.next();
	    current.incrementLength();
	    if (current.getLength() > MAX_SERVER_WAIT && current.getNeedSend() == false) {
                gw.getMessages().add(Messages.NEWSWIRE_MESSAGE, Messages.EMPTY_STRING, "at");
		oldTerrain = current.getOldTerrain();
		if (Terrain.IsMinedTerrain(oldTerrain)== true) {
		    oldTerrain -= Terrain.MINE_SUBTRACT;
		}
		map.setPosFromNet(current.getX(), current.getY(), oldTerrain);
		checkWater(pbs, bss, current.getX(), current.getY());
                gw.getBrainMap().setPos(current.getX(), current.getY(), oldTerrain, gw.getMines().existPos(current.getX(), current.getY()));
		if (oldTerrain == Terrain.CRATER) {
                    gw.getFloodFill().add(current.getX(), current.getY());
		}
		needRedraw = true;
		li.remove();
	    }
	}
	
	li = itemsInc.listIterator();
	while (li.hasNext()) {
	    current = (MapNetItem) li.next();
	    current.incrementLength();
	    if (current.getLength() > MAX_SERVER_WAIT) {
                gw.getMessages().add(Messages.NEWSWIRE_MESSAGE, Messages.EMPTY_STRING, "pt");
		map.setPosFromNet(current.getX(), current.getY(), current.getTerrain());
		checkWater(pbs, bss, current.getX(), current.getY());
                gw.getBrainMap().setPos(current.getX(), current.getY(), current.getTerrain(), gw.getMines().existPos(current.getX(), current.getY()));
		needRedraw = true;
		li.remove();
	    }
	}
        
	if (needRedraw == true) {
	    gw.reCalc();
	}
	
    }
    
    /**
     * A incoming map item has come from the server. If it is
     * in the waitinf for confirmation buffer remove it, else
     * if it exists in the incoming buffer replace it with the
     * new value otherwise add it to the incoming buffer
     *
     * @param mx X Map Co-Ordinate
     * @param my Y Map Co-Ordinate
     * @param terrain Incoming terrain
     */
     void incomingItem(int mx, int my, int terrain) {
	boolean done = false; // Have we finished searching
	MapNetItem current;
	ListIterator li;
	int currentTerrain;
	
	li = items.listIterator();
	while (done == false && li.hasNext()) {
	    current = (MapNetItem) li.next();
	    if (mx == current.getX() && my == current.getY()) {
		/* We have it alredy */
		done = true;
		li.remove();
		currentTerrain = map.getTerrainRaw(mx, my);
		if (currentTerrain == Terrain.RIVER && currentTerrain == Terrain.BOAT) {
                    gw.getMines().remove(mx, my);
                    gw.getBrainMap().setPos(current.getX(), current.getY(), this.map.getTerrainRaw(current.getX(), current.getY()), false);
		}
	    }
	}
	
	/* Check for exists in the incoming buffer packet */
	li = itemsInc.listIterator();
	while (done == false && li.hasNext()) {
	    current = (MapNetItem) li.next();
	    if (mx == current.getX() && my == current.getY()) {
		done = true;
	    }
	}
	
	/* If it isn't added */
	if (done == false) {
	    currentTerrain = map.getTerrainRaw(mx, my);
	    current = new MapNetItem(mx, my, terrain, currentTerrain, false, 0);
	    itemsInc.add(current);
	}
     }
    
    /**
     * A packet has arrived. Here is a peice of map info in it.
     *
     * @param mx X Map Co-Ordinate
     * @param my Y Map Co-Ordinate
     * @param terrain Incoming terrain
     */
     public void packet(int mx, int my, int terrain) {
	boolean done = false; /* Finished Looping */
	MapNetItem current;
	ListIterator li;
	
	li = items.listIterator();
	while (done == false && li.hasNext()) {
	    current = (MapNetItem) li.next();
	    if (mx == current.getX() && my == current.getY()) {
		done = true;
		map.setPosFromNet(mx, my, terrain);
		li.remove();
                gw.getBrainMap().setPos(mx, my, terrain, gw.getMines().existPos(mx, my));
	    }
	}
	
	if (done == false) {
	    map.setPosFromNet(mx, my, terrain);
            gw.getBrainMap().setPos(mx, my, terrain, gw.getMines().existPos(mx, my));
	    if (terrain == Terrain.BUILDING || terrain == Terrain.ROAD) {
		/* Play the building sound */
		gw.getSoundDist().soundDist(SoundDist.MAN_BUILDING_NEAR, mx, my);
	    } else if (terrain == Terrain.CRATER) {
                gw.getFloodFill().add(mx, my);
	    }
	}
     }
     
     /**
      * converts an ArrayList of MapNetItems into a byte array in the format
      * of mx, my, terrain
      * 
      * @return byte array of MapNetItems
      * @param array ArrayList of MapNetItems
      * @param shouldCheck Do we need to check the MapNetItem need send and update it
      */
     private byte[] makeDataArray(ArrayList array, boolean shouldCheck) {
	 MapNetItem current;
	 ListIterator li;
	 byte bytes[] = new byte[array.size() * 3  ];
	 int count = 0;
	 

	 li = array.listIterator();
	 while (li.hasNext()) {
	     current = (MapNetItem) li.next();
	     if (shouldCheck == false || current.getNeedSend() == true) {
		 if (shouldCheck == true) {
		     current.setNeedSend(false);
		 }
		 bytes[count] = (byte) current.getX(); // FIXME
		 count++;
		 bytes[count] = (byte) current.getY();
		 count++;
		 bytes[count] = (byte) current.getTerrain();
		 count++;
	     }
	 }
	 return bytes;
     }
     
     /**
      * Make the client map part of the packet. 
      *
      * @return byte array of data
      */
     public byte[] makeClientPacket() {
	 byte[] returnValue;
	 
	 returnValue = makeDataArray(itemsInc, true);
	 return returnValue;
     }

     /**
      * Make the map part of the packet. Is destructive on data
      *
      * @return byte array of data
      */
     public byte[] makePacket() {
	 byte[] returnValue;
	 
	 returnValue = makeDataArray(items, false);
	 items.clear();
	 return returnValue;
     }

     
    /**
     * Checks an square updated through mapNetUpdate to see if it should be filled to overcome the mines problem.
     *
     * @param pbs Pillboxes object
     * @param bss Bases object
     * @param xValue X Co-ordinate
     * @param yValue Y Co-ordinate
     */
    
    private void checkWater(Pillboxes pbs, Bases bss, int xValue, int yValue) {
	int above;       /* Squares around */
	int below;
	int leftPos;
	int rightPos;
	
	if (map.getTerrainRaw(xValue, yValue) == Terrain.CRATER || map.getTerrainRaw(xValue, yValue) == Terrain.MINE_CRATER) {
	    above = getTerrainTranslate(pbs, bss, xValue, (yValue-1));
	    below = getTerrainTranslate(pbs, bss, xValue, yValue+1);
	    leftPos = getTerrainTranslate(pbs, bss, xValue-1, yValue);
	    rightPos = getTerrainTranslate(pbs, bss, xValue+1, yValue);
	
	    if (leftPos == Terrain.DEEP_SEA || leftPos == Terrain.BOAT || leftPos == Terrain.RIVER || rightPos == Terrain.DEEP_SEA || rightPos == Terrain.BOAT || rightPos == Terrain.RIVER || above == Terrain.DEEP_SEA || above == Terrain.RIVER || above == Terrain.BOAT || below == Terrain.DEEP_SEA || below == Terrain.BOAT || below == Terrain.RIVER)  {
		/* Do Fill */
		map.setPosFromNet(xValue, yValue, Terrain.RIVER);
		gw.getMines().remove(xValue, yValue);
	    }	    
	}
    }
    
    /** Translates the terrain at x,y to the terrain or if a base or pillbox
     * is at the position it returns Terrain.ROAD
     *
     * mahna mahna
     * @return terrain at x,y or Terrain.ROAD if base or pillbox present
     * @param pbs Pillboxes on this map
     * @param bss Bases on this map
     * @param x X Map position
     * @param y Y Map position
     */
    private int getTerrainTranslate(Pillboxes pbs, Bases bss, int x, int y) {
	if (bss.existPos(x, y) == true) {
	    return Terrain.ROAD;
	}
	if (pbs.existPos(x, y) == true) {
	    return Terrain.ROAD;
	} 
	
	return map.getTerrainRaw(x, y);
    }
}
