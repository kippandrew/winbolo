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
*Filename:      winbolo.c
*Author:        John Morrison
*Creation Date: 31/10/98
*Last Modified: 03/02/03
*Purpose:
*  Provides the front end for viewing maps
*********************************************************/


#include <windows.h>
#include <string.h>
#include <commdlg.h>
#include <mmsystem.h>
#include <time.h>
#include "..\..\bolo\backend.h"
#include "..\..\bolo\frontend.h"
#include "..\draw.h"
#include "..\sound.h"
#include "..\input.h"
#include "..\cursor.h"
#include "..\font.h"
#include "..\positions.h"
#include "..\resource.h"
#include "..\dialogsysteminfo.h"
#include "..\dialoggameinfo.h"
#include "..\dialognetworkinfo.h"
#include "..\dialogkeysetup.h"
#include "..\dialogabout.h"
#include "..\dialogsetname.h"
#include "..\dialogmessages.h"
#include "..\gamefront.h"
#include "..\..\bolo\network.h"
#include "..\lang.h"
#include "..\brainshandler.h"
#include "..\clientmutex.h"
#include "..\winutil.h"
#include "..\winbolo.h"


void windowStartTutorial();
bool isTutorial;

/* Handle to the application Instance and window */
HINSTANCE appInst;
HWND appWnd;

/* Time since the game timer last went up */
DWORD oldTick;
DWORD ttick;
DWORD oldFrameTick;

/* The frame rate */
int frameRate = FRAME_RATE_30;
/* Time between frame updates based on Frame rate */
int frameRateTime = (int) (MILLISECONDS / FRAME_RATE_30) - 1;

/* Current building item selected */
buildSelect BsCurrent = BsTrees;

/* Whether the Gunsight is shown or not */
bool showGunsight = FALSE;

/* Whether the sound effects are turn on or not */
bool soundEffects = TRUE;
/* Do we play background sound */
bool backgroundSound = TRUE;

/* Is the sound card of the ISA variety */
bool isISASoundCard = TRUE;

/* 
 new players */
bool allowNewPlayers = TRUE;

/* 5 Message Shown Status */
bool showNewswireMessages = TRUE;
bool showAssistantMessages = TRUE;
bool showAIMessages = FALSE;
bool showNetworkStatusMessages = TRUE;
bool showNetworkDebugMessages = FALSE;


/* Automatic Scrolling or not */
bool autoScrollingEnabled = FALSE;

/* The Window scaling */
BYTE zoomFactor = ZOOM_FACTOR_NORMAL;

/* Whether Pillbox & base labels should be shown */
bool showPillLabels = FALSE;
bool showBaseLabels = FALSE;

/* Labels of stuff */
bool labelSelf = TRUE;
labelLen labelMsg = lblShort;
labelLen labelTank = lblShort;

/* The timer for game updates */
unsigned int timerGameID = 1;
unsigned int timerFrameID = 2;

/* Stuff for the system info dialog box */
DWORD dwSysFrameTotal = 0;
DWORD dwSysFrame = 0;
DWORD dwSysGameTotal = 0;
DWORD dwSysGame = 0;
DWORD dwSysBrainTotal = 0;
DWORD dwSysBrain = 0;

/* Key Settings */
keyItems keys;

/* Fix for breaking draw code with scrolling messages */
bool drawBusy = FALSE;

/* Hide the main view or not */
bool hideMainView = FALSE;

HWND msgWnd = NULL;
HWND keyWnd = NULL;
HWND sysWnd = NULL;
HWND netWnd = NULL;
HWND infoWnd = NULL;


/* Are we in a menu or not */
bool isInMenu;

bool doingTutorial;

/* Time to quit */
bool winboloQuit;

bool finishedLoop = FALSE;

void test();

time_t ticks = 0;

#include "../../bolo/network.h"

/*********************************************************
*NAME:          WinMain
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 4/1/00
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
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR szCmdLine, int nCmdShow) {
  HACCEL hAccel; /* Accelerator table */
  int timerFlush; /* Used to flush the timer */


  appInst = hInst;
  winboloQuit = FALSE;
  
/*time_t currTime; /* Current Time *
  time(&currTime);
  if (currTime > 943884002) {
    MessageBoxA(NULL, "This beta version of WinBolo has expired. Please download a more recent version", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    exit(0);
  } else {
    MessageBoxA(NULL, "NOTE: This beta version of WinBolo expires on the 30/11/99", DIALOG_BOX_TITLE, MB_ICONINFORMATION);
  }  */

  initWinboloTimer();

  hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR));
  if (clientMutexCreate() == FALSE) {
    MessageBoxA(NULL, langGetText(STR_WBERR_MUTEXCREATE), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    return 0;
  }

  appWnd = gameFrontStart(hInst, szCmdLine, nCmdShow, &keys, FALSE); /* Start Game */
  if (appWnd == NULL) {
    exit(0);
  }
  winboloQuit = FALSE;
  while (winboloQuit == FALSE) {
    isInMenu = FALSE;
    finishedLoop = FALSE;
    windowReCreate();
    windowApplyMenuChecks();
    inputActivate();

    /* Set up Timers */
    if (soundEffects == TRUE) {
      soundISASoundCard(isISASoundCard);
    }
    Sleep(500);
    oldTick = winboloTimer();
    oldFrameTick = oldTick;
    timerGameID = timeSetEvent(GAME_TICK_LENGTH, 10000, windowGameTimer, 0, TIME_PERIODIC);
    timerFrameID = timeSetEvent(frameRateTime, 10000, windowFrameRateTimer, 0, TIME_PERIODIC);
    winboloQuit = TRUE;
    finishedLoop = FALSE;
    gameFrontRun(hInst, appWnd, hAccel, nCmdShow);
    finishedLoop = TRUE;
  
    /* Kill Timers */
    timeKillEvent(timerGameID);
    timeKillEvent(timerFrameID);
    timerFlush = 0;
    while (timerFlush<10000) {
      timerFlush++;
    }
    clientMutexRelease();
    gameFrontEnd(hInst, appWnd, &keys, TRUE, winboloQuit); /* Shutdown game */
    doingTutorial = FALSE;  
    if (winboloQuit == FALSE) {
      gameFrontStart(hInst, szCmdLine, nCmdShow, &keys, TRUE); /* Start Game */
    }
  }

  endWinboloTimer();
  clientMutexDestroy();

/*  dbg = _CrtDumpMemoryLeaks() ;
  if (dbg != FALSE) {
    dbg = 1;
  }    */
  return 0;
}

