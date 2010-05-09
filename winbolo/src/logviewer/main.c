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


// This is the main file and contains most of the windows code exluding sound/draw
//
// URL underlining and hand subclassing derived from: http://www.wischik.com/lu/programmer/setdlgitemurl.html

#include <windows.h>
#include <winuser.h>
#include <string.h>
#include <commdlg.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <time.h>
#include "backend.h"
#include "global.h"
#include "clientmutex.h"
#include "dialogwindowsize.h"
#include "dialogfullscreensetup.h"
#include "draw.h"
#include "sound.h"
#include "font.h"
#include "dns.h"
#include "positions.h"
#include "resource.h"


#define UWM_ADDSTRING (WM_APP + 1)


/* controlsEnable() paramters for enabling the control buttons */ 
#define STATE_STOPPED 0
#define STATE_PLAYING 1
#define STATE_PAUSED 2
#define STATE_ENDOFLOG 3
#define STATE_STARTOFLOG 4

/* Converts 0 = MF_UNCHECKED, 1 == MF_CHECKED for menu checking
   Useful converting Mac's CheckItem menu function call which takes
   0 and 1's for checked arguments */
#define MENU_CHECK_WIN32(X) ((X==0) ? (MF_UNCHECKED) : (MF_CHECKED))
#define MENU_ENABLE_WIN32(X) ((X==0) ? (MF_GRAYED) : (MF_ENABLED))

void windowPlay();
void windowPause();
void windowStop(bool corruptLog);
void screenTankCentred(bool enabled);
void start();
BOOL CALLBACK dialogWindowTeamColoursCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );
void controlsEnable(int state);
void controlEnable(int id, bool state, UINT menuId);
void mainSavePrefernces();





/* Team Colours array */
BYTE tc[17]; // MAX_PLAYERS+1
/* Window Handles */
HWND hMainWnd, hGameEventsWnd, hControlsWnd, hGameInfoWnd, hItemInfoWnd;HMENU hPopupMenu;
/* Old Window Processing Handles */
FARPROC oldWndProc;
FARPROC oldURLLabelProc;
FARPROC oldButtonProc;
/* Application instance */
HINSTANCE appInst;
/* Timers for game id and timers */
UINT timerGameID = 0;
UINT timerFrameID = 0;
unsigned int timerGameInfo = 3; /* The timer for getting the game time remaining etc.*/
HFONT hFont = NULL; // Underlined font for URL clicky clicky
bool playIsPlaying = FALSE;
bool isLoaded = FALSE;
bool isQuitting;
bool isSoundsPlaying;
BYTE dlgGameInfoNumPlayers;
long dlgGameInfoTimeRemain;
long dlgGameInfoStartDelay;
bool wantScreenUpdate = FALSE;
bool doubleSpeed = FALSE;
bool useTeamColours = FALSE;
BYTE speed;
bool useFullscreen;
int timerSleep;

void frontEndPlaySound(sndEffects value) {
  if (isSoundsPlaying == TRUE) {
    soundPlayEffect(value);
  }
}


// itemTypes
// 0 = clear
// 1 = base
// 2 = pillbox
void updateItem(BYTE itemType, BYTE itemNumber, BYTE owner, BYTE x, BYTE y, BYTE armour, BYTE shells, BYTE mines, bool inTank) {
  static BYTE oldItemType = 5;  // Force this to be set on first call
  static BYTE oldItemNumber = 0;
  static BYTE oldOwner = 0;
  static BYTE oldX = 0;
  static BYTE oldY = 0;
  static BYTE oldArmour = 0;
  static BYTE oldShells = 0;
  static BYTE oldMines = 0;
  static bool oldInTank = 33; // Force to be set as well
  char line[256];
  char player[64];

  if (itemType == 0) {
    if (oldItemType != 0) {
      oldItemType = 0;
      oldX = 0;
      oldY = 0;
      oldArmour = 0;
      oldShells = 0;
      oldMines = 0;
      oldInTank = 3;
      oldOwner = 0;
      oldItemNumber = 100;
      SetDlgItemText(hItemInfoWnd, IDC_ITEM_TYPE, "");
      SetDlgItemText(hItemInfoWnd, IDC_LOCATION, "");
      SetDlgItemText(hItemInfoWnd, IDC_ITEM_OWNER, "");
      SetDlgItemText(hItemInfoWnd, IDC_ITEM_ARMOUR, "");
      SetDlgItemText(hItemInfoWnd, IDC_ITEM_SHELLS, "");
      SetDlgItemText(hItemInfoWnd, IDC_ITEM_MINES, "");
      SetDlgItemText(hItemInfoWnd, IDC_ITEM_INTANK, "");
      EnableWindow(GetDlgItem(hItemInfoWnd, IDC_CENTRE_ON_MAP), FALSE);
      ShowWindow(GetDlgItem(hItemInfoWnd, IDC_BASE), SW_HIDE);
      ShowWindow(GetDlgItem(hItemInfoWnd, IDC_PILLBOX), SW_HIDE);
    }
  } else if (itemType != oldItemType) {
    oldItemType = itemType;
    if (itemType == 1) {
      // Base
      ShowWindow(GetDlgItem(hItemInfoWnd, IDC_BASE), SW_SHOW);
      ShowWindow(GetDlgItem(hItemInfoWnd, IDC_PILLBOX), SW_HIDE);
//      SetDlgItemText(hItemInfoWnd, IDC_ITEM_TYPE, "Base");
    } else {
      // Pillbox
      ShowWindow(GetDlgItem(hItemInfoWnd, IDC_BASE), SW_HIDE);
      ShowWindow(GetDlgItem(hItemInfoWnd, IDC_PILLBOX), SW_SHOW);
   //   SetDlgItemText(hItemInfoWnd, IDC_ITEM_TYPE, "Pillbox");
      SetDlgItemText(hItemInfoWnd, IDC_ITEM_SHELLS, "");
      SetDlgItemText(hItemInfoWnd, IDC_ITEM_MINES, "");

      oldMines = 0;
      oldShells = 0;
    }
  }


  if (oldItemNumber != itemNumber && itemType != 0) {
    oldItemNumber = itemNumber;
    SetDlgItemInt(hItemInfoWnd, IDC_ITEM_TYPE, itemNumber, FALSE);
  }

  // Show rest of items only if they have changed
  if (owner != oldOwner) {
    oldOwner = owner;
    if (owner == NEUTRAL) {
      strcpy(player, "Neutral");
    } else {
      screenGetPlayerName(player, owner);
    }
    sprintf(line, "%s (%d)", player, owner);
    SetDlgItemText(hItemInfoWnd, IDC_ITEM_OWNER, line);
  }

  if (x != oldX || y != oldY) {
    oldX = x;
    oldY = y;
    sprintf(line, "X: %d, Y: %d", x, y);
    SetDlgItemText(hItemInfoWnd, IDC_LOCATION, line);
  }
  if (oldArmour != armour) {
    oldArmour = armour;
    sprintf(line, "%d", armour);
    SetDlgItemText(hItemInfoWnd, IDC_ITEM_ARMOUR, line);
  }
  if (oldShells != shells) {
    oldShells = shells;
    sprintf(line, "%d", shells);
    SetDlgItemText(hItemInfoWnd, IDC_ITEM_SHELLS, line);
  }
  if (oldMines != mines) {
    oldMines = mines;
    sprintf(line, "%d", mines);
    SetDlgItemText(hItemInfoWnd, IDC_ITEM_MINES, line);
  }
  if (oldInTank != inTank && itemType != 0) {
    oldInTank = inTank;
    if (inTank == TRUE) {
      SetDlgItemText(hItemInfoWnd, IDC_ITEM_INTANK, "Yes");
      EnableWindow(GetDlgItem(hItemInfoWnd, IDC_CENTRE_ON_MAP), FALSE);

    } else {
      SetDlgItemText(hItemInfoWnd, IDC_ITEM_INTANK, "No");
      EnableWindow(GetDlgItem(hItemInfoWnd, IDC_CENTRE_ON_MAP), TRUE);

    }
  }


}


void screenRequestUpdate();

// Called once a second to update things now every 250 ms for screen paints (when not running)
void updateSecond() {
  static long lastSeconds = -1;
  BYTE numPlayers;
  char line[256];

  if (isLoaded == TRUE) {
    screenRequestUpdate();
    clientMutexWaitFor();
    numPlayers = screenGetNumPlayers();
    dlgGameInfoTimeRemain = screenGetGameTimeLeft();
    dlgGameInfoStartDelay = screenGetGameStartDelay();
    clientMutexRelease();

    if (dlgGameInfoTimeRemain != UNLIMITED_GAME_TIME) {
      dlgGameInfoTimeRemain /= NUM_SECONDS_MINUTE;
      dlgGameInfoTimeRemain /= GAME_NUMGAMETICKS_SEC;
      dlgGameInfoTimeRemain++;
      sprintf(line, "About %d minutes", dlgGameInfoTimeRemain );
      SetDlgItemText(hGameInfoWnd, IDC_GAME_TIMELIMIT, line);
    }

    if (dlgGameInfoStartDelay > 0) {
      dlgGameInfoStartDelay /= GAME_NUMGAMETICKS_SEC; /* Convert ticks back to seconds */
      sprintf(line, "%d second", dlgGameInfoStartDelay);
      if (dlgGameInfoStartDelay > 0) {
        strcat(line, "s");
      }
      SetDlgItemText(hGameInfoWnd, IDC_GAME_STARTDELAY, line);
    } else if (dlgGameInfoStartDelay == 0 && lastSeconds != 0) {
      lastSeconds = 0;
      SetDlgItemText(hGameInfoWnd, IDC_GAME_STARTDELAY, "");
    }

    if (numPlayers != dlgGameInfoNumPlayers) {
      dlgGameInfoNumPlayers = numPlayers;
      sprintf(line, "%d", numPlayers);
      SetDlgItemText(hGameInfoWnd, IDC_NUM_PLAYERS, line);
    }
    screenGetTime(line);
    SetDlgItemText(hControlsWnd, IDC_CURRENT_TIME, line);

    if (wantScreenUpdate == TRUE && playIsPlaying == FALSE) {
      clientMutexWaitFor();
      screenUpdate(redraw);
      clientMutexRelease();
      wantScreenUpdate = FALSE;
    }
  }
}

