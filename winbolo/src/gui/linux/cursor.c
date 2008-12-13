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
*Last Modified: 19/6/00
*Purpose:
*  Loads the different Cursors
*********************************************************/

#include <stdlib.h>
#include "SDL.h"
#include "../../bolo/global.h"
#include "../../bolo/backend.h"
#include "../tiles.h"
#include "../positions.h"
#include "cursor.h"

/* Is the cursor inside the main view area */
bool cursorInMainView = FALSE;
SDL_Cursor *saveCursor = NULL;
SDL_Cursor *boloCursor = NULL;

Uint8 cd[8] = { 0,0,0,0,0,0,0,0 };
Uint8 cm[8] = { 56, 40, 238, 124, 238, 40, 56 };
/*********************************************************
*NAME:          cursorSetup
*AUTHOR:        John Morrison
*CREATION DATE: 2/1/99
*LAST MODIFIED: 2/1/99
*PURPOSE:
*  Loads and Sets up cursors
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
* appInst - Handle to the application (Required to 
*           load bitmaps from resources)
* appWnd  - Main Window Handle (Required for clipper)
*********************************************************/
bool cursorSetup() {
  bool returnValue; /* Value to return */

  returnValue = TRUE;
  saveCursor = SDL_GetCursor();
  boloCursor = SDL_CreateCursor(cd, cm, 7, 7, 3, 3);
  cursorSetCursor(TRUE);
  if (boloCursor == NULL || saveCursor == NULL) {
    fprintf(stderr, "Error building cursor\n");
    returnValue = FALSE;
  }
  return returnValue;
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
void cursorCleanup() {
  if (boloCursor != NULL) {
    SDL_FreeCursor(boloCursor);
    boloCursor = NULL;
  }
}

/*********************************************************
*NAME:          cursorSetCursor
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/99
*LAST MODIFIED: 8/1/99
*PURPOSE:
*  Loads and Sets up cursors a cursor on the screen
*  Sets it as the system cursor to stop flickering
*
*ARGUMENTS:
* normalCurs - Is it the normal cursor or Bolo one ?
*********************************************************/
void cursorSetCursor(bool normalCurs) {
  if (normalCurs == TRUE) {
    SDL_SetCursor(saveCursor);
    //hCurs = LoadCursor(appInst, MAKEINTRESOURCE(IDC_POINTER));
  } else {
    SDL_SetCursor(boloCursor);
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
void cursorMove(int mouseX, int mouseY) {
  int xPos;        /* X and Y positions of the mouse adjusted */ 
  int yPos;        /* to the window */
  BYTE zoomFactor; /* Zooming size */
  int left;        /* Rect co-ordinates */
  int right;
  int top;
  int bottom;


/*FIXME  if (GetForegroundWindow() != hWnd) {
    if (cursorInMainView == TRUE) {
      cursorSetCursor(appInst, TRUE);
      cursorInMainView = FALSE;
    }
    return;
  } else { */
    zoomFactor = 1; //FIXME: windowGetZoomFactor();
    xPos = mouseX;
    yPos = mouseY;
    left = zoomFactor * MAIN_OFFSET_X;
    right = left + (MAIN_SCREEN_SIZE_X * (zoomFactor * TILE_SIZE_X));
    top = zoomFactor * MAIN_OFFSET_Y;
    bottom = top + (MAIN_SCREEN_SIZE_Y * (zoomFactor * TILE_SIZE_Y));
    if (xPos >= left && xPos <= right && yPos >= top && yPos <= bottom) {
      if (cursorInMainView == FALSE) {
        cursorSetCursor(FALSE);
        cursorInMainView = TRUE;
      }
    } else {
      if (cursorInMainView == TRUE) {
        cursorSetCursor(TRUE);
        cursorInMainView = FALSE;
      }
    }
//  }
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
bool cursorPos(int mouseX, int mouseY, BYTE *xValue, BYTE *yValue) {
  static int oldX; /* Cursor old pos */
  static int oldY; 
  int xPos;        /* X and Y positions of the mouse adjusted */ 
  int yPos;        /* to the window */
  BYTE zoomFactor; /* The zooming factor */
  div_t dt;        /* Used for integer division */

  if (cursorInMainView == TRUE) { 
    if (mouseX != oldX || mouseY != oldY) {
      oldX = mouseX;
      oldY = mouseY;
      zoomFactor = 1; //FIXME: windowGetZoomFactor();
      xPos = mouseX;
      yPos = mouseY;
      xPos -= zoomFactor * MAIN_OFFSET_X;
      yPos -= zoomFactor * MAIN_OFFSET_Y;
      dt = div(xPos, (zoomFactor * (MAIN_SCREEN_SIZE_X+1)));
      *xValue = (BYTE) (dt.quot+1);
      dt = div(yPos, (zoomFactor * (MAIN_SCREEN_SIZE_Y+1)));
      *yValue = (BYTE) (dt.quot+1);
    }
  } else {
    *xValue = 0;
    *yValue = 0;
  }
  return cursorInMainView;
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
void cursorAcquireCursor() {
  int xPos;        /* X and Y positions of the mouse adjusted */ 
  int yPos;        /* to the window */
  BYTE zoomFactor; /* The zooming factor */

  return;
  /* FIXME: Is relevent?
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
  } */
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
  cursorSetCursor(TRUE);
}

