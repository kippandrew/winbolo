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
 * NetChangePNB.java
 *
 * Created on November 30, 2003, 12:34 AM
 */

package com.jbolo.bolo;

/**
 *
 * @author  John Morrison
 */
public class NetChangePNB extends com.jbolo.bolo.NetChange {
    
    
    
    /** A base has been captured network message */
    public static final int NPNB_BASE_CAPTURE = 0;
    
    /** A base has refuelled a players armour network message */
    public static final int NPNB_BASE_REFUEL_ARMOUR = 1;
    
    /** A base has refuelled a players shells network message */
    public static final int NPNB_BASE_REFUEL_SHELLS = 2;
    
    /** A base has refuelled a players mines network message */
    public static final int NPNB_BASE_REFUEL_MINES = 3;
    
    /** A base has been hit network message */
    public static final int NPNB_BASE_HIT = 4;
    
    /** A pill has been captured network message */
    public static final int NPNB_PILL_CAPTURE = 5;
    
    /** A pillbox has been hit network message */
    public static final int NPNB_PILL_HIT = 6;
    
    /** A pillbox has been picked up network message */
    public static final int NPNB_PILL_PICKUP = 7;
    
    /** A pillbox has been dropped network message */
    public static final int NPNB_PILL_DROP = 8;
    
    /** A pill has been shot network message */
    public static final int NPNB_PILL_SHOOT = 9;
    
    /** A pill has been repaired network message */
    public static final int NPNB_PILL_REPAIR = 10;
    
    /** A pill is dead network message */
    public static final int NPNB_PILL_DEAD = 11;
    
    /** network message */
    public static final int NPNB_LGM_DEAD = 12;
    
    /** network message */
    public static final int NPNB_LGM_FARMTREE = 13;
    
    /** network message */
    public static final int NPNB_LGM_BUILDROAD = 14;
    
    /** network message */
    public static final int NPNB_SAVEMAP = 15;
    
    
    
