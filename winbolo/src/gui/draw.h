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
*Name:          Draw
*Filename:      draw.h
*Author:        John Morrison
*Creation Date: 13/12/98
*Last Modified: 26/01/02
*Purpose:
*  System Specific Drawing routines (Uses Direct Draw)
*********************************************************/

#ifndef _DRAW_H
#define _DRAW_H

#define STRICT 1

#include <windows.h>
#include "..\bolo\global.h"
#include "..\bolo\backend.h"

/* Size of the message string on the screen */
#define MESSAGE_STRING_SIZE 68

/* Length of the kills/Deaths string */
#define STRING_SIZE 3

/* Skin settings */
#define DRAW_SKINS_DIRECTORY "skins"
#define DRAW_SKINS_BGFILE "screen.bmp"
#define DRAW_SKINS_TILESFILE "skin.bmp"
#define DRAW_SKINS_TILESFILE32 "skin32.bmp"

/*********************************************************
*NAME:          drawSetup
*AUTHOR:        John Morrison
*CREATION DATE: 13/10/98
*LAST MODIFIED: 13/10/98
*PURPOSE:
*  Sets up drawing systems, direct draw structures etc.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
* appInst - Handle to the application (Required to 
*           load bitmaps from resources)
* appWnd  - Main Window Handle (Required for clipper)
*********************************************************/
bool drawSetup(HINSTANCE appInst, HWND appWnd);

/*********************************************************
*NAME:          drawCleanup
*AUTHOR:        John Morrison
*CREATION DATE: 13/10/98
*LAST MODIFIED: 13/10/98
*PURPOSE:
*  Destroys and cleans up drawing systems, direct draw 
*  structures etc.
*
*ARGUMENTS:
*
*********************************************************/
void drawCleanup(void);

/*********************************************************
*NAME:          drawMainScreen
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 27/05/00
*PURPOSE:
*  Updates the Main Window View
*
*ARGUMENTS:
*  value    - Pointer to the sceen structure
*  mineView - Pointer to the screen mines structure
*  tks      - Pointer to the screen tank structure
*  gs       - Pointer to the screen gunsight structure
*  sBullets - The screen Bullets structure
*  lgms     - Screen Builder structure 
*  rcWindow - Window region
*  showPillLabels - Show the pillbox labels?
*  showBaseLabels - Show the base labels?
*  srtDelay       - The start delay in ticks.
*                  If greater then 0 should draw countdown
*  isPillView     - TRUE if we are in pillbox view
*  edgeX          - Edge X offset for smooth scrolling
*  edgeY          - Edge Y offset for smooth scrolling
*  useCursor      - True if to draw the cursor
*  cursorLeft     - Cursor left position
*  cursorTop      - Cursor Top position
*  tank           - Pointer to the player's tank structure
*********************************************************/
void drawMainScreen(screen *value, screenMines *mineView, screenTanks *tks, screenGunsight *gs, screenBullets *sBullets, screenLgm *lgms, RECT *rcWindow, bool showPillLabels, bool showBaseLabels, long srtDelay, bool isPillView, int edgeX, int edgeY, bool useCursor, BYTE cursorLeft, BYTE cursorTop, tank *tank);

/*********************************************************
*NAME:          drawGetFrameRate
*AUTHOR:        John Morrison
*CREATION DATE: 16/12/98
*LAST MODIFIED: 16/12/98
*PURPOSE:
*  Returns the frame rate being achieved
*
*ARGUMENTS:
*
*********************************************************/
int drawGetFrameRate(void);

/*********************************************************
*NAME:          drawBackground
*AUTHOR:        John Morrison
*CREATION DATE: 20/12/98
*LAST MODIFIED: 20/12/98
*PURPOSE:
*  Draws the background graphic. Returns if the operation
*  is successful or not.
*
*ARGUMENTS:
*  appInst - The application instance
*  appWnd  - The application Window 
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
bool drawBackground(HINSTANCE appInst, HWND appWnd, int xValue, int yValue);

/*********************************************************
*NAME:          drawSelectIndentsOn
*AUTHOR:        John Morrison
*CREATION DATE: 20/12/98
*LAST MODIFIED: 20/12/98
*PURPOSE:
*  Draws the indents around the five building selection 
*  graphics on the left based on the buildSelect value.
*  Draws the red dot as well.
*
*ARGUMENTS:
*  value   - The currently selected build icon
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawSelectIndentsOn(buildSelect value, int xValue, int yValue);

/*********************************************************
*NAME:          drawSelectIndentsOff
*AUTHOR:        John Morrison
*CREATION DATE: 20/12/98
*LAST MODIFIED: 20/12/98
*PURPOSE:
*  Draws the indents around the five building selection 
*  graphics off the left based on the buildSelect value.
*  Draws the red dot as well.
*
*ARGUMENTS:
*  value   - The currently selected build icon
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawSelectIndentsOff(buildSelect value, int xValue, int yValue);

/*********************************************************
*NAME:          drawRedrawAll
*AUTHOR:        John Morrison
*CREATION DATE: 20/12/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Redraws everything (except the main view)
*
*ARGUMENTS:
*  appInst         - Application Instance
*  appWnd          - Application Window
*  value           - The currently selected build icon
*  rcWindow        - The window co-ords
*  showPillsStatus - Should the the pill status be shown
*  showBasesStatus - Should the the base status be shown
*********************************************************/
void drawRedrawAll(HINSTANCE appInst, HWND appWnd, buildSelect value, RECT *rcWindow, bool showPillsStatus, bool showBasesStatus);

