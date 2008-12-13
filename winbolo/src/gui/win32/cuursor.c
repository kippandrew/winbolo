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
*Name:          Cursor
*Filename:      cursor.c
*Author:        John Morrison
*Creation Date: 26/12/98
*Last Modified: 18/01/02
*Purpose:
*  Loads the different Cursors
*********************************************************/

#include <stdlib.h>
#include <windows.h>
#include <winuser.h>
#include "..\..\bolo\global.h"
#include "..\..\bolo\backend.h"
#include "..\winbolo.h"
#include "..\tiles.h"
#include "..\positions.h"
#include "..\resource.h"
#include "..\cursor.h"

/* Is the cursor inside the main view area */
bool cursorInMainView = FALSE;
HCURSOR saveCurs = NULL;

/*********************************************************
*NAME:          cursorSetup
*AUTHOR:        John Morrison
*CREATION DATE: 02/01/99
*LAST MODIFIED: 18/01/02
*PURPOSE:
*  Loads and Sets up cursors
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
* appInst - Handle to the application (Required to 
*           load bitmaps from resources)
* appWnd  - Main Window Handle (Required for clipper)
*********************************************************/
bool cursorSetup(HINSTANCE appInst, HWND appWnd) {
  saveCurs = CopyCursor(LoadCursor(NULL, IDC_ARROW));
  return TRUE;
}

/*********************************************************
*NAME:          cursorCleanup
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Destroys and cleans up cursors systems
*
*ARGUMENTS:
*  appInst  - Application instance
*********************************************************/
void cursorCleanup(HINSTANCE appInst) {
  cursorSetCursor(appInst, TRUE);
}

/*********************************************************
*NAME:          cursorSetCursor
*AUTHOR:        John Morrison
*CREATION DATE:  8/1/99
*LAST MODIFIED: 18/1/02
*PURPOSE:
*  Loads and Sets up cursors a cursor on the screen
*  Sets it as the system cursor to stop flickering
*
*ARGUMENTS:
* appInst    - Handle to the application (Required to 
*              load bitmaps from resources)
* normalCurs - Is it the normal cursor or Bolo one ?
*********************************************************/
void cursorSetCursor(HINSTANCE appInst, bool normalCurs) {
  HCURSOR hCurs = NULL; /* The cursor to set */

  if (normalCurs == TRUE) {
    hCurs = CopyCursor(saveCurs);
  } else {
    hCurs = LoadCursor(appInst, MAKEINTRESOURCE(IDC_BOLOCURSOR));  
  }
  if (hCurs != NULL) {
     SetSystemCursor(hCurs, 32512);
  }
}

/*********************************************************
*NAME:          cursorMove
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/00
*PURPOSE:
*  The cursor has moved. Check for a changed icon
*
*ARGUMENTS:
*  appInst  - Application instance
*  hWnd     - Main Window handle
*  rcWindow - Rect with the windows co-ordinates
*********************************************************/
void cursorMove(HINSTANCE appInst, HWND hWnd, RECT rcWindow) {
  POINT mousePos;  /* Used to get the mouse position */
  int xPos;        /* X and Y positions of the mouse adjusted */ 
  int yPos;        /* to the window */
  BYTE zoomFactor; /* Zooming size */
  int leftPos;     /* Rect co-ordinates */
  int rightPos;
  int topPos;
  int bottomPos;


  if (GetForegroundWindow() != hWnd) {
    if (cursorInMainView == TRUE) {
      cursorSetCursor(appInst, TRUE);
      cursorInMainView = FALSE;
    }
    return;
  } else {
    GetCursorPos(&mousePos);
    zoomFactor = windowGetZoomFactor();
    xPos = mousePos.x - rcWindow.left;
    yPos = mousePos.y - rcWindow.top;
    leftPos = zoomFactor * MAIN_OFFSET_X;
    rightPos = leftPos + (MAIN_SCREEN_SIZE_X * (zoomFactor * TILE_SIZE_X));
    topPos = zoomFactor * MAIN_OFFSET_Y;
    bottomPos = topPos + (MAIN_SCREEN_SIZE_Y * (zoomFactor * TILE_SIZE_Y));
    if (xPos >= leftPos && xPos <= rightPos && yPos >= topPos && yPos <= bottomPos) {
      if (cursorInMainView == FALSE) {
        cursorSetCursor(appInst, FALSE);
        cursorInMainView = TRUE;
      }
    } else {
      if (cursorInMainView == TRUE) {
        cursorSetCursor(appInst, TRUE);
        cursorInMainView = FALSE;
      }
    }
  }
}

