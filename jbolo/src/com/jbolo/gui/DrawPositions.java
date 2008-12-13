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
 * DrawPositions.java
 *
 * Created on October 6, 2003, 7:38 PM
 */
package com.jbolo.gui;

/**
 * Contains constants about locations of tiles in the tile file. Also provides
 * some useful static methods
 * 
 * @author John Morrison
 */
public class DrawPositions {

	/* Offset of the man view screen from the window edge */
	public static final int MAIN_OFFSET_X = 81;
	public static final int MAIN_OFFSET_Y = 18;

	public static final int STATUS_ITEM_SIZE_X = 12;
	public static final int STATUS_ITEM_SIZE_Y = 12;

	/* Offsets of the build select boxes from the window edge */
	public static final int BS_ITEM_SIZE_X = 54;
	public static final int BS_ITEM_SIZE_Y = 50;
	public static final int BS_TREE_OFFSET_X = 17;
	public static final int BS_TREE_OFFSET_Y = 13;
	public static final int BS_ROAD_OFFSET_X = BS_TREE_OFFSET_X;
	public static final int BS_ROAD_OFFSET_Y = (BS_TREE_OFFSET_Y + BS_ITEM_SIZE_Y);
	public static final int BS_BUILDING_OFFSET_X = BS_TREE_OFFSET_X;
	public static final int BS_BUILDING_OFFSET_Y = (BS_TREE_OFFSET_Y + 2 * BS_ITEM_SIZE_Y);
	public static final int BS_PILLBOX_OFFSET_X = BS_TREE_OFFSET_X;
	public static final int BS_PILLBOX_OFFSET_Y = (BS_TREE_OFFSET_Y + 3 * BS_ITEM_SIZE_Y);
	public static final int BS_MINE_OFFSET_X = BS_TREE_OFFSET_X;
	public static final int BS_MINE_OFFSET_Y = (BS_TREE_OFFSET_Y + 4 * BS_ITEM_SIZE_Y);

	/* Offsets of the build select dots from the window edge */
	public static final int BS_DOT_ITEM_SIZE_X = 6;
	public static final int BS_DOT_ITEM_SIZE_Y = 6;
	public static final int BS_DOT_TREE_OFFSET_X = 8;
	public static final int BS_DOT_TREE_OFFSET_Y = 35;
	public static final int BS_DOT_ROAD_OFFSET_X = BS_DOT_TREE_OFFSET_X;
	public static final int BS_DOT_ROAD_OFFSET_Y = 85;
	public static final int BS_DOT_BUILDING_OFFSET_X = BS_DOT_TREE_OFFSET_X;
	public static final int BS_DOT_BUILDING_OFFSET_Y = 135;
	public static final int BS_DOT_PILLBOX_OFFSET_X = BS_DOT_TREE_OFFSET_X;
	public static final int BS_DOT_PILLBOX_OFFSET_Y = 185;
	public static final int BS_DOT_MINE_OFFSET_X = BS_DOT_TREE_OFFSET_X;
	public static final int BS_DOT_MINE_OFFSET_Y = 235;

	/* The status items on the right of the main view */

	/* Size of the tanks, base and pills windows */
	public static final int STATUS_PILLS_WIDTH = 90;
	public static final int STATUS_PILLS_HEIGHT = 66;
	public static final int STATUS_BASES_WIDTH = 90;
	public static final int STATUS_BASES_HEIGHT = 66;
	public static final int STATUS_TANKS_WIDTH = 90;
	public static final int STATUS_TANKS_HEIGHT = 66;
	/* Offset from the window edges of them */
	public static final int STATUS_PILLS_LEFT = 335;
	public static final int STATUS_PILLS_TOP = 88;
	public static final int STATUS_BASES_LEFT = 335;
	public static final int STATUS_BASES_TOP = 157;
	public static final int STATUS_TANKS_LEFT = 335;
	public static final int STATUS_TANKS_TOP = 18; /* 13 */

	public static final int STATUS_ITEM_GAP_X = 2;
	public static final int STATUS_ITEM_GAP_Y = 9;

