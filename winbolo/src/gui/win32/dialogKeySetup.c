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
*Name:          Dialog - Key Setup
*Filename:      dialogkeysetup.c
*Author:        John Morrison
*Creation Date: 31/1/99
*Last Modified:  1/5/00
*Purpose:
*  Looks after the keyboard setup dialog box
*********************************************************/

#include <windows.h>
#include <string.h>
#include "..\winbolo.h"
#include "..\..\bolo\backend.h"
#include "..\resource.h"
#include "..\winutil.h"
#include "..\lang.h"
#include "..\dialogkeysetup.h"
#include "..\clientmutex.h"

nextKey nk;            /* The next key to be set */
keyItems keySetupKeys; /* Copy of the keys for use by the dialog box */
HWND hKeySetupWnd;      /* Window Handle of the dialog */
HWND hGetKeyWnd = NULL; /* Window Handle of the get key window */
bool dlgKeySetupWorking = FALSE; 

/*********************************************************
*NAME:          dialogKeySetupCallback
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED:  4/1/00
*PURPOSE:
*  The Key setup call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogKeySetupCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogKeySetupInit(hWnd);
    return TRUE;
    break;
  case WM_KEYDOWN:
    if (nk != nkNone) {
      dialogKeySetupProcess(hWnd, (int) wParam);
      return TRUE;
    }
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case IDCANCEL:
      windowShowSetKeys(wsrClose);
      EndDialog(hWnd, TRUE);
      break;
    case IDOK:
      windowShowSetKeys(wsrClose);
      windowSetKeys(&keySetupKeys);
      /* Autoslowdown */
      clientMutexWaitFor();
      if (IsDlgButtonChecked(hWnd, IDC_AUTOSLOWDOWN) == BST_CHECKED) {
        screenSetTankAutoSlowdown(TRUE);
      } else {
        screenSetTankAutoSlowdown(FALSE);
      }
      /* Auto gunsight */
      if (IsDlgButtonChecked(hWnd, IDC_AUTOGUNSIGHT) == BST_CHECKED) {
        screenSetTankAutoHideGunsight(TRUE);
      } else {
        screenSetTankAutoHideGunsight(FALSE);
      }
      clientMutexRelease();
      EndDialog(hWnd, TRUE);
      break;
    case IDC_CACCEL:
      dialogKeySetupClick(hWnd, nkForward);
      return TRUE;
      break;
    case IDC_CDECEL:
      dialogKeySetupClick(hWnd, nkBackward);
      return TRUE;
      break;
    case IDC_CLEFT:
      dialogKeySetupClick(hWnd, nkTurnLeft);
      return TRUE;
      break;
    case IDC_CRIGHT:
      dialogKeySetupClick(hWnd, nkTurnRight);
      return TRUE;
      break;
    case IDC_CINCREASE:
      dialogKeySetupClick(hWnd, nkIncrease);
      return TRUE;
      break;
    case IDC_CDECREASE:
      dialogKeySetupClick(hWnd, nkDecrease);
      return TRUE;
      break;
    case IDC_CSHOOT:
      dialogKeySetupClick(hWnd, nkShoot);
      return TRUE;
      break;
    case IDC_CLAYMINE:
      dialogKeySetupClick(hWnd, nkLayMine);
      return TRUE;
      break;
    case IDC_CTANKVIEW:
      dialogKeySetupClick(hWnd, nkTankView);
      return TRUE;
      break;
    case IDC_CPILLVIEW:
      dialogKeySetupClick(hWnd, nkPillView);
      return TRUE;
      break;
    case IDC_CSCROLLUP:
      dialogKeySetupClick(hWnd, nkUp);
      return TRUE;
      break;
    case IDC_CSCROLLDOWN:
      dialogKeySetupClick(hWnd, nkDown);
      return TRUE;
      break;
    case IDC_CSCROLLLEFT:
      dialogKeySetupClick(hWnd, nkLeft);
      return TRUE;
      break;
    case IDC_CSCROLLRIGHT:
      dialogKeySetupClick(hWnd, nkRight);
      return TRUE;
      break;

    }
    return FALSE;
    break;
  case WM_PAINT:
    break;
  case WM_CLOSE:
    /* Fall through to the WM_DESTROY message */
  case WM_DESTROY:
    EndDialog(hWnd, TRUE);
    break;
  }
  return FALSE;
}

