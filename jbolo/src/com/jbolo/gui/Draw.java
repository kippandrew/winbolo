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
 * Draw.java
 *
 * Created on October 5, 2003, 9:57 PM
 * @author John Morrison
 */

package com.jbolo.gui;

import java.awt.*;
import java.applet.*;
import java.awt.image.*;
import java.io.*;
import java.net.URL;
import java.util.ArrayList;
import java.util.ListIterator;

import com.jbolo.bolo.GameWorld;
import com.jbolo.bolo.TileNum;
import com.jbolo.bolo.GameClient;
import com.jbolo.bolo.GameItem;
import com.jbolo.bolo.ScreenTankItem;
import com.jbolo.bolo.ScreenItem;
import com.jbolo.bolo.Point;
import com.jbolo.bolo.Utils;

/**
 * Draw handles drawing item on the main screen
 * 
 * @author John Morrison
 */
public class Draw implements FrontEndDraw {

	/**
	 * A bufferedImage containing all the graphics located on the loaded file.
	 */
	private BufferedImage tiles;

	/** An Image containing the background image. */
	private Image background;

	/**
	 * Backbuffer used for double buffering
	 */
	private BufferedImage backbuffer; /* Tried VolatileImage */

	/** Backbuffers graphics item */
	private Graphics2D backbufferGraphics;

	/** Map backbuffer */
	private BufferedImage mapBuffer;
	/** Map backbuffer graphics */
	private Graphics2D mapBufferGraphics;

	/**
	 * Array of X co-ordinates of tiles located in the tile file. The positions
	 * are derived from DrawPositions and the indexes from TileNum.
	 * 
	 * @see com.jbolo.bolo.TileNum
	 * @see com.jbolo.gui.DrawPositions
	 */
	private int[] drawPosX;
	/**
	 * Array of Y co-ordinates of tiles located in the tile file. The positions
	 * are derived from DrawPositions and the indexes from TileNum.
	 * 
	 * @see com.jbolo.bolo.TileNum
	 * @see com.jbolo.gui.DrawPositions
	 */
	private int[] drawPosY;
	/**
	 * Internal Array used for caching drawing items. Contains the last known
	 * graphic at this position. If it is the same we do not need to redraw this
	 * tile on the backbuffer.
	 */
	private int[][] last;
	/**
	 * An array containing all the tiles as BufferedImages. Used to speedup draw
	 * positions. Indexed by TileNum
	 * 
	 * @see com.jbolo.bolo.TileNum
	 */
	private BufferedImage[] tileImg;

	/** The mines images with a transparent background */
	private Image minesImg;

	/** Image scaling factor */
	private int zoomFactor;

	/** The Base status subwindow */
	private BufferedImage baseStatusImage;

	/** The Base status subwindow */
	private BufferedImage pillsStatusImage;

	/** The Tanks status subwindow */
	private BufferedImage tanksStatusImage;

	/** The LGM status subwindow */
	private BufferedImage lgmStatusImage;

	/** Array of transparent tank images to draw */
	private Image[] tankImages;

	/** Array of transparent LGM images to draw */
	private Image[] lgmImages;

	/** Array of transparent shell iamges to draw */
	private Image[] shellImages;

	/** The tanks gunsight image */
	private Image gunsightImage;

	/** Last amount of base stock shells */
	private int lastBaseShells;
	/** Last amount of base stock mines */
	private int lastBaseMines;
	/** Last amount of base stock armour */
	private int lastBaseArmour;

	/** Build Select Indents on Image */
	private Image indentsOnImage;

	/** Build Select Indents off Image */
	private Image indentsOffImage;

	/** Image for cursor rectangle */
	private Image cursorImage;

	/** Font used to draw text */
	private Font courierFont;

	/** GraphicsInterface used to get the Graphics2D object as required */
	private GraphicsInterface gi;

	/** Old angle that the LGM is travelling at */
	// private float oldLgmAngle;

	/**
	 * Creates a new instance of Draw
	 * 
	 * @param gi
	 *            Interface for the graphics object
	 * @param zoomFactor
	 *            scaling factor for graphics
	 * @throws Exception
	 *             on setup error
	 */
	public Draw(GraphicsInterface gi, int zoomFactor) throws Exception {
		Image loadImage = null;
		URL url;
		int zf = 1; // Zoom Factor
		int count;
		int count2;
		int iw;
		int ih;
		Graphics2D g2d = gi.getGraphicsObject();
		this.gi = gi;
		this.zoomFactor = zoomFactor;
		InputStream inputStream;

		// oldLgmAngle = 0;
		lastBaseShells = 0;
		lastBaseMines = 0;
		lastBaseArmour = 0;
		drawPosX = new int[256];
		drawPosY = new int[256];
		tileImg = new BufferedImage[256];
		tankImages = new Image[96];
		lgmImages = new Image[4];
		shellImages = new Image[25];

		last = new int[18][18];
		for (count = 0; count < 18; count++) {
			for (count2 = 0; count2 < 18; count2++) {
				last[count][count2] = 1000;
			}
		}
		for (count = 0; count < 256; count++) {
			tileImg[count] = null;
		}

		courierFont = new Font("Courier", Font.PLAIN, 12);

		setupArrays(1);

		backbuffer = new BufferedImage(zf * GameClient.MAIN_BACK_BUFFER_SIZE_X
				* DrawPositions.TILE_SIZE_X, zf
				* GameClient.MAIN_BACK_BUFFER_SIZE_X
				* DrawPositions.TILE_SIZE_Y, BufferedImage.TYPE_INT_ARGB);
		// GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice().getDefaultConfiguration().createCompatibleVolatileImage(zf
		// * GameClient.MAIN_SCREEN_SIZE_X * DrawPositions.TILE_SIZE_X, zf *
		// GameClient.MAIN_SCREEN_SIZE_Y * DrawPositions.TILE_SIZE_Y);
		backbufferGraphics = (Graphics2D) backbuffer.getGraphics();
		mapBuffer = new BufferedImage(zf * GameClient.MAIN_BACK_BUFFER_SIZE_Y
				* DrawPositions.TILE_SIZE_X, zf
				* GameClient.MAIN_BACK_BUFFER_SIZE_Y
				* DrawPositions.TILE_SIZE_Y, BufferedImage.TYPE_INT_ARGB);
		// GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice().getDefaultConfiguration().createCompatibleVolatileImage(zf
		// * GameClient.MAIN_SCREEN_SIZE_X * DrawPositions.TILE_SIZE_X, zf *
		// GameClient.MAIN_SCREEN_SIZE_Y * DrawPositions.TILE_SIZE_Y);
		mapBufferGraphics = (Graphics2D) mapBuffer.getGraphics();

		try {
			/* Try to load the tile file */
			System.err.print("Loading tile file...");
			inputStream = Skin.GetInstance().getSkin();
			if (inputStream == null) {
				url = new URL("file:///c:/jbolo/tiles.bmp");
				inputStream = url.openStream();
			}
			loadImage = BMPLoader.Read(inputStream);
			/* Copy to the buffer */
			iw = loadImage.getWidth(null);
			ih = loadImage.getHeight(null);
			tiles = new BufferedImage(iw, ih, BufferedImage.TYPE_INT_ARGB);
			Graphics2D big = tiles.createGraphics();

			big.drawImage(loadImage, 0, 0, null);
			/* Try to load the baclground file */
			System.err.print("done\nLoading background image...");
			inputStream = Skin.GetInstance().getBackground();
			if (inputStream == null) {
				url = new URL("file:///c:/jbolo/background.bmp");
				inputStream = url.openStream();
			}
			background = BMPLoader.Read(inputStream);

			System.err.println("done");
			System.err.print("Loading images");
			setupTiles(zoomFactor);
			minesImg = tiles.getSubimage(DrawPositions.MINE_X * zoomFactor,
					DrawPositions.MINE_Y * zoomFactor, zoomFactor
							* DrawPositions.TILE_SIZE_X, zoomFactor
							* DrawPositions.TILE_SIZE_Y);
			minesImg = applyMask(g2d, minesImg, Color.GREEN);
			setupTankImages(g2d);
			setupLgmImages(g2d);
			setupShellImages(g2d);
			gunsightImage = setupGunsightImage(g2d);
			/* Indents images */
			indentsOffImage = tiles.getSubimage(zoomFactor
					* DrawPositions.INDENT_OFF_X, zoomFactor
					* DrawPositions.INDENT_OFF_Y, zoomFactor
					* DrawPositions.BS_ITEM_SIZE_X, zoomFactor
					* DrawPositions.BS_ITEM_SIZE_Y);
			indentsOffImage = applyMask(g2d, indentsOffImage, Color.GREEN);
			indentsOnImage = tiles.getSubimage(zoomFactor
					* DrawPositions.INDENT_ON_X, zoomFactor
					* DrawPositions.INDENT_ON_Y, zoomFactor
					* DrawPositions.BS_ITEM_SIZE_X, zoomFactor
					* DrawPositions.BS_ITEM_SIZE_Y);
			indentsOnImage = applyMask(g2d, indentsOnImage, Color.GREEN);
			cursorImage = tiles.getSubimage(DrawPositions.MOUSE_SQUARE_X
					* zoomFactor, DrawPositions.MOUSE_SQUARE_Y * zoomFactor,
					zoomFactor * DrawPositions.TILE_SIZE_X, zoomFactor
							* DrawPositions.TILE_SIZE_Y);
			cursorImage = applyMask(g2d, cursorImage, Color.GREEN);
			big.dispose();

			/* Create the base/Pill/Tanks status image */
			baseStatusImage = new BufferedImage(zoomFactor
					* DrawPositions.STATUS_BASES_WIDTH, zoomFactor
					* DrawPositions.STATUS_BASES_HEIGHT,
					BufferedImage.TYPE_INT_ARGB);
			fillImage(baseStatusImage, Color.BLACK);
			copyStatusIcon(baseStatusImage, tileImg[TileNum.BASE_GOOD],
					(zoomFactor * DrawPositions.STATUS_BASES_MIDDLE_ICON_X),
					(zoomFactor * DrawPositions.STATUS_BASES_MIDDLE_ICON_Y));
			pillsStatusImage = new BufferedImage(zoomFactor
					* DrawPositions.STATUS_BASES_WIDTH, zoomFactor
					* DrawPositions.STATUS_BASES_HEIGHT,
					BufferedImage.TYPE_INT_ARGB);
			fillImage(pillsStatusImage, Color.BLACK);
			copyStatusIcon(pillsStatusImage, tileImg[TileNum.PILL_EVIL_15],
					(zoomFactor * DrawPositions.STATUS_PILLS_MIDDLE_ICON_X),
					(zoomFactor * DrawPositions.STATUS_PILLS_MIDDLE_ICON_Y));
			tanksStatusImage = new BufferedImage(zoomFactor
					* DrawPositions.STATUS_TANKS_WIDTH, zoomFactor
					* DrawPositions.STATUS_TANKS_HEIGHT,
					BufferedImage.TYPE_INT_ARGB);
			fillImage(tanksStatusImage, Color.BLACK);
			copyStatusIcon(tanksStatusImage, this.tankImages[0],
					(zoomFactor * DrawPositions.STATUS_TANKS_MIDDLE_ICON_X),
					(zoomFactor * DrawPositions.STATUS_TANKS_MIDDLE_ICON_Y));

			/* Setup LGM image */
			lgmStatusImage = new BufferedImage(zoomFactor
					* DrawPositions.MAN_STATUS_WIDTH, zoomFactor
					* DrawPositions.MAN_STATUS_HEIGHT,
					BufferedImage.TYPE_INT_ARGB);
			fillImage(lgmStatusImage, Color.BLACK);
			System.err.println("Done");
		} catch (Exception e) {
			e.printStackTrace();
			throw new Exception(e.getMessage());
		}
	}

	/**
	 * Gets the draw zoom factor
	 * 
	 * @return the drawing scaling factor
	 */
	public int getZoomFactor() {
		return zoomFactor;
	}

	/**
	 * redrawAll retrays the entire screen
	 * 
	 * @param gw
	 *            Game client to get data from
	 * @param graphics
	 *            Graphics drawing object
	 */
	public void redrawAll(Graphics2D graphics, GameClient gw) {
		int count;
		int total;
		int kills;
		int deaths;

		graphics.drawImage(background, 0, 0, DrawPositions.MAIN_WINDOW_SIZE_X,
				DrawPositions.MAIN_WINDOW_SIZE_Y, null);

		/* Bases */
		fillImage(baseStatusImage, Color.BLACK);
		copyStatusIcon(baseStatusImage, tileImg[TileNum.BASE_GOOD],
				(zoomFactor * DrawPositions.STATUS_BASES_MIDDLE_ICON_X),
				(zoomFactor * DrawPositions.STATUS_BASES_MIDDLE_ICON_Y));
		total = gw.getNumBases();
		count = 0;

		while (count < total) {
			drawStatusBase(count, gw.getBaseStatus(count));
			count++;
		}
		drawCopyStatusImage(graphics, baseStatusImage,
				DrawPositions.STATUS_BASES_LEFT, DrawPositions.STATUS_BASES_TOP);
		/* Pills */
		fillImage(pillsStatusImage, Color.BLACK);
		copyStatusIcon(pillsStatusImage, tileImg[TileNum.PILL_EVIL_15],
				(zoomFactor * DrawPositions.STATUS_PILLS_MIDDLE_ICON_X),
				(zoomFactor * DrawPositions.STATUS_PILLS_MIDDLE_ICON_Y));
		total = gw.getNumPillboxes();
		count = 0;
		while (count < total) {
			drawStatusPillbox(count, gw.getPillboxStatus(count));
			count++;
		}
		drawCopyStatusImage(graphics, pillsStatusImage,
				DrawPositions.STATUS_PILLS_LEFT, DrawPositions.STATUS_PILLS_TOP);

		/* Tanks */
		count = 0;
		fillImage(tanksStatusImage, Color.BLACK);
		copyStatusIcon(tanksStatusImage, this.tankImages[0],
				(zoomFactor * DrawPositions.STATUS_TANKS_MIDDLE_ICON_X),
				(zoomFactor * DrawPositions.STATUS_TANKS_MIDDLE_ICON_Y));

		while (count < GameWorld.MAX_TANKS) {
			drawStatusTank(count, gw.getTankStatus(count));
			count++;
		}
		drawCopyStatusImage(graphics, tanksStatusImage,
				DrawPositions.STATUS_TANKS_LEFT, DrawPositions.STATUS_TANKS_TOP);
		feDrawStatusBaseBars(0, 0, 0, true);
		com.jbolo.bolo.Tank tank = gw.getTank();
		feDrawStatusTankBars(tank.getShells(), tank.getMines(), tank
				.getArmour(), tank.getTrees());

		feWriteKillsDeaths(gw.getTankKills(), gw.getTankDeaths());
	}