void frontEndSetGameInformation(bool clear, BYTE versionMajor, BYTE versionMinor, BYTE versionRevision, char *mapName, BYTE gameType, bool hiddenMines, BYTE aiType, long startDelay, long timeLimit, BYTE *wbnKey, long startTime) {
  char line[256];
  char *timestring, *ptr;     /* Pointers to print out game time things */

  SetDlgItemText(hGameInfoWnd, IDC_NUM_PLAYERS,"0");
  if (clear == TRUE) {
    SetDlgItemText(hGameInfoWnd, IDC_VERSION,"");
    SetDlgItemText(hGameInfoWnd, IDC_MAP_NAME, "");
    SetDlgItemText(hGameInfoWnd, IDC_GAME_TYPE, "");
    SetDlgItemText(hGameInfoWnd, IDC_HIDDEN_MINES, "");
    SetDlgItemText(hGameInfoWnd, IDC_COMPUTER_TANKS, "");
    SetDlgItemText(hGameInfoWnd, IDC_GAME_TIMELIMIT, "");
    SetDlgItemText(hGameInfoWnd, IDC_GAME_STARTDELAY, "");
    SetDlgItemText(hGameInfoWnd, IDC_WBN, "                 ");
    SetDlgItemText(hGameInfoWnd, IDC_WBN, "");
    SetDlgItemText(hGameInfoWnd, IDC_START_TIME, "");
    SetDlgItemText(hGameInfoWnd, IDC_NUM_PLAYERS , "");


  } else {
    line[0] = '\0';
    sprintf(line, "%d.%d%d", versionMajor, versionMinor, versionRevision);
    SetDlgItemText(hGameInfoWnd, IDC_VERSION,line);
    SetDlgItemText(hGameInfoWnd, IDC_MAP_NAME, mapName);
    /* Number of players */
    dlgGameInfoNumPlayers = 0;
    sprintf(line, "%d", dlgGameInfoNumPlayers);
    SendDlgItemMessage(hGameInfoWnd, IDC_NUM_PLAYERS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (line));
    /* Game type */
    switch (gameType) {
    case gameOpen:
      SendDlgItemMessage(hGameInfoWnd, IDC_GAME_TYPE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "Open");
      break;
    case gameTournament:
      SendDlgItemMessage(hGameInfoWnd, IDC_GAME_TYPE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "Tournament");
      break;
    default:
      /* gameStrictTournament */
      SendDlgItemMessage(hGameInfoWnd, IDC_GAME_TYPE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "Strict Tournament");
      break;
    }

    /* Hidden mines */
    if (hiddenMines == TRUE) {
      SendDlgItemMessage(hGameInfoWnd, IDC_HIDDEN_MINES, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "Yes");
    } else {
      SendDlgItemMessage(hGameInfoWnd, IDC_HIDDEN_MINES, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "No");
    }
    /* Computer players */
    switch (aiType) {
    case aiNone:
      SendDlgItemMessage(hGameInfoWnd, IDC_COMPUTER_TANKS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "No");
      break;
    case aiYes:
      SendDlgItemMessage(hGameInfoWnd, IDC_COMPUTER_TANKS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "Yes");
      break;
    default:
      /* aiYesAdvantage */
      SendDlgItemMessage(hGameInfoWnd, IDC_COMPUTER_TANKS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "Yes (Advantage)");
      break;
    }

    /* Time remaining */
    dlgGameInfoTimeRemain = timeLimit;
    dlgGameInfoStartDelay = startDelay;
    if (dlgGameInfoTimeRemain == UNLIMITED_GAME_TIME) {
      SendDlgItemMessage(hGameInfoWnd, IDC_GAME_TIMELIMIT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "Unlimited");
    }

    ptr = timestring = asctime(gmtime(&startTime));
    while (*ptr && *ptr != '\n') {
      ptr++;
    }
    *ptr = 0;
    sprintf(line, "%s GMT", timestring);
    SendDlgItemMessage(hGameInfoWnd, IDC_START_TIME, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) line);
 

    /* WBN */
    if (wbnKey[0] == '\0') {
      SetDlgItemText(hGameInfoWnd, IDC_WBN, "");
    } else {
      strncpy(line, wbnKey, 32);
      line[32] = '\0';
      SetDlgItemText(hGameInfoWnd, IDC_WBN, line);
    }
  }
}

BOOL CALLBACK dialogItemInfoCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
  switch ( uMsg ) {
  case WM_PAINT:
    break;
  case WM_CLOSE:
    CheckMenuItem(GetMenu(hMainWnd), ID_WINDOWS_ITEMINFORMATION, MF_UNCHECKED);
    ShowWindow(hWnd, SW_HIDE);
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
      case IDC_CENTRE_ON_MAP:
        screenCentreOnSelectedItem();
        break;
    }
    break;
  case WM_DESTROY:
    ShowWindow(hWnd, SW_HIDE);
    if (isQuitting == TRUE) {
      return EndDialog(hWnd, TRUE);
    }
    break;
  }

  return FALSE;
}

BOOL CALLBACK dialogEventsCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
  char *mem;
  int len;

  switch ( uMsg ) {
  case WM_PAINT:
    break;
  case WM_CLOSE:
    ShowWindow(hWnd, SW_HIDE);
    CheckMenuItem(GetMenu(hMainWnd), ID_WINDOWS_EVENTS, MF_UNCHECKED);
    break;
  case WM_DESTROY:
    ShowWindow(hWnd, SW_HIDE);
    if (isQuitting == TRUE) {
      return EndDialog(hWnd, TRUE);
    }
    break;
  case UWM_ADDSTRING:
    mem = (char *) lParam;
    SendDlgItemMessage(hWnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM) mem);
    len = SendDlgItemMessage(hWnd, IDC_LIST, LB_GETCOUNT, 0, 0);
    SendDlgItemMessage(hWnd, IDC_LIST, LB_SETTOPINDEX, len-1, 0);
    free(mem);
    break;
  }
  return FALSE;
}

BOOL CALLBACK forwardButtonCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  switch (uMsg) {
    case WM_LBUTTONDOWN:
      doubleSpeed = TRUE;
      break;
    case WM_LBUTTONUP:
      doubleSpeed = FALSE;
      break;
    } 
  return CallWindowProc((WNDPROC)oldButtonProc, hWnd, uMsg, wParam, lParam);
}


BOOL CALLBACK urlClickableCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  char line[256];
  switch (uMsg) {
    case WM_SETCURSOR:
      SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(32649)));
      return TRUE;
    case WM_NCHITTEST:
      GetDlgItemText(hGameInfoWnd, IDC_WBN, line, 256);
      if (strlen(line) == 32) {
        return HTCLIENT; // because normally a static returns HTTRANSPARENT, so disabling WM_SETCURSOR
      }
      return HTTRANSPARENT;
    } 
  return CallWindowProc((WNDPROC)oldURLLabelProc, hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK copyClipboardCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  static bool ctrlDown = FALSE;
  HANDLE cbHandle; /* handle to the clipboard data */
  char *dataPtr;   /* pointer to clipboard data */
  int count = 0;   /* Looping variable */
  int numItems ;
  int xPos;
  int yPos;
  HWND hCtlWnd;
  char line[512];
  RECT rcWindow;
  int *selections;
  long neededMemory;
  long memoryPos;

  switch (uMsg) {
  case WM_RBUTTONDOWN:
    xPos = LOWORD(lParam);  // horizontal position of cursor 
    yPos = HIWORD(lParam);  // vertical position of cursor 
    if (GetClientRect( hWnd, &rcWindow )) {
      if (ClientToScreen( hWnd, ( LPPOINT )&rcWindow )) {
        hCtlWnd = GetDlgItem(hGameEventsWnd, IDC_LIST);
        numItems = SendMessage(hCtlWnd, LB_GETCOUNT, 0, 0);
        EnableMenuItem(hPopupMenu, ID_COPY_COPY, MF_GRAYED);
        if (numItems > 0) {
          EnableMenuItem(hPopupMenu, ID_COPY_SELECTALL, MF_ENABLED);
          if (SendMessage(hCtlWnd, LB_GETSELCOUNT, 0, 0) > 0) {
            EnableMenuItem(hPopupMenu, ID_COPY_COPY, MF_ENABLED);
          }
        } else {
          EnableMenuItem(hPopupMenu, ID_COPY_SELECTALL, MF_GRAYED);
        }
        
        TrackPopupMenu(GetSubMenu(hPopupMenu, 0), 0, rcWindow.left+xPos, rcWindow.top+yPos, 0, hWnd, NULL);
      }
    }
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case ID_COPY_SELECTALL:
      hCtlWnd = GetDlgItem(hGameEventsWnd, IDC_LIST);
      SendMessage(hCtlWnd, LB_SELITEMRANGE, TRUE, MAKELPARAM(0,  numItems-1));
      break;
    case ID_COPY_COPY:
      hCtlWnd = GetDlgItem(hGameEventsWnd, IDC_LIST);
      numItems = SendMessage(hCtlWnd, LB_GETSELCOUNT, 0, 0);
      if (numItems > 0) {
        selections = malloc(sizeof(int) * numItems);
        if (selections != NULL) {
          numItems = SendMessage(hCtlWnd, LB_GETSELITEMS, numItems, (LPARAM) selections);
          if (numItems > 0) {
            // We have some selected items. Figure out how much memory we need to copy them
            neededMemory = 0;
            count = 0;
            while (count < numItems) {
              SendMessage(hWnd, LB_GETTEXT, selections[count], (LPARAM) line);
              neededMemory += SendMessage(hCtlWnd, LB_GETTEXTLEN, selections[count], 0) + 2;
              count++;
            }
            neededMemory++; // Add NULL to end
            if (OpenClipboard(hWnd)) {
              EmptyClipboard();
              cbHandle = GlobalAlloc(GMEM_MOVEABLE, neededMemory);
              if (cbHandle != NULL) {
                dataPtr = GlobalLock(cbHandle);
                count = 0;
                memoryPos = 0;
                while (count < numItems) {
                  memoryPos += SendMessage(hCtlWnd, LB_GETTEXT, selections[count], (LPARAM) dataPtr+memoryPos);
                  *(dataPtr+memoryPos) = '\r';
                  memoryPos++;
                  *(dataPtr+memoryPos) = '\n';
                  memoryPos++;
                  count++;
                }
                *(dataPtr+memoryPos) = '\0';
                GlobalUnlock(cbHandle);
                SetClipboardData(CF_TEXT, cbHandle);
              }
              CloseClipboard();
            }
          }
          free(selections);
        }
      }
      break;
    }

    break;
  }
  return CallWindowProc((WNDPROC)oldWndProc, hWnd, uMsg, wParam, lParam);
}