/*********************************************************
*NAME:          drawStatusBase
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
*  Draws the base status for a particular base
*
*ARGUMENTS:
*  baseNum - The base number to draw (1-16)
*  ba      - The allience of the base
*  labels  - Should the label be shown
*********************************************************/
void drawStatusBase(BYTE baseNum, baseAlliance ba, bool labels);

/*********************************************************
*NAME:          drawStatusPillbox
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 23/1/99
*PURPOSE:
*  Draws the pillbox status for a particular pillbox
*
*ARGUMENTS:
*  pillNum - The base number to draw (1-16)
*  pa      - The allience of the pillbox
*  labels  - Should labels be drawn?
*********************************************************/
void drawStatusPillbox(BYTE pillNum, pillAlliance pb, bool labels);

/*********************************************************
*NAME:          drawStatusTank
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/99
*LAST MODIFIED: 14/2/99
*PURPOSE:
*  Draws the tank status for a particular tank
*
*ARGUMENTS:
*  tankNum - The tank number to draw (1-16)
*  ta      - The allience of the pillbox
*********************************************************/
void drawStatusTank(BYTE tankNum, tankAlliance ta);

/*********************************************************
*NAME:          drawStatusTankBars
*AUTHOR:        John Morrison
*CREATION DATE: 22/12/98
*LAST MODIFIED: 22/12/98
*PURPOSE:
*  Draws the tanks armour, shells etc bars.
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  shells  - Number of shells
*  mines   - Number of mines
*  armour  - Amount of armour
*  trees   - Amount of trees
*********************************************************/
void drawStatusTankBars(int xValue, int yValue, BYTE shells, BYTE mines, BYTE armour, BYTE trees);

/*********************************************************
*NAME:          drawShells
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Draws shells and explosions on the backbuffer.
*
*ARGUMENTS:
*  sBullets - The screen Bullets data structure 
*********************************************************/
void drawShells(screenBullets *sBullets);

/*********************************************************
*NAME:          drawMessages
*AUTHOR:        John Morrison
*CREATION DATE: 3/1/99
*LAST MODIFIED: 3/1/99
*PURPOSE:
*  Draws the message window
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  top    - The top string to draw
*  bottom - The bottom string to draw
*********************************************************/
void drawMessages(int xValue, int yValue, char *top, char *bottom);

/*********************************************************
*NAME:          drawTanks
*AUTHOR:        John Morrison
*CREATION DATE: 6/1/99
*LAST MODIFIED: 2/2/99
*PURPOSE:
*  Draws tanks on the backbuffer.
*
*ARGUMENTS:
*  tks - The screen Tanks data structure 
*********************************************************/
void drawTanks(screenTanks *tks);

/*********************************************************
*NAME:          drawKillsDeaths
*AUTHOR:        John Morrison
*CREATION DATE:  8/1/99
*LAST MODIFIED:  8/1/99
*PURPOSE:
*  Draws the tanks kills/deaths
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  kills  - The number of kills the tank has.
*  deaths - The number of times the tank has died
*********************************************************/
void drawKillsDeaths(int xValue, int yValue, int kills, int deaths);

/*********************************************************
*NAME:          drawStatusBaseBars
*AUTHOR:        John Morrison
*CREATION DATE: 11/1/98
*LAST MODIFIED: 11/1/98
*PURPOSE:
*  Draws the base armour, shells etc bars.
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  shells  - Number of shells
*  mines   - Number of mines
*  armour  - Amount of armour
*  redraw  - If set to true use the redraw last amounts
*********************************************************/
void drawStatusBaseBars(int xValue, int yValue, BYTE shells, BYTE mines, BYTE armour, bool redraw);

/*********************************************************
*NAME:          drawTanks
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 17/1/99
*PURPOSE:
*  Draws the builder
*
*ARGUMENTS:
*  lgms - The screenLgm data structure 
*********************************************************/
void drawLGMs(screenLgm *lgms);

/*********************************************************
*NAME:          drawCopyManStatus
*AUTHOR:        John Morrison
*CREATION DATE: 11/1/98
*LAST MODIFIED: 11/1/98
*PURPOSE:
*  Copys the LGM status on to the primary buffer
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawCopyManStatus(int xValue, int yValue);

/*********************************************************
*NAME:          drawSetManStatus
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/98
*LAST MODIFIED: 18/1/98
*PURPOSE:
*  Draws the man arrow status. If isDead is set to true
*  then the angle is ignored.
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  isDead - Is the man dead
*  angle  - The angle the item is facing
*********************************************************/
void drawSetManStatus(int xValue, int yValue, bool isDead, TURNTYPE angle);