	/**
	 * Redraws the main graphics window
	 * 
	 * @param graphics
	 *            Graphics drawing object
	 * @param view
	 *            multidensional array of tiles to draw
	 * @param mines
	 *            multidensional array of whether the tiles contain mines
	 * @param tanks
	 *            ArrayList of ScreenTankItems to draw
	 * @param shells
	 *            ArrayList of ScreenItems to draw for shells
	 * @param lgms
	 *            ArrayList of ScreenItems to draw for lgms
	 * @param ScreenItem
	 *            containing gunsight point or null if not on screen
	 * @param cursor
	 *            Cursor position on the screen or null if cursor is not on the
	 *            screen
	 * @param srtDelay
	 *            Start delay in ticks -1 for none
	 */
	public void feDrawMainView(int[][] view, boolean[][] mines,
			ArrayList<ScreenTankItem> tanks, ArrayList<ScreenItem> shells, ArrayList<ScreenTankItem> lgms,
			ScreenItem gunsight, Point cursor, long srtDelay) {
		int x;
		int y;
		int pos;
		int count;
		boolean done;
		boolean isPill;
		boolean isBase;
		int labelNum;
		boolean shouldDraw;
		Graphics2D graphics;
		// long startTime = System.currentTimeMillis();
		long newTime;
		long newTime2;
		count = 0;
		x = 0;
		y = 0;
		done = false;

		while (done == false) {
			pos = view[x][y];

			shouldDraw = false;
			if (last[x][y] == 10000) {
				shouldDraw = true;
			} else if (mines[x][y] == true) {
				if (last[x][y] != (0 - pos)) {
					shouldDraw = true;
				}
			} else if (last[x][y] != pos) {
				shouldDraw = true;
			}
			if (shouldDraw == true) {
				last[x][y] = pos;

				isPill = false;
				isBase = false;

				if (pos == TileNum.PILL_EVIL_15 || pos == TileNum.PILL_EVIL_14
						|| pos == TileNum.PILL_EVIL_13
						|| pos == TileNum.PILL_EVIL_12
						|| pos == TileNum.PILL_EVIL_11
						|| pos == TileNum.PILL_EVIL_10
						|| pos == TileNum.PILL_EVIL_9
						|| pos == TileNum.PILL_EVIL_8
						|| pos == TileNum.PILL_EVIL_7
						|| pos == TileNum.PILL_EVIL_6
						|| pos == TileNum.PILL_EVIL_5
						|| pos == TileNum.PILL_EVIL_4
						|| pos == TileNum.PILL_EVIL_3
						|| pos == TileNum.PILL_EVIL_2
						|| pos == TileNum.PILL_EVIL_1
						|| pos == TileNum.PILL_EVIL_0) {
					isPill = true;
				}
				if (pos == TileNum.PILL_GOOD_15 || pos == TileNum.PILL_GOOD_14
						|| pos == TileNum.PILL_GOOD_13
						|| pos == TileNum.PILL_GOOD_12
						|| pos == TileNum.PILL_GOOD_11
						|| pos == TileNum.PILL_GOOD_10
						|| pos == TileNum.PILL_GOOD_9
						|| pos == TileNum.PILL_GOOD_8
						|| pos == TileNum.PILL_GOOD_7
						|| pos == TileNum.PILL_GOOD_6
						|| pos == TileNum.PILL_GOOD_5
						|| pos == TileNum.PILL_GOOD_4
						|| pos == TileNum.PILL_GOOD_3
						|| pos == TileNum.PILL_GOOD_2
						|| pos == TileNum.PILL_GOOD_1
						|| pos == TileNum.PILL_GOOD_0) {
					isPill = true;
				}
				if (pos == TileNum.BASE_GOOD || pos == TileNum.BASE_NEUTRAL
						|| pos == TileNum.BASE_EVIL) {
					isBase = true;
				}

				try {
					mapBufferGraphics.drawImage(tileImg[pos], x
							* DrawPositions.TILE_SIZE_X, y
							* DrawPositions.TILE_SIZE_Y,
							DrawPositions.TILE_SIZE_X,
							DrawPositions.TILE_SIZE_Y, null);
				} catch (Exception e) {
					 e.printStackTrace();
				}
				if (mines[x][y] == true) {
					last[x][y] = 0 - pos;
					mapBufferGraphics.drawImage(minesImg, x
							* DrawPositions.TILE_SIZE_X, y
							* DrawPositions.TILE_SIZE_Y, null);
				}
			}
			/* Increment the variable */
			x++;
			if (x == GameClient.MAIN_BACK_BUFFER_SIZE_X) {
				y++;
				x = 0;
				if (y == GameClient.MAIN_BACK_BUFFER_SIZE_Y) {
					done = true;
				}
			}
		}
		// System.err.println("Map took: " + (System.currentTimeMillis() -
		// startTime));
		backbufferGraphics.drawImage(mapBuffer, 0, 0, null);
		// newTime = System.currentTimeMillis();
		// System.err.println("Copy took: " + (System.currentTimeMillis() -
		// startTime));
		// newTime = System.currentTimeMillis();

		drawShells(backbufferGraphics, shells);
		drawTanks(backbufferGraphics, tanks);
		drawLgms(backbufferGraphics, lgms);

		drawGunsight(backbufferGraphics, gunsight);
		// Draw cursor if required
		if (cursor != null) {
			backbufferGraphics.drawImage(cursorImage, zoomFactor
					* cursor.getX() * DrawPositions.TILE_SIZE_X, zoomFactor
					* cursor.getY() * DrawPositions.TILE_SIZE_Y, null);
		}
		// System.err.println("Item took: " + (System.currentTimeMillis() -
		// startTime));

		graphics = gi.getGraphicsObject();
		// newTime = System.currentTimeMillis();
		graphics.drawImage(backbuffer,
				zoomFactor * DrawPositions.MAIN_OFFSET_X, zoomFactor
						* DrawPositions.MAIN_OFFSET_Y, zoomFactor
						* DrawPositions.MAIN_OFFSET_X + zoomFactor
						* GameClient.MAIN_SCREEN_SIZE_X
						* DrawPositions.TILE_SIZE_X, zoomFactor
						* DrawPositions.MAIN_OFFSET_Y + zoomFactor
						* GameClient.MAIN_SCREEN_SIZE_Y
						* DrawPositions.TILE_SIZE_Y, zoomFactor
						* DrawPositions.TILE_SIZE_X, zoomFactor
						* DrawPositions.TILE_SIZE_Y, zoomFactor
						* GameClient.MAIN_SCREEN_SIZE_X
						* DrawPositions.TILE_SIZE_X + zoomFactor
						* DrawPositions.TILE_SIZE_X, zoomFactor
						* GameClient.MAIN_SCREEN_SIZE_Y
						* DrawPositions.TILE_SIZE_Y + zoomFactor
						* DrawPositions.TILE_SIZE_Y, null);

		// if (drawCount > 8) {
		// System.err.println("Had to draw " + drawCount + " items");
		// }
		graphics.dispose();
		// System.err.println("Fron took: " + (System.currentTimeMillis() -
		// newTime));
		// System.err.println("Tota took: " + (System.currentTimeMillis() -
		// startTime));

	}

	/**
	 * MarkRedraw marks this screen tile as requireing redraw on the next frame
	 * 
	 * @param si
	 *            The ScreenItem to check
	 * @param itemSize
	 *            The size of the item to determine if other tiles need redraw
	 */
	private void markRedraw(ScreenItem si, int itemSize) {
		/*
		 * try { last[si.getMX()][si.getMY()] = 10000; if (si.getPX() > itemSize &&
		 * si.getMX() < GameClient.MAIN_SCREEN_SIZE_X) {
		 * last[si.getMX()+1][si.getMY()] = 10000; if (si.getMY() <
		 * GameClient.MAIN_SCREEN_SIZE_Y) { last[si.getMX()+1][si.getMY()+1] =
		 * 10000; }
		 *  } if (si.getPY() > itemSize && si.getMY() <
		 * GameClient.MAIN_SCREEN_SIZE_Y) { last[si.getMX()][si.getMY()+1] =
		 * 10000; if (si.getMX() < GameClient.MAIN_SCREEN_SIZE_X) {
		 * last[si.getMX()+1][si.getMY()+1] = 10000; } } } catch (Exception e) {
		 * e.printStackTrace(); System.err.println("MX: " + si.getMX() + "MY: " +
		 * si.getMY() + " PX: " + si.getPX() + " PY: " + si.getPY()); }
		 */
	}

	/**
	 * Daws the gungsight on the screen
	 * 
	 * @param graphics
	 *            Graphics2D to draw to
	 * @param si
	 *            Gunsight or null for no gunsight
	 */
	private void drawGunsight(Graphics2D graphics, ScreenItem si) {
		int x;
		int y;

		if (si != null) {
			x = (si.getMX() * DrawPositions.TILE_SIZE_X * zoomFactor)
					+ (si.getPX() * zoomFactor);
			y = (si.getMY() * DrawPositions.TILE_SIZE_Y * zoomFactor)
					+ (si.getPY() * zoomFactor);
			graphics.drawImage(gunsightImage, x, y, null);
			markRedraw(si, 0);
		}
	}

	/**
	 * Draws shells and explosions onto the back buffer
	 * 
	 * @param graphics
	 *            Graphics drawing object
	 * @param shells
	 *            List of shells to draw
	 */
	private void drawShells(Graphics2D graphics, ArrayList shells) {
		ListIterator li;
		ScreenItem si;

		li = shells.listIterator();
		while (li.hasNext()) {
			si = (ScreenItem) li.next();
			drawImage(graphics, shellImages[si.getFrame()], si.getMX(), si
					.getMY(), si.getPX(), si.getPY());
			if (si.getFrame() >= TileNum.SHELL_EXPLOSION8
					&& si.getFrame() <= TileNum.SHELL_EXPLOSION1) {
				markRedraw(si, 0);
			} else {
				markRedraw(si, 10);
			}

		}
	}

	private void drawImage(Graphics2D graphics, Image image, int mx, int my,
			int px, int py) {
		int x;
		int y;
		x = (mx * DrawPositions.TILE_SIZE_X * zoomFactor) + (px * zoomFactor);
		y = (my * DrawPositions.TILE_SIZE_Y * zoomFactor) + (py * zoomFactor);
		graphics.drawImage(image, x, y, null);

	}

	/**
	 * Draws tanks onto the back buffer
	 * 
	 * @param graphics
	 *            Graphics drawing object
	 * @param tanks
	 *            List of tanks to draw
	 */
	private void drawTanks(Graphics2D graphics, ArrayList tanks) {
		ListIterator li;
		ScreenTankItem st;

		li = tanks.listIterator();
		while (li.hasNext()) {
			st = (ScreenTankItem) li.next();
			drawImage(graphics, tankImages[st.getFrame()], st.getMX(), st
					.getMY(), st.getPX(), st.getPY());
			markRedraw((ScreenItem) st, 0);
		}
	}

	/**
	 * Draws lgms onto the back buffer
	 * 
	 * @param graphics
	 *            Graphics drawing object
	 * @param lgms
	 *            List of LGMs to draw
	 */
	private void drawLgms(Graphics2D graphics, ArrayList lgms) {
		ListIterator li;
		ScreenItem si;

		li = lgms.listIterator();
		while (li.hasNext()) {
			si = (ScreenItem) li.next();
			drawImage(graphics, lgmImages[si.getFrame()], si.getMX(), si
					.getMY(), si.getPX(), si.getPY());
			markRedraw(si, 0);
		}
	}

	/**
	 * Copies the base status window onto the graphics context
	 * 
	 * @param x
	 *            X destination position on the main screen to copy status to
	 * @param y
	 *            Y destination position on the main screen to copy status to
	 * @param graphics
	 *            Graphics context
	 * @param img
	 *            Image to copy
	 */
	private void drawCopyStatusImage(Graphics2D graphics, Image img, int x,
			int y) {
		x *= zoomFactor;
		y *= zoomFactor;
		graphics.drawImage(img, x, y, null);
	}

