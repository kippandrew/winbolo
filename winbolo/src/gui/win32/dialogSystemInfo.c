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
*Name:          Dialog - System Info
*Filename:      dialogsysteminfo.c
*Author:        John Morrison
*Creation Date: 23/1/99
*Last Modified: 29/4/00
*Purpose:
*  Looks after the system info dialog box.
*********************************************************/

#include <windows.h>
#include "..\..\bolo\global.h"
#include "..\draw.h"
#include "..\winbolo.h"
#include "..\resource.h"
#include "..\lang.h"
#include "..\dialogsysteminfo.h"

unsigned int timerSystemInfo = 53; /* The timer for getting the system info / frame rate etc.*/

/*********************************************************
*NAME:          dialogSysInfoCallback
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
*  The System Info call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogSysInfoCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogSysInfoSetup(hWnd);
    break;
  case WM_TIMER:
    dialogSysInfoUpdate(hWnd);
    break;
  case WM_PAINT:
    break;
  case WM_CLOSE:
    /* Fall through to the WM_DESTROY message */
  case WM_DESTROY:
    KillTimer(hWnd, timerSystemInfo);
    windowShowSysInfo(wsrClose);
    return EndDialog(hWnd, TRUE);
    break;
  }
  return FALSE;
}

/*********************************************************
*NAME:          dialogSysInfoSetup
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 29/4/00
*PURPOSE:
* Sets up the system info dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogSysInfoSetup(HWND hWnd) {
  /* Set language */
  SetWindowTextA(hWnd, langGetText(STR_DLGSYSINFO_TITLE));
  SetDlgItemTextA(hWnd, IDC_CPUUSAGES, langGetText(STR_DLGSYSINFO_CPUUSAGE));
  SetDlgItemTextA(hWnd, IDC_SIMMODELS, langGetText(STR_DLGSYSINFO_SIMMODELING));
  SetDlgItemTextA(hWnd, IDC_COMPROCESSS, langGetText(STR_DLGSYSINFO_COMPROCESSING));
  SetDlgItemTextA(hWnd, IDC_GRAPHICSDISPLAYS, langGetText(STR_DLGSYSINFO_GRAHPICSDISPLAY));
  SetDlgItemTextA(hWnd, IDC_AITANKS, langGetText(STR_DLGSYSINFO_AICONTROLPROCESSING));
  SetDlgItemTextA(hWnd, IDC_TOTALS, langGetText(STR_DLGSYSINFO_TOTAL));
  SetDlgItemTextA(hWnd, IDC_FPSS, langGetText(STR_DLGSYSINFO_GRAPHICSFPS));

  timerSystemInfo = (unsigned int) SetTimer(hWnd, timerSystemInfo, MILLISECONDS, NULL);
  dialogSysInfoUpdate(hWnd);
}


/*********************************************************
*NAME:          dialogSysInfoSetup
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
* Updates the system info dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogSysInfoUpdate(HWND hWnd) {
  int value;       /* Used to hold the frame rate */
  float graphics;
  float sim;
  float net;
  char str[FILENAME_MAX]; /* Frame rate as a string */

  /* Frame Rate */
  str[0] = '\0';
  value = drawGetFrameRate();
  sprintf(str, "%d", value);
  SendDlgItemMessageA(hWnd, IDC_FRAME_RATE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));
  
  /* Graphics */
  value = windowGetDrawTime();
  str[0] = '\0';
  graphics = (float) ((value/SECOND) *100.0);
  sprintf(str, "%.2f %%", graphics);
  SendDlgItemMessageA(hWnd, IDC_GRAPHICS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));
  
  /* Sim */
  value = windowGetSimTime();
  str[0] = '\0';
  sim = (float) ((value/SECOND)*100.0);
  sprintf(str, "%.2f %%", sim);
  SendDlgItemMessageA(hWnd, IDC_SIM, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));

  /* Net */
  value = windowGetNetTime();
  str[0] = '\0';
  net = (float) ((value/SECOND)*100.0);
  sprintf(str, "%.2f %%", net);
  SendDlgItemMessageA(hWnd, IDC_COMMUNICATION, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));

  /* AI */
  value = windowGetAiTime();
  str[0] = '\0';
  net = (float) ((value/SECOND)*100.0);
  sprintf(str, "%.2f %%", net);
  SendDlgItemMessageA(hWnd, IDC_AI, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));

  /* Total */
  sim += graphics + net;
  sprintf(str, "%.2f %%", sim);
  SendDlgItemMessageA(hWnd, IDC_TOTAL, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));
}
