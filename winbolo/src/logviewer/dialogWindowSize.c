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
*Name:          Dialog - Window Size
*Filename:      dialogWindowSize.c
*Author:        John Morrison
*Creation Date: 07/05/01
*Last Modified: 07/05/01
*Purpose:
*  Looks after the Game setup dialog
*********************************************************/

#include <windows.h>
#include <commdlg.h>
#include "global.h"
#include "backend.h"
#include "resource.h"

#include "dialogwindowsize.h"

void windowResize();

/*********************************************************
*NAME:          dialogWindowSizeCallback
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  The Opening Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogWindowSizeCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  char str[100];
  BYTE x;
  BYTE y;

  switch ( uMsg ) {
  case WM_INITDIALOG:
    sprintf(str, "%d", screenGetSizeX());
    SetDlgItemText(hWnd, IDC_WIDTH, str);
    sprintf(str, "%d", screenGetSizeY());
    SetDlgItemText(hWnd, IDC_HEIGHT, str);    
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case IDOK:
      GetDlgItemText(hWnd, IDC_WIDTH, str, sizeof(str));
      x = atoi(str);
      GetDlgItemText(hWnd, IDC_HEIGHT, str, sizeof(str));
      y = atoi(str);
      if (x < 5 || x> 99 || y<5 || y>99) {
        MessageBox(hWnd, "Window width and height dimensions must be between 5-99 map squares.,", "Log Viewer", MB_OK); 
      } else {
        screenSetSizeX(x);
        screenSetSizeY(y);;
        windowResize();
        EndDialog(hWnd,TRUE);
      }
      break;
    case IDCANCEL:
      /* Cancel */
      EndDialog(hWnd,TRUE);
      break;
    default:
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
