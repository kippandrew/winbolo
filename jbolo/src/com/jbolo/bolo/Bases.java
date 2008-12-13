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

import java.io.ByteArrayOutputStream;
import com.jbolo.bolo.map.BoloMap;
import com.jbolo.winbolonet.WBN;

/** The Bases object
 * @author John Morrison
 */
public class Bases extends GameItems {

    /** Size of bases object */
    public static final int SIZEOF_BASES = 260;

    
    /** Range something has to be in to be shown on the base status display
     * 7 map squares or 1792 WORLD units */
    public static final double BASE_STATUS_RANGE = 1792.0;
    
    /** Constructor
     * @param gw The game world that created this GameItems
     */
    public Bases(GameWorld gw) {
        super(gw);
        items = new Base[GameItems.MAX_ITEMS];
    }
    
    /** Sets the number of bases in the game
     * @param numItems number of bases to set
     * @throws Exception if range exeeds maximum
     */
    public void setNumItems(int numItems) throws Exception {
        super.setNumItems(numItems);
        byte count = 0;
        
        while (count < numItems) {
            items[count] = new Base(count);
            count++;
        }
        while (count < MAX_ITEMS) {
            items[count] = null;
            count++;
        }
    }
    
    /**
     * Sets the Base's owner to owner and sends a newswrite message to
     * inform players. If a base is stolen it removes all its possessions
     *
     * @returns previous owner.
     * @param pillNum Item number to set
     * @param owner New owner of the item
     * @param migrate If this is a migration network message. If it is
     * no message is required to be sent.
     */
    public int setOwner(int itemNum, int owner, boolean migrate) {
        int returnValue = GameItem.NEUTRAL; // Old owner
        Base item;
        Players players;
        String msg;
        
        if (itemNum >= 0 && itemNum < numItems) {
            item = (Base) items[itemNum];
            players = gw.getPlayers();
            returnValue = item.getOwner();
            if (returnValue == GameItem.NEUTRAL && migrate == false && owner != GameItem.NEUTRAL) {
                /* Neutral pill */
                msg = players.makeMessageName(owner) + " just captured a neutral " + item.getName();
                gw.getMessages().add(Messages.NEWSWIRE_MESSAGE, "Newswire", msg);
            } else if (owner == GameItem.NEUTRAL) {
                /* Do nothing */
            } else if (gw.getPlayers().isAlly(returnValue, owner) == false && migrate == false) {
                /* Stole pill */
                msg = players.makeMessageName(owner) + " just stole " + item.getName() + " from " + players.makeMessageName(0);
                gw.getMessages().add(Messages.NEWSWIRE_MESSAGE, "Newswire", msg);
                item.setArmour(0);
                item.setShells(0);
                item.setMines(0);
            }
            item.setOwner(owner);
            gw.getLog().addEvent(item.getLogOwnerType(), (byte) itemNum, (byte) owner, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
            
            /* Winbolo.Net stuff */
            /* FIXME: From C code: Check if in an alliance?????? */
            if (migrate == false && owner != GameItem.NEUTRAL) {
                if (returnValue == GameItem.NEUTRAL) {
                    WBN.GetInstance().addEvent(item.getWBNEventCapture(), gw.isServer(), owner, 0);
                } else {
                    WBN.GetInstance().addEvent(item.getWBNEventSteal(), gw.isServer(), owner, returnValue);
                }
            }
            if (gw.isServer() == false) {
                item.updateFrontEndStatus(gw, itemNum);
            }
        }
        
        return returnValue;
    }
    
    
    
    /** Sets up a specific base
     * @param itemNum Item number to set
     * @param x x position of the base
     * @param y y position of the base
     * @param owner Base owner
     * @param armour amount of armour in the base
     * @param shells number of shells in the base
     * @param mines number of mines in the base
     */
    public void set(int itemNum, int x, int y, int owner, int armour, int shells, int mines) {
        Base bs;
        bs = (Base) items[itemNum];
        bs.set(x, y, owner, armour, shells, mines);
        gw.getLog().addEvent(Log.log_BaseSetOwner, (byte) itemNum, (byte) bs.getOwner(), (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
        gw.getLog().addEvent(Log.log_BaseSetStock, (byte) itemNum, (byte) shells, (byte) mines, (byte) armour, (short) 0, Log.EMPTY_STRING);
    }
    
    /** Test to see if a base a x,y can be hit by player number
     * @return if playerNum can hit base at x,y
     * @param x X map position
     * @param y Y map position
     * @param hitBy Player number to test
     */
    public boolean canHit(int x, int y, int hitBy) {
        boolean returnValue = false;
        int itemNum;
        Base bs;
        
        if (hitBy != GameItem.NEUTRAL) {
            itemNum = getItemNumFromPos(x, y);
            if (itemNum != -1) {
                bs = (Base) items[itemNum];
                if (bs.getOwner() != GameItem.NEUTRAL && gw.getPlayers().isAlly(bs.getOwner(), hitBy) == false && bs.getArmour() > 0) {
                    returnValue = true;
                }
            }
        }
        return returnValue;
    }
    
    /** The server has issued a refuel command to a base
     * @param baseNum Baes number to update
     * @param addAmount Amount to add to the base
     */
    public void serverRefuel(int baseNum, int addAmount) {
        int oldArmour;
        Base b;
        if (baseNum >=0 && baseNum < numItems) {
            b = (Base) items[baseNum];
            /* Armour */
            oldArmour = b.getArmour();
            b.setArmour(oldArmour + addAmount);
            /* Update the frontend status as required */
            if (oldArmour == Base.DEAD && b.getArmour() > Base.DEAD) {
                gw.getFrontEndDraw().feDrawStatusBase(baseNum,this.getAllianceNum(baseNum));
            }
            
            /* Shells */
            b.setShells(b.getShells() + addAmount);
            /* Mines */
            b.setMines(b.getMines() + addAmount);
        }
    }
    
    /**
     * A network call has been made that some one is refueling armour from a base.
     * @param baseNum Base number its happening to
     */
    public void netGiveArmour(int baseNum) {
        Base b;
        if (baseNum > 0 && baseNum < numItems) {
            b = (Base) items[baseNum];
            if (b.getArmour() - Base.ARMOUR_GIVE >= Base.MIN_ARMOUR) {
                b.setArmour(b.getArmour() - Base.ARMOUR_GIVE);
                b.setRefuelTime(Base.REFUEL_ARMOUR);
            }
        }
    }
    
    /**
     * A network call has been made that some one is refueling shells from a base.
     * @param baseNum Base number its happening to
     */
    public void netGiveShells(int baseNum) {
        Base b;
        if (baseNum > 0 && baseNum < numItems) {
            b = (Base) items[baseNum];
            if (b.getShells() - Base.SHELLS_GIVE >= Base.MIN_SHELLS) {
                b.setShells(b.getShells() - Base.SHELLS_GIVE);
                b.setRefuelTime(Base.REFUEL_SHELLS);
            }
        }
    }
    
    /**
     * A network call has been made that some one is refueling mines from a base.
     * @param baseNum Base number its happening to
     */
    public void netGiveMines(int baseNum) {
        Base b;
        if (baseNum > 0 && baseNum < numItems) {
            b = (Base) items[baseNum];
            if (b.getMines() - Base.MINES_GIVE >= Base.MIN_MINES) {
                b.setMines(b.getMines() - Base.MINES_GIVE);
                b.setRefuelTime(Base.REFUEL_MINES);
            }
        }
    }
    
    /**
     * This base has been hit. Do some damage to it.
     *
     * @param x X position of the base
     * @param y Y position of the base
     */
    public void damagePos(int x, int y) {
        int itemNum;
        Base b;
        
        itemNum = getItemNumFromPos(x, y);
        if (itemNum != -1) {
            b = (Base) items[itemNum];
            b.setArmour(b.getArmour() - Base.DAMAGE);
            if (b.getArmour() < 0) {
                b.setArmour(0);
            }
            if (b.getArmour() == 0) {
                gw.getFrontEndDraw().feDrawStatusBase(itemNum, GameItem.GAME_ITEM_DEAD);
            }
        }
    }
    
    /**
     * Called once per game tick to update all base stocks
     */
    public void update() {
        int count;
        double numPlayers;         /* Number of players */
        double maxTime;
        Base b;
        boolean needUpdate;
        Tank tank;
        int itemNum = -1;
        
        numPlayers = gw.getPlayers().getNumPlayers();
        count = 0;
        maxTime = 800.0 / numPlayers;
        needUpdate = gw.isServerOrSinglePlayerGame();
        while (count < numItems) {
            b = (Base) items[count];
            b.update(gw, maxTime, needUpdate);
            count++;
        }
        
        /* Client check for refuelling */
        if (gw.isServer() == false) {
            tank = gw.getTank();
            if (tank.isDead() == false) {
                itemNum = getItemNumFromPos(tank.getMX(), tank.getMY());
                if (itemNum != -1) {
                    b = (Base) items[itemNum];
                    if (b.getJustStopped() == false) {
                        b.refuel(gw, tank);
                    } else {
                        b.setJustStopped(false);
                        b.setRefuelTime(Base.REFUEL_ARMOUR);
                    }
                }
            }
        }
        
        count = 0;
        while (count < numItems) {
            if (count != itemNum) {
                b = (Base) items[count];
                b.setJustStopped(true);
            }
            count++;
        }
    }
    
    /**
     * Checks to see if a game item at a certain position is capturable or not
     *
     * @return If the game item is capturable
     * @param xValue X position of the pill
     * @param yValue Y position of the pill
     */
    public boolean isCapturable(int xValue, int yValue) {
        int itemNum;
        Base b;
        
        itemNum = getItemNumFromPos(xValue, yValue);
        if (itemNum != -1) {
            b = (Base) items[itemNum];
            if (b.getOwner() == GameItem.NEUTRAL || b.getArmour() <= Base.MIN_ARMOUR_CAPTURE) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * Removes mines from under bases on the map
     *
     * @param mp BoloMap to change
     */
    public void clearMines(BoloMap mp) {
        int count = 0;
        int terrain;
        
        while (count < numItems) {
            terrain = mp.getPos(items[count].getX(), items[count].getY());
            if (terrain >= Terrain.MINE_START && terrain <= Terrain.MINE_END) {
                mp.setPos(items[count].getX(), items[count].getY(), terrain-Terrain.MINE_SUBTRACT, false, true);
            }
            count++;
        }
    }
    
    /** Updates the front end status for a base
     * @param itemNum Base number to update
     */
    public void updateFrontEndStatus(int itemNum) {
        items[itemNum].updateFrontEndStatus(gw, itemNum);
    }
    
    /**
     * Sets this game item from the array of bytes received from the network
     *
     * @param data byte array to create from
     */
    public void setFromNetData(byte[] data, int offset) {
        int count = 0;
        Base b;
        int itemNum;
        
        numItems = data[offset+SIZEOF_BASES-1];
        try {
            setNumItems(numItems);
        } catch (Exception e) {
            e.printStackTrace();
        }
        while (count < numItems) {
            items[count] = new Base((byte) count);
            count++;
        }
        
        count = offset;
        itemNum = 0;
        while (count < data.length) {
            b = (Base) items[itemNum];
            b.setX(Utils.ConvertByteToInt(data[count]));
            count++;
            b.setY(Utils.ConvertByteToInt(data[count]));
            count++;
            
            b.setOwner(Utils.ConvertByteToInt(data[count]));
            count++;
            b.setArmour(Utils.ConvertByteToInt(data[count]));
            count++;
            b.setShells(Utils.ConvertByteToInt(data[count]));
            count++;
            b.setMines(Utils.ConvertByteToInt(data[count]));
            count++;
            b.setRefuelTime(Utils.ConvertByteToInt(data[count]));
            count++;
            //FIXME endianisms Maybe use a short instead!
            count++;
            count++;
            b.setBaseTime(Utils.ConvertByteToInt(data[count]));
            count++;
            b.setJustStopped(Utils.ByteToBoolean(data[count]));
            count++;
            itemNum++;
        }
    }
    
    /**
     * Generates a network sendable data array for copying to other players
     *
     * @return byte array to send
     */
    public byte[] makeNetData() {
        ByteArrayOutputStream bos = new ByteArrayOutputStream(16 * 10);
        int count = 0;
        Base b;
        
        while (count < numItems) {
            b = (Base) items[count];
            bos.write(b.getX());
            bos.write(b.getY());
            bos.write(b.getOwner());
            bos.write(b.getArmour());
            bos.write(b.getShells());
            bos.write(b.getMines());
            bos.write(b.getRefuelTime());
            bos.write(b.getBaseTime());
            bos.write(Utils.BooleanToByte(b.getJustStopped()));
            count++;
        }
        bos.write(numItems);
        return bos.toByteArray();
    }
    
    /**
     * Changes all bases of owner owner back to neutral
     *
     * @param owner Owner to change
     */
    public void setNeutralOwner(int owner) {
        int count = 0;
        Log l = gw.getLog();
        NetChangePNB pnb = gw.getPNB();
        
        while (count < numItems) {
            if (items[count].getOwner() == owner) {
                items[count].setOwner(GameItem.NEUTRAL);
                l.addEvent(Log.log_BaseSetOwner, (byte) count, (byte) GameItem.NEUTRAL, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
                pnb.add(NetChangePNB.NPNB_BASE_CAPTURE, count, GameItem.NEUTRAL, items[count].getX(), items[count].getY());
            }
            count++;
        }
    }
    
    /**
     * Causes all bases owned by one owner to be now owned by the new
     * owner as the old owner as left the game.
     *
     * @param oldOwner The old owner to change
     * @param newOwner The new owner of the pillboxes
     */
    public void migrate(int oldOwner, int newOwner) {
        Base b;
        int count = 0;
        boolean isServer = gw.isServer();
        Log l = gw.getLog();
        NetChangeMNT mnt = gw.getMNT();
        
        while (count < numItems) {
            b = (Base) items[count];
            if (b.getOwner() == oldOwner) {
                b.setOwner(newOwner);
                mnt.add(NetChangeMNT.NMNT_BASEMIGRATE, count, newOwner, b.getX(), b.getY());
                l.addEvent(Log.log_BaseSetOwner, (byte) count, (byte) newOwner, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
            }
            count++;
        }
    }
}
