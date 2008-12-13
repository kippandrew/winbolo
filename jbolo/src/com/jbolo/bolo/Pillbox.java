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
import com.jbolo.bolo.map.BoloMap;

/** Pillbox is used to represent a specific game pillxo1
 * @author John Morrison
 */
public class Pillbox extends GameItem {
    
    /** The game item name used in sending messages. Eg Pillbox */
    public static String NAME = "pillbox";
    /** Pillbox Attack normal fire rate */
    public static int PILLBOX_ATTACK_NORMAL = 100;
    /** Pillbox cooldown rate */
    public static int PILLBOX_COOLDOWN_TIME = 32;
    /** Pillbox maximum fire rate */
    private static int PILLBOX_MAX_FIRERATE = 5;
    /** Pillbox maximum armour */
    public static int PILLBOX_MAX_ARMOUR = 15;
    /** Pillbox maximum range in world units */
    private static int PILLBOX_RANGE = 2048;
    /** Pillbox maximum fire distance in map squares */
    private static int PILLBOX_FIRE_DISTANCE = 9;
    /** Pillbox not found at position return value */
    public static int PILL_NOT_FOUND = 254;
    /** Pillbox maximum fire rate */
    public static byte PILL_MAX_HEALTH = 15;
    /** Pillbox left range of vision for watching bases for hits */
    public static int PILL_BASE_HIT_LEFT = -9;
    /** Pillbox right range of vision for watching bases for hits */
    public static int PILL_BASE_HIT_RIGHT = 9;
    /** Pillbox up range of vision for watching bases for hits */
    public static int PILL_BASE_HIT_TOP = -9;
    /** Pillbox down range of vision for watching bases for hits */
    public static int PILL_BASE_HIT_BOTTOM = 9;
    
    /** The maximum amount of time we will iterate to aim a shell to hit a
     * tank. Added to fix the rare tank run away bug so the shell will never
     * hit the tank
     */
    private static int MAX_AIM_ITERATE = 200;
    
    /** The minimum hiding distance for a tank in trees in world units */
    private static int MIN_TREEHIDE_DIST = 768;
    
    /** Reload timer - When shoots goes to zero then counts back to speed to fire again */
    private int reload;   
    /** Amount of armour this pillbox has */
    private int armour;
    /** Speed of the pillbox refire rate */
    private int speed;
    /** Is this pillbox inside a tank? */
    private boolean inTank;
    /** Has this pillbox just seen a tank? */
    private boolean justSeen;
    /** Pillbox cooldown time */
    private int coolDown;
    
    /**
     * Constructor
     * @param itemNum The number of this item 0-15
     */
    public Pillbox(byte itemNum) {
	super(itemNum);
        speed = PILLBOX_ATTACK_NORMAL;
        reload = PILLBOX_ATTACK_NORMAL;
        coolDown = 0;
        inTank = false;
        justSeen = false;
    }
    
    /** Gets the alliance number of this pillbox
     * @return int
     */
    public int getAllianceNum() {
	return owner;
    }
    
    /**
     * Gets the attacking speed of a pillbox
     * @return The pillbox attack speed
     */    
    public int getAttackSpeed() {
        return speed;
    }
    
    /** Sets up this Pillbox
     * @param x X position
     * @param y Y position
     * @param owner owner of the pillbox
     * @param armour armour of the pillbox
     * @param speed firing speed of the pillbox
     * @param inTank is this pillbox in a tank
     * @param justSeen has this pillbox just seen a tank
     */
    public void set(int x, int y, int owner, int armour, int speed, boolean inTank, boolean justSeen) {
	this.x = x;
	this.y = y;
	this.owner = checkOwner(owner);
	if (speed != PILLBOX_ATTACK_NORMAL) {
	    coolDown = PILLBOX_COOLDOWN_TIME;
	}
	this.armour = armour;
	this.speed = speed;
	this.inTank = inTank;
	this.justSeen = justSeen;
	this.reload = 0;
    }
    
