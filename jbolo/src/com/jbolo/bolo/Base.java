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

import com.jbolo.winbolonet.WBN;

/** Base is used to represent a specific game base
 * @author John Morrison
 */
public class Base extends GameItem {
    
    /** The game item name used in sending messages. Eg Pillbox */
    public static final String NAME = "Base";
    
    /** Maximum amount of  that a base has */
    public static final byte FULL_ARMOUR = 90;
    
    /** Maximum amount of shells that a base has */
    public static final byte FULL_SHELLS = 90;
    
    /** Maximum amount of mines that a base has */
    public static final byte FULL_MINES = 90;
    
    /** Minimum armour required for a capture */
    public static final byte MIN_ARMOUR_CAPTURE = 1;
    
    /** Amount of armour a base has to be considered dead */
    public static final int DEAD = 0;
    
    /** Minimum amount of armoura base has to have to give to a tank */
    public static final byte MIN_ARMOUR = 3;
    
    /** Minimum amount of shells a base has to have to give to a tank */
    public static final byte MIN_SHELLS = 0;
    
    /** Minimum amount of mines a base has to have to give to a tank */
    public static final byte MIN_MINES = 0;
    
    /** Amount of armour a base gives a tank when refueling */
    public static final int ARMOUR_GIVE = 5;
    
    /** Amount of shells a base gives a tank when refueling */
    public static final int SHELLS_GIVE = 1;
    
    /** Amount of mines a base gives a tank when refuelling */
    public static final int MINES_GIVE = 1;
    
    /** Amount of time between refuelling each item for armour */
    public static final int REFUEL_ARMOUR = 40;
    
    /** Amount of time between refuelling each item for shells */
    public static final int REFUEL_SHELLS = 10;
    
    /** Amount of time between refuelling each item for mines  */
    public static final int REFUEL_MINES = 10;
    
    /** Amount of damage a shell does to a base */
    public static final int DAMAGE = 5;
    
    /** Amount of armour in the base */
    private int armour;
    /** Amount of shells in the base */
    private int shells;
    /** Amount of mines in the base */
    private int mines;
    
    /** Time between refuelling */
    private int refuelTime;
    
    /** Base Time after refuel */
    private int baseTime;
    
    /** If the base has just stopped refuelling */
    private boolean justStopped;
    
    /** Constructor
     *
     * @param itemNum The number of this item 0-15
     */
    public Base(byte itemNum) {
        super(itemNum);
        refuelTime = 0;
        baseTime = 0;
        justStopped = false;
        shells = 0;
        mines = 0;
        armour = 0;
        owner = NEUTRAL;
    }
    
    /** Sets up this base
     * @param x X position
     * @param y Y position
     * @param owner owner of the base
     * @param armour armour in the base
     * @param shells shells in the base
     * @param mines mines in the base
     */
    public void set(int x, int y, int owner, int armour, int shells, int mines) {
        this.x = x;
        this.y = y;
        this.owner = checkOwner(owner);
        this.armour = armour;
        this.shells = shells;
        this.mines = mines;
        refuelTime = 0;
        baseTime = 0;
        justStopped = false;
    }
    
    /** Returns the armour in the base
     * @return Armour
     */
    public int getArmour() {
        return armour;
    }
    
    /** Returns the number of shells in the base
     * @return Shells
     */
    public int getShells() {
        return shells;
    }
    
    /** Returns the number of mines in the base
     * @return Mines
     */
    public int getMines() {
        return mines;
    }
    
    /**
     * Sets the amount of armour the base has
     * Prevents over and understocking by setting illegal values to the max or min.
     *
     * @param value Value to set to
     */
    public void setArmour(int value) {
        if (value < 0) {
            value = 0;
        } else if (value >= FULL_ARMOUR) {
            value = FULL_ARMOUR;
        }
        armour = value;
    }
    
    /**
     * Sets the amount of shells the base has
     * Prevents over and understocking by setting illegal values to the max or min.
     *
     * @param value Value to set to
     */
    public void setShells(int value) {
        if (value < 0) {
            value = 0;
        } else if (value >= FULL_SHELLS) {
            value = FULL_SHELLS;
        }
        shells = value;
    }
    
    /**
     * Sets the amount of mines the base has.
     * Prevents over and understocking by setting illegal values to the max or min.
     *
     * @param value Value to set to
     */
    public void setMines(int value) {
        if (value < 0) {
            value = 0;
        } else if (value >= FULL_MINES) {
            value = FULL_MINES;
        }
        mines = value;
    }
    
    /**
     * Sets the refuel time of a base
     * @param value Value to set to
     */
    public void setRefuelTime(int value) {
        refuelTime = value;
    }
    
    /**
     * Sets the base time
     *
     * @param value Value to set
     */
    public void setBaseTime(int value) {
        baseTime = value;
    }
    
    /**
     * Sets whether this base has just stopped refuelling
     * @param value Value to set to
     */
    public void setJustStopped(boolean value) {
        justStopped = value;
    }
    
    /**
     * Gets whether this base has just stopped
     *
     * @return boolean
     */
    public boolean getJustStopped() {
        return justStopped;
    }
    