/*********************************************************
*NAME:          dialogKeySetupInit
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED:  1/5/00
*PURPOSE:
*  Setups up the dialog box
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogKeySetupInit(HWND hWnd) {
  char str[FILENAME_MAX]; /* String to output */


  /* Setup languages */
  SetWindowTextA(hWnd, langGetText(STR_DLGKEYSETUP_TITLE));
  SetDlgItemTextA(hWnd, IDC_BLURB, langGetText(STR_DLGKEYSETUP_BLURB));
  SetDlgItemTextA(hWnd, IDC_CACCEL, langGetText(STR_DLGKEYSETUP_FASTER));
  SetDlgItemTextA(hWnd, IDC_DECEL, langGetText(STR_DLGKEYSETUP_SLOWER));
  SetDlgItemTextA(hWnd, IDC_CLEFT, langGetText(STR_DLGKEYSETUP_TURNLEFT));
  SetDlgItemTextA(hWnd, IDC_CRIGHT, langGetText(STR_DLGKEYSETUP_TURNRIGHT));
  SetDlgItemTextA(hWnd, IDC_CINCREASE, langGetText(STR_DLGKEYSETUP_INCREASE));
  SetDlgItemTextA(hWnd, IDC_CDECREASE, langGetText(STR_DLGKEYSETUP_DECREASE));
  SetDlgItemTextA(hWnd, IDC_CSHOOT, langGetText(STR_DLGKEYSETUP_SHOOT));
  SetDlgItemTextA(hWnd, IDC_CLAYMINE, langGetText(STR_DLGKEYSETUP_LAYMINE));
  SetDlgItemTextA(hWnd, IDC_CTANKVIEW, langGetText(STR_DLGKEYSETUP_TANKVIEW));
  SetDlgItemTextA(hWnd, IDC_CPILLVIEW, langGetText(STR_DLGKEYSETUP_PILLVIEW));
  SetDlgItemTextA(hWnd, IDC_CSCROLLUP, langGetText(STR_DLGKEYSETUP_SCROLLUP));
  SetDlgItemTextA(hWnd, IDC_CSCROLLDOWN, langGetText(STR_DLGKEYSETUP_SCROLLDOWN));
  SetDlgItemTextA(hWnd, IDC_CSCROLLLEFT, langGetText(STR_DLGKEYSETUP_SCROLLLEFT));
  SetDlgItemTextA(hWnd, IDC_CSCROLLRIGHT, langGetText(STR_DLGKEYSETUP_SCROLLRIGHT));
  SetDlgItemTextA(hWnd, IDC_LEFTS, langGetText(STR_DLGKEYSETUP_LEFT));
  SetDlgItemTextA(hWnd, IDC_RIGHTS, langGetText(STR_DLGKEYSETUP_RIGHT));
  SetDlgItemTextA(hWnd, IDC_AUTOSLOWDOWN, langGetText(STR_DLGKEYSETUP_AUTOSLOWDOWN));
  SetDlgItemTextA(hWnd, IDC_AUTOGUNSIGHT, langGetText(STR_DLGKEYSETUP_AUTOGUNSIGHT));
  SetDlgItemTextA(hWnd, IDC_DRIVES, langGetText(STR_DLGKEYSETUP_DRIVETANK));
  SetDlgItemTextA(hWnd, IDC_ROTATES, langGetText(STR_DLGKEYSETUP_TURNTANK));
  SetDlgItemTextA(hWnd, IDC_GUNRANGES, langGetText(STR_DLGKEYSETUP_GUNRANGE));
  SetDlgItemTextA(hWnd, IDC_WEAPONS, langGetText(STR_DLGKEYSETUP_WEAPONS));
  SetDlgItemTextA(hWnd, IDC_VIEWS, langGetText(STR_DLGKEYSETUP_VIEW));
  SetDlgItemTextA(hWnd, IDC_SCROLLS, langGetText(STR_DLGKEYSETUP_SCROLL));
  SetDlgItemTextA(hWnd, IDOK, langGetText(STR_OK));
  SetDlgItemTextA(hWnd, IDCANCEL, langGetText(STR_CANCEL));



  if (screenGetTankAutoSlowdown() == TRUE) {
    CheckDlgButton(hWnd, IDC_AUTOSLOWDOWN, BST_CHECKED);
  }
  if (screenGetTankAutoHideGunsight() == TRUE) {
    CheckDlgButton(hWnd, IDC_AUTOGUNSIGHT, BST_CHECKED);
  }
  
  str[0] = '\0';
  nk = nkNone;
  windowGetKeys(&keySetupKeys);
  
  winUtilDIKeyToString(keySetupKeys.kiForward, str);
  dialogKeySetupDisplayString(hWnd, nkForward, str);
  winUtilDIKeyToString(keySetupKeys.kiBackward, str);
  dialogKeySetupDisplayString(hWnd, nkBackward, str);
  winUtilDIKeyToString(keySetupKeys.kiLeft, str);
  dialogKeySetupDisplayString(hWnd, nkTurnLeft, str);
  winUtilDIKeyToString(keySetupKeys.kiRight, str);
  dialogKeySetupDisplayString(hWnd, nkTurnRight, str);
  winUtilDIKeyToString(keySetupKeys.kiShoot, str);
  dialogKeySetupDisplayString(hWnd, nkShoot, str);
  winUtilDIKeyToString(keySetupKeys.kiLayMine, str);
  dialogKeySetupDisplayString(hWnd, nkLayMine, str);

  winUtilDIKeyToString(keySetupKeys.kiGunIncrease, str);
  dialogKeySetupDisplayString(hWnd, nkIncrease, str);
  winUtilDIKeyToString(keySetupKeys.kiGunDecrease, str);
  dialogKeySetupDisplayString(hWnd, nkDecrease, str);
  winUtilVirtKeyToString(keySetupKeys.kiTankView, str);
  dialogKeySetupDisplayString(hWnd, nkTankView, str);
  winUtilVirtKeyToString(keySetupKeys.kiPillView, str);
  dialogKeySetupDisplayString(hWnd, nkPillView, str);
  winUtilDIKeyToString(keySetupKeys.kiScrollUp, str);
  dialogKeySetupDisplayString(hWnd, nkUp, str);
  winUtilDIKeyToString(keySetupKeys.kiScrollDown, str);
  dialogKeySetupDisplayString(hWnd, nkDown, str);
  winUtilDIKeyToString(keySetupKeys.kiScrollLeft, str);
  dialogKeySetupDisplayString(hWnd, nkLeft, str);
  winUtilDIKeyToString(keySetupKeys.kiScrollRight, str);
  dialogKeySetupDisplayString(hWnd, nkRight, str);

  winUtilCenterWindow(hWnd);
}

