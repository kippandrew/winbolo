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

import com.jbolo.winbolonet.*;
import com.jbolo.bolo.map.GameItemRect;

/**
 * GameItems is used to hold a list of GameItems that have map co-ordinates
 * 
 * @author John Morrison
 * @see Pillboxes
 * @see Bases
 * @see Starts
 */
public abstract class GameItems {

	/** The maximum items of each type to exist in the game. */
	public static final int MAX_ITEMS = 16;

	/** Value that defines an item not found in getClosest() */
	public static final int NOT_FOUND = 254;

	/** Max distance for a base to be shown as a status */
	public static final double BASE_STATUS_RANGE = 1792.0;

	/** The number of items in this GameItems */
	protected int numItems;

	/**
	 * Array of the GameItems
	 * 
	 * @see GameItem
	 * @see Pillboxe
	 * @see Base
	 * @see Start
	 */
	protected GameItem[] items;

	/** The GameWorld that owns these GameItems */
	protected GameWorld gw;

	/**
	 * Constructor
	 * 
	 * @param gw
	 *            The game world this game items belong to
	 */
	public GameItems(GameWorld gw) {
		numItems = 0;
		this.gw = gw;
	}

	/**
	 * Returns the number of game items contained in this GameItems
	 * 
	 * @return Number of game items
	 */
	public int getNumItems() {
		return numItems;
	}

	/**
	 * Sets the number of game items in this GameItems
	 * 
	 * @param numItems
	 *            Number of items in this GameItems to set.
	 * @throws Exception
	 *             if game items is greater then MAX_ITEMS or less then zero.
	 */
	public void setNumItems(int numItems) throws Exception {
		int count = 0;
		if (numItems < 0 || numItems > MAX_ITEMS) {
			throw new Exception(numItems + " is out of range for item numbers");
		}
		this.numItems = numItems;
		while (count < MAX_ITEMS) {
			items[count] = null;
			count++;
		}

	}

	/**
	 * Returns a GameItemRect containing the maximum extends of the items.
	 * 
	 * @return GameItemRect containing the maximum extends of the items.
	 * @see GameItemRect
	 */
	public GameItemRect getGameItemRect() {
		int top = -1;
		int bottom = 255;
		int left = -1;
		int right = 255;
		int count = 0;
		GameItem test;

		while (count < numItems) {
			test = items[count];
			if (test.getY() > top) {
				top = test.getY();
			}
			if (test.getY() < bottom) {
				bottom = test.getY();
			}

			if (test.getX() < left) {
				left = test.getX();
			}

			if (test.getX() > right) {
				right = test.getX();
			}
			count++;
		}

		return new GameItemRect(top, bottom, left, right);
	}

	/**
	 * moveAll moves all GameItems by offset x,y
	 * 
	 * @param x
	 *            Amount to move the GameItem on the X axis
	 * @param y
	 *            Amount to move the GameItem on the Y axis
	 */
	public void moveAll(int x, int y) {
		int count = 0;
		while (count < numItems) {
			items[count].setX(items[count].getX() + x);
			items[count].setY(items[count].getY() + y);
			count++;
		}
	}

	/**
	 * Determines if a GameItem exists at position x,y
	 * 
	 * @return true if a GameItem exists at this pos
	 * @param x
	 *            X position of this game item
	 * @param y
	 *            Y position of this game item
	 */
	public boolean existPos(int x, int y) {
		if (getItemNumFromPos(x, y) == -1) {
			return false;
		}
		return true;
	}

	/**
	 * Returns the game Item of an item at position x,y
	 * 
	 * @return true if a GameItem exists at this pos, -1 if not found
	 * @param x
	 *            X position of this game item
	 * @param y
	 *            Y position of this game item
	 */
	protected int getItemNumFromPos(int x, int y) {
		int count = 0;
		while (count < numItems) {
			if (items[count].getX() == x && items[count].getY() == y) {
				return count;
			}
			count++;
		}
		return -1;

	}

	/**
	 * Returns the owner of the item is at map position x,y
	 * 
	 * @param x
	 *            X position
	 * @param y
	 *            Y position
	 * @return item owner, GameItem.NEUTRAL if not found
	 */
	public int getOwnerFromPos(int x, int y) {
		int returnValue = GameItem.NEUTRAL;
		int itemNum;

		itemNum = this.getItemNumFromPos(x, y);
		if (itemNum != -1) {
			returnValue = items[itemNum].getOwner();
		}
		return returnValue;
	}

	/**
	 * Gets the owner for a particular item number
	 * 
	 * @return the items owner or GameItem.NEUTRAL if out of bounds
	 * @param itemNum
	 *            Item to get for
	 */
	public int getOwner(int itemNum) {
		if (itemNum < MAX_ITEMS) {
			return items[itemNum].getOwner();
		}
		return GameItem.NEUTRAL;
	}