BOOL CALLBACK dialogGameInfoCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
  HDC hDC;
  HWND HDlgItemWnd;
  char line[256];
  char var[256];
  

  switch ( uMsg ) {
  case WM_INITDIALOG:
      timerGameInfo = SetTimer(hWnd, timerGameInfo, 250, NULL);
      break;
  case WM_PAINT:
    break;
	case WM_CTLCOLORSTATIC:
		hDC = (HDC) wParam;
		HDlgItemWnd = (HWND) lParam;
		if (hDC && HDlgItemWnd == GetDlgItem(hGameInfoWnd, IDC_WBN)) {
      // Make the font
      if (hFont == NULL) {
        TEXTMETRIC tm; 
        LOGFONT lf;
        GetTextMetrics(hDC,&tm);
        lf.lfHeight=tm.tmHeight;
        lf.lfWidth=0;
        lf.lfEscapement=0;
        lf.lfOrientation=0;
        lf.lfWeight=tm.tmWeight;
        lf.lfItalic=tm.tmItalic;
        lf.lfUnderline=TRUE;
        lf.lfStrikeOut=tm.tmStruckOut;
        lf.lfCharSet=tm.tmCharSet;
        lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
        lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
        lf.lfQuality=DEFAULT_QUALITY;
        lf.lfPitchAndFamily=tm.tmPitchAndFamily;
        GetTextFace(hDC,LF_FACESIZE,lf.lfFaceName);
        hFont=CreateFontIndirect(&lf);
      }
      SelectObject(hDC,hFont);
      
      SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, RGB(0,0, 255));
			return (int) GetStockObject(NULL_BRUSH);
		}
		break;
  case WM_TIMER:
    // Process stuff that happens every second
    updateSecond();
    break;
  case WM_CLOSE:
    CheckMenuItem(GetMenu(hMainWnd), ID_WINDOWS_GAMEINFORMATION, MF_UNCHECKED);
    ShowWindow(hWnd, SW_HIDE);
    break;
  case WM_DESTROY:
    ShowWindow(hWnd, SW_HIDE);
    if (isQuitting == TRUE) {
      KillTimer(hWnd, timerGameInfo);
      return EndDialog(hWnd, TRUE);
    }
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
      case IDC_WBN:
        GetDlgItemText(hWnd, IDC_WBN, var, 256);
        if (strlen(var) == 32) {
          sprintf(line, "http://www.winbolo.net/gamelog.php?key=%s", var);
          ShellExecute(NULL, "open", line, NULL, NULL, SW_SHOW);
        }
        break;
    }

  }
  return FALSE;
}

void screenRewind();
void screenFastForward();


void windowFastForward() {
  clientMutexWaitFor();
  drawDirtyScreen();
  screenFastForward();
//  screenGetTime(line);
  drawDirtyScreen();
  screenUpdate(redraw);

//  screenUpdate(redraw);
  
  clientMutexRelease();
  controlEnable(ID_REW, TRUE, ID_ACTION_REWIND);
/*  if (playIsPlaying == TRUE) {
    controlsEnable(STATE_PLAYING);
  } else {
    controlsEnable(STATE_PAUSED);
    wantScreenUpdate = TRUE;
  }
  if (strcmp("00:00", line) != 0) {
    controlEnable(ID_REW, TRUE);
  }
*/
}

void windowRewind() {
  char line[64];
  
  clientMutexWaitFor();
  drawDirtyScreen();
  screenRewind();
  drawDirtyScreen();
  screenUpdate(redraw);

  screenGetTime(line);
// jm chend here
  clientMutexRelease();
  if (playIsPlaying == TRUE) {
    controlsEnable(STATE_PLAYING);
  } else {
    controlsEnable(STATE_PAUSED);
    wantScreenUpdate = TRUE;
  }
  if (strcmp("00:00", line) == 0) {
    controlEnable(ID_REW, FALSE, ID_ACTION_REWIND);
  }
}




// Function that creates a trackbar.
// Parameters:
//    HWND hWndParent - Parent window of the trackbar.
//    DWORD dwStyles - Window styles for the trackbar.
//    WORD wID - ID of the trackbar.
//    HINSTANCE hInst - Current instance.
//    RECT rclTrack - Size and position of the trackbar.
//    LONG lPageSize - Sets the page size; use 0 to ignore.
//    LONG lPosition - Sets the initial position.
//    LONG lRange - Sets the range.
//    LONG lFreq - Sets the frequency of ticks; use 0 to ignore.
//    LONG lSelection - Sets the selection; use 0 to ignore.
//    BOOL bRedraw - Redraw flag: TRUE to redraw; otherwise, FALSE.
//
HWND myCreateTrackBar ( HWND hWndParent, DWORD dwStyles, WORD wID, HINSTANCE 
   hInst, RECT rclTrack, LONG lPageSize, LONG lPosition, LONG lRange, 
   LONG lFreq, LONG lSelection, BOOL bRedraw)
{

  HWND hWndSlider;

  // Create the trackbar.
  hWndSlider  = CreateWindowEx(
    0L,              // No extended styles.
    TRACKBAR_CLASS,  // Trackbar class name.

    "",              // No default text.
    WS_CHILD | WS_VISIBLE | dwStyles,   // Window styles.
    rclTrack.left, rclTrack.top, rclTrack.right, rclTrack.bottom,   // Size and position.
    hWndParent,      // Parent window handle.
    (HMENU)wID,      // ID for the trackbar.
    hInst,           // Current instance.
    NULL);           // No class data.



  if (hWndSlider == NULL) {
    return NULL;
  }



  // Set the page size.

  if (lPageSize) {
      SendMessage(hWndSlider, TBM_SETPAGESIZE, 0L, lPageSize);
  }


  // Set the tick frequency.
  if (lFreq) {
      SendMessage(hWndSlider, TBM_SETTICFREQ, lFreq, 0L);
  }


  // Set the initial range.
  SendMessage(hWndSlider, TBM_SETRANGE, FALSE, lRange);

  // Set the initial position.
  SendMessage(hWndSlider, TBM_SETPOS, FALSE, lPosition);

  // Set the selection.
  if (lSelection) {
      SendMessage(hWndSlider, TBM_SETSEL, FALSE, lSelection);
  }


  // Redraw if specified.

  if (bRedraw) {
      InvalidateRect(hWndSlider, NULL, TRUE);
      UpdateWindow(hWndSlider);

  }
  return hWndSlider;

}


void updateSpeed(BYTE speed, bool updateSlider) {
// Speed
  switch (speed) {
  case 2:
    SetDlgItemText(hControlsWnd, IDC_SPEED, "1.2x");
    timerSleep = 18;
    break;
  case 3:
    SetDlgItemText(hControlsWnd, IDC_SPEED, "1.4x");
    timerSleep = 16;
    break;
  case 4:
    SetDlgItemText(hControlsWnd, IDC_SPEED, "1.6x");
    timerSleep = 14;
    break;
  case 5:
    SetDlgItemText(hControlsWnd, IDC_SPEED, "1.8x");
    timerSleep = 12;
    break;
  case 6:
    SetDlgItemText(hControlsWnd, IDC_SPEED, "2x");
    timerSleep = 10;
    break;
  case 7:
    SetDlgItemText(hControlsWnd, IDC_SPEED, "2.2x");
    timerSleep = 8;
    break;
  case 8:
    SetDlgItemText(hControlsWnd, IDC_SPEED, "2.4x");
    timerSleep = 6;
    break;
  case 9:
    SetDlgItemText(hControlsWnd, IDC_SPEED, "Fast");
    timerSleep = 1;
    break;
  case 1:
  default:
    SetDlgItemText(hControlsWnd, IDC_SPEED, "1x");
    timerSleep = 20;
    speed = 1;
    break;
  }

  if (updateSlider == TRUE) {
    PostMessage(GetDlgItem(hControlsWnd, IDC_SLIDER1), TBM_SETPOS, TRUE, speed);
  }
}



BOOL CALLBACK dialogControlsCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
  HWND hScroll;
  RECT r;

  switch ( uMsg ) {
  case WM_INITDIALOG:
    r.top = 80;
    r.bottom = 40;
    r.left = 100;
    r.right = 200;
    hScroll = myCreateTrackBar ( hWnd, TBS_AUTOTICKS | TBS_BOTTOM , IDC_SLIDER1, appInst, r, 1, 1, MAKELONG(1,9), 1, MAKELONG(5,20), TRUE);

    break;
  case WM_PAINT:
    break;
  case WM_CLOSE:
    CheckMenuItem(GetMenu(hMainWnd), ID_WINDOWS_CONTROLS, MF_UNCHECKED);
    ShowWindow(hWnd, SW_HIDE);
    break;
  case WM_DESTROY:
    ShowWindow(hWnd, SW_HIDE);
    if (isQuitting == TRUE) {
      return EndDialog(hWnd, TRUE);
    }
    break;
  case WM_HSCROLL:
    speed = (BYTE) SendMessage(GetDlgItem(hWnd, IDC_SLIDER1), TBM_GETPOS, 0, 0);
    updateSpeed(speed, FALSE);
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case ID_PLAY:
      windowPlay();
      break;
    case ID_PAUSE:
      windowPause();
      break;
    case ID_REW:
      windowRewind();
      break;
    case ID_FASTFORWARD:
      windowFastForward();
      break;
    case ID_STOP:
      windowStop(FALSE);
      break;
    }
  }
  return FALSE;
}