/*********************************************************
*NAME:          windowCreate
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 16/12/98
*PURPOSE:
*  Sets up the main window.
*
*ARGUMENTS:
*  hInst     - Handle to the app instance
*  nCmdShow  - Window State on start up
*********************************************************/
HWND windowCreate(HINSTANCE hInst, int nCmdShow) {
  HWND returnValue; /* Value to return */
  WNDCLASSA wc;      /* Window Class to create */
  WNDCLASSA wc2;

  /* Set up the Window Class */
  wc.hCursor        = LoadCursor(0,IDC_ARROW);//NULL;
  wc.hIcon          = LoadIcon(hInst, MAKEINTRESOURCE(IDI_BOLO));
  wc.lpszMenuName   = NULL;
  wc.lpszClassName  = WIND_CLASSNAME;
  wc.hbrBackground  = 0;
  wc.hInstance      = hInst;
  wc.style          = 0;
  wc.lpfnWndProc    = ExWindowProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = 0;
  wc.hbrBackground  = 0;

  if (RegisterClassA(&wc)) {
    returnValue = CreateWindowA(
     WIND_CLASSNAME,                 /* Class name */
     WIND_TITLE,                     /* Caption */ 
    (WS_OVERLAPPED + WS_CAPTION + WS_SYSMENU + WS_MINIMIZEBOX), /* Style */
     CW_USEDEFAULT, CW_USEDEFAULT,   /* Position */
     zoomFactor * TOTAL_WINDOW_SIZE_X + 4, zoomFactor * TOTAL_WINDOW_SIZE_Y + 42,   /* Size */
     NULL,							             /* No parent */
     langGetMenu(),                           /* Menu */
     hInst,                          /* inst handle */
     0                               /* no params */
     );
  } else {
    returnValue = NULL;
  }
  appWnd = returnValue;
  if (appWnd != NULL) {
    if (brainsHandlerLoadBrains(appWnd) == FALSE) {
      MessageBoxA(NULL, langGetText(STR_WBERR_BRAINLISTLOAD), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    }
  } else {
	SetWindowTextA(appWnd, "WinBolo");
  }

  wc2.hCursor        = LoadCursor(0,IDC_ARROW);
  wc2.hIcon          = LoadIcon(hInst, MAKEINTRESOURCE(IDI_BOLO));
  wc2.lpszMenuName   = NULL;
  wc2.lpszClassName  = WIND_KEYCLASSNAME;
  wc2.hbrBackground  = 0;
  wc2.hInstance      = hInst;
  wc2.style          = 0;
  wc2.lpfnWndProc    = GetKeyProc;
  wc2.cbClsExtra     = 0;
  wc2.cbWndExtra     = 0;
  wc2.hbrBackground  = 0;
  
  /* Setup the Get Key window */
  
  if ((RegisterClassA(&wc2)) == 0) {
    MessageBoxA(NULL, langGetText(STR_WBERR_KEYCLASSSETUP), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
  }
  return returnValue;
}

/*********************************************************
*NAME:          ExWindowProc
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED:  14/6/00
*PURPOSE:
*  The main window call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More message parameters
*********************************************************/
LRESULT CALLBACK ExWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  static bool doneInitTutorial = FALSE;
  RECT rcWindow;
  int xPos;
  int yPos;
  
  appWnd = hWnd;
  switch (msg) {
  case WM_ACTIVATE:
    if (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) {
      drawRestore();
      if (GetClientRect( appWnd, &rcWindow )) {
        if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
          cursorAcquireCursor(appInst, rcWindow);
          if (doneInitTutorial == FALSE && isTutorial == TRUE) {
            doneInitTutorial = TRUE;
            windowStartTutorial();
          }
        }
      }
    } else {
      cursorLeaveWindow();
    }
    break;
  case WM_KILLFOCUS:
    cursorLeaveWindow();
    break;
  case WM_PAINT:
    /* Redraw the window */
    if (GetClientRect( appWnd, &rcWindow )) {
      if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
        cursorMove(appInst, appWnd, rcWindow);
        clientMutexWaitFor();
        drawRedrawAll(appInst, appWnd, BsCurrent, &rcWindow, showPillLabels, showBaseLabels);
        clientMutexRelease();
      }
    }
    break;
  case WM_KEYDOWN:
    /* Keys Pressed - Test for scroll keys*/
     windowKeyPressed((int) wParam);
     break;
  case WM_DESTROY:
    /* Quit time */
    PostQuitMessage(0);
    break;
  case WM_ENTERMENULOOP:
    isInMenu = TRUE;
    break;
  case WM_EXITMENULOOP:
    isInMenu = FALSE;
    break;
  case WM_MOUSEMOVE:
    {
    BYTE cursorX, cursorY;
    if (GetClientRect(appWnd, &rcWindow )) {
      if (ClientToScreen(appWnd, ( LPPOINT )&rcWindow )) {
        if (cursorPos(&rcWindow, &cursorX, &cursorY) == TRUE) {
          if (cursorX >16  || cursorY > 16) {
            cursorX = 100;
          }

          screenSetCursorPos(cursorX, cursorY);
        }
      }
    }
    }
    break;
  case WM_LBUTTONDOWN:
    xPos = LOWORD(lParam);  // horizontal position of cursor 
    yPos = HIWORD(lParam);  // vertical position of cursor 
    if (GetClientRect( appWnd, &rcWindow )) {
      if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
        windowMouseClick(rcWindow.left, rcWindow.top, xPos, yPos);
      }
    }
    break;
  case WM_DISPLAYCHANGE:
    clientMutexWaitFor();
    drawCleanup();
    Sleep(100);
    drawSetup(appInst, appWnd);
    clientMutexRelease();
    if (GetClientRect( appWnd, &rcWindow )) {
      if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
        clientMutexWaitFor();
        drawRedrawAll(appInst, appWnd, BsCurrent, &rcWindow, showPillLabels, showBaseLabels);
        clientMutexRelease();
      }
    }
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case ID_FILEMENU_NEW:
      winboloQuit = FALSE;
      PostQuitMessage(0);
      break;
    case ID_FILEMENU_SAVEMAP:
      windowSaveMap();
      break;
    case ID_FILEMENU_GAMEINFO:
      /* Game Info Dialog Box */
      windowShowGameInfo(wsrOpen);
      break;
    case ID_FILEMENU_SYSTEMINFO:
      /* System Info Dialog Box */
      windowShowSysInfo(wsrOpen);
      break;
    case ID_FILEMENU_NETWORKINFO:
      windowShowNetInfo(wsrOpen);
      break;
    case ID_FILEMENU_EXIT:
      PostQuitMessage(0);
      break;
    case ID_EDITMENU_GUNSIGHT:
      /* Gunsight Show/Hide Clicked */
      windowShowGunsight(hWnd);
      break;
    case ID_EDITMENU_AUTOMATICSCROLLING:
      windowAutomaticScrolling(hWnd);
      break;
    case ID_EDITMENU_PILLLABELS:
      windowShowPillLabels(hWnd);
      break;
    case ID_EDITMENU_BASELABELS:
      windowShowBaseLabels(hWnd);
      break;
    case ID_FRAMERATEMENU_10:
      windowSetFrameRate(hWnd, FRAME_RATE_10, TRUE);
      break;
    case ID_FRAMERATEMENU_12:
      windowSetFrameRate(hWnd, FRAME_RATE_12, TRUE);
      break;
    case ID_FRAMERATEMENU_15:
      windowSetFrameRate(hWnd, FRAME_RATE_15, TRUE);
      break;
    case ID_FRAMERATEMENU_20:
      windowSetFrameRate(hWnd, FRAME_RATE_20, TRUE);
      break;
    case ID_FRAMERATEMENU_30:
      windowSetFrameRate(hWnd, FRAME_RATE_30, TRUE);
      break;
    case ID_FRAMERATEMENU_50:
      windowSetFrameRate(hWnd, FRAME_RATE_50, TRUE);
      break;
    case ID_FRAMERATEMENU_60:
      windowSetFrameRate(hWnd, FRAME_RATE_60, TRUE);
      break;
    case ID_EDIT_MESSAGESENDERNAMES_SHORT:
      windowSetMessageLabelLen(hWnd, lblShort);
      break;
    case ID_EDIT_MESSAGESENDERNAMES_LONG:
      windowSetMessageLabelLen(hWnd, lblLong);
      break;
    case ID_EDIT_TANKLABELS_NONE:
      windowSetTankLabelLen(hWnd, lblNone);
      break;
    case ID_EDIT_TANKLABELS_SHORT:
      windowSetTankLabelLen(hWnd, lblShort);
      break;
    case ID_EDIT_TANKLABELS_LONG:
      windowSetTankLabelLen(hWnd, lblLong);
      break;
    case ID_EDIT_TANKLABELS_DONTLABELOWNTANK:
      windowLabelOwnTank(hWnd);
      break;
    case ID_EDITMENU_HIDEMAINVIEW:
      windowHideMainView(hWnd);
      break;
    case ID_BOLOMENU_ALLOWNEWPLAYERS:
      windowMenuAllowNewPlayers(hWnd);
      break;
    case ID_BOLOMENU_REQUESTALLIANCE:
      windowMenuAllowAllianceRequest(hWnd);
      break;
    case ID_BOLOMENU_SETKEYS:
      /* Set keys dialog box */
      windowShowSetKeys(wsrOpen);
      break;
    case ID_BOLOMENU_CHANGENAME:
      /* Set player name dialog box */
      windowShowSetPlayerName(wsrOpen);
      break;
    case ID_BOLOMENU_SOUNDEFFECTS:
      windowSoundEffects(hWnd);
      break;
    case ID_BOLOMENU_BACKGROUNDSOUND:
      windowBackgroundSoundChange(hWnd, TRUE);
      break;
    case ID_BOLOMENU_ISASOUNDCARD:
      windowISASoundCard();
      break;
    case ID_BOLOMENU_NEWSWIREMESSAGES:
      windowMenuNewswire(hWnd);
      break;
    case ID_BOLOMENU_ASSISTANTMESSAGES:
      windowMenuAssistant(hWnd);
      break;
    case ID_BOLOMENU_AIBRAINMESSAGES:
      windowMenuAI(hWnd);
      break;
    case ID_BOLOMENU_NETWORKSTATUSMESSAGES:
      windowMenuNetwork(hWnd);
      break;
    case ID_BOLOMENU_NETWORKDEBUGMESSAGES:
      windowMenuNetworkDebug(hWnd);
      break;
    case ID_BOLOMENU_REQUESTALLIENCE:
      screenRequestAlliance();
      break;
    case ID_BOLOMENU_LEAVEALLIENCE:
      screenLeaveAlliance();
      break;
    case ID_PLAYERSMENU_SENDMESSAGE:
      windowShowSendMessages(wsrOpen);
      break;
    case ID_PLAYERSMENU_SELECTALL:
      screenCheckAllNonePlayers(TRUE);
      break;
    case ID_PLAYERSMENU_SELECTNONE:
      screenCheckAllNonePlayers(FALSE);
      break;
    case ID_PLAYERSMENU_SELECTALLIES:
      screenCheckAlliedPlayers();
      break;
    case ID_PLAYERSMENU_SELECTNEARBYTANKS:
      screenCheckNearbyPlayers();
      break;
    case ID_PLAYERSMENU_01:
      screenTogglePlayerCheckState(player01);
      break;
    case ID_PLAYERSMENU_02:
      screenTogglePlayerCheckState(player02);
      break;
    case ID_PLAYERSMENU_03:
      screenTogglePlayerCheckState(player03);
      break;
    case ID_PLAYERSMENU_04:
      screenTogglePlayerCheckState(player04);
      break;
    case ID_PLAYERSMENU_05:
      screenTogglePlayerCheckState(player05);
      break;
    case ID_PLAYERSMENU_06:
      screenTogglePlayerCheckState(player06);
      break;
    case ID_PLAYERSMENU_07:
      screenTogglePlayerCheckState(player07);
      break;
    case ID_PLAYERSMENU_08:
      screenTogglePlayerCheckState(player08);
      break;
    case ID_PLAYERSMENU_09:
      screenTogglePlayerCheckState(player09);
      break;
    case ID_PLAYERSMENU_10:
      screenTogglePlayerCheckState(player10);
      break;
    case ID_PLAYERSMENU_11:
      screenTogglePlayerCheckState(player11);
      break;
    case ID_PLAYERSMENU_12:
      screenTogglePlayerCheckState(player12);
      break;
    case ID_PLAYERSMENU_13:
      screenTogglePlayerCheckState(player13);
      break;
    case ID_PLAYERSMENU_14:
      screenTogglePlayerCheckState(player14);
      break;
    case ID_PLAYERSMENU_15:
      screenTogglePlayerCheckState(player15);
      break;
    case ID_PLAYERSMENU_16:
      screenTogglePlayerCheckState(player16);
      break;
    case ID_HELP_HELP:
      winUtilOpenHelpFile(appInst, hWnd);
      break;
    case ID_HELPMENU_ABOUT:
      windowShowAboutBox();
      break;
    case ID_WINDOWSIZE_NORMAL:
      windowZoomChange(ZOOM_FACTOR_NORMAL);
      break;
    case ID_WINDOWSIZE_DOUBLE:
      windowZoomChange(ZOOM_FACTOR_DOUBLE);
      break;
    case ID_WINDOWSIZE_QUAD:
      windowZoomChange(ZOOM_FACTOR_QUAD);
      break;
    case ID_BRAINSMENU_MANUAL:
      brainsHandlerManual(hWnd);
      break;
    default:
      if (LOWORD (wParam) >= BRAINS_RESOURCE_OFFSET && LOWORD(wParam) <= (brainsHandlerGetNum() + BRAINS_RESOURCE_OFFSET)) {
        brainsHandlerItem(hWnd, LOWORD(wParam));
      } else {
        brainsHandlerBrainsItem(hWnd, LOWORD(wParam));
      }
    }
    break;
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*********************************************************
*NAME:          frontEndDrawMainScreen
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 18/11/99
*PURPOSE:
*  Updates the Window View
*
*ARGUMENTS:
*  value      - Pointer to the sceen structure
*  mineView   - Pointer to the screen mine structure
*  tks        - Pointer to the screen tank structure
*  gs         - pointer to the screen gunsight structure
*  lgms       - Pointer to the screen builder structure
*  srtDelay   - Start delay. If this is greater then 0
*               Then the delay screen should be drawn
*  isPillView - TRUE if we are in pillbox view
*  edgeX      - X Offset for smooth scrolling
*  edgeY      - Y Offset for smooth scrolling
*********************************************************/
void frontEndDrawMainScreen(screen *value, screenMines *mineView, screenTanks *tks, screenGunsight *gs, screenBullets *sBullet, screenLgm *lgms, long srtDelay, bool isPillView, int edgeX, int edgeY) {
  RECT rcWindow;
  static BYTE oldCursorX = 0;
  static BYTE oldCursorY = 0;

  if (hideMainView == FALSE && drawBusy == FALSE) {
    BYTE cursorX;
    BYTE cursorY;
    bool showCursor;
    /* get the client (drawing) rectangle. */
    if (GetClientRect( appWnd, &rcWindow )) {
      /* Convert the coordinates from client relative to screen relative. */
      if (ClientToScreen(appWnd, (LPPOINT) &rcWindow )) {
  //    showCursor = cursorPos(&rcWindow, &cursorX, &cursorY);
        showCursor = screenGetCursorPos(&cursorX, &cursorY);
        if (showCursor == TRUE && (oldCursorX != cursorX || oldCursorY != cursorY)) {
          oldCursorX = cursorX;
          oldCursorY = cursorY;
          cursorSetPos(rcWindow, cursorX, cursorY);
        } 

        drawMainScreen(value,mineView,tks, gs, sBullet, lgms, &rcWindow, showPillLabels, showBaseLabels, srtDelay, isPillView, edgeX, edgeY, showCursor, cursorX, cursorY);
      }
    }
  }
}

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
HINSTANCE windowGetInstance() {
  return appInst;
}

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
void windowShowAboutBox() {
  DialogBox(appInst, MAKEINTRESOURCE(IDD_ABOUT), appWnd, dialogAboutCallback);
}

