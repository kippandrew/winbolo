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
*Name:          Dialog - Tracker Setup
*Filename:      dialogTrackerSetup.c
*Author:        John Morrison
*Creation Date: 13/11/99
*Last Modified:  30/4/00
*Purpose:
*  Looks after the tracker setup dialog box
*********************************************************/

#include <stdlib.h>
#include <windows.h>
#include "..\..\bolo\global.h"
#include "..\gamefront.h"
#include "..\lang.h"
#include "..\resource.h"
#include "..\dialogtrackersetup.h"

/*********************************************************
*NAME:          dialogTrackerSetupCallback
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
*  The Tracker Setup Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogTrackerSetupCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND dlgWnd; /* Dialog Item Window Handle */

  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogTrackerSetupInit(hWnd);
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case IDCANCEL:
      /* Cancel */
      EndDialog(hWnd, TRUE);
      break;
    case IDOK:
      if (dialogTrackerSetupDone(hWnd) == TRUE) {
        EndDialog(hWnd, TRUE);
      }
      break;
    case IDC_USETRACKER:
      dlgWnd = GetDlgItem(hWnd, IDC_TRACKERADDRESS);
      if (IsDlgButtonChecked(hWnd, IDC_USETRACKER) == BST_UNCHECKED) {
        EnableWindow(dlgWnd, FALSE);
        dlgWnd = GetDlgItem(hWnd, IDC_TRACKERPORT);
        EnableWindow(dlgWnd, FALSE);
      } else {
        EnableWindow(dlgWnd, TRUE);
        dlgWnd = GetDlgItem(hWnd, IDC_TRACKERPORT);
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

/*********************************************************
*NAME:          dialogTrackerSetupInit
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED:  30/4/00
*PURPOSE:
* Called on dialog startup to set up.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogTrackerSetupInit(HWND hWnd) { 
  HWND dlgWnd;             /* Dialog items HWND */
  char addr[FILENAME_MAX]; /* Tracker address   */
  unsigned short port;     /* Port              */
  bool enabled;            /* Use of tracker enabled */
  
  /* Set the language */
  SetWindowText(hWnd, langGetText(STR_DLGTRACKER_TITLE));
  SetDlgItemText(hWnd, IDOK, langGetText(STR_OK));
  SetDlgItemText(hWnd, IDCANCEL, langGetText(STR_CANCEL));
  SetDlgItemText(hWnd, IDC_USETRACKER, langGetText(STR_DLGTRACKER_USETRACKER));
  SetDlgItemText(hWnd, IDC_TRACKERADDRESSS, langGetText(STR_DLGTRACKER_TRACKERADDRESS));
  SetDlgItemText(hWnd, IDC_TRACKERPORTS, langGetText(STR_DLGTRACKER_TRACKERPORT));

  gameFrontGetTrackerOptions(addr, &port, &enabled);
  /* Set the textboxes up */

  SetDlgItemText(hWnd, IDC_TRACKERADDRESS, addr);
  itoa(port, addr, BASE_10);
  SetDlgItemText(hWnd, IDC_TRACKERPORT, addr);

  if (enabled == TRUE) {
    dlgWnd = GetDlgItem(hWnd, IDC_TRACKERADDRESS);
    EnableWindow(dlgWnd, TRUE);
    dlgWnd = GetDlgItem(hWnd, IDC_TRACKERPORT);
    EnableWindow(dlgWnd, TRUE);
    CheckDlgButton(hWnd, IDC_USETRACKER, BST_CHECKED);
  }
}

/*********************************************************
*NAME:          dialogTrackerSetupDone
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
* Called on dialog finishing to pass the game options
* back to the gameFront module. Returns whether all 
* parameters are valid
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
bool dialogTrackerSetupDone(HWND hWnd) {
  bool returnValue;         /* Value to return */
  char addr[FILENAME_MAX]; /* Tracker address   */
  unsigned short port;     /* Port              */
  bool enabled;            /* Use of tracker enabled */

  returnValue = TRUE;
  enabled = TRUE;
  if (IsDlgButtonChecked(hWnd, IDC_USETRACKER) == BST_UNCHECKED) {
    enabled = FALSE;
  }

  GetDlgItemText(hWnd, IDC_TRACKERPORT, addr, sizeof(addr));
  port = (unsigned short) atoi(addr);
  GetDlgItemText(hWnd, IDC_TRACKERADDRESS, addr, sizeof(addr));
  gameFrontSetTrackerOptions(addr, port, enabled);

  return returnValue;
}
