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
*Filename:      gamefront.c
*Author:        John Morrison
*Creation Date: 27/01/99
*Last Modified: 21/04/06
*Purpose:
*  Provides the front end for dialog/preferences etc.
*********************************************************/

#include <windows.h>
#include <winbase.h>
#define DIRECTINPUT_VERSION 0x0300
#include <dinput.h>
#include <string.h>
#include <stdlib.h>
#include <commdlg.h>
#include <ctype.h>
#include <time.h>
#include "resource.h"
#include "..\..\bolo\backend.h"
#include "..\..\bolo\global.h"
#include "..\..\bolo\network.h"
#include "..\..\bolo\players.h"
#include "..\..\server\servercore.h"
#include "..\..\server\servernet.h"
#include "..\..\winbolonet\winbolonet.h"
#include "..\brainsHandler.h"
#include "..\clientmutex.h"
#include "..\cursor.h"
#include "..\dialoggamefinder.h"
#include "..\dialoggamesetup.h"
#include "..\dialogLanguages.h"
#include "..\dialogkeysetup.h"
#include "..\dialogopening.h"
#include "..\dialogpassword.h"
#include "..\dialogsetname.h"
#include "..\dialogskins.h"
#include "..\dialogudpsetup.h"
#include "..\draw.h"
#include "..\font.h"
#include "..\gamefront.h"
#include "..\input.h"
#include "..\lang.h"
#include "..\skins.h"
#include "..\sound.h"
#include "..\winbolo.h"
#include "..\winutil.h"



/* Game playing options */
char fileName[FILENAME_MAX]; /* filename and path of the map to use or command line arguement */
char password[MAP_STR_SIZE]; /* game password */
bool hiddenMines;            /* Hidden mines allowed */
aiType compTanks = aiNone;   /* Whether computer tanks are allowed */
gameType gametype;           /* The type of game being played */
long startDelay;             /* The start delay */
long timeLen;                /* Game time length */
bool gameFrontRemeber;       /* Remember player name? */

/* UDP stuff */
char gameFrontName[PLAYER_NAME_LEN]; /* Player Name */
char gameFrontUdpAddress[FILENAME_MAX]; /* IP of target machine */
unsigned short gameFrontMyUdp;
unsigned short gameFrontTargetUdp;

char gameFrontTrackerAddr[FILENAME_MAX];
unsigned short gameFrontTrackerPort;
bool gameFrontTrackerEnabled;

/* Window handle */
HWND gameFrontWnd;

/* Winbolo.net settings */
char gameFrontWbnPass[FILENAME_MAX];
bool gameFrontWbnUse;
bool gameFrontWbnSavePass;

/* Dialog states */
openingStates dlgState = openStart;

bool isServer = FALSE; /* Are we server of a net game */
DWORD oldTick;     /* Number of ticks passed - Server variable */
DWORD serverOldTick;
DWORD serverTick;

unsigned int serverTimerGameID = 5; /* Server timer ID */
/* Game Tick */
#define SERVER_TICK_LENGTH (GAME_TICK_LENGTH*2)

bool useAutoslow; /* Other key preferences options */
bool useAutohide;

bool wantRejoin; /* Do we want to rejoin */

time_t ticks; /* Server tick counter for cheat prevention */

/* Server process stuff */
PROCESS_INFORMATION pi;     /* Process information    */

char gameFrontSkinName[512]; /* Current Skin Name */

extern HWND msgWnd;
extern HWND keyWnd;
extern HWND sysWnd;
extern HWND netWnd;
extern HWND infoWnd;

extern bool isTutorial;

