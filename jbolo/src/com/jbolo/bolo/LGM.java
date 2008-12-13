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
 * LGM.java
 *
 * Created on December 30, 2003, 8:11 PM
 */

package com.jbolo.bolo;

import com.jbolo.bolo.map.BoloMap;

/**
 * Class incorporates the tanks builder (lgm)
 * 
 * @author John Morrison
 */
public class LGM extends com.jbolo.bolo.GameWorldItem {

	/** Build boat request - Internal */
	private static final int LGM_BOAT_REQUEST = 5;

	/** This LGM is not carrying a pill */
	public static final int LGM_NO_PILL = 37;

	/** The frame number for the helicopter */
	public static final int LGM_HELICOPTER_FRAME = 3;

	/** Man is doing nothing */
	private static int LGM_IDLE = 6;

	/** Blessed area surrounding the tank */
	private static int LGM_TANKBOAT_RETURN = 160;

	/** Blessed area surrounding the tank */
	private static int LGM_TANKBOAT_LEAVE = 144;

	/** There are 3 s in animation 0-3. Frame 3 is the parachute */
	public static final int LGM_MAX_FRAMES = 2;

	/** The LGM is idle */
	private static int LGM_STATE_IDLE = 0;
	/** The LGM is moving to a task */
	private static int LGM_STATE_GOING = 1;
	/** The LGM is returning from a task */
	private static int LGM_STATE_RETURN = 2;

	/** Speed the helicpter flys at */
	private static int LGM_HELICOPTER_SPEED = 3;

	/** 20 ticks to builds something */
	private static int LGM_BUILD_TIME = 20;

	/** Min distance away from thing to achieve goal */
	private static int LGM_MIN_GOAL = -16;
	/** Max distance away from thing to achieve goal */
	private static int LGM_MAX_GOAL = 16;

	/** Min and Max distance away from thing to achieve goal */
	private static int LGM_RETURN_MIN_GOAL = (-16 * 5);
	/** Min and Max distance away from thing to achieve goal */
	private static int LGM_RETURN_MAX_GOAL = (16 * 5);

	/** The costs to build a road */
	private static int LGM_COST_ROAD = 2;
	/** The costs to build a building */
	private static int LGM_COST_BUILDING = 2;
	/** The costs to fix a building */
	private static int LGM_COST_REPAIRBUILDING = 1;
	/** The costs to build a boat */
	private static int LGM_COST_BOAT = 20;
	/** The costs to build a pillbox */
	private static int LGM_COST_PILLNEW = 4;
	/** The costs to repair a pillbox */
	private static int LGM_COST_PILLREPAIR = 2;
	/** The costs to lay a mine */
	private static int LGM_COST_MINE = 1;

	/** This LGM has gathered some trees */
	private static int LGM_GATHER_TREE = 4;

	/** Where the man is heading X position */
	private int destX;
	/** Where the man is heading Y position */
	private int destY;
	/** Present state 0 = Idle, 1 = Going to do work, 2 = returning to tank */
	private int state;
	/* & Is the LGM counting down waiting for something? */
	private int waitTime;
	/** Is the builder in the tank */
	private boolean inTank;
	/** Is the builder dead ? */
	private boolean isDead;
	/** Number of trees the builder is carrying */
	private int numTrees;
	/** Number of mines the builder is carrying */
	private int numMines;
	/** Number of pills being carried (the pill number) */
	private int numPills;
	/** What the builders current task is */
	private int action;
	/** X Map Co-ordinates of next action */
	private int nextX;
	/** Y Map Co-ordinates of next action */
	private int nextY;
	/** What the next action to do */
	private int nextAction;
	/** Animation frame */
	private int frame;
	/** The lgm blessed X map square (can travel over it) */
	private int blessX;
	/** The lgm blessed Y map square (can travel over it) */
	private int blessY;
	/** For brain - 0 = free, 1 = touching wall, 2 = completely stuck */
	private int obstructed;
	/** The LGM player Number */
	private int playerNum;

	/** The GameWorld that this LGM belongs in */
	private GameWorld gw;

	/** The tank that this LGM belongs to */
	private Tank tank;

	/**
	 * Creates a new instance of LGM
	 * 
	 * @param gw
	 *            GameWorld this LGM is in
	 * @param playerNum
	 *            The Player number that belongs to this LGM
	 * @param tank
	 *            The tank that this LGM belongs to
	 */
	public LGM(GameWorld gw, int playerNum, Tank tank) {
		super();
		this.gw = gw;
		this.tank = tank;
		this.playerNum = playerNum;
		x = 0;
		y = 0;
		inTank = true;
		isDead = false;
		nextAction = LGM_IDLE;
		action = LGM_IDLE;
		state = LGM_STATE_IDLE;
		frame = 0;
		numTrees = 0;
		numMines = 0;
		numPills = LGM_NO_PILL;
		waitTime = 0;
		blessX = 0;
		blessY = 0;
		obstructed = 0;
	}

	/**
	 * Sets this LGM's player number
	 * 
	 * @param value
	 *            Value to set
	 */
	public void setPlayerNum(int value) {
		playerNum = value;
	}

	/**
	 * Out tank has died. Cancel any pending orders
	 */
	public void tankDied() {
		nextAction = LGM_IDLE;
	}

