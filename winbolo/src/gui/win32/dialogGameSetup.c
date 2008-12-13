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
*Name:          Dialog - Game Setup
*Filename:      dialoggamesetup.c
*Author:        John Morrison
*Creation Date: 27/1/99
*Last Modified:  2/7/00
*Purpose:
*  Looks after the Game setup dialog
*********************************************************/

#include <windows.h>
#include <commdlg.h>
#include "..\..\bolo\global.h"
#include "..\winbolo.h"
#include "..\gamefront.h"
#include "..\resource.h"
#include "..\..\bolo\backend.h"
#include "..\lang.h"
#include "..\resource.h"
#include "..\dialoggamesetup.h"
#include "..\mappreview.h"

/*********************************************************
*NAME:          dialogGameSetupCallback
*AUTHOR:        John Morrison
*CREATION DATE: 27/1/99
*LAST MODIFIED: 27/1/99
*PURPOSE:
*  The Opening Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogGameSetupCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND dlgWnd; /* Dialog Item Window Handle */

  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogGameSetupInit(hWnd);
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case IDC_OK:
      dialogGameSetupDone(hWnd);
      gameFrontSetDlgState(openFinished);
      mapPreviewDestroy();
      EndDialog(hWnd,TRUE);
      break;
    case IDC_ALLOWCOMP1:
      dlgWnd = GetDlgItem(hWnd, IDC_ALLOWCOMP2);
      if (IsDlgButtonChecked(hWnd, IDC_ALLOWCOMP1) == BST_UNCHECKED) {
        /* Unselect the other button */
        EnableWindow(dlgWnd, FALSE);
      } else {
        EnableWindow(dlgWnd, TRUE);
      }
      break;
    case IDC_GAMESTART:
      dlgWnd = GetDlgItem(hWnd, IDC_EDITSTART);
      if (IsDlgButtonChecked(hWnd, IDC_GAMESTART) == BST_UNCHECKED) {
        EnableWindow(dlgWnd, FALSE);
      } else {
        EnableWindow(dlgWnd, TRUE);
      }
      break;
    case IDC_CHECKTIME_LIMIT:
      dlgWnd = GetDlgItem(hWnd, IDC_EDITTIMELIMIT);
      if (IsDlgButtonChecked(hWnd, IDC_CHECKTIME_LIMIT) == BST_UNCHECKED) {
        EnableWindow(dlgWnd, FALSE);
      } else {
        EnableWindow(dlgWnd, TRUE);
      }
      break;
    case IDC_CHECKPASS:
      dlgWnd = GetDlgItem(hWnd, IDC_EDITPASS);
      if (IsDlgButtonChecked(hWnd, IDC_CHECKPASS) == BST_UNCHECKED) {
        EnableWindow(dlgWnd, FALSE);
      } else {
        EnableWindow(dlgWnd, TRUE);
      }
      break;
    case IDC_CHOOSE_MAP:
      dialogGameSetupChooseMap(hWnd);
      break;
    case IDCANCEL:
      /* Cancel */
      gameFrontSetDlgState(openWelcome);
      mapPreviewDestroy();
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


/*********************************************************
*NAME:          dialogGameFinderDlgNotify
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  The open dialog file chooser notify event
*
*ARGUMENTS:
*  hDlg - Handle to the dialog
*  pofn - Notify message
*********************************************************/
BOOL NEAR PASCAL dialogGameFinderDlgNotify(HWND hDlg, LPOFNOTIFY pofn) {
	char szFile[MAX_PATH];
  HWND hWnd;
  HDC hDC;
	switch (pofn->hdr.code) {
		/* The selection has changed.  */
		case CDN_SELCHANGE:
			if (CommDlg_OpenSave_GetFilePath(GetParent(hDlg), szFile, sizeof(szFile)) <= sizeof(szFile)) {
        hWnd = GetDlgItem(hDlg, IDC_MAP);
        if (hWnd != NULL) {
          hDC = GetDC(hWnd);
          if (hDC != NULL) {
            mapPreviewDraw(szFile, hDC);
            ReleaseDC(hWnd, hDC);
          }
        }
			}
		break;
	}

	return TRUE;
}

