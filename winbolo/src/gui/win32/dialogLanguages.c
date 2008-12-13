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
*Name:          Dialog - Languages
*Filename:      dialogLanguages.c
*Author:        John Morrison
*Creation Date: 29/4/00
*Last Modified: 13/6/00
*Purpose:
*  Looks after the Languages dialog
*********************************************************/

#include <windows.h>
#include "..\dialogLanguages.h"
#include "..\lang.h"
#include "..\winutil.h"
#include "..\winbolo.h"
#include "..\gamefront.h"
#include "..\resource.h"

#define _UNICODE

#define DEFAULT_ENGLISH "English (Default)"

/* Hack to reuse code. We are only interested in the server member of them. 
The rest can be ignored. Stupid list boxes you can't get the number from */
char langPath[FILENAME_MAX]; /* Path to the language files */
char prevLang[FILENAME_MAX]; /* Path and filename to the previous language */

void windowReCreate();

/*********************************************************
*NAME:          dialogLanguagesCallback
*AUTHOR:        John Morrison
*CREATION DATE: 29/4/00
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  The Languages Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogLanguagesCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND hList;

  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogLanguagesInit(hWnd);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDC_LIST1:
      if (HIWORD(wParam) == LBN_SELCHANGE) {
        char loadName[FILENAME_MAX];
        char langName[FILENAME_MAX];
        int nItem, itemNum;
        hList = GetDlgItem(hWnd, IDC_LIST1); 
        nItem = SendMessage(hList, LB_GETCURSEL, 0, 0); 
        itemNum = SendMessage(hList, LB_GETITEMDATA, nItem, 0); 
        SendMessage(hList, LB_GETTEXT, nItem, (LPARAM) (LPCTSTR) langName); 
        if (strcmp(langName, DEFAULT_ENGLISH) == 0) {
          langLoadFile(NULL, "");
          dialogLanguagesSetText(hWnd);
        } else {
          strcat(langName, LANG_EXTENSION);
          strcpy(loadName, langPath);
          strcat(loadName, SLASH_STRING);
          strcat(loadName, langName);
          if (langLoadFile(loadName, langName) == TRUE) {
            /* Change it */
            dialogLanguagesSetText(hWnd);
          } else {
            MessageBox(NULL, "Unable to load Language File", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
          }
        }
      }
      break;

    case IDCANCEL:
      strcat(langPath, SLASH_STRING);
      strcat(langPath, prevLang);
      langLoadFile(langPath, prevLang);
      EndDialog(hWnd, TRUE);
      break;
    case IDOK: 
      windowReCreate();
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
*NAME:          dialogLanguagesSetInit
*AUTHOR:        John Morrison
*CREATION DATE: 29/4/00
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  The Languages Dialog init function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogLanguagesInit(HWND hWnd) {
  gameFrontSetDlgState(openWelcome);
  dialogLanguagesSetText(hWnd);
  dialogLanguagesLoadList(hWnd);
  langGetFileName(prevLang);
}

/*********************************************************
*NAME:          dialogLanguagesLoadList
*AUTHOR:        John Morrison
*CREATION DATE: 29/4/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  Loads the list of available languages into the 
*  dialog box
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogLanguagesLoadList(HWND hWnd) {
  BOOL ret;                  /* Function return */
  WIN32_FIND_DATA fd;        /* Find data */
  HANDLE findHandle;         /* The find handle */
  char checkPath[MAX_PATH];  /* Path to check */
  char select[FILENAME_MAX]; /* Item to select */
  int count;                 /* Items position */
  HWND hList;                /* The list box handle */  

  hList = GetDlgItem(hWnd, IDC_LIST1);
  count = 1;
  SendMessage(hList, LB_ADDSTRING, 0, (LPARAM) DEFAULT_ENGLISH); 
  SendMessage(hList, LB_SETITEMDATA, count-1, (LPARAM) count-1); 
  count++;
  strcpy(checkPath, LANG_DIR_STRING);
  if (winUtilWBSubDirExist(checkPath) == TRUE) {
    /* Okay Directory exists */
    strcpy(langPath, checkPath);
    strcat(checkPath, SLASH_STRING);
    strcat(checkPath, LANG_FILTER_STRING);
  
    findHandle = FindFirstFile(checkPath, &fd);
    if (findHandle != INVALID_HANDLE_VALUE) {
      /* Atleast One file exist */
      if (fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
        /* Not a directory */
        fd.cFileName[strlen(fd.cFileName)-4] = '\0';
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM) fd.cFileName); 
        SendMessage(hList, LB_SETITEMDATA, count-1, (LPARAM) count-1); 
        count++;
      }
      ret = FindNextFile(findHandle, &fd);
      while (ret != 0) {
        /* Process remaining */
        if (fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
          /* Not a directory */
          fd.cFileName[strlen(fd.cFileName)-4] = '\0';
          SendMessage(hList, LB_ADDSTRING, 0, (LPARAM) fd.cFileName); 
          SendMessage(hList, LB_SETITEMDATA, count-1, (LPARAM) count-1); 
          count++;
        }
        ret = FindNextFile(findHandle, &fd);
      }
    }
    FindClose(findHandle);
  }

  langGetFileName(select);

  if (strcmp(select, "") == 0) {
    strcpy(select, DEFAULT_ENGLISH);
  } else {
    select[strlen(select)-4] = '\0';
  }
  SendMessage(hList, LB_SELECTSTRING, -1, (LPARAM) select);
}

/*********************************************************
*NAME:          dialogLanguagesSetText
*AUTHOR:        John Morrison
*CREATION DATE: 29/4/00
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Sets the text in the dialog box
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogLanguagesSetText(HWND hWnd) {
  /* Set dialog language text */
  SetDlgItemText(hWnd, IDOK, langGetText(STR_OK));  
  SetDlgItemText(hWnd, IDCANCEL, langGetText(STR_CANCEL));
  SetDlgItemText(hWnd, IDC_LANGNAMES, langGetText(STR_DLGLANG_NAME_CAPTION));
  SetDlgItemText(hWnd, IDC_LANGNAME, langGetText(STR_DLGLANG_NAME));
  SetDlgItemText(hWnd, IDC_AUTHORS, langGetText(STR_DLGLANG_AUTHOR_CAPTION));
  SetDlgItemText(hWnd, IDC_AUTHOR, langGetText(STR_DLGLANG_AUTHOR));
  SetDlgItemText(hWnd, IDC_NOTESS, langGetText(STR_DLGLANG_NOTES_CAPTION));
  SetDlgItemText(hWnd, IDC_NOTES, langGetText(STR_DLGLANG_NOTES));
  SetDlgItemText(hWnd, IDC_DEFAULT, langGetText(STR_DLGLANG_DEFAULTNOTE));
  SetWindowText(hWnd, langGetText(STR_DLGLANG_TITLE));
}
