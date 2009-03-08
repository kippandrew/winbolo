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
*Name:          Dialog - Game Finder
*Filename:      dialogGameFinder.c
*Author:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 22/6/00
*Purpose:
*  Looks after the Game finder dialog
*********************************************************/

#include <string.h>
#include <windows.h>
#include "..\..\bolo\global.h"
#include "..\gamefront.h"
#include "..\resource.h"
#include "..\currentGames.h"
#include "..\netClient.h"
#include "..\dialogTrackerSetup.h"
#include "..\dialogsetname.h"
#include "..\lang.h"
#include "..\winbolo.h"
#include "..\dialoggamefinder.h"

#define EMPTY_STRING "\0"

bool useTracker = TRUE;  /* Do we use the tracker for lookups */
char dialogTitle[FILENAME_MAX]; /* Title of the dialog box */
char motd[4096];
currentGames cg = NULL;

/*********************************************************
*NAME:          dialogGameFinderSetMethod
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 23/1/00
*PURPOSE:
*  Sets the lookup method for the finding games and the
*  title of the dialog box.
*
*ARGUMENTS:
*  title            - Title of the dialog box
*  useTrackerMethod - TRUE if we are to use the tracker
*                     for game lookups. Else we broadcast
*********************************************************/
void dialogGameFinderSetMethod(char *title, bool useTrackerMethod) {
  motd[0] = '\0';
  strcpy(dialogTitle, title);
  useTracker = useTrackerMethod;
}