void windowRemoveEvents() {
  char line[512]; // Line length
  char strTime[128]; // String time
  bool done = FALSE;
  HWND hWnd = GetDlgItem(hGameEventsWnd, IDC_LIST);
  int numItems = SendMessage(hWnd, LB_GETCOUNT, 0, 0);
  int count; // Looping variable

  screenGetTime(strTime);
  while (done == FALSE && numItems > 0) {
    SendMessage(hWnd, LB_GETTEXT, numItems - 1, (LPARAM) line);
    count = 0;
    while (line[count] != ' ' && line[count] != '\0') {
      count++;
    }
    line[count] = '\0';
    if (strcmp(line, strTime) > 0) {
      // remove it
      SendMessage(hWnd, LB_DELETESTRING, numItems - 1, 0);
      numItems--;
    } else {
      done = TRUE;
    }
  }
}

void windowAddEvent(int eventType, char *msg) {
  char line[512] = { '\0' };
  char *mem;
  int len;

  if (eventType == 0) {
    screenGetTime(line);
    strcat(line, " - ");
  }

  strcat(line, msg);
  len = strlen(line);
  if (len > 0) {
    mem = malloc(len+1);
    if (mem != NULL) {
      strcpy(mem, line);
      PostMessage(hGameEventsWnd, UWM_ADDSTRING, 0, (LPARAM) mem);
    }
  }
}


void controlEnable(int id, bool state, UINT menuId) {
  EnableWindow(GetDlgItem(hControlsWnd, id), state);

  if (menuId != 0) {
    EnableMenuItem(GetMenu(hMainWnd), menuId, MENU_ENABLE_WIN32(state));
  }
}

/* Enable or disable the controls 
* State 0 = Stopped
* State 1 = Playing
* State 2 = Paused
* State 3 = EOF
* State 4 = Start of file
*/
void controlsEnable(int state) {
  static int oldState = -1;

  if (oldState != state) {
    switch (state) {
    case STATE_STOPPED: /* Stopped */
      controlEnable(ID_REW, FALSE, ID_ACTION_REWIND);
      controlEnable(ID_PLAY, FALSE, ID_ACTION_PLAY);
      controlEnable(ID_PAUSE, FALSE, ID_ACTION_PAUSE);
      controlEnable(ID_STOP, FALSE, ID_ACTION_STOP);
      controlEnable(ID_FASTFORWARD, FALSE, ID_ACTION_FASTFORWARD);
      controlEnable(ID_FORWARD, FALSE, 0);
      break;
    case STATE_PLAYING: /* Playing */
      controlEnable(ID_REW, TRUE, ID_ACTION_REWIND);
      controlEnable(ID_PLAY, FALSE, ID_ACTION_PLAY);
      controlEnable(ID_PAUSE, TRUE, ID_ACTION_PAUSE);
      controlEnable(ID_STOP, TRUE, ID_ACTION_STOP);
      controlEnable(ID_FASTFORWARD, TRUE, ID_ACTION_FASTFORWARD);
      controlEnable(ID_FORWARD, TRUE, 0);
      break;
    case STATE_PAUSED: /* Paused */
      controlEnable(ID_REW, TRUE, ID_ACTION_REWIND);
      controlEnable(ID_PLAY, TRUE, ID_ACTION_PLAY);
      controlEnable(ID_PAUSE, FALSE, ID_ACTION_PAUSE);
      controlEnable(ID_STOP, TRUE, ID_ACTION_STOP);
      controlEnable(ID_FASTFORWARD, TRUE, ID_ACTION_FASTFORWARD);
      controlEnable(ID_FORWARD, FALSE, 0);
      break;
    case STATE_ENDOFLOG: /* End of log */
      controlEnable(ID_REW, TRUE, ID_ACTION_REWIND);
      controlEnable(ID_PLAY, FALSE, ID_ACTION_PLAY);
      controlEnable(ID_PAUSE, FALSE, ID_ACTION_PAUSE);
      controlEnable(ID_STOP, TRUE, ID_ACTION_STOP);
      controlEnable(ID_FASTFORWARD, FALSE, 0);
      controlEnable(ID_FORWARD, FALSE, ID_ACTION_FASTFORWARD);
      break;
    case STATE_STARTOFLOG:
      controlEnable(ID_REW, FALSE, ID_ACTION_REWIND);
      controlEnable(ID_PLAY, TRUE, ID_ACTION_PLAY);
      controlEnable(ID_PAUSE, FALSE, ID_ACTION_PAUSE);
      controlEnable(ID_STOP, TRUE, ID_ACTION_STOP);
      controlEnable(ID_FASTFORWARD, TRUE, ID_ACTION_FASTFORWARD);
      controlEnable(ID_FORWARD, FALSE, 0);
    }
    oldState = state;
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
#define DKS_VK_LEFT 37
#define DKS_VK_RIGHT 39
#define DKS_VK_UP 38
#define DKS_VK_DOWN 40
  clientMutexWaitFor();
  if (keyCode == DKS_VK_UP) {
     screenUpdate(up);
  } else if (keyCode == DKS_VK_DOWN) {
     screenUpdate(down);
  } else if (keyCode == DKS_VK_LEFT) {
     screenUpdate(left);
  } else if (keyCode == DKS_VK_RIGHT) {
     screenUpdate(right);
  }
  clientMutexRelease();
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
  clientMutexRelease();
  if (GetClientRect(hMainWnd, &rcWindow )) {
    /* Convert the coordinates from client relative to screen relative. */
    if (ClientToScreen(hMainWnd, (LPPOINT) &rcWindow )) {
      drawMainScreen(value,mineView,tks, gs, sBullet, lgms, &rcWindow, FALSE, FALSE, srtDelay, isPillView, edgeX, edgeY, FALSE, 0, 0);
    }
  }
}

void CALLBACK windowFrameTimer(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
  clientMutexWaitFor();
  screenUpdate(redraw);
  if (playIsPlaying == TRUE) {
    timerFrameID = timeSetEvent(50, 1000, windowFrameTimer, 0, TIME_ONESHOT);
  }
}

void CALLBACK windowTimer(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
  clientMutexWaitFor();
  screenLogTick();
  if (doubleSpeed == TRUE) {
    screenLogTick();
    screenLogTick();
  }
  clientMutexRelease();
  if (playIsPlaying == TRUE) {
    timerGameID = timeSetEvent(timerSleep, 1000, windowTimer, 0, TIME_ONESHOT);
  }
}

void windowNeedRedraw(HWND hWnd) {
  clientMutexWaitFor();
  if (isLoaded == TRUE) {
    wantScreenUpdate = TRUE;
//    windowFrameTimer(0, 0, 0, 0, 0);
  } else {
    RECT rcWindow;
    if (GetClientRect(hWnd, &rcWindow )) {
      /* Convert the coordinates from client relative to screen relative. */
      drawLogo(appInst, hWnd, &rcWindow);
    }
  }
  clientMutexRelease();
}

#define OPEN_FILE_FILTERS "WinBolo Log Files\0*.WBV\0All Files\0*.*\0\0"
#define OPEN_FILE_TITLE "Open File...\0"
#define DEFAULT_FILE_EXTENSION "wbv\0"

/* cmdLine is NULL if to show dialog otherwise call the aplication */
void windowOpenFile(char *cmdLine) {
  OPENFILENAME ofn;              /* File name to be saved */
  char fileName[FILENAME_MAX];  /* The filename and path that should be opened */
  bool dlgOK;                   /* Did th dialog work ok? */
  char memoryBuff[FILENAME_MAX];


  windowStop(FALSE);
  fileName[0] = '\0';

  if (cmdLine == NULL) {
    /* Fill the open file dialog box with stuff */
    memset( &ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize	= sizeof(OPENFILENAME);
    ofn.hwndOwner	= hMainWnd;
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
    ofn.Flags		= OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = DEFAULT_FILE_EXTENSION;
    
    dlgOK = (bool) GetOpenFileName( &ofn ); /* Do the dialog box */
  } else {
    dlgOK = TRUE;
    strcpy(fileName, cmdLine);
  }
  if (dlgOK == TRUE) {
    GetPrivateProfileString("LOGVIEWER", "Memory Buffer", "4", memoryBuff, FILENAME_MAX, PREFERENCE_FILE);
    if (screenLoadMap(fileName, atoi(memoryBuff)) == FALSE) {
      MessageBox(hMainWnd, "Could not open log file", DIALOG_BOX_TITLE, MB_ICONINFORMATION);
      isLoaded = FALSE;
    } else {
      isLoaded = TRUE;
      EnableMenuItem(GetMenu(hMainWnd), ID_FILE_SAVEMAP, MF_ENABLED);
      controlsEnable(STATE_STARTOFLOG);
      controlEnable(ID_PLAY, TRUE, ID_ACTION_PLAY);
      SendMessage(GetDlgItem(hGameEventsWnd, IDC_LIST), LB_RESETCONTENT, 0, 0);
      windowNeedRedraw(hMainWnd);
    }
  }  
}

#define SAVE_FILE_FILTERS "WinBolo Map Files\0*.MAP\0All Files\0*.*\0\0"
#define SAVE_FILE_TITLE "Save Map File...\0"
#define SAVE_DEFAULT_FILE_EXTENSION "map\0"


bool saveFileOwnership;


BOOL CALLBACK saveDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
  if (uMsg == WM_DESTROY) {
    saveFileOwnership = FALSE;
    if (IsDlgButtonChecked(hDlg, IDC_CHECKOWNERSHIP) == BST_CHECKED) {
      saveFileOwnership = TRUE;
    }
  }
	return FALSE;
}


// Saves the map file
void windowSaveMap() {
  OPENFILENAME ofn;              /* File name to be saved */
  char fileName[FILENAME_MAX];  /* The filename and path that should be opened */
  char currentTime[FILENAME_MAX]; /* Current time in the log */
  bool dlgOK;                   /* Did th dialog work ok? */
  bool saveOK;                  /* Did the file save OK */
  int count = 0;
  int len;

  windowPause();
  fileName[0] = '\0';

  /* Get the name of the map */
  clientMutexWaitFor();
  screenGetMapName(fileName);
  screenGetTime(currentTime);
  clientMutexRelease();

  len = strlen(currentTime);
  while (count < len) {
    if (currentTime[count] == ':') {
      currentTime[count] = '.';
    }
    count++;
  }

  strcat(fileName, " - ");
  strcat(fileName, currentTime);

  /* Fill the open file dialog box with stuff */
  memset( &ofn, 0, sizeof(OPENFILENAME));
  ofn.lStructSize	= sizeof(OPENFILENAME);
  ofn.hwndOwner	= hMainWnd;
  ofn.hInstance	= appInst;
  ofn.lpstrFilter	= SAVE_FILE_FILTERS;
  ofn.lpstrCustomFilter = NULL;
  ofn.nFilterIndex	= 0;
  ofn.nMaxCustFilter = 0;

  ofn.lpstrFile	= fileName;
  ofn.nMaxFile	= sizeof(fileName);
	ofn.lpstrFileTitle	= NULL;
	ofn.nMaxFileTitle	= 0;
 	ofn.lpstrInitialDir = NULL;
  ofn.lpstrTitle	= NULL;
	ofn.lpfnHook = saveDialogProc;
	ofn.lpTemplateName = MAKEINTRESOURCE(IDD_COMDLG32);
  ofn.Flags		= OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER |OFN_ENABLETEMPLATE | OFN_ENABLEHOOK ;
  ofn.lpstrDefExt = SAVE_DEFAULT_FILE_EXTENSION;
  
  dlgOK = (bool) GetSaveFileName( &ofn ); /* Do the dialog box */
  if (dlgOK == TRUE) {
    saveOK = screenSaveMap(fileName, saveFileOwnership);
    if (saveOK == FALSE) {
      MessageBox(hMainWnd, "Error Saving Map File", DIALOG_BOX_TITLE, MB_ICONINFORMATION);
    }
  } else {
    DWORD d;
       d = GetLastError();
  }

}

BOOL CALLBACK dialogAboutCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  switch ( uMsg ) {
  case WM_INITDIALOG:
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case IDOK:
      EndDialog(hWnd,TRUE);
      break;
    default:
      break;
    }
    break;
  case WM_PAINT:
    break;
  case WM_DESTROY:
      EndDialog(hWnd,TRUE);
    break;
  }
  return FALSE;
}

