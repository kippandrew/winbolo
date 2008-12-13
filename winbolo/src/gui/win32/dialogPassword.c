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
*Name:          Dialog - Password
*Filename:      dialogpassword.c
*Author:        John Morrison
*Creation Date: 24/2/99
*Last Modified: 29/4/00
*Purpose:
*  Looks after the Get password dialog box
*********************************************************/

#include <windows.h>
#include "..\..\bolo\global.h"
#include "..\gamefront.h"
#include "..\resource.h"
#include "..\lang.h"
#include "..\dialogpassword.h"

/*********************************************************
*NAME:          dialogPasswordCallback
*AUTHOR:        John Morrison
*CREATION DATE: 24/1/99
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  The Opening Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogPasswordCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  char pass[MAP_STR_SIZE]; /* The password to get */

  switch ( uMsg ) {
  case WM_INITDIALOG:
    /* Set languages */
    SetWindowText(hWnd, langGetText(STR_DLGPASSWORD_TITLE));
    SetDlgItemText(hWnd, IDC_BLURB, langGetText(STR_DLGPASSWORD_BLURB));
    SetDlgItemText(hWnd, IDOK, langGetText(STR_OK));
    SetFocus(GetDlgItem(hWnd, IDC_PASSWORD));
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK:
      GetDlgItemText(hWnd, IDC_PASSWORD, pass, (sizeof(pass)));
      gameFrontSetGameOptions(pass, gameOpen, FALSE, FALSE, 0, 0, TRUE);
      EndDialog(hWnd, TRUE);
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
