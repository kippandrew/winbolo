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
*Name:          WinBolo Brain Template
*Filename:      BrainTemplate.c
*Author:        John Morrison
*Creation Date: 7/12/99
*Last Modified:  8/1/00
*Purpose:
* This file is a sample skeleton brain that can be used
* to assist in writing plugin brains for WinBolo.
*
* 8/1/00 -  Added code submitted by db48 that fixes the
*           menu popup.
* 7/12/99 - Initial revision.
*
* WinBolo (C) 1998-2000 John Morrison
* Bolo    (C) 1987-1995 Stuart Cheshire
*
*********************************************************/

/* Includes */

#include "brain.h"
#include  <windows.h>
#include "resource.h"

/* Defines */

/* What we want to call our menu entry */
#define BRAIN_MENU_NAME "Template" 
/* Generic brain error */
#define BRAIN_ERROR -1

/* Converts 0 = MF_UNCHECKED, 1 == MF_CHECKED for menu checking
   Useful converting Mac's CheckItem menu function call which takes
   0 and 1's for checked arguments */
#define MENU_CHECK_WIN32(X) ((X==0) ? (MF_UNCHECKED) : (MF_CHECKED))

/* Mac->Window function calls defines. Although these aren't used
   in this template that may prove to be useful to brain authors
   porting Mac Bolo brain code to windows 

  #define TickCount() (GetTickCount() /60) // TickCount() = Number of 1/60 second ticks since start up 
  #define SysBeep(X) (Beep(0, X)) // System beep
  #define NewPtr(X) (malloc(X))   // Memory allocate
  #define DisposPtr(X) (free(X))  // Memory deallocate
  #define CheckItem(X,Y,Z) (CheckMenuItem(X, Y, MENU_CHECK_WIN32(Z))) // Checking menu items 
  #define ShowWindow(X) (ShowWindow(X, SW_SHOW)) // Showing windows
  #define HideWindow(X) (ShowWindow(X, SW_HIDE)) // Hiding windows
 
  Mac Specific Types -> Windows Specific types
  typedef HMENU MenuHandle;
  typedef HWND WindowPtr; */

/* Globals */
HINSTANCE brainInst;      /* This Brains instance */
HMENU brainMenu = NULL;   /* Our Menu             */
HWND hAboutDialog = NULL; /* About Dialogbox      */
Boolean opt1, opt2, opt3; /* Menu options         */

/* Functions */

/* About dialog box callback. We use dialogbox rather then a simple
   MessageBox because a MessageBox stops WinBolo from continuing 
   until the user has clicked OK in the MessageBox. */
BOOL CALLBACK aboutDialogCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_COMMAND && LOWORD(wParam) == IDOK) {
    /* OK Pressed. Hide the window till next time it is called */
    ShowWindow(hAboutDialog, SW_HIDE);
  }
  return FALSE;
}


/* Sample brain think call. Returns TRUE. 
   Add brain details here. */
Boolean brainThink(const BrainInfo *info) {
  return TRUE;
}

/* A sample brainOpen function. It shows how to add a menu entry 
   onto WinBolo's menu and creates an about dialogbox. Returns 
   success of the opertaion. */
Boolean brainOpen(void) {
  Boolean returnValue; /* Value to return          */
  HWND hMainWnd;       /* Handle to WinBolo window */
  HMENU hMenu;         /* Handle to WinBolo's menu */

  returnValue = TRUE;
  brainMenu = NULL;
  opt1 = 0;
  opt2 = 0;
  opt3 = 0;
  hMainWnd = GetActiveWindow();
  hMenu = GetMenu(hMainWnd);

  /* Check we could get handles to the WinBolo window */
  if (hMenu == NULL || hMainWnd == NULL) {
    returnValue = FALSE;
  }
  
  /* Load our resouce */
  if (returnValue == TRUE) {
    brainMenu = LoadMenu(brainInst, MAKEINTRESOURCE(IDR_BRAIN_MENU));
    if (brainMenu == NULL) {
      returnValue = FALSE;
    }
  }

  /* Added our menu */
  if (returnValue == TRUE) {
    AppendMenu(hMenu, MF_POPUP, (UINT) brainMenu, BRAIN_MENU_NAME);
    DrawMenuBar(hMainWnd);
  }

  /* Load about box */
  if (returnValue == TRUE) {
    hAboutDialog = CreateDialog(brainInst, MAKEINTRESOURCE(IDD_ABOUT), NULL, aboutDialogCallback);
    if (hAboutDialog == NULL) {
      returnValue = FALSE;
    }
  }
  /* Add any brain specific startup code here... */

  return returnValue;
}

