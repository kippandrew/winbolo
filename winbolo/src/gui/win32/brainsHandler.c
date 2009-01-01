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
*Name:          brainsHandler
*Filename:      brainsHandler.c
*Author:        John Morrison
*Creation Date: 23/11/99
*Last Modified: 29/4/00
*Purpose:
*  Handles front end brain operations. Menu setting etc.
*
*NOTE: This file must be compiled with optimisations 
*      disabled under Visual Studio (VC++) 5 with any
*      or none of the service packs installed due to a
*      a compiler bug causing crashes whenever a brain
*      is loaded/unloaded
*********************************************************/

#include <string.h>
#include <windows.h>
#include <winbase.h>
#include "..\..\bolo\global.h"
#include "..\..\bolo\brain.h"
#include "..\..\bolo\backend.h"
#include "..\resource.h"
#include "..\winbolo.h"
#include "..\brainsHandler.h"
#include "..\winutil.h"
#include "..\clientmutex.h"
#include "..\lang.h"

#define BRAINMAIN_PROC "BrainMain"

char brainsDir[MAX_PATH]; /* The Brains Directory */
int brainsNum; /* Number of brains */
bool brainsRunning; /* Is a brain running */
UINT brainsRunningId; /* The ID Number of the brain */
HINSTANCE brainsInst; /* Brains Instance */
FARPROC brainsProc;   /* The brains Proceedure */
bool bIsFirst;        /* Is this the first time a brain will be called */
bool brainsProcExecuting = FALSE; /* Is the brain proc in execution */
BrainInfo bInfo; /* The Brain Info */
bool brainsLoadedOnce = FALSE; /* We only want this to be displayed once */

/*********************************************************
*NAME:          brainsHandlerLoadBrainMenuItems
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 24/11/99
*PURPOSE:
*  Loads the brain file name into the menu.
*  Returns success
*
*ARGUMENTS:
*  brainsList - The list of brains to add on to.
*********************************************************/
bool brainsHandlerLoadBrainMenuItems(HMENU brainsList) {
  bool returnValue;         /* Value to return */
  BOOL ret;                 /* Function return */
  WIN32_FIND_DATA fd;       /* Find data */
  HANDLE findHandle;        /* The find handle */
  char checkPath[MAX_PATH]; /* Path to check */

  returnValue = TRUE;
  strcpy(checkPath, brainsDir);
  strcat(checkPath, SLASH_STRING);
  strcat(checkPath, BRAINS_FILTER_STRING);
  
  findHandle = FindFirstFile(checkPath, &fd);
  if (findHandle != INVALID_HANDLE_VALUE) {
    /* Atleast One file exist */
    if (fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
      /* Not a directory */
      fd.cFileName[strlen(fd.cFileName)-4] = '\0';
      AppendMenuA(brainsList, MF_STRING, (unsigned int) (brainsNum+BRAINS_RESOURCE_OFFSET), fd.cFileName);
      brainsNum++;
    }
    ret = FindNextFile(findHandle, &fd);
    while (ret != 0) {
      /* Process remaining */
      if (fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
        /* Not a directory */
        fd.cFileName[strlen(fd.cFileName)-4] = '\0';
        AppendMenuA(brainsList, MF_STRING, (unsigned int) (brainsNum+BRAINS_RESOURCE_OFFSET), fd.cFileName);
        brainsNum++;
      }
      ret = FindNextFile(findHandle, &fd);
    }
  }
  FindClose(findHandle);
  return returnValue;
}