/*********************************************************
*NAME:          cursorPos
*AUTHOR:        John Morrison
*CREATION DATE:  8/1/99
*LAST MODIFIED: 27/3/99
*PURPOSE:
*  Returns whether the cursor is inside the main view.
*  If it is then it returns xValue & yValue with the 
*  screen square that it is in. If it is not in the main
*  view it fills them with 0
*
*ARGUMENTS:
*  rcWindow - Rect with the windows co-ordinates
*  xValue   - Pointer to store x Main Sqaure
*  yValue   - Pointer to store y Main Square
*********************************************************/
bool cursorPos(RECT *rcWindow, BYTE *xValue, BYTE *yValue) {
  POINT mousePos;  /* Used to get the mouse position */
  int xPos;        /* X and Y positions of the mouse adjusted */ 
  int yPos;        /* to the window */
  BYTE zoomFactor; /* The zooming factor */
  div_t dt;        /* Used for integer division */

  if (cursorInMainView == TRUE) { 
    if (GetCursorPos(&mousePos) == 0) {
      return FALSE;
    }
    zoomFactor = windowGetZoomFactor();
    xPos = mousePos.x - rcWindow->left;
    yPos = mousePos.y - rcWindow->top;
    xPos -= zoomFactor * MAIN_OFFSET_X;
    yPos -= zoomFactor * MAIN_OFFSET_Y;
    dt = div(xPos, (zoomFactor * (MAIN_SCREEN_SIZE_X+1)));
    *xValue = (BYTE) (dt.quot+1);
    dt = div(yPos, (zoomFactor * (MAIN_SCREEN_SIZE_Y+1)));
    *yValue = (BYTE) (dt.quot+1);
    if (*xValue > MAIN_SCREEN_SIZE_X || *yValue > MAIN_SCREEN_SIZE_Y) {
      return FALSE;
    }
    return TRUE;
  } else {
    *xValue = 0;
    *yValue = 0;
  }
  return FALSE;
}

/*********************************************************
*NAME:          cursorAcquireCursor
*AUTHOR:        John Morrison
*CREATION DATE: 13/1/99
*LAST MODIFIED: 27/3/99
*PURPOSE:
*  The window has just aquired the cursor. Set its icon
*  accordingly
*
*ARGUMENTS:
*  appInst  - Application instance
*  rcWindow - Rect with the windows co-ordinates
*********************************************************/
void cursorAcquireCursor(HINSTANCE appInst, RECT rcWindow) {
  POINT mousePos;  /* Used to get the mouse position */
  int xPos;        /* X and Y positions of the mouse adjusted */ 
  int yPos;        /* to the window */
  BYTE zoomFactor; /* The zooming factor */

//  cursorMove(appInst, windowWnd(), rcWindow);
  GetCursorPos(&mousePos);
  zoomFactor = windowGetZoomFactor();
  xPos = mousePos.x - rcWindow.left;
  yPos = mousePos.y - rcWindow.top;

  if ((xPos >= (zoomFactor * MAIN_OFFSET_X) && xPos <= ((zoomFactor * MAIN_OFFSET_X) + ((zoomFactor * MAIN_SCREEN_SIZE_X) * (zoomFactor * TILE_SIZE_X)))) && (yPos >= (zoomFactor * MAIN_OFFSET_Y) && yPos <= ((zoomFactor * MAIN_OFFSET_Y) + ((zoomFactor * MAIN_SCREEN_SIZE_Y) * (zoomFactor * TILE_SIZE_Y))))) {
    cursorSetCursor(appInst, FALSE);
    cursorInMainView = TRUE;
  } else {
    cursorSetCursor(appInst, TRUE);
    cursorInMainView = FALSE;
  } 
}

/*********************************************************
*NAME:          cursorLeaveWindow
*AUTHOR:        John Morrison
*CREATION DATE: 13/1/99
*LAST MODIFIED: 28/3/99
*PURPOSE:
*  The window has just lost the focus. Update the cursor
*
*ARGUMENTS:
*
*********************************************************/
void cursorLeaveWindow(void) {
  cursorSetCursor(windowGetInstance(), TRUE);
}

/*********************************************************
*NAME:          cursorSetPos
*AUTHOR:        John Morrison
*CREATION DATE: 21/1/01
*LAST MODIFIED: 21/1/01
*PURPOSE:
*  Sets the cursors position on the screen
*
*ARGUMENTS:
*  rcWindow - Rect with the windows co-ordinates
*  xValue   - X poition on the main screen
*  yValue   - Y position on the main screen
*********************************************************/
void cursorSetPos(RECT rcWindow, BYTE xValue, BYTE yValue) {
  static BYTE lastX = 0;
  static BYTE lastY = 0;
  BYTE zoomFactor; /* The zooming factor */
  int x;
  int y;

//  cursorMove(windowGetInstance(), windowWnd(), rcWindow);
  if (cursorInMainView == TRUE && yValue != 0 && xValue != 0 && (lastX != xValue || lastY != yValue)) {
    char str[255];
    lastX = xValue;
    lastY = yValue;
    sprintf(str, "%d,%d", xValue, yValue);
  //  messageAdd(0, "sp", str);
    xValue--;
    yValue--;
    zoomFactor = windowGetZoomFactor();
    x = xValue * zoomFactor * TILE_SIZE_X + 8;
    y = yValue * zoomFactor * TILE_SIZE_Y + 8;
    
//    SetCursorPos(zoomFactor * MAIN_OFFSET_X + rcWindow.left + x, zoomFactor * MAIN_OFFSET_Y + rcWindow.top + y);
  }
}
