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
*Name:          Font
*Filename:      font.h
*Author:        John Morrison
*Creation Date:  3/1/99
*Last Modified: 19/6/00
*Purpose:
*  Create/Select Font Routines
*********************************************************/

#ifndef _FONT_H
#define _FONT_H

#include <windows.h>
#include "global.h"


/* Font Name */
#define FONT_NAME "Courier New"

/* Size of the font */
#define FONT_HEIGHT 10 /* 8 */
#define FONT_HEIGHT_NORMAL_ZOOM 8
#define TINY_FONT_HEIGHT 5

/*********************************************************
*NAME:          fontCreate
*AUTHOR:        John Morrison
*CREATION DATE: 3/1/99
*LAST MODIFIED: 3/1/99
*PURPOSE:
*  Loads Font
*
*ARGUMENTS:
* appInst - Handle to the application (Required to 
*           load bitmaps from resources)
* appWnd  - Main Window Handle (Required for clipper)
*********************************************************/
bool fontSetup(HINSTANCE appInst, HWND appWnd);

/*********************************************************
*NAME:          fontCleanup
*AUTHOR:        John Morrison
*CREATION DATE: 3/1/99
*LAST MODIFIED: 3/1/99
*PURPOSE:
*  Destroys font
*
*ARGUMENTS:
*
*********************************************************/
void fontCleanup(void);

/*********************************************************
*NAME:          fontSelect
*AUTHOR:        John Morrison
*CREATION DATE: 3/1/99
*LAST MODIFIED: 3/1/99
*PURPOSE:
*  Selects the font into the DC
*
*ARGUMENTS:
* hDC - The Decive context to select into
*********************************************************/
void fontSelect(HDC hDC);

/*********************************************************
*NAME:          fontSelectTiny
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
*  Selects the tiny font into the DC
*
*ARGUMENTS:
* hDC - The Decive context to select into
*********************************************************/
void fontSelectTiny(HDC hDC);

/*********************************************************
*NAME:          fontPointToHeight
*AUTHOR:        John Morrison
*CREATION DATE: 13/9/98
*LAST MODIFIED: 13/9/98
*PURPOSE:
*  Turns a font height from points to logical Height
*  (Used by createFont)
*
*ARGUMENTS:
*  pntSize   - Size of Font in Points
*  hWindowDC - Handle to the Window
*********************************************************/
int fontPointToHeight(int pntSize, HDC hWindowDC);

#endif /* _FONT_H */