/*********************************************************
*NAME:          windowGameTimer
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED:  8/1/00
*PURPOSE:
* The Game Timer. If there are no events to prcess this 
* routine is called. If the elapsed
*
*ARGUMENTS:
*
*********************************************************/
void CALLBACK windowGameTimer(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
  static bool inBrain = FALSE;  /* Are we allready calling the brain? */
  static bool justKeys = FALSE; /* Just the keys tick or whole game? */
  static BYTE t2 = 0;
  static int trackerTime = 11500;   /* When we should update the tracker */
  static int cheatCheck = 0;
  RECT rcWindow;
//  DWORD tick;     /* Number of ticks passed */
  tankButton tb;  /* Buttons being pressed */
  bool isShoot;   /* Whether the tank fire key is being pressed */
  bool used = FALSE;
  bool brainRunning; /* is a brain running */

  brainRunning = brainHandlerIsBrainRunning();
  isShoot = FALSE;
  tb = 0;



  ttick = winboloTimer();
  /* Update the game objects if required */
  if ((ttick - oldTick) > GAME_TICK_LENGTH) {
    /* Get the keyboard state */
    while ((ttick - oldTick) > GAME_TICK_LENGTH) {
      if (doingTutorial == FALSE) {
        if (justKeys == TRUE) {
          if (brainRunning == FALSE) {
            tb = inputGetKeys(appWnd, &keys, isInMenu);
          } else {
            inputScroll(appWnd, &keys, isInMenu);
          }

          clientMutexWaitFor();
          screenKeysTick(tb, brainRunning);
          clientMutexRelease();
          justKeys = FALSE;
        } else {
          t2++;
          trackerTime++;
          if (brainRunning == FALSE) {
            tb = inputGetKeys(appWnd, &keys, isInMenu);
            isShoot = inputIsFireKeyPressed(appWnd, &keys, isInMenu);
          } else {
            inputScroll(appWnd, &keys, isInMenu);
          }
          clientMutexWaitFor();
          screenGameTick(tb, isShoot, brainRunning);
          clientMutexRelease();
          ticks++;
          justKeys = TRUE;
          used = TRUE;
        }
        oldTick += GAME_TICK_LENGTH;
        if (oldTick > ttick) {
          oldTick = ttick;
        }

      }
    }
  } 
  dwSysGame += (winboloTimer() - ttick);

  if (used == TRUE) {
    netMakeDataPosPacket();
    netMakeTokenPacket();
  }

  /* AI */
  if (used == TRUE && inBrain == FALSE && brainRunning == TRUE && netGetStatus() != netFailed) {
    clientMutexWaitFor();
    inBrain = TRUE;
    clientMutexRelease();
    ttick = winboloTimer();
    brainHandlerRun(appWnd);
    dwSysBrain += winboloTimer() - ttick;
    clientMutexWaitFor();
    inBrain = FALSE;
    clientMutexRelease();
  } else if (used == TRUE) {
//    MessageBoxA(NULL, "e", "b", MB_OK);
  }

  if (t2 >= 19) {
    netSecond();
    dwSysFrameTotal = dwSysFrame;
    dwSysGameTotal = dwSysGame;
    dwSysBrainTotal = dwSysBrain;
    dwSysBrain = 0;
    dwSysFrame = 0;
    dwSysGame = 0;
    t2 = 0;
    winboloCC();
  }

  if (trackerTime >= (6000)) {
    netSendTrackerUpdate();
    trackerTime = 0;
  }

  ttick = winboloTimer();
  if (GetClientRect(appWnd, &rcWindow )) {
    if (ClientToScreen(appWnd, ( LPPOINT )&rcWindow )) {
      cursorMove(appInst, appWnd, rcWindow);
    }
  }
  dwSysFrame += (winboloTimer() - ttick);
}


/*********************************************************
*NAME:          windowFrameRateTimer
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 29/3/99
*PURPOSE:
* The Frame rate Timer has gone off. Update the screen.
*
*ARGUMENTS:
*
*********************************************************/
void CALLBACK windowFrameRateTimer(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
  DWORD tick;

  if (hideMainView == FALSE) {
    tick = winboloTimer();
    if ( (int) (tick - oldFrameTick) >= frameRateTime) {
      clientMutexWaitFor();
      if (finishedLoop == FALSE) {
        screenUpdate(redraw);
        clientMutexRelease();
      }
      oldFrameTick = winboloTimer();
    }
    dwSysFrame += (winboloTimer() - tick);
  }
}

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
void windowKeyPressed(int keyCode) {

/*  if (keyCode == keys.kiScrollUp) {
//     screenUpdate(up);
  } else if (keyCode == keys.kiScrollDown) {
//     screenUpdate(down);
  } else if (keyCode == keys.kiScrollLeft) {
//     screenUpdate(left);
  } else if (keyCode == keys.kiScrollRight) {
//     screenUpdate(right);
  } else if (keyCode == keys.kiGunIncrease) {
//     screenGunsightRange(TRUE);
  } else if (keyCode == keys.kiGunDecrease) {
//     screenGunsightRange(FALSE); */
/*  } else */if (keyCode == keys.kiTankView) {
     screenTankView();
  } else if (keyCode == keys.kiPillView) {
     screenPillView(0, 0);
  }
}

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
void windowSetFrameRate(HWND hWnd, int newFrameRate, bool setTimer) {
  HMENU hMenu; /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  
  /* Uncheck the old item */
  switch (frameRate) {
  case FRAME_RATE_10:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_10, MF_UNCHECKED);
    break;
  case FRAME_RATE_12:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_12, MF_UNCHECKED);
    break;
  case FRAME_RATE_15:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_15, MF_UNCHECKED);
    break;
  case FRAME_RATE_20:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_20, MF_UNCHECKED);
    break;
  case FRAME_RATE_30:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_30, MF_UNCHECKED);
    break;
  case FRAME_RATE_50:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_50, MF_UNCHECKED);
    break;
  case FRAME_RATE_60:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_60, MF_UNCHECKED);
    break;
  }
  /* Check the new item  */
  /* and set the number of times the item should be called a second */
  switch (newFrameRate) {
  case FRAME_RATE_10:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_10, MF_CHECKED);
    frameRateTime = (int) (MILLISECONDS / FRAME_RATE_10);
    break;
  case FRAME_RATE_12:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_12, MF_CHECKED);
    frameRateTime = (int) (MILLISECONDS / FRAME_RATE_12);
    break;
  case FRAME_RATE_15:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_15, MF_CHECKED);
    frameRateTime = (int) (MILLISECONDS / FRAME_RATE_15);
    break;
  case FRAME_RATE_20:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_20, MF_CHECKED);
    frameRateTime = (int) (MILLISECONDS / FRAME_RATE_20) - 1;
    break;
  case FRAME_RATE_30:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_30, MF_CHECKED);
    frameRateTime = (int) (MILLISECONDS / FRAME_RATE_30) - 1;
    break;
  case FRAME_RATE_50:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_50, MF_CHECKED);
    frameRateTime = (int) (MILLISECONDS / FRAME_RATE_50)-4;
    break;
  case FRAME_RATE_60:
    CheckMenuItem(hMenu, ID_FRAMERATEMENU_60, MF_CHECKED);
    frameRateTime = (int) (MILLISECONDS / FRAME_RATE_60)-4;
    break;
  }
  frameRate = newFrameRate;
  if (setTimer == TRUE) {
    timeKillEvent(timerFrameID);
    timerFrameID = timeSetEvent(frameRateTime, 10000, windowFrameRateTimer, 0, TIME_PERIODIC);
  }
}

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
void windowMouseClick(int xWin, int yWin, int xPos, int yPos) {
  DWORD tick;
  RECT rcWindow;         /* The window */
  BYTE xValue;           /* X & Y values if clicked inside main view*/
  BYTE yValue;
  buildSelect newSelect; /* The new selection */


  rcWindow.left = xWin;
  rcWindow.top = yWin;
  /* Check for inside Main View */
  if ((cursorPos(&rcWindow, &xValue, &yValue)) == TRUE) {
    clientMutexWaitFor();
    screenManMove(BsCurrent);
    clientMutexRelease();
  } else {
    newSelect = NO_SELECT;
    if (xPos >= (zoomFactor * BS_TREE_OFFSET_X) && xPos <= ((zoomFactor * BS_TREE_OFFSET_X) + (zoomFactor * BS_ITEM_SIZE_X)) && yPos >= (zoomFactor * BS_TREE_OFFSET_Y) && yPos <= ((zoomFactor * BS_TREE_OFFSET_Y) + (zoomFactor * BS_ITEM_SIZE_Y))) {
      newSelect = BsTrees;
    } else if (xPos >= (zoomFactor * BS_ROAD_OFFSET_X) && xPos <= ((zoomFactor * BS_ROAD_OFFSET_X) + (zoomFactor * BS_ITEM_SIZE_X)) && yPos >= (zoomFactor * BS_ROAD_OFFSET_Y) && yPos <= ((zoomFactor * BS_ROAD_OFFSET_Y) + (zoomFactor * BS_ITEM_SIZE_Y))) {
      newSelect = BsRoad;
    } else if (xPos >= (zoomFactor * BS_BUILDING_OFFSET_X) && xPos <= ((zoomFactor * BS_BUILDING_OFFSET_X) + (zoomFactor * BS_ITEM_SIZE_X)) && yPos >= (zoomFactor * BS_BUILDING_OFFSET_Y) && yPos <= ((zoomFactor * BS_BUILDING_OFFSET_Y) + (zoomFactor * BS_ITEM_SIZE_Y))) {
      newSelect = BsBuilding;
    } else if (xPos >= (zoomFactor * BS_PILLBOX_OFFSET_X) && xPos <= ((zoomFactor * BS_PILLBOX_OFFSET_X) + (zoomFactor * BS_ITEM_SIZE_X)) && yPos >= (zoomFactor * BS_PILLBOX_OFFSET_Y) && yPos <= ((zoomFactor * BS_PILLBOX_OFFSET_Y) + (zoomFactor * BS_ITEM_SIZE_Y))) {
      newSelect = BsPillbox;
    } else if (xPos >= (zoomFactor * BS_MINE_OFFSET_X) && xPos <= ((zoomFactor * BS_MINE_OFFSET_X) + (zoomFactor * BS_ITEM_SIZE_X)) && yPos >= (zoomFactor * BS_MINE_OFFSET_Y) && yPos <= ((zoomFactor * BS_MINE_OFFSET_Y) + (zoomFactor * BS_ITEM_SIZE_Y))) {
      newSelect = BsMine;
    } 

    if (newSelect != NO_SELECT && newSelect != BsCurrent) {
      tick = winboloTimer();
      drawSelectIndentsOff(BsCurrent, xWin, yWin);
      drawSelectIndentsOn(newSelect, xWin, yWin);
      BsCurrent = newSelect;
      dwSysFrame += (winboloTimer() - tick);
    }
  }
}

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
void windowShowGunsight(HWND hWnd) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);

  if (showGunsight == TRUE) {
    screenSetGunsight(FALSE);
    showGunsight = FALSE;
    CheckMenuItem(hMenu, ID_EDITMENU_GUNSIGHT, MF_UNCHECKED);
  } else {
    screenSetGunsight(TRUE);
    showGunsight = TRUE;
    CheckMenuItem(hMenu, ID_EDITMENU_GUNSIGHT, MF_CHECKED);
  }

}