/*********************************************************
*NAME:          dialogGameFinderOpeningProc
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  The open dialog file chooser notify event
*
*ARGUMENTS:
*  hDlg   - Handle to the window
*  uMsg   - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogGameFinderOpeningProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	char szFile[MAX_PATH];
  HWND hWnd;
  HDC hDC;
 
	switch (uMsg)	{
    case WM_PAINT:
			if (CommDlg_OpenSave_GetFilePath(GetParent(hDlg), szFile, sizeof(szFile)) <= sizeof(szFile)) {
        hWnd = GetDlgItem(hDlg, IDC_MAP);
        if (hWnd != NULL) {
          hDC = GetDC(hWnd);
          if (hDC != NULL) {
            mapPreviewDraw(szFile, hDC);
            ReleaseDC(hWnd, hDC);
          }
        }
			} 
      return FALSE;
		case WM_NOTIFY:
			dialogGameFinderDlgNotify(hDlg, (LPOFNOTIFY)lParam);
      break;
		default:
			return FALSE;
	}
	return TRUE;
}



/*********************************************************
*NAME:          dialogGameSetupChooseMap
*AUTHOR:        John Morrison
*CREATION DATE: 28/1/99
*LAST MODIFIED: 28/1/99
*PURPOSE:
*  The Choose Map button has been pushed. Operate the
*  dialog box.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameSetupChooseMap(HWND hWnd) {
  OPENFILENAME ofn;   /* File name to be opened */
  char fileName[FILENAME_MAX]; /* The filename and path that should be opened */
  bool openOK;         /* Did the dialog/file open OK */

  fileName[0] = '\0';
  /* Fill the open file dialog box with stuff */
  memset( &ofn, 0, sizeof(OPENFILENAME));
  ofn.lStructSize	= sizeof(OPENFILENAME);
  ofn.hwndOwner	= hWnd;
  ofn.hInstance	= windowGetInstance();
  ofn.lpstrFilter	= OPEN_FILE_FILTERS;
  ofn.lpstrCustomFilter = NULL;
  ofn.nFilterIndex	= 0;
  ofn.nMaxCustFilter = 0;

  ofn.lpstrFile	= fileName;
  ofn.nMaxFile	= sizeof(fileName);
	ofn.lpstrFileTitle	= NULL;
	ofn.nMaxFileTitle	= 0;
 	ofn.lpstrInitialDir = NULL;
  ofn.lpstrTitle	= NULL;
	ofn.lpfnHook = dialogGameFinderOpeningProc;
	ofn.lpTemplateName = MAKEINTRESOURCE(IDD_COMDLG32);
  ofn.Flags   = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_SHOWHELP | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;

  
  openOK = GetOpenFileName( &ofn ); /* Do the dialog box */
  if (openOK == TRUE) {
    dialogGameSetupMapCheck(hWnd, fileName);
  }

}


/*********************************************************
*NAME:          dialogGameSetupMapCheck
*AUTHOR:        John Morrison
*CREATION DATE: 28/1/99
*LAST MODIFIED: 12/12/99
*PURPOSE:
* Checks to see if the map is valid set the Map name 
* to be it and enable the dialog OK button - 
* else disable the OK button
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  fileName - filename to check
*********************************************************/
void dialogGameSetupMapCheck(HWND hWnd, char *fileName) {
  char mapName[FILENAME_MAX]; /* Name of the map */
  char output[FILENAME_MAX];  /* String to output */
  bool openOK;                /* Was the map opened OK? */
  
  openOK = TRUE;
  mapName[0] = '\0';
  output[0] = '\0';

  openOK = screenLoadMap(fileName, gameOpen, FALSE, 0, UNLIMITED_GAME_TIME, langGetText(STR_DLGGAMESETUP_DEFAULTNAME), TRUE);
  if (openOK == FALSE) {
    MessageBox(NULL, langGetText(STR_DLGGAMESETUP_ERROROPENINGMAP), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
    SendDlgItemMessage(hWnd, IDC_SELECTEDMAP, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) langGetText(STR_DLGGAMESETUP_SELECTEDMAPINBUILT));
    gameFrontSetFileName("");
  } else {
    screenGetMapName(mapName);
    strcpy(output, langGetText(STR_DLGGAMESETUP_SELECTEDMAP));
    strcat(output, mapName);
    gameFrontSetFileName(fileName);
    SendDlgItemMessage(hWnd, IDC_SELECTEDMAP, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (output));
  }
}

