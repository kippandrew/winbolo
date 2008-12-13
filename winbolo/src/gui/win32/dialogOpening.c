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
*Name:          Dialog - Opening
*Filename:      dialogopening.c
*Author:        John Morrison
*Creation Date: 27/01/99
*Last Modified: 28/09/03
*Purpose:
*  Looks after the Opening/Welcoming dialog
*********************************************************/

#include <windows.h>
#include "..\..\bolo\global.h"
#include "..\winbolo.h"
#include "..\gamefront.h"
#include "resource.h"
#include "..\input.h"
#include "..\lang.h"
#include "..\dialogopening.h"
#include "..\cursor.h"
#include "..\winutil.h"
#include "..\dialogwinbolonet.h"
      
/*********************************************************
*NAME:          dialogOpeningCallback
*AUTHOR:        John Morrison
*CREATION DATE: 27/01/99
*LAST MODIFIED: 28/09/03
*PURPOSE:
*  The Opening Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogOpeningCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  keyItems keys;
  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogOpeningSetup(hWnd);
    return 0;
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDC_OPENINGHELP:
      winUtilOpenHelpFile(windowGetInstance(), hWnd);
      break;
    case IDC_WBNSETUP:
      DialogBox(windowGetInstance(), MAKEINTRESOURCE(IDD_WINBOLONET), hWnd, dialogWinbolonetCallback);
      break;
    case IDC_LANG:
      gameFrontSetDlgState(openLang);
      EndDialog(hWnd, TRUE);
      break;
    case IDC_SKINSSETUP:
      gameFrontSetDlgState(openSkins);
      EndDialog(hWnd, TRUE);
    case IDOK: 
      EndDialog(hWnd, TRUE);
      break;
    case IDC_GAMETYPE00:
      gameFrontSetDlgState(openTutorial);
      break;
    case IDC_GAMETYPE01:
      gameFrontSetDlgState(openSetup);
      break;
    case IDC_GAMETYPE02:
      gameFrontSetDlgState(openUdp);
      break;
    case IDC_GAMETYPE3:
      gameFrontSetDlgState(openLan);
      break;
    case IDC_GAMETYPE4:
      gameFrontSetDlgState(openInternet);
      break;
    default:
      /* IDQUIT */
      windowGetKeys(&keys);
      gameFrontEnd(windowGetInstance(), gameFrontGetWnd(), &keys, FALSE, TRUE);
      EndDialog(hWnd, TRUE);
      exit(0);
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

/*********************************************************
*NAME:          dialogOpeningSetup
*AUTHOR:        John Morrison
*LAST MODIFIED: 28/4/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  The Opening Dialog setup function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
void dialogOpeningSetup(HWND hWnd) {
  char *text; /* Text from language system */

  /* Setup language */
  text = langGetText(STR_DLGOPENING_TEXT);
  SetDlgItemText(hWnd, IDC_BLURB, text);
  text = langGetText(STR_DLGOPENING_OPTION0);
  SetDlgItemText(hWnd, IDC_GAMETYPE00, text);
  text = langGetText(STR_DLGOPENING_OPTION1);
  SetDlgItemText(hWnd, IDC_GAMETYPE01, text);
  text = langGetText(STR_DLGOPENING_OPTION2);
  SetDlgItemText(hWnd, IDC_GAMETYPE02, text);
  text = langGetText(STR_DLGOPENING_OPTION3);
  SetDlgItemText(hWnd, IDC_GAMETYPE4, text);
  text = langGetText(STR_DLGOPENING_OPTION4);
  SetDlgItemText(hWnd, IDC_GAMETYPE3, text);
  text = langGetText(STR_DLGOPENING_SKIP);
  SetDlgItemText(hWnd, IDC_SKIP_DIALOG, text);
  text = langGetText(STR_DLGOPENING_TITLE);
  SetWindowText(hWnd, text);
  text = langGetText(STR_DLGOPENING_BUTTON2);
  SetDlgItemText(hWnd, IDCANCEL, text);
  text = langGetText(STR_DLGOPENING_BUTTON3);
  SetDlgItemText(hWnd, IDC_LANG, text);
  text = langGetText(STR_DLGOPENING_BUTTON4);
  SetDlgItemText(hWnd, IDC_OPENINGHELP, text);
  text = langGetText(STR_OK);
  SetDlgItemText(hWnd, IDOK, text);
  gameFrontSetDlgState(openSetup);
  CheckDlgButton(hWnd, IDC_GAMETYPE01, BST_CHECKED);
}
