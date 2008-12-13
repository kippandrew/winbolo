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
 * BoloCursor.java
 *
 * Created on January 1, 2004, 8:30 PM
 */

package com.jbolo.gui;

import java.awt.*;
import java.awt.event.*;
import com.jbolo.bolo.GameClient;

/**
 * Handles the cursor movement and clicky clicky for the game.
 * 
 * @author John Morrison
 */
public class BoloCursor implements MouseListener, MouseMotionListener {
	/** We aren't selecting anything */
	private static int NO_SELECT = -1;

	/** The existing window cursor to change back to */
	private Cursor normalCursor;
	/** The bolo cursor */
	private Cursor crosshairCursor;
	/** The window we work on */
	private Frame window;
	/**
	 * Is the cursor in the main view and needs to be changed to the crosshair
	 * cursor
	 */
	private boolean inMainView;
	/** The current building item select */
	private int currentSelect;
	/** The drawing object to use to draw indents */
	private Draw draw;
	/** Game Client for this cursor */
	private GameClient gc;

	/**
	 * Creates a new instance of BoloCursor
	 * 
	 * @param frame
	 *            Frame this cursor belongs to
	 * @param draw
	 *            Drawing class used to draw indents
	 * @param gc
	 *            GameClient used to send build requests to
	 */
	public BoloCursor(Frame frame, Draw draw, GameClient gc) {
		this.window = frame;
		this.draw = draw;
		this.gc = gc;
		normalCursor = frame.getCursor();
		crosshairCursor = new Cursor(Cursor.CROSSHAIR_CURSOR);
		inMainView = false;
		currentSelect = GameClient.LGM_TREE_REQUEST;
	}

	/**
	 * Sets the cursor.
	 * 
	 * @param isInView
	 *            Is it the Bolo cursor or the normal one, If true then its the
	 *            bolo one
	 */
	public void set(boolean isInView) {
		if (isInView == false) {
			window.setCursor(normalCursor);
		} else {
			window.setCursor(crosshairCursor);
		}
	}

	/**
	 * Finalise method overridden to reset the cursor back to the normal cursor
	 */
	protected void finalize() {
		window.setCursor(normalCursor);
	}

	/**
	 * Checks to see if this mouse position is in the main view or not
	 * 
	 * @param p
	 *            The mouse point to check
	 * @return If the mouse is in the main view
	 */
	private boolean isInMainView(Point p) {
		int zoomFactor = draw.getZoomFactor();
		int x = (int) p.getX();
		int y = (int) p.getY();
		int left;
		int right;
		int top;
		int bottom;
		int xSquare;
		int ySquare;

		left = zoomFactor * DrawPositions.MAIN_OFFSET_X;
		right = left
				+ (zoomFactor * DrawPositions.TILE_SIZE_X * GameClient.MAIN_SCREEN_SIZE_X);
		top = zoomFactor * DrawPositions.MAIN_OFFSET_Y;
		bottom = top
				+ (zoomFactor * DrawPositions.TILE_SIZE_Y * GameClient.MAIN_SCREEN_SIZE_Y);

		if (x >= left && x <= right && y >= top && y <= bottom) {
			/* Sent to game world */
			x -= zoomFactor * DrawPositions.MAIN_OFFSET_X;
			y -= zoomFactor * DrawPositions.MAIN_OFFSET_Y;
			xSquare = 1 + (x / (zoomFactor * (GameClient.MAIN_SCREEN_SIZE_X + 1)));
			ySquare = 1 + (y / (zoomFactor * (GameClient.MAIN_SCREEN_SIZE_Y + 1)));
			gc.setCursorPos(xSquare, ySquare);
			return true;
		}
		return false;
	}

	/**
	 * Gets the current building select
	 * 
	 * @return the current building select
	 */
	public int getCurrentSelect() {
		return currentSelect;
	}

