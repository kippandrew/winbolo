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
*Creation Date: 27/1/99
*Last Modified:   6/7/00
*Purpose:
*  Provides the front end for dialog/preferences etc.
*********************************************************/

#include <string.h>
#include <stdlib.h>
#include <commdlg.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include "./bolo/global.h"
#include "../gui/resource.h"
#include "../bolo/backend.h"
#include "draw.h"
#include "sound.h"
#include "input.h"
#include "cursor.h"
#include "font.h"
/*#include "dialogopening.h"
#include "dialoggamesetup.h"
#include "dialogudpsetup.h"
#include "dialogpassword.h"
#include "dialoggamefinder.h"
#include "dialogLanguages.h"
#include "dialogkeysetup.h"
#include "dialogsetname.h" */
#include "../bolo/network.h"
#include "../gui/lang.h"
//#include "..\brainsHandler.h"
#include "../gui/clientmutex.h"
//#include "..\winutil.h"
#include "gamefront.h"

/* Game playing options */
char fileName[FILENAME_MAX]; /* filename and path of the map to use or command line arguement */
char password[MAP_STR_SIZE]; /* game password */
bool hiddenMines;            /* Hidden mines allowed */
aiType compTanks = aiNone;   /* Whether computer tanks are allowed */
gameType gametype;           /* The type of game being played */
long startDelay;             /* The start delay */
long timeLen;                /* Game time length */
bool gameFrontRemeber;       /* Remeber player name? */

/* UDP stuff */
char gameFrontName[PLAYER_NAME_LEN]; /* Player Name */
char gameFrontUdpAddress[FILENAME_MAX]; /* IP of target machine */
unsigned short gameFrontMyUdp;
unsigned short gameFrontTargetUdp;

char gameFrontTrackerAddr[FILENAME_MAX];
unsigned short gameFrontTrackerPort;
bool gameFrontTrackerEnabled;

/* Window handle */
//HWND gameFrontWnd;


/* Dialog states */
openingStates dlgState = openStart;

bool isServer = FALSE; /* Are we server of a net game */

bool useAutoslow; /* Other key preferences options */
bool useAutohide;



bool wantRejoin; /* Do we want to rejoin */

/* Server process stuff */
//PROCESS_INFORMATION pi;     /* Process information    */

/* STDIn for the server */
//HANDLE hSaveStdin, hChildStdinRd, hChildStdinWrDup, hChildStdinWr;

//extern HWND msgWnd;
//extern HWND keyWnd;
//extern HWND sysWnd;
//extern HWND netWnd;
//extern HWND infoWnd;

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



void windowStartTutorial();


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

  isTutorial = FALSE;
  password[0] = '\0';
  gameFrontName[0] = '\0';
  gameFrontUdpAddress[0] = '\0';
  wantRejoin = FALSE;
  langSetup();

//  dialogSetNameInGame(FALSE);

  /* Read preferences */
// gameFrontGetPrefs(keys, &useAutoslow, &useAutohide);

  /* Process the command line argument - Remove "" from around it */