    /**
     * Gets the base refuelling time
     *
     * @param int
     */
    public int getRefuelTime() {
        return refuelTime;
    }
    
    /**
     * Gets the base time
     *
     * @return int
     */
    public int getBaseTime() {
        return baseTime;
    }
    
    /**
     * Called once a game tick to update base stocks
     *
     * @param gw The GameWorld this base is on
     * @param maxTime Maximum time between updates
     * @param needUpdate Do we need to update
     */
    public void update(GameWorld gw, double maxTime, boolean needUpdate) {
        baseTime++;
        if (baseTime >= maxTime) {
            if (needUpdate == true) {
                updateStock(gw);
            }
            baseTime = 0;
        }
        if (refuelTime > 0) {
            refuelTime--;
        }
    }
    
    /**
     * Called when a stock update is needed
     *
     * @param gw The GameWorld this base is on
     */
    private void updateStock(GameWorld gw) {
        int addAmount;
        int oldArmour;
        addAmount = 1;
        
        oldArmour = armour;
        setArmour((int) (armour + addAmount));
        if (oldArmour == DEAD && armour > DEAD) { /* FIXME: From C code - Changed to hardcoded value */
            if (gw.isServer() == false) {
                gw.getFrontEndDraw().feDrawStatusBase(itemNum, getAlliance(gw));
            }
        }
        setShells(shells + addAmount);
        setMines(mines + addAmount);
        if (addAmount > 0) {
            gw.getMNT().add(NetChangeMNT.NMNT_BASERELOAD, itemNum, addAmount, 0, 0);
        }
        gw.getLog().addEvent(Log.log_BaseSetStock, itemNum, (byte) shells, (byte) mines, (byte) armour, (short) 0, Log.EMPTY_STRING);
    }
    
    /**
     * This base is being occupied by a tank, refuel it
     *
     * @param gw GameWorld this base is on
     * @param tank Tank that is on the base
     */
    public void refuel(GameWorld gw, Tank tank) {
        if (refuelTime == 0) {
            if (gw.getPlayers().isAlly(owner, tank.getPlayerNum()) == true) {
                if (tank.getArmour() < Tank.ARMOUR_FULL && (armour - ARMOUR_GIVE) > MIN_ARMOUR) {
                    setArmour(armour - ARMOUR_GIVE);
                    tank.addArmour(ARMOUR_GIVE);
                    refuelTime = REFUEL_ARMOUR;
                    gw.getPNB().add(NetChangePNB.NPNB_BASE_REFUEL_ARMOUR, itemNum, tank.getPlayerNum(), 0, 0);
                    gw.getFrontEndDraw().feDrawStatusBaseBars(shells, mines, armour, false);
                } else if (tank.getShells() < Tank.SHELLS_FULL && (shells - SHELLS_GIVE) > MIN_SHELLS) {
                    tank.addShells(SHELLS_GIVE);
                    setShells(shells - SHELLS_GIVE);
                    refuelTime = REFUEL_SHELLS;
                    gw.getPNB().add(NetChangePNB.NPNB_BASE_REFUEL_SHELLS, itemNum, tank.getPlayerNum(), 0, 0);
                    gw.getFrontEndDraw().feDrawStatusBaseBars(shells, mines, armour, false);
                }  else if (tank.getMines() < Tank.MINES_FULL && (shells - MINES_GIVE) > MIN_MINES) {
                    tank.addMines(MINES_GIVE);
                    setMines(mines - MINES_GIVE);
                    refuelTime = REFUEL_MINES;
                    gw.getPNB().add(NetChangePNB.NPNB_BASE_REFUEL_MINES, itemNum, tank.getPlayerNum(), 0, 0);
                    gw.getFrontEndDraw().feDrawStatusBaseBars(shells, mines, armour, false);
                }
                gw.getLog().addEvent(Log.log_BaseSetStock, itemNum, (byte) shells, (byte) mines, (byte) armour, (short) 0, Log.EMPTY_STRING );
            }
        }
    }
    
    /**
     * Gets the Log type for setting the owner for this GameItem type
     * @return byte containing the log owner type
     */
    public byte getLogOwnerType() {
        return Log.log_BaseSetOwner;
    }
    
    /**
     * Updates the front end game status window when the item changes
     *
     * @param gw - Game World this item belongs to
     * @param itemNum - Item Number to change
     */
    public void updateFrontEndStatus(GameWorld gw, int itemNum) {
        gw.getFrontEndDraw().feDrawStatusBase(itemNum, getAlliance(gw));
    }
    
    /** Gets the name of the object
     * @return The object name
     */
    public String getName() {
        return NAME;
    }
    
    /**
     * Gets the WBN event type for stealing this item
     *
     * @return byte containing the WBN event type
     */
    public byte getWBNEventSteal() {
        return WBN.WINBOLO_NET_EVENT_BASE_STEAL;
    }
    
    /**
     * Gets the WBN event type for capturing this item
     *
     * @return byte containing the WBN event type
     */
    public byte getWBNEventCapture() {
        return WBN.WINBOLO_NET_EVENT_BASE_CAPTURE;
    }
}