/*********************************************************
*NAME:          dialogGameFinderCallback
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 22/6/00
*PURPOSE:
*  The GameFinder Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogGameFinderCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND hList;
  HWND hJoin;
  unsigned int nItem;
  int itemNum;

  switch ( uMsg ) {
  case WM_INITDIALOG:
    dialogGameFinderInit(hWnd);
    break;
  case WM_COMMAND:
    switch (LOWORD (wParam)) {
    case IDC_LIST:
      if (HIWORD(wParam) == LBN_SELCHANGE) {
        hList = GetDlgItem(hWnd, IDC_LIST); 
        nItem = (unsigned int) SendMessage(hList, LB_GETCURSEL, 0, 0); 
        itemNum = SendMessage(hList, LB_GETITEMDATA, nItem, 0); 
        dialogGameFinderSetItem(hWnd, itemNum);
        hJoin = GetDlgItem(hWnd, ID_JOIN); 
        EnableWindow(hJoin, TRUE);
        EnableWindow(GetDlgItem(hWnd, ID_REJOIN), TRUE);
      }
      break;
    case ID_JOIN:
      if (dialogGameFinderJoin(hWnd) == TRUE) {
        if (gameFrontSetDlgState(openUdpJoin) == TRUE) {
          EndDialog(hWnd,TRUE);
        }
      }
      break;
    case IDC_REJOIN:
      if (dialogGameFinderJoin(hWnd) == TRUE) {
        gameFrontEnableRejoin();
        if (gameFrontSetDlgState(openUdpJoin) == TRUE) {
          EndDialog(hWnd,TRUE);
        }
      }
      break;      
    case IDC_SETPLAYERNAME:
      DialogBox(windowGetInstance(), MAKEINTRESOURCE(IDD_SETPLAYERNAME), hWnd, dialogSetNameCallback);
      break;
    case IDC_MESSAGE:
      MessageBoxA(hWnd, motd, langGetText(STR_DLGGAMEFINDER_MESSAGEOFTHEDAY), MB_OK);
      break;
    case IDC_REFRESH:
      dialogGameFinderRefresh(hWnd);
      break;
    case IDC_JOINADDRESS: {
      char address[FILENAME_MAX];       /* Address of the machine to join */
      char playerName[PLAYER_NAME_LEN]; /* Player Name */
      unsigned short port;              /* Port of the machine to join    */

      if (GetDlgItemText(hWnd, IDC_PORT, address, sizeof(address)) > 0) {
        port = (unsigned short) atoi(address);
        GetDlgItemText(hWnd, IDC_SERVERADDRESS, address, (sizeof(address)));
        gameFrontGetPlayerName(playerName);
        gameFrontSetUdpOptions(playerName, address, port, 0);
      }
      gameFrontSetDlgState(openInternetManual);
      EndDialog(hWnd,TRUE);
      }
      break;
    case IDC_NEW: {
        char playerName[PLAYER_NAME_LEN]; /* Player Name */
        gameFrontGetPlayerName(playerName);
        gameFrontSetUdpOptions(playerName, (char *) "", 27500, 0);
      gameFrontSetDlgState(openInternetSetup);
      EndDialog(hWnd,TRUE);
                  }
      break;
    case IDC_TRACKER:
      DialogBox(windowGetInstance(), MAKEINTRESOURCE(IDD_TRACKERSETUP), hWnd, dialogTrackerSetupCallback);
      break;
    case IDCANCEL:
      gameFrontSetDlgState(openWelcome);
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
*NAME:          dialogGameFinderInit
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 02/6/00
*PURPOSE:
* Called on dialog startup.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameFinderInit(HWND hWnd) {
  cg = currentGamesCreate();

  if (useTracker == TRUE) {
    strcpy(motd, langGetText(STR_DLGGAMEFINDER_REFRESHFIRST));
  } else {
    EnableWindow(GetDlgItem(hWnd, IDC_MESSAGE), FALSE);
    EnableWindow(GetDlgItem(hWnd, IDC_TRACKER), FALSE);
  }

  
  dialogGameFinderClear(hWnd);
  /* Set the title */
  SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) dialogTitle);
  /* Buttons and frame names */
  SetDlgItemTextA(hWnd, IDC_FRAME, langGetText(STR_DLGGAMEFINDER_SELECTEDGAME)); 
  SetDlgItemTextA(hWnd, IDC_MESSAGE, langGetText(STR_DLGGAMEFINDER_MESSAGEOFTHEDAY)); 
  SetDlgItemTextA(hWnd, IDC_TRACKER, langGetText(STR_DLGTCP_TRACKERSETUP)); 
  SetDlgItemTextA(hWnd, IDC_NEW, langGetText(STR_DLGTCP_NEW)); 
  SetDlgItemTextA(hWnd, IDC_JOINADDRESS, langGetText(STR_DLGGAMEFINDER_JOINADDRESS)); 
  SetDlgItemTextA(hWnd, IDC_REFRESH, langGetText(STR_DLGGAMEFINDER_REFRESH)); 
  SetDlgItemTextA(hWnd, ID_JOIN, langGetText(STR_DLGTCP_JOIN)); 
  SetDlgItemTextA(hWnd, IDC_REJOIN, langGetText(STR_DLGTCP_REJOIN)); 
  SetDlgItemTextA(hWnd, IDCANCEL, langGetText(STR_CANCEL)); 
  SetDlgItemTextA(hWnd, IDC_SETPLAYERNAME, langGetText(STR_DLGGAMEFINDER_PLAYERNAME)); 

  /* Static text fields */
  SetDlgItemTextA(hWnd, IDC_SADRESS, langGetText(STR_DLGGAMEFINDER_ADDRESS)); 
  SetDlgItemTextA(hWnd, IDC_SPORT, langGetText(STR_DLGGAMEFINDER_PORT)); 
  SetDlgItemTextA(hWnd, IDC_VERSIONS, langGetText(STR_DLGGAMEFINDER_VERSION)); 
  SetDlgItemTextA(hWnd, IDC_MAPNAMES, langGetText(STR_DLGGAMEINFO_MAPNAME)); 
  SetDlgItemTextA(hWnd, IDC_NOPLAYERSS, langGetText(STR_DLGGAMEINFO_NUMPLAYERS)); 
  SetDlgItemTextA(hWnd, IDC_GAMETYPES, langGetText(STR_DLGGAMEINFO_GAMETYPE)); 
  SetDlgItemTextA(hWnd, IDCANCEL, langGetText(STR_CANCEL)); 
  SetDlgItemTextA(hWnd, IDC_FREEPILLSS, langGetText(STR_DLGGAMEFINDER_PILLS)); 
  SetDlgItemTextA(hWnd, IDC_FREEBASESS, langGetText(STR_DLGGAMEFINDER_BASES)); 
  SetDlgItemTextA(hWnd, IDC_HIDMINES, langGetText(STR_DLGGAMEFINDER_HIDDENMINES)); 
  SetDlgItemTextA(hWnd, IDC_PASSS, langGetText(STR_DLGGAMEFINDER_PASSWORD)); 
  SetDlgItemTextA(hWnd, IDC_BRAINSS, langGetText(STR_DLGGAMEFINDER_BRAINS));  
  SetDlgItemTextA(hWnd, IDC_STATUSS, langGetText(STR_DLGGAMEFINDER_STATUSBLURB));  
  SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENT_IDLE));  
}