    /** Creates a new instance of NetChangePNB
     *
     * @pAram gw The GameWorld this NetChangePNB belongs to
     */
    public NetChangePNB(GameWorld gw) {
	super(gw);
    }
    
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
    boolean extractClient(int event, int itemNum, int owner, int opt1, int opt2) {
	boolean needCalc; /* Needs a recalc */
	int playerNum;  /* Our player Number */
	Tank tank;
	Pillbox p;
	
	playerNum = gw.getPlayers().getSelf();
	needCalc = false;
	
	switch (event) {
	    case NPNB_BASE_CAPTURE:
		/* Base capture */
		gw.getBases().setOwner(opt1, opt2, owner, false);
                gw.getFrontEndDraw().feDrawStatusBase(itemNum, gw.getBases().getAllianceNum(itemNum));
		needCalc = true;
		break;
	    case NPNB_BASE_REFUEL_ARMOUR:
		if (playerNum != owner) {
		    gw.getBases().netGiveArmour(itemNum);
		}
		break;
	    case NPNB_BASE_REFUEL_SHELLS:
		if (playerNum != owner) {
		    gw.getBases().netGiveShells(itemNum);
		}
		break;
	    case NPNB_BASE_REFUEL_MINES:
		if (playerNum != owner) {
		    gw.getBases().netGiveMines(itemNum);
		}
		break;
	    case NPNB_BASE_HIT:
		gw.getBases().damagePos(opt1, opt2);
		if (owner != playerNum) {
		    gw.getPills().baseHit(opt1, opt2, gw.getBases().getOwnerFromPos(opt1, opt2));
		    /* Play sound */
		    gw.getSoundDist().soundDist(SoundDist.SHOT_BUILDING_NEAR, opt1, opt2);
		}
		break;
	    case NPNB_PILL_HIT:
		if (owner != playerNum) {
		    gw.getPills().damagePos(opt1, opt2, true, false);
		} else {
		    gw.getPills().damagePos(opt1, opt2, true, true);
		}
		/* Play sound */
		needCalc = true;
		gw.getSoundDist().soundDist(SoundDist.SHOT_BUILDING_NEAR, opt1, opt2);
		break;
	    case NPNB_PILL_CAPTURE:
		tank = gw.getTank();
		if (owner != playerNum) {
		    gw.getPills().setOwner(itemNum, owner, false);
		    tank.stopCarryingPill(itemNum);
		} else {
		    /* Check we aren't dead */
		    if (tank.isDead() == true) {
			p = gw.getPills().getPillbox(itemNum);
			add(NPNB_PILL_DEAD, itemNum, owner, p.getX(), p.getY());
			tank.stopCarryingPill(itemNum);
		    } else {
			gw.getPills().setOwner(itemNum, owner, false);
		    }
		}
		needCalc = true;
		break;
	    case NPNB_PILL_PICKUP:
		if (owner == playerNum) {
		    /* Check we aren't dead */
		    tank = gw.getTank();
		    if (tank.isDead() == true) {
			p = gw.getPills().getPillbox(itemNum);
			add(NPNB_PILL_DEAD, itemNum, owner, p.getX(), p.getY());
			tank.stopCarryingPill(itemNum);
		    } else {
			tank.putPill(itemNum);
			tank.putPill(itemNum);
			gw.getPills().setInTank(itemNum, true);
                        gw.getFrontEndDraw().feDrawStatusPillbox(itemNum, gw.getPills().getAllianceNum(itemNum));
		    }
		} else {
		    gw.getPills().setInTank(itemNum, true);
                    gw.getFrontEndDraw().feDrawStatusPillbox(itemNum, gw.getPills().getAllianceNum(itemNum));
		}
		needCalc = true;
		break;
	    case NPNB_PILL_DROP:
		p = gw.getPills().getPillbox(itemNum);
		p.setX(opt1);
		p.setY(opt2);
		p.setOwner(owner);
		p.setArmour((byte) Pillbox.PILLBOX_MAX_ARMOUR);
		p.setCoolDown(0);
		p.setInTank(false);
		p.setJustSeen(false);
                gw.getFrontEndDraw().feDrawStatusPillbox(itemNum, gw.getPills().getAllianceNum(itemNum));
		gw.getSoundDist().soundDist(SoundDist.MAN_BUILDING_NEAR, opt1, opt2);
		needCalc = true;
		break;		
	    case NPNB_PILL_DEAD:
		p = gw.getPills().getPillbox(itemNum);
		p.setX(opt1);
		p.setY(opt2);
		p.setOwner(owner);
		p.setArmour((byte) 0);
		p.setCoolDown(0);
		p.setInTank(false);
                gw.getFrontEndDraw().feDrawStatusPillbox(itemNum, gw.getPills().getAllianceNum(itemNum));
		needCalc = true;
		break;
	    case NPNB_PILL_REPAIR:
		/* Pill repair */
		gw.getPills().repairPos(opt1, opt2);
		gw.getSoundDist().soundDist(SoundDist.MAN_BUILDING_NEAR, opt1, opt2);
                gw.getFrontEndDraw().feDrawStatusPillbox(itemNum, gw.getPills().getAllianceNum(itemNum));
		needCalc = true;
		break;
	    case NPNB_LGM_DEAD:
		/* LGM Died */
		if (itemNum == 0) {
		    this.message(owner, " just lost his builder");
		    gw.getSoundDist().soundDist(SoundDist.MAN_DYING_NEAR, opt1, opt2);
		    
		    if (owner == playerNum) {
			//FIXME lgmSetIsDead(screenGetLgmFromPlayerNum(playerNum), TRUE);^M
		    }
		} else if (itemNum == 1 && owner == playerNum) {
		    //FIXME lgmSetIsDead(screenGetLgmFromPlayerNum(playerNum), false);
		}
		break;
	    case NPNB_LGM_FARMTREE:
		/* LGM Farmed a tree */
		if (gw.getBoloMap().getPos(opt1, opt2) == Terrain.FOREST) {
		    gw.getBoloMap().setPos(opt1, opt2, Terrain.GRASS, false, false);
		}
		needCalc = true;
		gw.getSoundDist().soundDist(SoundDist.FARMING_TREE_NEAR, opt1, opt2);
		break;
	    case NPNB_LGM_BUILDROAD:
		/* LGM Built a road */
		gw.getBoloMap().setPos(opt1, opt2, Terrain.ROAD, false, false);
		gw.getSoundDist().soundDist(SoundDist.MAN_BUILDING_NEAR, opt1, opt2);
		needCalc = true;
		break;
	    case NPNB_SAVEMAP:
		message(owner, " just saved map file");
                gw.getLog().addEvent(Log.log_SaveMap, (byte) owner, (byte) 0, (byte) 0, (byte) 0, (byte) 0, Log.EMPTY_STRING);
		break;
		
	}
	return needCalc;
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
    boolean extractServer(int event, int itemNum, int owner, int opt1, int opt2) {
	//FIXME
	return false;
    }
    
}