/*********************************************************
*NAME:          windowSoundEffects
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 27/3/98
*PURPOSE:
* Turns the sound effects on or off.
* Updates the menu
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowSoundEffects(HWND hWnd) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  if (soundEffects == TRUE) {
    soundEffects = FALSE;
    CheckMenuItem(hMenu, ID_BOLOMENU_SOUNDEFFECTS, MF_UNCHECKED);
    EnableMenuItem(hMenu, ID_BOLOMENU_BACKGROUNDSOUND, MF_GRAYED);  
    EnableMenuItem(hMenu, ID_BOLOMENU_ISASOUNDCARD, MF_GRAYED);
  } else {
    soundEffects = TRUE;
    CheckMenuItem(hMenu, ID_BOLOMENU_SOUNDEFFECTS, MF_CHECKED);
    EnableMenuItem(hMenu, ID_BOLOMENU_BACKGROUNDSOUND, MF_ENABLED);
    EnableMenuItem(hMenu, ID_BOLOMENU_ISASOUNDCARD, MF_ENABLED);
  }
}

/*********************************************************
*NAME:          windowPlaySound
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 28/12/98
*PURPOSE:
*  Plays a sound effect if it is turned on
*
*ARGUMENTS:
*  value - The sound effect to play
*********************************************************/
void windowPlaySound(sndEffects value) {
  if (soundEffects == TRUE) {
    soundPlayEffect(value);
  }
}

/*********************************************************
*NAME:          frontEndUpdateTankStatusBars
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 28/12/98
*PURPOSE:
* Function is called when the tanks status bars need to 
* be updated
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  shells  - Number of shells
*  mines   - Number of mines
*  armour  - Amount of armour
*  trees   - Amount of trees
*********************************************************/
void frontEndUpdateTankStatusBars(BYTE shells, BYTE mines, BYTE armour, BYTE trees) {
  RECT rcWindow;
  DWORD tick;
  
  tick = winboloTimer();
  // get the client (drawing) rectangle.
  if (GetClientRect( appWnd, &rcWindow )) {
    if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
      drawStatusTankBars(rcWindow.left, rcWindow.top, shells, mines, armour, trees);
    }
  }
  dwSysFrame += (winboloTimer() - tick);
}

/*********************************************************
*NAME:          frontEndSoundEffects
*AUTHOR:        John Morrison
*CREATION DATE: 28/12/98
*LAST MODIFIED: 28/12/98
*PURPOSE:
*  Plays a sound effect if it is turned on
*
*ARGUMENTS:
*  value - The sound effect to play
*********************************************************/
void frontEndPlaySound(sndEffects value) {
  if (soundEffects == TRUE) {
    soundPlayEffect(value);
  }
}

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
void windowDisableSound() {
  HMENU hMenu; /* Handle to the Menu */
  
  hMenu = GetMenu(appWnd);
  soundEffects = FALSE;
  isISASoundCard = FALSE;
  CheckMenuItem(hMenu, ID_BOLOMENU_SOUNDEFFECTS, MF_UNCHECKED);
  CheckMenuItem(hMenu, ID_BOLOMENU_BACKGROUNDSOUND, MF_UNCHECKED);
  CheckMenuItem(hMenu, ID_BOLOMENU_ISASOUNDCARD, MF_UNCHECKED);
  EnableMenuItem(hMenu, ID_BOLOMENU_SOUNDEFFECTS, MF_GRAYED);
  EnableMenuItem(hMenu, ID_BOLOMENU_BACKGROUNDSOUND, MF_GRAYED);  
  EnableMenuItem(hMenu, ID_BOLOMENU_ISASOUNDCARD, MF_GRAYED);
}


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
void windowISASoundCard() {
  HMENU hMenu; /* Handle to the Menu */

  hMenu = GetMenu(appWnd);
  if (isISASoundCard == TRUE) {
    CheckMenuItem(hMenu, ID_BOLOMENU_ISASOUNDCARD, MF_UNCHECKED);
    isISASoundCard = FALSE;
  } else {
    CheckMenuItem(hMenu, ID_BOLOMENU_ISASOUNDCARD, MF_CHECKED);
    isISASoundCard = TRUE;
  }
  if (soundEffects == TRUE && soundIsPlayable() == TRUE) {
    soundISASoundCard(isISASoundCard);
  }
}

/*********************************************************
*NAME:          frontEndStatusPillbox
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  3/1/99
*PURPOSE:
*  Sets the pillbox status for a particular pillbox
*
*ARGUMENTS:
*  pillNum - The base number to draw (1-16)
*  pa      - The allience of the pillbox
*********************************************************/
void frontEndStatusPillbox(BYTE pillNum, pillAlliance pb) {
  DWORD tick;
  RECT rcWindow;

  tick = winboloTimer();
  drawStatusPillbox(pillNum, pb, showPillLabels);
  if (GetClientRect( appWnd, &rcWindow )) {
    if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
      drawCopyPillsStatus(rcWindow.left, rcWindow.top);
    }
  }
  dwSysFrame += (winboloTimer() - tick);
}

/*********************************************************
*NAME:          frontEndStatusTank
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
*  Sets the tank status for a particular tank
*
*ARGUMENTS:
*  tankNum - The tank number to draw (1-16)
*  ts      - The allience of the tank
*********************************************************/
void frontEndStatusTank(BYTE tankNum, tankAlliance ts) {
  DWORD tick;
  RECT rcWindow;

  tick = winboloTimer();
  drawStatusTank(tankNum, ts);
  if (GetClientRect( appWnd, &rcWindow )) {
    if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
      drawCopyTanksStatus(rcWindow.left, rcWindow.top);
    }
  }
  dwSysFrame += (winboloTimer() - tick);
}

/*********************************************************
*NAME:          frontEndMessages
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  3/1/99
*PURPOSE:
*  The messages must be drawn on the screen
*
*ARGUMENTS:
*  top    - The top line to write
*  bottom - The bottom line to write
*********************************************************/
void frontEndMessages(char *top, char *bottom) {
  DWORD tick;
  RECT rcWindow;

  if (drawBusy == FALSE) {
    tick = winboloTimer();
    if (GetClientRect( appWnd, &rcWindow )) {
      if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
        drawMessages(rcWindow.left, rcWindow.top, top, bottom);
      }
    }
    dwSysFrame += (winboloTimer() - tick);
  }
}

/*********************************************************
*NAME:          frontEndKillsDeaths
*AUTHOR:        John Morrison
*CREATION DATE:  8/1/99
*LAST MODIFIED:  8/1/99
*PURPOSE:
*  The tank kills/deaths must be updated
*
*ARGUMENTS:
*  kills  - The number of kills the tank has.
*  deaths - The number of times the tank has died
*********************************************************/
void frontEndKillsDeaths(int kills, int deaths) {
  DWORD tick;
  RECT rcWindow;

  if (drawBusy == FALSE) {
    tick = winboloTimer();
    if (GetClientRect( appWnd, &rcWindow )) {
      if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
        drawKillsDeaths(rcWindow.left, rcWindow.top, kills, deaths);
      }
    }
    dwSysFrame += (winboloTimer() - tick);
  }
}


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
void windowMenuAllowNewPlayers(HWND hWnd) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  if (allowNewPlayers == TRUE) {
    allowNewPlayers = FALSE;
    CheckMenuItem(hMenu, ID_BOLOMENU_ALLOWNEWPLAYERS, MF_UNCHECKED);
  } else {
    allowNewPlayers = TRUE;
    CheckMenuItem(hMenu, ID_BOLOMENU_ALLOWNEWPLAYERS, MF_CHECKED);
  }
  netSetAllowNewPlayers(allowNewPlayers);
}


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
void windowMenuAllowAllianceRequest(HWND hWnd) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  if (GetMenuState(hMenu, ID_BOLOMENU_REQUESTALLIANCE, 0) == MF_CHECKED) {
    CheckMenuItem(hMenu, ID_BOLOMENU_REQUESTALLIANCE, MF_UNCHECKED);
  } else {
    CheckMenuItem(hMenu, ID_BOLOMENU_REQUESTALLIANCE, MF_CHECKED);
  }
}

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
void windowMenuNewswire(HWND hWnd) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  if (showNewswireMessages == TRUE) {
    showNewswireMessages = FALSE;
    CheckMenuItem(hMenu, ID_BOLOMENU_NEWSWIREMESSAGES, MF_UNCHECKED);
  } else {
    showNewswireMessages = TRUE;
    CheckMenuItem(hMenu, ID_BOLOMENU_NEWSWIREMESSAGES, MF_CHECKED);
  }
  screenShowMessages(MSG_NEWSWIRE, showNewswireMessages);
}

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
void windowMenuAssistant(HWND hWnd) {

  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  if (showAssistantMessages == TRUE) {
    showAssistantMessages = FALSE;
    CheckMenuItem(hMenu, ID_BOLOMENU_ASSISTANTMESSAGES, MF_UNCHECKED);
  } else {
    showAssistantMessages = TRUE;
    CheckMenuItem(hMenu, ID_BOLOMENU_ASSISTANTMESSAGES, MF_CHECKED);
  }
  screenShowMessages(MSG_ASSISTANT, showAssistantMessages );
}

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
void windowMenuAI(HWND hWnd) {

  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  if (showAIMessages == TRUE) {
    showAIMessages = FALSE;
    CheckMenuItem(hMenu, ID_BOLOMENU_AIBRAINMESSAGES, MF_UNCHECKED);
  } else {
    showAIMessages = TRUE;
    CheckMenuItem(hMenu, ID_BOLOMENU_AIBRAINMESSAGES, MF_CHECKED);
  }
  screenShowMessages(MSG_AI, showAIMessages);
}

