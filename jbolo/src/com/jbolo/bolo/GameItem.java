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

/**
 * GameItem is used to represent a specific game item that has map co-ordinates
 * 
 * @author John Morrison
 * @see Pillbox
 * @see Base
 * @see Start
 */
public abstract class GameItem {

	/**
	 * Owner Information used for drawing. The GameItem is owned by this player
	 */
	public static final int GAME_ITEM_OWN_GOOD = 0;
	/**
	 * Information used for drawing. The GameItem is owned by an ally of this
	 * player
	 */
	public static final int GAME_ITEM_ALLY_GOOD = 1;
	/**
	 * Owner information used for drawing. The GameItem is neutral to all
	 * players
	 */
	public static final int GAME_ITEM_NEUTRAL = 3;
	/**
	 * Owner information used for drawing. The GameItem is dead and has no
	 * armour
	 */
	public static final int GAME_ITEM_DEAD = 4;
	/**
	 * Owner information used for drawing. The GameItem is owned by an enemy of
	 * this player
	 */
	public static final int GAME_ITEM_EVIL = 5;
	/** Owner information used for drawing. The GameItem is in our tank */
	public static final int GAME_ITEM_INTANK_SELF = 6;
	/** Owner information used for drawing. The GameItem is in an ally tank */
	public static final int GAME_ITEM_INTANK_ALLY = 7;
	/** Owner information used for drawing. The GameItem is in an enemy tank */
	public static final int GAME_ITEM_INTANK_EVIL = 8;
	/** Game Item is not there. ie blank */
	public static final int GAME_ITEM_BLANK = 9;

	/** A neutral item has a value of 0xFF */
	public static final int NEUTRAL = 255;

	/** The game item name used in sending messages. Eg Pillbox */
	public static String NAME = "Game Item";

	/** The owner of this item */
	protected int owner;
	/** The x co-ordinate of this item */
	protected int x;
	/** The y co-ordinate of this item */
	protected int y;
	/** The itemNum of this item */
	protected byte itemNum;

	/**
	 * Constructor
	 * 
	 * @param itemNum
	 *            The number of this item 0-15
	 */
	public GameItem(byte itemNum) {
		this.itemNum = itemNum;
	}

	/**
	 * Returns the owner of the item
	 * 
	 * @return int
	 */
	public int getOwner() {
		return owner;
	}

	/**
	 * Checks that the owner is in the range of valid owners.
	 * 
	 * @param owner
	 *            Checks that the owner is in a valid range. If it is not, it
	 *            returns the owner NEUTRAL
	 * @return The owner within valid range.
	 */
	protected int checkOwner(int owner) {
		if (owner > (GameItems.MAX_ITEMS - 1) && owner != GameItem.NEUTRAL) {
			return NEUTRAL;
		}
		return owner;
	}

	/**
	 * Sets the owner of the item
	 * 
	 * @param owner
	 *            Owner to set
	 */
	public void setOwner(int owner) {
		this.owner = owner;
	}

	/**
	 * Returns the X position of the item
	 * 
	 * @return X position
	 */
	public int getX() {
		return x;
	}

	/**
	 * Returns the Y position of the item
	 * 
	 * @return y position
	 */
	public int getY() {
		return y;
	}

	/**
	 * Sets the item X position
	 * 
	 * @param x
	 *            X position to set
	 */
	public void setX(int x) {
		this.x = x;
	}

	/**
	 * Sets the item Y position
	 * 
	 * @param y
	 *            Y position to set
	 */
	public void setY(int y) {
		this.y = y;
	}

	/**
	 * Sets the item num
	 * 
	 * @param itemNum
	 *            Item number to set
	 * @return the old item number
	 */
	public byte setItemNum(byte itemNum) {
		byte oldItemNum;

		oldItemNum = this.itemNum;
		this.itemNum = itemNum;
		return oldItemNum;
	}

	/**
	 * Returns the item number
	 * 
	 * @return item number
	 */
	public int getItemNum() {
		return itemNum;
	}

	/**
	 * Returns if this item is at map position x,y
	 * 
	 * @param x
	 *            X position
	 * @param y
	 *            Y position
	 * @return boolean
	 */
	public boolean existPos(int x, int y) {
		if (this.x == x && this.y == y) {
			return true;
		}
		return false;
	}

	/**
	 * Returns the alliance number of this game item
	 * 
	 * @param gw
	 *            Game World this item is in
	 * @return The Alliance number, GAME_ITEM_EVIL if not found
	 */
	public int getAlliance(GameWorld gw) {
		Players players = gw.getPlayers();
		if (getArmour() == 0) {
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
	 * @param gw -
	 *            Game World this item belongs to
	 * @param itemNum -
	 *            Item Number to change
	 */
	public abstract void updateFrontEndStatus(GameWorld gw, int itemNum);

	/**
	 * Returns the armour in the game item
	 * 
	 * @return Armour
	 */
	public abstract int getArmour();

	/**
	 * Gets the Log typeo for setting the owner for this GameItem type
	 * 
	 * @return byte containing the log owner type
	 */
	public abstract byte getLogOwnerType();

	/**
	 * Gets the WBN event type for stealing this item
	 * 
	 * @return byte containing the WBN event type
	 */
	public abstract byte getWBNEventSteal();

	/**
	 * Gets the WBN event type for capturing this item
	 * 
	 * @return byte containing the WBN event type
	 */
	public abstract byte getWBNEventCapture();

	/**
	 * Gets the name of the object #return The object name
	 */
	public abstract String getName();
}
