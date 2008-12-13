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
 * GameClient.java
 *
 * Created on January 2, 2004, 6:11 PM
 */

package com.jbolo.bolo;

/**
 * Interface provided by Bolo for a client.
 * 
 * @author John Morrison
 */
public interface GameClient {
	/** Maximum number of tanks in a game */
	public static int MAX_TANKS = 16;

	/** Main screen size X */
	public static int MAIN_SCREEN_SIZE_X = 15;
	/** Main Screen Size Y */
	public static int MAIN_SCREEN_SIZE_Y = 15;
	/** Back Buffer Size X */
	public static int MAIN_BACK_BUFFER_SIZE_X = 17;
	/** Back Buffer Size Y */
	public static int MAIN_BACK_BUFFER_SIZE_Y = 17;
	/** The maximum length of a player name */
	public static final int MAX_NAME_LENGTH = 32;
	/** Send a message to all players in the game message send type */
	public static final int MESSAGE_ALL_PLAYERS = 0;
	/** Send a message to all allies in the game message send type */
	public static final int MESSAGE_ALL_ALLIES = 1;
	/** Send a message to all nearby tanks in the game message send type */
	public static final int MESSAGE_ALL_NEARBY = 2;
	/**
	 * Send a message to selections on the players menu in the game message send
	 * type
	 */
	public static final int MESSAGE_SELECTIONS_PLAYERS_MENU = 3;
	/** Build tree request */
	public static final int LGM_TREE_REQUEST = 0;
	/** Build road request */
	public static final int LGM_ROAD_REQUEST = 1;
	/** Build building request */
	public static final int LGM_BUILDING_REQUEST = 2;
	/** Build pillbox request */
	public static final int LGM_PILL_REQUEST = 3;
	/** Lay mine request */
	public static final int LGM_MINE_REQUEST = 4;

	/**
	 * Gets the map name
	 * 
	 * @return The map name in this game
	 */
	public String getMapName();

	/**
	 * Returns the game type
	 * 
	 * @return the game type
	 */
	public int getGameType();

	/**
	 * Gets if the game allows computer tanks
	 * 
	 * @return if the game allows computer tanks
	 */
	public int getComputerTanksAllowed();

	/**
	 * Gets the number of players in this game
	 * 
	 * @return the number of players in this game
	 */
	public int getNumberPlayers();

	/**
	 * Gets if hidden mines are allowed in this game
	 * 
	 * @return if the game allows hidden mines
	 */
	public boolean isHiddenMinesAllowed();

	/**
	 * Gets the amount of time left in the game in game ticks
	 * 
	 * @return the amount of time left in a game or -1 for unlimitted
	 */
	public long getGameTimeLeft();

	/**
	 * Request for tank to lay mine has occured
	 */
	public void tankLayMine();

	/**
	 * Gets the number of bases in this game client
	 * 
	 * @return the number of bases in this game
	 */
	public int getNumBases();

	/**
	 * Gets the number of pillboxes in this game client
	 * 
	 * @return the number of pillboxes in this game
	 */
	public int getNumPillboxes();

	/**
	 * Gets the base status for a base in this game client
	 * 
	 * @return the base status for a base in this game client
	 * @param itemNum
	 *            to get (0-15)
	 * @see GameItem
	 */
	public int getBaseStatus(int itemNum);

	/**
	 * Gets the pillbox status for a pillbox in this game client
	 * 
	 * @return the pillbox status for a pillbox in this game client
	 * @param itemNum
	 *            to get (0-15)
	 * @see GameItem
	 */
	public int getPillboxStatus(int itemNum);

	/**
	 * Gets the number of players in this game
	 * 
	 * @return the number of players in this game client
	 */
	public int getNumPlayers();

	/**
	 * Gets the tank status for a tank in this game client
	 * 
	 * @return the tank status for a tank in this game client
	 * @param itemNum
	 *            to get (0-15)
	 * @see GameItem
	 */
	public int getTankStatus(int itemNum);

	/**
	 * Gets the number of kills this tank has had
	 * 
	 * @return Number of kills this tank has had
	 */
	public int getTankKills();

	/**
	 * Gets the number of deaths this tank has had
	 * 
	 * @return Number of deaths this tank has had
	 */
	public int getTankDeaths();