/*********************************************************
*NAME:          dialogMemoryBufferCallback
*AUTHOR:        John Morrison
*PURPOSE:
*  The Memory Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogMemoryBufferCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  char buff[FILENAME_MAX];
  switch ( uMsg ) {
  case WM_INITDIALOG:
    GetPrivateProfileString("LOGVIEWER", "Memory Buffer", "4", buff, FILENAME_MAX, PREFERENCE_FILE);
    if (atoi(buff) < 1) {
      buff[0] = '4';
      buff[1] = '\n';
    }
    SetDlgItemText(hWnd, IDC_MEMORYEDIT, buff);
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case IDOK:
      GetDlgItemText(hWnd, IDC_MEMORYEDIT, buff, FILENAME_MAX);
      if (strlen(buff) == 0 || atoi(buff) < 1) {
        MessageBox(hWnd, "Minimum memory buffer size is 1Mb", "Log Viewer", MB_OK);
      } else if (atoi(buff) > 99) {
        MessageBox(hWnd, "Maximum memory buffer size is 99Mb", "Log Viewer", MB_OK);
      } else {
        WritePrivateProfileString("LOGVIEWER", "Memory Buffer", buff, PREFERENCE_FILE);
        EndDialog(hWnd,TRUE);
      }
      break;
    case IDCANCEL:
      EndDialog(hWnd,TRUE);
      break;
    default:
      break;
    }
    break;
  case WM_PAINT:
    break;
  case WM_DESTROY:
    EndDialog(hWnd,TRUE);
    break;
  }
  return FALSE;
}


void setWindowVisible(HWND hWnd, HMENU hMenu, UINT menuItem) {
  if (GetMenuState(hMenu, menuItem, MF_BYCOMMAND) == MF_CHECKED) {
    CheckMenuItem(hMenu, menuItem, MF_UNCHECKED);
    ShowWindow(hWnd, SW_HIDE);
    SetFocus(hMainWnd);
  } else {
    CheckMenuItem(hMenu, menuItem, MF_CHECKED);
    ShowWindow(hWnd, SW_SHOW);
    SetFocus(hWnd);
  }
}


#define STRERR_HELPFILE "Unable to open WinBolo Log Viewer help file"

/*********************************************************
*NAME:          winUtilWBSubDirExist
*AUTHOR:        John Morrison
*CREATION DATE: 29/4/00
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Checks to see if a subdirectory exists off the WinBolo
*  Directory. Returns existance. Also modifies subDirName
*  to complete path of directory
*
*ARGUMENTS:
*  subDirName - The directory name to search for. Upon
*               successful find it also modifies this to
*               the complete path of the directory
*********************************************************/
bool winUtilWBSubDirExist(char *subDirName) {
  bool returnValue;            /* Value to return */
  char fileName[FILENAME_MAX]; /* Filename used in lookup */
  WIN32_FIND_DATA fd;          /* Find data */
  HANDLE findHandle;           /* The find handle  */
  unsigned int count;          /* Looping variable */

  returnValue = TRUE;
  /* Get the current directory */
  GetModuleFileName((HMODULE) windowGetInstance(), fileName, FILENAME_MAX);
  count = strlen(fileName);
  while (fileName[count] != '\\') {
    count--;
  }

  if (count < FILENAME_MAX-1) {
    fileName[count+1] = EMPTY_CHAR;
  }
  strcat(fileName, subDirName);

  /* Look for the directory */
  findHandle = FindFirstFile(fileName, &fd);
  if (findHandle != INVALID_HANDLE_VALUE) {
    if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
      returnValue = FALSE;
    }
  } else {
    returnValue = FALSE;
  } 
  FindClose(findHandle);
  
  /* Copy the path into the parameter */
  if (returnValue == TRUE) {
    strcpy(subDirName, fileName);
  }
  return returnValue;
}


