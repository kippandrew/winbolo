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
*Name:          Game Front
*Filename:      gamefront.h
*Author:        John Morrison
*Creation Date: 27/01/99
*Last Modified: 24/06/02
*Purpose:
*  Provides the front end for viewing maps
*********************************************************/

#ifndef GAMEFRONT_H
#define GAMEFRONT_H

#include <windows.h>
#define DIRECTINPUT_VERSION 0x0300
#include <dinput.h>
#include "..\bolo\global.h"
#include "..\bolo\backend.h"
#include "input.h"
#include "winbolo.h"


/* Default keys if loading preferences screws up */
#define DEFAULT_FORWARD 18
#define DEFAULT_BACKWARD 32
#define DEFAULT_LEFT 31
#define DEFAULT_RIGHT 33
#define DEFAULT_SHOOT DIK_SPACE
#define DEFAULT_LAY_MINE 42

/* Values of the arrow keys, return */
#define DEFAULT_SCROLLLEFT 75
#define DEFAULT_SCROLLUP 72
#define DEFAULT_SCROLLRIGHT 77
#define DEFAULT_SCROLLDOWN 76
/* Views, dunno the first two, then letters T-G-B */
#define DEFAULT_TANKVIEW 84
#define DEFAULT_PILLVIEW 71
#define DEFAULT_ALLYVIEW 21
#define DEFAULT_LGMVIEW 35
#define DEFAULT_BASEVIEW 50
/* Quick keys, number row keys 1-5 */
#define DEFAULT_QUICKTREE 2
#define DEFAULT_QUICKROAD 3
#define DEFAULT_QUICKWALL 4
#define DEFAULT_QUICKPILLBOX 5
#define DEFAULT_QUICKMINE 6

/* + and - Keys */
#define DEFAULT_SCROLL_GUNINCREASE 78
#define DEFAULT_SCROLL_GUNDECREASE 28
#define DEFAULT_SCROLL_PILLVIEW 71

#define PREFERENCE_FILE "WinBolo.ini"

#define TRACKER_ADDRESS "tracker.winbolo.com"
#define TRACKER_PORT 50000
#define TRACKER_ENABLE FALSE

#define YESNO_TO_TRUEFALSE(X) (tolower(X) == 'y' ? 1 : 0)
#define TRUEFALSE_TO_STR(X) (X == TRUE ? "Yes" : "No")

/* Opening Dialog State machine */
typedef enum {
  openStart,
  openLang,   /* Language selection dialog */
  openSkins,   /* Skins selection dialog */
  openWelcome,
  openTutorial,
  openSetup,
  openUdp,
  openUdpJoin,
  openUdpManual,
  openUdpSetup,
  openLan,
  openLanJoin,
  openLanManual,
  openLanSetup,
  openInternet,
  openInternetJoin,
  openInternetManual,
  openInternetSetup,
  openFinished,
  openSettings
} openingStates;

/*********************************************************
*NAME:          gameFrontStart
*AUTHOR:        John Morrison
*CREATION DATE: 27/1/99
*LAST MODIFIED: 11/6/00
*PURPOSE:
*  Handles opening dialog boxes and starts up game
*  subsystems. Returns window Handle to main window
*
*ARGUMENTS:
*  hInst    - Handle to the app instance
*  cmdLine  - Command line
*  nCmdShow - Window state 
*  keys     - Structure that holds the keys
*  isLoaded - Have we loaded before?
*********************************************************/
HWND gameFrontStart(HINSTANCE hInst, char *cmdLine, int nCmdShow, keyItems *keys, bool isLoaded);

/*********************************************************
*NAME:          gameFrontEnd
*AUTHOR:        John Morrison
*CREATION DATE: 27/10/98
*LAST MODIFIED:  11/6/00
*PURPOSE:
*  Handles game shutdown.
*
*ARGUMENTS:
*  hInst      - Handle to the app instance
*  hWnd       - Handle to the main window
*  keys       - Pointer to hold Key Preferences
*  gamePlayed - TRUE if we actually entered the main 
*               screen (ie played a game)
*  isQuiting  - TRUE if we are quiting
*********************************************************/
void gameFrontEnd(HINSTANCE hInst, HWND hWnd, keyItems *keys, bool gamePlayed, bool isQuiting);