/*********************************************************
*NAME:          dialogKeySetupDisplayString
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* Sets the given dialog item to the string given.
*
*ARGUMENTS:
*  hWnd  - Window Handle of the dialog 
*  value - The dialog item to set.
*  str   - The string to set it to.
*********************************************************/
void dialogKeySetupDisplayString(HWND hWnd, nextKey value, char *str) {
  switch (value) {
  case nkForward:
    SendDlgItemMessageA(hWnd, IDC_ACCEL, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkBackward:
    SendDlgItemMessageA(hWnd, IDC_DECEL, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkTurnLeft:
    SendDlgItemMessageA(hWnd, IDC_LEFT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkTurnRight:
    SendDlgItemMessageA(hWnd, IDC_RIGHT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkIncrease:
    SendDlgItemMessageA(hWnd, IDC_INCREASE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkDecrease:
    SendDlgItemMessageA(hWnd, IDC_DECREASE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkShoot:
    SendDlgItemMessageA(hWnd, IDC_SHOOT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkLayMine:
    SendDlgItemMessageA(hWnd, IDC_LAYMINE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkTankView:
    SendDlgItemMessageA(hWnd, IDC_TANKVIEW, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkPillView:
    SendDlgItemMessageA(hWnd, IDC_PILLVIEW, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkUp:
    SendDlgItemMessageA(hWnd, IDC_SCROLLUP, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkDown:
    SendDlgItemMessageA(hWnd, IDC_SCROLLDOWN, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkLeft:
    SendDlgItemMessageA(hWnd, IDC_SCROLLLEFT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  case nkRight:
    SendDlgItemMessageA(hWnd, IDC_SCROLLRIGHT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    break;
  default:
    break;
  }

}

/*********************************************************
*NAME:          dialogKeySetupClick
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 28/4/00
*PURPOSE:
* One of the buttons has been clicked. Check it (uncheck
* others etc.)  and set up the get key window.
*
*ARGUMENTS:
*  hWnd    - Window Handle of the dialog 
*  newKey  - The new key to get
*********************************************************/
void dialogKeySetupClick(HWND hWnd, nextKey newKey) {
  char strWndTitle[FILENAME_MAX]; /* Title of the dialog box */
  
  hKeySetupWnd = hWnd;
  strWndTitle[0] = '\0';
  strcpy(strWndTitle, langGetText(STR_DLGKEYSETUP_NEWKEYFOR));

  /* Destroy the get key window if exists */
  if (hGetKeyWnd != NULL) {
    DestroyWindow(hGetKeyWnd);
  }

  if (dlgKeySetupWorking == FALSE) {
    dlgKeySetupWorking = TRUE;
    switch (newKey) {
    case nkForward:
      if (nk == nkForward) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CACCEL, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_FORWARD));
        CheckDlgButton(hWnd, IDC_CACCEL, BST_CHECKED);
      }
      break;
    case nkBackward:
      if (nk == nkBackward) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CDECEL, BST_UNCHECKED);
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_BACKWARD));
        CheckDlgButton(hWnd, IDC_CDECEL, BST_CHECKED);
      }    
      break;
    case nkTurnLeft:
      if (nk == nkTurnLeft) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CLEFT, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_ROTATELEFT));
        CheckDlgButton(hWnd, IDC_CLEFT, BST_CHECKED);
      }
      break;
    case nkTurnRight:
      if (nk == nkTurnRight) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CRIGHT, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_ROTATERIGHT));
        CheckDlgButton(hWnd, IDC_CRIGHT, BST_CHECKED);
      }
      break;
    case nkIncrease:
      if (nk == nkIncrease) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CINCREASE, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_INCREASE));
        CheckDlgButton(hWnd, IDC_CINCREASE, BST_CHECKED);
      }
      break;
    case nkDecrease:
      if (nk == nkDecrease) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CDECREASE, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_DECREASE));
        CheckDlgButton(hWnd, IDC_CDECREASE, BST_CHECKED);
      }
      break;
    case nkShoot:
      if (nk == nkShoot) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CSHOOT, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_SHOOT));
        CheckDlgButton(hWnd, IDC_CSHOOT, BST_CHECKED);
      }
      break;
    case nkLayMine:
      if (nk == nkLayMine) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CLAYMINE, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_LAYMINE));
        CheckDlgButton(hWnd, IDC_CLAYMINE, BST_CHECKED);
      }
      break;
    case nkTankView:
      if (nk == nkTankView) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CTANKVIEW, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_TANKVIEW));
        CheckDlgButton(hWnd, IDC_CTANKVIEW, BST_CHECKED);
      }
      break;
    case nkPillView:
      if (nk == nkPillView) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CPILLVIEW, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_PILLVIEW));
        CheckDlgButton(hWnd, IDC_CPILLVIEW, BST_CHECKED);
      }
      break;
    case nkUp:
      if (nk == nkUp) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CSCROLLUP, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_SETSCROLLUP));
        CheckDlgButton(hWnd, IDC_CSCROLLUP, BST_CHECKED);
      }
      break;
    case nkDown:
      if (nk == nkDown) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CSCROLLDOWN, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_SETSCROLLDOWN));
        CheckDlgButton(hWnd, IDC_CSCROLLDOWN, BST_CHECKED);
      }
      break;
    case nkLeft:
      if (nk == nkLeft) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CSCROLLLEFT, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_SETSCROLLLEFT));
        CheckDlgButton(hWnd, IDC_CSCROLLLEFT, BST_CHECKED);
      }
      break;
    case nkRight:
      if (nk == nkRight) {
        newKey = nkNone;
        CheckDlgButton(hWnd, IDC_CSCROLLRIGHT, BST_UNCHECKED);    
      } else {
        strcat(strWndTitle, langGetText(STR_DLGKEYSETUP_SETSCROLLRIGHT));
        CheckDlgButton(hWnd, IDC_CSCROLLRIGHT, BST_CHECKED);
      }
      break;
    default:
      break;
    }
  }


  nk = newKey;
  dlgKeySetupWorking = FALSE;


  if (nk != nkNone) {
    hGetKeyWnd = CreateWindowA(
       WIND_KEYCLASSNAME,                 /* Class name */
       strWndTitle,                     /* Caption */ 
      (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU), /* Style */
       CW_USEDEFAULT, CW_USEDEFAULT,   /* Position */
       300, 0,   /* Size */
       hWnd,							             /* No parent */
       NULL,     /* Menu */
       windowGetInstance(),                          /* inst handle */
       0                               /* no params */
    );
    winUtilCenterWindow(hGetKeyWnd);
    ShowWindow(hGetKeyWnd, SW_SHOW);
  }

}