/*********************************************************
*NAME:          dialogGameSetupInit
*AUTHOR:        John Morrison
*CREATION DATE: 28/1/99
*LAST MODIFIED: 28/4/00
*PURPOSE:
* Called on dialog startup to handle command line
* arguments 
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameSetupInit(HWND hWnd) {
  HWND dlgWnd;                 /* Dialog Control Window Handle */
  char fileName[FILENAME_MAX]; /* Name of the map              */
  char password[FILENAME_MAX]; /* Dialog Options               */
  char *text;                  /* Used to hold temp lang text  */
  gameType gt;                 
  bool hm;
  aiType ai;
  long sd;
  long tlimit;


  mapPreviewSetup();

  text = langGetText(STR_DLGGAMESETUP_TITLE);
  SetWindowText(hWnd, text);
  text = langGetText(STR_DLGGAMESETUP_BLURB);
  SetDlgItemText(hWnd, IDC_BLURB, text);
  SetDlgItemText(hWnd, IDC_SELECTEDMAP, langGetText(STR_DLGGAMESETUP_SELECTEDMAPINBUILT));
  text = langGetText(STR_DLGGAMESETUP_CHOOSEMAP);
  SetDlgItemText(hWnd, IDC_CHOOSE_MAP, text);
  text = langGetText(STR_DLGGAMESETUP_RADIO1);
  SetDlgItemText(hWnd, IDC_RADIO1, text);
  text = langGetText(STR_DLGGAMESETUP_RADIO2);
  SetDlgItemText(hWnd, IDC_RADIO2, text);
  text = langGetText(STR_DLGGAMESETUP_RADIO3);
  SetDlgItemText(hWnd, IDC_RADIO3, text);
  text = langGetText(STR_DLGGAMESETUP_HIDDENMINES);
  SetDlgItemText(hWnd, IDC_HIDDEN_MINES, text);
  text = langGetText(STR_DLGGAMESETUP_ALLOWCOMPTANKS);
  SetDlgItemText(hWnd, IDC_ALLOWCOMP1, text);
  text = langGetText(STR_DLGGAMESETUP_ALLOWCOMPTANKSADV);
  SetDlgItemText(hWnd, IDC_ALLOWCOMP2, text);

  text = langGetText(STR_DLGGAMESETUP_PASSWORD);
  SetDlgItemText(hWnd, IDC_CHECKPASS, text);
  text = langGetText(STR_DLGGAMESETUP_STARTDELAY);
  SetDlgItemText(hWnd, IDC_GAMESTART, text);
  text = langGetText(STR_DLGGAMESETUP_TIMELIMIT);
  SetDlgItemText(hWnd, IDC_CHECKTIME_LIMIT, text);
  text = langGetText(STR_DLGGAMESETUP_SECONDS);
  SetDlgItemText(hWnd, IDC_SECONDS, text);
  text = langGetText(STR_DLGGAMESETUP_MINUTES);
  SetDlgItemText(hWnd, IDC_MINUTES, text);
  text = langGetText(STR_OK);
  SetDlgItemText(hWnd, IDC_OK, text);
  text = langGetText(STR_CANCEL);
  SetDlgItemText(hWnd, IDCANCEL, text);


  gameFrontGetGameOptions(password, &gt, &hm, &ai, &sd, &tlimit);
  /* Check Buttons */
  dlgWnd = GetDlgItem(hWnd, IDC_EDITPASS);
  if (password[0] != '\0') {
    CheckDlgButton(hWnd, IDC_CHECKPASS, BST_CHECKED);
    SetDlgItemText(hWnd, IDC_EDITPASS, password);
    EnableWindow(dlgWnd, TRUE);
  }

  switch (gt) {
  case gameOpen:
    CheckDlgButton(hWnd, IDC_RADIO1, BST_CHECKED);
    break;
  case gameTournament:
    CheckDlgButton(hWnd, IDC_RADIO2, BST_CHECKED);
    break;
  default:
    CheckDlgButton(hWnd, IDC_RADIO3, BST_CHECKED);
    break;
  }

  if (hm == TRUE) {
    CheckDlgButton(hWnd, IDC_HIDDEN_MINES, BST_CHECKED);
  }

  dlgWnd = GetDlgItem(hWnd, IDC_ALLOWCOMP2);
  switch (ai) {
  case aiYesAdvantage:
    CheckDlgButton(hWnd, IDC_ALLOWCOMP1, BST_CHECKED);
    CheckDlgButton(hWnd, IDC_ALLOWCOMP2, BST_CHECKED);
    EnableWindow(dlgWnd, TRUE);
    break;
  case aiYes:
    CheckDlgButton(hWnd, IDC_ALLOWCOMP1, BST_CHECKED);
    EnableWindow(dlgWnd, TRUE);
    break;
  case aiNone:
  default:
    break;
  }
  if (tlimit != UNLIMITED_GAME_TIME) {
    CheckDlgButton(hWnd, IDC_CHECKTIME_LIMIT, BST_CHECKED);
    dlgWnd = GetDlgItem(hWnd, IDC_EDITTIMELIMIT);
    EnableWindow(dlgWnd, TRUE);
    tlimit /= GAME_NUMGAMETICKS_SEC;
    tlimit /= NUM_SECONDS;
    ltoa(tlimit, password, 10);
    SetDlgItemText(hWnd, IDC_EDITTIMELIMIT, password);
  }
  if (sd > 0) {
    CheckDlgButton(hWnd, IDC_GAMESTART, BST_CHECKED);
    dlgWnd = GetDlgItem(hWnd, IDC_EDITSTART);
    EnableWindow(dlgWnd, TRUE);
    sd /= GAME_NUMGAMETICKS_SEC;
    ltoa(sd, password, 10);
    SetDlgItemText(hWnd, IDC_EDITSTART, password);
  }


  /* Check arguments */
  fileName[0] = '\0';
  gameFrontGetCmdArg(fileName);
  if (fileName[0] != '\0') {
    dialogGameSetupMapCheck(hWnd, fileName);
  }
}