/*********************************************************
*NAME:          gameFrontRun
*AUTHOR:        John Morrison
*CREATION DATE: 27/1/99
*LAST MODIFIED:  4/2/99
*PURPOSE:
*  Handles game running.
*
*ARGUMENTS:
*  hInst    - Handle to the app instance
*  appWnd   - Window Handle
*  hAccel   - Accelerator table
*  nCmdShow - Window state
*********************************************************/
void gameFrontRun(HINSTANCE hInst, HWND appWnd, HACCEL hAccel, int nCmdShow);

/*********************************************************
*NAME:          gameFrontDialogs
*AUTHOR:        John Morrison
*CREATION DATE: 27/1/99
*LAST MODIFIED: 27/1/99
*PURPOSE:
*  Handles dialogs for setting up a game.
*
*ARGUMENTS:
*  appInst - Handle to the app instance
*********************************************************/
void gameFrontDialogs(HINSTANCE appInst);

/*********************************************************
*NAME:          gameFrontSetDlgState
*AUTHOR:        John Morrison
*CREATION DATE: 27/1/99
*LAST MODIFIED:  6/7/00
*PURPOSE:
*  Changes the dialog state machine
*
*ARGUMENTS:
*  newState - Handle to the app instance
*********************************************************/
bool gameFrontSetDlgState(openingStates newState);

/*********************************************************
*NAME:          gameFrontGetCmdArg
*AUTHOR:        John Morrison
*CREATION DATE: 28/1/99
*LAST MODIFIED: 28/1/99
*PURPOSE:
* Gets the command line argument (or last map used)
*
*ARGUMENTS:
*  getName - Holds the commandline argument
*********************************************************/
void gameFrontGetCmdArg(char *getName);

/*********************************************************
*NAME:          gameFrontSetFileName
*AUTHOR:        John Morrison
*CREATION DATE: 28/1/99
*LAST MODIFIED: 28/1/99
*PURPOSE:
* Sets the map to use. Maps have been verified as OK
*
*ARGUMENTS:
*  getName - Holds the filename
*********************************************************/
void gameFrontSetFileName(char *getName);

/*********************************************************
*NAME:          gameFrontSetGameOptions
*AUTHOR:        John Morrison
*CREATION DATE: 28/1/99
*LAST MODIFIED: 3/12/99
*PURPOSE:
* Sets the game options up.
*
*ARGUMENTS:
*  pword    - Holds the password. (Empty if none)
*  gtype    - Holds the game type.
*  hm       - Are hidden mines allowed?
*  ai       - Are computer tanks allowed etc.
*  sd       - Game start delay
*  tlimit   - Game time limit
*  justPass - TRUE if we want to just set the password
*********************************************************/
void gameFrontSetGameOptions(char *pword, gameType gt, bool hm, aiType ai, long sd, long tlimit, bool justPass);

/*********************************************************
*NAME:          gameFrontGetGameOptions
*AUTHOR:        John Morrison
*CREATION DATE: 19/4/99
*LAST MODIFIED: 19/4/99
*PURPOSE:
* Gets the game options.
*
*ARGUMENTS:
*  pword  - Holds the password. (Empty if none)
*  gtype  - Holds the game type.
*  hm     - Are hidden mines allowed?
*  ai     - Are computer tanks allowed etc.
*  sd     - Game start delay
*  tlimit - Game time limit
*********************************************************/
void gameFrontGetGameOptions(char *pword, gameType *gt, bool *hm, aiType *ai, long *sd, long *tlimit);

/*********************************************************
*NAME:          gameFrontGetUdpOptions
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Gets the UDP options
*
*ARGUMENTS:
*  pn       - Pointer to hold the player name
*  add      - Pointer to hold target machine address
*  theirUdp - Pointer to hold target machine UDP port
*  myUdp    - Pointer to this machine UDP port
*********************************************************/
void gameFrontGetUdpOptions(char *pn, char *add, unsigned short *theirUdp, unsigned short *myUdp);

/*********************************************************
*NAME:          gameFrontSetUdpOptions
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Gets the UDP options
*
*ARGUMENTS:
*  pn       - Player name
*  add      - Target machine address
*  theirUdp - Target machine UDP port
*  myUdp    - This machine UDP port
*********************************************************/
void gameFrontSetUdpOptions(char *pn, char *add, unsigned short theirUdp, unsigned short myUdp);

/*********************************************************
*NAME:          gameFrontGetPassword
*AUTHOR:        John Morrison
*CREATION DATE: 24/2/99
*LAST MODIFIED: 24/2/99
*PURPOSE:
* The network module has tried to join a game with a 
* password, request it here.
*
*ARGUMENTS:
* pword - Password slected
*********************************************************/
void gameFrontGetPassword(char *pword);