	public static final int STATUS_BASE_1_X = 4;
	public static final int STATUS_BASE_1_Y = 4;
	public static final int STATUS_BASE_2_X = (STATUS_BASE_1_X
			+ STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X);
	public static final int STATUS_BASE_2_Y = STATUS_BASE_1_Y;
	public static final int STATUS_BASE_3_X = (STATUS_BASE_1_X + 2 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_3_Y = STATUS_BASE_1_Y;
	public static final int STATUS_BASE_4_X = (STATUS_BASE_1_X + 3 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_4_Y = STATUS_BASE_1_Y;
	public static final int STATUS_BASE_5_X = (STATUS_BASE_1_X + 4 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_5_Y = STATUS_BASE_1_Y;
	public static final int STATUS_BASE_6_X = (STATUS_BASE_1_X + 5 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_6_Y = STATUS_BASE_1_Y;
	public static final int STATUS_BASE_7_X = STATUS_BASE_1_X;
	public static final int STATUS_BASE_7_Y = (STATUS_BASE_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_BASE_8_X = (STATUS_BASE_1_X + (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_8_Y = (STATUS_BASE_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_BASE_9_X = (STATUS_BASE_1_X + 4 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_9_Y = (STATUS_BASE_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_BASE_10_X = (STATUS_BASE_1_X + 5 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_10_Y = (STATUS_BASE_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_BASE_11_X = STATUS_BASE_1_X;
	public static final int STATUS_BASE_11_Y = (STATUS_BASE_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_BASE_12_X = (STATUS_BASE_1_X + (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_12_Y = (STATUS_BASE_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_BASE_13_X = (STATUS_BASE_1_X + 2 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_13_Y = (STATUS_BASE_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_BASE_14_X = (STATUS_BASE_1_X + 3 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_14_Y = (STATUS_BASE_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_BASE_15_X = (STATUS_BASE_1_X + 4 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_15_Y = (STATUS_BASE_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_BASE_16_X = (STATUS_BASE_1_X + 5 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_BASE_16_Y = (STATUS_BASE_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	/* The middle square for drawing the icon */
	public static final int STATUS_BASES_MIDDLE_ICON_X = 37;
	public static final int STATUS_BASES_MIDDLE_ICON_Y = ((STATUS_BASE_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y) - 2);

	/* Pillbox Status Position */
	public static final int STATUS_PILLBOX_1_X = 4;
	public static final int STATUS_PILLBOX_1_Y = 4;
	public static final int STATUS_PILLBOX_2_X = (STATUS_PILLBOX_1_X
			+ STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X);
	public static final int STATUS_PILLBOX_2_Y = STATUS_PILLBOX_1_Y;
	public static final int STATUS_PILLBOX_3_X = (STATUS_PILLBOX_1_X + 2 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_3_Y = STATUS_PILLBOX_1_Y;
	public static final int STATUS_PILLBOX_4_X = (STATUS_PILLBOX_1_X + 3 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_4_Y = STATUS_PILLBOX_1_Y;
	public static final int STATUS_PILLBOX_5_X = (STATUS_PILLBOX_1_X + 4 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_5_Y = STATUS_PILLBOX_1_Y;
	public static final int STATUS_PILLBOX_6_X = (STATUS_PILLBOX_1_X + 5 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_6_Y = STATUS_PILLBOX_1_Y;
	public static final int STATUS_PILLBOX_7_X = STATUS_PILLBOX_1_X;
	public static final int STATUS_PILLBOX_7_Y = (STATUS_PILLBOX_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_PILLBOX_8_X = (STATUS_PILLBOX_1_X + (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_8_Y = (STATUS_PILLBOX_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_PILLBOX_9_X = (STATUS_PILLBOX_1_X + 4 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_9_Y = (STATUS_PILLBOX_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_PILLBOX_10_X = (STATUS_PILLBOX_1_X + 5 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_10_Y = (STATUS_PILLBOX_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_PILLBOX_11_X = STATUS_PILLBOX_1_X;
	public static final int STATUS_PILLBOX_11_Y = (STATUS_PILLBOX_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_PILLBOX_12_X = (STATUS_PILLBOX_1_X + (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_12_Y = (STATUS_PILLBOX_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_PILLBOX_13_X = (STATUS_PILLBOX_1_X + 2 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_13_Y = (STATUS_PILLBOX_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_PILLBOX_14_X = (STATUS_PILLBOX_1_X + 3 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_14_Y = (STATUS_PILLBOX_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_PILLBOX_15_X = (STATUS_PILLBOX_1_X + 4 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_15_Y = (STATUS_PILLBOX_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_PILLBOX_16_X = (STATUS_PILLBOX_1_X + 5 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_PILLBOX_16_Y = (STATUS_PILLBOX_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	/* The middle square for drawing the icon */
	public static final int STATUS_PILLS_MIDDLE_ICON_X = 37;
	public static final int STATUS_PILLS_MIDDLE_ICON_Y = ((STATUS_BASE_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y) - 2);

	/* Tank Status positions */
	public static final int STATUS_TANKS_1_X = 4;
	public static final int STATUS_TANKS_1_Y = 4;
	public static final int STATUS_TANKS_2_X = (STATUS_TANKS_1_X
			+ STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X);
	public static final int STATUS_TANKS_2_Y = STATUS_TANKS_1_Y;
	public static final int STATUS_TANKS_3_X = (STATUS_TANKS_1_X + 2 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_3_Y = STATUS_TANKS_1_Y;
	public static final int STATUS_TANKS_4_X = (STATUS_TANKS_1_X + 3 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_4_Y = STATUS_TANKS_1_Y;
	public static final int STATUS_TANKS_5_X = (STATUS_TANKS_1_X + 4 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_5_Y = STATUS_TANKS_1_Y;
	public static final int STATUS_TANKS_6_X = (STATUS_TANKS_1_X + 5 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_6_Y = STATUS_TANKS_1_Y;
	public static final int STATUS_TANKS_7_X = STATUS_TANKS_1_X;
	public static final int STATUS_TANKS_7_Y = (STATUS_TANKS_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_TANKS_8_X = (STATUS_TANKS_1_X + (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_8_Y = (STATUS_TANKS_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_TANKS_9_X = (STATUS_TANKS_1_X + 4 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_9_Y = (STATUS_TANKS_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_TANKS_10_X = (STATUS_TANKS_1_X + 5 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_10_Y = (STATUS_TANKS_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y);
	public static final int STATUS_TANKS_11_X = STATUS_TANKS_1_X;
	public static final int STATUS_TANKS_11_Y = (STATUS_TANKS_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_TANKS_12_X = (STATUS_TANKS_1_X + (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_12_Y = (STATUS_TANKS_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_TANKS_13_X = (STATUS_TANKS_1_X + 2 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_13_Y = (STATUS_TANKS_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_TANKS_14_X = (STATUS_TANKS_1_X + 3 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_14_Y = (STATUS_TANKS_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_TANKS_15_X = (STATUS_TANKS_1_X + 4 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_15_Y = (STATUS_TANKS_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	public static final int STATUS_TANKS_16_X = (STATUS_TANKS_1_X + 5 * (STATUS_ITEM_GAP_X + STATUS_ITEM_SIZE_X));
	public static final int STATUS_TANKS_16_Y = (STATUS_TANKS_1_Y + 2 * (STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y));
	/* The middle square for drawing the icon */
	public static final int STATUS_TANKS_MIDDLE_ICON_X = 37;
	public static final int STATUS_TANKS_MIDDLE_ICON_Y = ((STATUS_TANKS_1_Y
			+ STATUS_ITEM_GAP_Y + STATUS_ITEM_SIZE_Y) - 2);

	/* 3x - 80y - 9 Between Items From bottom of icons to top 13 8 from left */

	public static final int BAR_TANK_MULTIPLY = 2; /*
													 * Times item amounts by 2
													 * for 80 pixel high bars
													 */
	public static final int STATUS_TANK_BARS_TOP = 105;
	public static final int STATUS_TANK_BARS_HEIGHT = 135;
	public static final int STATUS_TANK_BARS_WIDTH = 3;
	public static final int STATUS_TANK_BARS_GAP = 9;
	public static final int STATUS_TANK_SHELLS = 446;
	public static final int STATUS_TANK_MINES = (STATUS_TANK_SHELLS + (STATUS_TANK_BARS_WIDTH + STATUS_TANK_BARS_GAP));
	public static final int STATUS_TANK_ARMOUR = (STATUS_TANK_SHELLS + 2 * (STATUS_TANK_BARS_WIDTH + STATUS_TANK_BARS_GAP));
	public static final int STATUS_TANK_TREES = (STATUS_TANK_SHELLS + 3 * (STATUS_TANK_BARS_WIDTH + STATUS_TANK_BARS_GAP));

	public static double BAR_BASE_MULTIPLY = (2.0 / 3.0); /*
															 * Times item
															 * amounts by 2/3
															 * for 54 pixel wide
															 * bars
															 */
	public static final int STATUS_BASE_BARS_TOP = 230;
	public static final int STATUS_BASE_BARS_HEIGHT = 3;
	public static final int STATUS_BASE_BARS_LEFT = 350;
	public static final int STATUS_BASE_BARS_GAP = 7;
	public static final int STATUS_BASE_SHELLS = STATUS_BASE_BARS_TOP;
	public static final int STATUS_BASE_MINES = (STATUS_BASE_SHELLS + (STATUS_BASE_BARS_HEIGHT + STATUS_BASE_BARS_GAP));
	public static final int STATUS_BASE_ARMOUR = (STATUS_BASE_SHELLS + 2 * (STATUS_BASE_BARS_HEIGHT + STATUS_BASE_BARS_GAP));
	public static final int STATUS_BASE_BARS_MAX_WIDTH = 60;

	/* Kills - Deaths Positions */
	public static final int STATUS_KILLS_TOP = 97; // 85
	public static final int STATUS_KILLS_LEFT = 462;
	public static final int STATUS_KILLS_HEIGHT = 25;
	public static final int STATUS_KILLS_WIDTH = 25;
	public static final int STATUS_DEATHS_LEFT = STATUS_KILLS_LEFT;
	public static final int STATUS_DEATHS_TOP = 111; // 99

	/* The message Window */
	public static final int MESSAGE_TOP = 272;
	public static final int MESSAGE_LEFT = 21;
	public static final int MESSAGE_HEIGHT = 35;
	public static final int MESSAGE_TOTALHEIGHT = 100;
	public static final int MESSAGE_WIDTH = 472;
	public static final int MESSAGE_TEXT_HEIGHT = 17;
	public static final int MESSAGE_TOP_LINE_X = 21;
	public static final int MESSAGE_TOP_LINE_Y = 272;
	public static final int MESSAGE_BOTTOM_LINE_X = 21;
	public static final int MESSAGE_BOTTOM_LINE_Y = 289;

	public static final int MAN_STATUS_WIDTH = 40;
	public static final int MAN_STATUS_HEIGHT = 40;

	/* Position on the screen */
	public static final int MAN_STATUS_X = 443;
	public static final int MAN_STATUS_Y = 27;

	public static final int MAN_STATUS_RADIUS = 20;
	public static final int MAN_STATUS_CENTER_X = MAN_STATUS_RADIUS;
	public static final int MAN_STATUS_CENTER_Y = MAN_STATUS_RADIUS;

	/* Tails are 5 pixels big */
	public static final int MAN_STATE_TAIL_SIZE_LENGTH = 4;

	/* You want the items label to be slightly off from the left of the object */
	public static final int LABEL_OFFSET_X = 4;

	/* Size of the tank label back buffer */
	public static final int TANK_LABEL_WIDTH = 200;
	public static final int TANK_LABEL_HEIGHT = 50;

	/* The name of the map tileset file (Now points to a resource) */
	public static final int TILE_FILE_X = 496;
	public static final int TILE_FILE_Y = 160;
	public static final int TILE_SIZE_X = 16;
	public static final int TILE_SIZE_Y = 16;
	/* Locations in the tile file of each tile */
	public static final int ROAD_HORZ_X = 0;
	public static final int ROAD_HORZ_Y = 0;
	public static final int ROAD_VERT_X = TILE_SIZE_X;
	public static final int ROAD_VERT_Y = 0;
	/* The road corners */
	public static final int ROAD_CORNER1_X = (2 * TILE_SIZE_X);
	public static final int ROAD_CORNER1_Y = 0;
	public static final int ROAD_CORNER2_X = (3 * TILE_SIZE_X);
	public static final int ROAD_CORNER2_Y = 0;
	public static final int ROAD_CORNER3_X = (4 * TILE_SIZE_X);
	public static final int ROAD_CORNER3_Y = 0;
	public static final int ROAD_CORNER4_X = (5 * TILE_SIZE_X);
	public static final int ROAD_CORNER4_Y = 0;
	/* The 4 solid road corners */
	public static final int ROAD_CORNER5_X = (6 * TILE_SIZE_X);
	public static final int ROAD_CORNER5_Y = 0;
	public static final int ROAD_CORNER6_X = (7 * TILE_SIZE_X);
	public static final int ROAD_CORNER6_Y = 0;
	public static final int ROAD_CORNER7_X = (8 * TILE_SIZE_X);
	public static final int ROAD_CORNER7_Y = 0;
	public static final int ROAD_CORNER8_X = (9 * TILE_SIZE_X);
	public static final int ROAD_CORNER8_Y = 0;
	/* Solid road */
	public static final int ROAD_SOLID_X = (10 * TILE_SIZE_X);
	public static final int ROAD_SOLID_Y = 0;
	/* Cross roads */
	public static final int ROAD_CROSS_X = (11 * TILE_SIZE_X);
	public static final int ROAD_CROSS_Y = 0;
	/* T Junctions */
	public static final int ROAD_T1_X = (12 * TILE_SIZE_X);
	public static final int ROAD_T1_Y = 0;
	public static final int ROAD_T2_X = (13 * TILE_SIZE_X);
	public static final int ROAD_T2_Y = 0;
	public static final int ROAD_T3_X = (14 * TILE_SIZE_X);
	public static final int ROAD_T3_Y = 0;
	public static final int ROAD_T4_X = (15 * TILE_SIZE_X);
	public static final int ROAD_T4_Y = 0;
	/* 1 side over other normal */
	public static final int ROAD_WATER1_X = (16 * TILE_SIZE_X);
	public static final int ROAD_WATER1_Y = 0;
	public static final int ROAD_WATER2_X = (17 * TILE_SIZE_X);
	public static final int ROAD_WATER2_Y = 0;
	public static final int ROAD_WATER3_X = (18 * TILE_SIZE_X);
	public static final int ROAD_WATER3_Y = 0;
	public static final int ROAD_WATER4_X = (19 * TILE_SIZE_X);
	public static final int ROAD_WATER4_Y = 0;
	/* Corners on water */
	public static final int ROAD_WATER5_X = (20 * TILE_SIZE_X);
	public static final int ROAD_WATER5_Y = 0;
	public static final int ROAD_WATER6_X = (21 * TILE_SIZE_X);
	public static final int ROAD_WATER6_Y = 0;
	public static final int ROAD_WATER7_X = (22 * TILE_SIZE_X);
	public static final int ROAD_WATER7_Y = 0;
	public static final int ROAD_WATER8_X = (23 * TILE_SIZE_X);
	public static final int ROAD_WATER8_Y = 0;
	/* Horizontal water peice */
	public static final int ROAD_WATER9_X = (24 * TILE_SIZE_X);
	public static final int ROAD_WATER9_Y = 0;
	/* Vertical = water peice */
	public static final int ROAD_WATER10_X = (25 * TILE_SIZE_X);
	public static final int ROAD_WATER10_Y = 0;
	/* Lone water peice */
	public static final int ROAD_WATER11_X = (26 * TILE_SIZE_X);
	public static final int ROAD_WATER11_Y = 0;
	/* Road single side peices */
	public static final int ROAD_SIDE1_X = (27 * TILE_SIZE_X);
	public static final int ROAD_SIDE1_Y = 0;
	public static final int ROAD_SIDE2_X = (28 * TILE_SIZE_X);
	public static final int ROAD_SIDE2_Y = 0;
	public static final int ROAD_SIDE3_X = (29 * TILE_SIZE_X);
	public static final int ROAD_SIDE3_Y = 0;
	public static final int ROAD_SIDE4_X = (30 * TILE_SIZE_X);
	public static final int ROAD_SIDE4_Y = 0;
	/* Deep Sea */
	public static final int DEEP_SEA_SOLID_X = 0;
	public static final int DEEP_SEA_SOLID_Y = TILE_SIZE_Y;
	public static final int DEEP_SEA_CORN1_X = (10 * TILE_SIZE_X) /*
																	 * L Shaped
																	 * Corner
																	 * peices
																	 */;
	public static final int DEEP_SEA_CORN1_Y = (3 * TILE_SIZE_Y);
	public static final int DEEP_SEA_CORN2_X = (11 * TILE_SIZE_X);
	public static final int DEEP_SEA_CORN2_Y = (3 * TILE_SIZE_Y);
	public static final int DEEP_SEA_CORN3_X = (12 * TILE_SIZE_X);
	public static final int DEEP_SEA_CORN3_Y = (3 * TILE_SIZE_Y);
	public static final int DEEP_SEA_CORN4_X = (13 * TILE_SIZE_X);
	public static final int DEEP_SEA_CORN4_Y = (3 * TILE_SIZE_Y);
	public static final int DEEP_SEA_SIDE1_X = (14 * TILE_SIZE_X) /* Sides */;
	public static final int DEEP_SEA_SIDE1_Y = (3 * TILE_SIZE_Y);
	public static final int DEEP_SEA_SIDE2_X = (15 * TILE_SIZE_X);
	public static final int DEEP_SEA_SIDE2_Y = (3 * TILE_SIZE_Y);
	public static final int DEEP_SEA_SIDE3_X = (16 * TILE_SIZE_X);
	public static final int DEEP_SEA_SIDE3_Y = (3 * TILE_SIZE_Y);
	public static final int DEEP_SEA_SIDE4_X = (17 * TILE_SIZE_X);
	public static final int DEEP_SEA_SIDE4_Y = (3 * TILE_SIZE_Y);
	/* Boat Peices */
	public static final int BOAT0_X = (18 * TILE_SIZE_X);
	public static final int BOAT0_Y = (3 * TILE_SIZE_Y);
	public static final int BOAT1_X = (11 * TILE_SIZE_X);
	public static final int BOAT1_Y = (6 * TILE_SIZE_Y);
	public static final int BOAT2_X = (12 * TILE_SIZE_X);
	public static final int BOAT2_Y = (6 * TILE_SIZE_Y);
	public static final int BOAT3_X = (13 * TILE_SIZE_X);
	public static final int BOAT3_Y = (6 * TILE_SIZE_Y);
	public static final int BOAT4_X = (14 * TILE_SIZE_X);
	public static final int BOAT4_Y = (6 * TILE_SIZE_Y);
	public static final int BOAT5_X = (15 * TILE_SIZE_X);
	public static final int BOAT5_Y = (6 * TILE_SIZE_Y);
	public static final int BOAT6_X = (16 * TILE_SIZE_X);
	public static final int BOAT6_Y = (6 * TILE_SIZE_Y);
	public static final int BOAT7_X = (17 * TILE_SIZE_X);
	public static final int BOAT7_Y = (6 * TILE_SIZE_Y);
	/* Mine posistion */
	public static final int MINE_X = (19 * TILE_SIZE_X);
	public static final int MINE_Y = (3 * TILE_SIZE_Y);
	public static final int TANK_X = (20 * TILE_SIZE_X);
	public static final int TANK_Y = (3 * TILE_SIZE_Y);
	/* River */
	public static final int RIVER_SOLID_X = (TILE_SIZE_X);
	public static final int RIVER_SOLID_Y = (TILE_SIZE_Y);
	/* Grass */
	public static final int GRASS_X = (2 * TILE_SIZE_X);
	public static final int GRASS_Y = TILE_SIZE_Y;
	/* Forest */
	public static final int FOREST_X = (3 * TILE_SIZE_X);
	public static final int FOREST_Y = (TILE_SIZE_Y);
	public static final int FOREST_SINGLE_X = (112 + TILE_SIZE_X);
	public static final int FOREST_SINGLE_Y = 144;
	public static final int FOREST_BR_X = (112 + (2 * TILE_SIZE_X));
	public static final int FOREST_BR_Y = 144;
	public static final int FOREST_BL_X = (112 + (3 * TILE_SIZE_X));
	public static final int FOREST_BL_Y = 144;
	public static final int FOREST_AL_X = (112 + (4 * TILE_SIZE_X));
	public static final int FOREST_AL_Y = 144;
	public static final int FOREST_AR_X = (112 + (5 * TILE_SIZE_X));
	public static final int FOREST_AR_Y = 144;
	public static final int FOREST_LEFT_X = (112 + (7 * TILE_SIZE_X));
	public static final int FOREST_LEFT_Y = 144;
	public static final int FOREST_RIGHT_X = (112 + (6 * TILE_SIZE_X));
	public static final int FOREST_RIGHT_Y = 144;
	public static final int FOREST_ABOVE_X = (112 + (9 * TILE_SIZE_X));
	public static final int FOREST_ABOVE_Y = 144;
	public static final int FOREST_BELOW_X = (112 + (8 * TILE_SIZE_X));
	public static final int FOREST_BELOW_Y = 144;
	/* Rubble */
	public static final int RUBBLE_X = (4 * TILE_SIZE_X);
	public static final int RUBBLE_Y = TILE_SIZE_Y;
	/* Crater */
	public static final int CRATER_X = (5 * TILE_SIZE_X);
	public static final int CRATER_Y = TILE_SIZE_Y;
	public static final int CRATER_SINGLE_X = (112 + TILE_SIZE_X);
	public static final int CRATER_SINGLE_Y = 144;
	public static final int CRATER_BR_X = (112 + (2 * TILE_SIZE_X));
	public static final int CRATER_BR_Y = 144;
	public static final int CRATER_BL_X = (112 + (3 * TILE_SIZE_X));
	public static final int CRATER_BL_Y = 144;
	public static final int CRATER_AL_X = (112 + (4 * TILE_SIZE_X));
	public static final int CRATER_AL_Y = 144;
	public static final int CRATER_AR_X = (112 + (5 * TILE_SIZE_X));
	public static final int CRATER_AR_Y = 144;
	public static final int CRATER_LEFT_X = (112 + (7 * TILE_SIZE_X));
	public static final int CRATER_LEFT_Y = 144;
	public static final int CRATER_RIGHT_X = (112 + (6 * TILE_SIZE_X));
	public static final int CRATER_RIGHT_Y = 144;
	public static final int CRATER_ABOVE_X = (112 + (9 * TILE_SIZE_X));
	public static final int CRATER_ABOVE_Y = 144;
	public static final int CRATER_BELOW_X = (112 + (8 * TILE_SIZE_X));
	public static final int CRATER_BELOW_Y = 144;
	/* Buildings */
	public static final int BUILD_SINGLE_X = (6 * TILE_SIZE_X) /* Single Peice */;
	public static final int BUILD_SINGLE_Y = TILE_SIZE_Y;
	public static final int BUILD_HORZ_X = (11 * TILE_SIZE_X) /*
																 * Horizontal
																 * and Vertical
																 * Peices
																 */;
	public static final int BUILD_HORZ_Y = TILE_SIZE_Y;
	public static final int BUILD_VERT_X = (12 * TILE_SIZE_X);
	public static final int BUILD_VERT_Y = TILE_SIZE_Y;
	public static final int BUILD_HORZEND1_X = (13 * TILE_SIZE_X) /*
																	 * End
																	 * Peices
																	 */;
	public static final int BUILD_HORZEND1_Y = TILE_SIZE_Y;
	public static final int BUILD_HORZEND2_X = (14 * TILE_SIZE_X);
	public static final int BUILD_HORZEND2_Y = TILE_SIZE_Y;
	public static final int BUILD_VERTEND1_X = (15 * TILE_SIZE_X);
	public static final int BUILD_VERTEND1_Y = TILE_SIZE_Y;
	public static final int BUILD_VERTEND2_X = (16 * TILE_SIZE_X);
	public static final int BUILD_VERTEND2_Y = TILE_SIZE_Y;
	public static final int BUILD_SOLID_X = (17 * TILE_SIZE_X) /*
																 * Solid -
																 * Peices on all
																 * sides
																 */;
	public static final int BUILD_SOLID_Y = TILE_SIZE_Y;
	public static final int BUILD_CORNER1_X = (18 * TILE_SIZE_X) /*
																	 * Corner
																	 * Peices
																	 */;
	public static final int BUILD_CORNER1_Y = TILE_SIZE_Y;
	public static final int BUILD_CORNER2_X = (19 * TILE_SIZE_X);
	public static final int BUILD_CORNER2_Y = TILE_SIZE_Y;
	public static final int BUILD_CORNER3_X = (20 * TILE_SIZE_X);
	public static final int BUILD_CORNER3_Y = TILE_SIZE_Y;
	public static final int BUILD_CORNER4_X = (21 * TILE_SIZE_X);
	public static final int BUILD_CORNER4_Y = TILE_SIZE_Y;
	public static final int BUILD_L1_X = (22 * TILE_SIZE_X) /* L - Shaped peices */;
	public static final int BUILD_L1_Y = TILE_SIZE_Y;
	public static final int BUILD_L2_X = (23 * TILE_SIZE_X);
	public static final int BUILD_L2_Y = TILE_SIZE_Y;
	public static final int BUILD_L3_X = (24 * TILE_SIZE_X);
	public static final int BUILD_L3_Y = TILE_SIZE_Y;
	public static final int BUILD_L4_X = (25 * TILE_SIZE_X);
	public static final int BUILD_L4_Y = TILE_SIZE_Y;
	public static final int BUILD_T1_X = (26 * TILE_SIZE_X) /* T- Shaped peices */;
	public static final int BUILD_T1_Y = TILE_SIZE_Y;
	public static final int BUILD_T2_X = (27 * TILE_SIZE_X);
	public static final int BUILD_T2_Y = TILE_SIZE_Y;
	public static final int BUILD_T3_X = (28 * TILE_SIZE_X);
	public static final int BUILD_T3_Y = TILE_SIZE_Y;
	public static final int BUILD_T4_X = (29 * TILE_SIZE_X);
	public static final int BUILD_T4_Y = TILE_SIZE_Y;
	public static final int BUILD_CROSS_X = (30 * TILE_SIZE_X) /* Cross Peice */;
	public static final int BUILD_CROSS_Y = TILE_SIZE_Y;
	public static final int BUILD_SIDE1_X = 0 /* Side Peices */;
	public static final int BUILD_SIDE1_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDE2_X = TILE_SIZE_X;
	public static final int BUILD_SIDE2_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDE3_X = (2 * TILE_SIZE_X);
	public static final int BUILD_SIDE3_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDE4_X = (3 * TILE_SIZE_X);
	public static final int BUILD_SIDE4_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN1_X = (4 * TILE_SIZE_X) /*
																	 * Side
																	 * Corners
																	 */;
	public static final int BUILD_SIDECORN1_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN2_X = (5 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN2_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN3_X = (6 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN3_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN4_X = (7 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN4_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN5_X = (8 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN5_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN6_X = (9 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN6_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN7_X = (10 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN7_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN8_X = (11 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN8_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN9_X = (12 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN9_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN10_X = (13 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN10_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN11_X = (14 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN11_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN12_X = (15 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN12_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN13_X = (16 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN13_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN14_X = (17 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN14_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN15_X = (18 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN15_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_SIDECORN16_X = (19 * TILE_SIZE_X);
	public static final int BUILD_SIDECORN16_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_TWIST1_X = (20 * TILE_SIZE_X) /*
																 * Strange
																 * Twisty Pieces
																 */;
	public static final int BUILD_TWIST1_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_TWIST2_X = (21 * TILE_SIZE_X);
	public static final int BUILD_TWIST2_Y = (2 * TILE_SIZE_Y);
	public static final int BUILD_MOST1_X = (22 * TILE_SIZE_X) /*
																 * Most covered
																 * Also last 4
																 * peices
																 */;
	public static final int BUILD_MOST1_Y = (2 * TILE_SIZE_X);
	public static final int BUILD_MOST2_X = (23 * TILE_SIZE_X);
	public static final int BUILD_MOST2_Y = (2 * TILE_SIZE_X);
	public static final int BUILD_MOST3_X = (24 * TILE_SIZE_X);
	public static final int BUILD_MOST3_Y = (2 * TILE_SIZE_X);
	public static final int BUILD_MOST4_X = (25 * TILE_SIZE_X);
	public static final int BUILD_MOST4_Y = (2 * TILE_SIZE_X);
	/* River Peices */
	public static final int RIVER_END1_X = (26 * TILE_SIZE_X) /*
																 * The 4 end U
																 * peices
																 */;
	public static final int RIVER_END1_Y = (2 * TILE_SIZE_Y);
	public static final int RIVER_END2_X = (27 * TILE_SIZE_X);
	public static final int RIVER_END2_Y = (2 * TILE_SIZE_Y);
	public static final int RIVER_END3_X = (28 * TILE_SIZE_X);
	public static final int RIVER_END3_Y = (2 * TILE_SIZE_Y);
	public static final int RIVER_END4_X = (29 * TILE_SIZE_X);
	public static final int RIVER_END4_Y = (2 * TILE_SIZE_Y);
	public static final int RIVER_SURROUND_X = (30 * TILE_SIZE_X) /*
																	 * Surrounded
																	 * on all
																	 * sides
																	 */;
	public static final int RIVER_SURROUND_Y = (2 * TILE_SIZE_Y);
	public static final int RIVER_SIDE1_X = 0 /* SIde peices eg - | | */;
	public static final int RIVER_SIDE1_Y = (3 * TILE_SIZE_Y);
	public static final int RIVER_SIDE2_X = TILE_SIZE_X;
	public static final int RIVER_SIDE2_Y = (3 * TILE_SIZE_Y);
	public static final int RIVER_ONESIDE1_X = (2 * TILE_SIZE_X) /*
																	 * One side
																	 * ground
																	 * others
																	 * water
																	 */;
	public static final int RIVER_ONESIDE1_Y = (3 * TILE_SIZE_Y);
	public static final int RIVER_ONESIDE2_X = (3 * TILE_SIZE_X);
	public static final int RIVER_ONESIDE2_Y = (3 * TILE_SIZE_Y);
	public static final int RIVER_ONESIDE3_X = (4 * TILE_SIZE_X);
	public static final int RIVER_ONESIDE3_Y = (3 * TILE_SIZE_Y);
	public static final int RIVER_ONESIDE4_X = (5 * TILE_SIZE_X);
	public static final int RIVER_ONESIDE4_Y = (3 * TILE_SIZE_Y);
	public static final int RIVER_CORN1_X = (6 * TILE_SIZE_X) /*
																 * Four corner L
																 * shaped peices
																 */;
	public static final int RIVER_CORN1_Y = (3 * TILE_SIZE_Y);
	public static final int RIVER_CORN2_X = (7 * TILE_SIZE_X);
	public static final int RIVER_CORN2_Y = (3 * TILE_SIZE_Y);
	public static final int RIVER_CORN3_X = (8 * TILE_SIZE_X);
	public static final int RIVER_CORN3_Y = (3 * TILE_SIZE_Y);
	public static final int RIVER_CORN4_X = (9 * TILE_SIZE_X);
	public static final int RIVER_CORN4_Y = (3 * TILE_SIZE_Y);
	/* Tank Tiles */
	public static final int TANK_SELF_0_X = 0;
	public static final int TANK_SELF_0_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_1_X = TILE_SIZE_X;
	public static final int TANK_SELF_1_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_2_X = (2 * TILE_SIZE_X);
	public static final int TANK_SELF_2_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_3_X = (3 * TILE_SIZE_X);
	public static final int TANK_SELF_3_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_4_X = (4 * TILE_SIZE_X);
	public static final int TANK_SELF_4_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_5_X = (5 * TILE_SIZE_X);
	public static final int TANK_SELF_5_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_6_X = (6 * TILE_SIZE_X);
	public static final int TANK_SELF_6_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_7_X = (7 * TILE_SIZE_X);
	public static final int TANK_SELF_7_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_8_X = (8 * TILE_SIZE_X);
	public static final int TANK_SELF_8_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_9_X = (9 * TILE_SIZE_X);
	public static final int TANK_SELF_9_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_10_X = (10 * TILE_SIZE_X);
	public static final int TANK_SELF_10_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_11_X = (11 * TILE_SIZE_X);
	public static final int TANK_SELF_11_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_12_X = (12 * TILE_SIZE_X);
	public static final int TANK_SELF_12_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_13_X = (13 * TILE_SIZE_X);
	public static final int TANK_SELF_13_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_14_X = (14 * TILE_SIZE_X);
	public static final int TANK_SELF_14_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELF_15_X = (15 * TILE_SIZE_X);
	public static final int TANK_SELF_15_Y = (4 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_0_X = (15 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_0_Y = (5 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_1_X = (16 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_1_Y = (5 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_2_X = (17 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_2_Y = (5 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_3_X = (18 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_3_Y = (5 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_4_X = (19 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_4_Y = (5 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_5_X = 0;
	public static final int TANK_SELFBOAT_5_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_6_X = TILE_SIZE_X;
	public static final int TANK_SELFBOAT_6_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_7_X = (2 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_7_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_8_X = (3 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_8_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_9_X = (4 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_9_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_10_X = (5 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_10_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_11_X = (6 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_11_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_12_X = (7 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_12_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_13_X = (8 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_13_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_14_X = (9 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_14_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_SELFBOAT_15_X = (10 * TILE_SIZE_X);
	public static final int TANK_SELFBOAT_15_Y = (6 * TILE_SIZE_Y);
	public static final int TANK_TRANSPARENT_X = 342;
	public static final int TANK_TRANSPARENT_Y = 62;
	public static final int TANK_GOOD_0_X = 304;
	public static final int TANK_GOOD_0_Y = 98;
	public static final int TANK_GOOD_1_X = (TANK_GOOD_0_X + TILE_SIZE_X);
	public static final int TANK_GOOD_1_Y = TANK_GOOD_0_Y;
	public static final int TANK_GOOD_2_X = (TANK_GOOD_0_X + (2 * TILE_SIZE_X));
	public static final int TANK_GOOD_2_Y = TANK_GOOD_0_Y;
	public static final int TANK_GOOD_3_X = 96;
	public static final int TANK_GOOD_3_Y = 144;
	public static final int TANK_GOOD_4_X = (TANK_GOOD_0_X + (3 * TILE_SIZE_X));
	public static final int TANK_GOOD_4_Y = (TANK_GOOD_0_Y);
	public static final int TANK_GOOD_5_X = (TANK_GOOD_0_X + (4 * TILE_SIZE_X));
	public static final int TANK_GOOD_5_Y = TANK_GOOD_0_Y;
	public static final int TANK_GOOD_6_X = (TANK_GOOD_0_X + (5 * TILE_SIZE_X));
	public static final int TANK_GOOD_6_Y = TANK_GOOD_0_Y;
	public static final int TANK_GOOD_7_X = (TANK_GOOD_0_X + (6 * TILE_SIZE_X));
	public static final int TANK_GOOD_7_Y = TANK_GOOD_0_Y;
	public static final int TANK_GOOD_8_X = (TANK_GOOD_0_X + (7 * TILE_SIZE_X));
	public static final int TANK_GOOD_8_Y = TANK_GOOD_0_Y;
	public static final int TANK_GOOD_9_X = (TANK_GOOD_0_X + (8 * TILE_SIZE_X));
	public static final int TANK_GOOD_9_Y = TANK_GOOD_0_Y;
	public static final int TANK_GOOD_10_X = (TANK_GOOD_0_X + (9 * TILE_SIZE_X));
	public static final int TANK_GOOD_10_Y = TANK_GOOD_0_Y;
	public static final int TANK_GOOD_11_X = (TANK_GOOD_0_X + (10 * TILE_SIZE_X));
	public static final int TANK_GOOD_11_Y = TANK_GOOD_0_Y;
	public static final int TANK_GOOD_12_X = (TANK_GOOD_0_X + (11 * TILE_SIZE_X));
	public static final int TANK_GOOD_12_Y = TANK_GOOD_0_Y;
	/* Was 273 */
	public static final int TANK_GOOD_13_X = 272;
	public static final int TANK_GOOD_13_Y = 114;
	public static final int TANK_GOOD_14_X = (TANK_GOOD_13_X + TILE_SIZE_X);
	public static final int TANK_GOOD_14_Y = TANK_GOOD_13_Y;
	public static final int TANK_GOOD_15_X = (TANK_GOOD_13_X + (2 * TILE_SIZE_X));
	public static final int TANK_GOOD_15_Y = TANK_GOOD_13_Y;
	/* Was 320 */
	public static final int TANK_GOODBOAT_0_X = 319;
	public static final int TANK_GOODBOAT_0_Y = 114;
	public static final int TANK_GOODBOAT_1_X = (TANK_GOODBOAT_0_X + TILE_SIZE_X);
	public static final int TANK_GOODBOAT_1_Y = TANK_GOODBOAT_0_Y;
	public static final int TANK_GOODBOAT_2_X = (TANK_GOODBOAT_0_X + (2 * TILE_SIZE_X));
	public static final int TANK_GOODBOAT_2_Y = TANK_GOODBOAT_0_Y;
	public static final int TANK_GOODBOAT_3_X = (TANK_GOODBOAT_0_X + (3 * TILE_SIZE_X));
	public static final int TANK_GOODBOAT_3_Y = TANK_GOODBOAT_0_Y;
	public static final int TANK_GOODBOAT_4_X = (TANK_GOODBOAT_0_X + (4 * TILE_SIZE_X));
	public static final int TANK_GOODBOAT_4_Y = TANK_GOODBOAT_0_Y;
	public static final int TANK_GOODBOAT_5_X = (TANK_GOODBOAT_0_X + (5 * TILE_SIZE_X));
	public static final int TANK_GOODBOAT_5_Y = TANK_GOODBOAT_0_Y;
	public static final int TANK_GOODBOAT_6_X = (TANK_GOODBOAT_0_X + (6 * TILE_SIZE_X));
	public static final int TANK_GOODBOAT_6_Y = TANK_GOODBOAT_0_Y;
	public static final int TANK_GOODBOAT_7_X = (TANK_GOODBOAT_0_X + (7 * TILE_SIZE_X));
	public static final int TANK_GOODBOAT_7_Y = TANK_GOODBOAT_0_Y;
	public static final int TANK_GOODBOAT_8_X = (TANK_GOODBOAT_0_X + (8 * TILE_SIZE_X));
	public static final int TANK_GOODBOAT_8_Y = TANK_GOODBOAT_0_Y;
	public static final int TANK_GOODBOAT_9_X = (TANK_GOODBOAT_0_X + (9 * TILE_SIZE_X));
	public static final int TANK_GOODBOAT_9_Y = TANK_GOODBOAT_0_Y;
	public static final int TANK_GOODBOAT_10_X = (TANK_GOODBOAT_0_X + (10 * TILE_SIZE_X));
	public static final int TANK_GOODBOAT_10_Y = TANK_GOODBOAT_0_Y;
	public static final int TANK_GOODBOAT_11_X = 0;
	public static final int TANK_GOODBOAT_11_Y = (8 * TILE_SIZE_Y);
	public static final int TANK_GOODBOAT_12_X = TILE_SIZE_X;
	public static final int TANK_GOODBOAT_12_Y = (8 * TILE_SIZE_Y);
	public static final int TANK_GOODBOAT_13_X = (2 * TILE_SIZE_X);
	public static final int TANK_GOODBOAT_13_Y = (8 * TILE_SIZE_Y);
	public static final int TANK_GOODBOAT_14_X = (3 * TILE_SIZE_X);
	public static final int TANK_GOODBOAT_14_Y = (8 * TILE_SIZE_Y);
	public static final int TANK_GOODBOAT_15_X = (4 * TILE_SIZE_X);
	public static final int TANK_GOODBOAT_15_Y = (8 * TILE_SIZE_Y);
	public static final int TANK_EVIL_0_X = (5 * TILE_SIZE_X);
	public static final int TANK_EVIL_0_Y = (8 * TILE_SIZE_Y);
	public static final int TANK_EVIL_1_X = (TANK_EVIL_0_X + TILE_SIZE_X);
	public static final int TANK_EVIL_1_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_2_X = (TANK_EVIL_0_X + (2 * TILE_SIZE_X));
	public static final int TANK_EVIL_2_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_3_X = (TANK_EVIL_0_X + (3 * TILE_SIZE_X));
	public static final int TANK_EVIL_3_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_4_X = (TANK_EVIL_0_X + (4 * TILE_SIZE_X));
	public static final int TANK_EVIL_4_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_5_X = (TANK_EVIL_0_X + (5 * TILE_SIZE_X));
	public static final int TANK_EVIL_5_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_6_X = (TANK_EVIL_0_X + (6 * TILE_SIZE_X));
	public static final int TANK_EVIL_6_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_7_X = (TANK_EVIL_0_X + (7 * TILE_SIZE_X));
	public static final int TANK_EVIL_7_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_8_X = (TANK_EVIL_0_X + (8 * TILE_SIZE_X));
	public static final int TANK_EVIL_8_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_9_X = (TANK_EVIL_0_X + (9 * TILE_SIZE_X));
	public static final int TANK_EVIL_9_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_10_X = (TANK_EVIL_0_X + (10 * TILE_SIZE_X));
	public static final int TANK_EVIL_10_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_11_X = (TANK_EVIL_0_X + (11 * TILE_SIZE_X));
	public static final int TANK_EVIL_11_Y = TANK_EVIL_0_Y;
	public static final int TANK_EVIL_12_X = 272;
	public static final int TANK_EVIL_12_Y = 130;
	public static final int TANK_EVIL_13_X = (TANK_EVIL_12_X + TILE_SIZE_X);
	public static final int TANK_EVIL_13_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVIL_14_X = (TANK_EVIL_12_X + (2 * TILE_SIZE_X));
	public static final int TANK_EVIL_14_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVIL_15_X = (TANK_EVIL_12_X + (3 * TILE_SIZE_X));
	public static final int TANK_EVIL_15_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_0_X = (TANK_EVIL_12_X + (4 * TILE_SIZE_X));
	public static final int TANK_EVILBOAT_0_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_1_X = (TANK_EVIL_12_X + (5 * TILE_SIZE_X));
	public static final int TANK_EVILBOAT_1_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_2_X = (TANK_EVIL_12_X + (6 * TILE_SIZE_X));
	public static final int TANK_EVILBOAT_2_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_3_X = (TANK_EVIL_12_X + (7 * TILE_SIZE_X));
	public static final int TANK_EVILBOAT_3_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_4_X = (TANK_EVIL_12_X + (8 * TILE_SIZE_X));
	public static final int TANK_EVILBOAT_4_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_5_X = (TANK_EVIL_12_X + (9 * TILE_SIZE_X));
	public static final int TANK_EVILBOAT_5_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_6_X = (TANK_EVIL_12_X + (10 * TILE_SIZE_X));
	public static final int TANK_EVILBOAT_6_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_7_X = (TANK_EVIL_12_X + (11 * TILE_SIZE_X));
	public static final int TANK_EVILBOAT_7_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_8_X = (TANK_EVIL_12_X + (12 * TILE_SIZE_X));
	public static final int TANK_EVILBOAT_8_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_9_X = (TANK_EVIL_12_X + (13 * TILE_SIZE_X));
	public static final int TANK_EVILBOAT_9_Y = TANK_EVIL_12_Y;
	public static final int TANK_EVILBOAT_10_X = 0;
	public static final int TANK_EVILBOAT_10_Y = (9 * TILE_SIZE_Y);
	public static final int TANK_EVILBOAT_11_X = TILE_SIZE_X;
	public static final int TANK_EVILBOAT_11_Y = (9 * TILE_SIZE_Y);
	public static final int TANK_EVILBOAT_12_X = (2 * TILE_SIZE_X);
	public static final int TANK_EVILBOAT_12_Y = (9 * TILE_SIZE_Y);
	public static final int TANK_EVILBOAT_13_X = (3 * TILE_SIZE_X);
	public static final int TANK_EVILBOAT_13_Y = (9 * TILE_SIZE_Y);
	public static final int TANK_EVILBOAT_14_X = (4 * TILE_SIZE_X);
	public static final int TANK_EVILBOAT_14_Y = (9 * TILE_SIZE_Y);
	public static final int TANK_EVILBOAT_15_X = (5 * TILE_SIZE_X);
	public static final int TANK_EVILBOAT_15_Y = (9 * TILE_SIZE_Y);
	/* Swamp */
	public static final int SWAMP_X = (7 * TILE_SIZE_X);
	public static final int SWAMP_Y = TILE_SIZE_Y;
	/* Shot = Building */;
	public static final int SHOT_BUILDING_X = (8 * TILE_SIZE_X);
	public static final int SHOT_BUILDING_Y = TILE_SIZE_Y;
	/* Pillboxes */
	public static final int PILL_EVIL15_X = (9 * TILE_SIZE_X);
	public static final int PILL_EVIL15_Y = TILE_SIZE_Y;
	public static final int PILL_EVIL14_X = (14 * TILE_SIZE_X);
	public static final int PILL_EVIL14_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL13_X = (13 * TILE_SIZE_X);
	public static final int PILL_EVIL13_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL12_X = (12 * TILE_SIZE_X);
	public static final int PILL_EVIL12_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL11_X = (11 * TILE_SIZE_X);
	public static final int PILL_EVIL11_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL10_X = (10 * TILE_SIZE_X);
	public static final int PILL_EVIL10_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL9_X = (9 * TILE_SIZE_X);
	public static final int PILL_EVIL9_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL8_X = (8 * TILE_SIZE_X);
	public static final int PILL_EVIL8_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL7_X = (7 * TILE_SIZE_X);
	public static final int PILL_EVIL7_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL6_X = (6 * TILE_SIZE_X);
	public static final int PILL_EVIL6_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL5_X = (5 * TILE_SIZE_X);
	public static final int PILL_EVIL5_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL4_X = (4 * TILE_SIZE_X);
	public static final int PILL_EVIL4_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL3_X = (3 * TILE_SIZE_X);
	public static final int PILL_EVIL3_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL2_X = (2 * TILE_SIZE_X);
	public static final int PILL_EVIL2_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL1_X = TILE_SIZE_X;
	public static final int PILL_EVIL1_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_EVIL0_X = 0;
	public static final int PILL_EVIL0_Y = (5 * TILE_SIZE_Y);
	public static final int PILL_GOOD15_X = (15 * TILE_SIZE_X);
	public static final int PILL_GOOD15_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD14_X = (14 * TILE_SIZE_X);
	public static final int PILL_GOOD14_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD13_X = (13 * TILE_SIZE_X);
	public static final int PILL_GOOD13_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD12_X = (12 * TILE_SIZE_X);
	public static final int PILL_GOOD12_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD11_X = (11 * TILE_SIZE_X);
	public static final int PILL_GOOD11_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD10_X = (10 * TILE_SIZE_X);
	public static final int PILL_GOOD10_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD9_X = (9 * TILE_SIZE_X);
	public static final int PILL_GOOD9_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD8_X = (8 * TILE_SIZE_X);
	public static final int PILL_GOOD8_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD7_X = (7 * TILE_SIZE_X);
	public static final int PILL_GOOD7_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD6_X = (6 * TILE_SIZE_X);
	public static final int PILL_GOOD6_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD5_X = (5 * TILE_SIZE_X);
	public static final int PILL_GOOD5_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD4_X = (4 * TILE_SIZE_X);
	public static final int PILL_GOOD4_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD3_X = (3 * TILE_SIZE_X);
	public static final int PILL_GOOD3_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD2_X = (2 * TILE_SIZE_X);
	public static final int PILL_GOOD2_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD1_X = TILE_SIZE_X;
	public static final int PILL_GOOD1_Y = (7 * TILE_SIZE_Y);
	public static final int PILL_GOOD0_X = 0;
	public static final int PILL_GOOD0_Y = (7 * TILE_SIZE_Y);
	/* Bases */
	public static final int BASE_GOOD_X = (10 * TILE_SIZE_X);
	public static final int BASE_GOOD_Y = TILE_SIZE_Y;
	public static final int BASE_NEUTRAL_X = (16 * TILE_SIZE_X);
	public static final int BASE_NEUTRAL_Y = (4 * TILE_SIZE_Y);
	public static final int BASE_EVIL_X = 112;
	public static final int BASE_EVIL_Y = 144;
	/* Gunsight */
	public static final int GUNSIGHT_X = (17 * TILE_SIZE_X);
	public static final int GUNSIGHT_Y = (4 * TILE_SIZE_Y);
	/* Explosion animation */
	public static final int EXPLOSION1_X = (29 * TILE_SIZE_X);
	public static final int EXPLOSION1_Y = (3 * TILE_SIZE_Y);
	public static final int EXPLOSION2_X = (30 * TILE_SIZE_X);
	public static final int EXPLOSION2_Y = (3 * TILE_SIZE_Y);
	public static final int EXPLOSION3_X = (29 * TILE_SIZE_X);
	public static final int EXPLOSION3_Y = (4 * TILE_SIZE_Y);
	public static final int EXPLOSION4_X = (30 * TILE_SIZE_X);
	public static final int EXPLOSION4_Y = (4 * TILE_SIZE_Y);
	public static final int EXPLOSION5_X = (29 * TILE_SIZE_X);
	public static final int EXPLOSION5_Y = (5 * TILE_SIZE_Y);
	public static final int EXPLOSION6_X = (30 * TILE_SIZE_X);
	public static final int EXPLOSION6_Y = (5 * TILE_SIZE_Y);
	public static final int EXPLOSION7_X = (18 * TILE_SIZE_X);
	public static final int EXPLOSION7_Y = (4 * TILE_SIZE_Y);
	public static final int EXPLOSION8_X = (19 * TILE_SIZE_X);
	public static final int EXPLOSION8_Y = (4 * TILE_SIZE_Y);
	/* Shells positions in the tile file */
	public static final int SHELL_0_X = 452;
	public static final int SHELL_0_Y = 72;
	public static final int SHELL_0_WIDTH = 3;
	public static final int SHELL_0_HEIGHT = 4;
	public static final int SHELL_1_X = 455;
	public static final int SHELL_1_Y = 72;
	public static final int SHELL_1_WIDTH = 3;
	public static final int SHELL_1_HEIGHT = 4;
	public static final int SHELL_2_X = 458;
	public static final int SHELL_2_Y = 72;
	public static final int SHELL_2_WIDTH = 4;
	public static final int SHELL_2_HEIGHT = 4;
	public static final int SHELL_3_X = 452;
	public static final int SHELL_3_Y = 76;
	public static final int SHELL_3_WIDTH = 4;
	public static final int SHELL_3_HEIGHT = 3;
	public static final int SHELL_4_X = 456;
	public static final int SHELL_4_Y = 76;
	public static final int SHELL_4_WIDTH = 4;
	public static final int SHELL_4_HEIGHT = 3;
	public static final int SHELL_5_X = 460;
	public static final int SHELL_5_Y = 76;
	public static final int SHELL_5_WIDTH = 4;
	public static final int SHELL_5_HEIGHT = 3;
	public static final int SHELL_6_X = 452;
	public static final int SHELL_6_Y = 79;
	public static final int SHELL_6_WIDTH = 4;
	public static final int SHELL_6_HEIGHT = 4;
	public static final int SHELL_7_X = 456;
	public static final int SHELL_7_Y = 79;
	public static final int SHELL_7_WIDTH = 3;
	public static final int SHELL_7_HEIGHT = 4;
	public static final int SHELL_8_X = 459;
	public static final int SHELL_8_Y = 79;
	public static final int SHELL_8_WIDTH = 3;
	public static final int SHELL_8_HEIGHT = 4;
	public static final int SHELL_9_X = 452;
	public static final int SHELL_9_Y = 83;
	public static final int SHELL_9_WIDTH = 3;
	public static final int SHELL_9_HEIGHT = 4;
	public static final int SHELL_10_X = 456;
	public static final int SHELL_10_Y = 83;
	public static final int SHELL_10_WIDTH = 3;
	public static final int SHELL_10_HEIGHT = 4;
	public static final int SHELL_11_X = 459;
	public static final int SHELL_11_Y = 83;
	public static final int SHELL_11_WIDTH = 4;
	public static final int SHELL_11_HEIGHT = 3;
	public static final int SHELL_12_X = 452;
	public static final int SHELL_12_Y = 87;
	public static final int SHELL_12_WIDTH = 4;
	public static final int SHELL_12_HEIGHT = 3;
	public static final int SHELL_13_X = 456;
	public static final int SHELL_13_Y = 87;
	public static final int SHELL_13_WIDTH = 4;
	public static final int SHELL_13_HEIGHT = 3;
	public static final int SHELL_14_X = 452;
	public static final int SHELL_14_Y = 90;
	public static final int SHELL_14_WIDTH = 4;
	public static final int SHELL_14_HEIGHT = 4;
	public static final int SHELL_15_X = 456;
	public static final int SHELL_15_Y = 90;
	public static final int SHELL_15_WIDTH = 4;
	public static final int SHELL_15_HEIGHT = 3;
	/* Indents around the selected building item */
	public static final int INDENT_ON_X = 320;
	public static final int INDENT_ON_Y = 48;
	public static final int INDENT_OFF_X = 374;
	public static final int INDENT_OFF_Y = 48;
	/* Indents around the selected building items dot */
	public static final int INDENT_DOT_ON_X = 434;
	public static final int INDENT_DOT_ON_Y = 48;
	public static final int INDENT_DOT_OFF_X = 428;
	public static final int INDENT_DOT_OFF_Y = 48;
	/* The mouse square to draw on a square */
	public static final int MOUSE_SQUARE_X = (18 * (TILE_SIZE_X));
	public static final int MOUSE_SQUARE_Y = (6 * (TILE_SIZE_Y));
	/* Status Tiles */
	public static final int STATUS_ITEM_DEAD_X = 440;
	public static final int STATUS_ITEM_DEAD_Y = 60;
	public static final int STATUS_BASE_NEUTRAL_X = 428;
	public static final int STATUS_BASE_NEUTRAL_Y = 54;
	public static final int STATUS_BASE_ALLIEGOOD_X = 452;
	public static final int STATUS_BASE_ALLIEGOOD_Y = 48;
	public static final int STATUS_BASE_GOOD_X = 440;
	public static final int STATUS_BASE_GOOD_Y = 48;
	public static final int STATUS_BASE_EVIL_X = 452;
	public static final int STATUS_BASE_EVIL_Y = 60;
	public static final int STATUS_PILLBOX_NEUTRAL_X = 428;
	public static final int STATUS_PILLBOX_NEUTRAL_Y = 66;
	public static final int STATUS_PILLBOX_GOOD_X = STATUS_BASE_GOOD_X;
	public static final int STATUS_PILLBOX_GOOD_Y = STATUS_BASE_GOOD_Y;
	public static final int STATUS_PILLBOX_EVIL_X = 428;
	public static final int STATUS_PILLBOX_EVIL_Y = 78;
	public static final int STATUS_PILLBOX_TANKGOOD_X = 440;
	public static final int STATUS_PILLBOX_TANKGOOD_Y = 72;
	public static final int STATUS_PILLBOX_TANKALLIE_X = 272;
	public static final int STATUS_PILLBOX_TANKALLIE_Y = 148;
	public static final int STATUS_PILLBOX_ALLIEGOOD_X = 452;
	public static final int STATUS_PILLBOX_ALLIEGOOD_Y = 48;
	public static final int STATUS_PILLBOX_TANKEVIL_X = 440;
	public static final int STATUS_PILLBOX_TANKEVIL_Y = 84;

	/* Tank Status */
	public static final int STATUS_TANK_NONE_X = (10 * TILE_SIZE_X);
	public static final int STATUS_TANK_NONE_Y = 0;
	public static final int STATUS_TANK_SELF_X = (STATUS_BASE_GOOD_X);
	public static final int STATUS_TANK_SELF_Y = (STATUS_BASE_GOOD_Y);
	public static final int STATUS_TANK_GOOD_X = 452;
	public static final int STATUS_TANK_GOOD_Y = 48;
	public static final int STATUS_TANK_EVIL_X = 452;
	public static final int STATUS_TANK_EVIL_Y = 60;
	public static final int LGM0_X = 431;
	public static final int LGM0_Y = 90;
	public static final int LGM1_X = 428;
	public static final int LGM1_Y = 90;
	public static final int LGM2_X = 434;
	public static final int LGM2_Y = 90;
	public static final int LGM_WIDTH = 3;
	public static final int LGM_HEIGHT = 4;
	public static final int LGM_HELICOPTER_X = (16 * TILE_SIZE_X);
	public static final int LGM_HELICOPTER_Y = (7 * TILE_SIZE_Y);

	public static final int MAIN_WINDOW_SIZE_X = 515;

	public static final int MAIN_WINDOW_SIZE_Y = 325;

	/** Creates a new instance of DrawPositions */
	public DrawPositions() {
	}
}
