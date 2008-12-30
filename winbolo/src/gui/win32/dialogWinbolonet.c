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
*Name:          Dialog - WinBolonet
*Filename:      dialogwinbolonet.c
*Author:        John Morrison
*Creation Date: 24/06/02
*Last Modified: 15/02/03
*Purpose:
*  Our Winbolo.net information
*********************************************************/

#include <windows.h>
#include "..\..\bolo\global.h"
#include "..\gamefront.h"
#include "resource.h"
#include "..\lang.h"
#include "..\dialogwinbolonet.h"

/*********************************************************
*NAME:          dialogWinbolonetSetup
*AUTHOR:        John Morrison
*CREATION DATE: 24/06/02
*LAST MODIFIED: 15/02/03
*PURPOSE:
*  Sets up the dialog window
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogWinbolonetSetup(HWND hWnd) {
  HWND dlgWnd;
  char userName[FILENAME_MAX];
  char password[FILENAME_MAX];
  bool useWbn;
  bool savePass;

  gameFrontGetPlayerName(userName);
  gameFrontGetWinbolonetSettings(password, &useWbn, &savePass);
  SetDlgItemTextA(hWnd, IDC_WBNNAME, userName);
  SetDlgItemTextA(hWnd, IDC_WBNPASS, password);

  dlgWnd = GetDlgItem(hWnd, IDC_WBNPASS);  
  if (useWbn == TRUE) {
    CheckDlgButton(hWnd, IDC_USEWBN, BST_CHECKED);
    EnableWindow(dlgWnd, TRUE);
  } else {
    EnableWindow(dlgWnd, FALSE);
  }
  if (savePass == TRUE) {
    CheckDlgButton(hWnd, IDC_SAVEPASS, BST_CHECKED);
  }
}

/*********************************************************
*NAME:          dialogWinbolonetSetup
*AUTHOR:        John Morrison
*CREATION DATE: 24/06/02
*LAST MODIFIED: 24/06/02
*PURPOSE:
* Called on dialog exit. Returns if the dialog can close
* or not because of configuration errors
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
bool dialogWinbolonetOK(HWND hWnd) {
  bool returnValue = TRUE; /* Value to return */
  char password[FILENAME_MAX];
  bool useWbn = FALSE;
  bool savePass = FALSE;

  if (IsDlgButtonChecked(hWnd, IDC_USEWBN) == BST_CHECKED) {
    useWbn = TRUE;
  }
  if (IsDlgButtonChecked(hWnd, IDC_SAVEPASS) == BST_CHECKED) {
    savePass = TRUE;
  }

  GetDlgItemText(hWnd, IDC_WBNPASS, password, (sizeof(password)-1));
  if (useWbn == TRUE && strlen(password) == 0) {
    returnValue = FALSE;
    MessageBoxA(NULL, "Sorry, you must enter a username and password if you wish to participate in winbolo.net", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
  }

  if (returnValue == TRUE) {
    /* Save values */
    gameFrontSetWinbolonetSettings(password, useWbn, savePass);
  }
  return returnValue;
}

/*********************************************************
*NAME:          dialogWinbolonetCallback
*AUTHOR:        John Morrison
*CREATION DATE: 24/06/02
*LAST MODIFIED: 24/06/02
*PURPOSE:
*  The Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogWinbolonetCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND dlgWnd;
  switch ( uMsg ) {
  case WM_INITDIALOG:
    /* Set languages */
    dialogWinbolonetSetup(hWnd);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDCANCEL:
      EndDialog(hWnd, TRUE);
      break;
    case IDOK:
      if (dialogWinbolonetOK(hWnd) == TRUE) {
        EndDialog(hWnd, TRUE);
      }
      break;
    case IDC_WBNMORE:
      /* Open information webpage */
      ShellExecute(hWnd, "open", "http://www.winbolo.net/", NULL, NULL,SW_SHOWNORMAL);
      break;
    case IDC_USEWBN:
      dlgWnd = GetDlgItem(hWnd, IDC_WBNPASS);
      if (IsDlgButtonChecked(hWnd, IDC_USEWBN) == BST_UNCHECKED) {
        EnableWindow(dlgWnd, FALSE);
      } else {
        EnableWindow(dlgWnd, TRUE);
      }
      break;
    }
    break;
  case WM_PAINT:
    break;
  case WM_DESTROY:
    break;
  }
  return FALSE;
}