/*********************************************************
*NAME:          gameFrontGetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 24/2/99
*LAST MODIFIED: 24/2/99
*PURPOSE:
* Gets the player name
*
*ARGUMENTS:
*  pn       - Pointer to hold the player name
*********************************************************/
void gameFrontGetPlayerName(char *pn);

/*********************************************************
*NAME:          gameFrontSetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 24/2/99
*LAST MODIFIED:  2/6/00
*PURPOSE:
* Sets the player name
*
*ARGUMENTS:
*  pn       - Player name to set to
*********************************************************/
void gameFrontSetPlayerName(char *pn);

/*********************************************************
*NAME:          gameFrontGetWnd
*AUTHOR:        John Morrison
*CREATION DATE: 24/2/99
*LAST MODIFIED: 24/2/99
*PURPOSE:
* Gets the main window handle. Called by the Opening
* dialog
*
*ARGUMENTS:
*
*********************************************************/
HWND gameFrontGetWnd(void);

/*********************************************************
*NAME:          gameFrontSetAIType
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 26/2/99
*PURPOSE:
* Sets the AI type of the game. (From networking module)
*
*ARGUMENTS:
*
*********************************************************/
void gameFrontSetAIType(aiType ait);

/*********************************************************
*NAME:          gameFrontGetPrefs
*AUTHOR:        John Morrison
*CREATION DATE: 19/4/99
*LAST MODIFIED:  4/1/00
*PURPOSE:
* Gets the preferences from the preferences file. Returns
* success.
*
*ARGUMENTS:
*  keys - Pointer to keys structure
*  useAutoslow - Pointer to hold auto slowdown
*  useAutohide - Pointer to hold auto gunsight show/hide
*********************************************************/
bool gameFrontGetPrefs(keyItems *keys, bool *useAutoslow, bool *useAutohide);

/*********************************************************
*NAME:          gameFrontPutPrefs
*AUTHOR:        John Morrison
*CREATION DATE: 19/4/99
*LAST MODIFIED: 19/4/99
*PURPOSE:
* Puts the preferences to the preferences file.
*
*ARGUMENTS:
*  keys       - Pointer to keys structure
*********************************************************/
void gameFrontPutPrefs(keyItems *keys);

/*********************************************************
*NAME:          gameFrontSetRemeber
*AUTHOR:        John Morrison
*CREATION DATE: 19/4/99
*LAST MODIFIED: 19/4/99
*PURPOSE:
* Sets whether we remeber the player name or not
*
*ARGUMENTS:
*  isSet - Value to set it to
*********************************************************/
void gameFrontSetRemeber(bool isSet);

/*********************************************************
*NAME:          gameFrontGetRemeber
*AUTHOR:        John Morrison
*CREATION DATE: 19/4/99
*LAST MODIFIED: 19/4/99
*PURPOSE:
* Gets whether we remeber the player name or not
*
*ARGUMENTS:
*
*********************************************************/
bool gameFrontGetRemeber();

/*********************************************************
*NAME:          gameFrontSetupServer
*AUTHOR:        John Morrison
*CREATION DATE: 3/11/99
*LAST MODIFIED: 3/11/99
*PURPOSE:
* Attempts to start the server process. Returns success
*
*ARGUMENTS:
*
*********************************************************/
bool gameFrontSetupServer();

/*********************************************************
*NAME:          gameFrontGetTrackerOptions
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
* Gets the tracker options
*
*ARGUMENTS:
*  address - Buffer to hold address
*  port    - Pointer to hold port
*  enabled - Pointer to hold enabled flag
*********************************************************/
void gameFrontGetTrackerOptions(char *address, unsigned short *port, bool *enabled);

/*********************************************************
*NAME:          gameFrontSetTrackerOptions
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
* Sets the tracker options
*
*ARGUMENTS:
*  address - New tracker address
*  port    - New tracker port
*  enabled - New tracker enabled flag
*********************************************************/
void gameFrontSetTrackerOptions(char *address, unsigned short port, bool enabled);

/*********************************************************
*NAME:          gameFrontEnableRejoin
*AUTHOR:        John Morrison
*CREATION DATE: 22/6/00
*LAST MODIFIED: 22/6/00
*PURPOSE:
* Sets it so we want rejoin
*
*ARGUMENTS:
*
*********************************************************/
void gameFrontEnableRejoin();