	/**
	 * Gets the building select from the mouse position
	 * 
	 * @param p
	 *            The mouse poisition point
	 * @return the building select or NO_SELECT if none selected
	 */
	private int getSelect(Point p) {
		int newSelect = NO_SELECT;
		int zoomFactor = draw.getZoomFactor();

		int xPos = (int) p.getX();
		int yPos = (int) p.getY();

		if (xPos >= (zoomFactor * DrawPositions.BS_TREE_OFFSET_X)
				&& xPos <= ((zoomFactor * DrawPositions.BS_TREE_OFFSET_X) + (zoomFactor * DrawPositions.BS_ITEM_SIZE_X))
				&& yPos >= (zoomFactor * DrawPositions.BS_TREE_OFFSET_Y)
				&& yPos <= ((zoomFactor * DrawPositions.BS_TREE_OFFSET_Y) + (zoomFactor * DrawPositions.BS_ITEM_SIZE_Y))) {
			newSelect = GameClient.LGM_TREE_REQUEST;
		} else if (xPos >= (zoomFactor * DrawPositions.BS_ROAD_OFFSET_X)
				&& xPos <= ((zoomFactor * DrawPositions.BS_ROAD_OFFSET_X) + (zoomFactor * DrawPositions.BS_ITEM_SIZE_X))
				&& yPos >= (zoomFactor * DrawPositions.BS_ROAD_OFFSET_Y)
				&& yPos <= ((zoomFactor * DrawPositions.BS_ROAD_OFFSET_Y) + (zoomFactor * DrawPositions.BS_ITEM_SIZE_Y))) {
			newSelect = GameClient.LGM_ROAD_REQUEST;
		} else if (xPos >= (zoomFactor * DrawPositions.BS_BUILDING_OFFSET_X)
				&& xPos <= ((zoomFactor * DrawPositions.BS_BUILDING_OFFSET_X) + (zoomFactor * DrawPositions.BS_ITEM_SIZE_X))
				&& yPos >= (zoomFactor * DrawPositions.BS_BUILDING_OFFSET_Y)
				&& yPos <= ((zoomFactor * DrawPositions.BS_BUILDING_OFFSET_Y) + (zoomFactor * DrawPositions.BS_ITEM_SIZE_Y))) {
			newSelect = GameClient.LGM_BUILDING_REQUEST;
		} else if (xPos >= (zoomFactor * DrawPositions.BS_PILLBOX_OFFSET_X)
				&& xPos <= ((zoomFactor * DrawPositions.BS_PILLBOX_OFFSET_X) + (zoomFactor * DrawPositions.BS_ITEM_SIZE_X))
				&& yPos >= (zoomFactor * DrawPositions.BS_PILLBOX_OFFSET_Y)
				&& yPos <= ((zoomFactor * DrawPositions.BS_PILLBOX_OFFSET_Y) + (zoomFactor * DrawPositions.BS_ITEM_SIZE_Y))) {
			newSelect = GameClient.LGM_PILL_REQUEST;
		} else if (xPos >= (zoomFactor * DrawPositions.BS_MINE_OFFSET_X)
				&& xPos <= ((zoomFactor * DrawPositions.BS_MINE_OFFSET_X) + (zoomFactor * DrawPositions.BS_ITEM_SIZE_X))
				&& yPos >= (zoomFactor * DrawPositions.BS_MINE_OFFSET_Y)
				&& yPos <= ((zoomFactor * DrawPositions.BS_MINE_OFFSET_Y) + (zoomFactor * DrawPositions.BS_ITEM_SIZE_Y))) {
			newSelect = GameClient.LGM_MINE_REQUEST;
		}
		return newSelect;
	}

	/**
	 * The mouse has been clicked on the canvas. Process build select changes
	 * and build requests
	 * 
	 * @param mouseEvent
	 *            The mouseEvent that occured
	 */
	public void mouseClicked(java.awt.event.MouseEvent mouseEvent) {
		Point p;
		int newSelect;
		if (this.window.isActive() == true) {
			p = mouseEvent.getPoint();

			if (isInMainView(p) == true) {
				gc.buildRequest(currentSelect);
			} else {
				newSelect = getSelect(p);
				if (newSelect != NO_SELECT && newSelect != this.currentSelect) {
					draw.drawSelectIndentsOff(currentSelect);
					draw.drawSelectIndentsOn(newSelect);
					currentSelect = newSelect;
				}
			}
		}
	}

	/**
	 * The mouse has been dragged on the canvas. Process it here
	 * 
	 * @param mouseEvent
	 *            The mouseEvent that occured
	 */
	public void mouseDragged(java.awt.event.MouseEvent mouseEvent) {
	}

	/**
	 * The mouse has entered the canvas. Process it here
	 * 
	 * @param mouseEvent
	 *            The mouseEvent that occured
	 */
	public void mouseEntered(java.awt.event.MouseEvent mouseEvent) {
	}

	/**
	 * The mouse has exited the canvas. Process it here
	 * 
	 * @param mouseEvent
	 *            The mouseEvent that occured
	 */
	public void mouseExited(java.awt.event.MouseEvent mouseEvent) {
	}

	/**
	 * The mouse has moved on the canvas. Process changing the cursor
	 * 
	 * @param mouseEvent
	 *            The mouseEvent that occured
	 */
	public void mouseMoved(java.awt.event.MouseEvent mouseEvent) {
		Point p;
		boolean newView;
		if (window.isActive() == true) {
			p = mouseEvent.getPoint();
			newView = isInMainView(p);
			if (newView != inMainView) {
				set(newView);
				inMainView = newView;
			}
		}

	}

	/**
	 * The mouse has been pressed dwn on the canvas. This is ignored.
	 * 
	 * @param mouseEvent
	 *            The mouseEvent that occured
	 */
	public void mousePressed(java.awt.event.MouseEvent mouseEvent) {
	}

	/**
	 * The mouse has been released on the canvas. This is ignored.
	 * 
	 * @param mouseEvent
	 *            The mouseEvent that occured
	 */
	public void mouseReleased(java.awt.event.MouseEvent mouseEvent) {
	}

}