    /**
     * Called every game tick to update the pillbox refiring time,
     * cooling off time and to fire at a tank if required
     * @param pbs
     * @param plrs
     * @param isServer
     * @param tankX
     * @param tankY
     * @param isAlive
     * @param tankJustFired
     * @param playerNum
     * @param tankSpeed
     * @param tankAngle
     * @param tankOnBoat
     * @param dist
     * @param mp BoloMap object
     * @param bss Bases object
     * @param shs Shells object
     */
    public void update(BoloMap mp, Pillboxes pbs, Bases bss, Shells shs, Players plrs, boolean isServer, int tankX, int tankY, boolean isAlive, boolean tankJustFired, int playerNum, int tankSpeed, float tankAngle, boolean tankOnBoat, SoundDist dist) { // FIXME , Bases bs, Tank tnk, Shells shs ){
	int diffX;
	int diffY;
	int wx;
	int wy;
	
	/* Update the reload time */
	if (reload < speed) {
	    reload++;
	}
	/* Adjust the cooldown time */
	if (coolDown > 0) {
	    coolDown--;
	    if (coolDown == 0) {
		speed++;
		if (speed < PILLBOX_ATTACK_NORMAL) {
		    coolDown = PILLBOX_COOLDOWN_TIME;
		}
	    }
	}
    
	float dir;
	
	/* Check to see if we should fire */
        //FIXME this is backwards should be client only or single player game
	if (isServer == true) {
	    if (armour > 0 && inTank == false && plrs.isAlly(owner, playerNum) == false && reload >= speed) {
		wx = Utils.GetWorldFromMap(x) + Utils.MAP_SQUARE_MIDDLE;
		wy = Utils.GetWorldFromMap(y) + Utils.MAP_SQUARE_MIDDLE;
		diffX = Utils.GetGap(tankX, wx);
		diffY = Utils.GetGap(tankY, wy);
		if (isAlive == true && (Utils.IsTankInTrees(mp, pbs, bss, tankX, tankY) == false || (diffX < MIN_TREEHIDE_DIST && diffY < MIN_TREEHIDE_DIST) || tankJustFired == true)) {
		    // Can shoot
		    if (Utils.IsItemInRange(wx, wy, tankX, tankY, PILLBOX_RANGE) == true) {
			/* Check for closer tanks */
			if (true) { // FIXME plrs.IsTankCloser(x, y, owner, Utils.GetItemRange(wx, wy, tankX, tankY)) == false
			    if (justSeen == true) {
				dir = targetTank(mp, pbs, bss, wx, wy, tankX, tankY, tankAngle, tankSpeed, tankOnBoat, playerNum);
				shs.add(wx, wy, dir, PILLBOX_FIRE_DISTANCE, GameItem.NEUTRAL, false);
				reload = 0;
				dist.soundDist(SoundDist.SHOOT_NEAR, x, y);
			    } else {
				/* They were just seen */
				justSeen = true;
				reload = 0;
			    }
			} else {
			    justSeen = false;
			}
		    }
				
		}
	    }
	}
    }
    
    /**
     * Checks if the pillbox can be hit at position x,y
     *
     * @param x X map position
     * @param y Y map position
     * @return boolean indicating if this pillbox is hit
     */
    public boolean isHit( int x, int y ){
	if (this.x == x && this.y == y && inTank == false && armour > 0) {
	    return true;
	}
	return false;
    }
    
    /**
     * Sets the amount of armour this pillbox has
     *
     * @param armour Amount to set
     */
    public void setArmour( byte armour ){
	this.armour = armour;
    }
    
    /**
     * Sets if this pillbox has just seen something
     * @param justSeen Value to set to
     */
    public void setJustSeen(boolean justSeen) {
	this.justSeen = justSeen;
    }
    
    /**
     * Sets this pillbox cool down speed
     * @param coolDown Value to set to
     */
    public void setCoolDown(int coolDown) {
	this.coolDown = coolDown;
    }
    
    /**
     * Returns the amount of armour this pillbox has
     *
     * @return Amount of armour this pillbox has
     */
    public int getArmour(){
	return armour;
    }

    /**
     * Gets if this tank has just seen us
     *
     * @return boolean
     */
    public boolean getJustSeen() {
        return justSeen;
    }
    
