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

/*********************************************************
*Name:          MapView
*Filename:      MapView.h
*Author:        John Morrison
*Creation Date: 31/10/98
*Last Modified: 31/10/98
*Purpose:
*  Provides the front end for viewing maps
*********************************************************/

#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <windows.h>
#include "global.h"
#include "screen.h"

/* The name of the main window Class */
#define WIND_CLASSNAME "BMap View" 
#define WIND_TITLE "Bolo Map Viewer"

/* Values of the arrow keys  and page up down home and keys*/
#define KEY_LEFT 37
#define KEY_UP 38
#define KEY_RIGHT 39
#define KEY_DOWN 40
#define PAGE_LEFT 35
#define PAGE_RIGHT 36
#define PAGE_UP 33
#define PAGE_DOWN 34

/* Open Dialog Box Stuff */
#define OPEN_FILE_FILTERS "Map Files\0*.MAP\0All Files\0*.*\0"
#define OPEN_FILE_TITLE "Open File..."

/* The name of the map tileset file (Now points to a resource) */
#define TILE_FILE IDB_TILES
#define TILE_FILE_X 496
#define TILE_FILE_Y 64//163

#define TILE_SIZE_X 16
#define TILE_SIZE_Y 16


/* Locations in the tile file of each tile */

#define ROAD_HORZ_X 0
#define ROAD_HORZ_Y 0
#define ROAD_VERT_X TILE_SIZE_X
#define ROAD_VERT_Y 0
/* The road corners */
#define ROAD_CORNER1_X  2 * TILE_SIZE_X
#define ROAD_CORNER1_Y 0
#define ROAD_CORNER2_X 3 * TILE_SIZE_X
#define ROAD_CORNER2_Y 0
#define ROAD_CORNER3_X 4 * TILE_SIZE_X
#define ROAD_CORNER3_Y 0
#define ROAD_CORNER4_X 5 * TILE_SIZE_X
#define ROAD_CORNER4_Y 0


/* The 4 solid road corners */

#define ROAD_CORNER5_X 6 * TILE_SIZE_X
#define ROAD_CORNER5_Y 0
#define ROAD_CORNER6_X 7 * TILE_SIZE_X
#define ROAD_CORNER6_Y 0
#define ROAD_CORNER7_X 8 * TILE_SIZE_X
#define ROAD_CORNER7_Y 0
#define ROAD_CORNER8_X 9 * TILE_SIZE_X
#define ROAD_CORNER8_Y 0

/* Solid road */

#define ROAD_SOLID_X 10 * TILE_SIZE_X
#define ROAD_SOLID_Y 0

/* Cross roads */
#define ROAD_CROSS_X 11 * TILE_SIZE_X
#define ROAD_CROSS_Y 0

/* T Junctions */
#define ROAD_T1_X 12 * TILE_SIZE_X    
#define ROAD_T1_Y 0
#define ROAD_T2_X 13 * TILE_SIZE_X    
#define ROAD_T2_Y 0
#define ROAD_T3_X 14 * TILE_SIZE_X    
#define ROAD_T3_Y 0
#define ROAD_T4_X 15 * TILE_SIZE_X    
#define ROAD_T4_Y 0


/* 1 side over other normal */
#define ROAD_WATER1_X 16 * TILE_SIZE_X 
#define ROAD_WATER1_Y 0
#define ROAD_WATER2_X 17 * TILE_SIZE_X 
#define ROAD_WATER2_Y 0
#define ROAD_WATER3_X 18 * TILE_SIZE_X 
#define ROAD_WATER3_Y 0
#define ROAD_WATER4_X 19 * TILE_SIZE_X 
#define ROAD_WATER4_Y 0

/* Corners on water */

#define ROAD_WATER5_X 20 * TILE_SIZE_X 
#define ROAD_WATER5_Y 0
#define ROAD_WATER6_X 21 * TILE_SIZE_X 
#define ROAD_WATER6_Y 0
#define ROAD_WATER7_X 22 * TILE_SIZE_X 
#define ROAD_WATER7_Y 0
#define ROAD_WATER8_X 23 * TILE_SIZE_X 
#define ROAD_WATER8_Y 0

/* Horizontal water peice */
#define ROAD_WATER9_X 24 * TILE_SIZE_X 
#define ROAD_WATER9_Y 0

/* Vertical water peice */
#define ROAD_WATER10_X 25 * TILE_SIZE_X 
#define ROAD_WATER10_Y 0

/* Lone water peice */
#define ROAD_WATER11_X 26 * TILE_SIZE_X 
#define ROAD_WATER11_Y 0