	/**
	 * Draws a base in the base status section of the window
	 * 
	 * @param pillNum
	 *            Pill number to draw
	 * @param pillAlliance
	 *            Pill alliance reference
	 */
	private void drawStatusPillbox(int pillNum, int pillAlliance) {
		String label;
		int width = 0;
		int height = 0;
		int left = 0;
		int top = 0;
		int destLeft = 0;
		int destTop = 0;
		Image tile;
		Graphics2D g = (Graphics2D) pillsStatusImage.getGraphics();

		switch (pillAlliance) {
		case GameItem.GAME_ITEM_ALLY_GOOD:
			left = zoomFactor * DrawPositions.STATUS_PILLBOX_ALLIEGOOD_X;
			top = zoomFactor * DrawPositions.STATUS_PILLBOX_ALLIEGOOD_Y;
			break;
		case GameItem.GAME_ITEM_DEAD:
			left = zoomFactor * DrawPositions.STATUS_ITEM_DEAD_X;
			top = zoomFactor * DrawPositions.STATUS_ITEM_DEAD_Y;
			break;
		case GameItem.GAME_ITEM_OWN_GOOD:
			left = zoomFactor * DrawPositions.STATUS_PILLBOX_GOOD_X;
			top = zoomFactor * DrawPositions.STATUS_PILLBOX_GOOD_Y;
			break;
		case GameItem.GAME_ITEM_NEUTRAL:
			left = zoomFactor * DrawPositions.STATUS_PILLBOX_NEUTRAL_X;
			top = zoomFactor * DrawPositions.STATUS_PILLBOX_NEUTRAL_Y;
			break;
		case GameItem.GAME_ITEM_INTANK_SELF:
			left = zoomFactor * DrawPositions.STATUS_PILLBOX_TANKGOOD_X;
			top = zoomFactor * DrawPositions.STATUS_PILLBOX_TANKGOOD_Y;
			break;
		case GameItem.GAME_ITEM_INTANK_ALLY:
			left = zoomFactor * DrawPositions.STATUS_PILLBOX_TANKALLIE_X;
			top = zoomFactor * DrawPositions.STATUS_PILLBOX_TANKALLIE_Y;
			break;
		case GameItem.GAME_ITEM_INTANK_EVIL:
			left = zoomFactor * DrawPositions.STATUS_PILLBOX_TANKEVIL_X;
			top = zoomFactor * DrawPositions.STATUS_PILLBOX_TANKEVIL_Y;
			break;
		case GameItem.GAME_ITEM_EVIL:
		default:
			/* Pillbox Evil */
			left = zoomFactor * DrawPositions.STATUS_PILLBOX_EVIL_X;
			top = zoomFactor * DrawPositions.STATUS_PILLBOX_EVIL_Y;
		}
		switch (pillNum) {
		case 0:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_1_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_1_Y);
			break;
		case 1:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_2_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_2_Y);
			break;
		case 2:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_3_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_3_Y);
			break;
		case 3:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_4_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_4_Y);
			break;
		case 4:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_5_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_5_Y);
			break;
		case 5:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_6_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_6_Y);
			break;
		case 6:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_7_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_7_Y);
			break;
		case 7:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_8_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_8_Y);
			break;
		case 8:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_9_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_9_Y);
			break;
		case 9:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_10_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_10_Y);
			break;
		case 10:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_11_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_11_Y);
			break;
		case 11:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_12_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_12_Y);
			break;
		case 12:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_13_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_13_Y);
			break;
		case 13:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_14_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_14_Y);
			break;
		case 14:
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_15_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_15_Y);
			break;
		default:
			/* PILLBOX_15: */
			destLeft = (zoomFactor * DrawPositions.STATUS_PILLBOX_16_X);
			destTop = (zoomFactor * DrawPositions.STATUS_PILLBOX_16_Y);
		}

		width = zoomFactor * DrawPositions.STATUS_ITEM_SIZE_X;
		height = zoomFactor * DrawPositions.STATUS_ITEM_SIZE_Y;
		tile = tiles.getSubimage(left, top, width, height);
		g.drawImage(tile, destLeft, destTop, null);
		g.dispose();
	}

	/**
	 * Draws a base in the base status section of the window
	 * 
	 * @param playerNum
	 *            Player number to draw
	 * @param playerAlliance
	 *            Player alliance reference
	 */
	public void drawStatusTank(int playerNum, int playerAlliance) {
		int width = 0;
		int height = 0;
		int left = 0;
		int top = 0;
		int destLeft = 0;
		int destTop = 0;
		Image tile;
		Graphics2D g;

		switch (playerAlliance) {
		case GameItem.GAME_ITEM_ALLY_GOOD:
			left = zoomFactor * DrawPositions.STATUS_BASE_ALLIEGOOD_X;
			top = zoomFactor * DrawPositions.STATUS_BASE_ALLIEGOOD_Y;
			break;
		case GameItem.GAME_ITEM_DEAD:
			left = zoomFactor * DrawPositions.STATUS_ITEM_DEAD_X;
			top = zoomFactor * DrawPositions.STATUS_ITEM_DEAD_Y;
			break;
		case GameItem.GAME_ITEM_OWN_GOOD:
			left = zoomFactor * DrawPositions.STATUS_BASE_GOOD_X;
			top = zoomFactor * DrawPositions.STATUS_BASE_GOOD_Y;
			break;
		case GameItem.GAME_ITEM_NEUTRAL:
			left = zoomFactor * DrawPositions.STATUS_BASE_NEUTRAL_X;
			top = zoomFactor * DrawPositions.STATUS_BASE_NEUTRAL_Y;
			break;
		case GameItem.GAME_ITEM_EVIL:
		default:
			/* Base Evil */
			left = zoomFactor * DrawPositions.STATUS_BASE_EVIL_X;
			top = zoomFactor * DrawPositions.STATUS_BASE_EVIL_Y;
		}
		switch (playerNum) {
		case 0:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_1_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_1_Y);
			break;
		case 1:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_2_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_2_Y);
			break;
		case 2:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_3_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_3_Y);
			break;
		case 3:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_4_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_4_Y);
			break;
		case 4:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_5_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_5_Y);
			break;
		case 5:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_6_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_6_Y);
			break;
		case 6:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_7_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_7_Y);
			break;
		case 7:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_8_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_8_Y);
			break;
		case 8:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_9_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_9_Y);
			break;
		case 9:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_10_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_10_Y);
			break;
		case 10:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_11_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_11_Y);
			break;
		case 11:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_12_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_12_Y);
			break;
		case 12:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_13_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_13_Y);
			break;
		case 13:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_14_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_14_Y);
			break;
		case 14:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_15_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_15_Y);
			break;
		default:
			/* BASE_15: */
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_16_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_16_Y);
		}

		width = zoomFactor * DrawPositions.STATUS_ITEM_SIZE_X;
		height = zoomFactor * DrawPositions.STATUS_ITEM_SIZE_Y;
		if (playerAlliance != GameItem.GAME_ITEM_BLANK) {
			g = (Graphics2D) tanksStatusImage.getGraphics();
			tile = tiles.getSubimage(left, top, width, height);
			g.drawImage(tile, destLeft, destTop, null);
			g.dispose();
		}

	}

	/**
	 * Draws a base in the base status section of the window
	 * 
	 * @param baseNum
	 *            Base number to draw
	 * @param baseAlliance
	 *            Base alliance reference
	 */
	public void drawStatusBase(int baseNum, int baseAlliance) {
		int width = 0;
		int height = 0;
		int left = 0;
		int top = 0;
		int destLeft = 0;
		int destTop = 0;
		Image tile;
		Graphics2D g = (Graphics2D) baseStatusImage.getGraphics();

		switch (baseAlliance) {
		case GameItem.GAME_ITEM_ALLY_GOOD:
			left = zoomFactor * DrawPositions.STATUS_BASE_ALLIEGOOD_X;
			top = zoomFactor * DrawPositions.STATUS_BASE_ALLIEGOOD_Y;
			break;
		case GameItem.GAME_ITEM_DEAD:
			left = zoomFactor * DrawPositions.STATUS_ITEM_DEAD_X;
			top = zoomFactor * DrawPositions.STATUS_ITEM_DEAD_Y;
			break;
		case GameItem.GAME_ITEM_OWN_GOOD:
			left = zoomFactor * DrawPositions.STATUS_BASE_GOOD_X;
			top = zoomFactor * DrawPositions.STATUS_BASE_GOOD_Y;
			break;
		case GameItem.GAME_ITEM_NEUTRAL:
			left = zoomFactor * DrawPositions.STATUS_BASE_NEUTRAL_X;
			top = zoomFactor * DrawPositions.STATUS_BASE_NEUTRAL_Y;
			break;
		case GameItem.GAME_ITEM_EVIL:
		default:
			/* Base Evil */
			left = zoomFactor * DrawPositions.STATUS_BASE_EVIL_X;
			top = zoomFactor * DrawPositions.STATUS_BASE_EVIL_Y;
		}
		switch (baseNum) {
		case 0:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_1_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_1_Y);
			break;
		case 1:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_2_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_2_Y);
			break;
		case 2:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_3_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_3_Y);
			break;
		case 3:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_4_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_4_Y);
			break;
		case 4:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_5_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_5_Y);
			break;
		case 5:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_6_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_6_Y);
			break;
		case 6:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_7_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_7_Y);
			break;
		case 7:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_8_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_8_Y);
			break;
		case 8:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_9_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_9_Y);
			break;
		case 9:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_10_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_10_Y);
			break;
		case 10:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_11_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_11_Y);
			break;
		case 11:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_12_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_12_Y);
			break;
		case 12:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_13_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_13_Y);
			break;
		case 13:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_14_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_14_Y);
			break;
		case 14:
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_15_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_15_Y);
			break;
		default:
			/* BASE_15: */
			destLeft = (zoomFactor * DrawPositions.STATUS_BASE_16_X);
			destTop = (zoomFactor * DrawPositions.STATUS_BASE_16_Y);
		}

		width = zoomFactor * DrawPositions.STATUS_ITEM_SIZE_X;
		height = zoomFactor * DrawPositions.STATUS_ITEM_SIZE_Y;

		tile = tiles.getSubimage(left, top, width, height);
		g.drawImage(tile, destLeft, destTop, null);
		g.dispose();
	}

	/**
	 * Returns a BufferedImage of a tile at position x, y
	 * 
	 * @param x
	 *            x location of the tile
	 * @param y
	 *            y location of the tile
	 * @param zoomFactor
	 *            scaling factor
	 * @return BufferedImage
	 */
	private BufferedImage setupTile(int x, int y, int zoomFactor) {
		return tiles.getSubimage(x, y, zoomFactor * DrawPositions.TILE_SIZE_X,
				zoomFactor * DrawPositions.TILE_SIZE_Y);
	}

	/**
	 * Sets up each of the tile images to cache them
	 * 
	 * @param zoomFactor
	 *            Scaling factor
	 */
	private void setupTiles(int zoomFactor) {
		int count; // Looping variable

		count = 17;
		while (count < 163) { // 123
			// out of sequence 163
			System.err.print(".");
			tileImg[count] = setupTile(drawPosX[count], drawPosY[count],
					zoomFactor);
			count++;
		}

		tileImg[TileNum.CRATER] = setupTile(drawPosX[TileNum.CRATER],
				drawPosY[TileNum.CRATER], zoomFactor);
		tileImg[TileNum.FOREST] = setupTile(drawPosX[TileNum.FOREST],
				drawPosY[TileNum.FOREST], zoomFactor);
		tileImg[TileNum.SWAMP] = setupTile(drawPosX[TileNum.SWAMP],
				drawPosY[TileNum.SWAMP], zoomFactor);
		tileImg[TileNum.RUBBLE] = setupTile(drawPosX[TileNum.RUBBLE],
				drawPosY[TileNum.RUBBLE], zoomFactor);
		tileImg[TileNum.GRASS] = setupTile(drawPosX[TileNum.GRASS],
				drawPosY[TileNum.GRASS], zoomFactor);
		tileImg[TileNum.HALFBUILDING] = setupTile(
				drawPosX[TileNum.HALFBUILDING], drawPosY[TileNum.HALFBUILDING],
				zoomFactor);

		count = 163;
		while (count < 182) {
			System.err.print(".");
			tileImg[count] = setupTile(drawPosX[count], drawPosY[count],
					zoomFactor);
			count++;
		}

		count = 137;
		while (count < 148) {
			System.err.print(".");
			tileImg[count] = setupTile(drawPosX[count], drawPosY[count],
					zoomFactor);
			count++;
		}

	}

	/**
	 * Applies a mask to an image to make it transparent
	 * 
	 * @return Image with a mask
	 * @param gd
	 *            The graphics to use
	 * @param img
	 *            Image to apply mask to
	 * @param keyColor
	 *            Colour to apply the mask on
	 */
	private Image applyMask(Graphics2D gd, Image img, Color keyColor) {
		final int w = img.getWidth(null);
		final int h = img.getHeight(null);
		final int[] pxls = getPixels(img);
		for (int i = 0; i < pxls.length; i++) {
			if (pxls[i] == keyColor.getRGB()) {
				pxls[i] = pxls[i] & 0x00ffffff;
			}
		}
		return getImage(pxls, w, h);
	}

	/**
	 * Gets an array of pixels for an image
	 * 
	 * @param img
	 *            Image to get pixels for
	 * @return int array
	 */
	private int[] getPixels(Image img) {
		final int[] pix = new int[img.getWidth(null) * img.getHeight(null)];
		final PixelGrabber pg = new PixelGrabber(img, 0, 0, img.getWidth(null),
				img.getHeight(null), pix, 0, img.getWidth(null));
		try {
			pg.grabPixels();
		} catch (Exception e) {
		}
		return pix;
	}

	/**
	 * Creates a new image of size x,y from pixels
	 * 
	 * @param pixels
	 *            Array of pixels
	 * @param w
	 *            width of image
	 * @param h
	 *            height of image
	 * @return Image
	 */
	private Image getImage(int[] pixels, int w, int h) {
		MemoryImageSource source = new MemoryImageSource(w, h, pixels, 0, w);
		return Toolkit.getDefaultToolkit().createImage(source);
	}

	/**
	 * Sets up the gunsight image
	 * 
	 * @param gd
	 *            Graphics Object
	 * @return Tank Gunsight image
	 */
	private Image setupGunsightImage(Graphics2D gd) {
		Image img;

		img = tiles.getSubimage(zoomFactor * DrawPositions.GUNSIGHT_X,
				zoomFactor * DrawPositions.GUNSIGHT_Y, zoomFactor
						* DrawPositions.TILE_SIZE_X, DrawPositions.TILE_SIZE_Y);
		return applyMask(gd, img, Color.GREEN);
	}

	/**
	 * Sets up the array of transparent shell images
	 * 
	 * @param gd
	 *            Graphics object
	 */
	private void setupShellImages(Graphics gd) {
		int count = 0;
		int x;
		int y;
		int height;
		int width;

		Image img;

		while (count < 25) {
			switch (count) {
			case TileNum.SHELL_EXPLOSION8:
				x = zoomFactor * DrawPositions.EXPLOSION8_X;
				y = zoomFactor * DrawPositions.EXPLOSION8_Y;
				width = (zoomFactor * DrawPositions.EXPLOSION8_X + zoomFactor
						* DrawPositions.TILE_SIZE_X);
				height = (zoomFactor * DrawPositions.EXPLOSION8_Y + zoomFactor
						* DrawPositions.TILE_SIZE_Y);
				break;
			case TileNum.SHELL_EXPLOSION7:
				x = zoomFactor * DrawPositions.EXPLOSION7_X;
				y = zoomFactor * DrawPositions.EXPLOSION7_Y;
				width = (zoomFactor * DrawPositions.EXPLOSION7_X + zoomFactor
						* DrawPositions.TILE_SIZE_X);
				height = (zoomFactor * DrawPositions.EXPLOSION7_Y + zoomFactor
						* DrawPositions.TILE_SIZE_Y);
				break;
			case TileNum.SHELL_EXPLOSION6:
				x = zoomFactor * DrawPositions.EXPLOSION6_X;
				y = zoomFactor * DrawPositions.EXPLOSION6_Y;
				width = (zoomFactor * DrawPositions.EXPLOSION6_X + zoomFactor
						* DrawPositions.TILE_SIZE_X);
				height = (zoomFactor * DrawPositions.EXPLOSION6_Y + zoomFactor
						* DrawPositions.TILE_SIZE_Y);
				break;
			case TileNum.SHELL_EXPLOSION5:
				x = zoomFactor * DrawPositions.EXPLOSION5_X;
				y = zoomFactor * DrawPositions.EXPLOSION5_Y;
				width = (zoomFactor * DrawPositions.EXPLOSION5_X + zoomFactor
						* DrawPositions.TILE_SIZE_X);
				height = (zoomFactor * DrawPositions.EXPLOSION5_Y + zoomFactor
						* DrawPositions.TILE_SIZE_Y);
				break;
			case TileNum.SHELL_EXPLOSION4:
				x = zoomFactor * DrawPositions.EXPLOSION4_X;
				y = zoomFactor * DrawPositions.EXPLOSION4_Y;
				width = (zoomFactor * DrawPositions.EXPLOSION4_X + zoomFactor
						* DrawPositions.TILE_SIZE_X);
				height = (zoomFactor * DrawPositions.EXPLOSION4_Y + zoomFactor
						* DrawPositions.TILE_SIZE_Y);
				break;
			case TileNum.SHELL_EXPLOSION3:
				x = zoomFactor * DrawPositions.EXPLOSION3_X;
				y = zoomFactor * DrawPositions.EXPLOSION3_Y;
				width = (zoomFactor * DrawPositions.EXPLOSION3_X + zoomFactor
						* DrawPositions.TILE_SIZE_X);
				height = (zoomFactor * DrawPositions.EXPLOSION3_Y + zoomFactor
						* DrawPositions.TILE_SIZE_Y);
				break;
			case TileNum.SHELL_EXPLOSION2:
				x = zoomFactor * DrawPositions.EXPLOSION2_X;
				y = zoomFactor * DrawPositions.EXPLOSION2_Y;
				width = (zoomFactor * DrawPositions.EXPLOSION2_X + zoomFactor
						* DrawPositions.TILE_SIZE_X);
				height = (zoomFactor * DrawPositions.EXPLOSION2_Y + zoomFactor
						* DrawPositions.TILE_SIZE_Y);
				break;
			case TileNum.SHELL_EXPLOSION1:
				x = zoomFactor * DrawPositions.EXPLOSION1_X;
				y = zoomFactor * DrawPositions.EXPLOSION1_Y;
				width = (zoomFactor * DrawPositions.EXPLOSION1_X + zoomFactor
						* DrawPositions.TILE_SIZE_X);
				height = (zoomFactor * DrawPositions.EXPLOSION1_Y + zoomFactor
						* DrawPositions.TILE_SIZE_Y);
				break;
			case TileNum.SHELL_DIR0:
				x = zoomFactor * DrawPositions.SHELL_0_X;
				y = zoomFactor * DrawPositions.SHELL_0_Y;
				width = (zoomFactor * DrawPositions.SHELL_0_X + zoomFactor
						* DrawPositions.SHELL_0_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_0_Y + zoomFactor
						* DrawPositions.SHELL_0_HEIGHT);
				break;
			case TileNum.SHELL_DIR1:
				x = zoomFactor * DrawPositions.SHELL_1_X;
				y = zoomFactor * DrawPositions.SHELL_1_Y;
				width = (zoomFactor * DrawPositions.SHELL_1_X + zoomFactor
						* DrawPositions.SHELL_1_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_1_Y + zoomFactor
						* DrawPositions.SHELL_1_HEIGHT);
				break;
			case TileNum.SHELL_DIR2:
				x = zoomFactor * DrawPositions.SHELL_2_X;
				y = zoomFactor * DrawPositions.SHELL_2_Y;
				width = (zoomFactor * DrawPositions.SHELL_2_X + zoomFactor
						* DrawPositions.SHELL_2_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_2_Y + zoomFactor
						* DrawPositions.SHELL_2_HEIGHT);
				break;
			case TileNum.SHELL_DIR3:
				x = zoomFactor * DrawPositions.SHELL_3_X;
				y = zoomFactor * DrawPositions.SHELL_3_Y;
				width = (zoomFactor * DrawPositions.SHELL_3_X + zoomFactor
						* DrawPositions.SHELL_3_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_3_Y + zoomFactor
						* DrawPositions.SHELL_3_HEIGHT);
				break;
			case TileNum.SHELL_DIR4:
				x = zoomFactor * DrawPositions.SHELL_4_X;
				y = zoomFactor * DrawPositions.SHELL_4_Y;
				width = (zoomFactor * DrawPositions.SHELL_4_X + zoomFactor
						* DrawPositions.SHELL_4_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_4_Y + zoomFactor
						* DrawPositions.SHELL_4_HEIGHT);
				break;
			case TileNum.SHELL_DIR5:
				x = zoomFactor * DrawPositions.SHELL_5_X;
				y = zoomFactor * DrawPositions.SHELL_5_Y;
				width = (zoomFactor * DrawPositions.SHELL_5_X + zoomFactor
						* DrawPositions.SHELL_5_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_5_Y + zoomFactor
						* DrawPositions.SHELL_5_HEIGHT);
				break;
			case TileNum.SHELL_DIR6:
				x = zoomFactor * DrawPositions.SHELL_6_X;
				y = zoomFactor * DrawPositions.SHELL_6_Y;
				width = (zoomFactor * DrawPositions.SHELL_6_X + zoomFactor
						* DrawPositions.SHELL_6_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_6_Y + zoomFactor
						* DrawPositions.SHELL_6_HEIGHT);
				break;
			case TileNum.SHELL_DIR7:
				x = zoomFactor * DrawPositions.SHELL_7_X;
				y = zoomFactor * DrawPositions.SHELL_7_Y;
				width = (zoomFactor * DrawPositions.SHELL_7_X + zoomFactor
						* DrawPositions.SHELL_7_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_7_Y + zoomFactor
						* DrawPositions.SHELL_7_HEIGHT);
				break;
			case TileNum.SHELL_DIR8:
				x = zoomFactor * DrawPositions.SHELL_8_X;
				y = zoomFactor * DrawPositions.SHELL_8_Y;
				width = (zoomFactor * DrawPositions.SHELL_8_X + zoomFactor
						* DrawPositions.SHELL_8_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_8_Y + zoomFactor
						* DrawPositions.SHELL_8_HEIGHT);
				break;
			case TileNum.SHELL_DIR9:
				x = zoomFactor * DrawPositions.SHELL_9_X;
				y = zoomFactor * DrawPositions.SHELL_9_Y;
				width = (zoomFactor * DrawPositions.SHELL_9_X + zoomFactor
						* DrawPositions.SHELL_9_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_9_Y + zoomFactor
						* DrawPositions.SHELL_9_HEIGHT);
				break;
			case TileNum.SHELL_DIR10:
				x = zoomFactor * DrawPositions.SHELL_10_X;
				y = zoomFactor * DrawPositions.SHELL_10_Y;
				width = (zoomFactor * DrawPositions.SHELL_10_X + zoomFactor
						* DrawPositions.SHELL_10_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_10_Y + zoomFactor
						* DrawPositions.SHELL_10_HEIGHT);
				break;
			case TileNum.SHELL_DIR11:
				x = zoomFactor * DrawPositions.SHELL_11_X;
				y = zoomFactor * DrawPositions.SHELL_11_Y;
				width = (zoomFactor * DrawPositions.SHELL_11_X + zoomFactor
						* DrawPositions.SHELL_11_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_11_Y + zoomFactor
						* DrawPositions.SHELL_11_HEIGHT);
				break;
			case TileNum.SHELL_DIR12:
				x = zoomFactor * DrawPositions.SHELL_12_X;
				y = zoomFactor * DrawPositions.SHELL_12_Y;
				width = (zoomFactor * DrawPositions.SHELL_12_X + zoomFactor
						* DrawPositions.SHELL_12_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_12_Y + zoomFactor
						* DrawPositions.SHELL_12_HEIGHT);
				break;
			case TileNum.SHELL_DIR13:
				x = zoomFactor * DrawPositions.SHELL_13_X;
				y = zoomFactor * DrawPositions.SHELL_13_Y;
				width = (zoomFactor * DrawPositions.SHELL_13_X + zoomFactor
						* DrawPositions.SHELL_13_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_13_Y + zoomFactor
						* DrawPositions.SHELL_13_HEIGHT);
				break;
			case TileNum.SHELL_DIR14:
				x = zoomFactor * DrawPositions.SHELL_14_X;
				y = zoomFactor * DrawPositions.SHELL_14_Y;
				width = (zoomFactor * DrawPositions.SHELL_14_X + zoomFactor
						* DrawPositions.SHELL_14_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_14_Y + zoomFactor
						* DrawPositions.SHELL_14_HEIGHT);
				break;
			default:
				/* SHELL_DIR15 */
				x = zoomFactor * DrawPositions.SHELL_15_X;
				y = zoomFactor * DrawPositions.SHELL_15_Y;
				width = (zoomFactor * DrawPositions.SHELL_15_X + zoomFactor
						* DrawPositions.SHELL_15_WIDTH);
				height = (zoomFactor * DrawPositions.SHELL_15_Y + zoomFactor
						* DrawPositions.SHELL_15_HEIGHT);
				break;

			}
			x *= zoomFactor;
			y *= zoomFactor;
			img = tiles.getSubimage(x, y, width - x, height - y);
			shellImages[count] = this.applyMask((Graphics2D) gd, img,
					Color.GREEN);
			System.err.print(".");
			count++;
		}
	}

	/**
	 * Sets up the array of transparent tank images
	 * 
	 * @param gd
	 *            Graphics object
	 */
	private void setupTankImages(Graphics2D gd) {
		int count = 0;
		int left;
		int top;
		Image img;

		while (count < 96) {
			switch (count) {
			case TileNum.TANK_SELF_0:
				left = DrawPositions.TANK_SELF_0_X;
				top = DrawPositions.TANK_SELF_0_Y;
				break;
			case TileNum.TANK_SELF_1:
				left = DrawPositions.TANK_SELF_1_X;
				top = DrawPositions.TANK_SELF_1_Y;
				break;
			case TileNum.TANK_SELF_2:
				left = DrawPositions.TANK_SELF_2_X;
				top = DrawPositions.TANK_SELF_2_Y;
				break;
			case TileNum.TANK_SELF_3:
				left = DrawPositions.TANK_SELF_3_X;
				top = DrawPositions.TANK_SELF_3_Y;
				break;
			case TileNum.TANK_SELF_4:
				left = DrawPositions.TANK_SELF_4_X;
				top = DrawPositions.TANK_SELF_4_Y;
				break;
			case TileNum.TANK_SELF_5:
				left = DrawPositions.TANK_SELF_5_X;
				top = DrawPositions.TANK_SELF_5_Y;
				break;
			case TileNum.TANK_SELF_6:
				left = DrawPositions.TANK_SELF_6_X;
				top = DrawPositions.TANK_SELF_6_Y;
				break;
			case TileNum.TANK_SELF_7:
				left = DrawPositions.TANK_SELF_7_X;
				top = DrawPositions.TANK_SELF_7_Y;
				break;
			case TileNum.TANK_SELF_8:
				left = DrawPositions.TANK_SELF_8_X;
				top = DrawPositions.TANK_SELF_8_Y;
				break;
			case TileNum.TANK_SELF_9:
				left = DrawPositions.TANK_SELF_9_X;
				top = DrawPositions.TANK_SELF_9_Y;
				break;
			case TileNum.TANK_SELF_10:
				left = DrawPositions.TANK_SELF_10_X;
				top = DrawPositions.TANK_SELF_10_Y;
				break;
			case TileNum.TANK_SELF_11:
				left = DrawPositions.TANK_SELF_11_X;
				top = DrawPositions.TANK_SELF_11_Y;
				break;
			case TileNum.TANK_SELF_12:
				left = DrawPositions.TANK_SELF_12_X;
				top = DrawPositions.TANK_SELF_12_Y;
				break;
			case TileNum.TANK_SELF_13:
				left = DrawPositions.TANK_SELF_13_X;
				top = DrawPositions.TANK_SELF_13_Y;
				break;
			case TileNum.TANK_SELF_14:
				left = DrawPositions.TANK_SELF_14_X;
				top = DrawPositions.TANK_SELF_14_Y;
				break;
			case TileNum.TANK_SELF_15:
				left = DrawPositions.TANK_SELF_15_X;
				top = DrawPositions.TANK_SELF_15_Y;
				break;
			case TileNum.TANK_SELFBOAT_0:
				left = DrawPositions.TANK_SELFBOAT_0_X;
				top = DrawPositions.TANK_SELFBOAT_0_Y;
				break;
			case TileNum.TANK_SELFBOAT_1:
				left = DrawPositions.TANK_SELFBOAT_1_X;
				top = DrawPositions.TANK_SELFBOAT_1_Y;
				break;
			case TileNum.TANK_SELFBOAT_2:
				left = DrawPositions.TANK_SELFBOAT_2_X;
				top = DrawPositions.TANK_SELFBOAT_2_Y;
				break;
			case TileNum.TANK_SELFBOAT_3:
				left = DrawPositions.TANK_SELFBOAT_3_X;
				top = DrawPositions.TANK_SELFBOAT_3_Y;
				break;
			case TileNum.TANK_SELFBOAT_4:
				left = DrawPositions.TANK_SELFBOAT_4_X;
				top = DrawPositions.TANK_SELFBOAT_4_Y;
				break;
			case TileNum.TANK_SELFBOAT_5:
				left = DrawPositions.TANK_SELFBOAT_5_X;
				top = DrawPositions.TANK_SELFBOAT_5_Y;
				break;
			case TileNum.TANK_SELFBOAT_6:
				left = DrawPositions.TANK_SELFBOAT_6_X;
				top = DrawPositions.TANK_SELFBOAT_6_Y;
				break;
			case TileNum.TANK_SELFBOAT_7:
				left = DrawPositions.TANK_SELFBOAT_7_X;
				top = DrawPositions.TANK_SELFBOAT_7_Y;
				break;
			case TileNum.TANK_SELFBOAT_8:
				left = DrawPositions.TANK_SELFBOAT_8_X;
				top = DrawPositions.TANK_SELFBOAT_8_Y;
				break;
			case TileNum.TANK_SELFBOAT_9:
				left = DrawPositions.TANK_SELFBOAT_9_X;
				top = DrawPositions.TANK_SELFBOAT_9_Y;
				break;
			case TileNum.TANK_SELFBOAT_10:
				left = DrawPositions.TANK_SELFBOAT_10_X;
				top = DrawPositions.TANK_SELFBOAT_10_Y;
				break;
			case TileNum.TANK_SELFBOAT_11:
				left = DrawPositions.TANK_SELFBOAT_11_X;
				top = DrawPositions.TANK_SELFBOAT_11_Y;
				break;
			case TileNum.TANK_SELFBOAT_12:
				left = DrawPositions.TANK_SELFBOAT_12_X;
				top = DrawPositions.TANK_SELFBOAT_12_Y;
				break;
			case TileNum.TANK_SELFBOAT_13:
				left = DrawPositions.TANK_SELFBOAT_13_X;
				top = DrawPositions.TANK_SELFBOAT_13_Y;
				break;
			case TileNum.TANK_SELFBOAT_14:
				left = DrawPositions.TANK_SELFBOAT_14_X;
				top = DrawPositions.TANK_SELFBOAT_14_Y;
				break;
			case TileNum.TANK_SELFBOAT_15:
				left = DrawPositions.TANK_SELFBOAT_15_X;
				top = DrawPositions.TANK_SELFBOAT_15_Y;
				break;
			case TileNum.TANK_GOOD_0:
				left = DrawPositions.TANK_GOOD_0_X;
				top = DrawPositions.TANK_GOOD_0_Y;
				break;
			case TileNum.TANK_GOOD_1:
				left = DrawPositions.TANK_GOOD_1_X;
				top = DrawPositions.TANK_GOOD_1_Y;
				break;
			case TileNum.TANK_GOOD_2:
				left = DrawPositions.TANK_GOOD_2_X;
				top = DrawPositions.TANK_GOOD_2_Y;
				break;
			case TileNum.TANK_GOOD_3:
				left = DrawPositions.TANK_GOOD_3_X;
				top = DrawPositions.TANK_GOOD_3_Y;
				break;
			case TileNum.TANK_GOOD_4:
				left = DrawPositions.TANK_GOOD_4_X;
				top = DrawPositions.TANK_GOOD_4_Y;
				break;
			case TileNum.TANK_GOOD_5:
				left = DrawPositions.TANK_GOOD_5_X;
				top = DrawPositions.TANK_GOOD_5_Y;
				break;
			case TileNum.TANK_GOOD_6:
				left = DrawPositions.TANK_GOOD_6_X;
				top = DrawPositions.TANK_GOOD_6_Y;
				break;
			case TileNum.TANK_GOOD_7:
				left = DrawPositions.TANK_GOOD_7_X;
				top = DrawPositions.TANK_GOOD_7_Y;
				break;
			case TileNum.TANK_GOOD_8:
				left = DrawPositions.TANK_GOOD_8_X;
				top = DrawPositions.TANK_GOOD_8_Y;
				break;
			case TileNum.TANK_GOOD_9:
				left = DrawPositions.TANK_GOOD_9_X;
				top = DrawPositions.TANK_GOOD_9_Y;
				break;
			case TileNum.TANK_GOOD_10:
				left = DrawPositions.TANK_GOOD_10_X;
				top = DrawPositions.TANK_GOOD_10_Y;
				break;
			case TileNum.TANK_GOOD_11:
				left = DrawPositions.TANK_GOOD_11_X;
				top = DrawPositions.TANK_GOOD_11_Y;
				break;
			case TileNum.TANK_GOOD_12:
				left = DrawPositions.TANK_GOOD_12_X;
				top = DrawPositions.TANK_GOOD_12_Y;
				break;
			case TileNum.TANK_GOOD_13:
				left = DrawPositions.TANK_GOOD_13_X;
				top = DrawPositions.TANK_GOOD_13_Y;
				break;
			case TileNum.TANK_GOOD_14:
				left = DrawPositions.TANK_GOOD_14_X;
				top = DrawPositions.TANK_GOOD_14_Y;
				break;
			case TileNum.TANK_GOOD_15:
				left = DrawPositions.TANK_GOOD_15_X;
				top = DrawPositions.TANK_GOOD_15_Y;
				break;
			case TileNum.TANK_GOODBOAT_0:
				left = DrawPositions.TANK_GOODBOAT_0_X;
				top = DrawPositions.TANK_GOODBOAT_0_Y;
				break;
			case TileNum.TANK_GOODBOAT_1:
				left = DrawPositions.TANK_GOODBOAT_1_X;
				top = DrawPositions.TANK_GOODBOAT_1_Y;
				break;
			case TileNum.TANK_GOODBOAT_2:
				left = DrawPositions.TANK_GOODBOAT_2_X;
				top = DrawPositions.TANK_GOODBOAT_2_Y;
				break;
			case TileNum.TANK_GOODBOAT_3:
				left = DrawPositions.TANK_GOODBOAT_3_X;
				top = DrawPositions.TANK_GOODBOAT_3_Y;
				break;
			case TileNum.TANK_GOODBOAT_4:
				left = DrawPositions.TANK_GOODBOAT_4_X;
				top = DrawPositions.TANK_GOODBOAT_4_Y;
				break;
			case TileNum.TANK_GOODBOAT_5:
				left = DrawPositions.TANK_GOODBOAT_5_X;
				top = DrawPositions.TANK_GOODBOAT_5_Y;
				break;
			case TileNum.TANK_GOODBOAT_6:
				left = DrawPositions.TANK_GOODBOAT_6_X;
				top = DrawPositions.TANK_GOODBOAT_6_Y;
				break;
			case TileNum.TANK_GOODBOAT_7:
				left = DrawPositions.TANK_GOODBOAT_7_X;
				top = DrawPositions.TANK_GOODBOAT_7_Y;
				break;
			case TileNum.TANK_GOODBOAT_8:
				left = DrawPositions.TANK_GOODBOAT_8_X;
				top = DrawPositions.TANK_GOODBOAT_8_Y;
				break;
			case TileNum.TANK_GOODBOAT_9:
				left = DrawPositions.TANK_GOODBOAT_9_X;
				top = DrawPositions.TANK_GOODBOAT_9_Y;
				break;
			case TileNum.TANK_GOODBOAT_10:
				left = DrawPositions.TANK_GOODBOAT_10_X;
				top = DrawPositions.TANK_GOODBOAT_10_Y;
				break;
			case TileNum.TANK_GOODBOAT_11:
				left = DrawPositions.TANK_GOODBOAT_11_X;
				top = DrawPositions.TANK_GOODBOAT_11_Y;
				break;
			case TileNum.TANK_GOODBOAT_12:
				left = DrawPositions.TANK_GOODBOAT_12_X;
				top = DrawPositions.TANK_GOODBOAT_12_Y;
				break;
			case TileNum.TANK_GOODBOAT_13:
				left = DrawPositions.TANK_GOODBOAT_13_X;
				top = DrawPositions.TANK_GOODBOAT_13_Y;
				break;
			case TileNum.TANK_GOODBOAT_14:
				left = DrawPositions.TANK_GOODBOAT_14_X;
				top = DrawPositions.TANK_GOODBOAT_14_Y;
				break;
			case TileNum.TANK_GOODBOAT_15:
				left = DrawPositions.TANK_GOODBOAT_15_X;
				top = DrawPositions.TANK_GOODBOAT_15_Y;
				break;
			case TileNum.TANK_EVIL_0:
				left = DrawPositions.TANK_EVIL_0_X;
				top = DrawPositions.TANK_EVIL_0_Y;
				break;
			case TileNum.TANK_EVIL_1:
				left = DrawPositions.TANK_EVIL_1_X;
				top = DrawPositions.TANK_EVIL_1_Y;
				break;
			case TileNum.TANK_EVIL_2:
				left = DrawPositions.TANK_EVIL_2_X;
				top = DrawPositions.TANK_EVIL_2_Y;
				break;
			case TileNum.TANK_EVIL_3:
				left = DrawPositions.TANK_EVIL_3_X;
				top = DrawPositions.TANK_EVIL_3_Y;
				break;
			case TileNum.TANK_EVIL_4:
				left = DrawPositions.TANK_EVIL_4_X;
				top = DrawPositions.TANK_EVIL_4_Y;
				break;
			case TileNum.TANK_EVIL_5:
				left = DrawPositions.TANK_EVIL_5_X;
				top = DrawPositions.TANK_EVIL_5_Y;
				break;
			case TileNum.TANK_EVIL_6:
				left = DrawPositions.TANK_EVIL_6_X;
				top = DrawPositions.TANK_EVIL_6_Y;
				break;
			case TileNum.TANK_EVIL_7:
				left = DrawPositions.TANK_EVIL_7_X;
				top = DrawPositions.TANK_EVIL_7_Y;
				break;
			case TileNum.TANK_EVIL_8:
				left = DrawPositions.TANK_EVIL_8_X;
				top = DrawPositions.TANK_EVIL_8_Y;
				break;
			case TileNum.TANK_EVIL_9:
				left = DrawPositions.TANK_EVIL_9_X;
				top = DrawPositions.TANK_EVIL_9_Y;
				break;
			case TileNum.TANK_EVIL_10:
				left = DrawPositions.TANK_EVIL_10_X;
				top = DrawPositions.TANK_EVIL_10_Y;
				break;
			case TileNum.TANK_EVIL_11:
				left = DrawPositions.TANK_EVIL_11_X;
				top = DrawPositions.TANK_EVIL_11_Y;
				break;
			case TileNum.TANK_EVIL_12:
				left = DrawPositions.TANK_EVIL_12_X;
				top = DrawPositions.TANK_EVIL_12_Y;
				break;
			case TileNum.TANK_EVIL_13:
				left = DrawPositions.TANK_EVIL_13_X;
				top = DrawPositions.TANK_EVIL_13_Y;
				break;
			case TileNum.TANK_EVIL_14:
				left = DrawPositions.TANK_EVIL_14_X;
				top = DrawPositions.TANK_EVIL_14_Y;
				break;
			case TileNum.TANK_EVIL_15:
				left = DrawPositions.TANK_EVIL_15_X;
				top = DrawPositions.TANK_EVIL_15_Y;
				break;
			case TileNum.TANK_EVILBOAT_0:
				left = DrawPositions.TANK_EVILBOAT_0_X;
				top = DrawPositions.TANK_EVILBOAT_0_Y;
				break;
			case TileNum.TANK_EVILBOAT_1:
				left = DrawPositions.TANK_EVILBOAT_1_X;
				top = DrawPositions.TANK_EVILBOAT_1_Y;
				break;
			case TileNum.TANK_EVILBOAT_2:
				left = DrawPositions.TANK_EVILBOAT_2_X;
				top = DrawPositions.TANK_EVILBOAT_2_Y;
				break;
			case TileNum.TANK_EVILBOAT_3:
				left = DrawPositions.TANK_EVILBOAT_3_X;
				top = DrawPositions.TANK_EVILBOAT_3_Y;
				break;
			case TileNum.TANK_EVILBOAT_4:
				left = DrawPositions.TANK_EVILBOAT_4_X;
				top = DrawPositions.TANK_EVILBOAT_4_Y;
				break;
			case TileNum.TANK_EVILBOAT_5:
				left = DrawPositions.TANK_EVILBOAT_5_X;
				top = DrawPositions.TANK_EVILBOAT_5_Y;
				break;
			case TileNum.TANK_EVILBOAT_6:
				left = DrawPositions.TANK_EVILBOAT_6_X;
				top = DrawPositions.TANK_EVILBOAT_6_Y;
				break;
			case TileNum.TANK_EVILBOAT_7:
				left = DrawPositions.TANK_EVILBOAT_7_X;
				top = DrawPositions.TANK_EVILBOAT_7_Y;
				break;
			case TileNum.TANK_EVILBOAT_8:
				left = DrawPositions.TANK_EVILBOAT_8_X;
				top = DrawPositions.TANK_EVILBOAT_8_Y;
				break;
			case TileNum.TANK_EVILBOAT_9:
				left = DrawPositions.TANK_EVILBOAT_9_X;
				top = DrawPositions.TANK_EVILBOAT_9_Y;
				break;
			case TileNum.TANK_EVILBOAT_10:
				left = DrawPositions.TANK_EVILBOAT_10_X;
				top = DrawPositions.TANK_EVILBOAT_10_Y;
				break;
			case TileNum.TANK_EVILBOAT_11:
				left = DrawPositions.TANK_EVILBOAT_11_X;
				top = DrawPositions.TANK_EVILBOAT_11_Y;
				break;
			case TileNum.TANK_EVILBOAT_12:
				left = DrawPositions.TANK_EVILBOAT_12_X;
				top = DrawPositions.TANK_EVILBOAT_12_Y;
				break;
			case TileNum.TANK_EVILBOAT_13:
				left = DrawPositions.TANK_EVILBOAT_13_X;
				top = DrawPositions.TANK_EVILBOAT_13_Y;
				break;
			case TileNum.TANK_EVILBOAT_14:
				left = DrawPositions.TANK_EVILBOAT_14_X;
				top = DrawPositions.TANK_EVILBOAT_14_Y;
				break;
			case TileNum.TANK_EVILBOAT_15:
				left = DrawPositions.TANK_EVILBOAT_15_X;
				top = DrawPositions.TANK_EVILBOAT_15_Y;
				break;

			default:
				/* TANK_TRANSPARENT */
				left = DrawPositions.TANK_TRANSPARENT_X;
				top = DrawPositions.TANK_TRANSPARENT_Y;
			}
			left *= zoomFactor;
			top *= zoomFactor;
			img = tiles.getSubimage(left, top, DrawPositions.TILE_SIZE_X
					* zoomFactor, DrawPositions.TILE_SIZE_Y * zoomFactor);
			tankImages[count] = this.applyMask(gd, img, Color.GREEN);
			System.err.print(".");
			count++;
		}
	}

	/**
	 * Sets up the array of transparent LGM images
	 * 
	 * @param gd
	 *            Graphics object
	 */
	private void setupLgmImages(Graphics2D gd) {
		int count = 0;
		int left = 0;
		int top = 0;
		Image img;

		while (count < 3) {
			switch (count) {
			case 0:
				left = zoomFactor * DrawPositions.LGM0_X;
				top = zoomFactor * DrawPositions.LGM0_Y;
				break;
			case 1:
				left = zoomFactor * DrawPositions.LGM1_X;
				top = zoomFactor * DrawPositions.LGM1_Y;
				break;
			case 2:
				left = zoomFactor * DrawPositions.LGM2_X;
				top = zoomFactor * DrawPositions.LGM2_Y;
				break;
			}
			img = tiles.getSubimage(left, top, zoomFactor
					* DrawPositions.LGM_WIDTH, zoomFactor
					* DrawPositions.LGM_HEIGHT);
			lgmImages[count] = applyMask(gd, img, Color.GREEN);
			count++;
		}
		// Helicopter
		img = tiles.getSubimage(zoomFactor * DrawPositions.LGM_HELICOPTER_X,
				zoomFactor * DrawPositions.LGM_HELICOPTER_Y, zoomFactor
						* DrawPositions.TILE_SIZE_X, zoomFactor
						* DrawPositions.TILE_SIZE_Y);
		lgmImages[3] = applyMask(gd, img, Color.GREEN);

	}

	/**
	 * Sets up an array of tile positions on the image file
	 * 
	 * @param zoomFactor
	 *            Scaling factor
	 */
	private void setupArrays(int zoomFactor) {
		drawPosX[TileNum.DEEP_SEA_SOLID] = zoomFactor
				* DrawPositions.DEEP_SEA_SOLID_X;
		drawPosY[TileNum.DEEP_SEA_SOLID] = zoomFactor
				* DrawPositions.DEEP_SEA_SOLID_Y;
		drawPosX[TileNum.DEEP_SEA_CORN1] = zoomFactor
				* DrawPositions.DEEP_SEA_CORN1_X;
		drawPosY[TileNum.DEEP_SEA_CORN1] = zoomFactor
				* DrawPositions.DEEP_SEA_CORN1_Y;
		drawPosX[TileNum.DEEP_SEA_CORN2] = zoomFactor
				* DrawPositions.DEEP_SEA_CORN2_X;
		drawPosY[TileNum.DEEP_SEA_CORN2] = zoomFactor
				* DrawPositions.DEEP_SEA_CORN2_Y;
		drawPosX[TileNum.DEEP_SEA_CORN3] = zoomFactor
				* DrawPositions.DEEP_SEA_CORN3_X;
		drawPosY[TileNum.DEEP_SEA_CORN3] = zoomFactor
				* DrawPositions.DEEP_SEA_CORN3_Y;
		drawPosX[TileNum.DEEP_SEA_CORN4] = zoomFactor
				* DrawPositions.DEEP_SEA_CORN4_X;
		drawPosY[TileNum.DEEP_SEA_CORN4] = zoomFactor
				* DrawPositions.DEEP_SEA_CORN4_Y;
		drawPosX[TileNum.DEEP_SEA_SIDE1] = zoomFactor
				* DrawPositions.DEEP_SEA_SIDE1_X;
		drawPosY[TileNum.DEEP_SEA_SIDE1] = zoomFactor
				* DrawPositions.DEEP_SEA_SIDE1_Y;
		drawPosX[TileNum.DEEP_SEA_SIDE2] = zoomFactor
				* DrawPositions.DEEP_SEA_SIDE2_X;
		drawPosY[TileNum.DEEP_SEA_SIDE2] = zoomFactor
				* DrawPositions.DEEP_SEA_SIDE2_Y;
		drawPosX[TileNum.DEEP_SEA_SIDE3] = zoomFactor
				* DrawPositions.DEEP_SEA_SIDE3_X;
		drawPosY[TileNum.DEEP_SEA_SIDE3] = zoomFactor
				* DrawPositions.DEEP_SEA_SIDE3_Y;
		drawPosX[TileNum.DEEP_SEA_SIDE4] = zoomFactor
				* DrawPositions.DEEP_SEA_SIDE4_X;
		drawPosY[TileNum.DEEP_SEA_SIDE4] = zoomFactor
				* DrawPositions.DEEP_SEA_SIDE4_Y;

		drawPosX[TileNum.BUILD_SINGLE] = zoomFactor
				* DrawPositions.BUILD_SINGLE_X;
		drawPosY[TileNum.BUILD_SINGLE] = zoomFactor
				* DrawPositions.BUILD_SINGLE_Y;
		drawPosX[TileNum.BUILD_SOLID] = zoomFactor
				* DrawPositions.BUILD_SOLID_X;
		drawPosY[TileNum.BUILD_SOLID] = zoomFactor
				* DrawPositions.BUILD_SOLID_Y;
		drawPosX[TileNum.BUILD_CORNER1] = zoomFactor
				* DrawPositions.BUILD_CORNER1_X;
		drawPosY[TileNum.BUILD_CORNER1] = zoomFactor
				* DrawPositions.BUILD_CORNER1_Y;
		drawPosX[TileNum.BUILD_CORNER2] = zoomFactor
				* DrawPositions.BUILD_CORNER2_X;
		drawPosY[TileNum.BUILD_CORNER2] = zoomFactor
				* DrawPositions.BUILD_CORNER2_Y;
		drawPosX[TileNum.BUILD_CORNER3] = zoomFactor
				* DrawPositions.BUILD_CORNER3_X;
		drawPosY[TileNum.BUILD_CORNER3] = zoomFactor
				* DrawPositions.BUILD_CORNER3_Y;
		drawPosX[TileNum.BUILD_CORNER4] = zoomFactor
				* DrawPositions.BUILD_CORNER4_X;
		drawPosY[TileNum.BUILD_CORNER4] = zoomFactor
				* DrawPositions.BUILD_CORNER4_Y;
		drawPosX[TileNum.BUILD_L1] = zoomFactor * DrawPositions.BUILD_L1_X;
		drawPosY[TileNum.BUILD_L1] = zoomFactor * DrawPositions.BUILD_L1_Y;
		drawPosX[TileNum.BUILD_L2] = zoomFactor * DrawPositions.BUILD_L2_X;
		drawPosY[TileNum.BUILD_L2] = zoomFactor * DrawPositions.BUILD_L2_Y;
		drawPosX[TileNum.BUILD_L3] = zoomFactor * DrawPositions.BUILD_L3_X;
		drawPosY[TileNum.BUILD_L3] = zoomFactor * DrawPositions.BUILD_L3_Y;
		drawPosX[TileNum.BUILD_L4] = zoomFactor * DrawPositions.BUILD_L4_X;
		drawPosY[TileNum.BUILD_L4] = zoomFactor * DrawPositions.BUILD_L4_Y;
		drawPosX[TileNum.BUILD_T1] = zoomFactor * DrawPositions.BUILD_T1_X;
		drawPosY[TileNum.BUILD_T1] = zoomFactor * DrawPositions.BUILD_T1_Y;
		drawPosX[TileNum.BUILD_T2] = zoomFactor * DrawPositions.BUILD_T2_X;
		drawPosY[TileNum.BUILD_T2] = zoomFactor * DrawPositions.BUILD_T2_Y;
		drawPosX[TileNum.BUILD_T3] = zoomFactor * DrawPositions.BUILD_T3_X;
		drawPosY[TileNum.BUILD_T3] = zoomFactor * DrawPositions.BUILD_T3_Y;
		drawPosX[TileNum.BUILD_T4] = zoomFactor * DrawPositions.BUILD_T4_X;
		drawPosY[TileNum.BUILD_T4] = zoomFactor * DrawPositions.BUILD_T4_Y;
		drawPosX[TileNum.BUILD_HORZ] = zoomFactor * DrawPositions.BUILD_HORZ_X;
		drawPosY[TileNum.BUILD_HORZ] = zoomFactor * DrawPositions.BUILD_HORZ_Y;
		drawPosX[TileNum.BUILD_VERT] = zoomFactor * DrawPositions.BUILD_VERT_X;
		drawPosY[TileNum.BUILD_VERT] = zoomFactor * DrawPositions.BUILD_VERT_Y;
		drawPosX[TileNum.BUILD_VERTEND1] = zoomFactor
				* DrawPositions.BUILD_VERTEND1_X;
		drawPosY[TileNum.BUILD_VERTEND1] = zoomFactor
				* DrawPositions.BUILD_VERTEND1_Y;
		drawPosX[TileNum.BUILD_VERTEND2] = zoomFactor
				* DrawPositions.BUILD_VERTEND2_X;
		drawPosY[TileNum.BUILD_VERTEND2] = zoomFactor
				* DrawPositions.BUILD_VERTEND2_Y;
		drawPosX[TileNum.BUILD_HORZEND1] = zoomFactor
				* DrawPositions.BUILD_HORZEND1_X;
		drawPosY[TileNum.BUILD_HORZEND1] = zoomFactor
				* DrawPositions.BUILD_HORZEND1_Y;
		drawPosX[TileNum.BUILD_HORZEND2] = zoomFactor
				* DrawPositions.BUILD_HORZEND2_X;
		drawPosY[TileNum.BUILD_HORZEND2] = zoomFactor
				* DrawPositions.BUILD_HORZEND2_Y;
		drawPosX[TileNum.BUILD_CROSS] = zoomFactor
				* DrawPositions.BUILD_CROSS_X;
		drawPosY[TileNum.BUILD_CROSS] = zoomFactor
				* DrawPositions.BUILD_CROSS_Y;
		drawPosX[TileNum.BUILD_SIDE1] = zoomFactor
				* DrawPositions.BUILD_SIDE1_X;
		drawPosY[TileNum.BUILD_SIDE1] = zoomFactor
				* DrawPositions.BUILD_SIDE1_Y;
		drawPosX[TileNum.BUILD_SIDE2] = zoomFactor
				* DrawPositions.BUILD_SIDE2_X;
		drawPosY[TileNum.BUILD_SIDE2] = zoomFactor
				* DrawPositions.BUILD_SIDE2_Y;
		drawPosX[TileNum.BUILD_SIDE3] = zoomFactor
				* DrawPositions.BUILD_SIDE3_X;
		drawPosY[TileNum.BUILD_SIDE3] = zoomFactor
				* DrawPositions.BUILD_SIDE3_Y;
		drawPosX[TileNum.BUILD_SIDE4] = zoomFactor
				* DrawPositions.BUILD_SIDE4_X;
		drawPosY[TileNum.BUILD_SIDE4] = zoomFactor
				* DrawPositions.BUILD_SIDE4_Y;
		drawPosX[TileNum.BUILD_SIDECORN1] = zoomFactor
				* DrawPositions.BUILD_SIDECORN1_X;
		drawPosY[TileNum.BUILD_SIDECORN1] = zoomFactor
				* DrawPositions.BUILD_SIDECORN1_Y;
		drawPosX[TileNum.BUILD_SIDECORN2] = zoomFactor
				* DrawPositions.BUILD_SIDECORN2_X;
		drawPosY[TileNum.BUILD_SIDECORN2] = zoomFactor
				* DrawPositions.BUILD_SIDECORN2_Y;
		drawPosX[TileNum.BUILD_SIDECORN3] = zoomFactor
				* DrawPositions.BUILD_SIDECORN3_X;
		drawPosY[TileNum.BUILD_SIDECORN3] = zoomFactor
				* DrawPositions.BUILD_SIDECORN3_Y;
		drawPosX[TileNum.BUILD_SIDECORN4] = zoomFactor
				* DrawPositions.BUILD_SIDECORN4_X;
		drawPosY[TileNum.BUILD_SIDECORN4] = zoomFactor
				* DrawPositions.BUILD_SIDECORN4_Y;
		drawPosX[TileNum.BUILD_SIDECORN5] = zoomFactor
				* DrawPositions.BUILD_SIDECORN5_X;
		drawPosY[TileNum.BUILD_SIDECORN5] = zoomFactor
				* DrawPositions.BUILD_SIDECORN5_Y;
		drawPosX[TileNum.BUILD_SIDECORN6] = zoomFactor
				* DrawPositions.BUILD_SIDECORN6_X;
		drawPosY[TileNum.BUILD_SIDECORN6] = zoomFactor
				* DrawPositions.BUILD_SIDECORN6_Y;
		drawPosX[TileNum.BUILD_SIDECORN7] = zoomFactor
				* DrawPositions.BUILD_SIDECORN7_X;
		drawPosY[TileNum.BUILD_SIDECORN7] = zoomFactor
				* DrawPositions.BUILD_SIDECORN7_Y;
		drawPosX[TileNum.BUILD_SIDECORN8] = zoomFactor
				* DrawPositions.BUILD_SIDECORN8_X;
		drawPosY[TileNum.BUILD_SIDECORN8] = zoomFactor
				* DrawPositions.BUILD_SIDECORN8_Y;
		drawPosX[TileNum.BUILD_SIDECORN9] = zoomFactor
				* DrawPositions.BUILD_SIDECORN9_X;
		drawPosY[TileNum.BUILD_SIDECORN9] = zoomFactor
				* DrawPositions.BUILD_SIDECORN9_Y;
		drawPosX[TileNum.BUILD_SIDECORN10] = zoomFactor
				* DrawPositions.BUILD_SIDECORN10_X;
		drawPosY[TileNum.BUILD_SIDECORN10] = zoomFactor
				* DrawPositions.BUILD_SIDECORN10_Y;
		drawPosX[TileNum.BUILD_SIDECORN11] = zoomFactor
				* DrawPositions.BUILD_SIDECORN11_X;
		drawPosY[TileNum.BUILD_SIDECORN11] = zoomFactor
				* DrawPositions.BUILD_SIDECORN11_Y;
		drawPosX[TileNum.BUILD_SIDECORN12] = zoomFactor
				* DrawPositions.BUILD_SIDECORN12_X;
		drawPosY[TileNum.BUILD_SIDECORN12] = zoomFactor
				* DrawPositions.BUILD_SIDECORN12_Y;
		drawPosX[TileNum.BUILD_SIDECORN13] = zoomFactor
				* DrawPositions.BUILD_SIDECORN13_X;
		drawPosY[TileNum.BUILD_SIDECORN13] = zoomFactor
				* DrawPositions.BUILD_SIDECORN13_Y;
		drawPosX[TileNum.BUILD_SIDECORN14] = zoomFactor
				* DrawPositions.BUILD_SIDECORN14_X;
		drawPosY[TileNum.BUILD_SIDECORN14] = zoomFactor
				* DrawPositions.BUILD_SIDECORN14_Y;
		drawPosX[TileNum.BUILD_SIDECORN15] = zoomFactor
				* DrawPositions.BUILD_SIDECORN15_X;
		drawPosY[TileNum.BUILD_SIDECORN15] = zoomFactor
				* DrawPositions.BUILD_SIDECORN15_Y;
		drawPosX[TileNum.BUILD_SIDECORN16] = zoomFactor
				* DrawPositions.BUILD_SIDECORN16_X;
		drawPosY[TileNum.BUILD_SIDECORN16] = zoomFactor
				* DrawPositions.BUILD_SIDECORN16_Y;
		drawPosX[TileNum.BUILD_TWIST1] = zoomFactor
				* DrawPositions.BUILD_TWIST1_X;
		drawPosY[TileNum.BUILD_TWIST1] = zoomFactor
				* DrawPositions.BUILD_TWIST1_Y;
		drawPosX[TileNum.BUILD_TWIST2] = zoomFactor
				* DrawPositions.BUILD_TWIST2_X;
		drawPosY[TileNum.BUILD_TWIST2] = zoomFactor
				* DrawPositions.BUILD_TWIST2_Y;
		drawPosX[TileNum.BUILD_MOST1] = zoomFactor
				* DrawPositions.BUILD_MOST1_X;
		drawPosY[TileNum.BUILD_MOST1] = zoomFactor
				* DrawPositions.BUILD_MOST1_Y;
		drawPosX[TileNum.BUILD_MOST2] = zoomFactor
				* DrawPositions.BUILD_MOST2_X;
		drawPosY[TileNum.BUILD_MOST2] = zoomFactor
				* DrawPositions.BUILD_MOST2_Y;
		drawPosX[TileNum.BUILD_MOST3] = zoomFactor
				* DrawPositions.BUILD_MOST3_X;
		drawPosY[TileNum.BUILD_MOST3] = zoomFactor
				* DrawPositions.BUILD_MOST3_Y;
		drawPosX[TileNum.BUILD_MOST4] = zoomFactor
				* DrawPositions.BUILD_MOST4_X;
		drawPosY[TileNum.BUILD_MOST4] = zoomFactor
				* DrawPositions.BUILD_MOST4_Y;

		drawPosX[TileNum.RIVER_END1] = zoomFactor * DrawPositions.RIVER_END1_X;
		drawPosY[TileNum.RIVER_END1] = zoomFactor * DrawPositions.RIVER_END1_Y;
		drawPosX[TileNum.RIVER_END2] = zoomFactor * DrawPositions.RIVER_END2_X;
		drawPosY[TileNum.RIVER_END2] = zoomFactor * DrawPositions.RIVER_END2_Y;
		drawPosX[TileNum.RIVER_END3] = zoomFactor * DrawPositions.RIVER_END3_X;
		drawPosY[TileNum.RIVER_END3] = zoomFactor * DrawPositions.RIVER_END3_Y;
		drawPosX[TileNum.RIVER_END4] = zoomFactor * DrawPositions.RIVER_END4_X;
		drawPosY[TileNum.RIVER_END4] = zoomFactor * DrawPositions.RIVER_END4_Y;
		drawPosX[TileNum.RIVER_SOLID] = zoomFactor
				* DrawPositions.RIVER_SOLID_X;
		drawPosY[TileNum.RIVER_SOLID] = zoomFactor
				* DrawPositions.RIVER_SOLID_Y;
		drawPosX[TileNum.RIVER_SURROUND] = zoomFactor
				* DrawPositions.RIVER_SURROUND_X;
		drawPosY[TileNum.RIVER_SURROUND] = zoomFactor
				* DrawPositions.RIVER_SURROUND_Y;
		drawPosX[TileNum.RIVER_SIDE1] = zoomFactor
				* DrawPositions.RIVER_SIDE1_X;
		drawPosY[TileNum.RIVER_SIDE1] = zoomFactor
				* DrawPositions.RIVER_SIDE1_Y;
		drawPosX[TileNum.RIVER_SIDE2] = zoomFactor
				* DrawPositions.RIVER_SIDE2_X;
		drawPosY[TileNum.RIVER_SIDE2] = zoomFactor
				* DrawPositions.RIVER_SIDE2_Y;
		drawPosX[TileNum.RIVER_ONESIDE1] = zoomFactor
				* DrawPositions.RIVER_ONESIDE1_X;
		drawPosY[TileNum.RIVER_ONESIDE1] = zoomFactor
				* DrawPositions.RIVER_ONESIDE1_Y;
		drawPosX[TileNum.RIVER_ONESIDE2] = zoomFactor
				* DrawPositions.RIVER_ONESIDE2_X;
		drawPosY[TileNum.RIVER_ONESIDE2] = zoomFactor
				* DrawPositions.RIVER_ONESIDE2_Y;
		drawPosX[TileNum.RIVER_ONESIDE3] = zoomFactor
				* DrawPositions.RIVER_ONESIDE3_X;
		drawPosY[TileNum.RIVER_ONESIDE3] = zoomFactor
				* DrawPositions.RIVER_ONESIDE3_Y;
		drawPosX[TileNum.RIVER_ONESIDE4] = zoomFactor
				* DrawPositions.RIVER_ONESIDE4_X;
		drawPosY[TileNum.RIVER_ONESIDE4] = zoomFactor
				* DrawPositions.RIVER_ONESIDE4_Y;
		drawPosX[TileNum.RIVER_CORN1] = zoomFactor
				* DrawPositions.RIVER_CORN1_X;
		drawPosY[TileNum.RIVER_CORN1] = zoomFactor
				* DrawPositions.RIVER_CORN1_Y;
		drawPosX[TileNum.RIVER_CORN2] = zoomFactor
				* DrawPositions.RIVER_CORN2_X;
		drawPosY[TileNum.RIVER_CORN2] = zoomFactor
				* DrawPositions.RIVER_CORN2_Y;
		drawPosX[TileNum.RIVER_CORN3] = zoomFactor
				* DrawPositions.RIVER_CORN3_X;
		drawPosY[TileNum.RIVER_CORN3] = zoomFactor
				* DrawPositions.RIVER_CORN3_Y;
		drawPosX[TileNum.RIVER_CORN4] = zoomFactor
				* DrawPositions.RIVER_CORN4_X;
		drawPosY[TileNum.RIVER_CORN4] = zoomFactor
				* DrawPositions.RIVER_CORN4_Y;

		drawPosX[TileNum.SWAMP] = zoomFactor * DrawPositions.SWAMP_X;
		drawPosY[TileNum.SWAMP] = zoomFactor * DrawPositions.SWAMP_Y;

		drawPosX[TileNum.ROAD_CORNER1] = zoomFactor
				* DrawPositions.ROAD_CORNER1_X;
		drawPosY[TileNum.ROAD_CORNER1] = zoomFactor
				* DrawPositions.ROAD_CORNER1_Y;
		drawPosX[TileNum.ROAD_CORNER2] = zoomFactor
				* DrawPositions.ROAD_CORNER2_X;
		drawPosY[TileNum.ROAD_CORNER2] = zoomFactor
				* DrawPositions.ROAD_CORNER2_Y;
		drawPosX[TileNum.ROAD_CORNER3] = zoomFactor
				* DrawPositions.ROAD_CORNER3_X;
		drawPosY[TileNum.ROAD_CORNER3] = zoomFactor
				* DrawPositions.ROAD_CORNER3_Y;
		drawPosX[TileNum.ROAD_CORNER4] = zoomFactor
				* DrawPositions.ROAD_CORNER4_X;
		drawPosY[TileNum.ROAD_CORNER4] = zoomFactor
				* DrawPositions.ROAD_CORNER4_Y;
		drawPosX[TileNum.ROAD_CORNER5] = zoomFactor
				* DrawPositions.ROAD_CORNER5_X;
		drawPosY[TileNum.ROAD_CORNER5] = zoomFactor
				* DrawPositions.ROAD_CORNER5_Y;
		drawPosX[TileNum.ROAD_CORNER6] = zoomFactor
				* DrawPositions.ROAD_CORNER6_X;
		drawPosY[TileNum.ROAD_CORNER6] = zoomFactor
				* DrawPositions.ROAD_CORNER6_Y;
		drawPosX[TileNum.ROAD_CORNER7] = zoomFactor
				* DrawPositions.ROAD_CORNER7_X;
		drawPosY[TileNum.ROAD_CORNER7] = zoomFactor
				* DrawPositions.ROAD_CORNER7_Y;
		drawPosX[TileNum.ROAD_CORNER8] = zoomFactor
				* DrawPositions.ROAD_CORNER8_X;
		drawPosY[TileNum.ROAD_CORNER8] = zoomFactor
				* DrawPositions.ROAD_CORNER8_Y;

		drawPosX[TileNum.ROAD_SIDE1] = zoomFactor * DrawPositions.ROAD_SIDE1_X;
		drawPosY[TileNum.ROAD_SIDE1] = zoomFactor * DrawPositions.ROAD_SIDE1_Y;
		drawPosX[TileNum.ROAD_SIDE2] = zoomFactor * DrawPositions.ROAD_SIDE2_X;
		drawPosY[TileNum.ROAD_SIDE2] = zoomFactor * DrawPositions.ROAD_SIDE2_Y;
		drawPosX[TileNum.ROAD_SIDE3] = zoomFactor * DrawPositions.ROAD_SIDE3_X;
		drawPosY[TileNum.ROAD_SIDE3] = zoomFactor * DrawPositions.ROAD_SIDE3_Y;
		drawPosX[TileNum.ROAD_SIDE4] = zoomFactor * DrawPositions.ROAD_SIDE4_X;
		drawPosY[TileNum.ROAD_SIDE4] = zoomFactor * DrawPositions.ROAD_SIDE4_Y;
		drawPosX[TileNum.ROAD_SOLID] = zoomFactor * DrawPositions.ROAD_SOLID_X;
		drawPosY[TileNum.ROAD_SOLID] = zoomFactor * DrawPositions.ROAD_SOLID_Y;
		drawPosX[TileNum.ROAD_CROSS] = zoomFactor * DrawPositions.ROAD_CROSS_X;
		drawPosY[TileNum.ROAD_CROSS] = zoomFactor * DrawPositions.ROAD_CROSS_Y;
		drawPosX[TileNum.ROAD_T1] = zoomFactor * DrawPositions.ROAD_T1_X;
		drawPosY[TileNum.ROAD_T1] = zoomFactor * DrawPositions.ROAD_T1_Y;
		drawPosX[TileNum.ROAD_T2] = zoomFactor * DrawPositions.ROAD_T2_X;
		drawPosY[TileNum.ROAD_T2] = zoomFactor * DrawPositions.ROAD_T2_Y;
		drawPosX[TileNum.ROAD_T3] = zoomFactor * DrawPositions.ROAD_T3_X;
		drawPosY[TileNum.ROAD_T3] = zoomFactor * DrawPositions.ROAD_T3_Y;
		drawPosX[TileNum.ROAD_T4] = zoomFactor * DrawPositions.ROAD_T4_X;
		drawPosY[TileNum.ROAD_T4] = zoomFactor * DrawPositions.ROAD_T4_Y;
		drawPosX[TileNum.ROAD_HORZ] = zoomFactor * DrawPositions.ROAD_HORZ_X;
		drawPosY[TileNum.ROAD_HORZ] = zoomFactor * DrawPositions.ROAD_HORZ_Y;
		drawPosX[TileNum.ROAD_VERT] = zoomFactor * DrawPositions.ROAD_VERT_X;
		drawPosY[TileNum.ROAD_VERT] = zoomFactor * DrawPositions.ROAD_VERT_Y;
		drawPosX[TileNum.ROAD_WATER1] = zoomFactor
				* DrawPositions.ROAD_WATER1_X;
		drawPosY[TileNum.ROAD_WATER1] = zoomFactor
				* DrawPositions.ROAD_WATER1_Y;
		drawPosX[TileNum.ROAD_WATER2] = zoomFactor
				* DrawPositions.ROAD_WATER2_X;
		drawPosY[TileNum.ROAD_WATER2] = zoomFactor
				* DrawPositions.ROAD_WATER2_Y;
		drawPosX[TileNum.ROAD_WATER3] = zoomFactor
				* DrawPositions.ROAD_WATER3_X;
		drawPosY[TileNum.ROAD_WATER3] = zoomFactor
				* DrawPositions.ROAD_WATER3_Y;
		drawPosX[TileNum.ROAD_WATER4] = zoomFactor
				* DrawPositions.ROAD_WATER4_X;
		drawPosY[TileNum.ROAD_WATER4] = zoomFactor
				* DrawPositions.ROAD_WATER4_Y;
		drawPosX[TileNum.ROAD_WATER5] = zoomFactor
				* DrawPositions.ROAD_WATER5_X;
		drawPosY[TileNum.ROAD_WATER5] = zoomFactor
				* DrawPositions.ROAD_WATER5_Y;
		drawPosX[TileNum.ROAD_WATER6] = zoomFactor
				* DrawPositions.ROAD_WATER6_X;
		drawPosY[TileNum.ROAD_WATER6] = zoomFactor
				* DrawPositions.ROAD_WATER6_Y;
		drawPosX[TileNum.ROAD_WATER7] = zoomFactor
				* DrawPositions.ROAD_WATER7_X;
		drawPosY[TileNum.ROAD_WATER7] = zoomFactor
				* DrawPositions.ROAD_WATER7_Y;
		drawPosX[TileNum.ROAD_WATER8] = zoomFactor
				* DrawPositions.ROAD_WATER8_X;
		drawPosY[TileNum.ROAD_WATER8] = zoomFactor
				* DrawPositions.ROAD_WATER8_Y;
		drawPosX[TileNum.ROAD_WATER9] = zoomFactor
				* DrawPositions.ROAD_WATER9_X;
		drawPosY[TileNum.ROAD_WATER9] = zoomFactor
				* DrawPositions.ROAD_WATER9_Y;
		drawPosX[TileNum.ROAD_WATER10] = zoomFactor
				* DrawPositions.ROAD_WATER10_X;
		drawPosY[TileNum.ROAD_WATER10] = zoomFactor
				* DrawPositions.ROAD_WATER10_Y;
		drawPosX[TileNum.ROAD_WATER11] = zoomFactor
				* DrawPositions.ROAD_WATER11_X;
		drawPosY[TileNum.ROAD_WATER11] = zoomFactor
				* DrawPositions.ROAD_WATER11_Y;

		drawPosX[TileNum.PILL_EVIL_15] = zoomFactor
				* DrawPositions.PILL_EVIL15_X;
		drawPosY[TileNum.PILL_EVIL_15] = zoomFactor
				* DrawPositions.PILL_EVIL15_Y;
		drawPosX[TileNum.PILL_EVIL_14] = zoomFactor
				* DrawPositions.PILL_EVIL14_X;
		drawPosY[TileNum.PILL_EVIL_14] = zoomFactor
				* DrawPositions.PILL_EVIL14_Y;
		drawPosX[TileNum.PILL_EVIL_13] = zoomFactor
				* DrawPositions.PILL_EVIL13_X;
		drawPosY[TileNum.PILL_EVIL_13] = zoomFactor
				* DrawPositions.PILL_EVIL13_Y;
		drawPosX[TileNum.PILL_EVIL_12] = zoomFactor
				* DrawPositions.PILL_EVIL12_X;
		drawPosY[TileNum.PILL_EVIL_12] = zoomFactor
				* DrawPositions.PILL_EVIL12_Y;
		drawPosX[TileNum.PILL_EVIL_11] = zoomFactor
				* DrawPositions.PILL_EVIL11_X;
		drawPosY[TileNum.PILL_EVIL_11] = zoomFactor
				* DrawPositions.PILL_EVIL11_Y;
		drawPosX[TileNum.PILL_EVIL_10] = zoomFactor
				* DrawPositions.PILL_EVIL10_X;
		drawPosY[TileNum.PILL_EVIL_10] = zoomFactor
				* DrawPositions.PILL_EVIL10_Y;
		drawPosX[TileNum.PILL_EVIL_9] = zoomFactor * DrawPositions.PILL_EVIL9_X;
		drawPosY[TileNum.PILL_EVIL_9] = zoomFactor * DrawPositions.PILL_EVIL9_Y;
		drawPosX[TileNum.PILL_EVIL_8] = zoomFactor * DrawPositions.PILL_EVIL8_X;
		drawPosY[TileNum.PILL_EVIL_8] = zoomFactor * DrawPositions.PILL_EVIL8_Y;
		drawPosX[TileNum.PILL_EVIL_7] = zoomFactor * DrawPositions.PILL_EVIL7_X;
		drawPosY[TileNum.PILL_EVIL_7] = zoomFactor * DrawPositions.PILL_EVIL7_Y;
		drawPosX[TileNum.PILL_EVIL_6] = zoomFactor * DrawPositions.PILL_EVIL6_X;
		drawPosY[TileNum.PILL_EVIL_6] = zoomFactor * DrawPositions.PILL_EVIL6_Y;
		drawPosX[TileNum.PILL_EVIL_5] = zoomFactor * DrawPositions.PILL_EVIL5_X;
		drawPosY[TileNum.PILL_EVIL_5] = zoomFactor * DrawPositions.PILL_EVIL5_Y;
		drawPosX[TileNum.PILL_EVIL_4] = zoomFactor * DrawPositions.PILL_EVIL4_X;
		drawPosY[TileNum.PILL_EVIL_4] = zoomFactor * DrawPositions.PILL_EVIL4_Y;
		drawPosX[TileNum.PILL_EVIL_3] = zoomFactor * DrawPositions.PILL_EVIL3_X;
		drawPosY[TileNum.PILL_EVIL_3] = zoomFactor * DrawPositions.PILL_EVIL3_Y;
		drawPosX[TileNum.PILL_EVIL_2] = zoomFactor * DrawPositions.PILL_EVIL2_X;
		drawPosY[TileNum.PILL_EVIL_2] = zoomFactor * DrawPositions.PILL_EVIL2_Y;
		drawPosX[TileNum.PILL_EVIL_1] = zoomFactor * DrawPositions.PILL_EVIL1_X;
		drawPosY[TileNum.PILL_EVIL_1] = zoomFactor * DrawPositions.PILL_EVIL1_Y;
		drawPosX[TileNum.PILL_EVIL_0] = zoomFactor * DrawPositions.PILL_EVIL0_X;
		drawPosY[TileNum.PILL_EVIL_0] = zoomFactor * DrawPositions.PILL_EVIL0_Y;

		drawPosX[TileNum.PILL_GOOD_15] = zoomFactor
				* DrawPositions.PILL_GOOD15_X;
		drawPosY[TileNum.PILL_GOOD_15] = zoomFactor
				* DrawPositions.PILL_GOOD15_Y;
		drawPosX[TileNum.PILL_GOOD_14] = zoomFactor
				* DrawPositions.PILL_GOOD14_X;
		drawPosY[TileNum.PILL_GOOD_14] = zoomFactor
				* DrawPositions.PILL_GOOD14_Y;
		drawPosX[TileNum.PILL_GOOD_13] = zoomFactor
				* DrawPositions.PILL_GOOD13_X;
		drawPosY[TileNum.PILL_GOOD_13] = zoomFactor
				* DrawPositions.PILL_GOOD13_Y;
		drawPosX[TileNum.PILL_GOOD_12] = zoomFactor
				* DrawPositions.PILL_GOOD12_X;
		drawPosY[TileNum.PILL_GOOD_12] = zoomFactor
				* DrawPositions.PILL_GOOD12_Y;
		drawPosX[TileNum.PILL_GOOD_11] = zoomFactor
				* DrawPositions.PILL_GOOD11_X;
		drawPosY[TileNum.PILL_GOOD_11] = zoomFactor
				* DrawPositions.PILL_GOOD11_Y;
		drawPosX[TileNum.PILL_GOOD_10] = zoomFactor
				* DrawPositions.PILL_GOOD10_X;
		drawPosY[TileNum.PILL_GOOD_10] = zoomFactor
				* DrawPositions.PILL_GOOD10_Y;
		drawPosX[TileNum.PILL_GOOD_9] = zoomFactor * DrawPositions.PILL_GOOD9_X;
		drawPosY[TileNum.PILL_GOOD_9] = zoomFactor * DrawPositions.PILL_GOOD9_Y;
		drawPosX[TileNum.PILL_GOOD_8] = zoomFactor * DrawPositions.PILL_GOOD8_X;
		drawPosY[TileNum.PILL_GOOD_8] = zoomFactor * DrawPositions.PILL_GOOD8_Y;
		drawPosX[TileNum.PILL_GOOD_7] = zoomFactor * DrawPositions.PILL_GOOD7_X;
		drawPosY[TileNum.PILL_GOOD_7] = zoomFactor * DrawPositions.PILL_GOOD7_Y;
		drawPosX[TileNum.PILL_GOOD_6] = zoomFactor * DrawPositions.PILL_GOOD6_X;
		drawPosY[TileNum.PILL_GOOD_6] = zoomFactor * DrawPositions.PILL_GOOD6_Y;
		drawPosX[TileNum.PILL_GOOD_5] = zoomFactor * DrawPositions.PILL_GOOD5_X;
		drawPosY[TileNum.PILL_GOOD_5] = zoomFactor * DrawPositions.PILL_GOOD5_Y;
		drawPosX[TileNum.PILL_GOOD_4] = zoomFactor * DrawPositions.PILL_GOOD4_X;
		drawPosY[TileNum.PILL_GOOD_4] = zoomFactor * DrawPositions.PILL_GOOD4_Y;
		drawPosX[TileNum.PILL_GOOD_3] = zoomFactor * DrawPositions.PILL_GOOD3_X;
		drawPosY[TileNum.PILL_GOOD_3] = zoomFactor * DrawPositions.PILL_GOOD3_Y;
		drawPosX[TileNum.PILL_GOOD_2] = zoomFactor * DrawPositions.PILL_GOOD2_X;
		drawPosY[TileNum.PILL_GOOD_2] = zoomFactor * DrawPositions.PILL_GOOD2_Y;
		drawPosX[TileNum.PILL_GOOD_1] = zoomFactor * DrawPositions.PILL_GOOD1_X;
		drawPosY[TileNum.PILL_GOOD_1] = zoomFactor * DrawPositions.PILL_GOOD1_Y;
		drawPosX[TileNum.PILL_GOOD_0] = zoomFactor * DrawPositions.PILL_GOOD0_X;
		drawPosY[TileNum.PILL_GOOD_0] = zoomFactor * DrawPositions.PILL_GOOD0_Y;

		drawPosX[TileNum.BASE_GOOD] = zoomFactor * DrawPositions.BASE_GOOD_X;
		drawPosY[TileNum.BASE_GOOD] = zoomFactor * DrawPositions.BASE_GOOD_Y;
		drawPosX[TileNum.BASE_NEUTRAL] = zoomFactor
				* DrawPositions.BASE_NEUTRAL_X;
		drawPosY[TileNum.BASE_NEUTRAL] = zoomFactor
				* DrawPositions.BASE_NEUTRAL_Y;
		drawPosX[TileNum.BASE_EVIL] = zoomFactor * DrawPositions.BASE_EVIL_X;
		drawPosY[TileNum.BASE_EVIL] = zoomFactor * DrawPositions.BASE_EVIL_Y;

		drawPosX[TileNum.FOREST] = zoomFactor * DrawPositions.FOREST_X;
		drawPosY[TileNum.FOREST] = zoomFactor * DrawPositions.FOREST_Y;
		drawPosX[TileNum.FOREST_SINGLE] = zoomFactor
				* DrawPositions.FOREST_SINGLE_X;
		drawPosY[TileNum.FOREST_SINGLE] = zoomFactor
				* DrawPositions.FOREST_SINGLE_Y;
		drawPosX[TileNum.FOREST_BR] = zoomFactor * DrawPositions.FOREST_BR_X;
		drawPosY[TileNum.FOREST_BR] = zoomFactor * DrawPositions.FOREST_BR_Y;
		drawPosX[TileNum.FOREST_BL] = zoomFactor * DrawPositions.FOREST_BL_X;
		drawPosY[TileNum.FOREST_BL] = zoomFactor * DrawPositions.FOREST_BL_Y;
		drawPosX[TileNum.FOREST_AR] = zoomFactor * DrawPositions.FOREST_AR_X;
		drawPosY[TileNum.FOREST_AR] = zoomFactor * DrawPositions.FOREST_AR_Y;
		drawPosX[TileNum.FOREST_AL] = zoomFactor * DrawPositions.FOREST_AL_X;
		drawPosY[TileNum.FOREST_AL] = zoomFactor * DrawPositions.FOREST_AL_Y;
		drawPosX[TileNum.FOREST_ABOVE] = zoomFactor
				* DrawPositions.FOREST_ABOVE_X;
		drawPosY[TileNum.FOREST_ABOVE] = zoomFactor
				* DrawPositions.FOREST_ABOVE_Y;
		drawPosX[TileNum.FOREST_BELOW] = zoomFactor
				* DrawPositions.FOREST_BELOW_X;
		drawPosY[TileNum.FOREST_BELOW] = zoomFactor
				* DrawPositions.FOREST_BELOW_Y;
		drawPosX[TileNum.FOREST_LEFT] = zoomFactor
				* DrawPositions.FOREST_LEFT_X;
		drawPosY[TileNum.FOREST_LEFT] = zoomFactor
				* DrawPositions.FOREST_LEFT_Y;
		drawPosX[TileNum.FOREST_RIGHT] = zoomFactor
				* DrawPositions.FOREST_RIGHT_X;
		drawPosY[TileNum.FOREST_RIGHT] = zoomFactor
				* DrawPositions.FOREST_RIGHT_Y;

		drawPosX[TileNum.CRATER] = zoomFactor * DrawPositions.CRATER_X;
		drawPosY[TileNum.CRATER] = zoomFactor * DrawPositions.CRATER_Y;
		drawPosX[TileNum.CRATER_SINGLE] = zoomFactor
				* DrawPositions.CRATER_SINGLE_X;
		drawPosY[TileNum.CRATER_SINGLE] = zoomFactor
				* DrawPositions.CRATER_SINGLE_Y;
		drawPosX[TileNum.CRATER_BR] = zoomFactor * DrawPositions.CRATER_BR_X;
		drawPosY[TileNum.CRATER_BR] = zoomFactor * DrawPositions.CRATER_BR_Y;
		drawPosX[TileNum.CRATER_BL] = zoomFactor * DrawPositions.CRATER_BL_X;
		drawPosY[TileNum.CRATER_BL] = zoomFactor * DrawPositions.CRATER_BL_Y;
		drawPosX[TileNum.CRATER_AR] = zoomFactor * DrawPositions.CRATER_AR_X;
		drawPosY[TileNum.CRATER_AR] = zoomFactor * DrawPositions.CRATER_AR_Y;
		drawPosX[TileNum.CRATER_AL] = zoomFactor * DrawPositions.CRATER_AL_X;
		drawPosY[TileNum.CRATER_AL] = zoomFactor * DrawPositions.CRATER_AL_Y;
		drawPosX[TileNum.CRATER_ABOVE] = zoomFactor
				* DrawPositions.CRATER_ABOVE_X;
		drawPosY[TileNum.CRATER_ABOVE] = zoomFactor
				* DrawPositions.CRATER_ABOVE_Y;
		drawPosX[TileNum.CRATER_BELOW] = zoomFactor
				* DrawPositions.CRATER_BELOW_X;
		drawPosY[TileNum.CRATER_BELOW] = zoomFactor
				* DrawPositions.CRATER_BELOW_Y;
		drawPosX[TileNum.CRATER_LEFT] = zoomFactor
				* DrawPositions.CRATER_LEFT_X;
		drawPosY[TileNum.CRATER_LEFT] = zoomFactor
				* DrawPositions.CRATER_LEFT_Y;
		drawPosX[TileNum.CRATER_RIGHT] = zoomFactor
				* DrawPositions.CRATER_RIGHT_X;
		drawPosY[TileNum.CRATER_RIGHT] = zoomFactor
				* DrawPositions.CRATER_RIGHT_Y;

		drawPosX[TileNum.RUBBLE] = zoomFactor * DrawPositions.RUBBLE_X;
		drawPosY[TileNum.RUBBLE] = zoomFactor * DrawPositions.RUBBLE_Y;
		drawPosX[TileNum.GRASS] = zoomFactor * DrawPositions.GRASS_X;
		drawPosY[TileNum.GRASS] = zoomFactor * DrawPositions.GRASS_Y;
		drawPosX[TileNum.HALFBUILDING] = zoomFactor
				* DrawPositions.SHOT_BUILDING_X;
		drawPosY[TileNum.HALFBUILDING] = zoomFactor
				* DrawPositions.SHOT_BUILDING_Y;

		drawPosX[TileNum.BOAT_0] = zoomFactor * DrawPositions.BOAT0_X;
		drawPosY[TileNum.BOAT_0] = zoomFactor * DrawPositions.BOAT0_Y;
		drawPosX[TileNum.BOAT_1] = zoomFactor * DrawPositions.BOAT1_X;
		drawPosY[TileNum.BOAT_1] = zoomFactor * DrawPositions.BOAT1_Y;
		drawPosX[TileNum.BOAT_2] = zoomFactor * DrawPositions.BOAT2_X;
		drawPosY[TileNum.BOAT_2] = zoomFactor * DrawPositions.BOAT2_Y;
		drawPosX[TileNum.BOAT_3] = zoomFactor * DrawPositions.BOAT3_X;
		drawPosY[TileNum.BOAT_3] = zoomFactor * DrawPositions.BOAT3_Y;
		drawPosX[TileNum.BOAT_4] = zoomFactor * DrawPositions.BOAT4_X;
		drawPosY[TileNum.BOAT_4] = zoomFactor * DrawPositions.BOAT4_Y;
		drawPosX[TileNum.BOAT_5] = zoomFactor * DrawPositions.BOAT5_X;
		drawPosY[TileNum.BOAT_5] = zoomFactor * DrawPositions.BOAT5_Y;
		drawPosX[TileNum.BOAT_6] = zoomFactor * DrawPositions.BOAT6_X;
		drawPosY[TileNum.BOAT_6] = zoomFactor * DrawPositions.BOAT6_Y;
		drawPosX[TileNum.BOAT_7] = zoomFactor * DrawPositions.BOAT7_X;
		drawPosY[TileNum.BOAT_7] = zoomFactor * DrawPositions.BOAT7_Y;

		/* Draw Tank frames */

		/*
		 * Do I want to do this? drawTankPosX[TileNum.TANK_SELF_0] = zoomFactor *
		 * DrawPositions.TANK_SELF_0_X; drawTankPosY[TileNum.TANK_SELF_0] =
		 * zoomFactor * DrawPositions.TANK_SELF_0_Y;
		 * drawTankPosX[TileNum.TANK_SELF_1] = zoomFactor *
		 * DrawPositions.TANK_SELF_1_X; drawTankPosY[TileNum.TANK_SELF_1] =
		 * zoomFactor * DrawPositions.TANK_SELF_1_Y;
		 * drawTankPosX[TileNum.TANK_SELF_2] = zoomFactor *
		 * DrawPositions.TANK_SELF_2_X; drawTankPosY[TileNum.TANK_SELF_2] =
		 * zoomFactor * DrawPositions.TANK_SELF_2_Y;
		 * drawTankPosX[TileNum.TANK_SELF_3] = zoomFactor *
		 * DrawPositions.TANK_SELF_3_X; drawTankPosY[TileNum.TANK_SELF_3] =
		 * zoomFactor * DrawPositions.TANK_SELF_3_Y;
		 * drawTankPosX[TileNum.TANK_SELF_4] = zoomFactor *
		 * DrawPositions.TANK_SELF_4_X; drawTankPosY[TileNum.TANK_SELF_4] =
		 * zoomFactor * DrawPositions.TANK_SELF_4_Y;
		 * drawTankPosX[TileNum.TANK_SELF_5] = zoomFactor *
		 * DrawPositions.TANK_SELF_5_X; drawTankPosY[TileNum.TANK_SELF_5] =
		 * zoomFactor * DrawPositions.TANK_SELF_5_Y;
		 * drawTankPosX[TileNum.TANK_SELF_6] = zoomFactor *
		 * DrawPositions.TANK_SELF_6_X; drawTankPosY[TileNum.TANK_SELF_6] =
		 * zoomFactor * DrawPositions.TANK_SELF_6_Y;
		 * drawTankPosX[TileNum.TANK_SELF_7] = zoomFactor *
		 * DrawPositions.TANK_SELF_7_X; drawTankPosY[TileNum.TANK_SELF_7] =
		 * zoomFactor * DrawPositions.TANK_SELF_7_Y;
		 * drawTankPosX[TileNum.TANK_SELF_8] = zoomFactor *
		 * DrawPositions.TANK_SELF_8_X; drawTankPosY[TileNum.TANK_SELF_8] =
		 * zoomFactor * DrawPositions.TANK_SELF_8_Y;
		 * drawTankPosX[TileNum.TANK_SELF_9] = zoomFactor *
		 * DrawPositions.TANK_SELF_9_X; drawTankPosY[TileNum.TANK_SELF_9] =
		 * zoomFactor * DrawPositions.TANK_SELF_9_Y;
		 * drawTankPosX[TileNum.TANK_SELF_10] = zoomFactor *
		 * DrawPositions.TANK_SELF_10_X; drawTankPosY[TileNum.TANK_SELF_10] =
		 * zoomFactor * DrawPositions.TANK_SELF_10_Y;
		 * drawTankPosX[TileNum.TANK_SELF_11] = zoomFactor *
		 * DrawPositions.TANK_SELF_11_X; drawTankPosY[TileNum.TANK_SELF_11] =
		 * zoomFactor * DrawPositions.TANK_SELF_11_Y;
		 * drawTankPosX[TileNum.TANK_SELF_12] = zoomFactor *
		 * DrawPositions.TANK_SELF_12_X; drawTankPosY[TileNum.TANK_SELF_12] =
		 * zoomFactor * DrawPositions.TANK_SELF_12_Y;
		 * drawTankPosX[TileNum.TANK_SELF_13] = zoomFactor *
		 * DrawPositions.TANK_SELF_13_X; drawTankPosY[TileNum.TANK_SELF_13] =
		 * zoomFactor * DrawPositions.TANK_SELF_13_Y;
		 * drawTankPosX[TileNum.TANK_SELF_14] = zoomFactor *
		 * DrawPositions.TANK_SELF_14_X; drawTankPosY[TileNum.TANK_SELF_14] =
		 * zoomFactor * DrawPositions.TANK_SELF_14_Y;
		 * drawTankPosX[TileNum.TANK_SELF_15] = zoomFactor *
		 * DrawPositions.TANK_SELF_15_X; drawTankPosY[TileNum.TANK_SELF_15] =
		 * zoomFactor * DrawPositions.TANK_SELF_15_Y;
		 * 
		 * drawTankPosX[TileNum.TANK_SELFBOAT_0] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_0_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_0] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_0_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_1] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_1_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_1] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_1_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_2] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_2_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_2] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_2_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_3] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_3_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_3] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_3_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_4] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_4_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_4] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_4_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_5] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_5_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_5] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_5_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_6] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_6_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_6] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_6_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_7] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_7_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_7] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_7_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_8] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_8_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_8] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_8_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_9] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_9_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_9] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_9_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_10] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_10_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_10] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_10_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_11] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_11_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_11] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_11_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_12] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_12_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_12] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_12_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_13] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_13_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_13] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_13_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_14] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_14_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_14] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_14_Y;
		 * drawTankPosX[TileNum.TANK_SELFBOAT_15] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_15_X;
		 * drawTankPosY[TileNum.TANK_SELFBOAT_15] = zoomFactor *
		 * DrawPositions.TANK_SELFBOAT_15_Y;
		 */

	}

	/**
	 * fillImage fills an image of colour fillColour
	 * 
	 * @param img
	 *            Image to fill
	 * @param fillColor
	 *            Colour to fill
	 */
	private void fillImage(Image img, Color fillColor) {
		Graphics2D g = (Graphics2D) img.getGraphics();
		g.setColor(fillColor);
		g.fillRect(0, 0, img.getWidth(null), img.getHeight(null));
		g.dispose();
	}

	/**
	 * copyStatusIcon copies a tile icon onto the image buffer
	 * 
	 * @param img
	 *            Image to copy the icon onto
	 * @param tile
	 *            Tile to copy
	 * @param x
	 *            X Position of destination
	 * @param y
	 *            Y Position of destination
	 */
	private void copyStatusIcon(Image img, Image tile, int x, int y) {
		Graphics2D g = (Graphics2D) img.getGraphics();
		g.drawImage(tile, x, y, null);
		g.dispose();
	}

	public static BufferedImage applyMask23(Graphics2D ing2d, Image img,
			final Color keycolor) {
		ImageFilter filter = new RGBImageFilter() {
			private int markerRGB = keycolor.getRGB() | 0xFF000000;

			public final int filterRGB(int x, int y, int rgb) {
				if ((rgb | 0xFF000000) == markerRGB) {
					return 0x00FFFFFF & rgb;
				} else
					return rgb;
			}
		};
		ImageProducer ip = new FilteredImageSource(img.getSource(), filter);
		Image masked = Toolkit.getDefaultToolkit().createImage(ip);

		BufferedImage dst = ing2d.getDeviceConfiguration()
				.createCompatibleImage(masked.getWidth(null),
						masked.getHeight(null), Transparency.BITMASK);
		Graphics2D g2d = dst.createGraphics();
		g2d.drawImage(masked, 0, 0, null);
		g2d.dispose();

		return dst;
	}

	/**
	 * Write Messages writes the news ticker to the screen
	 * 
	 * @param topLine
	 *            Top news ticker line
	 * @param bottomLine
	 *            Bottom news ticker line
	 */
	public void feWriteMessages(String topLine, String bottomLine) {
		Font font;
		Graphics2D g2d = gi.getGraphicsObject();

		/* Fill the background */
		g2d.setColor(Color.BLACK);
		g2d.setBackground(Color.BLACK);
		g2d.fillRect(zoomFactor * DrawPositions.MESSAGE_LEFT, zoomFactor
				* DrawPositions.MESSAGE_TOP, zoomFactor
				* DrawPositions.MESSAGE_WIDTH, 2 * zoomFactor
				* DrawPositions.MESSAGE_TEXT_HEIGHT);
		g2d.setFont(courierFont);

		/* Write the text */
		g2d.setColor(Color.WHITE);
		g2d.drawString(topLine, zoomFactor * DrawPositions.MESSAGE_LEFT,
				zoomFactor * (10 + DrawPositions.MESSAGE_TOP));
		g2d.drawString(bottomLine, zoomFactor * DrawPositions.MESSAGE_LEFT,
				zoomFactor * DrawPositions.MESSAGE_TEXT_HEIGHT + zoomFactor
						* (DrawPositions.MESSAGE_TOP + 10));
		g2d.dispose();
	}

	/**
	 * Sets the pillbox status for a particular pillbox
	 * 
	 * @param pillNum
	 *            The pill number to draw (0-15)
	 * @param pb -
	 *            The allience of the pillbox
	 */
	public void feDrawStatusPillbox(int pillNum, int pb) {
		Graphics2D g2d = gi.getGraphicsObject();
		drawStatusPillbox(pillNum, pb);
		drawCopyStatusImage(g2d, pillsStatusImage,
				DrawPositions.STATUS_PILLS_LEFT, DrawPositions.STATUS_PILLS_TOP);
		g2d.dispose();
	}

	/**
	 * Sets the tank status for a particular base
	 * 
	 * @param tankNum
	 *            The tank number to draw
	 * @param ts
	 *            The Tank status for the base
	 */
	public void feDrawStatusTank(int tankNum, int ts) {
		Graphics2D g2d = gi.getGraphicsObject();
		drawStatusTank(tankNum, ts);
		drawCopyStatusImage(g2d, tanksStatusImage,
				DrawPositions.STATUS_TANKS_LEFT, DrawPositions.STATUS_TANKS_TOP);
		g2d.dispose();
	}

	/**
	 * Draws the building select off border image
	 * 
	 * @param value
	 *            The build select option to use
	 */
	public void drawSelectIndentsOff(int value) {
		drawSelectIndents(indentsOffImage, value, false);
	}

	/**
	 * Draws the building select on border image
	 * 
	 * @param value
	 *            The build select option to use
	 */
	public void drawSelectIndentsOn(int value) {
		drawSelectIndents(indentsOnImage, value, true);
	}

	/**
	 * Draws the building select on/off border image
	 * 
	 * @param img
	 *            The image to draw (On or off)
	 * @param value
	 *            The build select option to use
	 * @param indentsOn
	 *            Are indents on or off. Use for drawing the dot
	 */
	private void drawSelectIndents(Image img, int value, boolean indentsOn) {
		int x;
		int y;
		int dotX;
		int dotY;
		int dotPosX;
		int dotPosY;

		Graphics2D g2d = gi.getGraphicsObject();
		switch (value) {
		case 0:
			x = zoomFactor * DrawPositions.BS_TREE_OFFSET_X;
			y = zoomFactor * DrawPositions.BS_TREE_OFFSET_Y;
			dotPosX = zoomFactor * DrawPositions.BS_DOT_TREE_OFFSET_X;
			dotPosY = zoomFactor * DrawPositions.BS_DOT_TREE_OFFSET_Y;
			break;
		case 1:
			x = zoomFactor * DrawPositions.BS_ROAD_OFFSET_X;
			y = zoomFactor * DrawPositions.BS_ROAD_OFFSET_Y;
			dotPosX = zoomFactor * DrawPositions.BS_DOT_ROAD_OFFSET_X;
			dotPosY = zoomFactor * DrawPositions.BS_DOT_ROAD_OFFSET_Y;
			break;
		case 2:
			x = zoomFactor * DrawPositions.BS_BUILDING_OFFSET_X;
			y = zoomFactor * DrawPositions.BS_BUILDING_OFFSET_Y;
			dotPosX = zoomFactor * DrawPositions.BS_DOT_BUILDING_OFFSET_X;
			dotPosY = zoomFactor * DrawPositions.BS_DOT_BUILDING_OFFSET_Y;
			break;
		case 3:
			x = zoomFactor * DrawPositions.BS_PILLBOX_OFFSET_X;
			y = zoomFactor * DrawPositions.BS_PILLBOX_OFFSET_Y;
			dotPosX = zoomFactor * DrawPositions.BS_DOT_PILLBOX_OFFSET_X;
			dotPosY = zoomFactor * DrawPositions.BS_DOT_PILLBOX_OFFSET_Y;
			break;
		case 4:
		default:
			x = zoomFactor * DrawPositions.BS_MINE_OFFSET_X;
			y = zoomFactor * DrawPositions.BS_MINE_OFFSET_Y;
			dotPosX = zoomFactor * DrawPositions.BS_DOT_MINE_OFFSET_X;
			dotPosY = zoomFactor * DrawPositions.BS_DOT_MINE_OFFSET_Y;
			break;
		}
		g2d.drawImage(img, x, y, null);

		/* Draw the dot */
		if (indentsOn == true) {
			dotX = zoomFactor * DrawPositions.INDENT_DOT_ON_X;
			dotY = zoomFactor * DrawPositions.INDENT_DOT_ON_Y;
		} else {
			dotX = zoomFactor * DrawPositions.INDENT_DOT_OFF_X;
			dotY = zoomFactor * DrawPositions.INDENT_DOT_OFF_Y;
		}
		Image dot = tiles.getSubimage(dotX, dotY, zoomFactor
				* DrawPositions.BS_DOT_ITEM_SIZE_X, zoomFactor
				* DrawPositions.BS_DOT_ITEM_SIZE_Y);
		g2d.drawImage(dot, dotPosX, dotPosY, null);
		g2d.dispose();
	}

	/**
	 * Sets the base status for a particular base
	 * 
	 * @param baseNum
	 *            The base number to draw (0-15)
	 * @param bs
	 *            The allience of the base
	 */
	public void feDrawStatusBase(int baseNum, int bs) {
		Graphics2D g2d = gi.getGraphicsObject();
		drawStatusBase(baseNum, bs);
		drawCopyStatusImage(g2d, baseStatusImage,
				DrawPositions.STATUS_BASES_LEFT, DrawPositions.STATUS_BASES_TOP);
		g2d.dispose();
	}

	/**
	 * Used to draw the base stock amounts
	 * 
	 * @param shells
	 *            Number of shells
	 * @param mines
	 *            Number of mines
	 * @param armour
	 *            Amount of armour
	 * @param redraw
	 *            Set to true if we want to redraw the last amounts
	 */
	public void feDrawStatusBaseBars(int shells, int mines, int armour,
			boolean redraw) {
		Graphics2D g2d = null;
		int x;
		int y;
		int width;
		int height;

		if (redraw == true || lastBaseShells != shells
				|| lastBaseMines != mines || lastBaseArmour != armour) {
			g2d = gi.getGraphicsObject();
			if (redraw == false) {
				lastBaseShells = shells;
				lastBaseMines = mines;
				lastBaseArmour = armour;
			}

			/* Make the area black first */
			x = zoomFactor * DrawPositions.STATUS_BASE_BARS_LEFT;
			y = zoomFactor * DrawPositions.STATUS_BASE_SHELLS;
			width = zoomFactor * DrawPositions.STATUS_BASE_BARS_MAX_WIDTH;
			height = zoomFactor * DrawPositions.STATUS_BASE_ARMOUR + zoomFactor
					* DrawPositions.STATUS_BASE_BARS_HEIGHT - zoomFactor
					* DrawPositions.STATUS_BASE_SHELLS;
			g2d.setBackground(Color.BLACK);
			g2d.setColor(Color.BLACK);
			g2d.fillRect(x, y, width, height);

			/* Draw the bases bars */
			if (lastBaseShells != 0 || lastBaseMines != 0
					|| lastBaseArmour != 0) {
				g2d.setBackground(Color.GREEN);
				g2d.setColor(Color.GREEN);
				x = zoomFactor * DrawPositions.STATUS_BASE_BARS_LEFT;

				/* Shells */
				y = zoomFactor * DrawPositions.STATUS_BASE_SHELLS;
				width = (int) Math.floor(lastBaseShells * (float) zoomFactor
						* (float) DrawPositions.BAR_BASE_MULTIPLY);
				height = zoomFactor * DrawPositions.STATUS_BASE_BARS_HEIGHT;
				g2d.fillRect(x, y, width, height);
				/* Mines */
				y = zoomFactor * DrawPositions.STATUS_BASE_MINES;
				height = zoomFactor * DrawPositions.STATUS_BASE_BARS_HEIGHT;
				width = (int) Math.floor(lastBaseMines * (float) zoomFactor
						* (float) DrawPositions.BAR_BASE_MULTIPLY);
				g2d.fillRect(x, y, width, height);
				/* Armour */
				y = zoomFactor * DrawPositions.STATUS_BASE_ARMOUR;
				height = zoomFactor * DrawPositions.STATUS_BASE_BARS_HEIGHT;
				width = (int) Math.floor(lastBaseArmour * (float) zoomFactor
						* (float) DrawPositions.BAR_BASE_MULTIPLY);
				g2d.fillRect(x, y, width, height);
			}
			g2d.dispose();
		}
	}

	/**
	 * Used to draw the tank stock amounts
	 * 
	 * @param shells
	 *            Number of shells
	 * @param mines
	 *            Number of mines
	 * @param armour
	 *            Amount of armour
	 * @param trees
	 *            Amount of trees
	 */
	public void feDrawStatusTankBars(int shells, int mines, int armour,
			int trees) {
		Graphics g2d;
		int x;
		int y;
		int width;
		int height;

		g2d = gi.getGraphicsObject();
		g2d.setColor(Color.BLACK);

		/* Fill area black first */
		y = (zoomFactor * DrawPositions.STATUS_TANK_BARS_TOP)
				+ (zoomFactor * DrawPositions.STATUS_TANK_BARS_HEIGHT)
				- ((zoomFactor * DrawPositions.BAR_TANK_MULTIPLY) * 40);
		height = (zoomFactor * DrawPositions.STATUS_TANK_BARS_TOP)
				+ (zoomFactor * DrawPositions.STATUS_TANK_BARS_HEIGHT) - y;
		x = (zoomFactor * DrawPositions.STATUS_TANK_SHELLS);
		width = (zoomFactor * DrawPositions.STATUS_TANK_TREES)
				+ (zoomFactor * DrawPositions.STATUS_TANK_BARS_WIDTH) - x;
		g2d.fillRect(x, y, width, height);
		g2d.setColor(Color.GREEN);
		width = zoomFactor * DrawPositions.STATUS_TANK_BARS_WIDTH;

		/* Shells */
		y = (zoomFactor * DrawPositions.STATUS_TANK_BARS_TOP)
				+ (zoomFactor * DrawPositions.STATUS_TANK_BARS_HEIGHT)
				- ((zoomFactor * DrawPositions.BAR_TANK_MULTIPLY) * shells);
		x = (zoomFactor * DrawPositions.STATUS_TANK_SHELLS);
		height = zoomFactor * DrawPositions.BAR_TANK_MULTIPLY * shells;
		g2d.fillRect(x, y, width, height);

		/* Mines */
		y = (zoomFactor * DrawPositions.STATUS_TANK_BARS_TOP)
				+ (zoomFactor * DrawPositions.STATUS_TANK_BARS_HEIGHT)
				- ((zoomFactor * DrawPositions.BAR_TANK_MULTIPLY) * mines);
		x = (zoomFactor * DrawPositions.STATUS_TANK_MINES);
		height = zoomFactor * DrawPositions.BAR_TANK_MULTIPLY * mines;
		g2d.fillRect(x, y, width, height);

		/* Armour */
		y = (zoomFactor * DrawPositions.STATUS_TANK_BARS_TOP)
				+ (zoomFactor * DrawPositions.STATUS_TANK_BARS_HEIGHT)
				- ((zoomFactor * DrawPositions.BAR_TANK_MULTIPLY) * armour);
		x = (zoomFactor * DrawPositions.STATUS_TANK_ARMOUR);
		height = zoomFactor * DrawPositions.BAR_TANK_MULTIPLY * armour;
		g2d.fillRect(x, y, width, height);
		/* Trees */
		y = (zoomFactor * DrawPositions.STATUS_TANK_BARS_TOP)
				+ (zoomFactor * DrawPositions.STATUS_TANK_BARS_HEIGHT)
				- ((zoomFactor * DrawPositions.BAR_TANK_MULTIPLY) * trees);
		x = (zoomFactor * DrawPositions.STATUS_TANK_TREES);
		height = zoomFactor * DrawPositions.BAR_TANK_MULTIPLY * trees;
		g2d.fillRect(x, y, width, height);

		g2d.dispose();

	}

	/**
	 * Writes the number of kills and deaths this player has had
	 * 
	 * @param kills
	 *            Number of kills to write
	 * @param deaths
	 *            Number of deaths to write
	 */
	public void feWriteKillsDeaths(int kills, int deaths) {
		Graphics2D g2d = gi.getGraphicsObject();

		/* Fill the background first */
		g2d.setColor(Color.BLACK);
		g2d.setBackground(Color.BLACK);
		g2d.fillRect(zoomFactor * DrawPositions.STATUS_KILLS_LEFT, zoomFactor
				* DrawPositions.STATUS_KILLS_TOP, zoomFactor
				* DrawPositions.STATUS_KILLS_WIDTH, zoomFactor
				* DrawPositions.STATUS_KILLS_HEIGHT);
		g2d.fillRect(zoomFactor * DrawPositions.STATUS_DEATHS_LEFT, zoomFactor
				* DrawPositions.STATUS_DEATHS_TOP, zoomFactor
				* DrawPositions.STATUS_KILLS_WIDTH, zoomFactor
				* DrawPositions.STATUS_KILLS_HEIGHT);

		/* Write the text */
		g2d.setColor(Color.WHITE);
		g2d.drawString(Integer.toString(kills), zoomFactor
				* DrawPositions.STATUS_KILLS_LEFT, zoomFactor
				* DrawPositions.STATUS_KILLS_TOP);
		g2d.drawString(Integer.toString(deaths), zoomFactor
				* DrawPositions.STATUS_DEATHS_LEFT, zoomFactor
				* DrawPositions.STATUS_DEATHS_TOP);
		g2d.dispose();
	}

	/**
	 * Draws the man arrow status. If isDead is set to true then the angle is
	 * ignored.
	 * 
	 * @param isDead
	 *            Is the man dead
	 * @param angle
	 *            The angle the item is facing
	 */
	public void feDrawManStatus(boolean isDead, float angle) {
		double dbAngle; /* Angle in radians */
		double dbTemp;
		int addX; /* X And and Y co-ordinates */
		int addY;
		Graphics2D g;

		fillImage(lgmStatusImage, Color.BLACK);
		angle += Utils.BRADIANS_SOUTH;
		if (angle >= Utils.BRADIANS_MAX) {
			angle -= Utils.BRADIANS_MAX;
		}

		if (angle >= Utils.BRADIANS_NORTH && angle < Utils.BRADIANS_EAST) {
			/* Convert bradians to degrees */
			dbAngle = (Utils.DEGREES_MAX / Utils.BRADIANS_MAX) * angle;
			/* Convert degrees to radians */
			dbAngle = (dbAngle / Utils.DEGREES_MAX) * Utils.RADIAN_MAX;

			addX = (zoomFactor * DrawPositions.MAN_STATUS_CENTER_X);
			addY = (zoomFactor * DrawPositions.MAN_STATUS_CENTER_Y);
			dbTemp = DrawPositions.MAN_STATUS_RADIUS * Math.sin(dbAngle);
			addX += (int) (zoomFactor * dbTemp);
			dbTemp = DrawPositions.MAN_STATUS_RADIUS * (Math.cos(dbAngle));
			addY -= (int) (zoomFactor * dbTemp);

		} else if (angle >= Utils.BRADIANS_EAST && angle < Utils.BRADIANS_SOUTH) {
			angle = Utils.BRADIANS_SOUTH - angle;
			/* Convert bradians to degrees */
			dbAngle = (Utils.DEGREES_MAX / Utils.BRADIANS_MAX) * angle;
			/* Convert degrees to radians */
			dbAngle = (dbAngle / Utils.DEGREES_MAX) * Utils.RADIAN_MAX;

			addX = (zoomFactor * DrawPositions.MAN_STATUS_CENTER_X);
			addY = (zoomFactor * DrawPositions.MAN_STATUS_CENTER_Y);
			dbTemp = DrawPositions.MAN_STATUS_RADIUS * (Math.sin(dbAngle));
			addX += (int) (zoomFactor * dbTemp);
			dbTemp = DrawPositions.MAN_STATUS_RADIUS * (Math.cos(dbAngle));
			addY += (int) (zoomFactor * dbTemp);
		} else if (angle >= Utils.BRADIANS_SOUTH && angle < Utils.BRADIANS_WEST) {
			angle = Utils.BRADIANS_WEST - angle;
			angle = Utils.BRADIANS_EAST - angle;
			/* Convert bradians to degrees */
			dbAngle = (Utils.DEGREES_MAX / Utils.BRADIANS_MAX) * angle;
			/* Convert degrees to radians */
			dbAngle = (dbAngle / Utils.DEGREES_MAX) * Utils.RADIAN_MAX;

			addX = (zoomFactor * DrawPositions.MAN_STATUS_CENTER_X);
			addY = (zoomFactor * DrawPositions.MAN_STATUS_CENTER_Y);
			dbTemp = DrawPositions.MAN_STATUS_RADIUS * (Math.sin(dbAngle));
			addX -= (int) (zoomFactor * dbTemp);
			dbTemp = DrawPositions.MAN_STATUS_RADIUS * (Math.cos(dbAngle));
			addY += (int) (zoomFactor * dbTemp);

		} else {
			angle = (float) Utils.BRADIANS_MAX - angle;
			/* Convert bradians to degrees */
			dbAngle = (Utils.DEGREES_MAX / Utils.BRADIANS_MAX) * angle;
			/* Convert degrees to radians */
			dbAngle = (dbAngle / Utils.DEGREES_MAX) * Utils.RADIAN_MAX;

			addX = (zoomFactor * DrawPositions.MAN_STATUS_CENTER_X);
			addY = (zoomFactor * DrawPositions.MAN_STATUS_CENTER_Y);
			dbTemp = DrawPositions.MAN_STATUS_RADIUS * (Math.sin(dbAngle));
			addX -= (int) (zoomFactor * dbTemp);
			dbTemp = DrawPositions.MAN_STATUS_RADIUS * (Math.cos(dbAngle));
			addY -= (int) (zoomFactor * dbTemp);
		}

		g = (Graphics2D) lgmStatusImage.getGraphics();
		g.setColor(Color.WHITE);
		if (isDead == true) {
			g.fillOval(0, 0, lgmStatusImage.getWidth() - 1, lgmStatusImage
					.getHeight() - 1);
		} else {
			g.drawOval(0, 0, lgmStatusImage.getWidth() - 1, lgmStatusImage
					.getHeight() - 1);
			g.drawLine(addX, addY, zoomFactor
					* DrawPositions.MAN_STATUS_CENTER_X, zoomFactor
					* DrawPositions.MAN_STATUS_CENTER_Y);
		}
		g.dispose();

		copyManStatus();
	}

	/**
	 * Clears the man arrow status.
	 */
	public void feDrawClearManStatus() {
		fillImage(lgmStatusImage, Color.BLACK);
		copyManStatus();
	}

	/**
	 * Draws the download line down the screen
	 * 
	 * @param justBlack
	 *            Just draw the screen black?
	 * @param yPos
	 *            Y Position of the map
	 */
	public void feDrawDownloadScreen(boolean justBlack, int yPos) {
		Graphics2D g2d = (Graphics2D) backbuffer.getGraphics();

		g2d.setBackground(Color.BLACK);
		g2d.setColor(Color.BLACK);
		g2d.fillRect(0, 0, backbuffer.getWidth(), backbuffer.getHeight());
		if (justBlack == false) {
			g2d.setBackground(Color.WHITE);
			g2d.setColor(Color.WHITE);
			g2d.fillRect(0, 0, backbuffer.getWidth(), yPos);
		}
		g2d.dispose();
		g2d = gi.getGraphicsObject();
		g2d.drawImage(backbuffer, zoomFactor * DrawPositions.MAIN_OFFSET_X,
				zoomFactor * DrawPositions.MAIN_OFFSET_Y, zoomFactor
						* DrawPositions.MAIN_OFFSET_X + zoomFactor
						* GameClient.MAIN_SCREEN_SIZE_X
						* DrawPositions.TILE_SIZE_X, zoomFactor
						* DrawPositions.MAIN_OFFSET_Y + zoomFactor
						* GameClient.MAIN_SCREEN_SIZE_Y
						* DrawPositions.TILE_SIZE_Y, zoomFactor
						* DrawPositions.TILE_SIZE_X, zoomFactor
						* DrawPositions.TILE_SIZE_Y, zoomFactor
						* GameClient.MAIN_SCREEN_SIZE_X
						* DrawPositions.TILE_SIZE_X + zoomFactor
						* DrawPositions.TILE_SIZE_X, zoomFactor
						* GameClient.MAIN_SCREEN_SIZE_Y
						* DrawPositions.TILE_SIZE_Y + zoomFactor
						* DrawPositions.TILE_SIZE_Y, null);
		g2d.dispose();
	}

	/**
	 * Copies the man status image to the screen
	 */
	private void copyManStatus() {
		Graphics2D g2d = gi.getGraphicsObject();
		g2d.drawImage(lgmStatusImage, zoomFactor * DrawPositions.MAN_STATUS_X,
				zoomFactor * DrawPositions.MAN_STATUS_Y, null);
		g2d.dispose();

	}
}

/*
 * 
 * Have you tried using FontMetrics.stringWidth(String str), or if anti-aliasing
 * and fractional metrics hints are enabled, FontMetrics.getStringBounds(String
 * str, Graphics context) ?
 */