/*  length = strlen(cmdLine);
  if (length > 0 ) {
    if (cmdLine[0] == '\"') {
      cmdLine[length-1] = '\0';
      cmdLine++;
    }
  }
  strcpy(fileName, cmdLine); */

  
  /* Initalise game subsystems */
  OKStart = TRUE;
  if (isLoaded == FALSE) {
//    appWnd = windowCreate(hInst, nCmdShow);
    if (appWnd == NULL) {
//      MessageBox(NULL, langGetText(STR_GAMEFRONTERR_WINDOW), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }
  
    if ((drawSetup(hInst, appWnd)) == FALSE) {
//      MessageBox(NULL, langGetText(STR_GAMEFRONTERR_DDRAW), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }

    if ((soundSetup(hInst, appWnd)) == FALSE) {
//      MessageBox(NULL, langGetText(STR_GAMEFRONTERR_DSOUND), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }

    if ((inputSetup(hInst, appWnd)) == FALSE) {
//      MessageBox(NULL, langGetText(STR_GAMEFRONTERR_DINPUT), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }

    if ((cursorSetup(hInst, appWnd)) == FALSE) {
//      MessageBox(NULL, langGetText(STR_GAMEFRONTERR_CURSOR), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }

    if ((fontSetup(hInst, appWnd)) == FALSE) {
//      MessageBox(NULL, langGetText(STR_GAMEFRONTERR_FONTS), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      OKStart = FALSE;
    }
  }

  /* Quit Program if start up failed */
  if (OKStart == FALSE) {
//    gameFrontEnd(hInst, appWnd, keys, FALSE, TRUE);
    exit(0);
  }

//  gameFrontWnd = appWnd;

  gameFrontDialogs(hInst); /* Process starting dialogs */

  screenSetAiType(compTanks);
  /* Set the key auto preferences up in the backend */

  if (isTutorial == FALSE) {
    screenSetTankAutoSlowdown(useAutoslow);
    screenSetTankAutoHideGunsight(useAutohide);
  }

  dlgState = openStart;
  dlgState = openStart;
  if (strncmp(fileName, "winbolo://", strlen("winbolo://")) == 0) {
    if (strcmp(fileName, "winbolo:///") != 0) {
      /* We clicked on a link */
      gameFrontSetAddressFromWebLink(fileName);
      dlgState = openInternetManual;
     }
    fileName[0] = EMPTY_CHAR;
  }

  
//  dialogSetNameInGame(TRUE);
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
  DWORD dwWritten; 
  
  clientMutexWaitFor();
//  ShowWindow(hWnd, FALSE);
  if (gamePlayed == TRUE) {
    useAutoslow = screenGetTankAutoSlowdown();
    useAutohide = screenGetTankAutoHideGunsight();
  }
//  brainsHandlerShutdown(hWnd);
  screenDestroy();
  netDestroy();
  gameFrontPutPrefs(keys);
  if (isQuiting == TRUE) {
    drawCleanup();
    soundCleanup();
    inputCleanup();
    cursorCleanup(hInst);
    fontCleanup();
//    DestroyWindow(hWnd);
//    UnregisterClass(WIND_CLASSNAME, hInst);
//    UnregisterClass(WIND_KEYCLASSNAME, hInst);
    langCleanup();
  }
/*  if (isServer == TRUE) {
    /* Quit server *
    DWORD code;
    GetExitCodeProcess(pi.hProcess, &code);
    if (code != 1) {
      /* Shut it down *
      if (WriteFile(hChildStdinWrDup, "quit\n", 5, &dwWritten, NULL) == FALSE) {
        DWORD d = GetLastError();
        MessageBox(NULL, "Thats not so good", "b", MB_OK);
      }
    }

  } */
  clientMutexRelease();

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

  done = FALSE; 
  msg.wParam = 0; /* In case something goes horribly wrong */
  ShowWindow(appWnd, nCmdShow);
  /* Start up messaging */
  while (done == FALSE) {
  }

/*  ShowWindow(appWnd, SW_HIDE);
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
  } */
  
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
      DialogBox(appInst, MAKEINTRESOURCE(IDD_OPENING), NULL, dialogOpeningCallback);
      break;
    case openLang:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_LANGUAGES), NULL, dialogLanguagesCallback);
      break;
    case openUdp:
    case openInternetManual:
    case openLanManual:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_TCPIP_SETUP), NULL, dialogUdpSetupCallback);
      break;
    case openSetup:
    case openInternetSetup:
    case openLanSetup:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_GAME_SETUP), NULL, dialogGameSetupCallback);
      break;
    case openUdpSetup:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_GAME_SETUP), NULL, dialogGameSetupCallback);
      break;
    case openInternet:
      /* Game Finder */
      dialogGameFinderSetMethod(langGetText(STR_GAMEFRONT_TRACKERFINDER_TITLE), TRUE);
      DialogBox(appInst, MAKEINTRESOURCE(IDD_GAMEFINDER), NULL, dialogGameFinderCallback);
      break;
    case openLan:
      /* Lan Game Finder */
      dialogGameFinderSetMethod(langGetText(STR_GAMEFRONT_LANFINDER_TITLE), FALSE);
      DialogBox(appInst, MAKEINTRESOURCE(IDD_GAMEFINDER), NULL, dialogGameFinderCallback);
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
    if (netSetup(netUdp, gameFrontMyUdp, gameFrontUdpAddress, gameFrontTargetUdp, password, FALSE, gameFrontTrackerAddr, gameFrontTrackerPort, gameFrontTrackerEnabled, wantRejoin) == FALSE) {
      wantRejoin = FALSE;
      MessageBox(NULL, langGetText(STR_GAMEFRONTERR_JOINGAME), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
      netDestroy();
      screenDestroy();
      returnValue = FALSE;
    } else {
      dlgState = openFinished;
    }
    SetCursor(LoadCursor(NULL, IDC_ARROW));
  } else if (dlgState == openInternetSetup && newState == openWelcome) {
    dlgState = openInternet;
  } else if (dlgState == openLanSetup && newState == openWelcome) {
    dlgState = openLan;
  } else if (dlgState == openLanManual && newState == openWelcome) {
    dlgState = openLan;
  } else if (dlgState == openInternetManual && newState == openWelcome) {
    dlgState = openInternet;
  } else if ((dlgState == openUdpSetup || dlgState == openInternetSetup || dlgState == openLanSetup) && newState == openFinished) {
    /* Start network game */
    dlgState = newState;
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    if (gameFrontSetupServer() == TRUE) {
      screenSetup(0, FALSE, 0, UNLIMITED_GAME_TIME);
      if (netSetup(netUdp, gameFrontMyUdp, "127.0.0.1", gameFrontTargetUdp, password, FALSE, gameFrontTrackerAddr, gameFrontTrackerPort, gameFrontTrackerEnabled, wantRejoin) == FALSE) {
        wantRejoin = FALSE;
        MessageBox(NULL, langGetText(STR_GAMEFRONTERR_JOINGAME), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
        netDestroy();
        screenDestroy();
//        dlgState = openStart;
        returnValue = FALSE;
      } else {
        dlgState = openFinished;
      }
     } else {
        MessageBox(NULL, langGetText(STR_GAMEFRONTERR_SPAWNSERVER), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
        dlgState = openStart;
     }
    SetCursor(LoadCursor(NULL, IDC_ARROW));
  } else if (dlgState == openSetup && newState == openFinished) {
    dlgState = openFinished;
    if (netSetup(netSingle, gameFrontMyUdp, gameFrontUdpAddress, gameFrontTargetUdp, password, FALSE, gameFrontTrackerAddr, gameFrontTrackerPort, gameFrontTrackerEnabled, wantRejoin) == FALSE) {
      wantRejoin = FALSE;
      MessageBox(NULL, langGetText(STR_GAMEFRONTERR_NETSINGLEPLAYER), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
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
  DialogBox(windowGetInstance(), MAKEINTRESOURCE(IDD_PASSWORD), NULL, dialogPasswordCallback);
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
  GetPrivateProfileString("SETTINGS", "Player Name", def, gameFrontName, FILENAME_MAX, PREFERENCE_FILE);
 
  /* Target Address */
  def[0] = '\0';
  GetPrivateProfileString("SETTINGS", "Target Address", def, gameFrontUdpAddress, FILENAME_MAX, PREFERENCE_FILE);
  
  /* Target UDP Port */
  itoa(DEFAULT_UDP_PORT, def, 10);
  GetPrivateProfileString("SETTINGS", "Target UDP Port", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontMyUdp = atoi(buff);
  gameFrontTargetUdp = atoi(buff);
  /* My UDP Port */
  itoa(DEFAULT_UDP_PORT, def, 10);
  GetPrivateProfileString("SETTINGS", "UDP Port", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontMyUdp = atoi(buff);

  /* Keys */
  itoa(DEFAULT_FORWARD, def, 10);
  GetPrivateProfileString("KEYS", "Forward", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiForward = atoi(buff);
  itoa(DEFAULT_BACKWARD, def, 10);
  GetPrivateProfileString("KEYS", "Backwards", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiBackward = atoi(buff);
  itoa(DEFAULT_LEFT, def, 10);
  GetPrivateProfileString("KEYS", "Left", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiLeft = atoi(buff);
  itoa(DEFAULT_RIGHT, def, 10);
  GetPrivateProfileString("KEYS", "Right", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiRight = atoi(buff);
  itoa(DEFAULT_SHOOT, def, 10);
  GetPrivateProfileString("KEYS", "Shoot", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiShoot = atoi(buff);
  itoa(DEFAULT_LAY_MINE, def, 10);
  GetPrivateProfileString("KEYS", "Lay Mine", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiLayMine = atoi(buff);
  itoa(DEFAULT_SCROLL_GUNINCREASE, def, 10);
  GetPrivateProfileString("KEYS", "Increase Range", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiGunIncrease = atoi(buff);
  itoa(DEFAULT_SCROLL_GUNDECREASE, def, 10);
  GetPrivateProfileString("KEYS", "Decrease Range", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiGunDecrease = atoi(buff);
  itoa(DEFAULT_TANKVIEW, def, 10);
  GetPrivateProfileString("KEYS", "Tank View", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiTankView = atoi(buff);
  itoa(DEFAULT_PILLVIEW, def, 10);
  GetPrivateProfileString("KEYS", "Pill View", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiPillView = atoi(buff);
  itoa(DEFAULT_SCROLLUP, def, 10);
  GetPrivateProfileString("KEYS", "Scroll Up", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiScrollUp = atoi(buff);
  itoa(DEFAULT_SCROLLDOWN, def, 10);
  GetPrivateProfileString("KEYS", "Scroll Down", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiScrollDown = atoi(buff);
  itoa(DEFAULT_SCROLLLEFT, def, 10);
  GetPrivateProfileString("KEYS", "Scroll Left", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiScrollLeft = atoi(buff);
  itoa(DEFAULT_SCROLLRIGHT, def, 10);
  GetPrivateProfileString("KEYS", "Scroll Right", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  keys->kiScrollRight = atoi(buff);

  /* Remeber */
  GetPrivateProfileString("SETTINGS", "Remember Player Name", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontRemeber = YESNO_TO_TRUEFALSE(buff[0]);

  /* Game Options */
  GetPrivateProfileString("GAME OPTIONS", "Hidden Mines", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  hiddenMines = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("GAME OPTIONS", "Allow Computer Tanks", "0", buff, FILENAME_MAX, PREFERENCE_FILE);
  compTanks = atoi(buff);
  GetPrivateProfileString("GAME OPTIONS", "Game Type", "1", buff, FILENAME_MAX, PREFERENCE_FILE);
  gametype = atoi(buff);
  GetPrivateProfileString("GAME OPTIONS", "Start Delay", "0", buff, FILENAME_MAX, PREFERENCE_FILE);
  startDelay = atoi(buff);
  ltoa(UNLIMITED_GAME_TIME, def, 10);
  GetPrivateProfileString("GAME OPTIONS", "Time Length", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  timeLen = atol(buff);
  GetPrivateProfileString("GAME OPTIONS", "Auto Slowdown", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  *useAutoslow = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("GAME OPTIONS", "Auto Show-Hide Gunsight", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  *useAutohide = YESNO_TO_TRUEFALSE(buff[0]);
    
    /* Tracker options */
  GetPrivateProfileString("TRACKER", "Address", TRACKER_ADDRESS, gameFrontTrackerAddr, FILENAME_MAX, PREFERENCE_FILE);
  itoa(TRACKER_PORT, def, 10);
  GetPrivateProfileString("TRACKER", "Port", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontTrackerPort = atoi(buff);
  GetPrivateProfileString("TRACKER", "Enabled", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  gameFrontTrackerEnabled = YESNO_TO_TRUEFALSE(buff[0]);



  /* Menu Items */
  itoa(FRAME_RATE_30, def, 10);
  GetPrivateProfileString("MENU", "Frame Rate", def, buff, FILENAME_MAX, PREFERENCE_FILE);
  frameRate = atoi(buff);
  GetPrivateProfileString("MENU", "Show Gunsight", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  showGunsight = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Sound Effects", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  soundEffects = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Allow Background Sound", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  backgroundSound = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "ISA Sound Card", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  isISASoundCard = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Show Gunsight", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  showGunsight = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Show Newswire Messages", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  showNewswireMessages = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Show Assistant Messages", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  showAssistantMessages = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Show AI Messages", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  showAIMessages = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Show Network Status Messages", "Yes", buff, FILENAME_MAX, PREFERENCE_FILE);
  showNetworkStatusMessages = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Show Network Debug Messages", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  showNetworkDebugMessages = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Autoscroll Enabled", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  autoScrollingEnabled = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Show Pill Labels", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  showPillLabels = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Show Base Labels", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  showBaseLabels = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Label Own Tank", "No", buff, FILENAME_MAX, PREFERENCE_FILE);
  labelSelf = YESNO_TO_TRUEFALSE(buff[0]);
  GetPrivateProfileString("MENU", "Window Size", "1", buff, FILENAME_MAX, PREFERENCE_FILE);
  zoomFactor = atoi(buff);
  GetPrivateProfileString("MENU", "Message Label Size", "1", buff, FILENAME_MAX, PREFERENCE_FILE);
  labelMsg = atoi(buff);
  GetPrivateProfileString("MENU", "Tank Label Size", "1", buff, FILENAME_MAX, PREFERENCE_FILE);
  labelTank = atoi(buff);
  

  /* Load in the language */
  GetPrivateProfileString("SETTINGS", "Language", "", buff, FILENAME_MAX, PREFERENCE_FILE);
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
  char buff[FILENAME_MAX];          /* Read Buffer               */

  /* Player Name */
  if ((netGetType() == netSingle || gameFrontRemeber == TRUE) && dlgState != openSetup) {
    screenGetPlayerName(playerName);
    WritePrivateProfileString("SETTINGS", "Player Name", playerName, PREFERENCE_FILE);
  } else {
    WritePrivateProfileString("SETTINGS", "Player Name", gameFrontName, PREFERENCE_FILE);
  }

  /* Target Address */
  WritePrivateProfileString("SETTINGS", "Target Address", gameFrontUdpAddress, PREFERENCE_FILE);
  
  /* Target UDP Port */
  itoa(gameFrontTargetUdp, buff, 10);
  WritePrivateProfileString("SETTINGS", "Target UDP Port", buff, PREFERENCE_FILE);
  /* My UDP Port */
  itoa(gameFrontMyUdp, buff, 10);
  WritePrivateProfileString("SETTINGS", "UDP Port", buff, PREFERENCE_FILE);
  /* Languages */
  langGetFileName(buff);
  WritePrivateProfileString("SETTINGS", "Language", buff, PREFERENCE_FILE);

  /* Keys */
  itoa(keys->kiForward, buff, 10);
  WritePrivateProfileString("KEYS", "Forward", buff, PREFERENCE_FILE);
  itoa(keys->kiBackward, buff, 10);
  WritePrivateProfileString("KEYS", "Backwards", buff, PREFERENCE_FILE);
  itoa(keys->kiLeft, buff, 10);
  WritePrivateProfileString("KEYS", "Left", buff, PREFERENCE_FILE);
  itoa(keys->kiRight, buff, 10);
  WritePrivateProfileString("KEYS", "Right", buff, PREFERENCE_FILE);
  itoa(keys->kiShoot, buff, 10);
  WritePrivateProfileString("KEYS", "Shoot", buff, PREFERENCE_FILE);
  itoa(keys->kiLayMine, buff, 10);
  WritePrivateProfileString("KEYS", "Lay Mine", buff, PREFERENCE_FILE);
  itoa(keys->kiGunIncrease, buff, 10);
  WritePrivateProfileString("KEYS", "Increase Range", buff, PREFERENCE_FILE);
  itoa(keys->kiGunDecrease, buff, 10);
  WritePrivateProfileString("KEYS", "Decrease Range", buff, PREFERENCE_FILE);
  itoa(keys->kiTankView, buff, 10);
  WritePrivateProfileString("KEYS", "Tank View", buff, PREFERENCE_FILE);
  itoa(keys->kiPillView, buff, 10);
  WritePrivateProfileString("KEYS", "Pill View", buff, PREFERENCE_FILE);
  itoa(keys->kiScrollUp, buff, 10);
  WritePrivateProfileString("KEYS", "Scroll Up", buff, PREFERENCE_FILE);
  itoa(keys->kiScrollDown, buff, 10);
  WritePrivateProfileString("KEYS", "Scroll Down", buff, PREFERENCE_FILE);
  itoa(keys->kiScrollLeft, buff, 10);
  WritePrivateProfileString("KEYS", "Scroll Left", buff, PREFERENCE_FILE);
  itoa(keys->kiScrollRight, buff, 10);
  WritePrivateProfileString("KEYS", "Scroll Right", buff, PREFERENCE_FILE);
  keys->kiMouseScrollHorz = 30; /* CJL add */

  /* Remember */
  WritePrivateProfileString("SETTINGS", "Remember Player Name", TRUEFALSE_TO_STR(gameFrontRemeber), PREFERENCE_FILE);

  /* Options */
  WritePrivateProfileString("GAME OPTIONS", "Hidden Mines", TRUEFALSE_TO_STR(hiddenMines), PREFERENCE_FILE);
  itoa(compTanks, buff, 10);
  WritePrivateProfileString("GAME OPTIONS", "Allow Computer Tanks", buff, PREFERENCE_FILE);
  compTanks = atoi(buff);
  itoa(gametype, buff, 10);
  WritePrivateProfileString("GAME OPTIONS", "Game Type", buff, PREFERENCE_FILE);
  ltoa(startDelay, buff, 10);
  WritePrivateProfileString("GAME OPTIONS", "Start Delay", buff, PREFERENCE_FILE);
  ltoa(timeLen, buff, 10);
  WritePrivateProfileString("GAME OPTIONS", "Time Length", buff, PREFERENCE_FILE);
  WritePrivateProfileString("GAME OPTIONS", "Auto Slowdown", TRUEFALSE_TO_STR(useAutoslow), PREFERENCE_FILE);
  WritePrivateProfileString("GAME OPTIONS", "Auto Show-Hide Gunsight", TRUEFALSE_TO_STR(useAutohide), PREFERENCE_FILE);
    
  /* Tracker */
  WritePrivateProfileString("TRACKER", "Address", gameFrontTrackerAddr, PREFERENCE_FILE);
  itoa(gameFrontTrackerPort, buff, 10);
  WritePrivateProfileString("TRACKER", "Port", buff, PREFERENCE_FILE);
  WritePrivateProfileString("TRACKER", "Enabled", TRUEFALSE_TO_STR(gameFrontTrackerEnabled), PREFERENCE_FILE);

  /* Menu Items */
  itoa(frameRate, buff, 10);
  WritePrivateProfileString("MENU", "Frame Rate", buff, PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Show Gunsight", TRUEFALSE_TO_STR(showGunsight), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Sound Effects", TRUEFALSE_TO_STR(soundEffects), PREFERENCE_FILE);

  WritePrivateProfileString("MENU", "Allow Background Sound", TRUEFALSE_TO_STR(backgroundSound), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "ISA Sound Card", TRUEFALSE_TO_STR(isISASoundCard), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Show Gunsight", TRUEFALSE_TO_STR(showGunsight), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Show Newswire Messages", TRUEFALSE_TO_STR(showNewswireMessages), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Show Assistant Messages", TRUEFALSE_TO_STR(showAssistantMessages), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Show AI Messages", TRUEFALSE_TO_STR(showAIMessages), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Show Network Status Messages", TRUEFALSE_TO_STR(showNetworkStatusMessages), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Show Network Debug Messages", TRUEFALSE_TO_STR(showNetworkDebugMessages), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Autoscroll Enabled", TRUEFALSE_TO_STR(autoScrollingEnabled), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Show Pill Labels", TRUEFALSE_TO_STR(showPillLabels), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Show Base Labels", TRUEFALSE_TO_STR(showBaseLabels), PREFERENCE_FILE);
  WritePrivateProfileString("MENU", "Label Own Tank", TRUEFALSE_TO_STR(labelSelf), PREFERENCE_FILE);
  itoa(zoomFactor, buff, 10);
  WritePrivateProfileString("MENU", "Window Size", buff, PREFERENCE_FILE);
  itoa(labelMsg, buff, 10);
  WritePrivateProfileString("MENU", "Message Label Size", buff, PREFERENCE_FILE);
  itoa(labelTank, buff, 10);
  WritePrivateProfileString("MENU", "Tank Label Size", buff, PREFERENCE_FILE);
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
  bool returnValue;           /* Value to return                 */
  char cmdLine[1024]; /* Command Line                   */
  char tmp[FILENAME_MAX];     /* Temp String */
  SECURITY_ATTRIBUTES sa;     /* Security attirubtes of the pipe */
  BOOL ret;                   /* Function return Value           */
  STARTUPINFO si;             /* Startup information    */
  long l;
 
  MessageBox(NULL, langGetText(STR_GAMEFRONT_SERVERSTARTMSG), DIALOG_BOX_TITLE, MB_ICONINFORMATION);

  returnValue = TRUE;
  cmdLine[0] = EMPTY_CHAR;

  strcat(cmdLine, " -map ");
  /* Make the command line */
  if (strcmp(fileName, "") != 0 ) {
    strcat(cmdLine, "\"");
    strcat(cmdLine, fileName);
    strcat(cmdLine, "\"");
  } else {
    strcat(cmdLine, "-inbuilt");
  }
  strcat(cmdLine, " " );
  strcat(cmdLine, "-port ");
  sprintf(tmp, "%d ", gameFrontTargetUdp);
  strcat(cmdLine, tmp);
  strcat(cmdLine, " -gametype ");
  switch (gametype) {
  case gameOpen:
    strcat(cmdLine, "open ");
    break;
  case gameTournament:
    strcat(cmdLine, "tournament ");
    break;
  case gameStrictTournament:
    strcat(cmdLine, "strict ");
    break;
  }
  strcat(cmdLine, " -mines ");
  if (hiddenMines == TRUE) {
    strcat(cmdLine, "yes ");
  } else {
    strcat(cmdLine, "no ");
  } 

  strcat(cmdLine, " -ai ");
  switch (compTanks) {
  case aiNone:
    strcat(cmdLine, "no ");
    break;
  case aiYes:
    strcat(cmdLine, "yes ");
    break;
  default:
    /* Yes with an advantage */
    strcat(cmdLine, "yesAdv ");
    break;
  }
  strcat(cmdLine, " -delay ");
  if (startDelay > 0) {
    l = startDelay;
    l /= GAME_NUMGAMETICKS_SEC;
  } else {
    l = 0;
  }
  sprintf(tmp, "%d ", l);
  strcat(cmdLine, tmp);

  strcat(cmdLine, " -limit ");
  if (timeLen > 0) {
    l = timeLen;
    l /= NUM_SECONDS;
    l /= GAME_NUMGAMETICKS_SEC;
  } else {
    l = timeLen;
  }
  sprintf(tmp, "%d ", l);
  strcat(cmdLine, tmp);
  if (gameFrontTrackerEnabled == TRUE && dlgState != openLanSetup) {
    /* Add on the tracker info */
    strcat(cmdLine, "-tracker ");
    sprintf(tmp, "%s:%d ", gameFrontTrackerAddr, gameFrontTrackerPort);
    strcat(cmdLine, tmp);
  }
  if (password[0] != '\0') {
    strcat(cmdLine, " -password ");
    strcat(cmdLine, password);
  }
  strcpy(tmp, "WinBoloDs.exe ");
  strcat(tmp, cmdLine);
  
  /* Set the bInheritHandle flag so pipe handles are inherited. */
  ZeroMemory(&sa, sizeof(sa));
  sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
  sa.bInheritHandle = TRUE; 
  sa.lpSecurityDescriptor = NULL; 

  ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
  ZeroMemory( &si, sizeof(STARTUPINFO) );
  si.cb = sizeof(STARTUPINFO); 
  si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  si.wShowWindow = SW_HIDE;


   hSaveStdin = GetStdHandle(STD_INPUT_HANDLE); 
 
// Create a pipe for the child process's STDIN. 
 
   if (CreatePipe(&hChildStdinRd, &hChildStdinWr, &sa, 0) == 0) {
     returnValue = FALSE; //ErrorExit("Stdin pipe creation failed\n"); 
   }
 
// Set a read handle to the pipe to be STDIN. 
 
   if (SetStdHandle(STD_INPUT_HANDLE, hChildStdinRd) == 0) {
      returnValue = FALSE;// ("Redirecting Stdin failed"); 
   } 
 
 //Duplicate the write handle to the pipe so it is not inherited. 
 
   ret = DuplicateHandle(GetCurrentProcess(), hChildStdinWr, GetCurrentProcess(), &hChildStdinWrDup, 0, FALSE, DUPLICATE_SAME_ACCESS); //FALSE
   if (ret == 0) {
     returnValue = FALSE; //ErrorExit("DuplicateHandle failed"); 
   }
 
   CloseHandle(hChildStdinWr); 
 

   if (returnValue == FALSE) {
     return FALSE;
   } 


  si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  si.hStdError =  GetStdHandle(STD_OUTPUT_HANDLE); 
  si.hStdInput = hChildStdinRd;

  ret = CreateProcess(NULL, tmp, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
  if (ret == FALSE) {
    returnValue = FALSE;
  } else {
    DWORD a;
    isServer = TRUE;
    Sleep(2000);
    GetExitCodeProcess(pi.hProcess, &a);
    if (a != STILL_ACTIVE) {
      returnValue = FALSE;
    }
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
*CREATION DATE: 1/5/00
*LAST MODIFIED: 1/5/00
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

  returnValue = FALSE;
  res = FindResource(windowGetInstance(), MAKEINTRESOURCE(IDR_EVERARD), "MAPS");
  if (res != NULL) {
    hGlobal = LoadResource(NULL, res);
    if (hGlobal != NULL) {
      buff = LockResource(hGlobal);
      if (buff != NULL) {
        returnValue = screenLoadCompressedMap(buff, 5097, "Everard Island", gametype, hiddenMines, startDelay, timeLen, gameFrontName, FALSE); //
      }
    }
  }

  return returnValue;
}

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
bool gameFrontLoadTutorial() {
  bool returnValue; /* Value to return */
  HGLOBAL hGlobal;  /* Resource handle */
  BYTE *buff;       /* Byte buffer     */
  HRSRC res;        /* FindResource return */

  returnValue = FALSE;
  res = FindResource(windowGetInstance(), MAKEINTRESOURCE(IDR_TUTORIAL), "MAPS");
  if (res != NULL) {
    hGlobal = LoadResource(NULL, res);
    if (hGlobal != NULL) {
      buff = LockResource(hGlobal);
      if (buff != NULL) {
        returnValue = screenLoadCompressedMap(buff, 4182, "Inbuilt Tutorial", gameStrictTournament, FALSE, 0, -1, langGetText(STR_DLGGAMESETUP_DEFAULTNAME), FALSE);
        if (returnValue == TRUE) {
          netSetup(netSingle, gameFrontMyUdp, gameFrontUdpAddress, gameFrontTargetUdp, password, FALSE, gameFrontTrackerAddr, gameFrontTrackerPort, gameFrontTrackerEnabled, wantRejoin);
        }
      }
    }
  }

  return returnValue;
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


