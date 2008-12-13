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
*Creation Date: 07/05/01
*Last Modified: 07/05/01
*Purpose:
*  Looks after the Game setup dialog
*********************************************************/

#include <windows.h>
#include <commdlg.h>
#include "global.h"
#include "backend.h"
#include "resource.h"

#define COLOUR_0 "Grey"
#define COLOUR_1 "Khaki"
#define COLOUR_2 "Green"
#define COLOUR_3 "Pink"
#define COLOUR_4 "Yellow"
#define COLOUR_5 "Light Blue"
#define COLOUR_6 "Orange"
#define COLOUR_7 "Light Purple"
#define COLOUR_8 "Aqua"
#define COLOUR_9 "Light Green"
#define COLOUR_10 "Light Grey"
#define COLOUR_11 "Red"
#define COLOUR_12 "Blue"
#define COLOUR_13 "Brown"
#define COLOUR_14 "Light Pink"
#define COLOUR_15 "Pale Green"
#define COLOUR_16 "Purple"


extern BYTE tc[17];


bool dialogWindowTeamColoursCheck(HWND hWnd) {
  BYTE check[17];
  BYTE count = 0;

  check[0] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM1, CB_GETCURSEL, 0, 0);
  check[1] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM2, CB_GETCURSEL, 0, 0);
  check[2] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM3, CB_GETCURSEL, 0, 0);
  check[3] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM4, CB_GETCURSEL, 0, 0);
  check[4] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM5, CB_GETCURSEL, 0, 0);
  check[5] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM6, CB_GETCURSEL, 0, 0);
  check[6] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM7, CB_GETCURSEL, 0, 0);
  check[7] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM8, CB_GETCURSEL, 0, 0);
  check[8] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM9, CB_GETCURSEL, 0, 0);
  check[9] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM10, CB_GETCURSEL, 0, 0);
  check[10] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM11, CB_GETCURSEL, 0, 0);
  check[11] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM12, CB_GETCURSEL, 0, 0);
  check[12] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM13, CB_GETCURSEL, 0, 0);
  check[13] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM14, CB_GETCURSEL, 0, 0);
  check[14] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM15, CB_GETCURSEL, 0, 0);
  check[15] = (BYTE) SendDlgItemMessage(hWnd, IDC_TEAM16, CB_GETCURSEL, 0, 0);
  check[16] = (BYTE) SendDlgItemMessage(hWnd, IDC_NEUTRAL, CB_GETCURSEL, 0, 0);

  // blah!

  while (count < 17) {
    tc[count] = check[count];
    count++;
  }

  


  return TRUE;
}

void dialogWindowTeamSetColours(HWND hWnd, int item, BYTE colourSelect) {
  HWND hItem = GetDlgItem(hWnd, item);

  if (hItem != NULL) {
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_0);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_1);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_2);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_3);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_4);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_5);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_6);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_7);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_8);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_9);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_10);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_11);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_12);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_13);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_14);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_15);
    SendMessage(hItem, CB_ADDSTRING, 0, (LPARAM) COLOUR_16);

    if (colourSelect > 16) {
      colourSelect = 16;
    }

    SendMessage(hItem, CB_SETCURSEL, colourSelect, 0);

  }

}

/*********************************************************
*NAME:          dialogWindowSizeCallback
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  The Opening Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogWindowTeamColoursCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND hItem = GetDlgItem(hWnd, IDC_TEAM1);

  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogWindowTeamSetColours(hWnd, IDC_TEAM1, tc[0]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM2, tc[1]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM3, tc[2]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM4, tc[3]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM5, tc[4]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM6, tc[5]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM7, tc[6]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM8, tc[7]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM9, tc[8]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM10, tc[9]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM11, tc[10]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM12, tc[11]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM13, tc[12]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM14, tc[13]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM15, tc[14]);
    dialogWindowTeamSetColours(hWnd, IDC_TEAM16, tc[15]);
    dialogWindowTeamSetColours(hWnd, IDC_NEUTRAL, tc[16]);
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case IDOK:
      if (dialogWindowTeamColoursCheck(hWnd) == TRUE) {
        EndDialog(hWnd, TRUE);
      }
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
