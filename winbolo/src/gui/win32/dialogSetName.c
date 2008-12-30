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
*Name:          Dialog - Set Player Name
*Filename:      dialogSetname.h
*Author:        John Morrison
*Creation Date: 01/02/99
*Last Modified: 29/04/00
*Purpose:
*  Looks after the set player name dialog box.
*********************************************************/

#include <string.h>
#include <windows.h>
#include "..\..\bolo\global.h"
#include "..\resource.h"
#include "..\lang.h"
#include "..\winbolo.h"
#include "..\..\bolo\backend.h"
#include "..\winutil.h"
#include "..\lang.h"
#include "..\gamefront.h"
#include "..\dialogsetname.h"

bool setNameInGame = FALSE; /* Are we in a game or not */

/*********************************************************
*NAME:          dialogSetNameCallback
*AUTHOR:        John Morrison
*CREATION DATE: 31/02/99
*LAST MODIFIED: 31/01/02
*PURPOSE:
*  The Set Player name dialog box call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogSetNameCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogSetNameInit(hWnd);
    return TRUE;
    break;
  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK) {
      dialogSetNameDone(hWnd);
      return TRUE;
    }
    return FALSE;
    break;
  case WM_SHOWWINDOW:
    SendDlgItemMessageA(hWnd, IDC_EDITNAME, EM_SETSEL, 0, -1);
    SetFocus(GetDlgItem(hWnd, IDC_EDITNAME));
    break;
  case WM_CLOSE:
    /* Fall through to the WM_DESTROY message */
  case WM_DESTROY:
    windowShowSetPlayerName(wsrClose);
    EndDialog(hWnd, TRUE);
    break;
  }
  return FALSE;
}

/*********************************************************
*NAME:          dialogSetNameInit
*AUTHOR:        John Morrison
*CREATION DATE: 01/02/99
*LAST MODIFIED: 31/01/02
*PURPOSE:
* Sets up the set player name dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogSetNameInit(HWND hWnd) {
  char playerName[FILENAME_MAX]; /* Player Name */

  /* Set language */
  SetWindowTextA(hWnd, langGetText(STR_DLGSETNAME_TITLE));
  SetDlgItemTextA(hWnd, IDC_BLURB, langGetText(STR_DLGSETNAME_BLURB));
  SetDlgItemTextA(hWnd, IDOK, langGetText(STR_OK));

  /* Get the name */
  playerName[0] = '\0';
  if (setNameInGame == TRUE) {
    screenGetPlayerName(playerName);
  } else {
    gameFrontGetPlayerName(playerName);
  }
  SendDlgItemMessageA(hWnd, IDC_EDITNAME, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (playerName));
  /* Center the window */
  winUtilCenterWindow(hWnd);
}


/*********************************************************
*NAME:          dialogSetNameDone
*AUTHOR:        John Morrison
*CREATION DATE: 01/02/99
*LAST MODIFIED: 30/01/02
*PURPOSE:
* Called when the OK button is pressed to update the player
* name
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogSetNameDone(HWND hWnd) {
  bool closeDialog;              /* Should the dialog be closed */
  bool changeOK;                 /* Is the name change ok */
  char oldName[PLAYER_NAME_LEN]; /* Old player name */
  char newName[PLAYER_NAME_LEN]; /* New player name */

  /* Set the name */
  oldName[0] = '\0';
  newName[0] = '\0';
  closeDialog = FALSE;
  changeOK = TRUE;

  screenGetPlayerName(oldName);
  GetDlgItemText(hWnd, IDC_EDITNAME, newName, (sizeof(newName)-1));
  newName[PLAYER_NAME_LAST-1] = '\0';
  utilStripName(newName);
  if (newName[0] == EMPTY_CHAR) {
    MessageBoxA(hWnd, "Sorry, you can not leave this blank", DIALOG_BOX_TITLE, MB_ICONINFORMATION);
  } else if (newName[0] == '*') {
    MessageBoxA(hWnd, "Sorry, names can not begin with a '*'", DIALOG_BOX_TITLE, MB_ICONINFORMATION);
  } else if (strcmp(oldName, newName) == 0) {
    /* Names are the same */
    closeDialog = TRUE;
  } else {
    if (setNameInGame == TRUE) {
      changeOK = screenSetPlayerName(newName);
      if (changeOK == FALSE) {
        if (strcmp(oldName, newName) != 0) {
          /* Someone else in the game must have our name already - Can't change */
          MessageBoxA(hWnd, langGetText(NETERR_PLAYERNAMEINUSE), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
        }
      } else {
        closeDialog = TRUE;
      }
    } else {
      /* Tell game front about */
      closeDialog = TRUE;
      gameFrontSetPlayerName(newName);
    }

  }

  /* Close the dialog if necessary */
  if (closeDialog == TRUE) {
    windowShowSetPlayerName(wsrClose);
    EndDialog(hWnd, TRUE);
  }
}

/*********************************************************
*NAME:          dialogSetNameInGame
*AUTHOR:        John Morrison
*CREATION DATE: 2/6/00
*LAST MODIFIED: 2/6/00
*PURPOSE:
* Sets whether we are inside a game or not
*
*ARGUMENTS:
*  inGame - Sets if we are inside a game or not
*********************************************************/
void dialogSetNameInGame(bool inGame) {
  setNameInGame = inGame;
}