    /**
     * Gets the current cooldown rate of this pillbox
     *
     * @return int
     */
    public int getCoolDown() {
        return coolDown;
    }
     
    /**
     * Gets the current reload time of this pillbox
     *
     * @return int
     */
    public int getReload() {
        return reload;
    }
    
    /**
     * Gets the pillboxs current speed
     *
     * @return int
     */
    public int getSpeed() {
        return speed;
    }
    
    /**
     * Sets the pillboxes reload time
     *
     * @param value Value to setg
     */
    public void setReload(int value) {
        reload = value;
    }
    
    /**
     * Sets whether this pillbox is in a tank or not
     * @param inTank value to set
     */
    public void setInTank( boolean inTank ){
	this.inTank = inTank;
    }
    
    /**
     * Sets the owner of this pillbox
     * @param owner The new owner
     * @param migrate Are we migrating because someone left the game
     * @return Returns the old owner
     */
    public int setOwner( int owner, boolean migrate ){
	int oldOwner;
	
	oldOwner = this.owner;
	this.owner = owner;
	return oldOwner;
    }
    
    /**
     * Returns if this pillbox is dead
     * @return boolean indicating if the pill is dead or not
     */
    public boolean isDead() {
	if (armour == 0) {
	    return true;
	}
	return false;
    }
    
    /**
     * This pillbox has been hit. Decrement its armour value by 1
     * @return new armour value
     */
    public int decrementArmour() {
	return decrementArmour(1);
    }
    
    /**
     * This pillbox has been hit. Decrement its armour value by amount
     * @param amount Amount to decrement the armour by
     * @return new armour value
     */
    public int decrementArmour(int amount) {
	armour -= amount;
	if (armour < 0) {
	    armour = 0;
	}
	return armour;
    }
    
    /**
     * Makes the pillbox angrier as it has been hit or a base near it has been hit
     */
     public void makeAngry() {
	coolDown = PILLBOX_COOLDOWN_TIME;
	if (speed > PILLBOX_MAX_FIRERATE) {
	    speed /=2.0;
	    if (speed < PILLBOX_MAX_FIRERATE) {
		speed = PILLBOX_MAX_FIRERATE;
	    }
	}
    }
    
    /** Returns if this pillbox is in a tank
     * @return boolean
     */
    public boolean isInTank() {
	return inTank;
    }
    
