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
import java.io.ByteArrayOutputStream;

/** The Pillboxes object
 * @author John Morrison
 */
public class Pillboxes extends GameItems {
    
    /** Size of pillboxes object */
    public static final int SIZEOF_PILLS = 145;
    
    /** Constructor
     * @param gw The game world that created this GameItems
     */
    public Pillboxes(GameWorld gw) {
        super(gw);
        items = new Pillbox[GameItems.MAX_ITEMS];
    }
    
    /** Sets the number of bases in the game
     * @param numItems number of bases to set
     * @throws Exception if range exeeds maximum
     */
    public void setNumItems(int numItems) throws Exception {
        super.setNumItems(numItems);
        byte count = 0;
        
        while (count < numItems) {
            items[count] = new Pillbox(count);
            count++;
        }
    }
    
    /**
     * Sets this pillbox in/out of a tank
     *
     * @param pillNum Pillbox to set
     * @param inTank Set if it is in tank
     */
    public void setInTank(int pillNum, boolean inTank) {
        Pillbox p;
        byte logValue = 0;
        if (pillNum >= 0 && pillNum < numItems) {
            p = (Pillbox) items[pillNum];
            p.setInTank(inTank);
            // Log it
            if (inTank == true) {
                logValue = 1;
            }
            gw.getLog().addEvent(Log.log_PillSetInTank, (byte) Utils.PutNibble((byte) pillNum, logValue), (byte) 0, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
        }
    }
    
    /** Sets up a specific pillbox
     * @param itemNum The pill to set
     * @param x X position of the pillbox
     * @param y Y position of the pillbox
     * @param owner owner of the pillbox
     * @param armour amount of armour on the pillbox
     * @param speed refiring speed of the pillbox
     * @param inTank is the pillbox in the tank
     * @param justSeen has the pillbox just seen a tank
     */
    public void set(int itemNum, int x, int y, int owner, int armour, int speed, boolean inTank, boolean justSeen) {
        Pillbox pb;
        Log l = gw.getLog();
        pb = (Pillbox) items[itemNum];
        pb.set(x, y, owner, armour, speed, inTank, justSeen);
        
        // Log it
        l.addEvent(Log.log_PillSetOwner, (byte) itemNum, (byte) owner, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
        l.addEvent(Log.log_PillSetHealth, (byte) Utils.PutNibble(itemNum, armour), (byte) 0, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
        l.addEvent(Log.log_PillSetHealth, (byte) Utils.PutNibble(itemNum, Utils.BooleanToByte(inTank)), (byte) owner, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
        l.addEvent(Log.log_PillSetInTank, (byte) itemNum, (byte) owner, (byte) x, (byte) y, (short) 0, Log.EMPTY_STRING);
    }
    
    /** Returns the screen health of a pillbox from TileNum
     * @param x x position of the pillbox
     * @param y y position of the pillbox
     * @return The pill screen health
     * @see TileNum
     */
    public int getScreenHealth(int x, int y) {
        int returnValue = TileNum.PILL_EVIL_15;
        int itemNum;
        Pillbox p;
        Players players;
        
        itemNum = getItemNumFromPos(x,y);
        
        if (itemNum != -1) {
            p = (Pillbox) items[itemNum];
            players = gw.getPlayers();
            if (players.isAlly(p.getOwner(), players.getSelf())  == false) {
                switch (p.getArmour()) {
                    case 15:
                        returnValue = TileNum.PILL_EVIL_15;
                        break;
                    case 14:
                        returnValue = TileNum.PILL_EVIL_14;
                        break;
                    case 13:
                        returnValue = TileNum.PILL_EVIL_13;
                        break;
                    case 12:
                        returnValue = TileNum.PILL_EVIL_12;
                        break;
                    case 11:
                        returnValue = TileNum.PILL_EVIL_11;
                        break;
                    case 10:
                        returnValue = TileNum.PILL_EVIL_10;
                        break;
                    case 9:
                        returnValue = TileNum.PILL_EVIL_9;
                        break;
                    case 8:
                        returnValue = TileNum.PILL_EVIL_8;
                        break;
                    case 7:
                        returnValue = TileNum.PILL_EVIL_7;
                        break;
                    case 6:
                        returnValue = TileNum.PILL_EVIL_6;
                        break;
                    case 5:
                        returnValue = TileNum.PILL_EVIL_5;
                        break;
                    case 4:
                        returnValue = TileNum.PILL_EVIL_4;
                        break;
                    case 3:
                        returnValue = TileNum.PILL_EVIL_3;
                        break;
                    case 2:
                        returnValue = TileNum.PILL_EVIL_2;
                        break;
                    case 1:
                        returnValue = TileNum.PILL_EVIL_1;
                        break;
                    case 0:
                    default:
                        returnValue = TileNum.PILL_EVIL_0;
                        break;
                }
            } else {
                switch(p.getArmour()) {
                    case 15:
                        returnValue = TileNum.PILL_GOOD_15;
                        break;
                    case 14:
                        returnValue = TileNum.PILL_GOOD_14;
                        break;
                    case 13:
                        returnValue = TileNum.PILL_GOOD_13;
                        break;
                    case 12:
                        returnValue = TileNum.PILL_GOOD_12;
                        break;
                    case 11:
                        returnValue = TileNum.PILL_GOOD_11;
                        break;
                    case 10:
                        returnValue = TileNum.PILL_GOOD_10;
                        break;
                    case 9:
                        returnValue = TileNum.PILL_GOOD_9;
                        break;
                    case 8:
                        returnValue = TileNum.PILL_GOOD_8;
                        break;
                    case 7:
                        returnValue = TileNum.PILL_GOOD_7;
                        break;
                    case 6:
                        returnValue = TileNum.PILL_GOOD_6;
                        break;
                    case 5:
                        returnValue = TileNum.PILL_GOOD_5;
                        break;
                    case 4:
                        returnValue = TileNum.PILL_GOOD_4;
                        break;
                    case 3:
                        returnValue = TileNum.PILL_GOOD_3;
                        break;
                    case 2:
                        returnValue = TileNum.PILL_GOOD_2;
                        break;
                    case 1:
                        returnValue = TileNum.PILL_GOOD_1;
                        break;
                    case 0:
                    default:
                        returnValue = TileNum.PILL_GOOD_0;
                        break;
                }
            }
        }
        return returnValue;
    }
    
    /**
     * Gets the amount of armour for a pillbox zt x,y
     *
     * @return armour amount of a pillbox or NOT_FOUND if pill not found
     * @param x X map location
     * @param y Y map location
     */
    public int getArmourPos(int x, int y) {
        int itemNum;
        Pillbox p;
        
        itemNum = getItemNumFromPos(x, y);
        if (itemNum != -1) {
            p = (Pillbox) items[itemNum];
            return p.getArmour();
        }
        return NOT_FOUND;
    }
    
    /** Returns if a pillbox is dead at this map position
     * @param x x position
     * @param y y position
     * @return If the pill is dead. true if the pill is not found
     */
    public boolean deadPos(int x, int y) {
        boolean returnValue = true;
        int itemNum;
        Pillbox p;
        
        itemNum = getItemNumFromPos(x, y);
        if (itemNum != -1) {
            p = (Pillbox) items[itemNum];
            if (p.getArmour() > 0) {
                returnValue = false;
            }
        }
        return returnValue;
    }
        
    /** checkPillView returns if a pillbox at pos is alive and allied to owner
     * @param x x position
     * @param y y position
     * @param owner Owner to check
     * @return boolean
     */
    public boolean checkPillView(int x, int y, int owner) {
        boolean returnValue = false;
        int itemNum;
        Pillbox p;
        
        itemNum = getItemNumFromPos(x, y);
        if (itemNum != -1) {
            p = (Pillbox) items[itemNum];
            if (p.getArmour() > 0 && gw.getPlayers().isAlly(p.getOwner(), owner) == true) {
                returnValue = true;
            }
        }
        return returnValue;
    }
    
    /**
     * A base has been hit. Check to see if any pillboxes are
     * in range and are allied to the base to see if they should
     * get angry
     *
     * @param x X position of base that was hit
     * @param y Y position of base that was hit
     * @param owner Owner of the base that was hit
     */
    public void baseHit(int x, int y, int owner) {
        int count;
        Pillbox p;
        Players players;
        int xDist;
        int yDist;
        
        players = gw.getPlayers();
        count = 0;
        while (count < numItems) {
            p = (Pillbox) items[count];
            xDist = p.getX() - x;
            yDist = p.getY() - y;
            if (p.isInTank() == false && p.getArmour() > 0 && xDist >= Pillbox.PILL_BASE_HIT_LEFT && xDist <= Pillbox.PILL_BASE_HIT_RIGHT && yDist >= Pillbox.PILL_BASE_HIT_TOP && yDist <= Pillbox.PILL_BASE_HIT_BOTTOM && players.isAlly(p.getOwner(), owner) == true) {
                // Is alive in range and an ally
                p.makeAngry();
            }
            count++;
        }
    }
    
    /**
     * Does damange amount of damage to a pillbox at x,y. Does not make the pillbox angry
     *
     * @return If pillbox is dead
     * @param x X position of the pillbox that got hit
     * @param y Y position of the pillbox that got hit
     * @param damage Amount of damage to do
     */
    public boolean damagePos(int x, int y, int damage) {
        return damagePos(x, y, true, false, damage);
    }
    
    /**
     * Does damage to a pillbox at x,y
     *
     * @return If pillbox is dead
     * @param x X position of the pillbox that got hit
     * @param y Y position of the pillbox that got hit
     * @param wantDamage True if we want to damage the pill
     * @param wantAngry True if we want to make the pill angry
     */
    public boolean damagePos(int x, int y, boolean wantDamage, boolean wantAngry) {
        return damagePos(x, y, wantDamage, wantAngry, 1);
    }
    
    /**
     * Does damage to a pillbox at x,y
     *
     * @return If pillbox is dead
     * @param x X position of the pillbox that got hit
     * @param y Y position of the pillbox that got hit
     * @param wantDamage True if we want to damage the pill
     * @param wantAngry True if we want to make the pill angry
     * @param damage Amount of damage to do to the pillbox
     */
    private boolean damagePos(int x, int y, boolean wantDamage, boolean wantAngry, int damage) {
        boolean returnValue = false;
        Pillbox p;
        int itemNum;
        int count = 0;
        
        while (count < numItems) {
            p = (Pillbox) items[count];
            if (p.isDead() == false && p.isInTank() == false && p.getX() == x && p.getY() == y) {
                if (wantDamage == true) {
                    p.decrementArmour(damage);
                    if (p.isDead() == true) {
                        returnValue = true;
                        gw.getFrontEndDraw().feDrawStatusPillbox(count, GameItem.GAME_ITEM_DEAD);
                        
                    }
                }
                
                gw.getLog().addEvent(Log.log_PillSetHealth, (byte) Utils.PutNibble((byte) count, (byte) p.getArmour()), (byte) 0, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
                if (wantAngry == true) {
                    p.makeAngry();
                }
                gw.reCalc();
            }
            count++;
        }
        return returnValue;
    }
    
    /**
     * Repairs a pill at the specific location
     *
     * @param x X position to fix
     * @param y Y position to fix
     */
    public void repairPos(int x, int y) {
        Pillbox p;
        int count = 0;
        
        while (count < numItems) {
            p = (Pillbox) items[count];
            if (p.getX() == x && p.getY() == y && p.isInTank() == false) {
                p.setArmour(Pillbox.PILL_MAX_HEALTH);
                if (gw.isServer() == false) {
                    p.updateFrontEndStatus(gw, count);
                }
                gw.getLog().addEvent((byte) Log.log_PillSetHealth, (byte) Utils.PutNibble((byte) count, (byte) Pillbox.PILL_MAX_HEALTH), (byte) 0, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
            }
            count++;
        }
    }
    
    /**
     * Gets the number of enemy pillboxes in a given rectangle
     *
     * @return number of pillboxes in the rectangle
     * @param left Left position
     * @param right Right posision
     * @param top Top position
     * @param bottom Bottom position
     */
    public int getNumInRect(int left, int right, int top, int bottom) {
        int returnValue = 0;
        int count = 0;
        Pillbox p;
        int self;
        Players plrs = gw.getPlayers();
        
        self = plrs.getSelf();
        while (count < numItems) {
            p = (Pillbox) items[count];
            if (p.isInTank() == false && p.isDead() == false && p.getX() >= left && p.getX() <= right && p.getY() >= top && p.getY() <= bottom && plrs.isAlly(self, p.getOwner()) == false) {
                returnValue++;
            }
            count++;
        }
        return returnValue;
    }
    
    /**
     * Gets the pillbox for pill num
     *
     * @return The pillbox for pillNum
     * @param pillNum The pill number for the pillbox to get
     */
    public Pillbox getPillbox(int pillNum) {
        if (pillNum > 0 && pillNum < numItems) {
            return (Pillbox) items[pillNum];
        }
        return null;
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
        Pillbox p;
        
        itemNum = getItemNumFromPos(xValue, yValue);
        if (itemNum != -1) {
            p = (Pillbox) items[itemNum];
            if (p.isDead() == true && p.isInTank() == false) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * Returns the pill number of a pillbox at that location
     *
     * @return the Pillbox number (1-16) or -1 if not found
     * @param x X position to check
     * @param y Y position to check
     * @param careInTank Whether we are about the in tank state
     * @param inTank The intank state to check if we care
     */
    public int getPillNum(int x, int y, boolean careInTank, boolean inTank) {
        int returnValue = -1;
        int count = 0;
        Pillbox p;
        
        while (count < numItems && returnValue == -1) {
            p = (Pillbox) items[count];
            if (p.getX() == x && p.getY() == y) {
                if (careInTank == false || (p.isInTank() == inTank)) {
                    returnValue = count;
                }
            }
            count++;
        }
        
        return returnValue;
    }
    
    /**
     * Gets the owner of a pillbox
     *
     * @return The pillbox owner or Neutral if out of range
     * @param pillNum Pillbox to get the owner for
     */
    public int getOwner(int pillNum) {
        Pillbox p;
        if (pillNum >= 0 && pillNum < numItems) {
            p = (Pillbox) items[pillNum];
            return p.getOwner();
        }
        
        return GameItem.NEUTRAL;
    }
    
    /** Determines if a GameItem exists at position x,y
     * @return true if a GameItem exists at this pos
     * @param x X position of this game item
     * @param y Y position of this game item
     */
    public boolean existPos(int x, int y) {
        Pillbox p;
        int itemNum;
        
        itemNum = getItemNumFromPos(x, y);
        if (itemNum != -1) {
            p = (Pillbox) items[itemNum];
            if (p.isInTank() == false) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * Returns whether an Item at the particular location
     * is owned by the player
     *
     * @return if the item is owned by owner or false if not found
     * @param owner Owner to check with
     * @param x X Location
     * @param y Y Location
     */
    public boolean amOwner(int owner, int x, int y) {
        int itemNum;
        Pillbox p;
        
        itemNum = getItemNumFromPos(x, y);
        if (itemNum != -1) {
            if (items[itemNum].getOwner() == owner || gw.getPlayers().isAlly(owner, items[itemNum].getOwner()) == true) {
                p = (Pillbox) items[itemNum];
                if (p.isInTank() == false) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    
    
    /**
     * isHit determins if a pillbox can be hit at position x, y
     * @param x X map position
     * @param y Y map positoin
     * @return if a pillbox can be hit
     */
    public boolean isHit(int x, int y) {
        Pillbox p;
        int count = 0;
        while (count < numItems) {
            p = (Pillbox) items[count];
            if (p.isHit(x, y) == true) {
                return true;
            }
            count++;
        }
        return false;
    }
    
    /**
     * Allows players to scroll through their pills.
     *
     * @return If a pillbox was found to look at in the requested direction
     * @param pos Point that initially holds the current view co-ordinates. Is overridden with new pill co-ordinates if found
     * @param xMove -1 for moving left, 1 for right, 0 for neither
     * @param yMove -1 for moving up, 1 for down, 0 for neither
     */
    public boolean moveView(Point pos, int xMove, int yMove) {
        boolean returnValue = false; /* Value to return */
        double nearest = 65000;   /* Nearest */
        int count = 0;       /* Looping variable */
        int myPlayerNum; /* Out player number */
        int found = 0;       /* Have we found the item */
        Players plrs = gw.getPlayers();
        double dist;
        int oldPill;
        Pillbox p;
        
        oldPill = getItemNumFromPos(pos.getX(), pos.getY());
        myPlayerNum = plrs.getSelf();
        while (count < numItems) {
            p = (Pillbox) items[count];
            if (count != oldPill && (plrs.isAlly(myPlayerNum, p.getOwner()) == true) && p.isDead() == false && p.isInTank() == false) {
                if ((yMove == 0 && (xMove < 0 && p.getX() < pos.getX()) || (xMove > 0 && p.getX() > pos.getX())) || (xMove == 0 && (yMove < 0 && p.getY() < pos.getY()) || (yMove > 0 && p.getY() > pos.getY()))) {
                    dist = Utils.GetItemRange(pos.getX(), pos.getY(), p.getX(), p.getY());
                    if (dist < nearest) {
                        nearest = dist;
                        found = count;
                        returnValue = true;
                    }
                }
            }
            count++;
        }
        
        if (returnValue == true) {
            pos.set(items[found].getX(), items[found].getY());
        }
        return returnValue;
    }
    
    /**
     ** Returns the next allied (and alive) pill exists. If so
     * then it puts its map co-ordinates into the parameters
     * passed. If a previous pill is being used the the
     * parameter 'prev' is true and mx & my are set to the last
     * pills location
     *
     * @param pos Point that initially holds the current view co-ordinates. Is overridden with new pill co-ordinates if found
     * @param prev Whether a previous pill is being passed
     */
    public boolean getNextView(Point pos, boolean prev) {
        boolean returnValue = true; /* Value to return */
        boolean done = false;        /* Finished */
        boolean okLoop = false;      /* Ok to loop */
        int playNumber;  /* My player number */
        int count = 0;       /* Counting variable */
        Players plrs = gw.getPlayers();
        Pillbox p;
        
        playNumber = plrs.getSelf();
        
        /* Find out the previous amount */
        if (prev == true) {
            count = getItemNumFromPos(pos.getX(), pos.getY());
            if (count == NOT_FOUND) {
                count = 0;
            } else {
                p = (Pillbox) items[count];
                if (p.getArmour() > 0 && p.isInTank() == false) {
                    okLoop = true;
                    count++;
                } else {
                    count = 0;
                }
            }
        }
        
        /* Find the next item */
        while (done == false && count < numItems) {
            p = (Pillbox) items[count];
            if (plrs.isAlly(playNumber, p.getOwner()) == true && p.isDead() == false &&  p.isInTank() == false) {
                done = true;
                pos.set(p.getX(), p.getY());
            }
            count++;
        }
        
        /* If not found still and we are loop do it here */
        if (done == false && okLoop == true) {
            count = 0;
            p = (Pillbox) items[count];
            while (done == false && count < numItems) {
                if (plrs.isAlly(playNumber, p.getOwner()) == true && p.isDead() == false && p.isInTank() == false) {
                    done = true;
                    pos.set(p.getX(), p.getY());
                }
                count++;
            }
        }
        
        /* If we still haven't found one then one doesn't exist at all */
        if (done == false) {
            returnValue = false;
        }
        
        return returnValue;
    }
    
    
    /**
     * We are currently in pillview with the pill at position
     * mx and my. This function checks that it is still alive
     * so we can continue viewing through it.
     *
     * @param mx Map X location
     * @param my Map Y location
     */
    public boolean checkView(int mx, int my) {
        boolean returnValue = true; /* Value to return */
        int playNumber;  /* My player number */
        int pillNum;     /* The pillbox number */
        Players plrs = gw.getPlayers();
        Pillbox p;
        
        pillNum = getItemNumFromPos(mx, my);
        playNumber = plrs.getSelf();
        
        if (pillNum == NOT_FOUND) {
            returnValue = false;
        } else {
            p = (Pillbox) items[pillNum];
            if (plrs.isAlly(playNumber, p.getOwner()) == false || p.isDead() == true || p.isInTank() == true) {
                returnValue = false;
            }
        }
        
        return returnValue;
    }
    
    public void update(BoloMap mp, Bases bss, Players plrs, Shells shs, Tank tank) {
        Pillbox p;
        int count = 0;
        
        while (count < numItems) {
            p = (Pillbox) items[count];
            p.update(mp, this, bss, shs, plrs, gw.isServerOrSinglePlayerGame() , tank.getX(), tank.getY(), !tank.isDead(), tank.getJustFired(), tank.getPlayerNum(), tank.getSpeed(), tank.getAngle(), tank.isOnBoat(), gw.getSoundDist());
            count++;
        }
    }
    
    /**
     * Sets this game item from the array of bytes received from the network
     *
     * @param data byte array to create from
     */
    public void setFromNetData(byte[] data, int offset) {
        int count = 0;
        int pos = offset;
        Pillbox p;
        int x;
        int y;
        int owner;
        int armour;
        int speed;
        boolean inTank;
        boolean justSeen;
        int reload;
        int coolDown;
        
        numItems = data[offset+SIZEOF_PILLS-1];
        try {
            setNumItems(numItems);
        } catch (Exception e){
            // Not going to happen from our checked packet
        }
        while (count < numItems) {
            items[count] = new Pillbox((byte) count);
            count++;
        }
        while (count < MAX_ITEMS) {
            items[count] = null;
        }
        
        count = 0;
        while (count < numItems) {
            p = (Pillbox) items[count];
            x = Utils.ConvertByteToInt(data[pos]);
            pos++;
            y = Utils.ConvertByteToInt(data[pos]);
            pos++;
            owner = Utils.ConvertByteToInt(data[pos]);
            pos++;
            armour = Utils.ConvertByteToInt(data[pos]);
            pos++;
            speed = Utils.ConvertByteToInt(data[pos]);
            pos++;
            inTank = Utils.ByteToBoolean(data[pos]);
            pos++;
            reload = Utils.ConvertByteToInt(data[pos]);
            pos++;
            coolDown = Utils.ConvertByteToInt(data[pos]);
            pos++;
            justSeen = Utils.ByteToBoolean(data[pos]);
            pos++;
            p.set(x, y, owner, armour, Pillbox.PILLBOX_ATTACK_NORMAL, inTank, justSeen);
            p.setCoolDown(coolDown);
            p.setReload(Pillbox.PILLBOX_ATTACK_NORMAL);
            count++;
        }
    }
    
    /**
     * Generates a network sendable data array for copying to other players
     *
     * @return byte array to send
     */
    public byte[] makeNetData() {
        ByteArrayOutputStream bos = new ByteArrayOutputStream(numItems * 8);
        int count = 0;
        Pillbox p;
        
        while (count < numItems) {
            p = (Pillbox) items[count];
            bos.write(p.getX());
            bos.write(p.getY());
            bos.write(p.getArmour());
            bos.write(p.getSpeed());
            bos.write(Utils.BooleanToByte(p.isInTank()));
            bos.write(p.getReload());
            bos.write(p.getCoolDown());
            bos.write(Utils.BooleanToByte(p.getJustSeen()));
            count++;
        }
        bos.write(numItems);
        return bos.toByteArray();
    }
    
    
    /**
     *  Changes all pills of owner owner back to neutral. Also
     *  if they are inside a tank it drops them.
     *
     * @param owner Owner to set back to neutral
     */
    public void dropSetNeutralOwner(int owner) {
        int count = 0;
        Pillbox p;
        Log l = gw.getLog();
        NetChangePNB pnb = gw.getPNB();
        
        while (count < numItems) {
            p = (Pillbox) items[count];
            if (p.getOwner() == owner) {
                p.setOwner(owner);
                pnb.add(NetChangePNB.NPNB_PILL_CAPTURE, count, GameItem.NEUTRAL, p.getX(), p.getY());
                l.addEvent(Log.log_PillSetOwner, (byte) count, (byte) GameItem.NEUTRAL, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
                if (p.isInTank() == true) {
                    p.setInTank(false);
                    l.addEvent(Log.log_PillSetInTank, (byte) Utils.PutNibble(count, 0), (byte) 0, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
                }
            }
            count++;
        }
    }
    
    /**
     * Causes all pillboxes owned by one owner to be now owned by the new owner as the old owner as left the game.
     * Pillboxes in tanks are dropped
     *
     * @param oldOwner The old owner to change
     * @param newOwner The new owner of the pillboxes
     */
    public void migrate(int oldOwner, int newOwner) {
        Pillbox p;
        int count = 0;
        boolean isServer = gw.isServer();
        Log l = gw.getLog();
        NetChangeMNT mnt = gw.getMNT();
        NetChangePNB pnb = gw.getPNB();
        
        while (count < numItems) {
            p = (Pillbox) items[count];
            if (p.getOwner() == oldOwner) {
                p.setOwner(newOwner);
                mnt.add(NetChangeMNT.NMNT_PILLMIGRATE, count, newOwner, p.getX(), p.getY());
                l.addEvent(Log.log_PillSetOwner, (byte) count, (byte) newOwner, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
                if (p.isInTank() == true && isServer == true) {
                    p.setInTank(false);
                    pnb.add(NetChangePNB.NPNB_PILL_DEAD, count, newOwner, p.getX(), p.getY());
                    l.addEvent(Log.log_PillSetPlace, (byte) count, (byte) p.getX(), (byte) p.getY(), (byte) 0, (short) 0, Log.EMPTY_STRING);
                    l.addEvent(Log.log_PillSetInTank, (byte) Utils.PutNibble(count, 0), (byte) 0, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
                }
            }
            count++;
        }
    }
    
    /**
     * Drops all pillboxes owned by owner
     *
     * @param owner Owner to drop pillboxes
     */
    public void explictDrop(int owner) {
        int count = 0;
        Pillbox p;
        Log l = gw.getLog();
        NetChangePNB pnb = gw.getPNB();
        
        while (count < numItems) {
            p = (Pillbox) items[count];
            if (p.getOwner() == owner && p.isInTank() == true) {
                p.setInTank(false);
                pnb.add(NetChangePNB.NPNB_PILL_DEAD, count, GameItem.NEUTRAL, p.getX(), p.getY());
                l.addEvent(Log.log_PillSetPlace, (byte) count, (byte) p.getX(), (byte) p.getY(), (byte) 0, (short) 0, Log.EMPTY_STRING);
                l.addEvent(Log.log_PillSetInTank, (byte) Utils.PutNibble(count, 0), (byte) 0, (byte) 0, (byte) 0, (short) 0, Log.EMPTY_STRING);
            }
            count++;
        }
    }
    
    /**
     * Returns a pillboxes attack speed
     *
     * @return attack speed of the pillbox or PILLBOX_ATTACK_NORMAL if out of range
     * @param pillNum Pillbox number to get for
     */
    public int getAttackSpeed(int pillNum) {
        Pillbox p;
        if (pillNum >= 0 && pillNum < numItems) {
            p = (Pillbox) items[pillNum];
            return p.getAttackSpeed();
        }
        return Pillbox.PILLBOX_ATTACK_NORMAL;
    }
}

