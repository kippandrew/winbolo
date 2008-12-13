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
 * TankExplosion.java
 *
 * Created on December 29, 2003, 8:58 PM
 */

package com.jbolo.bolo;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.ListIterator;
import java.io.ByteArrayOutputStream;
import com.jbolo.bolo.map.BoloMap;

/**
 * Provides the explosion trail when a tank gets killed. When it explodes it
 * updates the game worlds terrain, checks for mine chain reactions and plays sound
 *
 * @see TankExplosionItem
 * @provides bolo/tankexp.c
 * @author  John Morrison
 */
public class TankExplosion {
    
    /** A small tank explosion */
    public static final byte SMALL_EXPLOSION = 0;
    /** A big tank explosion */
    public static final byte BIG_EXPLOSION = 1;
    /** Tank Explosion length */
    public static int TK_EXPLODE_LENGTH = 40;
    /** Nunber of ticks between updating tank explosions */
    private static int UPDATE_TIME = 2;
    /** The amount a tank explosion moves */
    private static int MOVE_AMOUNT = 48;
    /** Check for colision numbers. */
    private static int WIDTH_CHECK = 24;
    /** Check for colision numbers. */
    private static int HEIGHT_CHECK = 32;
    /** The frame used when drawing on the screen */
    private static int TANK_EXPLOSION_FRAME = 8;
    /** Add or remove one map square */
    private static int MOVE_RIGHT = 1;
    /** Add or remove one map square */
    private static int MOVE_LEFT = -1;
    /** Amount of damage to do to a pillbox when a big explosion lands on it */
    private static int TANK_EXPLOSION_DAMAGE = 5;
    
    
    /** List of tank explosion items */
    private ArrayList<TankExplosionItem> items;
    
    /** The game world that this TankExplosion belongs to */
    private GameWorld gw;
    
    /** The time between updating the tank explosions */
    private int updateTime;
    
    /** Creates a new instance of TankExplosion
     * @param gw The gameWorld that this TankExplosion belongs to
     */
    public TankExplosion(GameWorld gw) {
        this.gw = gw;
        items = new ArrayList<TankExplosionItem>(16);
        updateTime = 0;
    }
    
    /**
     * Adds a new tank explosion to the list of tank explosions
     * @param x X world co-ordinate
     * @param y Y world co-ordinate
     * @param angle Angle the explosion is
     * @param length Length the tank explosion is travelling
     * @param explosionType The type of this explosion
     */
    public void add(int x, int y, float angle, int length, byte explosionType) {
        TankExplosionItem tki;
        
        tki = new TankExplosionItem(x, y, angle, length, explosionType, true, false, gw.getPlayers().getSelf());
        items.add(tki);
    }
    