/*********************************************************
*NAME:          dialogGameFinderClear
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 18/1/00
*PURPOSE:
* Clears the dialog of game information and empties the
* list box.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameFinderClear(HWND hWnd) {
  HWND hList;       /* The list box handle */
  int count;        /* Looping variable */
  
  /* Clear the text box */
  hList = GetDlgItem(hWnd, IDC_LIST);
  count = SendMessage(hList, LB_GETCOUNT, 0, 0);
  while (count >= 0) {
    /* Delete each item */
    SendMessage(hList, LB_DELETESTRING, count, 0); 
    count--;
  }

  /* Clear the information about the selected game */
  SetDlgItemTextA(hWnd, IDC_SERVERADDRESS, EMPTY_STRING);
  SetDlgItemTextA(hWnd, IDC_PORT, EMPTY_STRING);
  SetDlgItemTextA(hWnd, IDC_VERSION, EMPTY_STRING);
  SetDlgItemTextA(hWnd, IDC_MAPNAME, EMPTY_STRING);
  SetDlgItemTextA(hWnd, IDC_NOPLAYERS, EMPTY_STRING);
  SetDlgItemTextA(hWnd, IDC_FREEBASES, EMPTY_STRING);
  SetDlgItemTextA(hWnd, IDC_FREEPILLS, EMPTY_STRING);
  SetDlgItemTextA(hWnd, IDC_MINES, EMPTY_STRING);
  SetDlgItemTextA(hWnd, IDC_GAMETYPE, EMPTY_STRING);
  SetDlgItemTextA(hWnd, IDC_PASSWORD, EMPTY_STRING);
  SetDlgItemTextA(hWnd, IDC_BRAINS, EMPTY_STRING);
}

/*********************************************************
*NAME:          dialogGameFinderJoin
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified:  2/6/00
*PURPOSE:
* Called on attampt to join the currently selected game.
* Returns whether we can join the game or not. ie version
* numbers the same
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
bool dialogGameFinderJoin(HWND hWnd) {
  bool returnValue;                 /* Value to return */
  char address[FILENAME_MAX];       /* Address of the machine to join */
  char playerName[PLAYER_NAME_LEN]; /* Player Name */
  unsigned short port;              /* Port of the machine to join    */
  
  returnValue = TRUE;

  /* Check version */
  GetDlgItemText(hWnd, IDC_VERSION, address, (sizeof(address)));
  if (strncmp(address, STRVER,4) != 0) {
    MessageBoxA(hWnd, langGetText(STR_DLGGAMEFINDER_WRONGVERSION),DIALOG_BOX_TITLE, MB_ICONINFORMATION); 
    returnValue = FALSE;
  } else {
    GetDlgItemText(hWnd, IDC_PORT, address, (sizeof(address)));
    port = atoi(address);
    GetDlgItemText(hWnd, IDC_SERVERADDRESS, address, (sizeof(address)));
    gameFrontGetPlayerName(playerName);
	if(strlen(playerName)>0)
	{
		gameFrontSetUdpOptions(playerName, address, port, 0);
	} else
	{
		MessageBoxA(hWnd, langGetText(STR_DLGGAMEFINDER_PLAYERWRONG) ,DIALOG_BOX_TITLE, MB_ICONINFORMATION);
		returnValue = FALSE;
	}
  }

  return returnValue;
}

/*********************************************************
*NAME:          dialogGameFinderRefresh
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 18/1/00
*PURPOSE:
* Refreshs the list of active games.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameFinderRefresh(HWND hWnd) {
  SetCursor(LoadCursor(NULL, IDC_WAIT));
  motd[0] = '\0';
  dialogGameFinderClear(hWnd);
  currentGamesDestroy(&cg);
  cg = currentGamesCreate();
  dialogGameFinderSearch(hWnd);
  SetCursor(LoadCursor(NULL, IDC_ARROW));
}

/*********************************************************
*NAME:          dialogGameFinderBuildList
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 23/1/00
*PURPOSE:
* Fills the list box with the current games
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogGameFinderBuildList(HWND hWnd) {
  HWND hList; /* The list box handle */
  HWND hJoin; /* The join button */
  int count; /* Looping variable */
  int total; /* Total number of games found */
  char server[FILENAME_MAX]; /* Server name */

  total = currentGamesItemCount(&cg);
  hJoin = GetDlgItem(hWnd, ID_JOIN); 
  EnableWindow(hJoin, FALSE);
  EnableWindow(GetDlgItem(hWnd, ID_REJOIN), FALSE);
  if (total > 0) {
    count = 1;
    hList = GetDlgItem(hWnd, IDC_LIST);
    while (count <= total) {
      currentGamesGetServerName(&cg, count, server);
      SendMessage(hList, LB_ADDSTRING, 0, (LPARAM) server); 
      SendMessage(hList, LB_SETITEMDATA, count-1, (LPARAM) count-1); 
      count++;
    }
  } else {
    /* Display no games found message */
    SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_DLGGAMEFINDER_NOGAMESINPROGRESS));
  }
}