/* This is the brains closing function. It should free all 
   resources it uses as well as remove and menus it creates */
void brainClose(void) {
  HWND hMainWnd; /* Handle to WinBolo window */
  HMENU hMenu;   /* Handle to WinBolo's menu */

  if (brainMenu != NULL) {
    /* We added a menu so we shoudl remove it */
    hMainWnd = GetActiveWindow();
    hMenu = GetMenu(hMainWnd);
    DestroyMenu(brainMenu);
    RemoveMenu(hMenu, GetMenuItemCount(hMenu)-1, MF_BYPOSITION);
    DrawMenuBar(hMainWnd);
    brainMenu = NULL;
  }

  if (hAboutDialog != NULL) {
    /* Remove the about dialog box */
    EndDialog(hAboutDialog, TRUE);
    DestroyWindow(hAboutDialog);
  }
  /* Add any brain specific shutdown code here... */
}

/* This is a sample menu handler function. */
void brainMenuHandler(const BrainInfo *brainInfo) {
  switch (brainInfo->menu_item) {
  case ID_BRAINTEMPLATE_ABOUT:
    ShowWindow(hAboutDialog, SW_SHOW);
    SetForegroundWindow(hAboutDialog);
    break;
  case ID_BRAINTEMPLATE_OPTION1:
    opt1 ^= 1;
    CheckMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION1, MENU_CHECK_WIN32(opt1));
    break;
  case ID_BRAINTEMPLATE_OPTION2:
    opt2 ^= 1;
    CheckMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION2, MENU_CHECK_WIN32(opt2));
    break;
  case ID_BRAINTEMPLATE_OPTION3:
    opt3 ^= 1;
    CheckMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION3, MENU_CHECK_WIN32(opt3));
    break;
  }
}


/* The exported brain function that gets called.
   Returns 0 if OK or anything else on an error. 
   Remember if you return an error you will not be called again with 
   the BRAIN_CLOSE message */
__declspec( dllexport ) short BrainMain(const BrainInfo *brainInfo) {
  short returnValue; /* Value to return */
    
  returnValue = 0;
  if (brainInfo->InfoVersion != CURRENT_BRAININFO_VERSION) {
    returnValue = BRAIN_ERROR;
  } else {
    switch (brainInfo->operation) {
    case BRAIN_OPEN:
      /* Do brain startup. */
      if (brainOpen() == FALSE) {
        returnValue = BRAIN_ERROR;
      }
      break;
    case BRAIN_CLOSE:
      /* Do brain shutdown. */
      brainClose();
      break;
    case BRAIN_THINK:
      /* Do brain think call */
      if (brainThink(brainInfo) == FALSE) {
        returnValue = BRAIN_ERROR;
      }
      break;
    case BRAIN_MENU:
      /* Do brain menu action. */
      brainMenuHandler(brainInfo);
      break;
    default:
      /* This should never be reached so it must be an unexpected error.
         Shutdown and return an error. */
      MessageBox(NULL, "This branch should never be called.", "Error", MB_OK);
      returnValue = BRAIN_ERROR;
    }
  }

  if (returnValue != 0) {
    /* We must of encounted an error so shutdown */
    brainClose();
  }
  return returnValue;
}


/* Standard Win32 DLL Main entry point. It is useful to 
   save hInst for resource loading */
BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved) {
  brainInst = hInst;
	return TRUE;
}