/* Road single side peices */
#define ROAD_SIDE1_X 27 * TILE_SIZE_X 
#define ROAD_SIDE1_Y 0
#define ROAD_SIDE2_X 28 * TILE_SIZE_X 
#define ROAD_SIDE2_Y 0
#define ROAD_SIDE3_X 29 * TILE_SIZE_X 
#define ROAD_SIDE3_Y 0
#define ROAD_SIDE4_X 30 * TILE_SIZE_X 
#define ROAD_SIDE4_Y 0



/* Deep Sea */
#define DEEP_SEA_SOLID_X 0
#define DEEP_SEA_SOLID_Y TILE_SIZE_Y
#define DEEP_SEA_CORN1_X 10 * TILE_SIZE_X  /* L Shaped Corner peices */
#define DEEP_SEA_CORN1_Y 3 * TILE_SIZE_Y
#define DEEP_SEA_CORN2_X 11 * TILE_SIZE_X 
#define DEEP_SEA_CORN2_Y 3 * TILE_SIZE_Y
#define DEEP_SEA_CORN3_X 12 * TILE_SIZE_X 
#define DEEP_SEA_CORN3_Y 3 * TILE_SIZE_Y
#define DEEP_SEA_CORN4_X 13 * TILE_SIZE_X 
#define DEEP_SEA_CORN4_Y 3 * TILE_SIZE_Y
#define DEEP_SEA_SIDE1_X 14 * TILE_SIZE_X  /* Sides */
#define DEEP_SEA_SIDE1_Y 3 * TILE_SIZE_Y
#define DEEP_SEA_SIDE2_X 15 * TILE_SIZE_X 
#define DEEP_SEA_SIDE2_Y 3 * TILE_SIZE_Y
#define DEEP_SEA_SIDE3_X 16 * TILE_SIZE_X 
#define DEEP_SEA_SIDE3_Y 3 * TILE_SIZE_Y
#define DEEP_SEA_SIDE4_X 17 * TILE_SIZE_X 
#define DEEP_SEA_SIDE4_Y 3 * TILE_SIZE_Y

/* Boat Peice */
#define BOAT_X 18 * TILE_SIZE_X
#define BOAT_Y 3 * TILE_SIZE_Y

/* Mine posistion */
#define MINE_X 19 * TILE_SIZE_X
#define MINE_Y 3 * TILE_SIZE_Y

#define TANK_X 20 * TILE_SIZE_X
#define TANK_Y 3 * TILE_SIZE_Y

/* River */
#define RIVER_SOLID_X TILE_SIZE_X
#define RIVER_SOLID_Y TILE_SIZE_Y

/* Grass */
#define GRASS_X 2 * TILE_SIZE_X
#define GRASS_Y TILE_SIZE_Y

/* Forest */
#define FOREST_X 3 * TILE_SIZE_X
#define FOREST_Y TILE_SIZE_Y

/* Rubble */
#define RUBBLE_X 4 * TILE_SIZE_X
#define RUBBLE_Y TILE_SIZE_Y

/* Crater */
#define CRATER_X 5 * TILE_SIZE_X
#define CRATER_Y  TILE_SIZE_Y