/*********************************************************
*NAME:          dialogGameFinderSearch
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified:  4/6/00
*PURPOSE:
* Does the search for games.
*
*ARGUMENTS:
* hWnd - Handle to the dialog
**********************************************************/
void dialogGameFinderSearch(HWND hWnd) {
  char addr[FILENAME_MAX]; /* Tracker address */
  unsigned short port;     /* Tracker port    */
  bool dummy;              /* dummy variable  */
  bool ret;                /* Function return Value */

  if (useTracker == TRUE) {
    gameFrontGetTrackerOptions(addr, &port, &dummy);
    ret = netClientFindTrackedGames(hWnd, &cg, addr, port, motd);
  } else {
    ret = netClientFindBroadcastGames(hWnd, &cg);
  }

  if (ret == TRUE) {
    SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENT_IDLE));
    dialogGameFinderBuildList(hWnd);
  }
}

/*********************************************************
*NAME:          dialogGameFinderSetItem
*AUTHOR:        John Morrison
*Creation Date: 18/1/00
*Last Modified: 02/6/00
*PURPOSE:
* Sets the details up about a particular item
*
*ARGUMENTS:
*  hWnd    - Handle to the window
*  itemNum - The item number pressed 
*********************************************************/
void dialogGameFinderSetItem(HWND hWnd, int itemNum) {
  char address[FILENAME_MAX];
  char str[FILENAME_MAX];
  char mapName[MAP_STR_SIZE];
  unsigned short port;
  BYTE numPlayers;
  BYTE numBases;
  BYTE numPills;
  bool mines;
  gameType game;
  aiType ai;
  bool password;

  itemNum++;
  currentGamesGetItem(&cg, itemNum, address, &port, mapName, str, &numPlayers, &numBases, &numPills, &mines, &game, &ai, &password);
  SetDlgItemTextA(hWnd, IDC_SERVERADDRESS, address);
  SetDlgItemTextA(hWnd, IDC_MAPNAME, mapName);
  SetDlgItemTextA(hWnd, IDC_VERSION, str);

  sprintf(str, "%d", port);
  SetDlgItemTextA(hWnd, IDC_PORT, str);
  sprintf(str, "%d", numPlayers);
  SetDlgItemTextA(hWnd, IDC_NOPLAYERS, str);
  sprintf(str, "%d", numBases);
  SetDlgItemTextA(hWnd, IDC_FREEBASES, str);
  sprintf(str, "%d", numPills);
  SetDlgItemTextA(hWnd, IDC_FREEPILLS, str);
  if (mines == TRUE) {
    SetDlgItemTextA(hWnd, IDC_MINES, langGetText(STR_YES));
  } else {
    SetDlgItemTextA(hWnd, IDC_MINES, langGetText(STR_NO));
  }
  if (password == TRUE) {
    SetDlgItemTextA(hWnd, IDC_PASSWORD, langGetText(STR_YES));
  } else {
    SetDlgItemTextA(hWnd, IDC_PASSWORD, langGetText(STR_NO));
  }
  switch (ai) {
  case aiNone:
    SetDlgItemTextA(hWnd, IDC_BRAINS, langGetText(STR_NO));
    break;
  case aiYes:
    SetDlgItemTextA(hWnd, IDC_BRAINS, langGetText(STR_YES));
    break;
  case aiYesAdvantage:
  default:
    SetDlgItemTextA(hWnd, IDC_BRAINS, langGetText(STR_DLGGAMEFINDER_YESADV));
    break;
  }
  switch (game) {
  case gameOpen:
    SetDlgItemTextA(hWnd, IDC_GAMETYPE, langGetText(STR_DLGGAMEFINDER_OPEN));
    break;
  case gameTournament:
    SetDlgItemTextA(hWnd, IDC_GAMETYPE, langGetText(STR_DLGGAMEFINDER_TOURNAMENT));
    break;
  case gameStrictTournament:
  default:
    SetDlgItemTextA(hWnd, IDC_GAMETYPE, langGetText(STR_DLGGAMEFINDER_STRICTTOURNAMENT));
    break;
  }
}
