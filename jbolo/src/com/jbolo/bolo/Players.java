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
 * Players.java
 *
 * Created on October 9, 2003, 12:08 AM
 */

package com.jbolo.bolo;

import java.util.ArrayList;
import com.jbolo.gui.FrontEndDraw;
import com.jbolo.bolo.network.Network;

/**
 * 
 * @author John Morrison
 */
public class Players {
	/** Maximum number of players in a game */
	public static int MAX_PLAYERS = 16;

	/** The player name when there is no tank */
	private static String NO_TANK = "???";

	private static String THIS_COMPUTER = "This Computer";

	/**
	 * Tanks must be within 9 squares to be in the selected in the 'select
	 * nearby' option
	 */
	private static int PLAYER_MAX_SELECT_DISTANCE = 9;

	/** Minimum distance a tank can hide in trees in world units (3 map squares) */
	private static int MIN_TREEHIDE_DIST = 768;

	/** Array of Player objects representing the players in the game */
	private Player[] plrs;

	/** Player number of this player in the game */
	private int myPlayerNum;

	/** The game world that created this Players */
	private GameWorld gw;

	/**
	 * Creates a new instance of Players
	 * 
	 * @param gw
	 *            The game world that created this Players
	 */
	public Players(GameWorld gw) {
		int count = 0;
		plrs = new Player[MAX_PLAYERS];
		while (count < MAX_PLAYERS) {
			plrs[count] = null;
			count++;
		}
		this.gw = gw;
	}

