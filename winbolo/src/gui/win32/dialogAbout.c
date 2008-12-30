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
*Name:          Dialog - About
*Filename:      dialogAbout.h
*Author:        John Morrison
*Creation Date: 4/11/99
*Last Modified: 28/4/00
*Purpose:
*  Looks after the About dialog
*********************************************************/

#include <windows.h>
#include "..\dialogAbout.h"
#include "..\lang.h"
#include "..\resource.h"

/*********************************************************
*NAME:          dialogAboutCallback
*AUTHOR:        John Morrison
*CREATION DATE: 27/1/99
*LAST MODIFIED: 28/4/00
*PURPOSE:
*  The About Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogAboutCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogAboutInit(hWnd);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK: 
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

/*********************************************************
*NAME:          dialogAboutInit
*AUTHOR:        John Morrison
*CREATION DATE: 28/4/00
*LAST MODIFIED: 28/4/00
*PURPOSE:
*  The About Dialog init function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogAboutInit(HWND hWnd) {
  char *text;

  text = langGetText(STR_OK);
  SetDlgItemTextA(hWnd, IDOK, text);
  text = langGetText(STR_DLGABOUT_BLURB);
  SetDlgItemTextA(hWnd, IDC_BLURB, text);
  text = langGetText(STR_DLGABOUT_TITLE);
  SetWindowTextA(hWnd, text);
}
