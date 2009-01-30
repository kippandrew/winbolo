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
*Name:          Dialog - Skins
*Filename:      dialogSkins.c
*Author:        John Morrison
*Creation Date: 28/09/03
*Last Modified: 28/09/03
*Purpose:
*  Looks after the skins dialog
*********************************************************/

#include <windows.h>
#include "..\dialogskins.h"
#include "..\lang.h"
#include "..\winutil.h"
#include "resource.h"
#include "..\skins.h"
#include "..\winbolo.h"
#include "..\gamefront.h"

#define DEFAULT_NO_SKIN "No Skin (Default)"
#define SLASH_STRING "\\"

char prevSkin[FILENAME_MAX]; /* Path and filename to the previous language */

/*********************************************************
*NAME:          dialogSkinsCallback
*AUTHOR:        John Morrison
*CREATION DATE: 28/09/03
*LAST MODIFIED: 28/09/03
*PURPOSE:
*  The Skins Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogSkinsCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND hList;

  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogSkinsInit(hWnd);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDC_SKINLIST:
      if (HIWORD(wParam) == LBN_SELCHANGE) {
        char skinName[FILENAME_MAX];
        int nItem, itemNum;
        hList = GetDlgItem(hWnd, IDC_SKINLIST); 
        nItem = (int) SendMessage(hList, LB_GETCURSEL, 0, 0); 
        itemNum = (int) SendMessage(hList, LB_GETITEMDATA, nItem, 0); 
        SendMessage(hList, LB_GETTEXT, nItem, (LPARAM) (LPCTSTR) skinName); 
        if (strcmp(skinName, DEFAULT_NO_SKIN) == 0) {
          skinsLoadSkin("");
          dialogSkinsSetText(hWnd);
        } else {
          if (skinsLoadSkin(skinName) == TRUE) {
            /* Change it */
            dialogSkinsSetText(hWnd);
          } else {
            MessageBoxA(NULL, "Unable to load Skin File", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
          }
        }
      }
      break;

    case IDCANCEL:
      skinsLoadSkin(prevSkin);
      EndDialog(hWnd, TRUE);
      break;
    case IDOK: 
      gameFrontReloadSkins();
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
*NAME:          dialogSkinsSetInit
*AUTHOR:        John Morrison
*CREATION DATE: 28/09/03
*LAST MODIFIED: 28/09/03
*PURPOSE:
*  The Skins Dialog init function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogSkinsInit(HWND hWnd) {
  gameFrontSetDlgState(openWelcome);
  dialogSkinsSetText(hWnd);
  dialogSkinsLoadList(hWnd);
  skinsGetFileName(prevSkin);
}

