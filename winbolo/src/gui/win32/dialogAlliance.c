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
*Name:          Dialog - Alliance
*Filename:      dialogAlliance.c
*Author:        John Morrison
*Creation Date: 11/6/00
*Last Modified: 11/6/00
*Purpose:
*  Looks after the About dialog
*********************************************************/

#include <windows.h>
#include "..\..\bolo\global.h"
#include "..\..\bolo\network.h"
#include "..\winbolo.h"
#include "..\dialogalliance.h"
#include "..\positions.h"
#include "..\tiles.h"
#include "..\lang.h"
#include "..\resource.h"

BYTE allianceReqPlayerNum;

/*********************************************************
*NAME:          dialogAllianceCallback
*AUTHOR:        John Morrison
*CREATION DATE: 11/6/00
*LAST MODIFIED: 11/6/00
*PURPOSE:
*  The Alliance Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogAllianceCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  bool windowDestroyed = FALSE;
  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogAllianceInit(hWnd);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK: 
      netAllianceAccept(allianceReqPlayerNum);
      ShowWindow(hWnd, FALSE);
      break;
    case IDCANCEL:
      ShowWindow(hWnd, FALSE);
	  return TRUE;
      break;
    }
    break;
  case WM_PAINT:
    break;
  case WM_CLOSE:
    ShowWindow(hWnd, FALSE);
    break;
  case WM_DESTROY:
	windowDestroyed = ShowWindow(hWnd, FALSE);
	windowDestroyed = CloseWindow(hWnd);
	windowDestroyed = DestroyWindow(hWnd);
    break;
  }
  return FALSE;
}

/*********************************************************
*NAME:          dialogAllianceInit
*AUTHOR:        John Morrison
*CREATION DATE: 11/6/00
*LAST MODIFIED: 11/6/00
*PURPOSE:
*  The Alliance Dialog init function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogAllianceInit(HWND hWnd) {
  char *text;

  allianceReqPlayerNum = 0;
  text = langGetText(STR_DLGALLIANCE_ACCEPT);
  SetDlgItemTextA(hWnd, IDOK, text);
  text = langGetText(STR_DLGALLIANCE_DECLINE);
  SetDlgItemTextA(hWnd, IDCANCEL, text);
  text = langGetText(STR_DLGALLIANCE_TITLE);
  SetWindowTextA(hWnd, text);
}

/*********************************************************
*NAME:          dialogAllianceSetName
*AUTHOR:        John Morrison
*CREATION DATE: 11/6/00
*LAST MODIFIED: 11/6/00
*PURPOSE:
*  The Alliance Dialog init function
*
*ARGUMENTS:
*  hWnd       - Handle to the window
*  hParentWnd - The parent window handle
*  playerName - The player name to display
*  playerNum  - The player number that is requesting you 
*               alliance
*********************************************************/
void dialogAllianceSetName(HWND hWnd, HWND hParentWnd, char *playerName, BYTE playerNum) {
  char *text;
  char output[FILENAME_MAX];
  RECT rc;
  BYTE zoomFactor;

  /* Write the text */
  allianceReqPlayerNum = playerNum;
  text = langGetText(STR_DLGALLIANCE_BLURB);
  strcpy(output, playerName); 
  strcat(output, text);
  SetDlgItemTextA(hWnd, IDC_LABEL, output);
 
  /* Position the window */
  zoomFactor = windowGetZoomFactor();
  GetWindowRect(hParentWnd, &rc);
  rc.left += (zoomFactor * MAIN_OFFSET_X) + (zoomFactor * MAIN_SCREEN_SIZE_X * TILE_SIZE_X);
  SetWindowPos(hWnd, NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
  ShowWindow(hWnd, TRUE);
  SetFocus(hParentWnd);
}