/* Used to set the preferences. I'm to tired to do this properly */
extern int frameRate;
extern bool showGunsight;
extern bool soundEffects;
extern bool backgroundSound;
extern bool isISASoundCard;
extern bool showNewswireMessages;
extern bool showAssistantMessages;
extern bool showAIMessages;
extern bool showNetworkStatusMessages;
extern bool showNetworkDebugMessages;
extern bool autoScrollingEnabled;
extern BYTE zoomFactor;
extern bool showPillLabels;
extern bool showBaseLabels;
extern bool labelSelf;
extern labelLen labelMsg;
extern labelLen labelTank;


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
HWND gameFrontStart(HINSTANCE hInst, char *cmdLine, int nCmdShow, keyItems *keys, bool isLoaded) {
  HWND appWnd;      /* Window created and returned */
  int length;       /* Length of command line */
  bool OKStart;     /* Is the program OK to start? */


  /* Set Registry Keys */
  gameFrontSetRegistryKeys();

  isTutorial = FALSE;
  password[0] = '\0';
  gameFrontName[0] = '\0';
  gameFrontUdpAddress[0] = '\0';
  wantRejoin = FALSE;
  langSetup();

  dialogSetNameInGame(FALSE);

  /* Read preferences */
  gameFrontGetPrefs(keys, &useAutoslow, &useAutohide);


  /* Get the current skin */
  skinsSetup();
  skinsLoadSkin(gameFrontSkinName);


  /* Process the command line argument - Remove "" from around it */
  length = (int) strlen(cmdLine);
  if (length > 0 ) {
    if (cmdLine[0] == '\"') {
      cmdLine[length-1] = '\0';
      cmdLine++;
    }
  }
  strcpy(fileName, cmdLine);

  /* Initalise game subsystems */
  OKStart = TRUE;
  if (isLoaded == FALSE) {
    appWnd = windowCreate(hInst, nCmdShow);
    if (appWnd == NULL) {
      MessageBoxA(NULL, langGetText(STR_GAMEFRONTERR_WINDOW), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    } else {
      gameFrontWinBoloNetClient(appWnd, TRUE);
    }
  
    if ((drawSetup(hInst, appWnd)) == FALSE) {
      MessageBoxA(NULL, langGetText(STR_GAMEFRONTERR_DDRAW), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }

    if ((soundSetup(hInst, appWnd)) == FALSE) {
      MessageBoxA(NULL, langGetText(STR_GAMEFRONTERR_DSOUND), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }

    if ((inputSetup(hInst, appWnd)) == FALSE) {
      MessageBoxA(NULL, langGetText(STR_GAMEFRONTERR_DINPUT), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }

    if ((cursorSetup(hInst, appWnd)) == FALSE) {
      MessageBoxA(NULL, langGetText(STR_GAMEFRONTERR_CURSOR), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }

    if ((fontSetup(hInst, appWnd)) == FALSE) {
      MessageBoxA(NULL, langGetText(STR_GAMEFRONTERR_FONTS), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }
  }

  /* Quit Program if start up failed */
  if (OKStart == FALSE) {
    gameFrontEnd(hInst, appWnd, keys, FALSE, TRUE);
    exit(0);
  }

  gameFrontWnd = appWnd;


  dlgState = openStart;
  if (strncmp(fileName, "winbolo://", strlen("winbolo://")) == 0) {
    if (strcmp(fileName, "winbolo:///") != 0) {
      /* We clicked on a link */
      gameFrontSetAddressFromWebLink(fileName);  
      dlgState = openInternetManual;
    }
    fileName[0] = EMPTY_CHAR;
  }

  /* Setup the player's previous name */
  if (gameFrontRemeber) {
    playersSetMyLastPlayerName(gameFrontName);
  }
  

  
  gameFrontDialogs(hInst); /* Process starting dialogs */

  screenSetAiType(compTanks);
  /* Set the key auto preferences up in the backend */

  if (isTutorial == FALSE) {
    clientMutexWaitFor();
    screenSetTankAutoSlowdown(useAutoslow);
    screenSetTankAutoHideGunsight(useAutohide);
    clientMutexRelease();
  }
  dialogSetNameInGame(TRUE);
  return appWnd;
}

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
void gameFrontEnd(HINSTANCE hInst, HWND hWnd, keyItems *keys, bool gamePlayed, bool isQuiting) { 
  clientMutexWaitFor();
  ShowWindow(hWnd, FALSE);
  if (gamePlayed == TRUE) {
    useAutoslow = screenGetTankAutoSlowdown();
    useAutohide = screenGetTankAutoHideGunsight();
  }
  brainsHandlerShutdown(hWnd);
  screenDestroy();
  netDestroy();
  gameFrontPutPrefs(keys);
  if (isQuiting == TRUE) {
    drawCleanup();
    soundCleanup();
    inputCleanup();
    cursorCleanup(hInst);
    fontCleanup();
    DestroyWindow(hWnd);
    UnregisterClassA(WIND_CLASSNAME, hInst);
    UnregisterClassA(WIND_KEYCLASSNAME, hInst);
    langCleanup();
  }
  if (isServer == TRUE) {
    /* Quit server */
    gameFrontShutdownServer();
    isServer = FALSE;
  }
  clientMutexRelease();
  skinsDestroy();
  threadsDestroy();
}

/*********************************************************
*NAME:          gameFrontRun
*AUTHOR:        John Morrison
*CREATION DATE:  27/1/99
*LAST MODIFIED: 20/11/99
*PURPOSE:
*  Handles game running.
*
*ARGUMENTS:
*  hInst    - Handle to the app instance
*  appWnd   - Window Handle
*  hAccel   - Accelerator table
*  nCmdShow - Window state
*********************************************************/
void gameFrontRun(HINSTANCE hInst, HWND appWnd, HACCEL hAccel, int nCmdShow) {
  MSG msg;        /* Windows Messages */
  bool done;      /* Program finished? */
  bool process;   /* Whether to process the message or not */


  /* Uncheck the allow player name box if we are a winbolo.net participant */
  gameFrontWinBoloNetClient(appWnd, FALSE);
  threadsCreate(FALSE); /* Start threads even if they are started */


  done = FALSE; 
  msg.wParam = 0; /* In case something goes horribly wrong */
  ShowWindow(appWnd, nCmdShow);
  /* Start up messaging */
  while (done == FALSE) {
    if (GetMessage(&msg, NULL, 0, 0) == 0) {
    /* If it's a quit message, we're outta here */
       done = TRUE;
    } else {
      process = TRUE;
      /* Check dialogs */
      if (msgWnd != NULL) {
        if (IsDialogMessage(msgWnd, &msg) != 0) {
          process = FALSE;
        }
      }
      if (keyWnd != NULL) {
        if (IsDialogMessage(keyWnd, &msg) != 0) {
          process = FALSE;
        }
      }
      if (sysWnd != NULL) {
        if (IsDialogMessage(sysWnd, &msg) != 0) {
          process = FALSE;
        }
      }
      if (infoWnd != NULL) {
        if (IsDialogMessage(infoWnd, &msg) != 0) {
          process = FALSE;
        }
      }
      if (netWnd != NULL) {
        if (IsDialogMessage(netWnd, &msg) != 0) {
          process = FALSE;
        }
      }
      /* Process main window */
      if (process == TRUE) {
        if (!TranslateAccelerator (appWnd, hAccel, &msg)) {
          TranslateMessage (&msg);
          DispatchMessage (&msg);
        }
      }
    }     
  }

  ShowWindow(appWnd, SW_HIDE);
  if (msgWnd != NULL) {
    DestroyWindow(msgWnd);
  }
  if (keyWnd != NULL) {
    DestroyWindow(keyWnd);
  }
  if (sysWnd != NULL) {
    DestroyWindow(sysWnd);
  }
  if (infoWnd != NULL) {
    DestroyWindow(infoWnd);
  }
  if (netWnd != NULL) {
    DestroyWindow(netWnd);
  }
  
  screenLeaveGame();
}

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
void gameFrontDialogs(HINSTANCE appInst) {
  bool done; /* Are we finished with the dialogs? */


  done = FALSE;
  while (done == FALSE) {
    switch (dlgState) {
    case openStart:
      dlgState = openWelcome;
      break;
    case openWelcome:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_OPENING), NULL, (DLGPROC) dialogOpeningCallback);
      break;
    case openLang:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_LANGUAGES), NULL, (DLGPROC) dialogLanguagesCallback);
      break;
    case openSkins:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_SKINS), NULL, (DLGPROC) dialogSkinsCallback);
      break;
    case openUdp:
    case openInternetManual:
    case openLanManual:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_TCPIP_SETUP), NULL, (DLGPROC) dialogUdpSetupCallback);
      break;
    case openSetup:
    case openInternetSetup:
    case openLanSetup:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_GAME_SETUP), NULL, (DLGPROC) dialogGameSetupCallback);
      break;
    case openUdpSetup:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_GAME_SETUP), NULL, (DLGPROC) dialogGameSetupCallback);
      break;
    case openInternet:
      /* Game Finder */
      dialogGameFinderSetMethod(langGetText(STR_GAMEFRONT_TRACKERFINDER_TITLE), TRUE);
      DialogBox(appInst, MAKEINTRESOURCE(IDD_GAMEFINDER), NULL, (DLGPROC) dialogGameFinderCallback);
      break;
    case openLan:
      /* Lan Game Finder */
      dialogGameFinderSetMethod(langGetText(STR_GAMEFRONT_LANFINDER_TITLE), FALSE);
      DialogBox(appInst, MAKEINTRESOURCE(IDD_GAMEFINDER), NULL, (DLGPROC) dialogGameFinderCallback);
      break;
    case openTutorial:
      /* Load Tutorial */
      gameFrontLoadTutorial(); 
      screenNetSetupTankGo();
      isTutorial = TRUE;
      dlgState = openFinished; 
      break;
    case openFinished:
      done = TRUE;
      break;
    default:
      break;
    }
  }
}
  
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
bool gameFrontSetDlgState(openingStates newState) {
  bool returnValue; /* Value to return */

  returnValue = TRUE;

  if ((dlgState == openInternet || dlgState == openLan || dlgState == openUdp || dlgState == openLanManual || dlgState == openInternetManual) && newState == openUdpJoin) {
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    screenSetup(0, FALSE, 0, UNLIMITED_GAME_TIME);
    if (netSetup(netUdp, gameFrontMyUdp, gameFrontUdpAddress, gameFrontTargetUdp, password, FALSE, gameFrontTrackerAddr, gameFrontTrackerPort, gameFrontTrackerEnabled, wantRejoin, gameFrontWbnUse, gameFrontWbnPass) == FALSE) {
      wantRejoin = FALSE;
      MessageBoxA(NULL, langGetText(STR_GAMEFRONTERR_JOINGAME), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
      netDestroy();
      screenDestroy();
      gameFrontShutdownServer();
      returnValue = FALSE;
    } else {
      dlgState = openFinished;
    }
    SetCursor(LoadCursor(NULL, IDC_ARROW));
  } else if (( dlgState == openInternetManual || dlgState == openInternetSetup) && newState == openWelcome) {
    dlgState = openInternet;
  } else if ((dlgState == openLanManual || dlgState == openLanSetup) && newState == openWelcome) {
    dlgState = openLan;
  } else if ((dlgState == openUdpSetup || dlgState == openInternetSetup || dlgState == openLanSetup) && newState == openFinished) {
    /* Start network game */

    /* Starting servers from vista is currently broken. Don't allow it */
    if (winUtilDetectVista() == TRUE) {
      MessageBox(NULL, "Starting game servers from within the WinBolo client on Vista (or later) does not work correctly and has been disabled.\n\nIf you wish to start a new game server please use the dedicated server. See the WinBolo manual for details.", DIALOG_BOX_TITLE, MB_ICONINFORMATION);
    } else {
      dlgState = newState;
      SetCursor(LoadCursor(NULL, IDC_WAIT));
      if (gameFrontSetupServer() == TRUE) {
        screenSetup(0, FALSE, 0, UNLIMITED_GAME_TIME);
        if (netSetup(netUdp, gameFrontMyUdp, "127.0.0.1", gameFrontTargetUdp, password, TRUE, gameFrontTrackerAddr, gameFrontTrackerPort, gameFrontTrackerEnabled, wantRejoin, gameFrontWbnUse, gameFrontWbnPass) == FALSE) {
          wantRejoin = FALSE;
          MessageBoxA(NULL, "Unable to start server", DIALOG_BOX_TITLE, MB_ICONINFORMATION);
          netDestroy();
          screenDestroy();
          gameFrontShutdownServer();
          returnValue = FALSE;
          dlgState = openStart;
        } else {
          dlgState = openFinished;
        }
       } else {
          MessageBox(NULL, "Error starting server", DIALOG_BOX_TITLE, MB_ICONINFORMATION);
          dlgState = openStart;
       }
      SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
  } else if (dlgState == openSetup && newState == openFinished) {
    dlgState = openFinished;
    if (netSetup(netSingle, gameFrontMyUdp, gameFrontUdpAddress, gameFrontTargetUdp, password, TRUE, gameFrontTrackerAddr, gameFrontTrackerPort, gameFrontTrackerEnabled, wantRejoin, gameFrontWbnUse, gameFrontWbnPass) == FALSE) {
      wantRejoin = FALSE;
      MessageBoxA(NULL, langGetText(STR_GAMEFRONTERR_NETSINGLEPLAYER), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
      netDestroy();
      screenDestroy();
//      dlgState = openSetup;
      returnValue = FALSE;
    } else {
      if (strcmp(fileName, "") != 0) {
        screenLoadMap(fileName, gametype, hiddenMines, startDelay, timeLen, gameFrontName, FALSE);
      } else {
        gameFrontLoadInBuiltMap();
      }
    }
  } else {
    dlgState = newState;
  }

  return returnValue;
}

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
void gameFrontGetCmdArg(char *getName) {
  strcpy(getName, fileName);
}

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
void gameFrontSetFileName(char *getName) {
  strcpy(fileName, getName);
}


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
void gameFrontSetGameOptions(char *pword, gameType gt, bool hm, aiType ai, long sd, long tlimit, bool justPass) {
  strcpy(password, pword);
  if (justPass == FALSE) {
    gametype = gt;
    hiddenMines = hm;
    compTanks = ai;
    if (compTanks == aiNone) {
      brainsHandlerSet(windowWnd(), FALSE);
    } else {
      brainsHandlerSet(windowWnd(), TRUE);
    }
    startDelay = sd;
    timeLen = tlimit;
  }
}

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
void gameFrontGetGameOptions(char *pword, gameType *gt, bool *hm, aiType *ai, long *sd, long *tlimit) {
  strcpy(pword, password);
  *gt = gametype;
  *hm = hiddenMines;
  *ai = compTanks;
  *sd = startDelay;
  *tlimit = timeLen;
}

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
void gameFrontGetUdpOptions(char *pn, char *add, unsigned short *theirUdp, unsigned short *myUdp) {
  strcpy(pn, gameFrontName);
  strcpy(add, gameFrontUdpAddress);
  *myUdp = gameFrontMyUdp;
  *theirUdp = gameFrontTargetUdp;
}

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
void gameFrontSetUdpOptions(char *pn, char *add, unsigned short theirUdp, unsigned short myUdp) {
  strcpy(gameFrontName, pn);
  strcpy(gameFrontUdpAddress, add);
  gameFrontMyUdp = myUdp;
  gameFrontTargetUdp = theirUdp;
}

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
void gameFrontGetPassword(char *pword) {
  password[0] = '\0';
  DialogBox(windowGetInstance(), MAKEINTRESOURCE(IDD_PASSWORD), NULL, (DLGPROC) dialogPasswordCallback);
  strcpy(pword, password);
}

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
void gameFrontGetPlayerName(char *pn) {
  strcpy(pn, gameFrontName);
}

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
void gameFrontSetPlayerName(char *pn) {
  strcpy(gameFrontName, pn);
}

/*********************************************************
*NAME:          gameFrontGetWnd
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 26/2/99
*PURPOSE:
* Gets the main window handle. Called by the Opening
* dialog
*
*ARGUMENTS:
*
*********************************************************/
HWND gameFrontGetWnd(void) {
  return gameFrontWnd;
}

/*********************************************************
*NAME:          gameFrontSetAIType
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
* Sets the AI type of the game. (From networking module)
*
*ARGUMENTS:
*
*********************************************************/
void gameFrontSetAIType(aiType ait) {
  compTanks = ait;
  screenSetAiType(compTanks);
  if (compTanks == aiNone) {
    brainsHandlerSet(windowWnd(), FALSE);
  } else {
    brainsHandlerSet(windowWnd(), TRUE);
  }
}

/*********************************************************
*NAME:          gameFrontGetPrefs
*AUTHOR:        John Morrison
*CREATION DATE: 19/4/99
*LAST MODIFIED: 13/6/00
*PURPOSE:
* Gets the preferences from the preferences file. Returns
* success.
*
*ARGUMENTS:
*  keys - Pointer to keys structure
*  useAutoslow - Pointer to hold auto slowdown
*  useAutohide - Pointer to hold auto gunsight show/hide
*********************************************************/
bool gameFrontGetPrefs(keyItems *keys, bool *useAutoslow, bool *useAutohide) {
  char buff[FILENAME_MAX]; /* Read Buffer               */
  char def[FILENAME_MAX];  /* The default value      */
 
  /* Player Name */
  strcpy(def, langGetText(STR_DLGGAMESETUP_DEFAULTNAME));
  GetPrivateProfileStringA("SETTINGS", "Player Name", def, gameFrontName, FILENAME_MAX, PREFERENCE_FILE);
 
  /* Target Address */
  def[0] = '\0';
  GetPrivateProfileStringA("SETTINGS", "Target Address", def, gameFrontUdpAddress, FILENAME_MAX, PREFERENCE_FILE);
   
  /* Target UDP Port */
  itoa(DEFAULT_UDP_PORT, def, 10);
  GetPrivateProfileStringA("SETTINGS", "Target UDP Port",  def,  buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontMyUdp = atoi(buff);
  gameFrontTargetUdp = atoi(buff);
  /* My UDP Port */
  itoa(DEFAULT_UDP_PORT, def, 10);
  GetPrivateProfileStringA("SETTINGS", "UDP Port", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontMyUdp = atoi(buff);

  /* Keys */
  itoa(DEFAULT_FORWARD, def, 10);
  GetPrivateProfileStringA("KEYS", "Forward", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiForward = atoi(buff);
  itoa(DEFAULT_BACKWARD, def, 10);
  GetPrivateProfileStringA("KEYS", "Backwards", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiBackward = atoi(buff);
  itoa(DEFAULT_LEFT, def, 10);
  GetPrivateProfileStringA("KEYS", "Left", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiLeft = atoi(buff);
  itoa(DEFAULT_RIGHT, def, 10);
  GetPrivateProfileStringA("KEYS", "Right", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiRight = atoi(buff);
  itoa(DEFAULT_SHOOT, def, 10);
  GetPrivateProfileStringA("KEYS", "Shoot", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiShoot = atoi(buff);
  itoa(DEFAULT_LAY_MINE, def, 10);
  GetPrivateProfileStringA("KEYS", "Lay Mine", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiLayMine = atoi(buff);
  itoa(DEFAULT_SCROLL_GUNINCREASE, def, 10);
  GetPrivateProfileStringA("KEYS", "Increase Range", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiGunIncrease = atoi(buff);
  itoa(DEFAULT_SCROLL_GUNDECREASE, def, 10);
  GetPrivateProfileStringA("KEYS", "Decrease Range", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiGunDecrease = atoi(buff);
  itoa(DEFAULT_TANKVIEW, def, 10);
  GetPrivateProfileStringA("KEYS", "Tank View", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiTankView = atoi(buff);
  itoa(DEFAULT_PILLVIEW, def, 10);
  GetPrivateProfileStringA("KEYS", "Pill View", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiPillView = atoi(buff);
  itoa(DEFAULT_SCROLLUP, def, 10);
  GetPrivateProfileStringA("KEYS", "Scroll Up", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiScrollUp = atoi(buff);
  itoa(DEFAULT_SCROLLDOWN, def, 10);
  GetPrivateProfileStringA("KEYS", "Scroll Down", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiScrollDown = atoi(buff);
  itoa(DEFAULT_SCROLLLEFT, def, 10);
  GetPrivateProfileStringA("KEYS", "Scroll Left", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiScrollLeft = atoi(buff);
  itoa(DEFAULT_SCROLLRIGHT, def, 10);
  GetPrivateProfileStringA("KEYS", "Scroll Right", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiScrollRight = atoi(buff);


  /* Remeber */
  GetPrivateProfileStringA("SETTINGS", "Remember Player Name", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontRemeber = YESNO_TO_TRUEFALSE(buff[0]);

  /* Game Options */
  GetPrivateProfileStringA("GAME OPTIONS", "Hidden Mines", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  hiddenMines = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("GAME OPTIONS", "Allow Computer Tanks", "0", buff, FILENAME_MAX, PREFERENCE_FILE);
  compTanks = atoi(buff);
  GetPrivateProfileStringA("GAME OPTIONS", "Game Type", "1", buff, FILENAME_MAX, PREFERENCE_FILE);
  gametype = atoi(buff);
  GetPrivateProfileStringA("GAME OPTIONS", "Start Delay", "0", buff, FILENAME_MAX, PREFERENCE_FILE);
  startDelay = atoi(buff);
  ltoa(UNLIMITED_GAME_TIME, def, 10);
  GetPrivateProfileStringA("GAME OPTIONS", "Time Length", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  timeLen = atol(buff);
  GetPrivateProfileStringA("GAME OPTIONS", "Auto Slowdown", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  *useAutoslow = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("GAME OPTIONS", "Auto Show-Hide Gunsight", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  *useAutohide = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("GAME OPTIONS", "Skin", "", gameFrontSkinName, FILENAME_MAX, PREFERENCE_FILE);


  /* Tracker options */
  GetPrivateProfileStringA("TRACKER", "Address", TRACKER_ADDRESS, gameFrontTrackerAddr, FILENAME_MAX, PREFERENCE_FILE);
  itoa(TRACKER_PORT, def, 10);
  GetPrivateProfileStringA("TRACKER", "Port", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontTrackerPort = atoi(buff);
  GetPrivateProfileStringA("TRACKER", "Enabled", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontTrackerEnabled = YESNO_TO_TRUEFALSE(buff[0]);



  /* Menu Items */
  itoa(FRAME_RATE_30, def, 10);
  GetPrivateProfileStringA("MENU", "Frame Rate", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  frameRate = atoi(buff);
  GetPrivateProfileStringA("MENU", "Show Gunsight", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  showGunsight = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Sound Effects", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  soundEffects = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Allow Background Sound", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  backgroundSound = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "ISA Sound Card", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  isISASoundCard = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Show Gunsight", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  showGunsight = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Show Newswire Messages", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  showNewswireMessages = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Show Assistant Messages", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  showAssistantMessages = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Show AI Messages", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  showAIMessages = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Show Network Status Messages", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  showNetworkStatusMessages = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Show Network Debug Messages", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  showNetworkDebugMessages = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Autoscroll Enabled", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  autoScrollingEnabled = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Show Pill Labels", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  showPillLabels = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Show Base Labels", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  showBaseLabels = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Label Own Tank", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  labelSelf = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("MENU", "Window Size", "1", buff, FILENAME_MAX, PREFERENCE_FILE);
  zoomFactor = atoi(buff);
  GetPrivateProfileStringA("MENU", "Message Label Size", "1", buff, FILENAME_MAX, PREFERENCE_FILE);
  labelMsg = atoi(buff);
  GetPrivateProfileStringA("MENU", "Tank Label Size", "1", buff, FILENAME_MAX, PREFERENCE_FILE);
  labelTank = atoi(buff);

  /* Winbolo.net */
  GetPrivateProfileStringA("WINBOLO.NET", "Password", "", gameFrontWbnPass, FILENAME_MAX, PREFERENCE_FILE);
  GetPrivateProfileStringA("WINBOLO.NET", "Active", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontWbnUse  = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileStringA("WINBOLO.NET", "Save Password", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontWbnSavePass = YESNO_TO_TRUEFALSE(buff[0]);



  /* Load in the language */
  GetPrivateProfileStringA("SETTINGS", "Language", "", buff, FILENAME_MAX, PREFERENCE_FILE);
  if (strcmp(buff, "") != 0) {
    /* Must be something to load */
    strcpy(def, LANG_DIR_STRING);
    if (winUtilWBSubDirExist(def) == TRUE) {
    /* Okay Directory exists */
      strcat(def, SLASH_STRING);
      strcat(def, buff);
      langLoadFile(def, buff);
    }
  }


  return TRUE;
}

/*********************************************************
*NAME:          gameFrontPutPrefs
*AUTHOR:        John Morrison
*CREATION DATE: 19/4/99
*LAST MODIFIED: 13/6/00
*PURPOSE:
* Puts the preferences to the preferences file.
*
*ARGUMENTS:
*  keys       - Pointer to keys structure
*********************************************************/
void gameFrontPutPrefs(keyItems *keys) {
  char playerName[PLAYER_NAME_LEN]; /* Current player Name       */
  char playerName2[PLAYER_NAME_LEN]; /* Current player Name       */
  char buff[FILENAME_MAX];          /* Read Buffer               */
 
  /* Player Name */
  /* If it's a single-player game or we should remember the player's name and the dialog isn't at the Setup screen */
  if ((netGetType() == netSingle || gameFrontRemeber == TRUE) && dlgState != openSetup) {
    screenGetPlayerName(playerName);
    if (playerName[0] == '*') {
      strcpy(playerName2, playerName+1);
    } else {
      strcpy(playerName2, playerName);
    }
    strcpy(gameFrontName, playerName2);
    WritePrivateProfileStringA("SETTINGS", "Player Name", playerName2, PREFERENCE_FILE);
  } else {
	  WritePrivateProfileStringA("SETTINGS", "Player Name", gameFrontName, PREFERENCE_FILE);
  }

  /* Target Address */
  WritePrivateProfileStringA("SETTINGS", "Target Address", gameFrontUdpAddress, PREFERENCE_FILE);
  
  /* Target UDP Port */
  itoa(gameFrontTargetUdp, buff, 10);
  WritePrivateProfileStringA("SETTINGS", "Target UDP Port", buff, PREFERENCE_FILE);
  /* My UDP Port */
  itoa(gameFrontMyUdp, buff, 10);
  WritePrivateProfileStringA("SETTINGS", "UDP Port", buff, PREFERENCE_FILE);
  /* Languages */
  langGetFileName(buff);
  WritePrivateProfileStringA("SETTINGS", "Language", buff, PREFERENCE_FILE);

  /* Keys */
  itoa(keys->kiForward, buff, 10);
  WritePrivateProfileStringA("KEYS", "Forward", buff, PREFERENCE_FILE);
  itoa(keys->kiBackward, buff, 10);
  WritePrivateProfileStringA("KEYS", "Backwards", buff, PREFERENCE_FILE);
  itoa(keys->kiLeft, buff, 10);
  WritePrivateProfileStringA("KEYS", "Left", buff, PREFERENCE_FILE);
  itoa(keys->kiRight, buff, 10);
  WritePrivateProfileStringA("KEYS", "Right", buff, PREFERENCE_FILE);
  itoa(keys->kiShoot, buff, 10);
  WritePrivateProfileStringA("KEYS", "Shoot", buff, PREFERENCE_FILE);
  itoa(keys->kiLayMine, buff, 10);
  WritePrivateProfileStringA("KEYS", "Lay Mine", buff, PREFERENCE_FILE);
  itoa(keys->kiGunIncrease, buff, 10);
  WritePrivateProfileStringA("KEYS", "Increase Range", buff, PREFERENCE_FILE);
  itoa(keys->kiGunDecrease, buff, 10);
  WritePrivateProfileStringA("KEYS", "Decrease Range", buff, PREFERENCE_FILE);
  itoa(keys->kiTankView, buff, 10);
  WritePrivateProfileStringA("KEYS", "Tank View", buff, PREFERENCE_FILE);
  itoa(keys->kiPillView, buff, 10);
  WritePrivateProfileStringA("KEYS", "Pill View", buff, PREFERENCE_FILE);
  itoa(keys->kiScrollUp, buff, 10);
  WritePrivateProfileStringA("KEYS", "Scroll Up", buff, PREFERENCE_FILE);
  itoa(keys->kiScrollDown, buff, 10);
  WritePrivateProfileStringA("KEYS", "Scroll Down", buff, PREFERENCE_FILE);
  itoa(keys->kiScrollLeft, buff, 10);
  WritePrivateProfileStringA("KEYS", "Scroll Left", buff, PREFERENCE_FILE);
  itoa(keys->kiScrollRight, buff, 10);
  WritePrivateProfileStringA("KEYS", "Scroll Right", buff, PREFERENCE_FILE);
  /* Remember */
  WritePrivateProfileStringA("SETTINGS", "Remember Player Name", TRUEFALSE_TO_STR(gameFrontRemeber), PREFERENCE_FILE);

  /* Options */
  WritePrivateProfileStringA("GAME OPTIONS", "Hidden Mines", TRUEFALSE_TO_STR(hiddenMines), PREFERENCE_FILE);
  itoa(compTanks, buff, 10);
  WritePrivateProfileStringA("GAME OPTIONS", "Allow Computer Tanks", buff, PREFERENCE_FILE);
  compTanks = atoi(buff);
  itoa(gametype, buff, 10);
  WritePrivateProfileStringA("GAME OPTIONS", "Game Type", buff, PREFERENCE_FILE);
  ltoa(startDelay, buff, 10);
  WritePrivateProfileStringA("GAME OPTIONS", "Start Delay", buff, PREFERENCE_FILE);
  ltoa(timeLen, buff, 10);
  WritePrivateProfileStringA("GAME OPTIONS", "Time Length", buff, PREFERENCE_FILE);
  WritePrivateProfileStringA("GAME OPTIONS", "Auto Slowdown", TRUEFALSE_TO_STR(useAutoslow), PREFERENCE_FILE);
  WritePrivateProfileStringA("GAME OPTIONS", "Auto Show-Hide Gunsight", TRUEFALSE_TO_STR(useAutohide), PREFERENCE_FILE);
  skinsGetFileName(gameFrontSkinName);
  WritePrivateProfileStringA("GAME OPTIONS", "Skin", gameFrontSkinName, PREFERENCE_FILE);
    
  /* Tracker */
  WritePrivateProfileStringA("TRACKER", "Address", gameFrontTrackerAddr, PREFERENCE_FILE);
  itoa(gameFrontTrackerPort, buff, 10);
  WritePrivateProfileStringA("TRACKER", "Port", buff, PREFERENCE_FILE);
  WritePrivateProfileStringA("TRACKER", "Enabled", TRUEFALSE_TO_STR(gameFrontTrackerEnabled), PREFERENCE_FILE);

  /* Menu Items */
  itoa(frameRate, buff, 10);
  WritePrivateProfileStringA("MENU", "Frame Rate", buff, PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Show Gunsight", TRUEFALSE_TO_STR(showGunsight), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Sound Effects", TRUEFALSE_TO_STR(soundEffects), PREFERENCE_FILE);

  WritePrivateProfileStringA("MENU", "Allow Background Sound", TRUEFALSE_TO_STR(backgroundSound), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "ISA Sound Card", TRUEFALSE_TO_STR(isISASoundCard), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Show Gunsight", TRUEFALSE_TO_STR(showGunsight), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Show Newswire Messages", TRUEFALSE_TO_STR(showNewswireMessages), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Show Assistant Messages", TRUEFALSE_TO_STR(showAssistantMessages), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Show AI Messages", TRUEFALSE_TO_STR(showAIMessages), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Show Network Status Messages", TRUEFALSE_TO_STR(showNetworkStatusMessages), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Show Network Debug Messages", TRUEFALSE_TO_STR(showNetworkDebugMessages), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Autoscroll Enabled", TRUEFALSE_TO_STR(autoScrollingEnabled), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Show Pill Labels", TRUEFALSE_TO_STR(showPillLabels), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Show Base Labels", TRUEFALSE_TO_STR(showBaseLabels), PREFERENCE_FILE);
  WritePrivateProfileStringA("MENU", "Label Own Tank", TRUEFALSE_TO_STR(labelSelf), PREFERENCE_FILE);
  itoa(zoomFactor, buff, 10);
  WritePrivateProfileStringA("MENU", "Window Size", buff, PREFERENCE_FILE);
  itoa(labelMsg, buff, 10);
  WritePrivateProfileStringA("MENU", "Message Label Size", buff, PREFERENCE_FILE);
  itoa(labelTank, buff, 10);
  WritePrivateProfileStringA("MENU", "Tank Label Size", buff, PREFERENCE_FILE);

  /* Winbolo.net */
  if (gameFrontWbnSavePass == TRUE) {
    WritePrivateProfileStringA("WINBOLO.NET", "Password", gameFrontWbnPass, PREFERENCE_FILE);
  } else {
    WritePrivateProfileStringA("WINBOLO.NET", "Password", "", PREFERENCE_FILE);
  }
  WritePrivateProfileStringA("WINBOLO.NET", "Active", TRUEFALSE_TO_STR(gameFrontWbnUse ), PREFERENCE_FILE);
  WritePrivateProfileStringA("WINBOLO.NET", "Save Password", TRUEFALSE_TO_STR(gameFrontWbnSavePass), PREFERENCE_FILE);
  GetPrivateProfileStringA("WINBOLO.NET", "Host", "wbn.winbolo.net", buff, FILENAME_MAX, PREFERENCE_FILE);
  WritePrivateProfileStringA("WINBOLO.NET", "Host", buff, PREFERENCE_FILE);
}

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
void gameFrontSetRemeber(bool isSet) {
  gameFrontRemeber = isSet;
}

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
bool gameFrontGetRemeber() {
  return gameFrontRemeber;
}

/*********************************************************
*NAME:          serverGameTimer
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 20/3/99
*PURPOSE:
* The Game Timer. If there are no events to prcess this 
* routine is called. If the elapsed
*
*ARGUMENTS:
*
*********************************************************/
void CALLBACK serverGameTimer(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
  static int trackerTime = 5500;   /* When we should update the tracker */
  static int wbnTime = 0;
  bool isGameWon;

  if (isServer == FALSE) {
    return;
  }

  serverTick = winboloTimer();
  if ((serverTick - serverOldTick) > SERVER_TICK_LENGTH) {
    /* Get the keyboard state */
    while ((serverTick - serverOldTick) > SERVER_TICK_LENGTH) {
      trackerTime++;
      wbnTime++;
      
      threadsWaitForMutex();
      threadsSetContext(TRUE);
      serverCoreGameTick();
      threadsSetContext(FALSE);
      threadsReleaseMutex();
      ticks++;
      serverOldTick += SERVER_TICK_LENGTH;
    }
    if (winbolonetIsRunning() == TRUE && serverCoreGetActualGameType() != gameOpen) {
      threadsWaitForMutex();
      isGameWon = serverCoreCheckGameWin(FALSE);
      threadsReleaseMutex();
      if (isGameWon == TRUE) {
        gameFrontShutdownServer();
        return;
      }
    }
  } 

  threadsWaitForMutex();
  threadsSetContext(TRUE);  
  serverNetCheckRemovePlayers();
  serverNetMakePosPackets();
  serverNetMakeData();
  threadsSetContext(FALSE);
  threadsReleaseMutex();
  
  
  if (wbnTime > 100) {
    threadsWaitForMutex();
    winbolonetServerUpdate(serverCoreGetNumPlayers(), serverCoreGetNumNeutralBases(), serverCoreGetNumNeutralPills(), FALSE);
    threadsReleaseMutex();
    wbnTime = 0;
  }


  if (trackerTime >= (6000)) {
    serverNetSendTrackerUpdate();
    trackerTime = 0;
  }
}

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
void gameFrontShutdownServer() {
  if (isServer == TRUE) {
    isServer = FALSE;
    threadsSetContext(TRUE);
    serverNetSendQuitMessage();
    timeKillEvent(serverTimerGameID);
    serverNetDestroy();
    winbolonetDestroy();
    serverCoreDestroy();
    threadsSetContext(FALSE);
  }
}

/*********************************************************
*NAME:          gameFrontSetupServer
*AUTHOR:        John Morrison
*CREATION DATE: 3/11/99
*LAST MODIFIED:  3/1/00
*PURPOSE:
* Attempts to start the server process. Returns success
*
*ARGUMENTS:
*
*********************************************************/
bool gameFrontSetupServer() {
  bool returnValue;           /* Value to return */
  char tempPath[MAX_PATH]; /* Temp Path for writing out map from resource to read back in */
  char tempFile[MAX_PATH]; /* Temp filename for reading file */
  FILE *fp; /* File pointer used to write data out */
 
  returnValue = TRUE;
  isServer = FALSE;
  ticks = 0;

  threadsSetContext(TRUE);
  if (strcmp(fileName, "") != 0 ) {
    if (serverCoreCreate(fileName, gametype, hiddenMines, startDelay , timeLen ) == FALSE) {
      returnValue = FALSE;
      MessageBoxA(NULL, "Error Loading Map", DIALOG_BOX_TITLE, MB_OK);
    }
  } else {
    /* Use the inbuilt map */
    HGLOBAL hGlobal;  /* Resource handle */
    BYTE *buff;       /* Byte buffer     */
    HRSRC res;        /* FindResource return */

    res = FindResourceA(NULL, MAKEINTRESOURCE(IDR_EVERARD), "MAPS");
    if (res != NULL) {
      hGlobal = LoadResource(NULL, res);
      if (hGlobal != NULL) {
        buff = LockResource(hGlobal);
		if (buff != NULL) {
	        GetTempPathA(MAX_PATH, tempPath);
	        sprintf(tempFile, "%sEverard Island.map", tempPath);
	        fp = fopen(tempFile, "wb");
	        fwrite(buff, 1954, 1, fp);
	        fclose(fp);
			returnValue = serverCoreCreate(tempFile, gametype, hiddenMines, startDelay, timeLen);
		    DeleteFile(tempFile);
		}
      }
    }
  }


  if (returnValue == TRUE) {
    if (serverNetCreate(gameFrontTargetUdp, password, compTanks, gameFrontTrackerAddr, gameFrontTrackerPort, gameFrontTrackerEnabled, NULL, 0) == FALSE) {
      MessageBoxA(NULL,"Error starting Network. Is the port in use?", DIALOG_BOX_TITLE, MB_OK);
      serverNetDestroy();
      serverCoreDestroy();
      returnValue = FALSE;
    }
  }

  if (returnValue == TRUE && gameFrontWbnUse == TRUE) {
    char mapName[255];
    serverCoreGetMapName(mapName);
    winbolonetCreateServer(mapName, gameFrontTargetUdp, (BYTE) gametype, (BYTE) compTanks, serverCoreGetAllowHiddenMines(), (BYTE) (password[0] == 0 ? FALSE : TRUE), screenNumBases(), screenNumPills(), serverCoreGetNumNeutralBases(), serverCoreGetNumNeutralPills(), serverCoreGetNumPlayers(), serverCoreGetTimeGameCreated());
  }
  threadsSetContext(FALSE);


  if (threadsCreate(TRUE) == FALSE) {
    threadsDestroy();
    serverNetDestroy();
    serverCoreDestroy();
	  returnValue = FALSE;
  } 
  
  if (returnValue == TRUE) {
    isServer = TRUE;
    serverOldTick = winboloTimer();
    serverTimerGameID = timeSetEvent(SERVER_TICK_LENGTH, 10000, serverGameTimer, 0, TIME_PERIODIC);
  }
 
  return returnValue;;
}

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
void gameFrontGetTrackerOptions(char *address, unsigned short *port, bool *enabled) {
  strcpy(address, gameFrontTrackerAddr);
  *port = gameFrontTrackerPort;
  *enabled = gameFrontTrackerEnabled;
}

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
void gameFrontEnableRejoin() {
  wantRejoin = TRUE;
}

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
void gameFrontSetTrackerOptions(char *address, unsigned short port, bool enabled) {
  strcpy(gameFrontTrackerAddr, address);
  gameFrontTrackerPort = port;
  gameFrontTrackerEnabled = enabled;
}

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
bool gameFrontPreferencesExist() {
  bool returnValue;        /* Value to return           */
  char loc[FILENAME_MAX];  /* Preferences File Location */
  int value;               /* Read in value etc.        */
  WIN32_FIND_DATA fd;      /* Find data */
  HANDLE findHandle;       /* The find handle */


  returnValue = TRUE;
  /* Get the file location */
  value = GetWindowsDirectory(loc, FILENAME_MAX);
  if (loc[value] != '\\') {
    strcat(loc, "\\");
  }
  strcat(loc, PREFERENCE_FILE);
  findHandle = FindFirstFile(loc, &fd);
  if (findHandle == INVALID_HANDLE_VALUE) {
    returnValue = FALSE;
  } 
  FindClose(findHandle);

  return returnValue;
}

/*********************************************************
*NAME:          gameFrontLoadInBuiltMap
*AUTHOR:        John Morrison
*CREATION DATE: 01/05/00
*LAST MODIFIED: 21/04/06
*PURPOSE:
* Attempts to load the built in map by loading the 
* compress resource. Returns Success
*
*ARGUMENTS:
*
*********************************************************/
bool gameFrontLoadInBuiltMap() {
  bool returnValue; /* Value to return */
  HGLOBAL hGlobal;  /* Resource handle */
  BYTE *buff;       /* Byte buffer     */
  HRSRC res;        /* FindResource return */
  char tempPath[MAX_PATH]; /* Temp Path for writing out map from resource to read back in */
  char tempFile[MAX_PATH]; /* Temp filename for reading file */
  FILE *fp; /* File pointer used to write data out */


  returnValue = FALSE;
  res = FindResourceA(windowGetInstance(), MAKEINTRESOURCE(IDR_EVERARD), "MAPS");
  if (res != NULL) {
    hGlobal = LoadResource(NULL, res);
    if (hGlobal != NULL) {
      buff = LockResource(hGlobal);
      if (buff != NULL) {
        GetTempPathA(MAX_PATH, tempPath);
        sprintf(tempFile, "%sEverard Island.map", tempPath);
        fp = fopen(tempFile, "wb");
        fwrite(buff, 1954, 1, fp);
        fclose(fp);
        returnValue = screenLoadMap(tempFile, gametype, hiddenMines, startDelay, timeLen, gameFrontName, FALSE);
        DeleteFile(tempFile);
      }
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          gameFrontLoadTutorial
*AUTHOR:        John Morrison
*CREATION DATE: 01/05/00
*LAST MODIFIED: 21/04/06
*PURPOSE:
* Attempts to load the built in tutorial by loading the 
* compress resource. Returns Success
*
*ARGUMENTS:
*
*********************************************************/
bool gameFrontLoadTutorial() {
  bool returnValue; /* Value to return */
  HGLOBAL hGlobal;  /* Resource handle */
  BYTE *buff;       /* Byte buffer     */
  HRSRC res;        /* FindResource return */
  char tempPath[MAX_PATH]; /* Temp Path for writing out map from resource to read back in */
  char tempFile[MAX_PATH]; /* Temp filename for reading file */
  FILE *fp; /* File pointer used to write data out */

  returnValue = FALSE;
  res = FindResourceA(windowGetInstance(), MAKEINTRESOURCE(IDR_TUTORIAL), "MAPS");
  if (res != NULL) {
    hGlobal = LoadResource(NULL, res);
    if (hGlobal != NULL) {
      buff = LockResource(hGlobal);
      if (buff != NULL) {
        GetTempPathA(MAX_PATH, tempPath);
        sprintf(tempFile, "%sInbuilt Tutorial.map", tempPath);
        fp = fopen(tempFile, "wb");
        fwrite(buff, 3148 , 1, fp);
        fclose(fp);
        returnValue = screenLoadMap(tempFile, gameStrictTournament, FALSE, 0, -1, langGetText(STR_DLGGAMESETUP_DEFAULTNAME), FALSE);
        DeleteFile(tempFile);


//        returnValue = screenLoadCompressedMap(buff, 4182, (char *) "Inbuilt Tutorial", gameStrictTournament, FALSE, 0, -1, langGetText(STR_DLGGAMESETUP_DEFAULTNAME), FALSE);
        if (returnValue == TRUE) {
          netSetup(netSingle, gameFrontMyUdp, gameFrontUdpAddress, gameFrontTargetUdp, password, FALSE, gameFrontTrackerAddr, gameFrontTrackerPort, gameFrontTrackerEnabled, wantRejoin, gameFrontWbnUse, gameFrontWbnPass);
        }
      }
    }
  }

  return returnValue;
}

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
void gameFrontGetWinbolonetSettings(char *password, bool *useWbn, bool *savePass) {
  *useWbn = gameFrontWbnUse;
  *savePass = gameFrontWbnSavePass;
  strcpy(password, gameFrontWbnPass);
}

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
void gameFrontSetWinbolonetSettings(char *password, bool useWbn, bool savePass) {
  strcpy(gameFrontWbnPass, password);
  gameFrontWbnUse = useWbn;
  gameFrontWbnSavePass = savePass;
}


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
void gameFrontSetRegistryKeys() {
  HKEY hKey;
  DWORD dw;
  char str[1024];
  char fileName[1024];

  GetModuleFileName(windowGetInstance(), fileName, 1024);

  /* Top Key */
  if (RegCreateKeyEx(HKEY_CLASSES_ROOT, "winbolo", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw) == ERROR_SUCCESS) {
    strcpy(str, "URL:WinBolo Protocol");
    RegSetValueEx(hKey, NULL, 0, REG_SZ, str, (DWORD) strlen(str));
    str[0] = EMPTY_CHAR;
    RegSetValueEx(hKey, "URL Protocol", 0, REG_SZ, str, (DWORD) strlen(str));
    RegCloseKey(hKey);
  }

  /* Default Icon Subkey */
  if (RegCreateKeyEx(HKEY_CLASSES_ROOT, "winbolo\\DefaultIcon", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw) == ERROR_SUCCESS) {
    RegSetValue(hKey, NULL, REG_SZ, fileName, (DWORD) strlen(fileName));
    RegCloseKey(hKey);
  }

 if (RegCreateKeyEx(HKEY_CLASSES_ROOT, "winbolo\\shell\\open\\command", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw) == ERROR_SUCCESS) {
    strcpy(str, fileName);
    strcat(str, " %1");
    RegSetValue(hKey, NULL, REG_SZ, str, (DWORD) strlen(str));
    RegCloseKey(hKey);
  }
}

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
void gameFrontSetAddressFromWebLink(char *address) {
  char *tok;
  char *ptr;

  gameFrontTargetUdp = 0;
  ptr = address + strlen("winbolo://");
  tok = strtok(ptr, ":");
  if (tok != NULL) {
    strcpy(gameFrontUdpAddress, tok);
    tok = strtok(NULL, ":");
    if (tok != NULL) {
      gameFrontTargetUdp = atoi(tok);
    }
  }
}

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
void gameFrontWinBoloNetClient(HWND appWnd, bool forceShow) {
  bool wbnParticipant;  /* Is a winbolo.net participant */
  HMENU hMenu;          /* The menu of the Window */

  hMenu = GetMenu(appWnd);
  wbnParticipant = FALSE;
  if (netGetType() == netUdp) {
    wbnParticipant = winbolonetIsRunning();
  }
  if (forceShow == TRUE) {
    /* Show it */
    EnableMenuItem(hMenu, ID_BOLOMENU_CHANGENAME, MF_ENABLED);
  } 

  if (wbnParticipant == TRUE) {
    /* Hide it */
    EnableMenuItem(hMenu, ID_BOLOMENU_CHANGENAME, MF_GRAYED);
  }
}

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
void gameFrontReloadSkins() {
  drawCleanup();
  soundCleanup();
  if ((drawSetup(windowGetInstance(), gameFrontWnd)) == FALSE) {
    MessageBoxA(NULL, langGetText(STR_GAMEFRONTERR_DDRAW), (LPCSTR) DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
  }
  if ((soundSetup(windowGetInstance(), gameFrontWnd)) == FALSE) {
    MessageBoxA(NULL, langGetText(STR_GAMEFRONTERR_DSOUND), (LPCSTR) DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
  }
}

time_t serverMainGetTicks() {
  return ticks;
}