/*********************************************************
*NAME:          drawSetManClear
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/98
*LAST MODIFIED: 18/1/98
*PURPOSE:
*  Clears the lgm status display.
*
*ARGUMENTS:
*********************************************************/
void drawSetManClear(void);

/*********************************************************
*NAME:          drawSetBasesStatusClear
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Clears the bases status display.
*
*ARGUMENTS:
*********************************************************/
void drawSetBasesStatusClear(void);

/*********************************************************
*NAME:          drawSetPillsStatusClear
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Clears the Pills status display.
*
*ARGUMENTS:
*********************************************************/
void drawSetPillsStatusClear(void);

/*********************************************************
*NAME:          drawSetTanksStatusClear
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/98
*LAST MODIFIED: 14/2/98
*PURPOSE:
*  Clears the tanks status display.
*
*ARGUMENTS:
*********************************************************/
void drawSetTanksStatusClear(void);

/*********************************************************
*NAME:          drawCopyBasesStatus
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Copys the Bases status on to the primary buffer
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawCopyBasesStatus(int xValue, int yValue);

/*********************************************************
*NAME:          drawCopyBasesStatusBars
*AUTHOR:        Minhiriath
*CREATION DATE: 08/2/2009
*LAST MODIFIED: 08/2/2009
*PURPOSE:
*  Copys the Bases status bars on to the primary buffer
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawCopyBasesStatusBars(int xValue, int yValue);

/*********************************************************
*NAME:          drawCopyTankStatusBars
*AUTHOR:        Minhiriath
*CREATION DATE: 08/2/2009
*LAST MODIFIED: 08/2/2009
*PURPOSE:
*  Copys the tank status bars on to the primary buffer
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawCopyTankStatusBars(int xValue, int yValue);

/*********************************************************
*NAME:          drawCopyPillsStatus
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Copys the pills status on to the primary buffer
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawCopyPillsStatus(int xValue, int yValue);

/*********************************************************
*NAME:          drawCopyTanksStatus
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Copys the pills status on to the primary buffer
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawCopyTanksStatus(int xValue, int yValue);

/*********************************************************
*NAME:          drawStartDelay
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/98
*LAST MODIFIED: 29/1/98
*PURPOSE:
*  The start delay is still counting down. Draw it here.
*
*ARGUMENTS:
*  rcWindow - Window rectangle
*  srtDelay - The start delay
*********************************************************/
void drawStartDelay(RECT *rcWindow, long srtDelay);

/*********************************************************
*NAME:          drawTankLabel
*AUTHOR:        John Morrison
*CREATION DATE: 02/02/98
*LAST MODIFIED: 26/01/02
*PURPOSE:
*  Draws the tank label if required.
*
*ARGUMENTS:
*  str       - The string identifer of the tank
*  playerNum - Player number of the tank
*  mx        - Tank map X position
*  my        - Tank map Y position
*  px        - Tank pixel offset
*  py        - Tank pixel offset
*********************************************************/
void drawTankLabel(char *str, BYTE playerNum, int mx, int my, BYTE px, BYTE py);

/*********************************************************
*NAME:          drawPillInView
*AUTHOR:        John Morrison
*CREATION DATE: 3/2/98
*LAST MODIFIED: 3/2/98
*PURPOSE:
*  Draws the "Pillbox View" label
*
*ARGUMENTS:
*
*********************************************************/
void drawPillInView();

/*********************************************************
*NAME:          drawRestore
*AUTHOR:        John Morrison
*CREATION DATE: 23/3/98
*LAST MODIFIED: 23/3/98
*PURPOSE:
*  Restores all the DD objects and reloads graphics into
*  them
*
*ARGUMENTS:
*
*********************************************************/
void drawRestore(void);

/*********************************************************
*NAME:          drawDownloadScreen
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/98
*LAST MODIFIED: 13/12/98
*PURPOSE:
*  Draws the download line on the screen
*
*ARGUMENTS:
*  rcWindow  - Window Co-ordinates
*  justBlack - TRUE if we want a black screen
*********************************************************/
void drawDownloadScreen(RECT *rcWindow, bool justBlack);

/*********************************************************
*NAME:          drawMainScreenBlack
*AUTHOR:        John Morrison
*CREATION DATE: 29/3/98
*LAST MODIFIED: 29/3/98
*PURPOSE:
*  Draws the main screen black
*
*ARGUMENTS:
*  rcWindow - Window Co-ordinates
*********************************************************/
void drawMainScreenBlack(RECT *rcWindow);

/*********************************************************
*NAME:          drawSetupArrays
*AUTHOR:        John Morrison
*CREATION DATE: 28/5/00
*LAST MODIFIED: 28/5/00
*PURPOSE:
*  Sets up the drawing arrays to improve efficeny and
*  remove the giant switch statement every drawing loop
*
*ARGUMENTS:
*  zoomFactor - The scaling factor
*********************************************************/
void drawSetupArrays(BYTE zoomFactor);

#endif
