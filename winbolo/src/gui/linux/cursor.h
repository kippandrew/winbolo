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

#ifndef _CURSOR_H
#define _CURSOR_H

#include <stdlib.h>
#include "../../bolo/global.h"

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
bool cursorSetup(); 

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
void cursorCleanup(); 

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
void cursorSetCursor(bool normalCurs); 

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
void cursorMove(int mouseX, int mouseY); 

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
bool cursorPos(int mouseX, int mouseY, BYTE *xValue, BYTE *yValue); 

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
void cursorAcquireCursor(); 

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
void cursorLeaveWindow(void); 

#endif /* _CURSOR_H */