    /**
     * Updates the position of each explosion each game tick
     * @param map BoloMap this tank explosion is on
     * @param pbs Pillboxes this tank explosion is on
     * @param bss Bases this tank explosion is on
     */
    public void update(BoloMap map, Pillboxes pbs, Bases bss) {
        Iterator<TankExplosionItem> li;
        TankExplosionItem tki;
        Point p;
        int newX;
        int newY;
        int testX = 0;
        int testY = 0;
        int newmx;
        int newmy;
        int mx;
        int my;
        boolean isServer;
        int playerNum;
        int currentPos;
        
        updateTime++;
        if (updateTime < UPDATE_TIME) {
            return;
        }
        
        
        updateTime = 0;
        isServer = gw.isServer();
        playerNum = gw.getPlayers().getSelf();
        li = items.iterator();
        while (li.hasNext()) {
            tki = li.next();
            mx = tki.getMX();
            my = tki.getMY();
            
            p = Utils.CalcDistance(tki.getAngle(), MOVE_AMOUNT);
            if (tki.getLength() > 0) {
                /* Add the "flame trail */
                gw.getExplosions().add(mx, my, tki.getPX(), tki.getPY(), Explosion.START);
                newX = tki.getX() + p.getX();
                newY = tki.getY() + p.getY();
                if (isServer == false) {
                    testX = Utils.GetWorldFromMap(newX);
                    testY = Utils.GetWorldFromMap(newY);
                }
                
                /* Collision Test */
                if (newX > 0) {
                    newX += WIDTH_CHECK;
                } else {
                    newX -= WIDTH_CHECK;
                }
                
                if (newY > 0) {
                    newY += HEIGHT_CHECK;
                } else {
                    newY -= HEIGHT_CHECK;
                }
                
                newmx = Utils.GetMapFromWorld(newX);
                newmy = Utils.GetMapFromWorld(newY);
                if (map.getSpeed(pbs, bss, mx, newmy, GameItem.NEUTRAL, true) > 0) {
                    tki.setY(tki.getY() + p.getY());
                    if (isServer == false && tki.getCreator() == playerNum) {
                        if (testY > my) {
                            // FIXME screenMoveViewOffsetUp(FALSE);
                        } else if (testY < my) {
                            // FIXME screenMoveViewOffsetUp(TRUE);
                        } else {
                            my = my;
                        }
                    }
                    my = newmy;
                }
                
                if (map.getSpeed(pbs, bss, newmx, my, GameItem.NEUTRAL, true) > 0) {
                    tki.setX(tki.getX() + p.getX());
                    if (isServer == false && tki.getCreator() == playerNum) {
                        if (testX > mx) {
                            // FIXME screenMoveViewOffsetLeft(FALSE);
                        } else if (testX < mx) {
                            // FIXME screenMoveViewOffsetLeft(TRUE);
                        } else {
                            mx = mx;
                        }
                    }
                    my = newmy;
                }
                /* Update the length */
                tki.decrementLength();
                currentPos = map.getPos(mx, my);
                if (currentPos == Terrain.DEEP_SEA) {
                    if (tki.getOwn() == true) {
                        gw.getSoundDist().soundDist(SoundDist.TANK_SINK_NEAR, mx, my);
                        gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "Assistant Message", "Tank sunk in deep sea");
                    }
                    li.remove();
                } else if (currentPos == Terrain.FOREST) {
                    map.setPos(mx, my, Terrain.GRASS, false, false);
                    gw.getSoundDist().soundDist(SoundDist.SHOT_TREE_NEAR, mx, my);
                } else if (currentPos == Terrain.BOAT) {
                    /* Check for destroy boat */
                    map.setPos(mx, my, Terrain.RIVER, false, false);
                    gw.getSoundDist().soundDist(SoundDist.MINE_EXPLOSION_NEAR, mx, my);
                }
                gw.reCalc();
            } else {
                if (tki.getExplodeType() == SMALL_EXPLOSION) {
                    gw.getExplosions().add(tki.getMX(), tki.getMY(), 0, 0, Explosion.START);
                    currentPos = map.getPos(mx, my);
                    if (currentPos != Terrain.RIVER && currentPos != Terrain.DEEP_SEA) {
                        map.setPos(mx, my, Terrain.CRATER, false, false);
                        gw.getFloodFill().add(mx, my);
                        gw.reCalc();
                    }
                    /* FIXME
                     count = 1;
                     while (count < numLgm) {
                     lgmDeathCheck(lgms[count-1], mp, pb, bs, position->x, position->y, NEUTRAL);
                     count++;
                     }
                     */
                    gw.getSoundDist().soundDist(SoundDist.MINE_EXPLOSION_NEAR, mx, my);
                } else {
                    bigExplosion(map, pbs, bss, tki, Utils.CalcDistance(tki.getAngle(), MOVE_AMOUNT));
                }
                li.remove();
            }
        }
    }
    
    /** Creates an ArrayList of ScreenItems of the tank explosions if they are on the screen
     * @param bottomPos Bottom position of the screen
     * @param leftPos Left position of the screen
     * @param rightPos Right position of the screen
     * @param topPos Top position of the screen
     * @return ArrayList of ScreenItems
     */
    public ArrayList<ScreenItem> getScreenItems(int leftPos, int rightPos, int topPos, int bottomPos) {
        ArrayList<ScreenItem> returnValue = new ArrayList<ScreenItem>(64); // Value to return
        ScreenItem si;

        for (TankExplosionItem tki : items) {
            if (tki.getMX() >= leftPos && tki.getMX() <= rightPos && tki.getMY() >= topPos && tki.getMY() <= bottomPos) {
                si = new ScreenItem(tki.getMX() - leftPos, tki.getMY() - topPos, tki.getPX(), tki.getPY(), TANK_EXPLOSION_FRAME);
                returnValue.add(si);
            }
        }
        return returnValue;
    }
    
    /**
     * Network TKE data has arrived. Process it
     *
     * @param data Byte Array containing data
     * @param length Length of the data array
     * @param playerNum Our player number
     */
    public void netExtract(byte data[], int length, int playerNum) {
        TankExplosionItem tki;
        int count = 0;
        int x;
        int y;
        int creator;
        float angle;
        byte expType;
        boolean isSent = true;
        boolean isServer = gw.isServer();
        
        if (isServer == true) {
            isSent = false;
        }
        
        
        
        while (count < length) {
            /* Get each Data item out */
            x = data[count]; // FIXME endians
            count++;
            x = data[count];
            count++;
            y = data[count];
            count++;
            y = data[count];
            count++;
            
            angle = data[count];
            count++;
            angle = data[count];
            count++;
            angle = data[count];
            count++;
            
            length = data[count]; /* Length */
            count++;
            expType = data[count]; /* Explode Type */
            count++;
            creator = data[count]; /* Creator */
            count++;
            if (creator != playerNum) {
                /* Add it */
                if (isServer == false) {
/* FIXME                    BYTE lgmMX;
                    BYTE lgmMY;
                    BYTE lgmPX;
                    BYTE lgmPY;
                    BYTE lgmFrame;
                    playersGetLgmDetails(screenGetPlayers(), creator, &lgmMX, &lgmMY, &lgmPX, &lgmPY, &lgmFrame);
                    playersUpdate(screenGetPlayers(), creator, 0, 0, 0, 0, 0, 0, lgmMX, lgmMY, lgmPX, lgmPY, lgmFrame);
 **/
                }
                tki = new TankExplosionItem(x, y, angle, length, expType, isServer, isSent, creator);
                items.add(tki);
            }
        }
    }
    
    /**
     *  When we have the token we inform all the players of
     *  tke we have made since last time we had the token.
     *  Returns the length of the data created
     *
     * @return byte array of data to send
     */
    public byte[] netMake() {
        ByteArrayOutputStream bos = new ByteArrayOutputStream(items.size() * 5);
        ListIterator li = items.listIterator();
        TankExplosionItem tki;
        
        while (li.hasNext()) {
            tki = (TankExplosionItem) li.next();
            if (tki.getPacketSent() == false) {
                bos.write(tki.getX()); // FIXME endian
                bos.write(tki.getY()); // FIXME endian
                //                bos.write(tki.getAngle());
                bos.write(tki.getLength());
                bos.write(tki.getExplodeType());
                bos.write(tki.getCreator());
                tki.setPackSent(true);
            }
        }
        return bos.toByteArray();
    }
    
    /**
     * An explosion has happened. Check to see if it should
     * remove from grass/building data structures etc.
     * @param terrain Terrain type position x, y
     * @param x X map position
     * @param y Y map position
     */
    private void checkRemove(int terrain, int x, int y) {
        switch (terrain) {
            case Terrain.HALFBUILDING:
                gw.getBuildings().removePos(x, y);
                break;
            case Terrain.GRASS:
                gw.getGrass().removePos(x, y);
                break;
            case Terrain.RUBBLE:
                gw.getRubble().removePos(x, y);
                break;
            case Terrain.SWAMP:
                gw.getSwamp().removePos(x, y);
                break;
        }
    }
    
    /**
     * Processes each final explosion square of a big explosion
     * @param map BoloMap this tank explosion is on
     * @param pbs Pillboxes this tank explosion is on
     * @param mx X map position the explosion is occuring on
     * @param my Y map position the explosion is occuring on
     */
    private void bigExplosionProcessSquare(BoloMap map, Pillboxes pbs, int mx, int my) {
        int currentPos;
        
        gw.getExplosions().add(mx, my, 0, 0, Explosion.START);
        currentPos = map.getPos(mx, my);
        checkRemove(currentPos, mx, my);
        if (pbs.existPos(mx, mx) == true) {
            pbs.damagePos(mx, my, TANK_EXPLOSION_DAMAGE);
        } else if (currentPos != Terrain.BOAT && currentPos != Terrain.RIVER && currentPos != Terrain.DEEP_SEA) {
            map.setPos(mx, my, Terrain.CRATER, false, false);
            gw.getFloodFill().add(mx, my);
        }
        
        //FIXME
        /*
         *  count = 1;  while (count <= numLgm) {    lgmDeathCheck(lgms[count-1], mp, pb, bs, (WORLD) (((WORLD) (mx+moveX) << M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), (WORLD) (((WORLD) (my + moveY)<< M_W_SHIFT_SIZE) +MAP_SQUARE_MIDDLE), NEUTRAL);
    count++;
  }
         */
    }
    
    /** An explosion has happened. Do all the work involved with it
     * @param tki The tank explosion item that has exploded
     * @param map BoloMap this tank explosion is on
     * @param pbs Pillboxes this tank explosion is on
     * @param bss Bases this tank explosion is on
     * #param tki The Tank Explosion that is causing this big explosion
     * @param move The direction it is moving
     */
    private void bigExplosion(BoloMap map, Pillboxes pbs, Bases bss, TankExplosionItem tki, Point move) {
        int moveX;
        int moveY;
        int mx = tki.getMX();
        int my = tki.getMY();
        int currentPos;
        
        if (move.getX() > 0) {
            moveX = MOVE_RIGHT;
        } else {
            moveX = MOVE_LEFT;
        }
        if (move.getY() > 0) {
            moveY = MOVE_RIGHT;
        } else {
            moveY = MOVE_LEFT;
        }
        
        bigExplosionProcessSquare(map, pbs, mx+moveX, my+moveY);
        bigExplosionProcessSquare(map, pbs, mx+moveX, my);
        bigExplosionProcessSquare(map, pbs, mx, my+moveY);
        bigExplosionProcessSquare(map, pbs, mx, my);
        gw.getSoundDist().soundDist(SoundDist.BIG_EXPLOSION_NEAR, mx, my);
        gw.reCalc();
    }
    
    
}