void winUtilOpenHelpFile(HINSTANCE appInst, HWND hWnd) {
  char path[MAX_PATH]; /* The path to open */
  char file[MAX_PATH]; /* The file to open */

  strcpy(path, "Manual");
  if (winUtilWBSubDirExist(path) == TRUE) {
    strcpy(file, "file://");
    strcat(file, path);
    strcat(file, "/Log Viewer Manual.pdf");
    if ((int) ShellExecute(NULL, "open", file, NULL, NULL, SW_SHOW) <= 32) {
      MessageBox(hWnd, STRERR_HELPFILE, DIALOG_BOX_TITLE, MB_ICONINFORMATION); 
    }
  } else {
    /* Error */
    MessageBox(hWnd, STRERR_HELPFILE, DIALOG_BOX_TITLE, MB_ICONINFORMATION); 
  }
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
  int xPos;
  int yPos;
  HMENU hMenu;
  BYTE sizeX;
  BYTE sizeY;

  switch (msg) {
  case WM_ACTIVATE:
    clientMutexWaitFor();
    if (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) {
      if (useFullscreen == FALSE) {
        drawRestore();
        windowNeedRedraw(hWnd);
      }
    }
    clientMutexRelease();
    break;
  case WM_KEYDOWN:
    /* Keys Pressed - Test for arrow Keys*/
     windowKeyPressed((int) wParam);
     windowNeedRedraw(hWnd);
     break;
  case WM_PAINT:
    if (useFullscreen == FALSE) {
      windowNeedRedraw(hWnd);
    }
    break;
  case WM_DESTROY:
    /* Quit time */
    PostQuitMessage(0);
    break;
  case WM_CLOSE:
    mainSavePrefernces();
    break;
  case WM_LBUTTONDOWN:
    xPos = LOWORD(lParam);  // horizontal position of cursor 
    yPos = HIWORD(lParam);  // vertical position of cursor 

    clientMutexWaitFor();
    if (GetMenuState(GetMenu(hWnd), ID_OPTIONS_MODE_SELECTTEAM, MF_BYCOMMAND) == MF_CHECKED) {
      screenMouseClick(xPos, yPos);
    } else {
      screenMouseInformationClick(xPos, yPos);
    }
    if (playIsPlaying == FALSE && isLoaded == TRUE) {
      windowNeedRedraw(hWnd);
    }

    clientMutexRelease();
    break;
  case WM_RBUTTONDOWN:
    xPos = LOWORD(lParam);  // horizontal position of cursor 
    yPos = HIWORD(lParam);  // vertical position of cursor 

    clientMutexWaitFor();
    screenMouseCentreClick(xPos, yPos);
    clientMutexRelease();
    break;
  case WM_DISPLAYCHANGE:
    if (useFullscreen == FALSE) {
      clientMutexWaitFor();
      drawCleanup();
      drawSetup(appInst, hWnd);
      clientMutexRelease();
    }
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case ID_FILE_EXIT:
      mainSavePrefernces();
      PostQuitMessage(0);
      break;
    case ID_FILE_OPEN1:
      windowOpenFile(NULL);
      break;
    case ID_FILE_SAVEMAP:
      windowSaveMap();
      break;
    case ID_ACTION_PLAY:
      windowPlay();
      break;
    case ID_ACTION_PAUSE:
      windowPause();
      break;
    case ID_ACTION_REWIND:
      windowRewind();
      break;
    case ID_ACTION_FASTFORWARD:
      windowFastForward();
      break;
    case ID_ACTION_STOP:
      windowStop(FALSE);
      break;
    case ID_ACTION_FULLSCREEN:
      sizeX = screenGetSizeX();
      sizeY = screenGetSizeY();
      clientMutexWaitFor();
      drawCleanup();
      if (useFullscreen == TRUE) {
	      useFullscreen = FALSE;
	      CheckMenuItem(GetMenu(hWnd), ID_ACTION_FULLSCREEN, MF_UNCHECKED);
      } else {
	      useFullscreen = TRUE;
	      CheckMenuItem(GetMenu(hWnd), ID_ACTION_FULLSCREEN, MF_CHECKED);
      }
      if (drawSetup(appInst, hMainWnd) == FALSE) {
        drawCleanup();
        useFullscreen = FALSE;
        drawSetup(appInst, hMainWnd);
        MessageBox(hMainWnd, "Error Starting Fullscreen mode", "Log Viewer", MB_OK);
      }
      if (useFullscreen == TRUE) {
        screenSetSizeX(800/16);
        screenSetSizeY(600/16);
        windowPlay();
      } else {
        screenSetSizeX((BYTE) sizeX);
        screenSetSizeY((BYTE) sizeY);
      }
      clientMutexRelease();
      break;
    case ID_OPTIONS_WINDOWSIZE:
      clientMutexWaitFor();
      DialogBox(appInst, MAKEINTRESOURCE(IDD_WINDOWSIZE), hWnd, dialogWindowSizeCallback);
      clientMutexRelease();
      break;
    case ID_OPTIONS_TEAMCOLOURS:
      clientMutexWaitFor();
      DialogBox(appInst, MAKEINTRESOURCE(IDD_TEAMCOLOURS), hWnd, dialogWindowTeamColoursCallback);
      clientMutexRelease();
      break;
    case ID_OPTIONS_MEMORYBUFFER:
      clientMutexWaitFor();
      DialogBox(appInst, MAKEINTRESOURCE(IDD_MEMORY), hWnd, dialogMemoryBufferCallback);
      clientMutexRelease();
      break;
    case ID_OPTIONS_MODE_INFORMATION:
      hMenu = GetMenu(hWnd);
      CheckMenuItem(hMenu, ID_OPTIONS_MODE_SELECTTEAM, MF_UNCHECKED);
      CheckMenuItem(hMenu, ID_OPTIONS_MODE_INFORMATION, MF_CHECKED);
      break;
    case ID_OPTIONS_MODE_SELECTTEAM:
      hMenu = GetMenu(hWnd);
      CheckMenuItem(hMenu, ID_OPTIONS_MODE_SELECTTEAM, MF_CHECKED);
      CheckMenuItem(hMenu, ID_OPTIONS_MODE_INFORMATION, MF_UNCHECKED);
      break;
    case ID_OPTIONS_SOUNDEFFECTS:
      hMenu = GetMenu(hWnd);
      if (GetMenuState(hMenu, ID_OPTIONS_SOUNDEFFECTS, MF_BYCOMMAND) == MF_CHECKED) {
        CheckMenuItem(hMenu, ID_OPTIONS_SOUNDEFFECTS, MF_UNCHECKED);
        isSoundsPlaying = FALSE;
      } else {
        CheckMenuItem(hMenu, ID_OPTIONS_SOUNDEFFECTS, MF_CHECKED);
        isSoundsPlaying = TRUE;
      }
      break;
    case ID_OPTIONS_USETEAMCOLOURS:
      hMenu = GetMenu(hWnd);
      clientMutexWaitFor();
      if (GetMenuState(hMenu, ID_OPTIONS_USETEAMCOLOURS, MF_BYCOMMAND) == MF_CHECKED) {
        CheckMenuItem(hMenu, ID_OPTIONS_USETEAMCOLOURS, MF_UNCHECKED);
        useTeamColours = FALSE;
      } else {
        CheckMenuItem(hMenu, ID_OPTIONS_USETEAMCOLOURS, MF_CHECKED);
        useTeamColours = TRUE;
      }
      clientMutexRelease();
      break;

    case ID_OPTIONS_TANK_CENTRED:
      hMenu = GetMenu(hWnd);
      clientMutexWaitFor();
      if (GetMenuState(hMenu, ID_OPTIONS_TANK_CENTRED, MF_BYCOMMAND) == MF_CHECKED) {
        CheckMenuItem(hMenu, ID_OPTIONS_TANK_CENTRED, MF_UNCHECKED);
        screenTankCentred(FALSE);
      } else {
        CheckMenuItem(hMenu, ID_OPTIONS_TANK_CENTRED, MF_CHECKED);
        screenTankCentred(TRUE);
      }
      clientMutexRelease();
      break;
    case ID_OPTIONS_MODE_FULLSCREEN:
//      DialogBox(appInst, IDD_DIALOG_FULLSCREEN, hWnd, dialogFullscreenSetupCallback);
      break;
    case ID_OPTIONS_DNSLOOKUPS:
      hMenu = GetMenu(hWnd);
      if (GetMenuState(hMenu, ID_OPTIONS_DNSLOOKUPS, MF_BYCOMMAND) == MF_CHECKED) {
        CheckMenuItem(hMenu, ID_OPTIONS_DNSLOOKUPS, MF_UNCHECKED);
        dnsSetEnabled(FALSE);
      } else {
        if (dnsSetEnabled(TRUE) == TRUE) {
          CheckMenuItem(hMenu, ID_OPTIONS_DNSLOOKUPS, MF_CHECKED);
        } else {
          MessageBox(hWnd, "Error", "here", MB_OK);
        }

      }
      break;
    case ID_WINDOWS_CONTROLS:
      setWindowVisible(hControlsWnd, GetMenu(hWnd), ID_WINDOWS_CONTROLS);
      break;
    case ID_WINDOWS_GAMEINFORMATION:
      setWindowVisible(hGameInfoWnd, GetMenu(hWnd), ID_WINDOWS_GAMEINFORMATION);
      break;
    case ID_WINDOWS_ITEMINFORMATION:
      setWindowVisible(hItemInfoWnd, GetMenu(hWnd), ID_WINDOWS_ITEMINFORMATION);
      break;
    case ID_WINDOWS_EVENTS:
      setWindowVisible(hGameEventsWnd, GetMenu(hWnd), ID_WINDOWS_EVENTS);
      break;
	case ID_HELP_HELP:
		winUtilOpenHelpFile(appInst, hWnd);
		break;
    case ID_HELP_ABOUT:
      DialogBox(appInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, dialogAboutCallback);
    }

    break;
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}


void windowPlay() {
  if (playIsPlaying == FALSE) {
    timerGameID = timeSetEvent(20, 1000, windowTimer, 0, TIME_ONESHOT);
    timerFrameID = timeSetEvent(50, 1000, windowFrameTimer, 0, TIME_ONESHOT);
  }
  playIsPlaying = TRUE;
  controlsEnable(STATE_PLAYING);
}


void windowStop(bool corruptLog) {
  HWND hCtlWnd;

  if (corruptLog == TRUE) {
    MessageBox(hMainWnd, "Error: Corrupt Log File", "Log Viewer", MB_OK);
  }


  EnableMenuItem(GetMenu(hMainWnd), ID_FILE_SAVEMAP, MF_GRAYED);
  windowPause();
  Sleep(500);
  if (corruptLog == FALSE) {
    clientMutexWaitFor();
  }
  frontEndSetGameInformation(TRUE, 0, 0, 0, NULL, 0, 0, 0, 0, 0, NULL, 0);
  updateItem(0, 0, 0,0,0,0,0,0, FALSE);
  screenCloseLog();
  isLoaded = FALSE;
  controlsEnable(STATE_STOPPED);
  if (corruptLog == FALSE) {
    clientMutexRelease();
  }
  windowNeedRedraw(hMainWnd);
  hCtlWnd = GetDlgItem(hGameEventsWnd, IDC_LIST);
  SendMessage(hCtlWnd, LB_RESETCONTENT, 0, 0);
}

void windowPause() {
  clientMutexWaitFor();
  if (playIsPlaying == TRUE) {
    timeKillEvent(timerGameID);
    timeKillEvent(timerFrameID);
  }
  playIsPlaying = FALSE;
  clientMutexRelease();
  controlsEnable(STATE_PAUSED);
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
  WNDCLASS wc;      /* Window Class to create */
  char line[256];
  int sizeX;
  int sizeY;

  /* Set up the Window Class */
  wc.hCursor        = LoadCursor(0,IDC_ARROW);//NULL;
  wc.hIcon          = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
  wc.lpszMenuName   = NULL;
  wc.lpszClassName  = "LOGVIEW";
  wc.hbrBackground  = 0;
  wc.hInstance      = hInst;
  wc.style          = 0;
  wc.lpfnWndProc    = ExWindowProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = 0;
  wc.hbrBackground  = 0;

  // Get the screen size from the log file
  GetPrivateProfileString("LOGVIEWER", "ScreenSizeX", "50", line, 256, PREFERENCE_FILE);
  sizeX = atoi(line);
  if (sizeX < 5 || sizeX > 99) {
    sizeX = 50;
  }
  GetPrivateProfileString("LOGVIEWER", "ScreenSizeY", "38", line, 256, PREFERENCE_FILE);
  sizeY = atoi(line);
  if (sizeY < 5 || sizeY > 99) {
    sizeY = 38;
  }

  screenSetSizeX((BYTE) sizeX);
  screenSetSizeY((BYTE) sizeY);



  if (RegisterClass(&wc)) {
    returnValue = CreateWindow(
     "LOGVIEW",                 /* Class name */
     "Log Viewer",                     /* Caption */ 
    (WS_OVERLAPPED + WS_CAPTION + WS_SYSMENU + WS_MINIMIZEBOX), /* Style */
     CW_USEDEFAULT, CW_USEDEFAULT,   /* Position */
     sizeX* 16 + 4, sizeY * 16 + 42,   /* Size */
     NULL,							             /* No parent */
     LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1)),          /* Menu */
     hInst,                          /* inst handle */
     0                               /* no params */
     );
  } else {
    returnValue = NULL;
  }

  return returnValue;
}

#define YESNO_TO_TRUEFALSE(X) (tolower(X[0]) == 'y' ? 1 : 0)
#define TRUEFALSE_TO_STR(X) (X == TRUE ? "Yes" : "No")


void windowDisableSound() {
  CheckMenuItem(GetMenu(hMainWnd), ID_OPTIONS_SOUNDEFFECTS, MF_UNCHECKED);
  EnableMenuItem(GetMenu(hMainWnd), ID_OPTIONS_SOUNDEFFECTS, MF_GRAYED);
  isSoundsPlaying = FALSE;
}

