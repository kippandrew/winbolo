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
*Creation Date: 27/02/05
*Last Modified: 27/02/05
*Purpose:
*  The Fullscreen setup Dialog
*********************************************************/

#include <string.h>
#include <windows.h>
#include <commdlg.h>
#include <ddraw.h>
#include "global.h"
#include "draw.h"
#include "resource.h"
#include "dialogfullscreensetup.h"

/*********************************************************
*NAME:          dialogFullscreenSetupInit
*AUTHOR:        John Morrison
*Creation Date: 27/02/05
*Last Modified: 27/02/05
*PURPOSE:
*  The Fullscreen setup Dialog setup
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogFullscreenSetupInit(HWND hWnd) {
    HWND hItem = GetDlgItem(hWnd, IDC_LIST1);
    drawEnumarateDisplays(hItem);
}

void dialogFullscreenSetupUpdate(HWND hWnd) {
    HWND hItem = GetDlgItem(hWnd, IDC_LIST1);
    char *tok;
    char input[256];
    char str[256];
    int x;
    int y;

    SendMessage(hItem, LB_GETTEXT, SendMessage(hItem, LB_GETCURSEL, 0, 0), (LPARAM) input);

    tok = strtok(input, "x");
    x = atoi(tok);
    tok = strtok(NULL, "x");
    y = atoi(tok);

    sprintf(str, "Screen Size X: %d", x / 16);
    SetDlgItemText(hWnd, IDC_SS_X, str);
    sprintf(str, "Screen Size Y: %d", x / 16);
    SetDlgItemText(hWnd, IDC_SS_Y, str);
}

/*********************************************************
*NAME:          dialogFullscreenSetupCallback
*AUTHOR:        John Morrison
*Creation Date: 27/02/05
*Last Modified: 27/02/05
*PURPOSE:
*  The Fullscreen setup Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogFullscreenSetupCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {

  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogFullscreenSetupInit(hWnd);
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
  case IDC_LIST1:
    if (HIWORD(wParam) == LBN_SELCHANGE) {
      dialogFullscreenSetupUpdate(hWnd);
    }
    break;
    case IDOK:
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

BOOL WINAPI dialogFullScreenEnumCallback(LPDDSURFACEDESC lpDDSurfaceDesc,  LPVOID lpContext) {
  HWND hWnd = ( HWND )lpContext;
  char buff[256];
  LONG iIndex = 0;

  /* Skip 8 bit modes */
  if (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount < 16) {
    return DDENUMRET_OK;
  }

  sprintf( buff, "%dx%dx%d", lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight, lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount );
  iIndex = SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM) (LPSTR) buff);
  if (iIndex == LB_ERR) {
    return DDENUMRET_CANCEL;
  }

  return DDENUMRET_OK;
}