	/**
	 * Gets our tank in this game world
	 * 
	 * @return our tank in this game world
	 */
	public Tank getTank();

	/**
	 * Gets our tanks current player name
	 * 
	 * @return The tanks current name
	 */
	public String getPlayerName();

	/**
	 * Attempts to set our player name in this game
	 * 
	 * @return if the name change is allowed
	 * @param newName
	 *            Name to change to
	 */
	public boolean setPlayerName(String newName);

	/**
	 * Changes the gunsight range of our tank
	 * 
	 * @param increaseRange
	 *            If true our range is increased
	 */
	public void changeGunsightRange(boolean increaseRange);

	/**
	 * Sets whether newswire messages are shown
	 * 
	 * @param show
	 *            Are these messages shown or not
	 */
	public void showNewswireMessages(boolean show);

	/**
	 * Sets whether assistant messages are shown
	 * 
	 * @param show
	 *            Are these messages shown or not
	 */
	public void showAssistantMessages(boolean show);

	/**
	 * Sets whether brain messages are shown
	 * 
	 * @param show
	 *            Are these messages shown or not
	 */
	public void showBrainMessages(boolean show);

	/**
	 * Sets whether network status messages are shown
	 * 
	 * @param show
	 *            Are these messages shown or not
	 */
	public void showNetworkMessages(boolean show);

	/**
	 * Sets whether network debug messages are shown
	 * 
	 * @param show
	 *            Are these messages shown or not
	 */
	public void showDebugMessages(boolean show);

	/**
	 * Sets the players checked state from the menu
	 * 
	 * @param playerNum
	 *            Player number to set 2param state The state to set
	 */
	public void setPlayerCheckedState(int playerNum, boolean state);

	/**
	 * Gets the number of checked players on the players menu
	 * 
	 * @return number of checked players
	 * @param selectType
	 *            The select type to get, all players, neaby tanks
	 */
	public int getNumberCheckedPlayers(int selectType);

	/**
	 * Selects all players on the players menu
	 */
	public void selectAll();

	/**
	 * Selects no players on the players menu
	 */
	public void selectNone();

	/**
	 * Selects allies on the players menu
	 */
	public void selectAllies();

	/**
	 * Selects nearby tanks on the players menu
	 */
	public void selectNearby();

	/**
	 * Sends a message
	 * 
	 * @param messageType
	 *            The type of message to send
	 * @param message
	 *            The message to send
	 */
	public void sendMessage(int messageType, String message);

	/**
	 * Gets whether our tank has autoslowdown turned on
	 * 
	 * @return if autoslowdown is enabled on this tank
	 */
	public boolean getAutoslowState();

	/**
	 * Gets if auto show hide gunsight is turned on
	 * 
	 * @return if auto show hide gunsight is turned on
	 */
	public boolean getAutoShowHideGunsight();

	/**
	 * Sets if auto slowdown is turned
	 * 
	 * @param state
	 *            Stete to set
	 */
	public void setAutoslowState(boolean state);

	/**
	 * Sets if auto showhide is turned
	 * 
	 * @param state
	 *            Stete to set
	 */
	public void setAutoShowHideGunsight(boolean state);

	/**
	 * The mouse has been clicked indicating a build operation is requested.
	 * buildType is one of LGM_TREE_REQUEST,
	 * LGM_ROAD_REQUEST,LGM_BUILDING_REQUEST,LGM_PILL_REQUEST LGM_MINE_REQUEST;
	 * 
	 * @param buildType
	 *            The building type request
	 */
	public void buildRequest(int buildType);

	/**
	 * Sets the current cursor position in the game client in map squares from
	 * 1,1 in the visiable corner of the screen
	 * 
	 * @param x
	 *            X map position
	 * @param y
	 *            Y map position
	 */
	public void setCursorPos(int x, int y);

	/**
	 * Sets a players DNS address
	 * 
	 * @param ip
	 *            IP Address to set for
	 * @param hostname
	 *            Hostname to set it to
	 */
	public void setLocation(String ip, String hostname);

	/**
	 * Sets whether autoscrolling is enabled or not
	 * 
	 * @param state
	 *            State to set
	 */
	public void setAutoScroll(boolean state);

	/**
	 * Saves the map in its current state
	 * 
	 * @return success of saving file
	 * @param fileName
	 *            Filename to save to
	 */
	public boolean saveMap(String fileName);
}