	/**
	 * Gets the ScreenItem of this LGM
	 * 
	 * @return ScreenItem if item is on screen or null if it is not
	 * @param left
	 *            Left position of the screen
	 * @param right
	 *            Right position of the screen
	 * @param top
	 *            Top position of the screen
	 * @param bottom
	 *            Bottom of the screen
	 */
	public ScreenItem getScreenItem(int left, int right, int top, int bottom) {
		ScreenItem si = null;
		int wx, wy; // World location
		int mx, my; // Location of the lgm
		int f; // Frame

		if (inTank == false) {

			wx = x;
			wy = y;
			if (isDead == true) {
				f = LGM_HELICOPTER_FRAME;
				wx -= BoloMap.MAP_SQUARE_MIDDLE;
				wy -= BoloMap.MAP_SQUARE_MIDDLE;
			} else {
				f = frame;
				wx -= 1;
				wy -= 2;
			}

			mx = Utils.GetMapFromWorld(wx);
			my = Utils.GetMapFromWorld(wy);
			if (mx >= left && mx <= right && my >= top && my <= bottom) {
				si = new ScreenItem(mx - left, my - top, Utils
						.GetPixelFromWorld(wx), Utils.GetPixelFromWorld(wy), f);
			}
		}
		return si;
	}

	/**
	 * A new builder request has been created
	 * 
	 * @param mapX
	 *            Target map position
	 * @param mapY
	 *            Target map position
	 * @param action
	 *            Building request type
	 */
	public void addRequest(int mapX, int mapY, int action) {
		if (isDead == true && tank.isDead() == false) {
			gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME: Builder",
					"You can not build until your new man parachutes in");
		} else if (this.action != LGM_IDLE) {
			/* Busy doing something else Place in second request */
			nextX = mapX;
			nextY = mapY;
			nextAction = action;
		} else if (gw.isServerOrSinglePlayerGame() == true) {
			/* Man isn't doing something Check to make sure is possible first */
			newPrimaryRequest(mapX, mapY, action);
		} else {
			/* Network game */
			boolean ok;
			ok = checkNewRequest(mapX, mapY, action, false);
			if (ok) {
				gw.getMNT().add(NetChangeMNT.NMNT_MANACTION, action, playerNum,
						mapX, mapY);
			} else {
				backInTank(true);
			}
		}
	}

	private boolean checkIsMine;
	private int checkTrees;
	private int checkMines;
	private int checkPill;
	private int checkAction;

	private boolean checkNewRequest(int mapX, int mapY, int action,
			boolean perform) {
		boolean proceed; /* Is it OK to proceed with the action */
		boolean isBase; /* Is the present co-ordinate a base */
		boolean isPill; /* Is the present co-ordinate a pill */
		int tankX; /* Tank co-ordinates */
		int tankY;
		int pos; /* Map terrain at build request place */

		tankX = tank.getMX();
		tankY = tank.getMY();

		checkAction = action;
		proceed = true;
		checkIsMine = false;
		checkTrees = 0;
		checkMines = 0;
		checkPill = LGM_NO_PILL;

		pos = gw.getBoloMap().getPos(mapX, mapY);
		if (pos == Terrain.MINE_FOREST) {
			pos = Terrain.FOREST;
		}
		isPill = gw.getPills().existPos(mapX, mapY);
		isBase = gw.getBases().existPos(mapX, mapY);

		switch (checkAction) {
		case GameClient.LGM_TREE_REQUEST:
			if (pos != Terrain.FOREST || isBase == true || isPill == true) {
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"There are no trees");
				proceed = false;
			}
			break;
		case GameClient.LGM_ROAD_REQUEST:
			if (pos == Terrain.FOREST && isPill == false && isBase == false) {
				checkAction = GameClient.LGM_TREE_REQUEST;
			} else if (pos == Terrain.BOAT || pos == Terrain.DEEP_SEA
					|| pos == Terrain.BUILDING || pos == Terrain.HALFBUILDING
					|| isPill == true || isBase == true) {
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You can't build that there");
			} else if (pos == Terrain.ROAD) {
				proceed = false;
			} else if (tank.getLgmTrees(LGM_COST_ROAD, perform) == false) {
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You don't have enough trees");
			} else {
				checkTrees = LGM_COST_ROAD;
			}
			break;
		case GameClient.LGM_BUILDING_REQUEST:
			if (pos == Terrain.FOREST && isPill == false && isBase == false) {
				checkAction = GameClient.LGM_TREE_REQUEST;
			} else if (pos == Terrain.BOAT || pos == Terrain.DEEP_SEA
					|| isPill == true || isBase == true) {
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You can't build there");
			} else if (pos == Terrain.RIVER) {
				checkAction = LGM_BOAT_REQUEST;
				if (tank.getLgmTrees(LGM_COST_BOAT, perform) == false) {
					proceed = false;
					gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
							"You don't have enough trees");
				} else {
					checkTrees = LGM_COST_BOAT;
				}
			} else if (tankX == mapX && tankY == mapY) {
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You can't build on a tank silly!");
			} else if (pos == Terrain.HALFBUILDING) {
				if (tank.getLgmTrees(LGM_COST_REPAIRBUILDING, perform) == false) {
					proceed = false;
					gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
							"You don't have enough trees");
				} else {
					checkTrees = LGM_COST_REPAIRBUILDING;
				}
			} else if (pos == Terrain.BUILDING) {
				proceed = false;
			} else if (tank.getLgmTrees(LGM_COST_BUILDING, perform) == false) {
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You don't have enough trees");
			} else {
				checkTrees = LGM_COST_BUILDING;
			}

			break;
		case GameClient.LGM_PILL_REQUEST:
			if (pos == Terrain.BOAT || pos == Terrain.DEEP_SEA
					|| pos == Terrain.BUILDING || pos == Terrain.HALFBUILDING
					|| pos == Terrain.RIVER || isBase == true) {
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You can't build that there");
			} else if (pos == Terrain.FOREST && isPill == false) {
				checkAction = GameClient.LGM_TREE_REQUEST;
			} else if (tankX == mapX && tankY == mapY) {
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You can't build on a tank silly");
			} else if (isPill == true) {
				if (gw.getPills().getArmourPos(mapX, mapY) == Pillbox.PILL_MAX_HEALTH) {
					proceed = false;
					gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
							"You don't have enough trees");
				} else if (tank.getLgmTrees(LGM_COST_PILLREPAIR, perform) == false) {
					proceed = false;
					gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
							"You don't have enough trees");
				} else {
					checkTrees = LGM_COST_PILLREPAIR;
				}
				checkPill = LGM_NO_PILL;
				// FIXME
			} else if ((checkPill = tank.getCarriedPill(perform)) == LGM_NO_PILL) {
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You don't have a pillbox there");
			} else if (tank.getLgmTrees(LGM_COST_PILLNEW, perform) == false) {
				proceed = false;
				tank.putCarriedPill(checkPill);
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You don't have enough trees");
			} else {
				checkTrees = LGM_COST_PILLNEW;
			}
			break;
		case LGM_BOAT_REQUEST:
			if (pos != Terrain.RIVER || isPill == true || isBase == true) {
				proceed = false;
			} else if (tank.getLgmTrees(LGM_COST_BOAT, perform) == false) {
				proceed = false;
			} else {
				checkTrees = LGM_COST_BOAT;
			}
			break;
		default:
			/* Case LGM_REQUEST_MINE */
			if (pos == Terrain.DEEP_SEA || pos == Terrain.RIVER
					|| pos == Terrain.BUILDING || pos == Terrain.BOAT
					|| pos == Terrain.HALFBUILDING || isPill == true
					|| isBase == true) {
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You can't build that there");
			} else if (tank.getLgmMines(LGM_COST_MINE, perform) == false) {
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You don't have enough mines");
			} else {
				checkMines = LGM_COST_MINE;
			}
			break;
		}
		if (action == GameClient.LGM_TREE_REQUEST && pos != Terrain.FOREST) {
			proceed = false;
			gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
					"There isn't a tree there");
		}

		if (proceed == true) {
			/* Check for a visible mine */
			if ((gw.getMines().existPos(mapX, mapY)) == true) {
				checkIsMine = true;
				proceed = false;
				gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "FIXME",
						"You can't build on a mine");
				if (nextAction == GameClient.LGM_MINE_REQUEST && nextX == mapX
						&& nextY == mapY) {
					nextAction = LGM_IDLE;
				}
			}
		}

		// checkAction = action;
		return proceed;
	}

	/**
	 * A new primary request is wanted. Check if it is possible. If so then make
	 * it so.
	 * 
	 * @param mapX
	 *            X Co-ordinate of the new action
	 * @param mapY
	 *            Y Co-ordinate of the new action
	 * @param action
	 *            What the new action is
	 */
	private void newPrimaryRequest(int mapX, int mapY, int action) {
		int pillNum;
		boolean isMine;
		int trees = 0;
		int minesAmount = 0;

		pillNum = LGM_NO_PILL;
		/* If its OK to proceed then set it up */
		if (checkNewRequest(mapX, mapY, action, true) == true) {
			/* Reset variables from check object */
			isMine = checkIsMine;
			pillNum = checkPill;
			trees = checkTrees;
			minesAmount = checkMines;
			action = checkAction;

			numTrees = trees;
			numMines = minesAmount;
			if (isMine == true) {
				state = LGM_STATE_IDLE;
				inTank = true;
				backInTank(false);
			} else {
				destX = Utils.GetWorldFromMap(mapX) + BoloMap.MAP_SQUARE_MIDDLE;
				destY = Utils.GetWorldFromMap(mapY) + BoloMap.MAP_SQUARE_MIDDLE;
				x = tank.getX();
				y = tank.getY();
				state = LGM_STATE_GOING;
				this.action = action;
				inTank = false;
				switch (action) {
				case GameClient.LGM_ROAD_REQUEST:
					if (gw.getBoloMap().getPos(mapX, mapY) == Terrain.RIVER) {
						blessX = mapX;
						blessY = mapY;
					}
					break;
				case LGM_BOAT_REQUEST:
					blessX = mapX;
					blessY = mapY;
					break;
				case GameClient.LGM_BUILDING_REQUEST:
					blessX = mapX;
					blessY = mapY;
					break;
				case GameClient.LGM_PILL_REQUEST:
					numPills = pillNum;
					blessX = mapX;
					blessY = mapY;
					break;
				case GameClient.LGM_MINE_REQUEST:
					break;
				default:
					/* Do nothing */
					break;
				}
			}
			if (gw.isServer() == true) {
				// FIXME
				// serverNetSendManWorkingMessage(screenGetTankPlayer(tnk),
				// mapX, mapY, minesAmount, trees, pillNum);
			}
		} else {
			/* Reset variables from check object */
			isMine = checkIsMine;
			pillNum = checkPill;
			trees = checkTrees;
			minesAmount = checkMines;

			state = LGM_STATE_IDLE;
			tank.giveTrees(trees);
			tank.addMines(minesAmount);
			if (pillNum != LGM_NO_PILL) {
				tank.putPill(pillNum);
			}
			if (gw.isServer() == true) {
				// FIXME serverNetSendManReturnMessge(screenGetTankPlayer(tnk),
				// trees, minesAmount, pillNum);
			}
		}
	}

	/**
	 * Game tick has passed. Update the LGMs position
	 */
	public void update() {
		if (gw.isServerOrSinglePlayerGame() == true) {
			if (isDead == true) {
				parachutingIn();
			} else if (state != LGM_STATE_IDLE) {
				/* Man is out and about doing stuff */

				/* Update frame */
				frame++;
				if (frame > LGM_MAX_FRAMES) {
					frame = 0;
				}
				/* Check his not waiting */
				// FIXME obstructed = LGM_BRAIN_FREE;
				if (waitTime > 0) {
					waitTime--;
				} else if (state == LGM_STATE_GOING) {
					moveAway();
				} else {
					returnToTank();
				}
			}
		} else {
			if (state != LGM_STATE_IDLE) {
				/* Man is out and about doing stuff */

				/* Update frame */
				frame++;
				if (frame > LGM_MAX_FRAMES) {
					frame = 0;
				}
			}

			/* Update our location on screen */
			if (isDead == false && inTank == false) {
				gw.getFrontEndDraw().feDrawManStatus(false,
						Utils.CalcAngle(x, y, tank.getX(), tank.getY()));
			}
		}
	}

	/**
	 * Called when an item explodes to check to see if the lgm should be killed
	 * because he is on it.
	 * 
	 * @param wx
	 *            World X location of the explosion
	 * @param wy
	 *            World Y location of the explosion
	 * @param owner
	 *            Owner of the explosion (NEUTRAL for pillboxes)
	 */
	public void deathCheck(int wx, int wy, int owner) {
		String messageStr; /* Message to output */
		int lgmX; /* LGM X Map co-ordinate */
		int lgmY; /* LGM Y Map co-ordinate */
		boolean solid; /* Is the map square the lgm on solid or not */
		boolean dead; /* Are we dead */
		float dummy; /* Dummy variable used for paremeter passing */
		double distance;
		int pos;
		int mx, my;
		boolean finishedPillPlace; /* Used to place pills properly */
		int pillPlaceX;
		int pillPlaceY;
		int itemX = 0;
		int itemY = 0;
		int count;
		BoloMap mp;
		Pillboxes pbs;
		Bases bss;

		if (gw.isServerOrSinglePlayerGame() == true && isDead == false
				&& inTank == false) {
			mp = gw.getBoloMap();
			pbs = gw.getPills();
			bss = gw.getBases();
			dead = false;
			lgmX = Utils.GetMapFromWorld(x - 1);
			lgmY = Utils.GetMapFromWorld(y - 2);
			mx = Utils.GetMapFromWorld(wx);
			my = Utils.GetMapFromWorld(wy);

			distance = Utils.GetItemRange(x, y, wx, wy);
			pos = gw.getBoloMap().getPos(mx, my);
			solid = false;
			if (pos == Terrain.BUILDING || pos == Terrain.HALFBUILDING
					|| pbs.existPos(mx, my) == true
					|| bss.existPos(mx, my) == true) {
				solid = true;
			}
			if (solid == false && distance <= BoloMap.MAP_SQUARE_MIDDLE) {
				dead = true;
			} else if (solid == true && lgmX == mx && lgmY == my) {
				dead = true;
			}
			if (dead == true) {
				gw.getPNB().add(NetChangePNB.NPNB_LGM_DEAD, 0, playerNum, lgmX,
						lgmY);
				gw.getSoundDist().soundDist(SoundDist.MAN_DYING_NEAR, lgmX,
						lgmY);
				isDead = true;
				frame = LGM_HELICOPTER_FRAME;
				numTrees = 0;
				numMines = 0;
				nextAction = LGM_IDLE;
				if (numPills != LGM_NO_PILL) {
					/* Drop Pill */
					if (gw.isServerOrSinglePlayerGame() == true) {
						finishedPillPlace = false;
						count = 0;
						pillPlaceX = lgmX;
						pillPlaceY = lgmY;
						while (finishedPillPlace == false) {
							itemX = pillPlaceX;
							itemY = pillPlaceY + count;
							if (itemX > BoloMap.MAP_MINE_EDGE_LEFT
									&& itemX < BoloMap.MAP_MINE_EDGE_RIGHT
									&& itemY > BoloMap.MAP_MINE_EDGE_TOP
									&& itemY < BoloMap.MAP_MINE_EDGE_BOTTOM) {
								pos = mp.getPos(itemX, itemY);
								if (pbs.existPos(itemX, itemY) == false
										&& bss.existPos(itemX, itemY) == false
										&& pos != Terrain.BUILDING
										&& pos != Terrain.HALFBUILDING
										&& pos != Terrain.BOAT) {
									finishedPillPlace = true;
								}
							}
							count++;
							if (count == 10 && finishedPillPlace == false) {
								count = 0;
								itemY = pillPlaceY;
								pillPlaceX++;
							}
						}
						pbs.set(numPills, itemX, itemY, playerNum, 0,
								Pillbox.PILLBOX_ATTACK_NORMAL, false, false);
						gw.getPNB().add(NetChangePNB.NPNB_PILL_DEAD,
								numPills - 1, playerNum, itemX, itemY);
						if (gw.isServer() == false) {
							gw.getFrontEndDraw().feDrawStatusPillbox(numPills,
									gw.getPillboxStatus(numPills));
						}
					}
					numPills = LGM_NO_PILL;
					gw.reCalc();
				}
				destX = tank.getX();
				destY = tank.getY();
				if (destX == 0 || destY == 0) {
					destX = x;
					destY = y;
				}
				Start s = gw.getStarts().getStart();
				x = Utils.GetWorldFromMap(s.getX()) + BoloMap.MAP_SQUARE_MIDDLE;
				y = Utils.GetWorldFromMap(s.getY()) + BoloMap.MAP_SQUARE_MIDDLE;

				if (gw.isServer() == false) {
					gw.getFrontEndDraw().feDrawManStatus(true, 0.0f);
				}

				/* Log it */
				// FIXME gw.getLog().addEvent(Log.log_LostMan, (byte) playerNum,
				// 0, 0, 0, 0, Log.EMPTY_STRING);
				/* WinBolo.net it */
				/*
				 * FIXME winbolonetAddEvent(WINBOLO_NET_EVENT_LGM_LOST, TRUE,
				 * (*lgman)->playerNum, WINBOLO_NET_NO_PLAYER); if (owner !=
				 * NEUTRAL) { winbolonetAddEvent(WINBOLO_NET_EVENT_LGM_KILL,
				 * TRUE, owner, (*lgman)->playerNum); }
				 */
				/* Process message */
				Player p = gw.getPlayers().getPlayer(playerNum);
				messageStr = gw.getLabels().makeMessageLabel(p.getName(),
						p.getLocation())
						+ " just lost his builder.";
				gw.getMessages().add(Messages.NEWSWIRE_MESSAGE, "Newswire",
						messageStr);
			}
		}

	}

	/**
	 * Man is parachuting back in
	 */
	private void parachutingIn() {
		float angle; // Angle of travel
		Point p;

		angle = Utils.CalcAngle(x, y, destX, destY);
		p = Utils.CalcDistance(angle, LGM_HELICOPTER_SPEED);
		x += p.getX();
		y += p.getY();

		/* Check for achieving goal */
		if (((x - destX) >= -16 || (x - destX) <= 16)
				&& ((y - destY) >= -16 && (y - destY) <= 16)) {
			/* Arrived back at spot, Begin trek back to tank */
			if (gw.isServer() == true) {
				gw.getPNB().add(NetChangePNB.NPNB_LGM_DEAD, 1, playerNum, 0, 0);
			}
			isDead = false;
			state = LGM_STATE_RETURN;
			frame = 0;
		}
	}

	/**
	 * Man is moving towards his destination
	 */
	private void moveAway() {
		int speed; /* speed of terrain currently on */
		float angle; /* Angle of travel */
		float frontAngle; /* Front end angle of travel */
		int bmx; /* Man Map co-ords */
		int bmy;
		int newmx;
		int newmy;
		int newbmx;
		int newbmy;
		Point p; /* Add amounts */
		boolean noGo; /* Set to true if one direction couldn't move */
		boolean onBoat; /* Is the tank on a boat */
		BoloMap mp = gw.getBoloMap();

		/* Deal with the tank being on a boat */
		onBoat = checkTankBoat(LGM_TANKBOAT_LEAVE);

		noGo = false;

		angle = Utils.CalcAngle(x, y, destX, destY);
		frontAngle = Utils.CalcAngle(x, y, tank.getX(), tank.getY());
		if (gw.isServer() == false) {
			gw.getFrontEndDraw().feDrawManStatus(false, frontAngle);
		}
		bmx = getMX();
		bmy = getMY();

		if ((bmx == blessX && bmy == blessY) || onBoat == true) {
			speed = BoloMap.MAP_MANSPEED_TREFBASE;
		} else {
			speed = mp.getManSpeed(gw.getPills(), gw.getBases(), bmx, bmy,
					playerNum);
		}

		p = Utils.CalcDistance(angle, speed);
		newmx = x + p.getX();
		newmy = y + p.getY();

		newbmx = Utils.GetMapFromWorld(newmx);
		newbmy = Utils.GetMapFromWorld(newmy);

		if ((mp.getManSpeed(gw.getPills(), gw.getBases(), bmx, newbmy,
				playerNum)) > 0
				|| onBoat == true) {
			y = y + p.getY();
		} else if (bmx == blessX && newbmy == blessY
				&& checkBlessedSquare(bmx, newbmy) == true) {
			y = y + p.getY();
		} else {
			// FIXME (*lgman)->obstructed = LGM_BRAIN_PARTIAL;
			noGo = true;
			newbmy = bmy;
		}
		if (((mp.getManSpeed(gw.getPills(), gw.getBases(), newbmx, newbmy,
				playerNum))) > 0
				&& p.getX() != 0 || onBoat == true) {
			x = x + p.getX();
		} else if (newbmx == blessX && bmy == blessY
				&& checkBlessedSquare(newbmx, newbmy) == true) {
			x = x + p.getX();
		} else if (noGo == true || p.getY() == 0) {
			state = LGM_STATE_RETURN;
		}

		/* Check for achieved goal */
		if ((x - destX) >= LGM_MIN_GOAL && (x - destX) <= LGM_MAX_GOAL
				&& (y - destY) >= LGM_MIN_GOAL && (y - destY) <= LGM_MAX_GOAL) {
			/* Arrived */
			waitTime = LGM_BUILD_TIME;
			state = LGM_STATE_RETURN;
			doWork();
		}
	}

	/*
	 * This is to check the builder is close to the tank. If he is then it is
	 * considered "blessed" and can freely travel to it.
	 * 
	 * @param distance Distance to check
	 */
	private boolean checkTankBoat(int distance) {
		if (tank.isOnBoat() == true) {
			if (Math.abs(x - tank.getX()) < distance
					&& Math.abs(y - tank.getY()) < distance) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Checks if a map square is blessed or not
	 * 
	 * @return boolean indicating if it is blessed
	 * @param x
	 *            X map position to check
	 * @param y
	 *            Y map position to check
	 */
	private boolean checkBlessedSquare(int x, int y) {
		int pos = gw.getBoloMap().getPos(x, y);
		boolean isBase = gw.getBases().existPos(x, y);
		boolean isPill = gw.getPills().existPos(x, y);

		switch (action) {
		case GameClient.LGM_TREE_REQUEST:
			if (pos == Terrain.BUILDING || pos == Terrain.HALFBUILDING
					|| pos == Terrain.RIVER || pos == Terrain.BOAT
					|| pos == Terrain.DEEP_SEA || isPill == true) {
				return false;
			}
			break;
		case GameClient.LGM_ROAD_REQUEST:
			if (pos == Terrain.BUILDING || pos == Terrain.HALFBUILDING
					|| pos == Terrain.BOAT || pos == Terrain.DEEP_SEA
					|| isPill == true) {
				return false;
			}
			break;
		case GameClient.LGM_BUILDING_REQUEST:
			if (pos == Terrain.BUILDING || pos == Terrain.RIVER
					|| pos == Terrain.BOAT || pos == Terrain.DEEP_SEA
					|| isPill == true) {
				return false;
			}

			break;
		case GameClient.LGM_PILL_REQUEST:
			if (pos == Terrain.BUILDING || pos == Terrain.HALFBUILDING
					|| pos == Terrain.RIVER || pos == Terrain.BOAT
					|| pos == Terrain.DEEP_SEA) {
				return false;
			}
			break;
		case LGM_BOAT_REQUEST:
			if (pos != Terrain.RIVER || isPill == true) {
				return false;
			}
			break;
		default:
			/* Case LGM_REQUEST_MINE */
			if (pos == Terrain.DEEP_SEA || pos == Terrain.RIVER
					|| pos == Terrain.BUILDING || pos == Terrain.BOAT
					|| pos == Terrain.HALFBUILDING || isPill == true
					|| isBase == true) {
				return false;
			}
			break;
		}
		return true;
	}

	/**
	 * LGM is returning towards the tank
	 */
	private void returnToTank() {
		int speed; /* speed of terrain currently on */
		float angle; /* Angle of travel */
		int bmx; /* Man Map co-ords */
		int bmy;
		int newmx;
		int newmy;
		int newbmx;
		int newbmy;
		Point p; /* Add amounts */
		boolean onBoat; /* Is the tank on a boat */
		BoloMap mp = gw.getBoloMap();

		newmx = tank.getX();
		newmy = tank.getY();

		/* Deal with the tank being on a boat */
		onBoat = checkTankBoat(LGM_TANKBOAT_RETURN);

		angle = Utils.CalcAngle(x, y, newmx, newmy);
		if (gw.isServer() == false) {
			gw.getFrontEndDraw().feDrawManStatus(false, angle);
		}

		bmx = getMX();
		bmy = getMY();

		if (tank.getArmour() > Tank.ARMOUR_FULL || newmx == 0 || newmy == 0) {
			speed = 0;
			return;
		} else if ((bmx == blessX && bmy == blessY) || onBoat == true) {
			speed = BoloMap.MAP_MANSPEED_TREFBASE;
		} else {
			speed = mp.getManSpeed(gw.getPills(), gw.getBases(), bmx, bmy,
					playerNum);
		}
		if (speed == 0) {
			/* Do Nothing */

		}

		p = Utils.CalcDistance(angle, speed);
		newmx = x + p.getX();
		newmy = y + p.getY();

		newbmx = Utils.GetMapFromWorld(newmx);
		newbmy = Utils.GetMapFromWorld(newmy);

		if ((mp.getManSpeed(gw.getPills(), gw.getBases(), bmx, newbmy,
				playerNum)) > 0
				|| onBoat == true) {
			y = y + p.getY();
		} else if (bmx == blessX && newbmy == blessY) {
			y = y + p.getY();
		} else {
			// FIXME (*lgman)->obstructed = LGM_BRAIN_PARTIAL;
			newbmy = bmy;
		}
		if ((mp.getManSpeed(gw.getPills(), gw.getBases(), newbmx, newbmy,
				playerNum)) > 0
				|| onBoat == true) {
			x = x + p.getX();
		} else if (newbmx == blessX && newbmy == blessY) {
			x = x + p.getX();
		} else {
			/* Totally obstructed */
			// FIXME (*lgman)->obstructed = LGM_BRAIN_TOTAL;
		}

		/* If we have moved off the blesssed build square its no longer blessed */
		if (getMX() != blessX || getMY() != blessY) {
			blessX = 0;
			blessY = 0;
		}

		/* Check for achieved goal */
		newmx = tank.getX();
		newmy = tank.getY();
		if ((x - newmx) >= LGM_RETURN_MIN_GOAL
				&& (x - newmx) <= LGM_RETURN_MAX_GOAL
				&& (y - newmy) >= LGM_RETURN_MIN_GOAL
				&& (y - newmy) <= LGM_RETURN_MAX_GOAL) {
			/* Arrived back at tank */
			state = LGM_STATE_IDLE;
			inTank = true;
			action = LGM_IDLE;
			blessX = 0;
			blessY = 0;
			backInTank(true);
			if (gw.isServer() == false) {
				gw.getFrontEndDraw().feDrawClearManStatus();
			}
		}
	}

	/**
	 * Man has arrived at his objective. Do whatever he was suppose to do
	 */
	private void doWork() {
		int bmx; /* Dest Map co-ords */
		int bmy;
		boolean isMine; /* Is the item a mine */
		boolean isPill;
		boolean isBase;
		int terrain;
		BoloMap mp = gw.getBoloMap();

		bmx = getMX();
		bmy = getMY();

		isPill = gw.getPills().existPos(bmx, bmy);
		isBase = gw.getBases().existPos(bmx, bmy);

		isMine = false;
		terrain = mp.getPos(bmx, bmy);
		if (terrain >= Terrain.MINE_START && terrain <= Terrain.MINE_END) {
			terrain -= Terrain.MINE_SUBTRACT;
			isMine = true;
		}

		switch (action) {
		case GameClient.LGM_TREE_REQUEST:
			gw.getMinesExp().add(bmx, bmy);
			if (terrain == Terrain.FOREST && isBase == false && isPill == false) {
				if (isMine == true) {
					mp.setPos(bmx, bmy,
							(Terrain.GRASS + Terrain.MINE_SUBTRACT), true,
							false);
					gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, 0, bmx,
							bmy);
				} else {
					mp.setPos(bmx, bmy, Terrain.GRASS, true, false);
				}
				numTrees = LGM_GATHER_TREE;
				gw.getPNB().add(NetChangePNB.NPNB_LGM_FARMTREE, 0, playerNum,
						bmx, bmy);
				gw.getSoundDist().soundDist(SoundDist.FARMING_TREE_NEAR, bmx,
						bmy);
			}
			gw.reCalc();
			break;
		case GameClient.LGM_ROAD_REQUEST:
			/* HUH? minesExpAddItem(mp, bmx, bmy); */
			if (terrain != Terrain.BUILDING && terrain != Terrain.HALFBUILDING
					&& terrain != Terrain.BOAT && terrain != Terrain.DEEP_SEA
					&& isPill == false && isBase == false) {
				if (isMine == true) {
					mp.setPos(bmx, bmy, (Terrain.ROAD + Terrain.MINE_SUBTRACT),
							true, false);
					gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, 0, bmx,
							bmy);
				} else {
					mp.setPos(bmx, bmy, Terrain.ROAD, true, false);
				}
				numTrees = 0;
				gw.getPNB().add(NetChangePNB.NPNB_LGM_BUILDROAD, 0, playerNum,
						bmx, bmy);
				gw.getSoundDist().soundDist(SoundDist.MAN_BUILDING_NEAR, bmx,
						bmy);

				checkRemove(terrain, bmx, bmy);
				gw.reCalc();
			}
			break;
		case GameClient.LGM_BUILDING_REQUEST:
			if (terrain != Terrain.BUILDING && terrain != Terrain.RIVER
					&& terrain != Terrain.BOAT && terrain != Terrain.DEEP_SEA
					&& isPill == false && isBase == false) {
				if (isMine != true) {
					mp.setPos(bmx, bmy, Terrain.BUILDING, true, false);
				} else {
					gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, 0, bmx,
							bmy);
					gw.getMinesExp().add(bmx, bmy);
				}
				numTrees = 0;
				gw.getSoundDist().soundDist(SoundDist.MAN_BUILDING_NEAR, bmx,
						bmy);
				gw.getMNT().add(NetChangeMNT.NMNT_BUILDBUILDING, 0, playerNum,
						bmx, bmy);
				checkRemove(terrain, bmx, bmy);
				gw.reCalc();
			}
			break;
		case LGM_BOAT_REQUEST:
			if (terrain == Terrain.RIVER) {
				mp.setPos(bmx, bmy, Terrain.BOAT, true, false);
				gw.getMNT().add(NetChangeMNT.NMNT_BUILDBOAT, 0, playerNum, bmx,
						bmy);
				numTrees = 0;
				gw.reCalc();
			}
			break;
		case GameClient.LGM_MINE_REQUEST:
			if ((isPill == false && isBase == false)
					&& (terrain == Terrain.SWAMP || terrain == Terrain.CRATER
							|| terrain == Terrain.ROAD
							|| terrain == Terrain.FOREST
							|| terrain == Terrain.RUBBLE || terrain == Terrain.GRASS)) {
				if (isMine == true) {
					gw.getMinesExp().add(bmx, bmy);
					gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, 0, bmx,
							bmy);
				} else {
					gw.getMNT().add(NetChangeMNT.NMNT_MINEPLACE, 0, playerNum,
							bmx, bmy);
					mp.setPos(bmx, bmy, (terrain + Terrain.MINE_SUBTRACT),
							false, false);
					gw.getMines().add(bmx, bmy);
					numMines = 0;
					gw.getSoundDist().soundDist(SoundDist.MAN_LAYING_MINE_NEAR,
							bmx, bmy);
				}
				gw.reCalc();
			}
			break;
		case GameClient.LGM_PILL_REQUEST:
			if (numPills == LGM_NO_PILL) {
				/* Repair pill */
				if (isPill == true) {
					numTrees = 0;
					gw.getPills().repairPos(bmx, bmy);
					gw.getPNB().add(NetChangePNB.NPNB_PILL_REPAIR, 0,
							playerNum, bmx, bmy);
					gw.getSoundDist().soundDist(SoundDist.MAN_BUILDING_NEAR,
							bmx, bmy);
				}
			} else {
				if (isPill == false && isBase == false
						&& gw.getMines().existPos(bmx, bmy) == false
						&& terrain != Terrain.BUILDING
						&& terrain != Terrain.HALFBUILDING
						&& terrain != Terrain.RIVER && terrain != Terrain.BOAT
						&& terrain != Terrain.DEEP_SEA) {
					if (isMine == true) {
						gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, 0,
								bmx, bmy);
						gw.getMinesExp().add(bmx, bmy);
						gw.getFloodFill().add(bmx, bmy);
						deathCheck(Utils.GetWorldFromMap(bmx
								+ BoloMap.MAP_SQUARE_MIDDLE), Utils
								.GetWorldFromMap(bmy
										+ BoloMap.MAP_SQUARE_MIDDLE),
								GameItem.NEUTRAL);
						gw.getSoundDist().soundDist(
								SoundDist.MINE_EXPLOSION_NEAR, bmx, bmy);
						mp.setPos(bmx, bmy, Terrain.CRATER, true, false);
					} else {
						numTrees = 0;
						gw.getPills().set(numPills - 1, bmx, bmy, playerNum,
								Pillbox.PILL_MAX_HEALTH,
								gw.getPills().getAttackSpeed(numPills - 1),
								false, false);
						gw.getPNB().add(NetChangePNB.NPNB_PILL_DROP,
								numPills - 1, playerNum, bmx, bmy);
						gw.getSoundDist().soundDist(
								SoundDist.MAN_BUILDING_NEAR, bmx, bmy);
						if (gw.isServer() == false) {
							gw.getFrontEndDraw().feDrawStatusPillbox(numPills,
									gw.getPillboxStatus(numPills));
						}
						numPills = LGM_NO_PILL;

					}
				}
			}
			checkRemove(terrain, bmx, bmy);
			gw.reCalc();
			break;
		default:
			/* do nothing */
			break;
		}
	}

	/**
	 * An building operation has happened. Check to see if it should remove from
	 * grass/building data structures.
	 * 
	 * @param terrain
	 *            Terrain of the square
	 * @param x
	 *            X position of the square
	 * @param y
	 *            Y position of the square
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
		default:
			/* Do nothing */
			break;
		}
	}

	/**
	 * Man has arrived back in tank. Drop stuff off
	 * 
	 * @param sendItems
	 *            If TRUE, send items back to client
	 */
	private void backInTank(boolean sendItems) {
		int trees;
		int minesAmount;
		int pillNum;

		minesAmount = trees = 0;
		pillNum = LGM_NO_PILL;
		if (numTrees > 0) {
			tank.giveTrees(numTrees);
			trees = numTrees;
			numTrees = 0;
		}
		if (numPills != LGM_NO_PILL) {
			tank.putPill(numPills);
			pillNum = numPills;
			numPills = LGM_NO_PILL;
		}
		if (numMines > 0) {
			minesAmount = numMines;
			tank.addArmour(numMines);
			minesAmount = numMines;
			numMines = 0;
		}

		if (sendItems == false) {
			trees = 0;
			minesAmount = 0;
			pillNum = LGM_NO_PILL;
		}
		// Added
		if (gw.isServer() == true) {
			// FIXME serverNetSendManReturnMessge(screenGetTankPlayer(tnk),
			// trees, minesAmount, pillNum);
		} else {
			if (nextAction != LGM_IDLE) {
				if (true /* FIXME FIXME FIXME netGetType() == netSingle */) {
					newPrimaryRequest(nextX, nextY, nextAction);
				} else {
					/* Network game */
					/*
					 * FIXME BYTE pillNum; bool isMine; BYTE trees; BYTE
					 * minesAmount; bool ok; BYTE action; action =
					 * (*lgman)->nextAction; ok = lgmCheckNewRequest(lgman, mp,
					 * pb, bs, tnk, (*lgman)->nextX, (*lgman)->nextY, &action,
					 * &pillNum, &isMine, &trees, &minesAmount, FALSE);
					 * (*lgman)->numTrees = trees; (*lgman)->numMines =
					 * minesAmount;
					 * 
					 * if (ok) { netMNTAdd(screenGetNetMnt(), NMNT_MANACTION,
					 * action, (*lgman)->playerNum, (*lgman)->nextX,
					 * (*lgman)->nextY); (*lgman)->action = action; }
					 */
				}
				nextAction = LGM_IDLE;
			}
		}
	}
}
