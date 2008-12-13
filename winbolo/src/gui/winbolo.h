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
*Name:          WinBolo
*Filename:      WinBolo.h
*Author:        John Morrison
*Creation Date: 31/10/98
*Last Modified:  31/7/00
*Purpose:
*  Provides the front end for window/frontend
*********************************************************/

#ifndef _WINBOLO_H
#define _WINBOLO_H

#include <windows.h>
#include "..\bolo\global.h"
#include "..\bolo\backend.h"
#include "input.h"

/* The name of the main window Class */
#define WIND_CLASSNAME "WinBolo" 
#define WIND_TITLE "WinBolo"

#define DIALOG_BOX_TITLE "WinBolo"

#define BOLO_SOUNDS_DLL "BoloSounds.bsd"

/* Frame Rates */
/* Numbers slightly off because of timer innacuracies */
#define FRAME_RATE_10 19
#define FRAME_RATE_12 23
#define FRAME_RATE_15 28
#define FRAME_RATE_20 37
#define FRAME_RATE_30 55
#define FRAME_RATE_50 82
#define FRAME_RATE_60 111

/* Zoom Factors */
#define ZOOM_FACTOR_NORMAL 1
#define ZOOM_FACTOR_DOUBLE 2
#define ZOOM_FACTOR_QUAD 4

/* The size of the main window EXCLUDING Menus and Toolbar */
#define SCREEN_SIZE_X 515
#define SCREEN_SIZE_Y 325

#define TOTAL_WINDOW_SIZE_X (SCREEN_SIZE_X)
#define TOTAL_WINDOW_SIZE_Y (SCREEN_SIZE_Y)

/* Nothing has been selected */
#define NO_SELECT -1

/* Defines the number of milliseconds in a second */
/* Used for frame rate counting */
#define MILLISECONDS 1000


/* Open Dialog Box Stuff */
#define OPEN_FILE_FILTERS "Map Files\0*.MAP\0All Files\0*.*\0\0"
#define OPEN_FILE_TITLE "Open File...\0"
#define DEFAULT_FILE_EXTENSION "map\0"

/* Stuff to write in players menu if there are no tanks there */
#define STR_01 "1\0"
#define STR_02 "2\0"
#define STR_03 "3\0"
#define STR_04 "4\0"
#define STR_05 "5\0"
#define STR_06 "6\0"
#define STR_07 "7\0"
#define STR_08 "8\0"
#define STR_09 "9\0"
#define STR_10 "10\0"
#define STR_11 "11\0"
#define STR_12 "12\0"
#define STR_13 "13\0"
#define STR_14 "14\0"
#define STR_15 "15\0"
#define STR_16 "16\0"
#define STR_PLAYER_LEN 2

/* Window show request types */
typedef enum {
  wsrOpen, /* Window request to be open */
  wsrClose /* Window has closed */
} windowShowRequest;

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
*  lParam - More message parameters
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
*NAME:          windowGetInstance
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
* Returns the application instance
*
*ARGUMENTS:
*
*********************************************************/
HINSTANCE windowGetInstance();

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

