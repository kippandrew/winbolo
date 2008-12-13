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
 * NetChangeMNT.java
 *
 * Created on November 30, 2003, 12:38 AM
 */

package com.jbolo.bolo;

import com.jbolo.winbolonet.*;
import com.jbolo.bolo.map.BoloMap;

/**
 *
 * @author  John Morrison
 */
public class NetChangeMNT extends com.jbolo.bolo.NetChange {
    /** Pillboxes have been migrated to a different player network message */
    public static final int NMNT_PILLMIGRATE = 0;
    
    /** Man has placed mine network message */
    public static final int  NMNT_MINEPLACE = 1;
    
    /** Tank has placed mine network message */
    public static final int NMNT_MINETANKPLACE = 2;
    
    /** Mine has exploded network message */
    public static final int NMNT_MINEEXPLOSION = 3;
    
    /** Man has returned to tank network message */
    public static final int NMNT_MANRETURN = 4;
    
    /** Man has performed an action network message */
    public static final int NMNT_MANACTION = 5;
    
    /** Rubble network message */
    public static final int NMNT_RUBBLE = 6;
    
    /** Swamp network message */
    public static final int NMNT_SWAMP = 7;
    
    /** This players tank has been killed network message */
    public static final int NMNT_KILLME = 8;
    
    /** Bases have been migrated to a player network message */
    public static final int NMNT_BASEMIGRATE = 9;
    
    /** Base has some stock reloaded network message */
    public static final int NMNT_BASERELOAD = 10;
    
    /** LGM has built a building network message */
    public static final int NMNT_BUILDBUILDING = 11;
    
    /** LGM has built a boat network message */
    public static final int NMNT_BUILDBOAT = 12;
    
    /** Tank has dropped a boat network message */
    public static final int NMNT_DROPBOAT = 13;
    
    /** Tank has run over a boat network message */
    public static final int NMNT_RUNOVERBOAT = 14;
    