	/**
	 * returns the number of players in the game
	 * 
	 * @return number of players in the game
	 */
	public int getNumPlayers() {
		int count = 0;
		int total = 0;

		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				total++;
			}
			count++;
		}
		return total;
	}

	/**
	 * Gets the number of players that are allied to us
	 * 
	 * @return The number of players that are allied to us Atleast 1 as it
	 *         includes ourself
	 * @param playerNum
	 *            player number to get number of players
	 */
	public int getNumAllies(int playerNum) {
		int returnValue = 0; // Value to return

		if (plrs[playerNum] != null) {
			returnValue = plrs[playerNum].getNumAllies() + 1;
		}
		return returnValue;
	}

	/**
	 * Makes the net alliances for this player
	 * 
	 * @return byte array containing each alliance, array length contains number
	 *         of allies
	 * @param playerNum
	 *            Player to make alliances for
	 */
	public byte[] makeNetAlliances(int playerNum) {
		return plrs[playerNum].makeNetAllies();
	}

	/**
	 * Gets the number of tanks nearby our tank.
	 * 
	 * @return The number of tanks near our tank (including ourself)
	 * @param mx
	 *            The Map X location of our tank
	 * @param my
	 *            The Map Y location of our tank
	 */
	public int getNumNearby(int mx, int my) {
		int count = 0; // Looping variable
		int xDiff;
		int yDiff;
		int returnValue = 0; // Value to return

		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				xDiff = Utils.GetGap(mx, plrs[count].getMX());
				yDiff = Utils.GetGap(mx, plrs[count].getMY());
				if (count == myPlayerNum
						|| (xDiff <= PLAYER_MAX_SELECT_DISTANCE && yDiff <= PLAYER_MAX_SELECT_DISTANCE)) {
					returnValue++;
				}
			}
			count++;
		}
		return returnValue;
	}

	/**
	 * returns our players number
	 * 
	 * @returns player number
	 */
	public int getSelf() {
		return myPlayerNum;
	}

	public void setPlayer(int playerNum, String playerName, String location,
			int mx, int my, int px, int py, int frame, boolean onBoat,
			int numAllies, byte allies[]) {
		int count = 0;

		if (plrs[playerNum] == null) {
			plrs[playerNum] = new Player(playerName, location);
			plrs[playerNum]
					.update(mx, my, px, py, frame, onBoat, 0, 0, 0, 0, 0);
			while (count < numAllies) {
				plrs[playerNum].addAlliance(allies[count]);
				count++;
			}

			/*
			 * Update front end if we are in a running game (ie not in the
			 * joining phase)
			 */
			if (gw.getNetwork().getStatus() != Network.NET_STATE_FAILED) {
				if (gw.isServer() == false) {
					gw.getFrontEndClient().feSetPlayer(playerNum,
							plrs[playerNum].getName());
				} else {
					gw.getFrontEndClient().feSetPlayer(playerNum,
							plrs[playerNum].getNameLocation());
				}
				gw.getFrontEndDraw().feDrawStatusTank(playerNum,
						screenAlliance(playerNum));

			}

		}

	}

	/**
	 * Makes the screen Status for this tank
	 * 
	 * @return one of GameItem.GAME_ITEM_BLANK, GameItem.GAME_ITEM_OWM_GOOD,
	 *         GameItem.GAME_ITEM_ALLY_GOOD or GameItem.GAME_ITEM_EVIL
	 * @param playerNum
	 *            Player number to get
	 */
	public int screenAlliance(int playerNum) {
		if (plrs[playerNum] != null) {
			if (playerNum == myPlayerNum) {
				return GameItem.GAME_ITEM_OWN_GOOD;
			} else if (plrs[myPlayerNum].allianceExist(playerNum) == true) {
				return GameItem.GAME_ITEM_ALLY_GOOD;
			} else {
				return GameItem.GAME_ITEM_EVIL;
			}
		}

		return GameItem.GAME_ITEM_BLANK;
	}

	/**
	 * sets up our player $param playerNum player number to set
	 * 
	 * @param name
	 *            Players name to set
	 * @return if it is OK to setup (playername ok, playernum ok)
	 */
	public boolean setSelf(int playerNum, String name) {
		boolean returnValue = false;
		if (plrs[playerNum] == null) {
			if (isNameTaken(name) == false) {
				returnValue = true;
				myPlayerNum = playerNum;
				plrs[playerNum] = new Player(name, "");
				// FIXME Probably better to do this in netsetup go
				// gw.getLGM().setPlayerNum(playerNum);
				gw.getFrontEndClient().feSetPlayer(playerNum, name);
			}
		}

		return returnValue;
	}

	/**
	 * Sets/Changes a players name. If successful it announces it to game
	 * 
	 * @return if the name change is successful (Fails if name is in use or is
	 *         our player num)
	 * @param playerNum
	 *            Player name to change
	 * @param name
	 *            New name to change to
	 */
	public boolean setPlayerName(int playerNum, String name) {
		boolean returnValue = false;
		String message;
		String label;

		if (plrs[playerNum] != null) {
			if (isNameTaken(name) == false) {
				returnValue = true;
				if (playerNum == this.myPlayerNum) {
					label = gw.getLabels().makeMessageLabel(
							plrs[playerNum].getName(), "This Computer");
				} else {
					label = gw.getLabels().makeMessageLabel(
							plrs[playerNum].getName(),
							plrs[playerNum].getLocation());
				}
				message = "\"" + label + "\" just changed name to \"" + name
						+ "\"";
				gw.getMessages().add(Messages.NEWSWIRE_MESSAGE, "Newswire",
						message);
				plrs[playerNum].setName(name);

				gw.getFrontEndClient().feSetPlayer(playerNum,
						plrs[playerNum].getNameLocation());
				gw.getLog().addEvent(Log.log_ChangeName, (byte) 0, (byte) 0,
						(byte) 0, (byte) 0, (short) 0, name);
			}
		}

		return returnValue;
	}

	/**
	 * Updates a player with location data
	 */
	public void update(int playerNum, int mx, int my, int px, int py,
			int frame, boolean onBoat, int lgmMX, int lgmMY, int lgmPX,
			int lgmPY, int lgmFrame) {
		if (plrs[playerNum] != null) {
			plrs[playerNum].update(mx, my, px, py, frame, onBoat, lgmMX, lgmMY,
					lgmPX, lgmPY, lgmFrame);
		}
	}

	/**
	 * Called every game tick to update the LGM frame to give the illusion of
	 * movement
	 */
	public void gameTickUpdate() {
		int count = 0;
		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				plrs[count].lgmFrameUpdate();
			}
		}
	}

	/**
	 * Gets a player name for a player number
	 * 
	 * @param playerNum
	 *            to get player name for
	 * @return String the player name
	 */
	public String getPlayerName(int playerNum) {
		if (plrs[playerNum] != null) {
			return plrs[playerNum].getName();
		}
		return NO_TANK;
	}

	/**
	 * Gets a player location for a player number
	 * 
	 * @param playerNum
	 *            to get player location for
	 * @return String the player location
	 */
	public String getPlayerLocation(int playerNum) {
		if (plrs[playerNum] != null) {
			return plrs[playerNum].getLocation();
		}
		return NO_TANK;

	}

	/**
	 * Sets the players menu up with player names or empties them if not in use
	 */
	public void setPlayersMenu() {
		int count = 0;
		FrontEndClient fec = gw.getFrontEndClient();

		if (gw.isServer() == false) {
			while (count < MAX_PLAYERS) {
				if (plrs[count] != null) {
					if (count == myPlayerNum) {
						fec.feSetPlayer(count, plrs[count].getName());
					} else {
						fec.feSetPlayer(count, plrs[count].getNameLocation());
					}

				} else {
					fec.feSetPlayer(count, "");
				}
				count++;
			}
		}
	}

	/**
	 * Checks to see if any other player is closer to a pillbox that is shooting
	 * then us.
	 * 
	 * @return if a tank is closer then us (and not allied to the pillbox
	 * @param x
	 *            X world co-ordinate of the pillbox
	 * @param y
	 *            Y world co-ordinate of the pillbox
	 * @param pillOwner
	 *            Pillbox owner
	 * @param tankAmount
	 *            Distance the tank is from the pillbox
	 */
	public boolean isTankCloser(int x, int y, int pillOwner, double tankAmount) {
		boolean returnValue = false;
		int count = 0;
		int wx;
		int wy;
		int diffX;
		int diffY;
		double distance;

		while (count < MAX_PLAYERS && returnValue == false) {
			if (plrs[count] != null) {
				if (count != pillOwner
						&& plrs[count].allianceExist(pillOwner) == false
						&& plrs[count].getMX() != 0 && plrs[count].getMY() != 0) {

					wx = plrs[count].getWX();
					wy = plrs[count].getWY();
					diffX = Utils.GetGap(x, wx);
					diffY = Utils.GetGap(y, wy);
					if ((diffX < MIN_TREEHIDE_DIST && diffY < MIN_TREEHIDE_DIST)) { // FIXME
																					// gw.isItemInTrees(wx,
																					// wy)
																					// ==
																					// false
																					// ||
						distance = Utils.GetItemRange(x, y, wx, wy);
						if (distance < tankAmount) {
							returnValue = true;
						}

					}
				}
			}
			count++;
		}
		return returnValue;
	}

	/**
	 * Makes the list of ScreenItems of the other players lgms to display on the
	 * screen
	 * 
	 * @return ArrayList of ScreenTankItem
	 * @param left
	 *            Left position of the screen
	 * @param right
	 *            Right position of the screen
	 * @param top
	 *            Top position of the screen
	 * @param bottom
	 *            Bottom of the screen
	 * @param Tank
	 *            co-ordinates in world units
	 * @param Tank
	 *            co-ordinates in world units
	 */
	public ArrayList<ScreenItem> makeScreenLgms(int left, int right, int top, int bottom,
			int tankX, int tankY) {
		ArrayList<ScreenItem> returnValue = new ArrayList<ScreenItem>();
		Player p;
		String name;
		int count = 0;
		int mx;
		int my;
		int wx;
		int wy;
		int diffX;
		int diffY;
		ScreenItem si;

		while (count < MAX_PLAYERS) {
			p = plrs[count];
			if (p != null && count != myPlayerNum) {
				mx = p.getLgmMX();
				my = p.getLgmMY();
				if (mx >= left && mx <= right && my >= top && my <= bottom) {
					wx = p.getLgmWX();
					wy = p.getLgmWY();
					diffX = Math.abs(wx - tankX);
					diffY = Math.abs(wy - tankY);
					if (p.getLgmFrame() == LGM.LGM_HELICOPTER_FRAME
							|| Utils.IsTankInTrees(gw, wx, wy) == false
							|| (diffX < MIN_TREEHIDE_DIST && diffY < MIN_TREEHIDE_DIST)) {
						/* Add it */
						wx -= Tank.TANK_SUBTRACT;
						wy -= Tank.TANK_SUBTRACT;
						mx = Utils.GetMapFromWorld(wx);
						my = Utils.GetMapFromWorld(wy);
						si = new ScreenItem(mx, my, Utils.GetPixelFromWorld(wx)
								- left, Utils.GetPixelFromWorld(wy) - top, p
								.getFrame());
						returnValue.add(si);
					}
				}
			}
			count++;
		}

		return returnValue;
	}

	/**
	 * Makes the list of ScreenTankItems of the other players tanks to display
	 * on the screen
	 * 
	 * @return ArrayList of ScreenTankItem
	 * @param left
	 *            Left position of the screen
	 * @param right
	 *            Right position of the screen
	 * @param top
	 *            Top position of the screen
	 * @param bottom
	 *            Bottom of the screen
	 * @param Tank
	 *            co-ordinates in world units
	 * @param Tank
	 *            co-ordinates in world units
	 */
	public ArrayList<ScreenTankItem> makeScreenTanks(int left, int right, int top, int bottom,
			int tankX, int tankY) {
		ArrayList<ScreenTankItem> returnValue = new ArrayList<ScreenTankItem>(15);
		Player p;
		String name;
		int count = 0;
		int mx;
		int my;
		int wx;
		int wy;
		int diffX;
		int diffY;
		int frame;
		ScreenTankItem sti;

		while (count < MAX_PLAYERS) {
			p = plrs[count];
			if (p != null && count != myPlayerNum) {
				mx = p.getMX();
				my = p.getMY();
				if (mx >= left && mx <= right && my >= top && my <= bottom) {
					wx = p.getWX();
					wy = p.getWY();
					diffX = Math.abs(wx - tankX);
					diffY = Math.abs(wy - tankY);
					if (Utils.IsTankInTrees(gw, wx, wy) == false
							|| (diffX < MIN_TREEHIDE_DIST && diffY < MIN_TREEHIDE_DIST)) {
						/* Add it */
						wx -= Tank.TANK_SUBTRACT;
						wy -= Tank.TANK_SUBTRACT;
						mx = Utils.GetMapFromWorld(wx);
						my = Utils.GetMapFromWorld(wy);
						frame = p.getFrame();
						if (p.getOnBoat() == true) {
							frame += TileNum.TANK_BOAT_ADD;
						}

						if (p.allianceExist(myPlayerNum) == true) {
							frame += TileNum.TANK_GOOD_ADD;
						} else {
							frame += TileNum.TANK_EVIL_ADD;
						}
						name = gw.getLabels().makeMessageLabel(p.getName(),
								p.getLocation());
						sti = new ScreenTankItem(mx - left, my - right, Utils
								.GetPixelFromWorld(wx), Utils
								.GetPixelFromWorld(wy), frame, count, name);
						returnValue.add(sti);
					}
				}
			}

			count++;
		}

		return returnValue;
	}

	/**
	 * Makes the screen name for a specific player
	 * 
	 * @param playerNum
	 *            Player to make name for
	 */
	public String makeScreenName(int playerNum) {
		if (playerNum >= 0 && playerNum < Players.MAX_PLAYERS) {

			if (playerNum == myPlayerNum) {
				return gw.getLabels().makeMessageLabel(
						plrs[playerNum].getName(), Players.THIS_COMPUTER);
			} else if (plrs[playerNum] != null) {
				return gw.getLabels().makeMessageLabel(
						plrs[playerNum].getName(),
						plrs[playerNum].getLocation());
			}
		}

		return Players.NO_TANK;
	}

	/**
	 * Makes the message name for a specific player
	 * 
	 * @return String containing the player name defined for a message
	 * @param platerNum
	 *            Player number to get the player name
	 */
	public String makeMessageName(int playerNum) {
		Player player;
		if (playerNum == myPlayerNum) {
			player = plrs[playerNum];
			return gw.getLabels().makeMessageLabel(player.getName(),
					"This Computer");
		} else if (plrs[playerNum] == null) {
			return NO_TANK;
		}
		player = plrs[playerNum];
		return gw.getLabels().makeMessageLabel(player.getName(),
				player.getLocation());
	}

	/**
	 * Checks if playerA is allied to playerB
	 * 
	 * @return if players are allied
	 * @param playerA
	 *            Player A to check
	 * @param playerB
	 *            Player B to check
	 */
	public boolean isAlly(int playerA, int playerB) {
		boolean returnValue; /* Value to return */
		int check; /* Which item to check - In case one player has left */
		int check2; /* Whick item to check for - In case one player has left */

		check = check2 = GameItem.NEUTRAL;
		returnValue = false;

		if (playerA == playerB) {
			returnValue = true;
		} else if (playerA == GameItem.NEUTRAL || playerB == GameItem.NEUTRAL) {
			returnValue = false;
		} else {
			/* Check for exist */
			if (plrs[playerA] != null) {
				check = playerA;
				check2 = playerB;
			} else if (plrs[playerB] != null) {
				check = playerB;
				check2 = playerA;
			}

			if (check != GameItem.NEUTRAL) {
				returnValue = plrs[check].allianceExist(check2);
				;
			}
		}
		return returnValue;
	}

	/**
	 * Checks to see if a tank that isn't us or the shell owner is hit by a
	 * shell
	 * 
	 * @return the player number that was hit or GameItem.NEUTRAL if no one was
	 *         hit
	 * @param x
	 *            X world co-ordinate of the shell
	 * @param y
	 *            Y world co-ordinate of the shell
	 * @param angle
	 *            Angle the shell is travelling at
	 * @param owner
	 *            Owner of the shell
	 */
	public int isTankHit(int x, int y, float angle, int owner) {
		int returnValue = GameItem.NEUTRAL;
		int count = 0;
		int tankX;
		int tankY;
		float tankAngle;

		while (returnValue == GameItem.NEUTRAL && count < MAX_PLAYERS) {
			if (plrs[count] != null && count != myPlayerNum && count != owner) {
				tankX = plrs[count].getWX();
				tankY = plrs[count].getWY();
				tankAngle = plrs[count].getFrame() * Tank.TANK_FRAMES;
				if (Utils.IsTankHit(tankX, tankY, tankAngle, x, y, angle) == true) {
					returnValue = count;
				}

			}

			count++;
		}

		return returnValue;
	}

	/**
	 * Gets the first non used player number
	 * 
	 * @return player number that is free or GameItem.NEUTRAL if all used
	 */
	public int getFirstNotUsed() {
		int count = 0;

		while (count < MAX_PLAYERS) {
			if (plrs[count] == null) {
				return count;
			}
			count++;
		}
		return GameItem.NEUTRAL;
	}

	public void leaveGame(int playerNum) {
		int count = 0;
		String label;

		if (plrs[playerNum] != null) {
			while (count < MAX_PLAYERS) {
				if (plrs[count] != null && playerNum != myPlayerNum) {
					plrs[count].removeAlliance(playerNum);
				}
				count++;
			}

			label = "\"" + makeMessageName(playerNum) + "\" just quit game.";
			gw.getMessages().add(Messages.NEWSWIRE_MESSAGE, "Newswire", label);
			if (gw.isServer() == false) {
				gw.getFrontEndClient().feClearPlayer(playerNum);
				gw.getFrontEndClient().feSetPlayerCheckState(playerNum, false);
				gw.getFrontEndDraw().feDrawStatusTank(playerNum,
						GameItem.GAME_ITEM_BLANK);
			}
			plrs[playerNum] = null;
		}
	}

	/**
	 * Returns a player for playerNum
	 * 
	 * @return Player
	 * @param playerNum
	 *            Player number to get player for
	 */
	public Player getPlayer(int playerNum) {
		if (plrs[playerNum] != null) {
			return plrs[playerNum];
		}
		return null;
	}

	/**
	 * Sets a tanks location for an IP address
	 * 
	 * @param ip
	 *            IP address to change
	 * @param hostname
	 *            Hostname to change to
	 */
	public void setLocation(String ip, String hostname) {
		int count = 0;
		while (count < MAX_PLAYERS) {
			if (plrs[count] != null && count != myPlayerNum) {
				if (plrs[count].getLocation().equals(ip) == true) {
					plrs[count].setLocation(hostname);
					gw.getFrontEndClient().feSetPlayer(count,
							plrs[count].getNameLocation());
				}
			}
			count++;
		}
	}

	/**
	 * Sets all player name in the players menu
	 */
	public void setMenuItems() {
		FrontEndClient fec = gw.getFrontEndClient();
		int count = 0;
		if (gw.isServer() == false) {
			while (count < MAX_PLAYERS) {
				if (plrs[count] != null) {
					if (count == myPlayerNum) {
						fec.feSetPlayer(count, plrs[count].getName());
					} else {
						fec.feSetPlayer(count, plrs[count].getNameLocation());
					}
				}
				count++;
			}

			fec.feEnableRequestAllyMenu(false);
			fec.feEnableLeaveAllyMenu(false);
		}
	}

	/**
	 * Sets a players checked state
	 * 
	 * @param playerNum
	 *            player number to check
	 * @param state
	 *            Players new checked state
	 */
	public void setPlayerCheckedState(int playerNum, boolean state) {
		if (playerNum >= 0 && playerNum < MAX_PLAYERS) {
			if (plrs[playerNum] != null) {
				plrs[playerNum].setChecked(state);
				if (gw.isServer() == false) {
					gw.getFrontEndClient().feSetPlayerCheckState(playerNum,
							state);
				}
			}
		}
	}

	/**
	 * Gets the number of players we have checked
	 * 
	 * @return the number of checked players
	 */
	public int getNumChecked() {
		int returnValue = 0; // Value to return
		int count = 0; // Looping variable
		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				if (plrs[count].getChecked() == true) {
					returnValue++;
				}
			}
			count++;
		}

		return returnValue;
	}

	/**
	 * Selects nearby tanks
	 * 
	 * @param mx
	 *            Our tanks map X position
	 * @param my
	 *            Our tanks map y position
	 */
	public void selectNearby(int mx, int my) {
		int count = 0; // Looping variable
		int xDiff;
		int yDiff;
		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				xDiff = Utils.GetGap(mx, plrs[count].getMX());
				yDiff = Utils.GetGap(mx, plrs[count].getMY());
				if (count == myPlayerNum
						|| (xDiff <= PLAYER_MAX_SELECT_DISTANCE && yDiff <= PLAYER_MAX_SELECT_DISTANCE)) {
					plrs[count].setChecked(true);
				} else {
					plrs[count].setChecked(false);
				}
				if (gw.isServer() == false) {
					gw.getFrontEndClient().feSetPlayerCheckState(count,
							plrs[count].getChecked());
				}

			}
			count++;
		}
		setAllieMenu();
	}

	/**
	 * Toggles the players check state for this player number
	 * 
	 * @param playerNumber
	 *            to toggle
	 */
	public void toggleCheckedState(int playerNum) {
		if (plrs[playerNum] != null) {
			plrs[playerNum].setChecked(!plrs[playerNum].getChecked());

			if (gw.isServer() == false) {
				gw.getFrontEndClient().feSetPlayerCheckState(playerNum,
						plrs[playerNum].getChecked());
			}
		}
		setAllieMenu();
	}

	/**
	 * Checks/unchecks all players in the game and sets the menu
	 * 
	 * @param isChecked
	 *            Whether to check or uncheck all players
	 */
	public void selectAllNone(boolean isChecked) {
		int count = 0;
		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				plrs[count].setChecked(isChecked);
			}
			if (gw.isServer() == false) {
				gw.getFrontEndClient().feSetPlayerCheckState(count, isChecked);
			}
			count++;
		}
		setAllieMenu();
	}

	/**
	 * Selects allies
	 */
	public void selectAllies() {
		int count = 0; // Looping variable
		Player p = plrs[this.myPlayerNum];
		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				if (p.allianceExist(count) == true || count == myPlayerNum) {
					plrs[count].setChecked(true);
				} else {
					plrs[count].setChecked(false);
				}
				if (gw.isServer() == false) {
					gw.getFrontEndClient().feSetPlayerCheckState(count,
							plrs[count].getChecked());
				}
			}
			count++;
		}
		setAllieMenu();
	}

	/**
	 * Selects no allies
	 */
	public void selectNone() {
		int count = 0; // Looping variable

		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				plrs[count].setChecked(false);
				if (gw.isServer() == false) {
					gw.getFrontEndClient().feSetPlayerCheckState(count, false);
				}
			}
			count++;
		}
		setAllieMenu();
	}

	/**
	 * Selects no allies
	 */
	public void selectAll() {
		int count = 0; // Looping variable

		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				plrs[count].setChecked(true);
				if (gw.isServer() == false) {
					gw.getFrontEndClient().feSetPlayerCheckState(count, true);
				}
			}
			count++;
		}
		setAllieMenu();
	}

	/**
	 * Sends a message to of our allies
	 * 
	 * @param message
	 *            Message to send
	 */
	public void sendMessageAllAllies(String message) {
		int count = 0;

		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				if (plrs[myPlayerNum].allianceExist(count) == true) {
					// FIXME net netMessageSendPlayer((*plrs)->myPlayerNum,
					// count, messageStr);
				}
			}
			count++;
		}
	}

	/**
	 * Sends a message to all players selected on the players menu
	 * 
	 * @param message
	 *            Message to send
	 */
	public void sendMessageAllSelected(String message) {
		int count = 0;

		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				if (plrs[count].getChecked() == true) {
					if (count == myPlayerNum) {
						// Send to self
						gw.getMessages().add(myPlayerNum,
								makeMessageName(myPlayerNum), message);
					} else {
						// FIXME net netMessageSendPlayer((*plrs)->myPlayerNum,
						// count, messageStr);
					}
				}
			}
			count++;
		}
	}

	/**
	 * Sends a message to all nearby tanks
	 * 
	 * @param mx
	 *            Our tanks map X position
	 * @param my
	 *            Our tanks map y position
	 * @param message
	 *            Message to send
	 */
	public void sendMessgeAllNearby(int mx, int my, String message) {
		int count = 0; // Looping variable
		int xDiff;
		int yDiff;
		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				xDiff = Utils.GetGap(mx, plrs[count].getMX());
				yDiff = Utils.GetGap(mx, plrs[count].getMY());
				if (count == myPlayerNum
						|| (xDiff <= PLAYER_MAX_SELECT_DISTANCE && yDiff <= PLAYER_MAX_SELECT_DISTANCE)) {
					// FIXME netMessageSendPlayer((*plrs)->myPlayerNum, count,
					// messageStr);
				}
			}
			count++;
		}
	}

	/**
	 * Returns if a player number is in use
	 * 
	 * @return If the player number is in use
	 * @param playerNum
	 *            Player number to check
	 */
	public boolean isInUse(int playerNum) {
		if (plrs[playerNum] == null) {
			return false;
		}
		return true;
	}

	/**
	 * Process a request alliance to all checked players
	 */
	public void requestAlliance() {
		int count = 0;

		while (count < MAX_PLAYERS) {
			if (plrs[count] != null && count != myPlayerNum) {
				if (plrs[count].getChecked() == true
						&& plrs[myPlayerNum].allianceExist(count) == false) {
					// Place request
					// FIXME netRequestAlliance((*plrs)->myPlayerNum, count);}
				}
			}
			count++;
		}

	}

	/**
	 * Process a leave alliance request
	 * 
	 * @param playerNum
	 *            Player that is leaving the alliance
	 */
	public void leaveAlliance(int playerNum) {
		int count = 0;

		plrs[playerNum].clearAllAlliances();
		while (count < MAX_PLAYERS) {
			if (count != playerNum && plrs[count] != null) {
				plrs[count].removeAlliance(myPlayerNum);
			}
			count++;
		}

		updateScreen();
	}

	/**
	 * A player has been accepted into an alliance
	 * 
	 * @param acceptedBy
	 *            The player that accepted the person into the alliance
	 * @param newMember
	 *            The new member in the alliance
	 */
	public void acceptAlliance(int acceptedBy, int newMember) {
		int count = 0;
		int count2;
		Player playerA = plrs[acceptedBy];
		Player playerB = plrs[newMember];

		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				if (count == acceptedBy || playerA.allianceExist(count) == true) {
					count2 = 0;
					while (count2 < MAX_PLAYERS) {
						if (playerB.allianceExist(count2) == true) {
							plrs[count].addAlliance(count2);
						}
						count2++;
					}
				}

				if (count == newMember || playerB.allianceExist(count) == true) {
					count2 = 0;
					while (count2 < MAX_PLAYERS) {
						if (playerA.allianceExist(count2) == true) {
							plrs[count].addAlliance(count2);
						}
						count2++;
					}
				}
			}
			count++;
		}

		updateScreen();
	}

	/**
	 * Called if your connection is lost to a network game. Drops all other
	 * players except yourself.
	 */
	public void connectionLost() {
		byte allies[];
		int count = 0;

		allies = plrs[myPlayerNum].makeNetAllies();

		while (count < allies.length) {
			// FIXME gw.changeOwnership(allies[count]);
			count++;
		}

		count = 0;
		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				leaveGame(count);
			}
		}

		updateScreen();
	}

	/**
	 * Checks for a collision between our tank (given as variables & the tanks
	 * in the players structure)
	 * 
	 * @return PlayerCollision indicating if the collision occured (x = If left <
	 *         0 then hit from left > 0 from right y = If up < 0 then hit from
	 *         above > 0 from below)
	 * @param playerNum
	 *            player to check for
	 * @param x
	 *            World X co-ordinate
	 * @param y
	 *            World Y co-ordinate
	 */
	public PlayerCollision checkCollision(int playerNum, int x, int y) {
		PlayerCollision returnValue = new PlayerCollision();
		int count = 0;
		int leftPos = 0;
		int downPos = 0;
		int wx;
		int wy;
		int testX;
		int testY;

		while (count < MAX_PLAYERS) {
			if (count != myPlayerNum && plrs[count] != null) {
				// Test for collision
				wx = plrs[count].getWX();
				wy = plrs[count].getWY();
				if (wx > x) {
					testX = wx - x;
					leftPos = -1;
				} else {
					testX = x - wx;
					leftPos = 1;
				}
				if (testX < 128) {
					leftPos = 0;
				}

				if (wy > y) {
					testY = wy - y;
					downPos = 1;
				} else {
					testY = y - wy;
					downPos = -1;
				}
				if (testY < 128) {
					downPos = 0;
				}

				if (testX < (256) && testY < (256)) {
					returnValue.setCollision(true);
					returnValue.set(leftPos, downPos);
					return returnValue;
				}
			}
			count++;
		}

		return returnValue;
	}

	public void checkUpdate(int playerNum) {
		if (plrs[playerNum] != null) {
			plrs[playerNum].setMX(0);
			plrs[playerNum].setMY(0);
			plrs[playerNum].setNeedUpdate(true);
		}
	}

	public boolean needUpdate(int playerNum) {
		if (plrs[playerNum] != null) {
			if (plrs[playerNum].getNeedUpdate() == true) {
				plrs[playerNum].setNeedUpdate(false);
				return true;
			}
		}

		return false;
	}

	public void needUpdateDone() {
		int count = 0;

		while (count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				plrs[count].setNeedUpdate(false);
			}
			count++;
		}
	}

	/**
	 * Updates the screen items after an alliance has been formed or split
	 */
	private void updateScreen() {
		int count = 0;
		int total;
		FrontEndDraw fed = gw.getFrontEndDraw();

		// Update the screen
		if (gw.isServer() == false) {
			total = gw.getNumBases();
			count = 0;
			while (count < total) {
				fed.feDrawStatusBase(count, gw.getBaseStatus(count));
				count++;
			}
			total = gw.getNumPillboxes();
			count = 0;
			while (count < total) {
				fed.feDrawStatusPillbox(count, gw.getPillboxStatus(count));
				count++;
			}
			total = getNumPlayers();
			count = 0;
			while (count < total) {
				fed.feDrawStatusTank(count, screenAlliance(count));
				count++;
			}
			setAllieMenu();
		}
	}

	/**
	 * Returns if the name is taken
	 * 
	 * @return if the player name is taken
	 * @param name
	 *            Player name to check
	 */
	private boolean isNameTaken(String name) {
		boolean returnValue = false;
		int count = 0;

		while (returnValue == false && count < MAX_PLAYERS) {
			if (plrs[count] != null) {
				returnValue = plrs[count].testNameSame(name);
			}
			count++;
		}
		return returnValue;
	}

	/**
	 * * Sets the alliance request allowed or leave enabled state
	 */
	private void setAllieMenu() {
		int count = 0; // Looping variable
		FrontEndClient fec = gw.getFrontEndClient();
		boolean req = false; /* Enable the request menu */
		boolean leave = false; /* Enable the leave menu */

		count = 0;
		if (gw.isServer() == false) {
			while (count < MAX_PLAYERS && leave == false) {

				if (plrs[count] != null && count != myPlayerNum) {
					if (plrs[myPlayerNum].allianceExist(count) == true) {
						leave = true;
						req = false;
					} else if (plrs[count].getChecked() == true) {
						req = true;
					}
				}
				count++;
			}
			fec.feEnableRequestAllyMenu(req);
			fec.feEnableLeaveAllyMenu(leave);
		}
	}
}
