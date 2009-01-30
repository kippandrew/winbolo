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
*Name:          Dialog - Game Info
*Filename:      dialoggameinfo.c
*Author:        John Morrison
*Creation Date: 26/1/99
*Last Modified: 28/4/00
*Purpose:
*  Looks after the game info dialog box.
*********************************************************/

#include <windows.h>
#include "..\..\bolo\global.h"
#include "..\..\bolo\backend.h"
#include "..\winbolo.h"
#include "..\lang.h"
#include "..\resource.h"
#include "..\dialoggameinfo.h"
#include "..\..\bolo\backend.h"

unsigned int timerGameInfo = 3; /* The timer for getting the game time remaining etc.*/
BYTE dlgGameInfoNumPlayers;      /* Number of players in the game */
long dlgGameInfoTimeRemain;     /* Time remaining in the game */

/*********************************************************
*NAME:          dialogGameInfoCallback
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
*  The Game Info call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogGameInfoCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  char mapName[FILENAME_MAX];

  mapName[0] = '\0';
  
  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogGameInfoSetup(hWnd);
    return TRUE;
  case WM_TIMER:
    dialogGameInfoUpdate(hWnd);
    return TRUE;
  case WM_PAINT:
    break;
  case WM_CLOSE:
    /* Fall through to the WM_DESTROY message */
  case WM_DESTROY:
    KillTimer(hWnd, timerGameInfo);
    windowShowGameInfo(wsrClose);
    EndDialog(hWnd, TRUE);
    break;
  }
  return FALSE;
}

/*********************************************************
*NAME:          dialogGameInfoSetup
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Sets up the gameinfo dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogGameInfoSetup(HWND hWnd) {
  char mapName[FILENAME_MAX];  /* Map Name */
  int temp;                    /* Used to hold various values */
  bool bTemp;                  /* Used for holding misc bool variables */
  char *text;
  gameType *t;

  /* Set up languages */
  text = langGetText(STR_DLGGAMEINFO_TITLE);
  SetWindowTextA(hWnd, text);
  text = langGetText(STR_DLGGAMEINFO_MAPNAME);
  SetDlgItemTextA(hWnd, IDC_MAPS, text);
  text = langGetText(STR_DLGGAMEINFO_NUMPLAYERS);
  SetDlgItemTextA(hWnd, IDC_PLAYERSS, text);
  text = langGetText(STR_DLGGAMEINFO_GAMETYPE);
  SetDlgItemTextA(hWnd, IDC_GAMETS, text);
  text = langGetText(STR_DLGGAMEINFO_HIDDENMINES);
  SetDlgItemTextA(hWnd, IDC_HIDMINESS, text);

  text = langGetText(STR_DLGGAMEINFO_ALLOWCOMPTANKS);
  SetDlgItemTextA(hWnd, IDC_COMPTANKSS, text);
  text = langGetText(STR_DLGGAMEINFO_TIMELIMIT);
  SetDlgItemTextA(hWnd, IDC_GAMETIMES, text);

  /* Write the map name */
  mapName[0] = '\0';
  timerGameInfo = SetTimer(hWnd, timerGameInfo, MILLISECONDS, NULL);
  screenGetMapName(mapName);
  SendDlgItemMessageA(hWnd, IDC_MAP_NAME, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (mapName));
  
  /* Number of players */
  dlgGameInfoNumPlayers = screenGetNumPlayers();
  mapName[0] = '\0';
  sprintf(mapName, "%d", dlgGameInfoNumPlayers);
  SendDlgItemMessageA(hWnd, IDC_NUM_PLAYERS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (mapName));
  
  /* Game type */
  t = screenGetGameType();
  switch (*t) {
  case gameOpen:
    SendDlgItemMessageA(hWnd, IDC_GAME_TYPE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) langGetText(STR_DLGGAMEINFO_OPEN));
    break;
  case gameTournament:
    SendDlgItemMessageA(hWnd, IDC_GAME_TYPE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) langGetText(STR_DLGGAMEINFO_TOURN));
    break;
  default:
    /* gameStrictTournament */
    SendDlgItemMessageA(hWnd, IDC_GAME_TYPE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) langGetText(STR_DLGGAMEINFO_STRICT));
    break;
  }

  /* Hidden Mines */
  bTemp = screenGetAllowHiddenMines();
  if (bTemp == TRUE) {
    SendDlgItemMessageA(hWnd, IDC_HIDDEN_MINES, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) langGetText(STR_YES));
  } else {
    SendDlgItemMessageA(hWnd, IDC_HIDDEN_MINES, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) langGetText(STR_NO));
  }

  /* Computer players */
  temp = screenGetAiType();
  switch (temp) {
  case aiNone:
    SendDlgItemMessageA(hWnd, IDC_COMPUTER_TANKS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) langGetText(STR_NO));
    break;
  case aiYes:
    SendDlgItemMessageA(hWnd, IDC_COMPUTER_TANKS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) langGetText(STR_YES));
    break;
  default:
    /* aiYesAdvantage */
    SendDlgItemMessageA(hWnd, IDC_COMPUTER_TANKS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) langGetText(STR_DLGGAMEINFO_AIADV));
    break;
  }

  dlgGameInfoTimeRemain = screenGetGameTimeLeft();
  if (dlgGameInfoTimeRemain == UNLIMITED_GAME_TIME) {
    SendDlgItemMessageA(hWnd, IDC_GAME_TIMELIMIT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) langGetText(STR_NO));
  }
  dialogGameInfoUpdate(hWnd);
}

/*********************************************************
*NAME:          dialogGameInfoUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
* Called each time the dialog is supposed to be updated.
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogGameInfoUpdate(HWND hWnd) {
  BYTE newPlayers; /* The number of newPlayers in the game */
  char str[FILENAME_MAX];    /* Used to update things as required */


  newPlayers = screenGetNumPlayers();
  if (newPlayers != dlgGameInfoNumPlayers) {
    dlgGameInfoNumPlayers = newPlayers;
    sprintf(str, "%d", dlgGameInfoNumPlayers);
    SendDlgItemMessageA(hWnd, IDC_NUM_PLAYERS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));
  }
    if (dlgGameInfoTimeRemain != UNLIMITED_GAME_TIME) {
      dlgGameInfoTimeRemain = screenGetGameTimeLeft();
      dlgGameInfoTimeRemain /= NUM_SECONDS_MINUTE;
      dlgGameInfoTimeRemain /= GAME_NUMGAMETICKS_SEC;
      dlgGameInfoTimeRemain++;
      sprintf(str, langGetText(STR_DLGGAMEINFO_TIMEREMAINING), dlgGameInfoTimeRemain );
      SendDlgItemMessageA(hWnd, IDC_GAME_TIMELIMIT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    }
}