/* Buildings */
#define BUILD_SINGLE_X 6 * TILE_SIZE_X /* Single Peice */
#define BUILD_SINGLE_Y TILE_SIZE_Y
#define BUILD_HORZ_X 11 * TILE_SIZE_X /* Horizontal and Vertical Peices */
#define BUILD_HORZ_Y TILE_SIZE_Y 
#define BUILD_VERT_X 12 * TILE_SIZE_X
#define BUILD_VERT_Y TILE_SIZE_Y
#define BUILD_HORZEND1_X 13 * TILE_SIZE_X /* End Peices */
#define BUILD_HORZEND1_Y TILE_SIZE_Y
#define BUILD_HORZEND2_X 14 * TILE_SIZE_X
#define BUILD_HORZEND2_Y TILE_SIZE_Y
#define BUILD_VERTEND1_X 15 * TILE_SIZE_X
#define BUILD_VERTEND1_Y TILE_SIZE_Y
#define BUILD_VERTEND2_X 16 * TILE_SIZE_X
#define BUILD_VERTEND2_Y TILE_SIZE_Y
#define BUILD_SOLID_X 17 * TILE_SIZE_X /* Solid - Peices on all sides */
#define BUILD_SOLID_Y TILE_SIZE_Y
#define BUILD_CORNER1_X 18 * TILE_SIZE_X /* Corner Peices */
#define BUILD_CORNER1_Y TILE_SIZE_Y
#define BUILD_CORNER2_X 19 * TILE_SIZE_X
#define BUILD_CORNER2_Y TILE_SIZE_Y
#define BUILD_CORNER3_X 20 * TILE_SIZE_X
#define BUILD_CORNER3_Y TILE_SIZE_Y
#define BUILD_CORNER4_X 21 * TILE_SIZE_X
#define BUILD_CORNER4_Y TILE_SIZE_Y
#define BUILD_L1_X 22 * TILE_SIZE_X /* L - Shaped peices */
#define BUILD_L1_Y TILE_SIZE_Y
#define BUILD_L2_X 23 * TILE_SIZE_X
#define BUILD_L2_Y TILE_SIZE_Y
#define BUILD_L3_X 24 * TILE_SIZE_X
#define BUILD_L3_Y TILE_SIZE_Y
#define BUILD_L4_X 25 * TILE_SIZE_X
#define BUILD_L4_Y TILE_SIZE_Y
#define BUILD_T1_X 26 * TILE_SIZE_X /* T- Shaped peices */
#define BUILD_T1_Y TILE_SIZE_Y
#define BUILD_T2_X 27 * TILE_SIZE_X
#define BUILD_T2_Y TILE_SIZE_Y
#define BUILD_T3_X 28 * TILE_SIZE_X
#define BUILD_T3_Y TILE_SIZE_Y
#define BUILD_T4_X 29 * TILE_SIZE_X
#define BUILD_T4_Y TILE_SIZE_Y
#define BUILD_CROSS_X 30 * TILE_SIZE_X /* Cross Peice */
#define BUILD_CROSS_Y TILE_SIZE_Y
#define BUILD_SIDE1_X 0 /* Side Peices */
#define BUILD_SIDE1_Y 2 * TILE_SIZE_Y
#define BUILD_SIDE2_X TILE_SIZE_X
#define BUILD_SIDE2_Y 2 * TILE_SIZE_Y
#define BUILD_SIDE3_X 2 * TILE_SIZE_X
#define BUILD_SIDE3_Y 2 * TILE_SIZE_Y
#define BUILD_SIDE4_X 3 * TILE_SIZE_X
#define BUILD_SIDE4_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN1_X 4 * TILE_SIZE_X /* Side Corners */
#define BUILD_SIDECORN1_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN2_X 5 * TILE_SIZE_X
#define BUILD_SIDECORN2_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN3_X 6 * TILE_SIZE_X
#define BUILD_SIDECORN3_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN4_X 7 * TILE_SIZE_X
#define BUILD_SIDECORN4_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN5_X 8 * TILE_SIZE_X
#define BUILD_SIDECORN5_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN6_X 9 * TILE_SIZE_X
#define BUILD_SIDECORN6_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN7_X 10 * TILE_SIZE_X
#define BUILD_SIDECORN7_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN8_X 11 * TILE_SIZE_X
#define BUILD_SIDECORN8_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN9_X 12 * TILE_SIZE_X
#define BUILD_SIDECORN9_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN10_X 13 * TILE_SIZE_X
#define BUILD_SIDECORN10_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN11_X 14 * TILE_SIZE_X
#define BUILD_SIDECORN11_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN12_X 15 * TILE_SIZE_X
#define BUILD_SIDECORN12_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN13_X 16 * TILE_SIZE_X
#define BUILD_SIDECORN13_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN14_X 17 * TILE_SIZE_X
#define BUILD_SIDECORN14_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN15_X 18 * TILE_SIZE_X
#define BUILD_SIDECORN15_Y 2 * TILE_SIZE_Y
#define BUILD_SIDECORN16_X 19 * TILE_SIZE_X
#define BUILD_SIDECORN16_Y 2 * TILE_SIZE_Y
#define BUILD_TWIST1_X 20 * TILE_SIZE_X /* Strange Twisty Pieces */
#define BUILD_TWIST1_Y 2 * TILE_SIZE_Y
#define BUILD_TWIST2_X 21 * TILE_SIZE_X
#define BUILD_TWIST2_Y 2 * TILE_SIZE_Y
#define BUILD_MOST1_X 22 * TILE_SIZE_X /* Most covered Also last 4 peices */
#define BUILD_MOST1_Y 2 * TILE_SIZE_X 
#define BUILD_MOST2_X 23 * TILE_SIZE_X 
#define BUILD_MOST2_Y 2 * TILE_SIZE_X 
#define BUILD_MOST3_X 24 * TILE_SIZE_X 
#define BUILD_MOST3_Y 2 * TILE_SIZE_X 
#define BUILD_MOST4_X 25 * TILE_SIZE_X 
#define BUILD_MOST4_Y 2 * TILE_SIZE_X 