/*********************************************************
*NAME:          gameFrontPreferencesExist
*AUTHOR:        John Morrison
*CREATION DATE: 13/12/99
*LAST MODIFIED: 13/12/99
*PURPOSE:
* Returns whether the preferences file exists or not.
*
*ARGUMENTS:
*
*********************************************************/
bool gameFrontPreferencesExist();

/*********************************************************
*NAME:          gameFrontLoadInBuiltMap
*AUTHOR:        John Morrison
*CREATION DATE: 1/5/00
*LAST MODIFIED: 1/5/00
*PURPOSE:
* Attempts to load the built in map by loading the 
* compress resource. Returns Success
*
*ARGUMENTS:
*
*********************************************************/
bool gameFrontLoadInBuiltMap();

/*********************************************************
*NAME:          gameFrontLoadTutorial
*AUTHOR:        John Morrison
*CREATION DATE: 1/5/00
*LAST MODIFIED: 1/5/00
*PURPOSE:
* Attempts to load the built in tutorial by loading the 
* compress resource. Returns Success
*
*ARGUMENTS:
*
*********************************************************/
bool gameFrontLoadTutorial();

/*********************************************************
*NAME:          gameFrontSetWinbolonetSettings
*AUTHOR:        John Morrison
*CREATION DATE: 24/06/00
*LAST MODIFIED: 24/06/00
*PURPOSE:
* Sets the winbolo.net settings from the setup dialog
*
*ARGUMENTS:
* password - Password
* useWbn   - Should we participate in winbolo.net
* savePass - Should we save the password
*********************************************************/
void gameFrontSetWinbolonetSettings(char *password, bool useWbn, bool savePass);

/*********************************************************
*NAME:          gameFrontGetWinbolonetSettings
*AUTHOR:        John Morrison
*CREATION DATE: 24/06/00
*LAST MODIFIED: 24/06/00
*PURPOSE:
* Gets the winbolo.net settings for the setup dialog
*
*ARGUMENTS:
* password - Destination Password
* useWbn - Destination  Should we participate in wbn
* savePass - Destination for should save password
*********************************************************/
void gameFrontGetWinbolonetSettings(char *password, bool *useWbn, bool *savePass);

/*********************************************************
*NAME:          gameFrontSetRegistryKeys
*AUTHOR:        John Morrison
*CREATION DATE: 30/08/02
*LAST MODIFIED: 30/08/02
*PURPOSE:
* Sets the winbolo registry keys for winbolo:// game
* opening
*
*ARGUMENTS:
* password - Password
* useWbn - Should we participate in winbolo.net
*********************************************************/
void gameFrontSetRegistryKeys();

/*********************************************************
*NAME:          gameFrontSetAddressFromWebLink
*AUTHOR:        John Morrison
*CREATION DATE: 30/08/02
*LAST MODIFIED: 30/08/02
*PURPOSE:
* Sets the winbolo connect IP and port from a winbolo://
* URL link
*
*ARGUMENTS:
* address - Address passed to winbolo
*********************************************************/
void gameFrontSetAddressFromWebLink(char *address);

/*********************************************************
*NAME:          gameFrontWinBoloNetClient
*AUTHOR:        John Morrison
*CREATION DATE: 30/08/02
*LAST MODIFIED: 30/08/02
*PURPOSE:
* Enables/Disabled the "Change Player Name" menu item if
* we are a winbolo.net participant
*
*ARGUMENTS:
* appWnd - Window handle containing the menu
* forceShow - if TRUE we should enable the menu
*********************************************************/
void gameFrontWinBoloNetClient(HWND appWnd, bool forceShow);

/*********************************************************
*NAME:          gameFrontReloadSkins
*AUTHOR:        John Morrison
*CREATION DATE: 28/09/03
*LAST MODIFIED: 28/09/03
*PURPOSE:
* Restarts draw and sound subsystems to reload a new skin
*
*ARGUMENTS:
*
*********************************************************/
void gameFrontReloadSkins();

/*********************************************************
*NAME:          gameFrontShutdownServer
*AUTHOR:        John Morrison
*CREATION DATE: 11/12/03
*LAST MODIFIED: 11/12/03
*PURPOSE:
* Stops the server if we are hosting
*
*ARGUMENTS:
*
*********************************************************/
void gameFrontShutdownServer();

#endif