    /**
     * Operation
     *
     * @param mp
     * @param pb
     * @param x
     * @param y
     * @param tankX
     * @param tankY
     * @param angle
     * @param speed
     * @param onBoat
     * @return double
     */
    private float targetTank(BoloMap mp, Pillboxes pbs, Bases bss, int x, int y, int tankX, int tankY, float angle, int speed, boolean onBoat, int playerNum) {
	if (speed > 0) {
	    return targetTankMove(mp, pbs, bss, x, y, tankX, tankY, angle, speed, onBoat, playerNum);
	} 
	return Utils.CalcAngle(x, y, tankX, tankY);
    }
    
    
    /**
     * Returns the angle need to fire to hit a moving tank
     *
     * @param mp
     * @param bs
     * @param x
     * @param y
     * @param tankX
     * @param tankY
     * @param angle
     * @param speed
     * @param onBoat
     * @return double
     */
    private float targetTankMove(BoloMap mp, Pillboxes pbs, Bases bss, int x, int y, int tankX, int tankY, float angle, int speed, boolean onBoat, int playerNum) {
	float returnValue = 0;
	boolean found = false;
	int count = 1;
	int shellX = x;
	int shellY = y;
	Point tankAdd;
	float estimate;
	Point shellAdd;
	int tankTestAddX;
	int tankTestAddY;
	int bmx;
	int bmy;
	int newbmx;
	int newbmy;
	
	
	/* Get tank add amounts */
	tankAdd = Utils.CalcDistance(angle, speed);
	/* Get the initial estaimate */
	estimate = Utils.CalcAngle(x, y, tankX, tankY);
	/* Calcutlate Distance */
	shellAdd = Utils.CalcDistance(estimate, Shell.SPEED);
	shellX = x + shellAdd.getX();
	shellY = y + shellAdd.getY();
	while (found == false && count < MAX_AIM_ITERATE) {
	    if (Utils.IsTankHit(tankX, tankY, angle, shellX, shellY, estimate) == true) {
		found = true;
		returnValue = estimate;
	    }
	    count++;
	    tankTestAddX = tankX + tankAdd.getX();
	    tankTestAddY = tankY + tankAdd.getY();
	    if (tankAdd.getX() >= 0) {
		tankTestAddX += Tank.TANK_SUBTRACT;
	    } else {
		tankTestAddX -= Tank.TANK_SUBTRACT;
	    }
	    
	    if (tankAdd.getY() >= 0) {
		tankTestAddY += Tank.TANK_SUBTRACT;
	    } else {
		tankTestAddY -= Tank.TANK_SUBTRACT;
	    }
	    
	    newbmx = Utils.GetMapFromWorld(tankTestAddX);
	    newbmy = Utils.GetMapFromWorld(tankTestAddY);
	    bmx = Utils.GetMapFromWorld(tankX);
	    bmy = Utils.GetMapFromWorld(tankY);
	    
	    if ((mp.getSpeed(pbs, bss, bmx, newbmy, playerNum, onBoat) > 0 && onBoat == false) || (onBoat == true && speed >= Tank.BOAT_EXIT_SPEED)) {
		tankY = tankY + tankAdd.getY();
	    }
	    if ((mp.getSpeed(pbs, bss, newbmx, bmy, playerNum, onBoat) > 0 && onBoat == false) || (onBoat == true && speed >= Tank.BOAT_EXIT_SPEED)) {
		tankX = tankX + tankAdd.getX();
	    }
	    estimate = Utils.CalcAngle(x, y, tankX, tankY);
	    shellAdd = Utils.CalcDistance(estimate, Shell.SPEED);
	    shellX = x + ((count + 5) * shellAdd.getX());
	    shellY = y + ((count + 5) * shellAdd.getY());
            returnValue = estimate;
	}
	return returnValue;
    }
    
    /**
     * Gets the Log typeo for setting the owner for this GameItem type
     * @return byte containing the log owner type
     */
    public byte getLogOwnerType() {
	return Log.log_PillSetOwner;
    }
    
    /** Returns the alliance number of this game item
     * @param gw Game World this item is in
     * @return The Alliance number, GAME_ITEM_EVIL if not found
     */
    public int getAlliance(GameWorld gw) {
	Players players = gw.getPlayers();
	if (isInTank() == true) {
	    if (getOwner() == players.getSelf()) {
		return  GAME_ITEM_INTANK_SELF;
	    } else if (players.isAlly(getOwner(), players.getSelf()) == true) {
		return GAME_ITEM_INTANK_ALLY;
	    } else {
		return GAME_ITEM_INTANK_EVIL;
	    }
	} else if (getArmour() == 0) {
	    return GAME_ITEM_DEAD;
	} else if (getOwner() == NEUTRAL) {
	    return GAME_ITEM_NEUTRAL;
	} else if (getOwner() == players.getSelf()) {
	    return GAME_ITEM_OWN_GOOD;
	} else if (players.isAlly(getOwner(), players.getSelf()) == true) {
	    return GAME_ITEM_ALLY_GOOD;
	}
	return GAME_ITEM_EVIL;
    }
    
    /**
     * Updates the front end game status window when the item changes
     *
     * @param gw - Game World this item belongs to
     * @param itemNum - Item Number to change
     */
    public void updateFrontEndStatus(GameWorld gw, int itemNum) {
	gw.getFrontEndDraw().feDrawStatusPillbox(itemNum, getAlliance(gw));
    }
    
    /**
     * Gets the name of the object 
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
        return WBN.WINBOLO_NET_EVENT_PILL_STEAL;
    }

    /**
     * Gets the WBN event type for capturing this item
     *
     * @return byte containing the WBN event type
     */
    public byte getWBNEventCapture() {
        return WBN.WINBOLO_NET_EVENT_PILL_CAPTURE;
    }    
}
