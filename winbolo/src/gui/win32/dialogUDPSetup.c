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
*Name:          Dialog - UDP Setup
*Filename:      dialogudpsetup.c
*Author:        John Morrison
*Creation Date: 20/2/99
*Last Modified: 22/6/00
*Purpose:
*  Looks after the UDP (Interet) setup dialog box
*********************************************************/

#include <stdlib.h>
#include <windows.h>
#include <winuser.h>
#include <richedit.h>
#include "..\..\bolo\global.h"
#include "..\gamefront.h"
#include "..\resource.h"
#include "..\lang.h"
#include "..\dialogtrackersetup.h"
#include "..\dialogudpsetup.h"

FARPROC oldWndProc;
HWND dlgWnd;


/*********************************************************
*NAME:          dialogUdpSetupCallback
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 28/4/00
*PURPOSE:
*  The UDP Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogUdpSetupCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  dlgWnd = hWnd;

  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogUdpSetupInit(hWnd);
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case IDCANCEL:
      /* Cancel */
      gameFrontSetDlgState(openWelcome);
      EndDialog(hWnd,TRUE);
      break;
    case IDC_TRACKER:
      DialogBox(windowGetInstance(), MAKEINTRESOURCE(IDD_TRACKERSETUP), hWnd, dialogTrackerSetupCallback);
      break;
    case ID_NEW:
      {   char str[FILENAME_MAX];   /* Temp string for conversions */
      bool bad = FALSE;
      unsigned short udp;       /* Udp ports */
      unsigned short myUdp;
      GetDlgItemText(hWnd, IDC_UDP, str, (sizeof(str)));
      udp = atoi(str);
      if (udp < 0) {
        bad = TRUE;
      }
      GetDlgItemText(hWnd, IDC_MYUDP, str, (sizeof(str)));
      myUdp = atoi(str);
      if (myUdp < 0) {
        bad = TRUE;
      }

      if (bad == TRUE || udp == myUdp) {
        MessageBox(NULL, langGetText(STR_ERR_DLGTCP_PORTS), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
      } else if (dialogUdpSetupDone(hWnd, FALSE) == TRUE) {
        gameFrontSetDlgState(openUdpSetup);
        EndDialog(hWnd,TRUE);
      } else {
        MessageBox(hWnd, langGetText(STR_ERR_DLGTCP_NOTRIGHT), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
      }
      }
      break;
    case ID_JOIN:
      if (dialogUdpSetupDone(hWnd, TRUE) == TRUE) {
        EnableWindow(hWnd, FALSE);
        gameFrontSetDlgState(openUdpJoin);
        EndDialog(hWnd,TRUE);
      } else {
        MessageBox(hWnd, langGetText(STR_ERR_DLGTCP_NOTRIGHT), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
      }
      break;
    case ID_REJOIN:
      if (dialogUdpSetupDone(hWnd, TRUE) == TRUE) {
        EnableWindow(hWnd, FALSE);
        gameFrontEnableRejoin();
        gameFrontSetDlgState(openUdpJoin);
        EndDialog(hWnd,TRUE);
      } else {
        MessageBox(hWnd, langGetText(STR_ERR_DLGTCP_NOTRIGHT), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
      }
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

/*********************************************************
*NAME:          dialogUdpSetupIpCallback
*AUTHOR:        John Morrison
*CREATION DATE: 21/11/99
*LAST MODIFIED:  22/6/00
*PURPOSE:
* Callback for the target IP address control window to 
* handle pasting of "ip:port" text to auto split it up
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogUdpSetupIpCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  HANDLE cbHandle; /* handle to the clipboard data */
  char *dataPtr;   /* pointer to clipboard data */
  char str[1024];  /* String to paste in */
  int max;         /* Max string size  */
  int count;       /* Looping variable */
  int count2;      /* Looping variable */

  switch (uMsg) {
  case WM_PASTE:
    if (IsClipboardFormatAvailable(CF_TEXT)) {
      if (OpenClipboard(hWnd)) {
        cbHandle = GetClipboardData(CF_TEXT);
        dataPtr = GlobalLock(cbHandle);
        max = strlen(dataPtr);
        count = 0;
        
        /* Strip white space */
        while (count < max && dataPtr[count] == ' ') {
          count++;
        }
        /* Get the ip */
        count2 = 0;
        while (count < max && dataPtr[count] != ':') {
          str[count2] = dataPtr[count];
          count2++;
          count++;
        }
        str[count2] = '\0';
        SetDlgItemText(dlgWnd, IDC_EDITIP, str);
        /* Get the port */
        count++;
        count2 = 0;
        if (count < max) {
          while (count < max) {
            if (dataPtr[count] >= 48 && dataPtr[count] <=57) {
              str[count2] = dataPtr[count];
              count2++;
            }
            count++;
          }
          str[count2] = '\0';
          SetDlgItemText(dlgWnd, IDC_UDP, str);
        }
        GlobalUnlock(dataPtr);
        CloseClipboard();
      }
      
    }
    return 0;
    break;
  }
  return CallWindowProc((WNDPROC)oldWndProc, hWnd, uMsg, wParam, lParam);
}

/*********************************************************
*NAME:          dialogUdpSetupInit
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 22/6/00
*PURPOSE:
* Called on dialog startup to set up.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogUdpSetupInit(HWND hWnd) { 
  HWND dlgElemWnd;         /* Dialog Element Window */
  char pn[FILENAME_MAX];   /* Player name */
  char add[FILENAME_MAX];  /* Target machine address */
  unsigned short myUdp;    /* Udp Ports */
  unsigned short targetUdp;
  char str[FILENAME_MAX];  /* Used to hold str of UDP */

  /* Set language */
  SetWindowText(hWnd, langGetText(STR_DLGTCP_TITLE));
  SetDlgItemText(hWnd, IDC_BLURB, langGetText(STR_DLGTCP_BLURB));
  SetDlgItemText(hWnd, IDCANCEL, langGetText(STR_CANCEL));
  SetDlgItemText(hWnd, IDC_TRACKER, langGetText(STR_DLGTCP_TRACKERSETUP));
  SetDlgItemText(hWnd, ID_JOIN, langGetText(STR_DLGTCP_JOIN));
  SetDlgItemText(hWnd, IDC_NEW, langGetText(STR_DLGTCP_NEW));
  SetDlgItemText(hWnd, IDC_CHECK_REMEMBER, langGetText(STR_DLGTCP_REMEMBER));
  SetDlgItemText(hWnd, IDC_ADDRESSS, langGetText(STR_DLGTCP_MACHINENAME));
  SetDlgItemText(hWnd, IDC_THEREUDPS, langGetText(STR_DLGTCP_THEREUDP));
  SetDlgItemText(hWnd, IDC_OURUDPS, langGetText(STR_DLGTCP_USUDP));
  SetDlgItemText(hWnd, IDC_PLAYERNAMES, langGetText(STR_DLGTCP_NAME));
  SetDlgItemText(hWnd, IDC_NEWS, langGetText(STR_DLGTCP_NEWBLURB));
  SetDlgItemText(hWnd, IDC_JOINS, langGetText(STR_DLGTCP_JOINBLURB));
  SetDlgItemText(hWnd, IDC_REJOINS, langGetText(STR_DLGTCP_REJOINBLURB));
  SetDlgItemText(hWnd, ID_REJOIN, langGetText(STR_DLGTCP_REJOIN));
  
  gameFrontGetUdpOptions(pn, add, &targetUdp, &myUdp);
  
  /* Set the textboxes up */
  itoa(targetUdp, str, BASE_10);
  SetDlgItemText(hWnd, IDC_UDP, str);
  itoa(myUdp, str, BASE_10);
  SetDlgItemText(hWnd, IDC_MYUDP, str);
  SetDlgItemText(hWnd, IDC_PLAYERNAME, pn);
  SetDlgItemText(hWnd, IDC_EDITIP, add);

  if (gameFrontGetRemeber() == TRUE) {
    CheckDlgButton(hWnd, IDC_CHECK_REMEMBER, BST_CHECKED);
  }

  /* Set the edit control callback for pasting */
  dlgElemWnd = GetDlgItem(hWnd, IDC_EDITIP);
  oldWndProc = (FARPROC) SetWindowLong(dlgElemWnd, GWL_WNDPROC, (DWORD)dialogUdpSetupIpCallback);
  if (oldWndProc == 0) {
    MessageBox(hWnd, langGetText(STR_ERR_DLGTCP_SUBCLASS), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
  }
}

/*********************************************************
*NAME:          dialogUdpSetupDone
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 24/2/99
*PURPOSE:
* Called on dialog finishing to pass the game options
* back to the gameFront module. Returns whether all 
* parameters are valid
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  isJoin - True if we are joing a game else false
*********************************************************/
bool dialogUdpSetupDone(HWND hWnd, bool isJoin) {
  bool returnValue;         /* Value to return */
  char str[FILENAME_MAX];   /* Temp string for conversions */
  char add[FILENAME_MAX];   /* Target address */
  char pn[PLAYER_NAME_LEN]; /* Player name */
  unsigned short udp;       /* Udp ports */
  unsigned short myUdp;

  returnValue = TRUE;
  str[0] ='\0';
  GetDlgItemText(hWnd, IDC_PLAYERNAME, pn, (sizeof(pn)-1));
  utilStripName(pn);
  if (strlen(pn) == 0) {
    returnValue = FALSE;
  } else if (pn[0] == '*') {
    MessageBox(hWnd, "Sorry, names can not begin with a '*'", DIALOG_BOX_TITLE, MB_ICONINFORMATION);
	returnValue = FALSE;
  }


  GetDlgItemText(hWnd, IDC_EDITIP, add, (sizeof(add)));
  if (strlen(add) == 0 && isJoin == TRUE) {
    returnValue = FALSE;
  }
  GetDlgItemText(hWnd, IDC_UDP, str, (sizeof(str)));
  udp = atoi(str);
  if (udp < 0) {
    returnValue = FALSE;
  }
  GetDlgItemText(hWnd, IDC_MYUDP, str, (sizeof(str)));
  myUdp = atoi(str);
  if (myUdp < 0) {
    returnValue = FALSE;
  }

  if (IsDlgButtonChecked(hWnd, IDC_CHECK_REMEMBER) == BST_CHECKED) {
    gameFrontSetRemeber(TRUE);
  } else {
    gameFrontSetRemeber(FALSE);
  }

  if (returnValue == TRUE) {
    /* Send to the gamefront module */
    gameFrontSetUdpOptions(pn, add, udp, myUdp);
  }

  return returnValue;
}
