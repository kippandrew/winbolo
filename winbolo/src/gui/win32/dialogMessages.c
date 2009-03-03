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
*Name:          Dialog - Messages
*Filename:      dialogmessages.c
*Author:        John Morrison
*Creation Date:  5/2/99
*Last Modified: 29/4/00
*Purpose:
*  Looks after the send messages dialog box.
*********************************************************/

#include <string.h>
#include <windows.h>
#include <winuser.h>
#include <richedit.h>
#include "..\winbolo.h"
#include "..\..\bolo\backend.h"
#include "..\winutil.h"
#include "..\resource.h"
#include "..\lang.h"
#include "..\dialogmessages.h"


/* Time to wait before next message send */
#define WAIT_TIME 2000

int timerId = 1043;

/*********************************************************
*NAME:          dialogMessagesCallback
*AUTHOR:        John Morrison
*CREATION DATE: 5/1/99
*LAST MODIFIED: 3/1/00
*PURPOSE:
*  The Messages dialogbox call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogMessagesCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogMessagesSetup(hWnd);
    break;
  case WM_ACTIVATE:    
    if (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) {
      dialogMessagesChange(hWnd);
    }  
    break;
  case WM_PAINT:
    break;
  case WM_SETFOCUS:
    SetFocus(GetDlgItem(hWnd, IDC_EDITSEND)); 
    return TRUE;
    break;
  case WM_TIMER:
    EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
    KillTimer(hWnd, timerId);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK:
      /* Send message pressed */
      dialogMessagesSend(hWnd);
      return FALSE;
      break;
    case IDC_ALL_PLAYERS:
      dialogMessagesChange(hWnd);
      break;
    case IDC_ALL_ALLIES:
      dialogMessagesChange(hWnd);
      break;
      case IDC_ALL_NEARBY:
      dialogMessagesChange(hWnd);
      break;
    case IDC_ALL_SELECTION:
      dialogMessagesChange(hWnd);
      break;
    }
    break;
  case WM_CLOSE:
    /* Fall through to the WM_DESTROY message */
  case WM_DESTROY:
    windowShowSendMessages(wsrClose);
    SetFocus(windowWnd());
    EndDialog(hWnd, TRUE);
    break;
  }
  return FALSE;
}

/* void CALLBACK dialogMessagesTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
  EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
  KillTimer(hWnd, timerId);
}*/

/*********************************************************
*NAME:          dialogMessagesSetup
*AUTHOR:        John Morrison
*CREATION DATE:  5/1/99
*LAST MODIFIED: 29/4/00
*PURPOSE:
* Sets up the send message dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogMessagesSetup(HWND hWnd) {
  HWND controlWnd; /* Handle of the control to set focus on */


  /* Setup Languages */
  SetWindowTextA(hWnd, langGetText(STR_DLGMSG_TITLE));
  SetDlgItemTextA(hWnd, IDOK, langGetText(STR_DLGMSG_BUTTON)); 
  SetDlgItemTextA(hWnd, IDC_ALL_PLAYERS, langGetText(STR_DLGMSG_ALLPLAYERS));
  SetDlgItemTextA(hWnd, IDC_ALL_ALLIES, langGetText(STR_DLGMSG_ALLALLIES));
  SetDlgItemTextA(hWnd, IDC_ALL_NEARBY, langGetText(STR_DLGMSG_NEARBY));
  SetDlgItemTextA(hWnd, IDC_ALL_SELECTION, langGetText(STR_DLGMSG_SELECTION));


  winUtilCenterWindow(hWnd);
  CheckDlgButton(hWnd, IDC_ALL_PLAYERS, BST_CHECKED);
  dialogMessagesChange(hWnd);
  controlWnd = GetDlgItem(hWnd, IDC_EDITSEND);
  SendDlgItemMessageA(hWnd, IDC_EDITSEND,  EM_LIMITTEXT, 100, 0);
  SetFocus(controlWnd); 
}

/*********************************************************
*NAME:          dialogMessagesSend
*AUTHOR:        John Morrison
*CREATION DATE:  5/1/99
*LAST MODIFIED: 29/4/00
*PURPOSE:
* Send message button has been pressed. Process it here
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogMessagesSend(HWND hWnd) {
  char message[256]; /* Message to send */
  
  message[0] = '\0';
  GetDlgItemText(hWnd, IDC_EDITSEND, message, sizeof(message));
  if (message[0] != '\0') {
    /* There is something there - Send it */
    if (IsDlgButtonChecked(hWnd, IDC_ALL_PLAYERS) == BST_CHECKED) {
      screenSendMessageAllPlayers(message);
    } else if (IsDlgButtonChecked(hWnd, IDC_ALL_ALLIES) == BST_CHECKED) {
      screenSendMessageAllAllies(message);
    } else if (IsDlgButtonChecked(hWnd, IDC_ALL_NEARBY) == BST_CHECKED) {
      screenSendMessageAllNearby(message);
    } else {
      screenSendMessageAllSelected(message);
    }
    /* Disable the button */
    timerId = (int) SetTimer(hWnd, timerId, WAIT_TIME, NULL);
    EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);
    /* Select the text */
    SendDlgItemMessageA(hWnd, IDC_EDITSEND, EM_SETSEL, 0, -1);
  }
}

/*********************************************************
*NAME:          dialogMessagesChange
*AUTHOR:        John Morrison
*CREATION DATE: 7/4/99
*LAST MODIFIED: 3/1/00
*PURPOSE:
* Changes the "Sending to X players" message 
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogMessagesChange(HWND hWnd) {
  char output[256]; /* Output string                */
  int numSend;      /* Number of players to send to */

  /* Get the number of selected players */
  if (IsDlgButtonChecked(hWnd, IDC_ALL_PLAYERS) == BST_CHECKED) {
    numSend = screenGetNumPlayers();
  } else if (IsDlgButtonChecked(hWnd, IDC_ALL_ALLIES) == BST_CHECKED) {
    numSend = screenNumAllies();
  } else if (IsDlgButtonChecked(hWnd, IDC_ALL_NEARBY) == BST_CHECKED) {
    numSend = screenNumNearbyTanks();
  } else {
    numSend = screenNumCheckedPlayers();
  }

  /* Make the appropriate message */
  if (numSend == 1) {
    sprintf(output, langGetText(STR_DLGMSG_SENDPLAYER), 1);
  } else {
    sprintf(output, langGetText(STR_DLGMSG_SENDPLAYERS), numSend);
  }
  /* Print it */
  SendDlgItemMessageA(hWnd, IDC_NUM_PLAYERS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (output));
/*  SetFocus(GetDlgItem(hWnd, IDC_EDITSEND));  */
}
