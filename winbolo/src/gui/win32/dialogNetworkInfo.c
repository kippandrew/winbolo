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
*Name:          Dialog - Network Info
*Filename:      dialognetworkinfo.h
*Author:        John Morrison
*Creation Date:  3/3/99
*Last Modified: 29/4/00
*Purpose:
*  Looks after the network info dialog box.
*********************************************************/

#include <string.h>
#include <windows.h>
#include "..\..\bolo\global.h"
#include "..\..\bolo\network.h"
#include "..\resource.h"
#include "..\lang.h"
#include "..\winbolo.h"
#include "..\dialognetworkinfo.h"
#include "..\dnsLookups.h"
#include "..\..\bolo\players.h"
#include "..\..\bolo\backend.h"

/* There are 1000 miliseconds in a second */ 
#define SECOND 1000

unsigned int timerNetInfo = 3; /* The timer for getting the net info udpates */

/*********************************************************
*NAME:          dialogNetInfoCallback
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 3/3/99
*PURPOSE:
*  The Network Info call back function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogNetInfoCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogNetInfoSetup(hWnd);
    break;
  case WM_TIMER:
    dialogNetInfoUpdate(hWnd);
    break;
  case WM_PAINT:
    break;
  case WM_CLOSE:
    /* Fall through to the WM_DESTROY message */
  case WM_DESTROY:
    KillTimer(hWnd, timerNetInfo);
    windowShowNetInfo(wsrClose);
    return EndDialog(hWnd, TRUE);
    break;
  }
  return FALSE;
}

/*********************************************************
*NAME:          dialogNetInfoSetup
*AUTHOR:        John Morrison
*CREATION DATE:  3/3/99
*LAST MODIFIED: 29/4/00
*PURPOSE:
* Sets up the dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogNetInfoSetup(HWND hWnd) {
  char str[FILENAME_MAX]; /* Our address as a string */
  char addr[FILENAME_MAX];

  /* Setup languages */
  SetWindowTextA(hWnd, langGetText(STR_DLGNETINFO_TITLE));
  SetDlgItemTextA(hWnd, IDC_SERVERS, langGetText(STR_DLGNETINFO_SERVERADDRESS));
  SetDlgItemTextA(hWnd, IDC_THISADDRESSS, langGetText(STR_DLGNETINFO_THISGAMEADDRESS));
  SetDlgItemTextA(hWnd, IDC_SERVERPINGS, langGetText(STR_DLGNETINFO_SERVERPING));
  SetDlgItemTextA(hWnd, IDC_PPSPLS, langGetText(STR_DLGNETINFO_PACKETS));
  SetDlgItemTextA(hWnd, IDC_NETSTATUSS, langGetText(STR_DLGNETINFO_STATUS));
  SetDlgItemTextA(hWnd, IDC_NETERRORSS, langGetText(STR_DLGNETINFO_ERRORS));

  /* We are in a client and playing a networked game.. */
  if (threadsGetContext() == FALSE && netGetType() != netSingle)
  {
	  playersGetPlayerLocation(screenGetPlayers(),playersGetSelf(screenGetPlayers()),addr);
	  netGetOurAddressStr(str);
	  strcat(addr,strchr(str,':'));
	  SendDlgItemMessageA(hWnd, IDC_THISADDRESS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (addr));
  }
  else /* Single player game, no networking */
  {
      netGetOurAddressStr(str);
	  SendDlgItemMessageA(hWnd, IDC_THISADDRESS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));
  }
  timerNetInfo = SetTimer(hWnd, timerNetInfo, SECOND, NULL);
  dialogNetInfoUpdate(hWnd);
}

/*********************************************************
*NAME:          dialogNetInfoUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 1/9/99
*PURPOSE:
* Updates the network info dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
void dialogNetInfoUpdate(HWND hWnd) {
  int ping;               /* Downstream delay */
  int ppsec;              /* Packets per second */
  int numErrors;          /* Number of errors */
  char str[FILENAME_MAX]; /* Strings to display */

  /* Get addresses */
  netGetServerAddressStr(str);
  SendDlgItemMessageA(hWnd, IDC_SERVER, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));
  /* Get data */
  netGetStats(str, &ping, &ppsec, &numErrors);
  /* Print data items */
  SendDlgItemMessageA(hWnd, IDC_NETSTATUS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));
  sprintf(str, "%i", ping);
  SendDlgItemMessageA(hWnd, IDC_PING, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));
  sprintf(str, "%i", ppsec);
  SendDlgItemMessageA(hWnd, IDC_PACKETSPERSECOND, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));
  sprintf(str, "%i", numErrors);
  SendDlgItemMessageA(hWnd, IDC_NETERRORS, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) (str));
}