	/**
	 * getNumOwnedBy returns the number of game items owned by owner
	 * 
	 * @return the number of items owned by a player/neutral
	 * @param owner
	 *            player to get number of items owned by
	 */
	public int getNumOwnedBy(int owner) {
		int returnValue = 0;
		int count = 0;

		while (count < numItems) {
			if (items[count].getOwner() == owner) {
				returnValue++;
			}
			count++;
		}
		return returnValue;
	}

	/**
	 * Returns the number of neutral game items
	 * 
	 * @return The number of neutral game items
	 */
	public int getNumNeutral() {
		return getNumOwnedBy(GameItem.NEUTRAL);
	}

	/**
	 * Returns the alliance number of this game item
	 * 
	 * @param itemNum
	 *            item number to get
	 * @return The base Alliance number, Base.BASE_EVIL if not found
	 * @see Base
	 */
	public int getAllianceNum(int itemNum) {
		int returnValue = GameItem.GAME_ITEM_NEUTRAL;
		GameItem gi;
//		Players players = gw.getPlayers();

		if (itemNum >= 0 && itemNum < numItems) {
			gi = (GameItem) items[itemNum];
			returnValue = gi.getAlliance(gw);
		}
		return returnValue;
	}

	/**
	 * Returns the alliance number of a base
	 * 
	 * @param x
	 *            x position of the base
	 * @param y
	 *            y position of the base
	 * @return The base Alliance number, GameItem.BASE_EVIL if not found
	 */
	public int getAlliancePos(int x, int y) {
		int itemNum;
		int returnValue = GameItem.GAME_ITEM_EVIL;

		itemNum = getItemNumFromPos(x, y);
		if (itemNum != -1) {
			returnValue = getAllianceNum(itemNum);
		}
		return returnValue;
	}

	/**
	 * Sets the GameItem's owner to owner and sends a newswrite message to
	 * inform players
	 * 
	 * @returns previous owner.
	 * @param pillNum
	 *            Item number to set
	 * @param owner
	 *            New owner of the item
	 * @param migrate
	 *            If this is a migration network message. If it is no message is
	 *            required to be sent.
	 */
	public int setOwner(int itemNum, int owner, boolean migrate) {
		int returnValue = GameItem.NEUTRAL; // Old owner
		GameItem item;
		Players players;
		String msg;

		if (itemNum >= 0 && itemNum < numItems) {
			item = (GameItem) items[itemNum];
			players = gw.getPlayers();
			returnValue = item.getOwner();
			if (returnValue == GameItem.NEUTRAL && migrate == false
					&& owner != GameItem.NEUTRAL) {
				/* Neutral pill */
				msg = players.makeMessageName(owner)
						+ " just captured a neutral " + item.getName();
				gw.getMessages()
						.add(Messages.NEWSWIRE_MESSAGE, "Newswire", msg);
			} else if (owner == GameItem.NEUTRAL) {
				/* Do nothing */
			} else if (gw.getPlayers().isAlly(returnValue, owner) == false
					&& migrate == false) {
				/* Stole pill */
				msg = players.makeMessageName(owner) + " just stole "
						+ item.getName() + " from "
						+ players.makeMessageName(0);
				gw.getMessages()
						.add(Messages.NEWSWIRE_MESSAGE, "Newswire", msg);
			}
			item.setOwner(owner);
			gw.getLog().addEvent(item.getLogOwnerType(), (byte) itemNum,
					(byte) owner, (byte) 0, (byte) 0, (short) 0,
					Log.EMPTY_STRING);

			/* Winbolo.Net stuff */
			/* FIXME: From C code: Check if in an alliance?????? */
			if (migrate == false && owner != GameItem.NEUTRAL) {
				if (returnValue == GameItem.NEUTRAL) {
					WBN.GetInstance().addEvent(item.getWBNEventCapture(),
							gw.isServer(), owner, 0);
				} else {
					WBN.GetInstance().addEvent(item.getWBNEventSteal(),
							gw.isServer(), owner, returnValue);
				}
			}
			if (gw.isServer() == false) {
				item.updateFrontEndStatus(gw, itemNum);
			}
		}

		return returnValue;
	}

	/**
	 * Sets the GameItem's owner to owner and sends a newswrite message to
	 * inform players
	 * 
	 * @returns previous owner.
	 * @param x
	 *            Item X position
	 * @param y
	 *            Item Y position
	 * @param owner
	 *            New owner of the item
	 * @param migrate
	 *            If this is a migration network message. If it is no message is
	 *            required to be sent.
	 */
	public int setOwner(int x, int y, int owner, boolean migrate) {
		int returnValue = GameItem.NEUTRAL;
		int itemNum;

		itemNum = this.getItemNumFromPos(x, y);
		if (itemNum != -1) {
			returnValue = setOwner(itemNum, owner, migrate);
		}

		return returnValue;
	}