/*********************************************************
*NAME:          windowMenuNetwork
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/99
*PURPOSE:
* Bolo -> Network Status has been checked.
*
*ARGUMENTS:
*  hWnd - The application Window
*********************************************************/
void windowMenuNetwork(HWND hWnd) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  if (showNetworkStatusMessages == TRUE) {
    showNetworkStatusMessages = FALSE;
    CheckMenuItem(hMenu, ID_BOLOMENU_NETWORKSTATUSMESSAGES, MF_UNCHECKED);
  } else {
    showNetworkStatusMessages = TRUE;
    CheckMenuItem(hMenu, ID_BOLOMENU_NETWORKSTATUSMESSAGES, MF_CHECKED);
  }
  screenShowMessages(MSG_NETSTATUS, showNetworkStatusMessages);
}

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
void windowMenuNetworkDebug(HWND hWnd) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  if (showNetworkDebugMessages == TRUE) {
    showNetworkDebugMessages = FALSE;
    CheckMenuItem(hMenu, ID_BOLOMENU_NETWORKDEBUGMESSAGES, MF_UNCHECKED);
  } else {
    showNetworkDebugMessages = TRUE;
    CheckMenuItem(hMenu, ID_BOLOMENU_NETWORKDEBUGMESSAGES, MF_CHECKED);
  }
  screenShowMessages(MSG_NETWORK, showNetworkDebugMessages);
}

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
void windowShowPillLabels(HWND hWnd) {
  DWORD tick;
  HMENU hMenu;   /* The menu of the Window */
  BYTE count;    /* looping variable */
  BYTE pillStat; /* Status of the pill being worked on */
  BYTE total;    /* Number of pills to work on */
  RECT rcWindow; /* Window region */

  hMenu = GetMenu(hWnd);
  if (showPillLabels == TRUE) {
    showPillLabels = FALSE;
    CheckMenuItem(hMenu, ID_EDITMENU_PILLLABELS, MF_UNCHECKED);
  } else {
    showPillLabels = TRUE;
    CheckMenuItem(hMenu, ID_EDITMENU_PILLLABELS, MF_CHECKED);
  }

  tick = winboloTimer();
  drawSetPillsStatusClear();
  total = screenNumPills();
  for (count=1;count<=total;count++) {
    pillStat = screenPillAlliance(count);
    drawStatusPillbox(count, pillStat, showPillLabels);
  }
  
  
  if (GetClientRect( appWnd, &rcWindow )) {
    if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
      drawCopyPillsStatus(rcWindow.left, rcWindow.top);
    }
  }
  dwSysFrame += (winboloTimer() - tick);
}

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
void windowShowBaseLabels(HWND hWnd) {
  DWORD tick;
  HMENU hMenu;   /* The menu of the Window */
  BYTE count;    /* looping variable */
  BYTE baseStat; /* Status of the pill being worked on */
  BYTE total;    /* Number of pills to work on */
  RECT rcWindow; /* Window region */

  hMenu = GetMenu(hWnd);
  if (showBaseLabels == TRUE) {
    showBaseLabels = FALSE;
    CheckMenuItem(hMenu, ID_EDITMENU_BASELABELS, MF_UNCHECKED);
  } else {
    showBaseLabels = TRUE;
    CheckMenuItem(hMenu, ID_EDITMENU_BASELABELS, MF_CHECKED);
  }
  
  tick = winboloTimer();
  drawSetBasesStatusClear();
  total = screenNumBases();
  for (count=1;count<=total;count++) {
    baseStat = screenBaseAlliance(count);
    drawStatusBase(count, baseStat, showBaseLabels);
  }
  
  if (GetClientRect( appWnd, &rcWindow ) != 0) {
    if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
      drawCopyBasesStatus(rcWindow.left, rcWindow.top);
    }
  }
  dwSysFrame += (winboloTimer() - tick);

}

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
void windowAutomaticScrolling(HWND hWnd) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  if (autoScrollingEnabled == TRUE) {
    autoScrollingEnabled = FALSE;
    CheckMenuItem(hMenu, ID_EDITMENU_AUTOMATICSCROLLING, MF_UNCHECKED);
  } else {
    autoScrollingEnabled = TRUE;
    CheckMenuItem(hMenu, ID_EDITMENU_AUTOMATICSCROLLING, MF_CHECKED);
  }
  screenSetAutoScroll(autoScrollingEnabled);
}

/*********************************************************
*NAME:          frontEndStatusBase
*AUTHOR:        John Morrison
*CREATION DATE: 10/1/99
*LAST MODIFIED: 10/1/99
*PURPOSE:
*  Sets the base status for a particular base
*
*ARGUMENTS:
*  pillNum - The base number to draw (1-16)
*  bs      - The allience of the pillbox
*********************************************************/
void frontEndStatusBase(BYTE baseNum, baseAlliance bs) {
  DWORD tick;
  RECT rcWindow;

  tick = winboloTimer();
  drawStatusBase(baseNum, bs, showBaseLabels);
  if (GetClientRect( appWnd, &rcWindow ) != 0) {
    if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
      drawCopyBasesStatus(rcWindow.left, rcWindow.top);
    }
  }
  dwSysFrame += (winboloTimer() - tick);
}

/*********************************************************
*NAME:          frontEndUpdateBaseStatusBars
*AUTHOR:        John Morrison
*CREATION DATE: 11/1/99
*LAST MODIFIED: 11/1/99
*PURPOSE:
* Function is called when the tanks status bars need to 
* be updated
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  shells  - Number of shells
*  mines   - Number of mines
*  armour  - Amount of armour
*********************************************************/
void frontEndUpdateBaseStatusBars(BYTE shells, BYTE mines, BYTE armour) {
  DWORD tick;
  RECT rcWindow;

  tick = winboloTimer();
  if (GetClientRect( appWnd, &rcWindow )) {
    if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
      drawStatusBaseBars(rcWindow.left, rcWindow.top, shells, mines, armour, FALSE);
    }
  }
  dwSysFrame += (winboloTimer() - tick);
}


/*********************************************************
*NAME:          frontEndManStatus
*AUTHOR:        John Morrison
*CREATION DATE:  8/1/99
*LAST MODIFIED:  8/1/99
*PURPOSE:
*  The tank kills/deaths must be updated
*
*ARGUMENTS:
*  isDead - Is the man dead
*  angle  - The angle the man is facing
*********************************************************/
void frontEndManStatus(bool isDead, TURNTYPE angle) {
  DWORD tick;
  RECT rcWindow;

  tick = winboloTimer();
  if (GetClientRect( appWnd, &rcWindow )) {
    if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
      clientMutexWaitFor();
      drawSetManStatus(rcWindow.left, rcWindow.top, isDead, angle);

      clientMutexRelease();
    }
  }
  dwSysFrame += (winboloTimer() - tick);
}

/*********************************************************
*NAME:          frontEndDrawDownload
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/99
*LAST MODIFIED:  3/1/00
*PURPOSE:
* A screen redraw request has been made but we are still
* downloading network data. Draw progress line instead.
* 
*ARGUMENTS:
*  justBlack - TRUE if we want just a black screen
*********************************************************/
void frontEndDrawDownload(bool justBlack) {
  DWORD tick;
  RECT rcWindow;

  if (hideMainView == FALSE && drawBusy == FALSE) {
    tick = winboloTimer();
    if (GetClientRect( appWnd, &rcWindow )) {
      if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
        drawDownloadScreen(&rcWindow, justBlack);
      }
    }
    dwSysFrame += (winboloTimer() - tick);
  }
}

/*********************************************************
*NAME:          frontEndManClear
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/99
*LAST MODIFIED: 18/1/99
*PURPOSE:
*  Clears the man status (ie man is in tank)
*
*ARGUMENTS:
*
*********************************************************/
void frontEndManClear(void) {
  DWORD tick;
  RECT rcWindow;

  tick = winboloTimer();
  if (GetClientRect( appWnd, &rcWindow )) {
    if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
      clientMutexWaitFor();
      drawSetManClear();
      drawCopyManStatus(rcWindow.left, rcWindow.top);
      clientMutexRelease();
    }
  }
  dwSysFrame += (winboloTimer() - tick);
}

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
void windowZoomChange(BYTE amount) {
  DWORD tick;
  HMENU hMenu;   /* The menu of the Window */
  RECT rcWindow;
  bool noChange; /* Did we actually change? */

  noChange = FALSE;
  if (amount == zoomFactor) {
    noChange = TRUE;
  }
  hMenu = GetMenu(appWnd);

  /* Uncheck the old item */
  switch (zoomFactor) {
  case ZOOM_FACTOR_NORMAL:
    CheckMenuItem(hMenu, ID_WINDOWSIZE_NORMAL, MF_UNCHECKED);
    break;
  case ZOOM_FACTOR_DOUBLE:
    CheckMenuItem(hMenu, ID_WINDOWSIZE_DOUBLE, MF_UNCHECKED);
    break;
  default:
    /* ZOOM_FACTOR_QUAD: */
    CheckMenuItem(hMenu, ID_WINDOWSIZE_QUAD, MF_UNCHECKED);
    break;
  }
  /* Apply the change */
  windowSetZoomFactor(amount);
  /* Check the item */
  switch (zoomFactor) {
  case ZOOM_FACTOR_NORMAL:
    CheckMenuItem(hMenu, ID_WINDOWSIZE_NORMAL, MF_CHECKED);
    break;
  case ZOOM_FACTOR_DOUBLE:
    CheckMenuItem(hMenu, ID_WINDOWSIZE_DOUBLE, MF_CHECKED);
    break;
  default:
    /* ZOOM_FACTOR_QUAD: */
    CheckMenuItem(hMenu, ID_WINDOWSIZE_QUAD, MF_CHECKED);
    break;
  }
  /* Resize the Window and do a redraw */
  if (noChange == FALSE) {

    drawBusy = TRUE;
    clientMutexWaitFor();
    drawCleanup();
    fontCleanup();
    drawSetup(appInst, appWnd);
    fontSetup(appInst, appWnd);
    SetWindowPos(appWnd, HWND_NOTOPMOST, 0, 0, (zoomFactor * TOTAL_WINDOW_SIZE_X) +4, (zoomFactor * TOTAL_WINDOW_SIZE_Y) +42, (SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW));
    clientMutexRelease();
    drawBusy = FALSE;

    tick = winboloTimer();
    if (GetClientRect( appWnd, &rcWindow )) {
      if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
        clientMutexWaitFor();
        drawRedrawAll(appInst, appWnd, BsCurrent, &rcWindow, showPillLabels, showBaseLabels);
        clientMutexRelease();
      }
    }
    
    dwSysFrame += (winboloTimer() - tick);
  }
}

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
void windowSetZoomFactor(BYTE amount) {
  zoomFactor = amount;
}

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
BYTE windowGetZoomFactor() {
  return zoomFactor;
}