void getDef(char *dest, int index) {
  int val = 0;
  switch (index) {
    case 0:
      val = 11;
      break;
    case 1:
      val = 12;
      break;
    case 2:
      val = 2;
      break;
    case 3:
      val = 4;
      break;
    case 4:
      val = 16;
      break;
    case 5:
      val = 13;
      break;
    case 6:
      val = 8;
      break;
    case 7:
      val = 3;
      break;
    case 8:
      val = 6;
      break;
    case 9:
      val = 1;
      break;
    case 10:
      val = 5;
      break;
    case 11:
      val = 7;
      break;
    case 12:
      val = 9;
      break;
    case 13:
      val = 14;
      break;
    case 14:
      val = 15;
      break;
    case 15:
      val = 0;
      break;
    default:
      break;
  }
  sprintf(dest, "%d", val);
}


void mainLoadPreferences() {
  char line[FILENAME_MAX];
  char def[FILENAME_MAX];
  char val[FILENAME_MAX];

  HMENU hMenu = GetMenu(hMainWnd);
  bool dns;
  BYTE count;

  // Tank Centred
  GetPrivateProfileString("LOGVIEWER", "Tank Centred", "No", line, FILENAME_MAX, PREFERENCE_FILE);
  if (YESNO_TO_TRUEFALSE(line) == FALSE) {
    CheckMenuItem(hMenu, ID_OPTIONS_TANK_CENTRED, MF_UNCHECKED);
    screenTankCentred(FALSE);
  } else {
    CheckMenuItem(hMenu, ID_OPTIONS_TANK_CENTRED, MF_CHECKED);
    screenTankCentred(TRUE);
  }

  // Sound Effects
  if (GetMenuState(hMenu, ID_OPTIONS_SOUNDEFFECTS, MF_BYCOMMAND) != MF_GRAYED) {
    GetPrivateProfileString("LOGVIEWER", "Sounds", "Yes", line, FILENAME_MAX, PREFERENCE_FILE);
    isSoundsPlaying = YESNO_TO_TRUEFALSE(line); 
    if (isSoundsPlaying == TRUE) {
      CheckMenuItem(hMenu, ID_OPTIONS_SOUNDEFFECTS, MF_CHECKED);
    } else {
      CheckMenuItem(hMenu, ID_OPTIONS_SOUNDEFFECTS, MF_UNCHECKED);
    }
  }

  // DNS Lookups
  GetPrivateProfileString("LOGVIEWER", "DNS Lookups", "No", line, FILENAME_MAX, PREFERENCE_FILE);  
  dns = YESNO_TO_TRUEFALSE(line); 
  if (dns == TRUE) {
    dns = dnsSetEnabled(TRUE);
  }
  if (dns == TRUE) {
    CheckMenuItem(hMenu, ID_OPTIONS_DNSLOOKUPS, MF_CHECKED);
  }

  // Mode
  GetPrivateProfileString("LOGVIEWER", "Mode", "Information", line, FILENAME_MAX, PREFERENCE_FILE);
  if (line[0] == '\0' || tolower(line[0]) == 'i') {
    CheckMenuItem(hMenu, ID_OPTIONS_MODE_INFORMATION, MF_CHECKED);
  } else {
    CheckMenuItem(hMenu, ID_OPTIONS_MODE_SELECTTEAM, MF_CHECKED);
  }

  // Use Team Colours
  GetPrivateProfileString("LOGVIEWER","Use Team Colours", "Yes", line, FILENAME_MAX, PREFERENCE_FILE);
  if (line[0] == '\0' || tolower(line[0]) == 'y') {
    useTeamColours = TRUE;
    CheckMenuItem(hMenu, ID_OPTIONS_USETEAMCOLOURS, MF_CHECKED);
  } else {
    useTeamColours = FALSE;
    CheckMenuItem(hMenu, ID_OPTIONS_USETEAMCOLOURS, MF_UNCHECKED);
  }

  // Team colours
  count = 0;
  while (count < MAX_TANKS) {
    sprintf(line, "Team Colour %d", count+1);
    getDef(def , count);
    GetPrivateProfileString("LOGVIEWER", line, def, val, FILENAME_MAX, PREFERENCE_FILE);
    tc[count] = atoi(val);
    if (tc[count] > 16) {
      tc[count] = count;
    }
    count++;
  }
  GetPrivateProfileString("LOGVIEWER", "Neutral Colour", "10", val, FILENAME_MAX, PREFERENCE_FILE);
  tc[16] = atoi(val);
  if (tc[count] > 16) {
    tc[16] = 16;
  }
  GetPrivateProfileString("LOGVIEWER", "Playback Speed", "1", val, FILENAME_MAX, PREFERENCE_FILE);

  speed = atoi(val);
  if (speed > 6) {
    speed = 1;
  }
  updateSpeed(speed, TRUE);
}

void mainSaveWindowPreference(HMENU hMenu, UINT menuId, HWND hWnd, char *name, bool saveVisible) {
  char line[256];
  char val[64];
  bool set = TRUE;
  RECT rcWindow;


  if (GetWindowRect(hWnd, &rcWindow)) {
    if (saveVisible == TRUE) {
      // Visible
      sprintf(line, "%s.Visible", name);
      if (GetMenuState(hMenu, menuId, MF_BYCOMMAND) == MF_UNCHECKED) {
        set = FALSE;
      }
      WritePrivateProfileString("LOGVIEWER", line, TRUEFALSE_TO_STR(set), PREFERENCE_FILE);
    }
    // X & Y
    sprintf(line, "%s.X", name);
    sprintf(val, "%d", rcWindow.left);
    WritePrivateProfileString("LOGVIEWER", line, val, PREFERENCE_FILE);
    sprintf(line, "%s.Y", name);
    sprintf(val, "%d", rcWindow.top);
    WritePrivateProfileString("LOGVIEWER", line, val, PREFERENCE_FILE);
  } else {
    DWORD d;
    d = GetLastError();
  }
}


void mainSavePrefernces() {
  HMENU hMenu = GetMenu(hMainWnd);
  char line[256];
  char val[64];
  BYTE count; // Looping variable


  // Tank Centred
  if (GetMenuState(hMenu, ID_OPTIONS_TANK_CENTRED, MF_BYCOMMAND) == MF_CHECKED) {
    WritePrivateProfileString("LOGVIEWER", "Tank Centred", "Yes", PREFERENCE_FILE);
  } else {
    WritePrivateProfileString("LOGVIEWER", "Tank Centred", "No", PREFERENCE_FILE);
  }

  // Sound Effects
  WritePrivateProfileString("LOGVIEWER", "Sounds", TRUEFALSE_TO_STR(isSoundsPlaying), PREFERENCE_FILE);
  // DNS Lookups
  if (GetMenuState(hMenu, ID_OPTIONS_DNSLOOKUPS, MF_BYCOMMAND) == MF_CHECKED) {
    WritePrivateProfileString("LOGVIEWER", "DNS Lookups", "Yes", PREFERENCE_FILE);
  } else {
    WritePrivateProfileString("LOGVIEWER", "DNS Lookups", "No", PREFERENCE_FILE);
  }

  // Mode
  if (GetMenuState(hMenu, ID_OPTIONS_MODE_SELECTTEAM, MF_BYCOMMAND) == MF_CHECKED) {
    WritePrivateProfileString("LOGVIEWER", "Mode", "Select Teams", PREFERENCE_FILE);
  } else {
    WritePrivateProfileString("LOGVIEWER", "Mode", "Information", PREFERENCE_FILE);
  }

  // Team Colours
  if (useTeamColours == TRUE) {
    WritePrivateProfileString("LOGVIEWER", "Use Team Colours", "Yes", PREFERENCE_FILE);
  } else {
    WritePrivateProfileString("LOGVIEWER", "Use Team Colours", "No", PREFERENCE_FILE);
  }

  // Screen Size X & Y
  sprintf(line, "%d", screenGetSizeX());
  WritePrivateProfileString("LOGVIEWER", "ScreenSizeX", line, PREFERENCE_FILE);
  sprintf(line, "%d", screenGetSizeY());
  WritePrivateProfileString("LOGVIEWER", "ScreenSizeY", line, PREFERENCE_FILE);

  // window Preferences
  mainSaveWindowPreference(hMenu, ID_WINDOWS_CONTROLS, hControlsWnd, "Window.Controls", TRUE);
  mainSaveWindowPreference(hMenu, ID_WINDOWS_EVENTS, hGameEventsWnd, "Window.Events", TRUE);
  mainSaveWindowPreference(hMenu, ID_WINDOWS_GAMEINFORMATION, hGameInfoWnd, "Window.GameInformation", TRUE);
  mainSaveWindowPreference(hMenu, ID_WINDOWS_ITEMINFORMATION, hItemInfoWnd, "Window.ItemInformation", TRUE);
  mainSaveWindowPreference(hMenu, 0, hMainWnd, "Window.LogViewer", FALSE);

  // Write team colours
  count = 0;
  while (count < MAX_TANKS) {
    sprintf(line, "Team Colour %d", count+1);
    sprintf(val, "%d", tc[count]);
    WritePrivateProfileString("LOGVIEWER", line, val, PREFERENCE_FILE);
    count++;
  }
  sprintf(val, "%d", tc[16]);
  WritePrivateProfileString("LOGVIEWER", "Neutral Colour", val, PREFERENCE_FILE);


  // Write the speed
  sprintf(val, "%d", speed);
  WritePrivateProfileString("LOGVIEWER", "Playback Speed", val, PREFERENCE_FILE);

  // Write that we have written preferences
  WritePrivateProfileString("LOGVIEWER", "PreferencesSaved", "Yes", PREFERENCE_FILE);
}