/*********************************************************
*NAME:          brainsHandlerLoadBrains
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 24/12/99
*PURPOSE:
*  Loads the brain entries into the brains menu.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
*  hWnd  - Main Window Handle.
*********************************************************/
bool brainsHandlerLoadBrains(HWND hWnd) {
  bool returnValue; /* Value to return */
  HMENU mainMenu;   /* The main menu */
  HMENU brainsMenu;

  /* Set up the constants in the bInfo structure */
  bInfo.BoloVersion = 0x0114;
  bInfo.InfoVersion = CURRENT_BRAININFO_VERSION;
  bInfo.PrefsVRefNum = 0;
	bInfo.PrefsFileName = NULL;

  brainsMenu = NULL;
  strcpy(brainsDir, BRAINS_DIR_STRING);
  brainsNum = 0;
  brainsInst = NULL;
  brainsRunning = FALSE;
  
  returnValue = TRUE;
  mainMenu = GetMenu(hWnd);

  if (mainMenu == NULL) {
    returnValue = FALSE;
  } else {
    brainsMenu = GetSubMenu(mainMenu, (GetMenuItemCount(mainMenu)-2));
  }

  /* Look for the 'Brains' directory */
  if (returnValue == TRUE) {
    returnValue = winUtilWBSubDirExist(brainsDir);
    if (returnValue == FALSE && brainsLoadedOnce == FALSE) {
      MessageBoxA(NULL, langGetText(STR_BRAINERR_BRAINDIR), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      brainsLoadedOnce = TRUE;
    }
  }
  /* Load the brains */
  if (returnValue == TRUE && brainsMenu) {
    returnValue = brainsHandlerLoadBrainMenuItems(brainsMenu);
  }

  return returnValue;
}


/*********************************************************
*NAME:          brainsHandlerSet
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED:  3/12/99
*PURPOSE:
*  Sets each item in the menu to be disabled.
*
*ARGUMENTS:
*  hWnd    - Handle to the main window
*  enabled - TRUE if we should enable the items else
*            disable them
*********************************************************/
void brainsHandlerSet(HWND hWnd, bool enabled) {
  HMENU mainMenu;     /* Main Menu */
  HMENU brainsMenu;   /* Brains Menu */
  unsigned int count; /* Looping variable */

  
  mainMenu = GetMenu(hWnd);
  if (mainMenu != NULL) {
    brainsMenu = GetSubMenu(mainMenu, (GetMenuItemCount(mainMenu)-2));
    if (brainsMenu != NULL) {
      /* Disable all entries */
      for (count=BRAINS_RESOURCE_OFFSET;count <= (unsigned int) (brainsNum+BRAINS_RESOURCE_OFFSET);count++) {
        if (enabled) {
          EnableMenuItem(brainsMenu, count, MF_ENABLED);
        } else {
          EnableMenuItem(brainsMenu, count, MF_GRAYED);
        }
      }
      DrawMenuBar(hWnd);
    }
  }
}

/*********************************************************
*NAME:          brainsHandlerManual
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 12/12/99
*PURPOSE:
*  The manual brain button has been pressed
*
*ARGUMENTS:
*  hWnd - Window Handle
*********************************************************/
void brainsHandlerManual(HWND hWnd) {
  HMENU hMenu; /* Main menu handle */

  if (brainsRunning == TRUE) {
    /* Shut that brain down! */
    while(brainsProcExecuting) {} /* Loop until it isn't running */
    /* Prepare brain */
    clientMutexWaitFor();
    brainsProcExecuting = TRUE;
    brainsRunning = FALSE;
    bInfo.operation = BRAIN_CLOSE;
    brainsProc(&bInfo);
    brainsProcExecuting = FALSE;
    FreeLibrary(brainsInst);
    brainsInst = NULL;
    clientMutexRelease();
  }

  hMenu = GetMenu(hWnd);
  if (hMenu != NULL) {
    CheckMenuItem(hMenu, brainsRunningId, MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_BRAINSMENU_MANUAL, MF_CHECKED);
  }
}

/*********************************************************
*NAME:          brainsHandlerStart
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  Try to launch a new brain. Returns success.
*
*ARGUMENTS:
*  hWnd - Window Handle
*  str  - Filename and path of brain to load
*  name - Name of the brain
*********************************************************/
bool brainsHandlerStart(HWND hWnd, char *str, char *name) {
  bool returnValue; /* Value to return */
  short ret;
  clientMutexWaitFor();
  returnValue = TRUE;
  brainsInst = LoadLibrary(str);
  if (brainsInst == NULL) {
    MessageBoxA(hWnd, langGetText(STR_BRAINERR_LAUNCH), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    returnValue = FALSE;
  }

  if (returnValue == TRUE) {
    brainsProc = GetProcAddress(brainsInst, BRAINMAIN_PROC);
    if (brainsProc == NULL) {
      MessageBoxA(hWnd, langGetText(STR_BRAINERR_LAUNCHMAIN), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }
  /* Try to send the initial launch message */
  if (returnValue == TRUE) {
    clientMutexWaitFor();
    screenMakeBrainInfo(&bInfo, TRUE);
    bInfo.userdata = brainsInst;
    while(brainsProcExecuting) {} /* Loop until it isn't running */
    brainsProcExecuting = TRUE;
    bInfo.operation = BRAIN_OPEN;
    ret = brainsProc(&bInfo);
    screenExtractBrainInfo(&bInfo);
    clientMutexRelease();
    brainsProcExecuting = FALSE;

    if (ret != 0) {
      /* Shut it down */
      FreeLibrary(brainsInst);
      brainsInst = NULL;
      returnValue = FALSE;
      MessageBoxA(hWnd, langGetText(STR_BRAINERR_INIT), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    }
  }
  clientMutexRelease();
  return returnValue;
}


/*********************************************************
*NAME:          brainsHandlerItem
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  The brain selection has been made.
*
*ARGUMENTS:
*  hWnd - Window Handle
*  id   - Window parameter offset pressed.
*********************************************************/
void brainsHandlerItem(HWND hWnd, UINT id) {
  HMENU hMenu;         /* Windows menu handle      */
  char str[MAX_PATH];  /* Path to the brain to run */
  char name[MAX_PATH]; /* Menu Items Name          */
  int ret;             /* Function return value    */

  hMenu = GetMenu(hWnd);
  if (id != brainsRunningId || brainsRunning == FALSE) {
    /* First shut down the last brain if it is running */
    if (brainsRunning == TRUE) {
      brainsHandlerManual(hWnd);
    }
  
    /* Get the brains name */
    strcpy(str, brainsDir);
    strcat(str, SLASH_STRING);
    ret = GetMenuString(hMenu, id, name, MAX_PATH, MF_BYCOMMAND);
    if (ret != 0) {
      strcat(str, name);
      strcat(str, BRAINS_EXTENSION);
    } else {
      str[0] = '\0';
    }

    /* Try starting the new brain */
    brainsRunning = brainsHandlerStart(hWnd, str, name);
  
    if (brainsRunning == TRUE) {
      bIsFirst = TRUE;
      brainsRunningId = id;
      CheckMenuItem(hMenu, ID_BRAINSMENU_MANUAL, MF_UNCHECKED);
      CheckMenuItem(hMenu, brainsRunningId, MF_CHECKED);
    }
  }
}

/*********************************************************
*NAME:          brainsHandlerBrainsItem
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  A brains menu item has been selected.
*
*ARGUMENTS:
*  hWnd - Window Handle
*  id   - Window parameter offset pressed.
*********************************************************/
void brainsHandlerBrainsItem(HWND hWnd, UINT id) {
  short ret; /* Brain Return value */
  HMENU hMenu; /* Main Window Menu */
  
  if (id > 2000 && brainsRunning == TRUE) {
    
    bInfo.operation = 200;
    bInfo.menu_item = id;
    ret = brainsProc(&bInfo);
    /* Call the brain with the option */
    if (ret != 0) {
      FreeLibrary(brainsInst);
      brainsInst = NULL;
      brainsRunning = FALSE;
      hMenu = GetMenu(hWnd);
      if (hMenu != NULL) {
        CheckMenuItem(hMenu, brainsRunningId, MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_BRAINSMENU_MANUAL, MF_CHECKED);
      }

      MessageBoxA(hWnd, langGetText(STR_BRAINERR_EXEC), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    }
  }
}

/*********************************************************
*NAME:          brainsHandlerGetNum
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  Returns the number of brains loaded.
*
*ARGUMENTS:
*
*********************************************************/
int brainsHandlerGetNum() {
  return brainsNum;
}

/*********************************************************
*NAME:          brainsHandlerShutdown
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 24/11/99
*PURPOSE:
*  Shuts down the brains subsystem.
*
*ARGUMENTS:
*  hWnd - Pointer to the main window
*********************************************************/
void brainsHandlerShutdown(HWND hWnd) {
  if (brainsRunning == TRUE) {
    /* Shut it down */
    brainsHandlerManual(hWnd);
  }
}

/*********************************************************
*NAME:          brainHandlerIsBrainRunning
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 25/11/99
*PURPOSE:
*  Returns whether a brain is running or not
*
*ARGUMENTS:
*
*********************************************************/
bool brainHandlerIsBrainRunning() {
  return brainsRunning;
}

/*********************************************************
*NAME:          brainHandlerRun
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Called to execute the brains "THINK" call if it is
*  running
*
*ARGUMENTS:
*  hWnd - Main Window handler
*********************************************************/
void brainHandlerRun(HWND hWnd) {
  unsigned short ret; /* Brain return value */
  clientMutexWaitFor();
  if (brainsRunning == TRUE && brainsProcExecuting == FALSE) {
    /* Prepare brain */
//    clientMutexWaitFor();

    brainsProcExecuting = TRUE;
    if (bIsFirst == TRUE) {
      screenMakeBrainInfo(&bInfo, TRUE);
      bIsFirst = FALSE;
    } else {
      screenMakeBrainInfo(&bInfo, FALSE);
    }
//    clientMutexRelease();
    /* Call brain */

    bInfo.operation = BRAIN_THINK;
    if (brainsRunning == TRUE) {
      ret = brainsProc(&bInfo);
    } else {
      ret = -1;
    }
    if (ret != 0) {
      /* Operation failed */
      brainsHandlerManual(hWnd);
    } else {
      /* Operation succeeded */
  //    clientMutexWaitFor();
      screenExtractBrainInfo(&bInfo);
//      clientMutexRelease();
    }
    brainsProcExecuting = FALSE;
  } else if (brainsRunning == TRUE) {
  }
  clientMutexRelease();
}