/*********************************************************
*NAME:          frontEndGameOver
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 29/1/99
*PURPOSE:
*  Time limit is up. The game is over
*
*ARGUMENTS:
*
*********************************************************/
void frontEndGameOver(void) {
  KillTimer(appWnd, timerFrameID);
  KillTimer(appWnd, timerGameID);
  MessageBoxA(NULL, langGetText(STR_WBTIMELIMIT_END), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
  PostQuitMessage(0);
}

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
HWND windowWnd(void) {
  return appWnd;
}

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
int windowGetDrawTime(void) {
  return (int) dwSysFrameTotal;
}

/*********************************************************
*NAME:          windowGetNetTime
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
*  Returns number of miliseconds spent doing network work
*  last second
*
*ARGUMENTS:
*
*********************************************************/
int windowGetNetTime(void) {
  return netGetNetTime();
}

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
int windowGetAiTime(void) {
  return dwSysBrainTotal;
}

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
int windowGetSimTime(void) {
  return dwSysGameTotal;
}

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
void windowGetKeys(keyItems *value) {
  value->kiForward = keys.kiForward;
  value->kiBackward = keys.kiBackward;
  value->kiLeft = keys.kiLeft;
  value->kiRight = keys.kiRight;
  value->kiShoot = keys.kiShoot;
  value->kiLayMine = keys.kiLayMine;
  value->kiGunIncrease = keys.kiGunIncrease;
  value->kiGunDecrease = keys.kiGunDecrease;
  value->kiTankView = keys.kiTankView;
  value->kiPillView = keys.kiPillView;
  value->kiScrollUp = keys.kiScrollUp;
  value->kiScrollDown = keys.kiScrollDown;
  value->kiScrollLeft = keys.kiScrollLeft;
  value->kiScrollRight = keys.kiScrollRight;
}

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
void windowSetKeys(keyItems *value) {
  keys.kiForward = value->kiForward;
  keys.kiBackward = value->kiBackward;
  keys.kiLeft = value->kiLeft;
  keys.kiRight = value->kiRight;
  keys.kiShoot = value->kiShoot;
  keys.kiLayMine = value->kiLayMine;
  keys.kiGunIncrease = value->kiGunIncrease;
  keys.kiGunDecrease = value->kiGunDecrease;
  keys.kiTankView = value->kiTankView;
  keys.kiPillView = value->kiPillView;
  keys.kiScrollUp = value->kiScrollUp;
  keys.kiScrollDown = value->kiScrollDown;
  keys.kiScrollLeft = value->kiScrollLeft;
  keys.kiScrollRight = value->kiScrollRight;
}

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
void windowLabelOwnTank(HWND hWnd) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  if (labelSelf == TRUE) {
    labelSelf = FALSE;
    CheckMenuItem(hMenu, ID_EDIT_TANKLABELS_DONTLABELOWNTANK, MF_CHECKED);
  } else {
    labelSelf = TRUE;
    CheckMenuItem(hMenu, ID_EDIT_TANKLABELS_DONTLABELOWNTANK, MF_UNCHECKED);
  }
  screenSetLabelOwnTank(labelSelf);
}

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
void windowSetMessageLabelLen(HWND hWnd, labelLen newLen) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  CheckMenuItem(hMenu, ID_EDIT_MESSAGESENDERNAMES_SHORT, MF_UNCHECKED);
  CheckMenuItem(hMenu, ID_EDIT_MESSAGESENDERNAMES_LONG, MF_UNCHECKED);
  labelMsg = newLen;
  /* Select the new */
  switch (labelMsg) {
  case lblShort:
    CheckMenuItem(hMenu, ID_EDIT_MESSAGESENDERNAMES_SHORT, MF_CHECKED);
    break;
  default:
    /* LblLong */
    CheckMenuItem(hMenu, ID_EDIT_MESSAGESENDERNAMES_LONG, MF_CHECKED);
    break;
  }
  /* Call the back end */
  screenSetMesageLabelLen(labelMsg);
}

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
void windowSetTankLabelLen(HWND hWnd, labelLen newLen) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(hWnd);
  CheckMenuItem(hMenu, ID_EDIT_TANKLABELS_NONE, MF_UNCHECKED);
  CheckMenuItem(hMenu, ID_EDIT_TANKLABELS_SHORT, MF_UNCHECKED);
  /* LblLong */
  CheckMenuItem(hMenu, ID_EDIT_TANKLABELS_LONG, MF_UNCHECKED);
  labelTank = newLen;
    /* Select the new */
  switch (labelTank) {
  case lblNone:
    CheckMenuItem(hMenu, ID_EDIT_TANKLABELS_NONE, MF_CHECKED);
    break;
  case lblShort:
    CheckMenuItem(hMenu, ID_EDIT_TANKLABELS_SHORT, MF_CHECKED);
    break;
  default:
    /* LblLong */
    CheckMenuItem(hMenu, ID_EDIT_TANKLABELS_LONG, MF_CHECKED);
    break;
  }
    /* Call the back end */
  screenSetTankLabelLen(labelTank);
}

/*********************************************************
*NAME:          frontEndClearPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 4/2/99
*LAST MODIFIED: 4/2/99
*PURPOSE:
* Clears a player name from the menu and disables it.
* 
*ARGUMENTS:
*  value - The player number to clear
*********************************************************/
void frontEndClearPlayer(playerNumbers value) {
  HMENU hMenu;      /* Handle to the menu */
  MENUITEMINFO inf; /* Menu Item information */
  
  hMenu = GetMenu(appWnd); /* Get the menu */
  /* Setup the structure */
  inf.cbSize = sizeof(inf); 
  inf.fState = MFS_GRAYED;
  inf.fMask = (MIIM_STATE | MIIM_TYPE);
  inf.fType = MFT_STRING;
  inf.cch = STR_PLAYER_LEN;

  switch (value) {
  case player01:
    inf.dwTypeData = (char *) STR_01;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_01, FALSE, &inf);
    break;
  case player02:
    inf.dwTypeData = (char *) STR_02;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_02, FALSE, &inf);
    break;
  case player03:
    inf.dwTypeData = (char *) STR_03;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_03, FALSE, &inf);
    break;
  case player04:
    inf.dwTypeData = (char *) STR_04;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_04, FALSE, &inf);
    break;
  case player05:
    inf.dwTypeData = (char *) STR_05;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_05, FALSE, &inf);
    break;
  case player06:
    inf.dwTypeData = (char *) STR_06;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_06, FALSE, &inf);
    break;
  case player07:
    inf.dwTypeData = (char *) STR_07;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_07, FALSE, &inf);
    break;
  case player08:
    inf.dwTypeData = (char *) STR_08;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_08, FALSE, &inf);
    break;
  case player09:
    inf.dwTypeData = (char *) STR_09;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_09, FALSE, &inf);
    break;
  case player10:
    inf.dwTypeData = (char *) STR_10;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_10, FALSE, &inf);
    break;
  case player11:
    inf.dwTypeData = (char *) STR_10;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_11, FALSE, &inf);
    break;
  case player12:
    inf.dwTypeData = (char *) STR_12;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_12, FALSE, &inf);
    break;
  case player13:
    inf.dwTypeData = (char *) STR_13;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_13, FALSE, &inf);
    break;
  case player14:
    inf.dwTypeData = (char *) STR_14;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_14, FALSE, &inf);
    break;
  case player15:
    inf.dwTypeData = (char *) STR_15;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_15, FALSE, &inf);
    break;
  default:
    /* case player16: */
    inf.dwTypeData = (char *) STR_16;
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_16, FALSE, &inf);
    break;
  }
}

/*********************************************************
*NAME:          frontEndSetPlayer
*AUTHOR:        John Morrison
*CREATION DATE: 4/2/99
*LAST MODIFIED: 4/2/99
*PURPOSE:
* Sets a player name in the menu and enables it.
* 
*ARGUMENTS:
*  value - The player number to set
*  str   - String identifier of the name
*********************************************************/
void frontEndSetPlayer(playerNumbers value, char *str) {
  HMENU hMenu;      /* Handle to the menu */
  MENUITEMINFO inf; /* Menu Item information */
  
  hMenu = GetMenu(appWnd); /* Get the menu */
  
  /* Setup the structure */
  inf.cbSize = sizeof(inf); 
  inf.fState = MFS_ENABLED;
  inf.fMask = (MIIM_STATE | MIIM_TYPE);
  inf.fType = MFT_STRING;
  inf.cch = strlen(str);
  inf.dwTypeData = str;

  switch (value) {
  case player01:  
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_01, FALSE, &inf);
    break;
  case player02:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_02, FALSE, &inf);
    break;
  case player03:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_03, FALSE, &inf);
    break;
  case player04:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_04, FALSE, &inf);
    break;
  case player05:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_05, FALSE, &inf);
    break;
  case player06:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_06, FALSE, &inf);
    break;
  case player07:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_07, FALSE, &inf);
    break;
  case player08:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_08, FALSE, &inf);
    break;
  case player09:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_09, FALSE, &inf);
    break;
  case player10:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_10, FALSE, &inf);
    break;
  case player11:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_11, FALSE, &inf);
    break;
  case player12:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_12, FALSE, &inf);
    break;
  case player13:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_13, FALSE, &inf);
    break;
  case player14:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_14, FALSE, &inf);
    break;
  case player15:
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_15, FALSE, &inf);
    break;
  case player16:
  default:
    /* case player16: */
    SetMenuItemInfoA(hMenu, ID_PLAYERSMENU_16, FALSE, &inf);
    break;
  }
}