/*********************************************************
*NAME:          dialogGameSetupDone
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 29/1/99
*PURPOSE:
* Called on dialog finishing to pass the game options
* back to the gameFront module
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameSetupDone(HWND hWnd) {
  gameType gt;            /* Game type */
  bool hm;                /* Hidden Mines */
  aiType ai;              /* AI Type */
  long gsd;               /* Game start delay */
  long gl;                /* Game length */
  char str[MAP_STR_SIZE]; /* Holds password and used to temp hold strings before they turn into numbers */

  str[0] = '\0';
  gsd = 0;
  gl = UNLIMITED_GAME_TIME;

  /* Game type */
  if (IsDlgButtonChecked(hWnd, IDC_RADIO1) == BST_CHECKED) {
    gt = gameOpen;
  } else if (IsDlgButtonChecked(hWnd, IDC_RADIO2) == BST_CHECKED) {
    gt = gameTournament;
  } else {
    gt = gameStrictTournament;
  }

  /* Hidden Mines */
  if (IsDlgButtonChecked(hWnd, IDC_HIDDEN_MINES) == BST_CHECKED) {
    hm = TRUE;
  } else {
    hm = FALSE;
  }

  /* Allow AI */
  if (IsDlgButtonChecked(hWnd, IDC_ALLOWCOMP1) == BST_CHECKED) {
    if (IsDlgButtonChecked(hWnd, IDC_ALLOWCOMP2) == BST_CHECKED) {
      ai = aiYesAdvantage;
    } else {
      ai = aiYes;
    }
  } else {
    ai = aiNone;
  }

  /* Game Start Delay */
  if (IsDlgButtonChecked(hWnd, IDC_GAMESTART) == BST_CHECKED) {
    GetDlgItemText(hWnd, IDC_EDITSTART, str, sizeof(str));
    gsd = atol(str);
    if (gsd < 0) {
      gsd = 0;
    }
  }

  /* Get Game Len */
  if (IsDlgButtonChecked(hWnd, IDC_CHECKTIME_LIMIT) == BST_CHECKED) {
    GetDlgItemText(hWnd, IDC_EDITTIMELIMIT, str, sizeof(str));
    gl = atol(str);
    if (gl < 0) {
      gl = UNLIMITED_GAME_TIME;
    }
  }

  /* Get Password */
  str[0] = '\0';
  if (IsDlgButtonChecked(hWnd, IDC_CHECKPASS) == BST_CHECKED) {
    GetDlgItemText(hWnd, IDC_EDITPASS, str, sizeof(str));
  }

  /* Change Game Start delay from seconds to ticks */
  if (gsd > 0) {
    gsd *= GAME_NUMGAMETICKS_SEC;
  }

   /* Change Game Length from minutes to ticks */
  if (gl > 0) {
    gl *= NUM_SECONDS;
    gl *= GAME_NUMGAMETICKS_SEC;
  }

  /* Pass values back */
  gameFrontSetGameOptions(str, gt, hm, ai, gsd, gl, FALSE);
}