/*********************************************************
*NAME:          windowGameTimer
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED:  9/2/99
*PURPOSE:
* The Game Timer. If there are no events to prcess this 
* routine is called. If the elapsed
*
*ARGUMENTS:
*
*********************************************************/
void CALLBACK windowGameTimer(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

/*********************************************************
*NAME:          windowKeyPressed
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/98
*LAST MODIFIED: 31/1/99
*PURPOSE:
* When a key is pressed it determines what combination
* should be passed when the game is updated
*
*ARGUMENTS:
*  keyCode - The key code of the button that was pressed
*********************************************************/
void windowKeyPressed(int keyCode);

/*********************************************************
*NAME:          windowButtonAdd
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/98
*LAST MODIFIED: 26/11/98
*PURPOSE:
* When a tank button (accelerate/decelerate/left/right)
* is pressed it updates the state of the button to pass
*
*ARGUMENTS:
*  keyCode - The key code of the button that was pressed
*********************************************************/
void windowButtonAdd(int keyCode);

/*********************************************************
*NAME:          windowButtonRemove
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/98
*LAST MODIFIED: 26/11/98
*PURPOSE:
* When a tank button (accelerate/decelerate/left/right)
* is released it updates the state of the button to pass
*
*ARGUMENTS:
*  keyCode - The key code of the button that was pressed
*********************************************************/
void windowButtonRemove(int keyCode);

/*********************************************************
*NAME:          windowSetFrameRate
*AUTHOR:        John Morrison
*CREATION DATE: 16/12/98
*LAST MODIFIED: 24/6/00
*PURPOSE:
* Function is called to set the frame rate.
*
*ARGUMENTS:
*  hWnd         - Handle to the Window
*  newFrameRate - The framerate to set it to
*  setTimer     - Do we want to set the timer or not
*********************************************************/
void windowSetFrameRate(HWND hWnd, int newFrameRate, bool setTimer);

/*********************************************************
*NAME:          windowMouseClick
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
* The mouse button is pressed. Check to see if selected
* building things are changed
*
*ARGUMENTS:
*  xWin - X Co-ord of top position of window
*  yWin - Y Co-ord of top position of window
*  xPos - X Position of the mouse (relative to the window)
*  yPos - Y Position of the mouse (relative to the window)
*********************************************************/
void windowMouseClick(int xWin, int yWin, int xPos, int yPos);

/*********************************************************
*NAME:          windowShowGunsight
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 21/12/98
*PURPOSE:
* Gunsight Menu item has been access. Check/Uncheck the 
* menu item then call the screenShowGunsight function
*
*ARGUMENTS:
*  hWnd - Window Handle that called it
*********************************************************/
void windowShowGunsight(HWND hWnd);

/*********************************************************
*NAME:          windowSoundEffects
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 28/12/98
*PURPOSE:
* Turns the sound effects on or off.
* Updates the menu
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowSoundEffects(HWND hWnd);

/*********************************************************
*NAME:          windowDisableSound
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 28/12/98
*PURPOSE:
* Disables the sound Effects Menu Entries.
*
*ARGUMENTS:
*
*********************************************************/
void windowDisableSound();

/*********************************************************
*NAME:          windowISASoundCard
*AUTHOR:        John Morrison
*CREATION DATE: 29/12/98
*LAST MODIFIED: 29/12/98
*PURPOSE:
* Switches the state of the ISA Sound Card Menu Item
*
*ARGUMENTS:
*
*********************************************************/
void windowISASoundCard();

/*********************************************************
*NAME:          windowMenuAllowNewPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 1/6/00
*LAST MODIFIED: 1/6/00
*PURPOSE:
* Bolo -> Allow New Players has been checked.
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowMenuAllowNewPlayers(HWND hWnd);

/*********************************************************
*NAME:          windowMenuNewswire
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/99
*PURPOSE:
* Bolo -> Newswire has been checked.
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowMenuNewswire(HWND hWnd);

/*********************************************************
*NAME:          windowMenuAssistant
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/99
*PURPOSE:
* Bolo -> Assistant has been checked.
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowMenuAssistant(HWND hWnd);

/*********************************************************
*NAME:          windowMenuAI
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/99
*PURPOSE:
* Bolo -> AI has been checked.
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowMenuAI(HWND hWnd);

/*********************************************************
*NAME:          windowMenuNetwork
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/99
*PURPOSE:
* Bolo -> Network has been checked.
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowMenuNetwork(HWND hWnd);

/*********************************************************
*NAME:          windowMenuNetworkDebug
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/99
*PURPOSE:
* Bolo -> Network Debug Messages has been checked.
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowMenuNetworkDebug(HWND hWnd);

/*********************************************************
*NAME:          windowShowPillLabels
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
* Toggle Show Pillbox labels
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowShowPillLabels(HWND hWnd);

/*********************************************************
*NAME:          windowShowBaseLabels
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
* Toggle Show Pillbox labels
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowShowBaseLabels(HWND hWnd);

/*********************************************************
*NAME:          windowMenuNetwork
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
* Switch the Automatic Scrolling checkmark etc.
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowAutomaticScrolling(HWND hWnd);

/*********************************************************
*NAME:          windowSetZoomFactor
*AUTHOR:        John Morrison
*CREATION DATE: 21/1/99
*LAST MODIFIED: 21/1/99
*PURPOSE:
*  Sets the windows zoom factor
*
*ARGUMENTS:
*  amount - New Zoom Factor
*********************************************************/
void windowSetZoomFactor(BYTE amount);

/*********************************************************
*NAME:          windowSetZoomFactor
*AUTHOR:        John Morrison
*CREATION DATE: 21/1/99
*LAST MODIFIED: 21/1/99
*PURPOSE:
*  The menu items have been clicked. Change the check mark
*  and then update the zoom factor
*
*ARGUMENTS:
*  amount - New Zoom Factor
*********************************************************/
void windowZoomChange(BYTE amount);

/*********************************************************
*NAME:          windowGetZoomFactor
*AUTHOR:        John Morrison
*CREATION DATE: 21/1/99
*LAST MODIFIED: 21/1/99
*PURPOSE:
*  Gets the windows zoom factor
*
*ARGUMENTS:
*
*********************************************************/
BYTE windowGetZoomFactor();

/*********************************************************
*NAME:          windowFrameRateTimer
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
* The Frame rate Timer has gone off. Update the screen.
*
*ARGUMENTS:
*
*********************************************************/
void CALLBACK windowFrameRateTimer(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

/*********************************************************
*NAME:          windowWnd
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 29/1/99
*PURPOSE:
*  Returns the window handle of the main window
*
*ARGUMENTS:
*
*********************************************************/
HWND windowWnd(void);

/*********************************************************
*NAME:          windowGetDrawTime
*AUTHOR:        John Morrison
*CREATION DATE: 30/1/99
*LAST MODIFIED: 30/1/99
*PURPOSE:
*  Returns number of miliseconds spent drawing 
*  last second
*
*ARGUMENTS:
*
*********************************************************/
int windowGetDrawTime(void);

/*********************************************************
*NAME:          windowGetNetTime
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED: 23/2/99
*PURPOSE:
*  Returns number of miliseconds spent doing network work
*  last second
*
*ARGUMENTS:
*
*********************************************************/
int windowGetNetTime(void);

/*********************************************************
*NAME:          windowGetAiTime
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Returns number of miliseconds spent doing ai work
*  last second
*
*ARGUMENTS:
*
*********************************************************/
int windowGetAiTime(void);

/*********************************************************
*NAME:          windowGetSimTime
*AUTHOR:        John Morrison
*CREATION DATE: 30/1/99
*LAST MODIFIED: 30/1/99
*PURPOSE:
*  Returns number of miliseconds spent doing sim modelling
*  last second
*
*ARGUMENTS:
*
*********************************************************/
int windowGetSimTime(void);

/*********************************************************
*NAME:          windowGetKeys
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* Gets a copy of the keys
*
*ARGUMENTS:
*  value - Pointer to hold the copy of the keys
*********************************************************/
void windowGetKeys(keyItems *value);

/*********************************************************
*NAME:          windowSetKeys
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* Sets the keys to be that held in value
*
*ARGUMENTS:
*  value - Pointer to hold the copy of the keys
*********************************************************/
void windowSetKeys(keyItems *value);

/*********************************************************
*NAME:          windowLabelOwnTank
*AUTHOR:        John Morrison
*CREATION DATE: 2/2/99
*LAST MODIFIED: 2/2/99
*PURPOSE:
* Menu Item has been checked. Change the label own tank
* selection
*
*ARGUMENTS:
*  hWnd - Window Handle
*********************************************************/
void windowLabelOwnTank(HWND hWnd);

/*********************************************************
*NAME:          windowSetMessageLabelLen
*AUTHOR:        John Morrison
*CREATION DATE: 2/2/99
*LAST MODIFIED: 2/2/99
*PURPOSE:
* Menu Item has been checked. Changed the message label
* length
*
*ARGUMENTS:
*  hWnd   - Window Handle
*  newLen - The new label length
*********************************************************/
void windowSetMessageLabelLen(HWND hWnd, labelLen newLen);

/*********************************************************
*NAME:          windowSetTankLabelLen
*AUTHOR:        John Morrison
*CREATION DATE: 2/2/99
*LAST MODIFIED: 2/2/99
*PURPOSE:
* Menu Item has been checked. Changed the tank label
* length
*
*ARGUMENTS:
*  hWnd   - Window Handle
*  newLen - The new label length
*********************************************************/
void windowSetTankLabelLen(HWND hWnd, labelLen newLen);

/*********************************************************
*NAME:          windowSaveMap
*AUTHOR:        John Morrison
*CREATION DATE: 10/2/99
*LAST MODIFIED: 10/2/99
*PURPOSE:
* Handles saving of the map (Displaying the dialog 
* box etc)
* 
*ARGUMENTS:
*
*********************************************************/
void windowSaveMap();

/*********************************************************
*NAME:          windowShowGameInfo
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/99
*LAST MODIFIED: 14/2/99
*PURPOSE:
* Handles opening (preventing multiple openings) of the
* game info dialog box
* 
*ARGUMENTS:
*  req - type of window open/close request
*********************************************************/
void windowShowGameInfo(windowShowRequest req);

/*********************************************************
*NAME:          windowShowGameInfo
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/99
*LAST MODIFIED: 14/2/99
*PURPOSE:
* Handles opening (preventing multiple openings) of the
* sys info dialog box
* 
*ARGUMENTS:
*  req - type of window open/close request
*********************************************************/
void windowShowSysInfo(windowShowRequest req);

/*********************************************************
*NAME:          windowShowSetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/99
*LAST MODIFIED: 14/2/99
*PURPOSE:
* Handles opening (preventing multiple openings) of the
* set player name dialog box
* 
*ARGUMENTS:
*  req - type of window open/close request
*********************************************************/
void windowShowSetPlayerName(windowShowRequest req);

/*********************************************************
*NAME:          windowShowSendMessages
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/99
*LAST MODIFIED: 14/2/99
*PURPOSE:
* Handles opening (preventing multiple openings) of the
* send message dialog box
* 
*ARGUMENTS:
*  req - type of window open/close request
*********************************************************/
void windowShowSendMessages(windowShowRequest req);

/*********************************************************
*NAME:          windowShowSetKeys
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/99
*LAST MODIFIED: 14/2/99
*PURPOSE:
* Handles opening (preventing multiple openings) of the
* set keys dialog box
* 
*ARGUMENTS:
*  req - type of window open/close request
*********************************************************/
void windowShowSetKeys(windowShowRequest req);

/*********************************************************
*NAME:          windowShowNetInfo
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 3/3/99
*PURPOSE:
* Handles opening (preventing multiple openings) of the
* net info dialog box
* 
*ARGUMENTS:
*  req - type of window open/close request
*********************************************************/
void windowShowNetInfo(windowShowRequest req);

/*********************************************************
*NAME:          windowRedrawAll
*AUTHOR:        John Morrison
*CREATION DATE: 24/3/99
*LAST MODIFIED: 24/3/99
*PURPOSE:
* We need to redraw all screen items. Do so here
* 
*ARGUMENTS:
*
*********************************************************/
void windowRedrawAll(void);

/*********************************************************
*NAME:          windowGetBackgroundSound
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/99
*LAST MODIFIED: 27/3/99
*PURPOSE:
* Returns whether we are using background sound or not
* 
*ARGUMENTS:
*
*********************************************************/
bool windowGetBackgroundSound(void);

/*********************************************************
*NAME:          windowBackgroundSoundChange
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/99
*LAST MODIFIED: 31/7/00
*PURPOSE:
* The Backgound Sound menu item has been toggled.
* 
*ARGUMENTS:
*  wantInit - TRUE if we want to init the sound system
*********************************************************/
void windowBackgroundSoundChange(HWND hWnd, bool wantInit);

/*********************************************************
*NAME:          windowHideMainView
*AUTHOR:        John Morrison
*CREATION DATE: 29/3/99
*LAST MODIFIED: 29/3/99
*PURPOSE:
* The hide main view menu item has been toggled.
* 
*ARGUMENTS:
*
*********************************************************/
void windowHideMainView(HWND hWnd);

/*********************************************************
*NAME:          windowGetMutex
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Returns the programs Mutex handle for thread locking
* 
*ARGUMENTS:
*
*********************************************************/
HANDLE windowGetMutex();

/*********************************************************
*NAME:          windowReCreate
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 16/12/98
*PURPOSE:
*  Sets up the main window.
*
*ARGUMENTS:
*
*********************************************************/
void windowReCreate();

/*********************************************************
*NAME:          windowApplyMenuChecks
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 16/12/98
*PURPOSE:
*  Applys our menu settings to the main window
*
*ARGUMENTS:
*
*********************************************************/
void windowApplyMenuChecks();

/*********************************************************
*NAME:          windowMenuAllowAllianceRequest
*AUTHOR:        John Morrison
*CREATION DATE: 01/09/02
*LAST MODIFIED: 01/09/02
*PURPOSE:
* Bolo -> Allow New Alliance Request has been checked.
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowMenuAllowAllianceRequest(HWND hWnd);

/*********************************************************
*NAME:          windowShowAllianceRequest
*AUTHOR:        John Morrison
*CREATION DATE: 01/09/02
*LAST MODIFIED: 01/09/02
*PURPOSE:
* Returns if we should show this alliance request or not
*
*ARGUMENTS:
*********************************************************/
bool windowShowAllianceRequest();

#endif