/*********************************************************
*NAME:          frontEndSetPlayerCheckState
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/99
*LAST MODIFIED: 27/3/99
*PURPOSE:
* Checks/unchecks a player
* 
*ARGUMENTS:
* value     - The player number
* isChecked - Is the item checked
*********************************************************/
void frontEndSetPlayerCheckState(playerNumbers value, bool isChecked) {
  HMENU hMenu;         /* Handle to the menu */
  unsigned int check;  /* Should the item be checked or not */
  
  hMenu = GetMenu(appWnd); /* Get the menu */
  check = MF_UNCHECKED;
  if (isChecked == TRUE) {
    check = MF_CHECKED;
  }
 
  switch (value) {
  case player01:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_01, check);
    break;
  case player02:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_02, check);
    break;
  case player03:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_03, check);
    break;
  case player04:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_04, check);
    break;
  case player05:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_05, check);
    break;
  case player06:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_06, check);
    break;
  case player07:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_07, check);
    break;
  case player08:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_08, check);
    break;
  case player09:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_09, check);
    break;
  case player10:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_10, check);
    break;
  case player11:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_11, check);
    break;
  case player12:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_12, check);
    break;
  case player13:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_13, check);
    break;
  case player14:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_14, check);
    break;
  case player15:
    CheckMenuItem(hMenu, ID_PLAYERSMENU_15, check);
    break;
  case player16:
  default:
    /* case player16: */
    CheckMenuItem(hMenu, ID_PLAYERSMENU_16, check);
    break;
  }
}

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
void windowSaveMap() {
  OPENFILENAME ofn;              /* File name to be saved */
  char fileName[FILENAME_MAX];  /* The filename and path that should be opened */
  bool dlgOK;                   /* Did th dialog work ok? */
  bool saveOK;                  /* Did the file save OK */

  fileName[0] = '\0';

  /* Get the name of the map */
  screenGetMapName(fileName);

  /* Fill the open file dialog box with stuff */
  memset( &ofn, 0, sizeof(OPENFILENAME));
  ofn.lStructSize	= sizeof(OPENFILENAME);
  ofn.hwndOwner	= appWnd;
  ofn.hInstance	= appInst;
  ofn.lpstrFilter	= OPEN_FILE_FILTERS;
  ofn.lpstrCustomFilter = NULL;
  ofn.nFilterIndex	= 0;
  ofn.nMaxCustFilter = 0;

  ofn.lpstrFile	= fileName;
  ofn.nMaxFile	= sizeof(fileName);
	ofn.lpstrFileTitle	= NULL;
	ofn.nMaxFileTitle	= 0;
 	ofn.lpstrInitialDir = NULL;
  ofn.lpstrTitle	= NULL;
  ofn.Flags		= OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
  ofn.lpstrDefExt = DEFAULT_FILE_EXTENSION;
  
  dlgOK = (bool) GetSaveFileName( &ofn ); /* Do the dialog box */
  if (dlgOK == TRUE) {
    saveOK = screenSaveMap(fileName);
    if (saveOK == FALSE) {
      MessageBoxA(appWnd, langGetText(STR_WBERR_SAVEMAP), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
    }
  }
}

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
void windowShowGameInfo(windowShowRequest req) {
  if (req == wsrOpen && infoWnd == NULL) {
    infoWnd = CreateDialog(appInst, MAKEINTRESOURCE(IDD_GAME_INFO), NULL, dialogGameInfoCallback);
    ShowWindow(infoWnd, SW_SHOWNORMAL);
  } else if (req == wsrOpen) {
    SetFocus(infoWnd);
  } else if (req == wsrClose && infoWnd != NULL) {
    EndDialog(infoWnd , TRUE);
    infoWnd = NULL;
  }
}

/*********************************************************
*NAME:          windowShowGameInfo
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/99
*LAST MODIFIED: 20/11/99
*PURPOSE:
* Handles opening (preventing multiple openings) of the
* sys info dialog box
* 
*ARGUMENTS:
*  req - type of window open/close request
*********************************************************/
void windowShowSysInfo(windowShowRequest req) {
  if (req == wsrOpen && sysWnd == NULL) {
    sysWnd = CreateDialog(appInst, MAKEINTRESOURCE(IDD_SYSTEM_INFO), NULL, dialogSysInfoCallback);
    ShowWindow(sysWnd, SW_SHOWNORMAL);
  } else if (req == wsrOpen) {
    SetFocus(sysWnd);
  } else if (req == wsrClose && sysWnd != NULL) {
    EndDialog(sysWnd, TRUE);
    sysWnd = NULL;
  }
}

/*********************************************************
*NAME:          windowShowNetInfo
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 20/11/99
*PURPOSE:
* Handles opening (preventing multiple openings) of the
* net info dialog box
* 
*ARGUMENTS:
*  req - type of window open/close request
*********************************************************/
void windowShowNetInfo(windowShowRequest req) {
  if (req == wsrOpen && netWnd == NULL) {
    netWnd = CreateDialog(appInst, MAKEINTRESOURCE(IDD_NETWORK_INFO), NULL, dialogNetInfoCallback);
    ShowWindow(netWnd, SW_SHOWNORMAL);
  } else if (req == wsrOpen) {
    SetFocus(netWnd);
  } else if (req == wsrClose && netWnd != NULL) {
    EndDialog(netWnd, TRUE);
    netWnd = NULL;
  }
}

/*********************************************************
*NAME:          windowShowSetPlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/99
*LAST MODIFIED: 20/11/99
*PURPOSE:
* Handles opening (preventing multiple openings) of the
* set player name dialog box
* 
*ARGUMENTS:
*  req - type of window open/close request
*********************************************************/
void windowShowSetPlayerName(windowShowRequest req) {
  if (req == wsrOpen) {
    DialogBox(appInst, MAKEINTRESOURCE(IDD_SETPLAYERNAME), appWnd, dialogSetNameCallback);
  }
}

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
void windowShowSendMessages(windowShowRequest req) {
  if (req == wsrOpen && msgWnd == NULL) {
    msgWnd = CreateDialog(appInst, MAKEINTRESOURCE(IDD_MESSAGES), NULL, dialogMessagesCallback);
    ShowWindow(msgWnd, SW_SHOWNORMAL);
  } else if (req == wsrOpen) {
    SetFocus(msgWnd);
  } else if (req == wsrClose && msgWnd != NULL) {
    EndDialog(msgWnd, TRUE);
    msgWnd = NULL;
  }
}

/*********************************************************
*NAME:          windowShowSetKeys
*AUTHOR:        John Morrison
*CREATION DATE:  14/2/99
*LAST MODIFIED: 20/11/99
*PURPOSE:
* Handles opening (preventing multiple openings) of the
* set keys dialog box
* 
*ARGUMENTS:
*  req - type of window open/close request
*********************************************************/
void windowShowSetKeys(windowShowRequest req) {
  if (req == wsrOpen && keyWnd == NULL) {
    keyWnd = CreateDialog(appInst, MAKEINTRESOURCE(IDD_KEY_SETUP), appWnd, dialogKeySetupCallback);
    ShowWindow(keyWnd, SW_SHOWNORMAL);
  } else if (req == wsrOpen) {
    SetFocus(keyWnd);
  } else if (req == wsrClose && keyWnd != NULL) {
    EndDialog(keyWnd, TRUE);
    keyWnd = NULL;
  }
}

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
void windowRedrawAll(void) {
  RECT rcWindow;
  if (GetClientRect( appWnd, &rcWindow )) {
    if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
      clientMutexWaitFor();
      drawRedrawAll(appInst, appWnd, BsCurrent, &rcWindow, showPillLabels, showBaseLabels);
      clientMutexRelease();
    }
  }
}

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
bool windowGetBackgroundSound(void) {
  return backgroundSound;
}

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
void windowBackgroundSoundChange(HWND hWnd, bool wantInit) {
  HMENU hMenu; /* Handle to the Menu */
  hMenu = GetMenu(appWnd);
  if (backgroundSound == TRUE) {
    CheckMenuItem(hMenu, ID_BOLOMENU_BACKGROUNDSOUND, MF_UNCHECKED);
    backgroundSound = FALSE;
  } else {
    CheckMenuItem(hMenu, ID_BOLOMENU_BACKGROUNDSOUND, MF_CHECKED);
    backgroundSound = TRUE;
  }

  if (wantInit == TRUE) {
    soundEffects = FALSE;
    soundCleanup();
    soundSetup(appInst, hWnd);
    soundEffects = TRUE;
  }
}

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
void windowHideMainView(HWND hWnd) {
  DWORD tick;
  HMENU hMenu;   /* Handle to the Menu */
  RECT rcWindow; /* Window Size */

  hMenu = GetMenu(appWnd);
  if (hideMainView == TRUE) {
    CheckMenuItem(hMenu, ID_EDITMENU_HIDEMAINVIEW, MF_UNCHECKED);
    hideMainView = FALSE;
  } else {
    CheckMenuItem(hMenu, ID_EDITMENU_HIDEMAINVIEW, MF_CHECKED);
    hideMainView = TRUE;
    /* Clear the window */
    tick = winboloTimer();
    if (GetClientRect( appWnd, &rcWindow )) {
      if (ClientToScreen( appWnd, ( LPPOINT )&rcWindow )) {
        drawMainScreenBlack(&rcWindow);
      }
    }
    dwSysFrame += (winboloTimer() - tick);
  }
}

/*********************************************************
*NAME:          frontEndEnableRequestAllyMenu
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Request to enable/disable the request alliance menu
* item
* 
*ARGUMENTS:
*  enabled - TRUE for enabled/FALSE for diabled
*********************************************************/
void frontEndEnableRequestAllyMenu(bool enabled) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(appWnd);
  if (enabled == TRUE) {
    EnableMenuItem(hMenu, ID_BOLOMENU_REQUESTALLIENCE, MFS_ENABLED);  
  } else {
    EnableMenuItem(hMenu, ID_BOLOMENU_REQUESTALLIENCE, MF_GRAYED);  
  }
}