/*********************************************************
*NAME:          dialogSkinsLoadList
*AUTHOR:        John Morrison
*CREATION DATE: 28/09/03
*LAST MODIFIED: 28/09/03
*PURPOSE:
*  Loads the list of available skins into the 
*  dialog box
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogSkinsLoadList(HWND hWnd) {
  BOOL ret;                  /* Function return */
  WIN32_FIND_DATA fd;        /* Find data */
  HANDLE findHandle;         /* The find handle */
  char checkPath[MAX_PATH];  /* Path to check */
  char select[FILENAME_MAX]; /* Item to select */
  int count;                 /* Items position */
  HWND hList;                /* The list box handle */  
  char langPath[FILENAME_MAX]; /* Path to the language files */
  char testExtension[128];     /* Used to test the file extension */

  hList = GetDlgItem(hWnd, IDC_SKINLIST);
  count = 1;
  SendMessage(hList, LB_ADDSTRING, 0, (LPARAM) DEFAULT_NO_SKIN); 
  SendMessage(hList, LB_SETITEMDATA, count-1, (LPARAM) count-1); 
  count++;
  strcpy(checkPath, SKIN_DIR_STRING);
  if (winUtilWBSubDirExist(checkPath) == TRUE) {
    /* Okay Directory exists */
    strcpy(langPath, checkPath);
    strcat(checkPath, SLASH_STRING);
    strcat(checkPath, "*.*");
  
    findHandle = FindFirstFile(checkPath, &fd);
    if (findHandle != INVALID_HANDLE_VALUE) {
      /* Atleast One file exist */
      if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
        /* Not a directory */
        strcpy(testExtension, fd.cFileName + strlen(fd.cFileName)-4);
        if (strcmp(testExtension, ".zip") == 0 || strcmp(testExtension, ".wsf") == 0) {
          fd.cFileName[strlen(fd.cFileName)-4] = '\0';
          SendMessage(hList, LB_ADDSTRING, 0, (LPARAM) fd.cFileName); 
          SendMessage(hList, LB_SETITEMDATA, count-1, (LPARAM) count-1); 
          count++;
        }
      }
      ret = FindNextFile(findHandle, &fd);
      while (ret != 0) {
        /* Process remaining */
        if (fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
          /* Not a directory */
        strcpy(testExtension, fd.cFileName + strlen(fd.cFileName)-4);
          if (strcmp(testExtension, ".zip") == 0 || strcmp(testExtension, ".wsf") == 0) {
            fd.cFileName[strlen(fd.cFileName)-4] = '\0';
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM) fd.cFileName); 
            SendMessage(hList, LB_SETITEMDATA, count-1, (LPARAM) count-1); 
            count++;
          }
        }
        ret = FindNextFile(findHandle, &fd);
      }
    }
    FindClose(findHandle);
  }

  skinsGetFileName(select);

  if (strcmp(select, "") == 0) {
    strcpy(select, DEFAULT_NO_SKIN);
  }
  SendMessage(hList, LB_SELECTSTRING, -1, (LPARAM) select);
}

/*********************************************************
*NAME:          dialogSkinsSetText
*AUTHOR:        John Morrison
*CREATION DATE: 28/09/03
*LAST MODIFIED: 28/09/03
*PURPOSE:
*  Sets the text in the dialog box
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogSkinsSetText(HWND hWnd) {
  char buff[FILENAME_MAX]; /* Used to save the hold the information loaded from the file */
  char skinsFile[MAX_PATH]; /* Used to load the skins ini file */
  
  /* Set dialog language text */
  SetDlgItemTextA(hWnd, IDOK, langGetText(STR_OK));  
  SetDlgItemTextA(hWnd, IDCANCEL, langGetText(STR_CANCEL));
  SetDlgItemTextA(hWnd, IDC_SKINNAMES, langGetText(STR_DLGLANG_NAME_CAPTION));
  SetDlgItemTextA(hWnd, IDC_SKINNOTESS, langGetText(STR_DLGLANG_NOTES_CAPTION));
  SetDlgItemTextA(hWnd, IDC_SKINAUTHORS, langGetText(STR_DLGLANG_AUTHOR_CAPTION));
  
  if (skinsIsLoaded() == TRUE) {
    skinsGetSkinDirectory(skinsFile);
    strcat(skinsFile, "skin.ini");
    GetPrivateProfileString("Skin", "Name", "N/A", buff, FILENAME_MAX, skinsFile);
    SetDlgItemTextA(hWnd, IDC_SKINNAME, buff);
    GetPrivateProfileString("Skin", "Author", "N/A", buff, FILENAME_MAX, skinsFile);
    SetDlgItemTextA(hWnd, IDC_SKINAUTHOR, buff);
    GetPrivateProfileString("Skin", "Notes", "N/A", buff, FILENAME_MAX, skinsFile);
    SetDlgItemTextA(hWnd, IDC_SKINNOTES, buff);
  } else {
    SetDlgItemTextA(hWnd, IDC_SKINNAME, langGetText(STR_DLGLANG_NAME));
    SetDlgItemTextA(hWnd, IDC_SKINAUTHOR, langGetText(STR_DLGLANG_AUTHOR));
    SetDlgItemTextA(hWnd, IDC_SKINNOTES, langGetText(STR_DLGLANG_NOTES));
  }
  SetDlgItemTextA(hWnd, IDC_SKINDEFAULT, langGetText(STR_DLGSKIN_DEFAULTNOTE));
  SetWindowTextA(hWnd, langGetText(STR_DLGSKIN_TITLE));
}