/*********************************************************
*NAME:          dialogKeySetupProcess
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED:  1/5/00
*PURPOSE:
* A key has been pressed and we we are in the state to 
* capture the next key. Process it here and update the
* screen
*
*ARGUMENTS:
*  hWnd    - Window Handle of the dialog 
*  keyCode - KeyCode of the button pressed
*********************************************************/
void dialogKeySetupProcess(HWND hWnd, int keyCode) {
  char str[FILENAME_MAX]; /* String to output */

  str[0] = '\0'; 
  switch (nk) {
  case nkForward:
    keySetupKeys.kiForward = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiForward, str);
    SendDlgItemMessageA(hWnd, IDC_ACCEL, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CACCEL, BST_UNCHECKED);
    break;
  case nkBackward:
    keySetupKeys.kiBackward = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiBackward, str);
    SendDlgItemMessageA(hWnd, IDC_DECEL, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CDECEL, BST_UNCHECKED);
    break;
  case nkTurnLeft:
    keySetupKeys.kiLeft = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiLeft, str);
    SendDlgItemMessageA(hWnd, IDC_LEFT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CLEFT, BST_UNCHECKED);
    break;
  case nkTurnRight:
    keySetupKeys.kiRight = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiRight, str);
    SendDlgItemMessageA(hWnd, IDC_RIGHT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CRIGHT, BST_UNCHECKED);
    break;
  case nkIncrease:
    keySetupKeys.kiGunIncrease = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiGunIncrease, str);
    SendDlgItemMessageA(hWnd, IDC_INCREASE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CINCREASE, BST_UNCHECKED);
    break;
  case nkDecrease:
    keySetupKeys.kiGunDecrease = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiGunDecrease, str);
    SendDlgItemMessageA(hWnd, IDC_DECREASE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CDECREASE, BST_UNCHECKED);
    break;
  case nkShoot:
    keySetupKeys.kiShoot = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiShoot, str);
    SendDlgItemMessageA(hWnd, IDC_SHOOT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CSHOOT, BST_UNCHECKED);
    break;
  case nkLayMine:
    keySetupKeys.kiLayMine = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiLayMine, str);
    SendDlgItemMessageA(hWnd, IDC_LAYMINE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CLAYMINE, BST_UNCHECKED);
    break;
  case nkTankView:
    keySetupKeys.kiTankView = keyCode;
    winUtilVirtKeyToString(keySetupKeys.kiTankView, str);
    SendDlgItemMessageA(hWnd, IDC_TANKVIEW, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CTANKVIEW, BST_UNCHECKED);
    break;
  case nkPillView:
    keySetupKeys.kiPillView = keyCode;
    winUtilVirtKeyToString(keySetupKeys.kiPillView, str);
    SendDlgItemMessageA(hWnd, IDC_PILLVIEW, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CPILLVIEW, BST_UNCHECKED);
    break;
  case nkUp:
    keySetupKeys.kiScrollUp = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiScrollUp, str);
    SendDlgItemMessageA(hWnd, IDC_SCROLLUP, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CSCROLLUP, BST_UNCHECKED);
    break;
  case nkDown:
    keySetupKeys.kiScrollDown = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiScrollDown, str);
    SendDlgItemMessageA(hWnd, IDC_SCROLLDOWN, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CSCROLLDOWN, BST_UNCHECKED);
    break;
  case nkLeft:
    keySetupKeys.kiScrollLeft = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiScrollLeft, str);
    SendDlgItemMessageA(hWnd, IDC_SCROLLLEFT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CSCROLLLEFT, BST_UNCHECKED);
    break;
  case nkRight:
    keySetupKeys.kiScrollRight = winUtilVirtKeyToDI(keyCode);
    winUtilDIKeyToString(keySetupKeys.kiScrollRight, str);
    SendDlgItemMessageA(hWnd, IDC_SCROLLRIGHT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) str);
    CheckDlgButton(hWnd, IDC_CSCROLLRIGHT, BST_UNCHECKED);
    break;
  case nkNone:
  default:
    break;
  }
  DestroyWindow(hGetKeyWnd);
  nk = nkNone;
}