/*********************************************************
*NAME:          frontEndEnableRequestMenu
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Request to enable/disable the leave alliance menu item
* 
*ARGUMENTS:
*  enabled - TRUE for enabled/FALSE for diabled
*********************************************************/
void frontEndEnableLeaveAllyMenu(bool enabled) {
  HMENU hMenu;   /* The menu of the Window */

  hMenu = GetMenu(appWnd);
  if (enabled == TRUE) {
    EnableMenuItem(hMenu, ID_BOLOMENU_LEAVEALLIENCE, MFS_ENABLED);
  } else {
    EnableMenuItem(hMenu, ID_BOLOMENU_LEAVEALLIENCE, MF_GRAYED);
  }
}

/*********************************************************
*NAME:          frontEndShowGunsight
*AUTHOR:        John Morrison
*CREATION DATE: 4/1/99
*LAST MODIFIED: 4/1/99
*PURPOSE:
* Set the front end whether the gunsight is visible or not
* (Called by auto show/hide gunsight being triggered)
* 
*ARGUMENTS:
*  isShown - Is the gunsight shown or not
*********************************************************/
void frontEndShowGunsight(bool isShown) {
  showGunsight = !isShown;
  windowShowGunsight(appWnd);
}


bool frontEndTutorial(BYTE pos) {
  static BYTE upTo = 0;
  bool returnValue; /* Value to return */

  
  returnValue = FALSE;
  if (isTutorial == TRUE) {
    switch (upTo) {
    case 0:
      if (pos == 208) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL01), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 1:
      if (pos == 197) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL02), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND );        
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 2:
      if (pos == 192) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL03), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND );        
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 3:
      if (pos == 186) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL04), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 4:
      if (pos == 181) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL05), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 5:
      if (pos == 175) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL06), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 6:
      if (pos == 166) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL07), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 7:
      if (pos == 159) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL08), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL09), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 8:
      if (pos == 1) {
//        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL09), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
      }
      break;
    case 9:
      if (pos == 142) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL10), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL11), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 10:
      if (pos == 1) {
//        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL11), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
      }
      break;
    case 11:
      if (pos == 122) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL12), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 12:
      if (pos == 120) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL13), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL14), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 13:
      if (pos == 1) {
//        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL14), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
      }
      break;
    case 14:
      if (pos == 110) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL15), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 15:
      if (pos == 103) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL16), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 16:
      if (pos == 98) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL17), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 17:
      if (pos == 84) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL18), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL19), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 18:
      if (pos == 1) {
//        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL19), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
      }
      break;
    case 19:
      if (pos == 66) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL20), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL21), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 20:
      if (pos == 1) {
//        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL21), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
      }
      break;
    case 21:
      if (pos == 47) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL22), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL23), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL24), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        upTo++;
        upTo++;
        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    case 22:
      if (pos == 1) {
//        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL23), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
      }
      break;
    case 23:
      if (pos == 1) {
//        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL24), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
      }
      break;
    case 24:
      if (pos == 21) {
        doingTutorial = TRUE;
        clientMutexRelease();
        MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL25), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL );        
        returnValue = TRUE;
        upTo++;
        timeKillEvent(timerGameID);

        clientMutexWaitFor();
        doingTutorial = FALSE;
        oldTick = winboloTimer();
        ttick = oldTick;
      }
      break;
    default:
      break;
    }
  }
//  timerGameID = timeSetEvent(GAME_TICK_LENGTH, 10, windowGameTimer, 0, TIME_PERIODIC);

  return returnValue;
}

void windowStartTutorial() {

  autoScrollingEnabled = FALSE;
  showGunsight = FALSE;
  windowAutomaticScrolling(appWnd);
  windowShowGunsight(appWnd);

  MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL_START01), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND);
  MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL_START02), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND);
  MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL_START03), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND);
  MessageBoxA(appWnd, (LPCTSTR) langGetText(STR_TUTORIAL_START04), DIALOG_BOX_TITLE, MB_ICONINFORMATION | MB_SETFOREGROUND);
  doingTutorial = FALSE;
  oldTick = winboloTimer();
  ttick = oldTick;
}

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
bool windowShowAllianceRequest() {
  bool returnValue; /* Value to reutn */
  returnValue = FALSE;

  if (GetMenuState(GetMenu(appWnd), ID_BOLOMENU_REQUESTALLIANCE, 0) == MF_CHECKED) {
    returnValue = TRUE;
  }

  return returnValue;
}

/*********************************************************
*NAME:          windowShowAllianceRequest
*AUTHOR:        John Morrison
*CREATION DATE: 03/02/03
*LAST MODIFIED: 03/02/03
*PURPOSE:
* Sets whether the Allow Player Name Change menu item is
* enabled
*
*ARGUMENTS:
* allow - TRUE to enable/False to disable
*********************************************************/
void windowAllowPlayerNameChange(bool allow) {
  if (allow == TRUE) {
    EnableMenuItem(GetMenu(appWnd), ID_BOLOMENU_CHANGENAME, MFS_ENABLED);
  } else {
    EnableMenuItem(GetMenu(appWnd), ID_BOLOMENU_CHANGENAME, MF_GRAYED);
  }
}

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
void windowReCreate() {
  HMENU ret;

  ret = GetMenu(appWnd);
  DestroyMenu(ret);
  SetMenu(appWnd, langGetMenu());
  if (appWnd != NULL) {
    brainsHandlerLoadBrains(appWnd);
  } else {
    MessageBoxA(NULL, "Bad", "very", MB_OK);
  }
}

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
void windowApplyMenuChecks() {
  allowNewPlayers = TRUE;
  /* Tank Labels  */
  windowSetTankLabelLen(appWnd, labelTank); 
  /* Frame Rate */
  windowSetFrameRate(appWnd, frameRate, FALSE);
  /* Window Size */
  windowZoomChange(zoomFactor);
  /* Automatic scrolling */
  autoScrollingEnabled = !autoScrollingEnabled;
  windowAutomaticScrolling(appWnd);
  /* Gunsight */
  showGunsight = !showGunsight;
  windowShowGunsight(appWnd);
  /* Message Labels */
  windowSetMessageLabelLen(appWnd, labelMsg);
  labelSelf = !labelSelf;
  windowLabelOwnTank(appWnd);
  /* Pills and base labels */
  showPillLabels = !showPillLabels;
  windowShowPillLabels(appWnd);
  showBaseLabels = !showBaseLabels;
  windowShowBaseLabels(appWnd);

  /* Sound Effects */
  if (soundIsPlayable() == FALSE) {
    /* Disable windows */
    windowDisableSound();
  } else {
    soundEffects = !soundEffects;
    windowSoundEffects(appWnd);
    backgroundSound = !backgroundSound;
    windowBackgroundSoundChange(appWnd, FALSE);
    isISASoundCard = !isISASoundCard;
    windowISASoundCard();
  }
  showNewswireMessages = !showNewswireMessages;
  showAssistantMessages = !showAssistantMessages;
  showAIMessages = !showAIMessages;
  showNetworkStatusMessages = !showNetworkStatusMessages;
  showNetworkDebugMessages = !showNetworkDebugMessages;
  windowMenuNewswire(appWnd);
  windowMenuAssistant(appWnd);
  windowMenuAI(appWnd);
  windowMenuNetwork(appWnd);
  windowMenuNetworkDebug(appWnd);
  if (netGetType() == netSingle) {
    screenSetGunsight(showGunsight);
  }
  if (screenGetAiType() == aiNone) {
    brainsHandlerSet(windowWnd(), FALSE);
  } else {
    brainsHandlerSet(windowWnd(), TRUE);
  }
  screenSetPlayersMenu();
}

time_t windowsGetTicks() {
  return ticks;
}


BOOL CALLBACK winboloCCCallback (HWND hWnd, LPARAM lParam) {
  char check[256];
  char test1[64];
  char test2[64];
  char test3[64];
  char test4[64];
  char test5[64];
  char test6[64];
  bool cont = TRUE;

  GetWindowText(hWnd, check, 256);
  test1[0] = 'S';
  test1[1] = 'p';
  test1[2] = 'e';
  test1[3] = 'e';
  test1[4] = 'd';
  test1[5] = 'e';
  test1[6] = 'r';
  test1[7] = 'X';
  test1[8] = 'P';
  test1[9] = 0;

  test2[0] = 'S';
  test2[1] = 'p';
  test2[2] = 'e';
  test2[3] = 'e';
  test2[4] = 'd';
  test2[5] = ' ';
  test2[6] = 'G';
  test2[7] = 'e';
  test2[8] = 'a';
  test2[9] = 'r';
  test2[10] = 0;

  test3[0] = '?';
  test3[1] = '?';
  test3[2] = '?';
  test3[3] = '?';
  test3[4] = ' ';
  test3[5] = 'F';
  test3[6] = 'o';
  test3[7] = 'r';
  test3[8] = ' ';
  test3[9] = 'N';
  test3[10] = 'T';
  test3[11] = 0;

  test4[0] = 'U';
  test4[1] = 'l';
  test4[2] = 't';
  test4[3] = 'i';
  test4[4] = 'm';
  test4[5] = 'a';
  test4[6] = 't';
  test4[7] = 'e';
  test4[8] = ' ';
  test4[9] = 'S';
  test4[10] = 'p';
  test4[12] = 'e';
  test4[13] = 'e';
  test4[14] = 'd';
  test4[15] = ' ';
  test4[16] = 'H';
  test4[17] = 'a';
  test4[18] = 'c';
  test4[19] = 'k';
  test4[20] = 0;

  test5[0] = 'S';
  test5[1] = 'p';
  test5[2] = 'e';
  test5[3] = 'e';
  test5[4] = 'd';
  test5[5] = 'h';
  test5[6] = 'a';
  test5[7] = 'c';
  test5[8] = 'k';
  test5[9] = 0;

  test6[0] = 'A';
  test6[1] = ' ';
  test6[2] = 'S';
  test6[3] = 'p';
  test6[4] = 'e';
  test6[5] = 'e';
  test6[6] = 'd';
  test6[7] = 'e';
  test6[8] = 'r';
  test6[9] = 0;


  if (strncmp(check, test1, strlen(test1)) == 0) {
    cont = FALSE;
  } else if (strncmp(check, test2, strlen(test2)) == 0) {
    cont = FALSE;
  } else if (strncmp(check, test3, strlen(test3)) == 0) {
    cont = FALSE;
  } else if (strncmp(check, test4, strlen(test4)) == 0) {
    cont = FALSE;
  } else if (strncmp(check, test5, strlen(test5)) == 0) {
    cont = FALSE;
  } else if (strncmp(check, test6, strlen(test6)) == 0) {
    cont = FALSE;
  }

  if (cont == FALSE) {
    clientMutexWaitFor();
    screenSendMem();
    clientMutexRelease();
  }
  return cont;

}

int winboloCC() {
  if (netGetType() != netSingle) {
    EnumWindows(winboloCCCallback, 0);
  }
  return 0;
}