    /** Tank has run over a boat network message */
    public static final int NMNT_TANKHIT = 15;
    
    
    /** Creates a new instance of NetChangeMNT
     *
     * @param gw GameWorld this NetChangeMNT belongs to
     */
    public NetChangeMNT(GameWorld gw) {
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
        boolean needCalc; /* Screen needs a recalc */
        int pos;      /* Map position */
        Player p;
        BoloMap map;
        
        needCalc = false;
        
        switch (event) {
            case NMNT_PILLMIGRATE:
                gw.getPills().setOwner(itemNum, owner, true);
                break;
            case NMNT_KILLME:
                p = gw.getPlayers().getPlayer(owner);
                if (opt1 == 0xFF && opt2 == 0xFF) {
                    p.update(0, 0, 0, 0, 0, false, p.getLgmMX(), p.getLgmMY(), p.getLgmPX(), p.getLgmPY(), p.getLgmFrame());
                } else if (opt1 == gw.getPlayers().getSelf()) {
                    /* We killed a player */
                    gw.getTank().addKill();
                    p.update(0, 0, 0, 0, 0, false, p.getLgmMX(), p.getLgmMY(), p.getLgmPX(), p.getLgmPY(), p.getLgmFrame());
                    
                }
                break;
            case NMNT_MINEPLACE:
                map = gw.getBoloMap();
                pos = map.getPos(opt1, opt2);
                if (pos < Terrain.MINE_START) {
                    map.setPos(opt1, opt2, (pos + Terrain.MINE_SUBTRACT), false, false);
                }
                if (gw.getPlayers().isAlly(gw.getPlayers().getSelf(), owner) == true) {
                    needCalc = true;
                    gw.getMines().add(opt1, opt2);
                }
                /* Play the sound */
                gw.getSoundDist().soundDist(SoundDist.MAN_LAYING_MINE_NEAR, opt1, opt2);
                break;
            case NMNT_MINETANKPLACE:
                needCalc = true;
                gw.getMines().add(opt1, opt2);
                pos = gw.getBoloMap().getPos(opt1, opt2);
                if (pos < Terrain.MINE_START) {
                    gw.getBoloMap().setPos(opt1, opt2, (pos + Terrain.MINE_SUBTRACT), false, false);
                }
                if (owner != gw.getPlayers().getSelf()) {
                    /* Play the sound */
                    gw.getSoundDist().soundDist(SoundDist.MAN_LAYING_MINE_NEAR, opt1, opt2);
                }
                break;
            case NMNT_MINEEXPLOSION:
                if (owner != gw.getPlayers().getSelf()) {
                    gw.getExplosions().add(opt1, opt2, 0, 0, Explosion.START);
                    gw.getSoundDist().soundDist(SoundDist.MINE_EXPLOSION_NEAR, opt1, opt2);
                    //FIXME lgmDeathCheck(screenGetLgmFromPlayerNum(playersGetSelf()), mp, pb, bs, (WORLD) ((opt1 << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), (WORLD) ((opt2 << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), NEUTRAL);^M
                    //FIXME screenCheckTankMineDamage(opt1, opt2);^M
                    gw.getMinesExp().add(opt1, opt2);
                    /* Play the sound */
                    gw.getSoundDist().soundDist(SoundDist.MINE_EXPLOSION_NEAR, opt1, opt2);
                }
                break;
            case NMNT_MANRETURN:
                break;
            case NMNT_MANACTION:
                break;
            case NMNT_RUBBLE:
                gw.getRubble().addItem(opt1, opt2);
                break;
            case NMNT_SWAMP:
                gw.getSwamp().addItem(opt1, opt2);
                break;
            case NMNT_BASEMIGRATE:
                gw.getBases().setOwner(opt1, opt2, owner, true);
                break;
            case NMNT_BASERELOAD:
                gw.getBases().serverRefuel(itemNum, owner);
                break;
            case NMNT_BUILDBOAT:
                gw.getBoloMap().setPos(opt1, opt2, Terrain.BOAT, false, false);
                needCalc = true;
                break;
            case NMNT_BUILDBUILDING:
                gw.getBoloMap().setPos(opt1, opt2, Terrain.BUILDING, false, false);
                gw.getSoundDist().soundDist(SoundDist.MAN_BUILDING_NEAR, opt1, opt2);
                //FIXME		buildingRemovePos(opt1, opt2);
                needCalc = true;
                break;
            case NMNT_DROPBOAT:
                map = gw.getBoloMap();
                if (map.getPos(opt1, opt2) == Terrain.BOAT) {
                    map.setPos(opt1, opt2, Terrain.RIVER, false, false);
                }
                break;
            case NMNT_RUNOVERBOAT:
                if (owner != gw.getPlayers().getSelf()) {
                    gw.getExplosions().add(opt1, opt2, 0, 0, Explosion.START);
                    gw.getSoundDist().soundDist(SoundDist.MAN_BUILDING_NEAR, opt1, opt2);
                }
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
        int pos; /* Map position */
        boolean returnValue = true; /* Value to return */
        int amount;
        Tank t = null; // FIXME undow set = null to see potential errors
        switch (event) {
            case NMNT_PILLMIGRATE:
                gw.getPills().setOwner(itemNum, owner, true);
                break;
            case NMNT_KILLME:
                /* WinBolo.net Stuff */
                if (opt1 != GameItem.NEUTRAL) {
                    WBN.GetInstance().addEvent(WBN.WINBOLO_NET_EVENT_TANK_KILL, true, opt1, 0);
                }
                
/*FIXME		tk = screenGetTankFromPlayer(owner);
    tankDropPills(tk, mp, pb, bs);
    tankSetWorld(tk, 0, 0, 0, FALSE);
    tankSetOnBoat(tk, TRUE);
    tankResetHitCount(tk);
 */
                break;
            case NMNT_MINEPLACE:
            case NMNT_MINETANKPLACE:
                //FIXME tk = screenGetTankFromPlayer(owner);
                pos = gw.getBoloMap().getPos(opt1, opt2);
                amount = t.getMines();
                if (pos < Terrain.MINE_START && amount >= 1 ) {
                    amount--;
                    //FIXME                    t.setMines(amount);
                    gw.getBoloMap().setPos(opt1, opt2, (pos + Terrain.MINE_SUBTRACT), false, false);
                } else {
                    returnValue = false;
                }
                break;
            case NMNT_MINEEXPLOSION:
                gw.getMinesExp().add(opt1, opt2);
                break;
            case NMNT_MANRETURN:
                break;
            case NMNT_MANACTION:
                //    FIXME serverCoreLgmOperation(owner, opt1, opt2, itemNum);
                returnValue = false;
                break;
            case NMNT_RUBBLE:
                gw.getRubble().addItem(opt1, opt2);
                break;
            case NMNT_SWAMP:
                gw.getSwamp().addItem(opt1, opt2);
                break;
            case NMNT_BASEMIGRATE:
                gw.getBases().setOwner(opt1, opt2, owner, true);
                break;
            case NMNT_BASERELOAD:
                gw.getBases().serverRefuel(itemNum, owner);
                break;
            case NMNT_BUILDBOAT:
                break;
            case NMNT_BUILDBUILDING:
                break;
            case NMNT_DROPBOAT:
                break;
            case NMNT_RUNOVERBOAT:
                break;
            case NMNT_TANKHIT:
                /* Process a tank hit */
                //FIXME
                break;
                //                tk = serverCoreGetTankFromPlayer(owner);
                //                if (t != null) {
                //                    if (opt1 == 0xFF && opt2 == 0xFF) {
                /* Cheaters */
                //
                //                        netPlayersSetCheater(serverNetGetNetPlayers(), owner);
                //        printf("Cheat received - memory hack...\n");
                //                    } else if (opt1 < 127) {
                //                        tankAddHit(tk, opt1);
                //                    } else {
                //                        tankAddHit(tk, abs(127 - opt1));
                //                    }
                //                }
                //                break;
                
        }
        return returnValue;
    }
}