/*********************************************************
*NAME:          dialogKeySetupUnCheck
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
* Get Key window has shut down. Uncheck things if they
* haven't been unchecked
*
*ARGUMENTS:
*  hWnd   - Handle to the dialog
*********************************************************/ 
void dialogKeySetupUnCheck(HWND hWnd) {

  switch (nk) {
  case nkForward:
    CheckDlgButton(hWnd, IDC_CACCEL, BST_UNCHECKED);
    break;
  case nkBackward:
    CheckDlgButton(hWnd, IDC_CDECEL, BST_UNCHECKED);
    break;
  case nkTurnLeft:
    CheckDlgButton(hWnd, IDC_CLEFT, BST_UNCHECKED);
    break;
  case nkTurnRight:
    CheckDlgButton(hWnd, IDC_CRIGHT, BST_UNCHECKED);
    break;
  case nkIncrease:
    CheckDlgButton(hWnd, IDC_CINCREASE, BST_UNCHECKED);
    break;
  case nkDecrease:
    CheckDlgButton(hWnd, IDC_CDECREASE, BST_UNCHECKED);
    break;
  case nkShoot:
    CheckDlgButton(hWnd, IDC_CSHOOT, BST_UNCHECKED);
    break;
  case nkLayMine:
    CheckDlgButton(hWnd, IDC_CLAYMINE, BST_UNCHECKED);
    break;
  case nkTankView:
    CheckDlgButton(hWnd, IDC_CTANKVIEW, BST_UNCHECKED);
    break;
  case nkPillView:
    CheckDlgButton(hWnd, IDC_CPILLVIEW, BST_UNCHECKED);
    break;
  case nkUp:
    CheckDlgButton(hWnd, IDC_CSCROLLUP, BST_UNCHECKED);
    break;
  case nkDown:
    CheckDlgButton(hWnd, IDC_CSCROLLDOWN, BST_UNCHECKED);
    break;
  case nkLeft:
    CheckDlgButton(hWnd, IDC_CSCROLLLEFT, BST_UNCHECKED);
    break;
  case nkRight:
    CheckDlgButton(hWnd, IDC_CSCROLLRIGHT, BST_UNCHECKED);
    break;
  default:
    break;
  }
}

/*********************************************************
*NAME:          GetKeyProc
*AUTHOR:        John Morrison
*CREATION DATE: 1/2/99
*LAST MODIFIED: 1/2/99
*PURPOSE:
*  The main window call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More message parameters
*********************************************************/
LRESULT CALLBACK GetKeyProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
  case WM_CREATE:
    break;
  case WM_KEYDOWN:
    if (nk != nkNone) {
      dialogKeySetupProcess(hKeySetupWnd, (int) wParam);
      DestroyWindow(hWnd);
      return TRUE;
    }
    break;
  case WM_CLOSE:
    dialogKeySetupUnCheck(hKeySetupWnd);
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}
