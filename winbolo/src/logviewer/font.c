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
*Filename:      font.c
*Author:        John Morrison
*Creation Date:  3/1/99
*Last Modified: 19/6/00
*Purpose:
*  Create/Select/Destroy Font Routines
*********************************************************/

#include <windows.h>
#include "global.h"
#include "font.h"

HFONT hBoloFont = NULL; /* The Bolo Font */
HFONT hTinyFont = NULL; /* Pillbox status stuff */

/*********************************************************
*NAME:          fontCreate
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/997
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Loads Font
*
*ARGUMENTS:
* appInst - Handle to the application (Required to 
*           load bitmaps from resources)
* appWnd  - Main Window Handle (Required for clipper)
*********************************************************/
bool fontSetup(HINSTANCE appInst, HWND appWnd) {
  bool returnValue;  /* Value to return */
  int nHeight;       /* Height to make the font */
  HDC tempDC;        /* Temp DC */
  BYTE zoomFactor;   /* Scaling factor */
  int height;        /* Font height */

  returnValue = TRUE;
  zoomFactor = 1;
  if (zoomFactor == 1) {
    height = FONT_HEIGHT_NORMAL_ZOOM;
  } else {
    height = zoomFactor * FONT_HEIGHT;
  }
  tempDC = GetDC(appWnd);

  if (tempDC != NULL) {
    nHeight = fontPointToHeight(height, tempDC);
    hBoloFont = CreateFont(nHeight, 0, 0, 0, FALSE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FONT_NAME);
    DeleteDC(tempDC);
  }

  if (hBoloFont == NULL) {
    MessageBox(appWnd, "Can't Load Font", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    returnValue = FALSE;
  }

  tempDC = GetDC(appWnd);

  if (tempDC != NULL) {
    nHeight = fontPointToHeight((zoomFactor * TINY_FONT_HEIGHT), tempDC);
    hTinyFont = CreateFont(nHeight, 0, 0, 0, FALSE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FONT_NAME);
    DeleteDC(tempDC);
  }

  
  return returnValue;
}

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
void fontCleanup(void) {
  if (hTinyFont != NULL) {
    DeleteObject(hTinyFont);
    hTinyFont = NULL;
  }
  if (hBoloFont != NULL) {
    DeleteObject(hBoloFont);
    hBoloFont = NULL;
  }
}



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
void fontSelect(HDC hDC) {   
  SelectObject(hDC,hBoloFont);
}

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
void fontSelectTiny(HDC hDC) {
  SelectObject(hDC, hTinyFont);
}


/*********************************************************
*NAME:          fontPointToHeight
*AUTHOR:        John Morrison
*CREATION DATE: 13/9/98
*LAST MODIFIED: 13/9/98
*PURPOSE:
*  Turns a font height from points to logical Height
*
*ARGUMENTS:
*  pntSize   - Size of Font in Points
*  hWindowDC - Handle to the Window
*********************************************************/
int fontPointToHeight(int pntSize, HDC hWindowDC) {
  int returnValue; /* Value to Return */

  returnValue = -MulDiv(pntSize, GetDeviceCaps(hWindowDC, LOGPIXELSY), 72);
  return returnValue;
}
