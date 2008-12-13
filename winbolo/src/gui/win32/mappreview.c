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
*Name:          map preview
*Filename:      mappreview.c
*Author:        John Morrison
*Creation Date: 2/7/00
*Last Modified: 2/7/00
*Purpose:
*  Map previews in file open dialog box
*********************************************************/


#include <windows.h>
#include "..\mappreview.h"

HDC previewDC = NULL;
HBITMAP hBitmap;

/*********************************************************
*NAME:          mapPreviewSetup
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  Sets up map preview.
*
*ARGUMENTS:
*
*********************************************************/
void mapPreviewSetup() {
}

/*********************************************************
*NAME:          mapPreviewDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  Destroys map preview.
*
*ARGUMENTS:
*
*********************************************************/
void mapPreviewDestroy() {
}  

/*********************************************************
*NAME:          mapPreviewDraw
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  loads the map in filename and draws it in dest if it
*  is a valid map.
*
*ARGUMENTS:
*  fileName - Map to load
*  dest     - Destination device context
*********************************************************/
void mapPreviewDraw(char *fileName, HDC dest) {
  int x; /* Position variables */
  int y;
  HPEN pens[20]; /* Drawing Pens */
  BYTE *mapBuff = NULL;
  BYTE *ptr;
  HBRUSH brush;
  LOGBRUSH lplb;
  


  mapBuff = (BYTE *) malloc(256*256);
  if (mapBuff == NULL) {
    return;
  }
  if (screenGenerateMapPreview(fileName, mapBuff) == FALSE) {
    free(mapBuff);
    return;
  }
  SetCursor(LoadCursor(NULL, IDC_WAIT));  
  /* Draw setup */
  lplb.lbStyle = BS_SOLID;
  lplb.lbColor = RGB(0, 0, 255);
  lplb.lbHatch = HS_VERTICAL;
  brush = CreateBrushIndirect(&lplb);
  pens[0] = CreatePen(PS_SOLID, 0, RGB(179, 121, 15)); /* BUILDING */
  pens[1] = CreatePen(PS_SOLID, 0, RGB(0, 255, 255)); /* RIVER */
  pens[2] = CreatePen(PS_SOLID, 0, RGB(0, 128, 128)); /* SWAMP */
  pens[3] = CreatePen(PS_SOLID, 0, RGB(128, 64, 0)); /* CRATER */
  pens[4] = CreatePen(PS_SOLID, 0, RGB(0, 0, 0)); /* ROAD */
  pens[5] = CreatePen(PS_SOLID, 0, RGB(0, 128, 0)); /* FOREST */
  pens[6] = CreatePen(PS_SOLID, 0, RGB(234, 165, 123)); /* RUBBLE */
  pens[7] = CreatePen(PS_SOLID, 0, RGB(0, 255, 0)); /* GRASS */
  pens[8] = CreatePen(PS_SOLID, 0, RGB(223, 152, 19)); /* SHOT_BUILDING */
  pens[9] = CreatePen(PS_SOLID, 0, RGB(0, 0, 121)); /* BOAT */
  pens[10] = CreatePen(PS_SOLID, 0, RGB(0, 128, 128)); /* SWAMP + MINE */
  pens[11] = CreatePen(PS_SOLID, 0, RGB(128, 64, 0)); /* CRATER_MINE */
  pens[12] = CreatePen(PS_SOLID, 0, RGB(0, 0, 0)); /* ROAD_MINE  */
  pens[13] = CreatePen(PS_SOLID, 0, RGB(0, 128, 0)); /* FOREST_MINE */
  pens[14] = CreatePen(PS_SOLID, 0, RGB(234, 165, 123)); /* RUBBLE_MINE */
  pens[15] = CreatePen(PS_SOLID, 0, RGB(0, 255, 0)); /* GrassMine */
  pens[16] = CreatePen(PS_SOLID, 0, RGB(0, 0, 255)); /* DEEP_SEA */
  pens[17] = CreatePen(PS_SOLID, 0, RGB(255, 0, 0)); /* PILLBOX */
  pens[18] = CreatePen(PS_SOLID, 0, RGB(255, 255, 0)); /* BASE */
  pens[19] = CreatePen(PS_SOLID, 0, RGB(128, 128, 128)); /* START */

  /* Draw */
  x = 0;
  y = 0;
  ptr = mapBuff;
  SelectObject(dest, brush);
  Rectangle(dest, 0, 0, 255, 255);
  while (y < 255) {
    while (x < 255 && ptr) {
      if (*ptr == 16) {
        MoveToEx(dest, x+1, y, NULL);
      } else {
        SelectObject(dest, pens[*ptr]);
        LineTo(dest, x+1, y);
      }
      ptr++;
      x++;
    }
    x = 0;
    y++;
    MoveToEx(dest, 0, y, NULL);
  }

  /* Clean up */
  if (mapBuff) {
    free(mapBuff);
  }
  x = 0;
  while (x < 20) {
    DeleteObject(pens[x]);
    x++;
  }
  DeleteObject(brush);
  SetCursor(LoadCursor(NULL, IDC_ARROW));
}
