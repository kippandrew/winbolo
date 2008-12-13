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
 * NetChange.java
 *
 * Created on November 29, 2003, 10:22 PM
 */

package com.jbolo.bolo;

import java.util.ArrayList;
import java.util.ListIterator;

/** NetChange is used to queue, send and extract changes to a game world via the
 * network
 * @author John Morrison
 */
public abstract class NetChange {
    
    /** A string containing a single quote */
    private static String QUOTE_STR = "\"";
    
    /** items is a list of NetChangeItems */
    protected ArrayList items;
    
    /** GameWorld this NetChange belongs to */
    protected GameWorld gw;
    
    /** What id we are up to for server sends */
    protected int upto;
    
    /** Creates a new instance of NetChange
     *
     * @param gw GameWorld this NetChange belongs to
     */
    public NetChange(GameWorld gw) {
	this.gw = gw;
	upto = 0;
	items = new ArrayList();
    }
    
    /** Adds an item to send
     * @param event event type
     * @param itemNum Item number
     * @param owner owner of the change
     * @param opt1 Optional data 1 (Usually X position of the item)
     * @param opt2 Optional data 1 (Usually Y position of the item)
     */    
    public void add(int event, int itemNum, int owner, int opt1, int opt2) {
	NetChangeItem add;
	int item;
	int id;
	if (gw.getPlayers().getNumPlayers() > 0) { // FIXME netGetType() != netSingle &&
	    /* Check for multiplayer game */
	    item = Utils.PutNibble(event, itemNum);
	    if (gw.isServer() == true) {
		id = upto;
		upto++;
		if (upto == 250) {
		    upto = 0;
		}
	    } else {
		id = 0;
	    }
	    
	    add = new NetChangeItem(opt1, opt2, item, id, owner);
	    items.add(add);
	}
    }
    
    
    /** Makes the network data to send
     * @return byte array of data to send
     */    
    public byte[] make() {
	byte[] returnValue;
	ListIterator li = items.listIterator();
	NetChangeItem current;
	int count = 0;
	
	returnValue = new byte[items.size() * 5];
	while (li.hasNext()) {
	    current = (NetChangeItem) li.next();
	    returnValue[count] = (byte) current.getItem();
	    count++;
	    returnValue[count] = (byte) current.getOwner();
	    count++;
	    returnValue[count] = (byte) current.getId();
	    count++;
	    returnValue[count] = (byte) current.getX() ;
	    count++;
	    returnValue[count] = (byte) current.getY();
	    count++;
	}
	
	items.clear();
	return returnValue;
    }
    
    /**
     * Extracts a item as a server. Returns wether this
     * is an allowable operation (cheat checking)
     * 
     * @return If this is an allowable operation
     * @param event Event
     * @param itemNum Item number event occurred to
     * @param owner Owner of the event
     * @param opt1 Optional data 1
     * @param opt2 Optional data 2
     */
    abstract boolean extractServer(int event, int itemNum, int owner, int opt1, int opt2);
    
    /**
     * Extracts a item as a client. 
     *
     * @return if screen recalc is necessary
     * @param event Event
     * @param itemNum Item number event occurred to
     * @param owner Owner of the event
     * @param opt1 Optional data 1
     * @param opt2 Optional data 2
     */
    abstract boolean extractClient(int event, int itemNum, int owner, int opt1, int opt2);
    
    
    /**
     * Extracts the data. 
     *
     * @return if the data was sucessfully extracted
     * @param buff Bytes containing the data
     */
    public boolean extract(byte buff[]) {
	int count;        /* Looping variable */
	int event;       /* Event Type */
	int itemNum;     /* Item Number */
	int opt1;        /* Optional Data */
	int opt2;
	boolean testCalc;    /* Does the screen need a recalc? */
	boolean needCalc;    /* Does the screen need a recalc? */
	int id;          /* Item ID */
	int owner;       /* Who owns this */
	int eventNibble;
	boolean isServer;    /* Are we server */
	boolean errOccurred; /* Has an packet avert error occurred? */
	
	errOccurred = false;
	count = 0;
	needCalc = false;
	isServer = gw.isServer();
	while (count < buff.length) {
	    event = Utils.GetHighNibble(buff[count]);
	    itemNum = Utils.GetLowNibble(buff[count]);
	    eventNibble = buff[count];
	    count++;
	    owner = buff[count];
	    count++;
	    id = buff[count];
	    count++;
	    opt1 = buff[count];
	    count++;
	    opt2 = buff[count];
	    count++;
	    /* Process the occurence */
	    if (isServer == true) {
		if (extractServer(event, itemNum, owner, opt1, opt2) == true) {
		    //netMNTAdd(event, itemNum, owner, opt1, opt2);
		}
	    } else {
		testCalc = extractClient(event, itemNum, owner, opt1, opt2);
		if (needCalc == false) {
		    needCalc = testCalc;
		}
	    }
	    
	}
	/* Recalc Screen if required */
	if (needCalc == true) {
	    gw.reCalc();
	}
	
	if (errOccurred == true) {
	    gw.getMessages().add(Messages.NETWORK_MESSAGE, "", "mnt-a");
	}
	return true;
    }
    
    /**
     * Some messages need to be displayed to the screen. Do so here
     *
     * @param playerNum Player number that it relates to
     * @param msg Message to send
     */
    protected void message(int playerNum, String msg) {
	String name; // Player name this pertains to
	
	name = gw.getPlayers().makeMessageName(playerNum);
	msg = QUOTE_STR + name + QUOTE_STR + msg;
	gw.getMessages().add(Messages.NEWSWIRE_MESSAGE, "Newswire", msg);
    }
}