/* River Peices */
#define RIVER_END1_X 26 * TILE_SIZE_X /* The 4 end U peices */
#define RIVER_END1_Y 2 * TILE_SIZE_Y
#define RIVER_END2_X 27 * TILE_SIZE_X 
#define RIVER_END2_Y 2 * TILE_SIZE_Y
#define RIVER_END3_X 28 * TILE_SIZE_X 
#define RIVER_END3_Y 2 * TILE_SIZE_Y
#define RIVER_END4_X 29 * TILE_SIZE_X 
#define RIVER_END4_Y 2 * TILE_SIZE_Y
#define RIVER_SURROUND_X 30 * TILE_SIZE_X  /* Surrounded on all sides */
#define RIVER_SURROUND_Y 2 * TILE_SIZE_Y
#define RIVER_SIDE1_X 0   /* SIde peices eg - | |   */
#define RIVER_SIDE1_Y 3 * TILE_SIZE_Y
#define RIVER_SIDE2_X TILE_SIZE_X
#define RIVER_SIDE2_Y 3 * TILE_SIZE_Y
#define RIVER_ONESIDE1_X 2 * TILE_SIZE_X /* One side ground others water */
#define RIVER_ONESIDE1_Y 3 * TILE_SIZE_Y
#define RIVER_ONESIDE2_X 3 * TILE_SIZE_X
#define RIVER_ONESIDE2_Y 3 * TILE_SIZE_Y
#define RIVER_ONESIDE3_X 4 * TILE_SIZE_X
#define RIVER_ONESIDE3_Y 3 * TILE_SIZE_Y
#define RIVER_ONESIDE4_X 5 * TILE_SIZE_X
#define RIVER_ONESIDE4_Y 3 * TILE_SIZE_Y
#define RIVER_CORN1_X 6 * TILE_SIZE_X /* Four corner L shaped peices */
#define RIVER_CORN1_Y 3 * TILE_SIZE_Y
#define RIVER_CORN2_X 7 * TILE_SIZE_X
#define RIVER_CORN2_Y 3 * TILE_SIZE_Y
#define RIVER_CORN3_X 8 * TILE_SIZE_X
#define RIVER_CORN3_Y 3 * TILE_SIZE_Y
#define RIVER_CORN4_X 9 * TILE_SIZE_X
#define RIVER_CORN4_Y 3 * TILE_SIZE_Y



/* Swamp */
#define SWAMP_X 7 * TILE_SIZE_X
#define SWAMP_Y TILE_SIZE_Y

/* Shot Building */
#define SHOT_BUILDING_X 8 * TILE_SIZE_X
#define SHOT_BUILDING_Y TILE_SIZE_Y

/* Pillboxes */
#define PILL_EVIL_X 9 * TILE_SIZE_X
#define PILL_EVIL_Y TILE_SIZE_Y

/* Bases */
#define BASE_GOOD_X 10 * TILE_SIZE_X
#define BASE_GOOD_Y TILE_SIZE_Y

/*********************************************************
*NAME:          WinMain
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
*  Main Function. Creates the window and sets up 
*  message handling
*
*ARGUMENTS:
*  hInst     - Handle to the app instance
*  hInstPrev - Handle to the prious App Instance
*  szCmdLine - String pointer to the command line
*  nCmdShow  - Window State on start up
*********************************************************/
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR szCmdLine, int nCmdShow);

/*********************************************************
*NAME:          windowCreate
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
*  Sets up the main window.
*
*ARGUMENTS:
*  hInst     - Handle to the app instance
*  nCmdShow  - Window State on start up
*********************************************************/
HWND windowCreate(HINSTANCE hInst, int nCmdShow);

/*********************************************************
*NAME:          windowCallBack
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
*  The main window call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - ????
*********************************************************/
LRESULT CALLBACK ExWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*********************************************************
*NAME:          windowUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
*  Updates the Window View
*
*ARGUMENTS:
*  value  - Pointer to the sceen structure
*********************************************************/
//void Ex_OnPaint(screen *value);


/*********************************************************
*NAME:          windowSetupTiles
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
*  Checks for the map files existance and loads it
*  returns whether it was sucessful or not
*  Should be called on application startup
*
*ARGUMENTS:
*
*********************************************************/
bool windowSetupTiles();

/*********************************************************
*NAME:          windowTilesDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
* Destroys the tile DC's. 
* Should be called on application exit
*
*ARGUMENTS:
*
*********************************************************/
void windowDestroyTiles();

/*********************************************************
*NAME:          windowLoadMap
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
* Function does map loading
*
*ARGUMENTS:
*
*********************************************************/
void windowLoadMap();

/*********************************************************
*NAME:          windowShowAboutBox
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
* Creates and shows the about box
*
*ARGUMENTS:
*
*********************************************************/
void windowShowAboutBox();

/*********************************************************
*NAME:          windowAboutBoxPro
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
*  The about box call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - ????
*********************************************************/
LRESULT CALLBACK windowAboutBoxProc( HWND hwnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);


#endif