	/**
	 * Returns the base Number of the base closest and is either neutral or
	 * allied to the tank and inside the range. If no base is inside the range
	 * it returnsReturns the base Number of the base closest and is ^M either
	 * neutral or allied to the tank and inside the range. If no base is inside
	 * the range it returns
	 * 
	 * @return The closest GameItem or NOT_FOUND if not found
	 * @param x
	 *            x position to get closest for
	 * @param Y
	 *            y position to get closest for
	 */
	public int getClosest(int x, int y) {
		int returnValue = NOT_FOUND;
		double oldDistance = Bases.BASE_STATUS_RANGE;
		double distance;
		int count;
		int baseX;
		int baseY;
		int self;
		int gapX;
		int gapY;

		Players players;
		players = gw.getPlayers();
		self = players.getSelf();
		count = 0;
		while (count < numItems) {
			if (items[count].getOwner() == GameItem.NEUTRAL
					|| players.isAlly(self, items[count].getOwner()) == true) {
				baseX = Utils.GetWorldFromMap(items[count].getX());
				baseX += Utils.MAP_SQUARE_MIDDLE;
				baseY = Utils.GetWorldFromMap(items[count].getY());
				baseY += Utils.MAP_SQUARE_MIDDLE;

				gapX = Utils.GetGap(baseX, x);
				gapY = Utils.GetGap(baseY, y);

				distance = Math.sqrt(gapX * gapX + gapY * gapY);
				if (distance < oldDistance) {
					returnValue = count;
					oldDistance = distance;
				}
			}
			count++;
		}

		return returnValue;
	}

	/**
	 * Gets a game item for a itemNum.
	 * 
	 * @return GameItem of itemNum or null if out of range
	 * @param itemNum
	 *            Item number to get for
	 */
	public GameItem getItem(int itemNum) {
		if (itemNum >= 0 && itemNum < numItems) {
			return items[itemNum];
		}
		return null;
	}

	/**
	 * Returns whether an Item at the particular location is owned by the player
	 * 
	 * @return if the item is owned by owner or false if not found
	 * @param owner
	 *            Owner to check with
	 * @param x
	 *            X Location
	 * @param y
	 *            Y Location
	 */
	public boolean amOwner(int owner, int x, int y) {
		int itemNum;

		itemNum = getItemNumFromPos(x, y);
		if (itemNum != -1) {
			if (items[itemNum].getOwner() == owner
					|| gw.getPlayers().isAlly(owner, items[itemNum].getOwner()) == true) {
				return true;
			}
		}

		return false;
	}

	/**
	 * Checks to see if any GameItems are within range of mx,my
	 * 
	 * @param mx
	 *            X map position to check
	 * @param my
	 *            Y map position to check
	 * @param playerNum
	 *            Player number to check for
	 * @param distance
	 *            Distance to cehck
	 * @return false if there is a gameitem within range of distance
	 */
	public boolean checkRanges(int mx, int my, int playerNum, double distance) {
		int count = 0;
		Pillbox p;
		Players plrs = gw.getPlayers();
		int x = Utils.GetWorldFromMap(mx);
		int y = Utils.GetWorldFromMap(my);

		while (count < numItems) {
			p = (Pillbox) items[count];
			if (p.getOwner() == GameItem.NEUTRAL
					|| plrs.isAlly(playerNum, p.getOwner()) == false) {
				if (Utils.IsItemInRange(x, y, Utils.GetWorldFromMap(p.getX())
						+ Utils.MAP_SQUARE_MIDDLE, Utils.GetWorldFromMap(p
						.getY())
						+ Utils.MAP_SQUARE_MIDDLE, distance) == true) {
					return false;
				}
			}
			count++;
		}

		return true;
	}

	/**
	 * Gets if a GameItem owned by playerNum is within 10 squares of x, y
	 * 
	 * @return boolean
	 * @param playerNum
	 *            Player to check for
	 * @param x
	 *            X location
	 * @param y
	 *            Y location
	 */
	public boolean isInView(int playerNum, int x, int y) {
		int count = 0;
		Players plrs = gw.getPlayers();
		int gapX;
		int gapY;

		while (count < numItems) {
			if (plrs.isAlly(playerNum, items[count].getOwner()) == true) {
				gapX = items[count].getX() - x;
				gapY = items[count].getY() - y;
				if (gapX >= -10 && gapX <= 10 && gapY >= -10 && gapY <= 10) {
					return true;
				}
			}
			count++;
		}

		return false;
	}

	/**
	 * Checks to see if a game item at a certain position is capturable or not
	 * 
	 * @return If the game item is capturable
	 * @param xValue
	 *            X position of the pill
	 * @param yValue
	 *            Y position of the pill
	 */
	public abstract boolean isCapturable(int xValue, int yValue);

	/**
	 * Generates a network sendable data array for copying to other players
	 * 
	 * @return byte array to send
	 */
	public abstract byte[] makeNetData();

	/**
	 * Sets this game item from the array of bytes received from the network
	 * 
	 * @param data
	 *            byte array to create from
	 * @param offset
	 *            Offset into array to start
	 */
	public abstract void setFromNetData(byte data[], int offset);
}