void placeWindow(HMENU hMenu, HWND hWnd, UINT menuId, char *name) {
  char line[256];
  char val[256];
  RECT size;
  int cx;
  int cy;
  int prefX;
  int prefY;


  // Position
  cx = GetSystemMetrics(SM_CXSCREEN);
  cy = GetSystemMetrics(SM_CYSCREEN);


  sprintf(line, "%s.X", name);
  GetPrivateProfileString("LOGVIEWER", line, "10", val, 256, PREFERENCE_FILE);
  prefX = atoi(val);
  if (prefX < 0 || prefX > cx) {
    prefX = 10;
  }
  sprintf(line, "%s.Y", name);
  GetPrivateProfileString("LOGVIEWER", line, "10", val, 256, PREFERENCE_FILE);
  prefY = atoi(val);
  if (prefY < 0 || prefY > cx) {
    prefY = 10;
  }


  GetWindowRect(hWnd, &size);
  SetWindowPos(hWnd, NULL, prefX, prefY, size.right - size.left, size.bottom - size.top, 0);


  // Visible
  sprintf(line, "%s.Visible", name);
  GetPrivateProfileString("LogViewer", line, "Yes", val, 256, PREFERENCE_FILE);
  if (YESNO_TO_TRUEFALSE(val) == TRUE) {
    ShowWindow(hWnd, SW_SHOW);
    CheckMenuItem(hMenu, menuId, MF_CHECKED);
  } else {
    ShowWindow(hWnd, SW_HIDE);
    CheckMenuItem(hMenu, menuId, MF_UNCHECKED);
  }


}

void placeWindows(int nCmdShow) {
  char line[256];
  RECT pos;
  RECT size;
  int x;
  int cx;
  int cy;
  int prefX;
  int prefY;
  HMENU hMenu = GetMenu(hMainWnd);

  cx = GetSystemMetrics(SM_CXSCREEN);
  cy = GetSystemMetrics(SM_CYSCREEN);


  // Main Window
  GetPrivateProfileString("LOGVIEWER", "Window.LogViewer.X", "10", line, 256, PREFERENCE_FILE);
  prefX = atoi(line);
  if (prefX < 0 || prefX > cx) {
    prefX = 10;
  }
  GetPrivateProfileString("LOGVIEWER", "Window.LogViewer.Y", "10", line, 256, PREFERENCE_FILE);
  prefY = atoi(line);
  if (prefY < 0 || prefY > cx) {
    prefY = 10;
  }
  GetWindowRect(hMainWnd, &size);
  SetWindowPos(hMainWnd, NULL, prefX, prefY, size.right - size.left, size.bottom - size.top, 0);
  ShowWindow(hMainWnd, nCmdShow);


  GetPrivateProfileString("LOGVIEWER", "PreferencesSaved", "No", line, 256, PREFERENCE_FILE);
  if (YESNO_TO_TRUEFALSE(line) == FALSE) {

    GetWindowRect(hMainWnd, &pos);
    x = pos.right;
    GetWindowRect(hControlsWnd, &size);
    SetWindowPos(hControlsWnd, NULL, pos.right, pos.top, size.right - size.left, size.bottom - size.top, 0);
    ShowWindow(hControlsWnd, SW_SHOW);
    GetWindowRect(hControlsWnd, &pos);
    GetWindowRect(hGameEventsWnd, &size);
    SetWindowPos(hGameEventsWnd, NULL, x, pos.bottom, size.right - size.left, size.bottom - size.top, 0);
    GetWindowRect(hControlsWnd, &pos);
    GetWindowRect(hGameInfoWnd, &size);
    SetWindowPos(hGameInfoWnd, NULL, x, pos.bottom, size.right - size.left, size.bottom - size.top, 0);
    ShowWindow(hGameInfoWnd, SW_SHOW);  
    GetWindowRect(hGameInfoWnd, &pos);
    GetWindowRect(hItemInfoWnd, &size);
    SetWindowPos(hItemInfoWnd, NULL, x, pos.bottom, size.right - size.left, size.bottom - size.top, 0);
    ShowWindow(hItemInfoWnd, SW_SHOW);
    GetWindowRect(hItemInfoWnd, &pos);
    GetWindowRect(hGameEventsWnd, &size);
    SetWindowPos(hGameEventsWnd, NULL, x, pos.bottom, size.right - size.left, size.bottom - size.top, 0);
    ShowWindow(hGameEventsWnd, SW_SHOW);
  } else {
    // Controls Window
    placeWindow(hMenu, hControlsWnd, ID_WINDOWS_CONTROLS, "Window.Controls");
    placeWindow(hMenu, hGameEventsWnd, ID_WINDOWS_EVENTS, "Window.Events");
    placeWindow(hMenu, hGameInfoWnd, ID_WINDOWS_GAMEINFORMATION, "Window.GameInformation");
    placeWindow(hMenu, hItemInfoWnd, ID_WINDOWS_ITEMINFORMATION, "Window.ItemInformation");
  }
}


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
  MSG msg;        /* Windows Messages */
  HACCEL hAccel; /* Accelerator table */
  bool done;      /* Program finished? */
  HWND dlgElemWnd;

  InitCommonControls(); 
  useFullscreen = FALSE;
  isQuitting = FALSE;
  isLoaded = FALSE;
  done = FALSE; 
  msg.wParam = 0; /* In case something goes horribly wrong */
  appInst = hInst;
  hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));

  if (hAccel == NULL) {
    MessageBox(NULL, "Could not load accelerators", "Log Viewer", MB_OK);
    exit(0);
  }

  hPopupMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_COPYMENU));
  if (hPopupMenu == NULL) {
    MessageBox(NULL, "Couldn't Load menu", "Log Viewer", MB_OK);
    exit(0);
  }

  if (clientMutexCreate() == FALSE) {
    MessageBox(NULL, "I couldn't make a mutex", "Log Viewer", MB_OK);
    DestroyMenu(hPopupMenu);
    exit(0);
  }

  hMainWnd = windowCreate(hInst, nCmdShow);
  hGameEventsWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_GAMEEVENTS), hMainWnd, dialogEventsCallback);
  hControlsWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_CONTROLS), hMainWnd, dialogControlsCallback);
  hGameInfoWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_GAMEINFO), hMainWnd, dialogGameInfoCallback);
  hItemInfoWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ITEMINFO), hMainWnd, dialogItemInfoCallback);
  

  if (hMainWnd == NULL || hGameEventsWnd == NULL || hControlsWnd == NULL || hItemInfoWnd == NULL) {
    MessageBox(NULL, "Error creating windows", "Log Viewer", MB_OK);
    exit(0);
  }

  /* Set the edit control callback for pasting */
  dlgElemWnd = GetDlgItem(hGameEventsWnd, IDC_LIST);
  oldWndProc = (FARPROC) SetWindowLong(dlgElemWnd, GWL_WNDPROC, (DWORD)copyClipboardCallback);
  dlgElemWnd = GetDlgItem(hGameInfoWnd, IDC_WBN);
  oldURLLabelProc = (FARPROC) SetWindowLong(dlgElemWnd, GWL_WNDPROC, (DWORD)urlClickableCallback);
  dlgElemWnd = GetDlgItem(hControlsWnd, ID_FORWARD);
  oldButtonProc = (FARPROC) SetWindowLong(dlgElemWnd, GWL_WNDPROC, (DWORD)forwardButtonCallback);
  if (oldWndProc == 0 || oldURLLabelProc == 0 || oldButtonProc == 0) {
    MessageBox(hMainWnd, "Subclass bad", "bad!", MB_ICONEXCLAMATION);
  }




  if (fontSetup(appInst, hMainWnd) == FALSE) {
    DestroyWindow(hMainWnd);
    clientMutexDestroy();
    exit(0);
  }

  if (drawSetup(appInst, hMainWnd) == FALSE) {
    DestroyWindow(hMainWnd);
    clientMutexDestroy();
    fontCleanup();
    exit(0);
  }

  isSoundsPlaying = TRUE;
  if (soundSetup(appInst, hMainWnd) == FALSE) {
    DestroyWindow(hMainWnd);
    drawCleanup();
    clientMutexDestroy();
    fontCleanup();
    exit(0);
  }


  /* Load Preferences */
  mainLoadPreferences();
  
  placeWindows(nCmdShow);
  SetFocus(hMainWnd);

  /* See if we have been passed a command line argument */
  if (strlen(szCmdLine) > 0) {
    windowOpenFile(szCmdLine);
  }

  

  /* Start up messaging */
  while (done == FALSE) {
    if (GetMessage(&msg, NULL, 0, 0) == 0) {
    /* If it's a quit message, we're outta here */
       done = TRUE;
    } else if (!TranslateAccelerator (hMainWnd, hAccel, &msg)) {
       TranslateMessage (&msg);
       DispatchMessage (&msg);
    }
  }
  windowStop(FALSE);
  isQuitting = TRUE;

  soundCleanup();
  drawCleanup();
  DestroyWindow(hGameEventsWnd);
  DestroyWindow(hControlsWnd);
  DestroyWindow(hGameInfoWnd);
  DestroyWindow(hItemInfoWnd);
  DestroyWindow(hMainWnd);
  DestroyMenu(hPopupMenu);
  DestroyAcceleratorTable(hAccel);
  clientMutexDestroy();
  fontCleanup();
  if (hFont != NULL) {
    DeleteObject(hFont);
  }
  dnsShutdown();
  return 0;
}

void windowResize() {
  clientMutexWaitFor();
  SetWindowPos(hMainWnd, NULL, 0, 0, screenGetSizeX()* 16 + 4, screenGetSizeY()* 16 + 42, SWP_NOMOVE);
  drawCleanup();
  drawSetup(appInst, hMainWnd );
  clientMutexRelease();
}

void finished() {
  playIsPlaying = FALSE;
  controlsEnable(STATE_ENDOFLOG);
}


void startOfLog() {
  controlEnable(ID_REW, FALSE, ID_ACTION_REWIND);
}

long FAR PASCAL windowFullScreenProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
    BOOL bActive;

    switch ( message )
    {
        case WM_ACTIVATEAPP:
            bActive = wParam;
            break;

        case WM_CREATE:
            break;

        case WM_SETCURSOR:
            SetCursor( NULL );    // Turn off the mouse cursor
            return TRUE;
 
        case WM_KEYDOWN:
            switch ( wParam )
            {
                case VK_ESCAPE:
                case VK_F12:
                  clientMutexWaitFor();
                    drawCleanup();
                    useFullscreen = FALSE;
                    drawSetup(appInst, hMainWnd);
                    clientMutexRelease();
                    //PostMessage( hWnd, WM_CLOSE, 0, 0 );
                    break;
            }
            break;

        case WM_DESTROY:
            break;
        }

    return DefWindowProc( hWnd, message, wParam, lParam );

} /* WindowProc */
