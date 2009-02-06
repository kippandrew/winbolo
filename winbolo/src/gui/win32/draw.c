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
*Name:          Draw
*Filename:      draw.c
*Author:        John Morrison
*Creation Date: 13/12/98
*Last Modified: 26/01/02
*Purpose:
*  System Specific Drawing routines (Uses Direct Draw)
*********************************************************/

#include <math.h>
#include <windows.h>
#include <string.h>
#include <ddraw.h>
#include "..\..\bolo\global.h"
#include "..\..\bolo\backend.h"
#include "..\..\bolo\network.h"
#include "..\..\bolo\screenlgm.h"
#include "..\tiles.h"
#include "..\..\bolo\tilenum.h"
#include "..\positions.h"
#include "..\winbolo.h"
#include "..\resource.h"
#include "..\cursor.h"
#include "..\font.h"
#include "..\clientmutex.h"
#include "..\lang.h"
#include "..\winutil.h"
#include "..\draw.h"
#include "..\skins.h"

/* Direct Draw Surfaces */
LPDIRECTDRAW lpDD = NULL;
LPDIRECTDRAWSURFACE lpDDSPrimary = NULL;
LPDIRECTDRAWSURFACE lpDDSBackBuffer = NULL;
LPDIRECTDRAWSURFACE lpDDSTiles = NULL;
LPDIRECTDRAWSURFACE lpDDSMessages = NULL;
LPDIRECTDRAWSURFACE lpDDSManStatus = NULL;
LPDIRECTDRAWSURFACE lpDDSPillsStatus = NULL;
LPDIRECTDRAWSURFACE lpDDSBasesStatus = NULL;
LPDIRECTDRAWSURFACE lpDDSTankStatus = NULL;
LPDIRECTDRAWSURFACE lpDDSTankLabels = NULL;
LPDIRECTDRAWCLIPPER lpDDClipper = NULL;

/* Used for drawing the man status */
HBRUSH hManBrush = NULL;
HBRUSH hNarrowBrush = NULL;
HPEN hManPen = NULL;

/* Used for storing time */
DWORD	g_dwFrameTime = 0;
/* Number of frames dispayed this second */
DWORD	g_dwFrameCount = 0;
/* The total frames per second for last second */
DWORD g_dwFrameTotal = 0;


int drawPosX[255];
int drawPosY[255];
int drawPlayerLens[MAX_TANKS][3];

/*********************************************************
*NAME:          drawSetup
*AUTHOR:        John Morrison
*CREATION DATE: 13/10/98
*LAST MODIFIED:  29/4/00
*PURPOSE:
*  Sets up drawing systems, direct draw structures etc.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
* appInst - Handle to the application (Required to 
*           load bitmaps from resources)
* appWnd  - Main Window Handle (Required for clipper)
*********************************************************/
bool drawSetup(HINSTANCE appInst, HWND appWnd) {
  bool returnValue;       /* Value to return */
  BYTE zoomFactor;        /* scaling factor */
  HRESULT res;            /* Direct Draw Function returns */
  DDSURFACEDESC primDesc; /* Surface description */
  HBITMAP hTiles = NULL;  /* The tile file bitmap resource */
  HDC hTilesDC = NULL;    /* The tile file resource DC */
  HDC hDDSTilesDC = NULL; /* Temp DC of a DDS use to copy tile file into */
  DDPIXELFORMAT ddpf;     /* DD Pixel Format to get green */
  DDBLTFX fx;             /* FX for the colour fills */
  LOGBRUSH lb;            /* Used in creation of the brush */
  RECT src;               /* Used for copying the bases & pills icon in */
  char fileName[MAX_PATH]; /* Used to load the external files */
  RECT dest;              /* Used for copying the bases & pills icon in */
  BYTE count;
  bool usingDoubleSkin;    /* Are we using the double sized skin */

  /* Palette Checking */
  DDSURFACEDESC   ddsd;
  hTiles = NULL;
  usingDoubleSkin = FALSE;

  /* Storing player name lengths */
  count = 0;
  while (count < MAX_TANKS) {
    drawPlayerLens[count][1] = -1;
    count++;
  }

  returnValue = TRUE;
  zoomFactor = windowGetZoomFactor();

  /* Create the Direct Draw Object */
  
  res = DirectDrawCreate(NULL, &lpDD, NULL);
  if (FAILED(res)) {
    returnValue = FALSE;
    MessageBoxA(NULL, langGetText(STR_DRAWERROR_CREATEOBJECT), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
  }
  
  /* Set the co-op level */
  if (returnValue == TRUE) {
    res = lpDD->lpVtbl->SetCooperativeLevel(lpDD,NULL,DDSCL_NORMAL);
    if (FAILED(res)) {
      MessageBoxA(NULL, langGetText(STR_DRAWERROR_SETCOOPLEVEL), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }

  /* Create the primary surface */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS;
    primDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSPrimary, NULL);
    if (FAILED(res)) {
      MessageBoxA(NULL, langGetText(STR_DRAWERROR_CREATEPRIMARY), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }
  
  /* Get the pixel format */
  ddpf.dwSize = sizeof(ddpf);
  res = lpDDSPrimary->lpVtbl->GetPixelFormat(lpDDSPrimary, &ddpf);
  if (FAILED(res)) {
    MessageBoxA(NULL, langGetText(STR_DRAWERROR_GETPIXELFORMAT), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    returnValue = FALSE;
  }

  /* Palette Checking Stuff */
  ZeroMemory( &ddsd, sizeof( ddsd ) );
  ddsd.dwSize = sizeof( ddsd );
  if (FAILED(lpDDSPrimary->lpVtbl->GetSurfaceDesc(lpDDSPrimary, &ddsd))) {
    returnValue = FALSE;
    MessageBoxA(NULL, langGetText(STR_DRAWERROR_GETDESC), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
  }
  
  if (( ddsd.ddpfPixelFormat.dwRGBBitCount < 16 )) { /* FIXME: Was also (ddsd.ddpfPixelFormat.dwFlags != DDPF_RGB) || but taken out as it stops matrox G4's from working in 32bit colour mode */
    returnValue = FALSE;
    MessageBoxA(NULL, langGetText(STR_DRAWERROR_TOOFEWCOLOURS), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
  }

  /* End Palette Checking Stuff */



  /* Create the back buffer surface */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    primDesc.dwWidth = zoomFactor * MAIN_BACK_BUFFER_SIZE_X * TILE_SIZE_X;
    primDesc.dwHeight = zoomFactor * MAIN_BACK_BUFFER_SIZE_Y * TILE_SIZE_Y;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSBackBuffer, NULL);
    if (FAILED(res)) {
      MessageBoxA(NULL, langGetText(STR_DRAWERROR_BUFFERCREATE), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }

  /* Create the tile buffer and copy the bitmap into it */
  if (returnValue == TRUE) {
    /* Create the buffer */
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
    primDesc.ddckCKSrcBlt.dwColorSpaceLowValue = ddpf.dwGBitMask;
    primDesc.ddckCKSrcBlt.dwColorSpaceHighValue = ddpf.dwGBitMask;
    primDesc.dwWidth = zoomFactor * TILE_FILE_X;
    primDesc.dwHeight = zoomFactor * TILE_FILE_Y;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSTiles, NULL);
    if (FAILED(res)) {
      MessageBoxA(NULL, "Creating DD Tile buffer and copying resource into it Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Copy the bitmap into it */
      hTiles = NULL;
      /* Try to load from skins file First first */
      if (skinsIsLoaded() == TRUE) {
        /* Try double size skin file */
        if (zoomFactor == 2) {
          skinsGetSkinDirectory(fileName);
          if (fileName[strlen(fileName)-1] != '\\') {
            strcat(fileName, "\\");
          }
          strcat(fileName, DRAW_SKINS_TILESFILE32);
          hTiles = (HBITMAP) LoadImage(NULL, fileName, IMAGE_BITMAP, 2 * TILE_FILE_X , 2 * TILE_FILE_Y, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
          if (hTiles != NULL) {
            usingDoubleSkin = TRUE;
          }
        }
        if (hTiles == NULL) {
          /* Try single size skin file */
          skinsGetSkinDirectory(fileName);
          if (fileName[strlen(fileName)-1] != '\\') {
            strcat(fileName, "\\");
          }
          strcat(fileName, DRAW_SKINS_TILESFILE);
          hTiles = (HBITMAP) LoadImage(NULL, fileName, IMAGE_BITMAP, TILE_FILE_X , TILE_FILE_Y, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        }
      } 
      if (hTiles == NULL) {
        hTiles = (HBITMAP) LoadImage(appInst, MAKEINTRESOURCE(IDB_TILE), IMAGE_BITMAP, TILE_FILE_X , TILE_FILE_Y, LR_CREATEDIBSECTION);
      }
      if (hTiles !=NULL) {
        hTilesDC = CreateCompatibleDC(NULL);
        SelectObject(hTilesDC, hTiles);
        lpDDSTiles->lpVtbl->Restore(lpDDSTiles);
        res = lpDDSTiles->lpVtbl->GetDC(lpDDSTiles,&hDDSTilesDC);
        if (res != DD_OK) {
          MessageBoxA(NULL, langGetText(STR_DRAWERROR_GETDCFAILED), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
        }
        if (usingDoubleSkin == FALSE) {
          StretchBlt(hDDSTilesDC, 0, 0, zoomFactor * TILE_FILE_X, zoomFactor * TILE_FILE_Y, hTilesDC, 0, 0, TILE_FILE_X, TILE_FILE_Y, SRCCOPY);
        } else {
          BitBlt(hDDSTilesDC, 0, 0, zoomFactor * TILE_FILE_X, zoomFactor * TILE_FILE_Y, hTilesDC, 0, 0, SRCCOPY);
        }
        res = lpDDSTiles->lpVtbl->ReleaseDC(lpDDSTiles, hDDSTilesDC);
        DeleteDC(hTilesDC);
        DeleteObject(hTiles);
      } else {
        MessageBoxA(NULL, "Creating DD Tile buffer and copying resource into it Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
        returnValue = FALSE;
      }
    }
  }

  /* Create the tank label back buffer */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
    primDesc.ddckCKSrcBlt.dwColorSpaceLowValue = ddpf.dwGBitMask;
    primDesc.ddckCKSrcBlt.dwColorSpaceHighValue = ddpf.dwGBitMask;
    primDesc.dwWidth = zoomFactor * MAIN_BACK_BUFFER_SIZE_X * TILE_SIZE_X; //zoomFactor * TANK_LABEL_WIDTH;
    primDesc.dwHeight = zoomFactor * (2* TANK_LABEL_HEIGHT - 20);
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSTankLabels, NULL);
    if (FAILED(res)) {
      returnValue = FALSE;
      MessageBoxA(NULL, "Creating DD Tank Label Back buffer Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    }
  }

  /* Create the Messages Buffer */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    primDesc.dwWidth = (zoomFactor * MESSAGE_WIDTH);
    primDesc.dwHeight = (zoomFactor * MESSAGE_TOTALHEIGHT);
    if (zoomFactor == ZOOM_FACTOR_NORMAL) {
      primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    } else {
      primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    }
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSMessages, NULL);
    if (res == DDERR_OUTOFMEMORY ) {
      MessageBoxA(NULL, "Creating No Mem", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else if (res ==DDERR_OUTOFVIDEOMEMORY) {
      MessageBoxA(NULL, "Creating No Vide Mem", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else if (res == DDERR_INVALIDPARAMS ) {
      MessageBoxA(NULL, "Bad Parms", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else if (FAILED(res)) {
      MessageBoxA(NULL, "Creating DD Messages Back buffer Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Fill the surface black */
      ZeroMemory(&fx, sizeof(fx));
      fx.dwSize = sizeof(fx);
      fx.dwFillColor =  0;
      lpDDSMessages->lpVtbl->Blt(lpDDSMessages, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
    }
  }

  /* Create the Man status window */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    primDesc.dwWidth = zoomFactor * MAN_STATUS_WIDTH;
    primDesc.dwHeight = zoomFactor * MAN_STATUS_HEIGHT;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSManStatus, NULL);
    if (FAILED(res)) {
      MessageBoxA(NULL, "Creating DD LGM Status Back buffer Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Fill the surface black */
      ZeroMemory(&fx, sizeof(fx));
      fx.dwSize = sizeof(fx);
      fx.dwFillColor =  0;
      lpDDSManStatus->lpVtbl->Blt(lpDDSManStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
    }
  }

  /* Create the Base status window */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    primDesc.dwWidth = zoomFactor * STATUS_BASES_WIDTH;
    primDesc.dwHeight = zoomFactor * STATUS_BASES_HEIGHT;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSBasesStatus, NULL);
    if (FAILED(res)) {
      MessageBoxA(NULL, "Creating DD Bases Status Back buffer Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Fill the surface black */
      ZeroMemory(&fx, sizeof(fx));
      fx.dwSize = sizeof(fx);
      fx.dwFillColor =  0;
      lpDDSBasesStatus->lpVtbl->Blt(lpDDSBasesStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
      /* Copy the icon in */
      src.left = zoomFactor * BASE_GOOD_X;
      src.top = zoomFactor * BASE_GOOD_Y;
      src.right = zoomFactor * BASE_GOOD_X + zoomFactor * TILE_SIZE_X;
      src.bottom = zoomFactor * BASE_GOOD_Y + zoomFactor * TILE_SIZE_Y;
      dest.top = (zoomFactor * STATUS_BASES_MIDDLE_ICON_Y);
      dest.left = (zoomFactor * STATUS_BASES_MIDDLE_ICON_X);
      dest.right = dest.left + (zoomFactor * TILE_SIZE_X);
      dest.bottom = dest.top + (zoomFactor * TILE_SIZE_Y);
      lpDDSBasesStatus->lpVtbl->Blt(lpDDSBasesStatus, &dest, lpDDSTiles, &src, DDBLT_WAIT, NULL);
    }
  }

  /* Create the Pillboxes status window */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    primDesc.dwWidth = zoomFactor * STATUS_PILLS_WIDTH;
    primDesc.dwHeight = zoomFactor * STATUS_PILLS_HEIGHT;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSPillsStatus, NULL);
    if (FAILED(res)) {
      MessageBoxA(NULL, "Creating DD Tanks Status Back buffer Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Fill the surface black */
      ZeroMemory(&fx, sizeof(fx));
      fx.dwSize = sizeof(fx);
      fx.dwFillColor =  0;
      lpDDSPillsStatus->lpVtbl->Blt(lpDDSPillsStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
      /* Copy the icon in */
      src.left = zoomFactor * PILL_GOOD15_X;
      src.top = zoomFactor * PILL_GOOD15_Y;
      src.right = zoomFactor * PILL_GOOD15_X + zoomFactor * TILE_SIZE_X;
      src.bottom = zoomFactor * PILL_GOOD15_Y + zoomFactor * TILE_SIZE_Y;
      dest.top = (zoomFactor * STATUS_PILLS_MIDDLE_ICON_Y);
      dest.left = (zoomFactor * STATUS_PILLS_MIDDLE_ICON_X);
      dest.right = dest.left + (zoomFactor * TILE_SIZE_X);
      dest.bottom = dest.top + (zoomFactor * TILE_SIZE_Y);
      lpDDSPillsStatus->lpVtbl->Blt(lpDDSPillsStatus, &dest, lpDDSTiles, &src, DDBLT_WAIT, NULL);

    }
  }

  /* Create the Tanks status window */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    primDesc.dwWidth = zoomFactor * STATUS_TANKS_WIDTH;
    primDesc.dwHeight = zoomFactor * STATUS_TANKS_HEIGHT;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSTankStatus, NULL);
    if (FAILED(res)) {
      MessageBoxA(NULL, "Creating DD Pills Status Back buffer Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Fill the surface black */
      ZeroMemory(&fx, sizeof(fx));
      fx.dwSize = sizeof(fx);
      fx.dwFillColor =  0;
      lpDDSTankStatus->lpVtbl->Blt(lpDDSTankStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
      /* Copy the icon in */
      src.left = zoomFactor * TANK_SELF_0_X;
      src.top = zoomFactor * TANK_SELF_0_Y;
      src.right = zoomFactor * TANK_SELF_0_X + zoomFactor * TILE_SIZE_X;
      src.bottom = zoomFactor * TANK_SELF_0_Y + zoomFactor * TILE_SIZE_Y;
      dest.top = (zoomFactor * STATUS_TANKS_MIDDLE_ICON_Y);
      dest.left = (zoomFactor * STATUS_TANKS_MIDDLE_ICON_X);
      dest.right = dest.left + (zoomFactor * TILE_SIZE_X);
      dest.bottom = dest.top + (zoomFactor * TILE_SIZE_Y);
      lpDDSTankStatus->lpVtbl->Blt(lpDDSTankStatus, &dest, lpDDSTiles, &src, DDBLT_WAIT | DDBLT_KEYSRC, NULL);

    }
  }

  /* Create the clipper */
  if (returnValue == TRUE) {
    res = lpDD->lpVtbl->CreateClipper(lpDD, 0, &lpDDClipper, 0);
    if (FAILED(res)) {
      MessageBoxA(NULL, langGetText(STR_DRAWERROR_CLIPPERFAILED), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }

  /* Set the clipper to the window */
  if (returnValue == TRUE) {
    res = lpDDClipper->lpVtbl->SetHWnd(lpDDClipper, 0, appWnd);
    if (FAILED(res)) {
      MessageBoxA(NULL, langGetText(STR_DRAWERROR_CLIPPERFAILED), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }

  /* Set the Primary Surface to use the clipper */
  if (returnValue == TRUE) {
    res = lpDDSPrimary->lpVtbl->SetClipper(lpDDSPrimary, lpDDClipper);
    if (FAILED(res)) {
      MessageBoxA(NULL, "Associating DD Clipper with surface Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
      lpDDClipper = NULL;
    }
  }

  /* Release the clipper */
  if (lpDDClipper != NULL) {  
    lpDDClipper->lpVtbl->Release(lpDDClipper);
    lpDDClipper = NULL;
  }

  /* Create the Brush and Pen */
  lb.lbStyle = BS_SOLID;
  lb.lbColor = 0x00FFFFFF;
  lb.lbHatch = HS_CROSS;

  hManBrush = CreateBrushIndirect(&lb);
  if (hManBrush == NULL) {
    MessageBoxA(NULL, langGetText(STR_DRAWERROR_BRUSH), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    returnValue = FALSE;
  }
  lb.lbStyle = BS_HOLLOW;
  lb.lbColor = 0x00FFFFFF;
  lb.lbHatch = HS_CROSS;

  hNarrowBrush = CreateBrushIndirect(&lb);
  if (hManBrush == NULL) {
    MessageBoxA(NULL, langGetText(STR_DRAWERROR_BRUSH), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    returnValue = FALSE;
  }

  hManPen = CreatePen(PS_SOLID, 0, 0x00FFFFFF);
  if (hManPen == NULL) {
    MessageBoxA(NULL, langGetText(STR_DRAWERROR_PEN), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    returnValue = FALSE;
  }

  drawSetupArrays(zoomFactor);
  return returnValue;
}

/*********************************************************
*NAME:          drawCleanup
*AUTHOR:        John Morrison
*CREATION DATE: 13/12/98
*LAST MODIFIED: 13/2/98
*PURPOSE:
*  Destroys and cleans up drawing systems, direct draw 
*  structures etc.
*
*ARGUMENTS:
*
*********************************************************/
void drawCleanup(void) {

  if (hManPen != NULL) {
    DeleteObject(hManPen);
    hManPen = NULL;
  }  
  if (hNarrowBrush != NULL) {
    DeleteObject(hNarrowBrush);
    hNarrowBrush = NULL;
  }
  if (hManBrush != NULL) {
    DeleteObject(hManBrush);
    hManBrush = NULL;
  }
  if (lpDDSTankStatus != NULL) {
    lpDDSTankStatus->lpVtbl->Release(lpDDSTankStatus);
    lpDDSTankStatus = NULL;
  }
  if (lpDDSTankLabels != NULL) {
    lpDDSTankLabels->lpVtbl->Release(lpDDSTankLabels);
    lpDDSTankLabels = NULL;
  }
  if (lpDDSBasesStatus != NULL) {
    lpDDSBasesStatus->lpVtbl->Release(lpDDSBasesStatus);
    lpDDSBasesStatus = NULL;
  }
  if (lpDDSPillsStatus != NULL) {
    lpDDSPillsStatus->lpVtbl->Release(lpDDSPillsStatus);
    lpDDSPillsStatus = NULL;
  }
  if (lpDDSManStatus != NULL) {
    lpDDSManStatus->lpVtbl->Release(lpDDSManStatus);
    lpDDSManStatus = NULL;
  }
  if (lpDDSMessages != NULL) {
    lpDDSMessages->lpVtbl->Release(lpDDSMessages);
    lpDDSMessages = NULL;
  }
  if (lpDDSTiles != NULL) {
    lpDDSTiles->lpVtbl->Release(lpDDSTiles);
    lpDDSTiles = NULL;
  }
  if (lpDDSBackBuffer != NULL) {
    lpDDSBackBuffer->lpVtbl->Release(lpDDSBackBuffer);
    lpDDSBackBuffer = NULL;
  }
  if (lpDDSPrimary != NULL) {
    lpDDSPrimary->lpVtbl->Release(lpDDSPrimary);
    lpDDSPrimary = NULL;
  }
  if (lpDD != NULL) {
    lpDD->lpVtbl->Release(lpDD);
    lpDD = NULL;
  }
}

/*********************************************************
*NAME:          drawPillInView
*AUTHOR:        John Morrison
*CREATION DATE:  3/2/98
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Draws the "Pillbox View" label
*
*ARGUMENTS:
*
*********************************************************/
void drawNetFailed() {
  HDC hDC = NULL; /* Temp DC of the surface */
  RECT textRect;  /* Defines the text rectangle */
  int x;          /* X And Y Locations on the back buffer to do the drawing */
  int y;
  char *text;     /* Used in language getting */
  int textLen;    /* Length of the text */
  BYTE zoomFactor;

  zoomFactor = windowGetZoomFactor();
  textRect.left = 0;
  textRect.right = TANK_LABEL_WIDTH;
  textRect.top = 0;
  textRect.bottom = TANK_LABEL_HEIGHT;

  if (SUCCEEDED(lpDDSTankLabels->lpVtbl->GetDC(lpDDSTankLabels, &hDC))) {
    /* Draw it on the back buffer */
    fontSelect(hDC);
    SetBkColor(hDC, RGB(0,255,0));
    SetTextColor(hDC, RGB(255, 255, 255));
    text = "Network Failed - Resyncing";
    textLen = (int) strlen(text);
    DrawTextA(hDC, text, textLen, &textRect, (DT_LEFT | DT_NOCLIP | DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE));
    DrawTextA(hDC, text, textLen, &textRect, (DT_LEFT | DT_NOCLIP | DT_NOPREFIX | DT_SINGLELINE));
    if (FAILED(lpDDSTankLabels->lpVtbl->ReleaseDC(lpDDSTankLabels, hDC))) {
      MessageBoxA(NULL, "This should never happen - Error Releasing DC for drawing Pillbox view", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    }
    x = zoomFactor * 3 * TILE_SIZE_X;
    y = zoomFactor * 8 * TILE_SIZE_Y;
    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTankLabels, &textRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
  }
}

/*********************************************************
*NAME:          drawMainScreen
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 27/05/00
*PURPOSE:
*  Updates the Main Window View
*
*ARGUMENTS:
*  value    - Pointer to the sceen structure
*  mineView - Pointer to the screen mines structure
*  tks      - Pointer to the screen tank structure
*  gs       - Pointer to the screen gunsight structure
*  sBullets - The screen Bullets structure
*  lgms     - Screen Builder structure 
*  rcWindow - Window region
*  showPillLabels - Show the pillbox labels?
*  showBaseLabels - Show the base labels?
*  srtDelay       - The start delay in ticks.
*                  If greater then 0 should draw countdown
*  isPillView     - TRUE if we are in pillbox view
*  edgeX          - Edge X offset for smooth scrolling
*  edgeY          - Edge Y offset for smooth scrolling
*  useCursor      - True if to draw the cursor
*  cursorLeft     - Cursor left position
*  cursorTop      - Cursor Top position
*  tank           - Pointer to the player's tank structure
*********************************************************/
void drawMainScreen(screen *value, screenMines *mineView, screenTanks *tks, screenGunsight *gs, screenBullets *sBullets, screenLgm *lgms, RECT *rcWindow, bool showPillLabels, bool showBaseLabels, long srtDelay, bool isPillView, int edgeX, int edgeY, bool useCursor, BYTE cursorLeft, BYTE cursorTop, tank *tank) {
  RECT output;     /* Output Rectangle */
  bool done;       /* Finished Looping */
  int count;       /* Looping Variable */
  BYTE x;          /* X and Y co-ordinates */
  BYTE y;
  int outputX;     /* X and Y co-ordinates in the tile image */
  int outputY;
  BYTE pos;        /* Current position */
  BYTE zoomFactor; /* Scaling factor */
  char str[255];   /* Frame Rate Counting Stuff */
  DWORD time;
  bool isPill;     /* Is the square a pill */
  bool isBase;     /* Is the square a base */
  BYTE labelNum;   /* Returns the label number */
  HDC hDC;         /* Temp DC of the surface */
  static int staticOffset = 0; /* There is a row of static squares in tiles.bmp that we'll cycle through */
  static int staticCount = 0;  /* We want to change the static square that is displayed every N ticks */
  static int staticLast = 1000;  /* Static last is used to make sure static only updates once per tick. Set to arbitary value to be overridden on first use */

  if (lpDDSPrimary->lpVtbl->IsLost(lpDDSPrimary) == DDERR_SURFACELOST) {
    drawRestore();
  }
  if (lpDDSBackBuffer->lpVtbl->IsLost(lpDDSBackBuffer) == DDERR_SURFACELOST) {
    lpDDSBackBuffer->lpVtbl->Restore(lpDDSBackBuffer);
  }

  /* Waiting for game to start. Draw coutdown */
  if (srtDelay > 0) { 
    drawStartDelay(rcWindow, srtDelay);
  } else if (tankGetDeathWait(tank) != 0 && ((tankGetLastTankDeath(tank) == LAST_DEATH_BY_DEEPSEA && tankGetDeathWait(tank) < STATIC_ON_TICKS_DEEPSEA) || (tankGetLastTankDeath(tank) == LAST_DEATH_BY_SHELL && tankGetDeathWait(tank) < STATIC_ON_TICKS_SHELL))) {
    /* Tank died and is waiting to respawn, throw some static on the screen */ 
    if (tankGetDeathWait(tank) != staticLast) {
      staticLast = (*tank)->deathWait;
      for (x = 1; x < 16; x++) {
        for (y = 1; y < 16; y++) {
          staticCount = rand() % STATIC_CHANGE_TICKS;
          zoomFactor = windowGetZoomFactor();        
          output.left = zoomFactor * (STATIC_X + staticOffset);
          output.top = zoomFactor * STATIC_Y;
          output.right = zoomFactor * (STATIC_X + staticOffset) + zoomFactor * TILE_SIZE_X;
          output.bottom = zoomFactor * STATIC_Y + zoomFactor * TILE_SIZE_Y;
          lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, (zoomFactor * y * TILE_SIZE_X), (zoomFactor * x * TILE_SIZE_Y), lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
		}
	  }
      if (staticCount % STATIC_CHANGE_TICKS == 0) {
        staticOffset += TILE_SIZE_X;
        if (staticOffset >= TILE_FILE_X) {
          staticOffset = 0;
		}
        staticCount = 0;
	  }
      staticCount++;

      /* Copy the back buffer to the window */
	  output.left = (zoomFactor * TILE_SIZE_X) + edgeX;
	  output.top = (zoomFactor * TILE_SIZE_Y) + edgeY;
	  output.right = (zoomFactor * MAIN_SCREEN_SIZE_X * TILE_SIZE_X) + (zoomFactor * TILE_SIZE_X) + edgeX;
	  output.bottom = (zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y) + (zoomFactor * TILE_SIZE_Y) + edgeY;
	  rcWindow->left = rcWindow->left + (zoomFactor * MAIN_OFFSET_X);
	  rcWindow->top = rcWindow->top + (zoomFactor * MAIN_OFFSET_Y);
	  rcWindow->right = rcWindow->left + (MAIN_SCREEN_SIZE_X * (zoomFactor * TILE_SIZE_X));
	  rcWindow->bottom = rcWindow->top + (zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y);
	  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, rcWindow, lpDDSBackBuffer, &output, DDBLT_WAIT, NULL);
	}
  } else {
  count = 0;
  x = 0;
  y = 0;
  done = FALSE;
  zoomFactor = windowGetZoomFactor();
  str[0] = '\0';

  
  while (done == FALSE) {
    pos = screenGetPos(value,x,y);
    isPill = FALSE;
    isBase = FALSE;
    outputX = drawPosX[pos];
    outputY = drawPosY[pos];
    if (pos == PILL_EVIL_15 || pos == PILL_EVIL_14 || pos == PILL_EVIL_13 || pos == PILL_EVIL_12 || pos == PILL_EVIL_11 || pos == PILL_EVIL_10 || pos == PILL_EVIL_9 || pos == PILL_EVIL_8 || pos == PILL_EVIL_7 || pos == PILL_EVIL_6 || pos == PILL_EVIL_5 || pos == PILL_EVIL_4 || pos == PILL_EVIL_3 || pos == PILL_EVIL_2 || pos == PILL_EVIL_1 || pos == PILL_EVIL_0) {
      isPill = TRUE;
    }
    if (pos == PILL_GOOD_15 || pos == PILL_GOOD_14 || pos == PILL_GOOD_13 || pos == PILL_GOOD_12 || pos == PILL_GOOD_11 || pos == PILL_GOOD_10 || pos == PILL_GOOD_9 || pos == PILL_GOOD_8 || pos == PILL_GOOD_7 || pos == PILL_GOOD_6 || pos == PILL_GOOD_5 || pos == PILL_GOOD_4 || pos == PILL_GOOD_3 || pos == PILL_GOOD_2 || pos == PILL_GOOD_1 || pos == PILL_GOOD_0) {
      isPill = TRUE;
    }
    if (pos == BASE_GOOD || pos == BASE_NEUTRAL || pos == BASE_EVIL) {
      isBase = TRUE;
    }

    /* Drawing */

    /* Draw the map block */
    output.left = outputX;
    output.right = outputX + zoomFactor * TILE_SIZE_X;
    output.top = outputY;
    output.bottom = outputY + zoomFactor * TILE_SIZE_Y;
    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, zoomFactor * (x * TILE_SIZE_X), (zoomFactor * y * TILE_SIZE_Y), lpDDSTiles ,&output, DDBLTFAST_WAIT);

    /* Draw Mines */
    if ((screenIsMine(mineView,x,y)) == TRUE) { 
      output.left = zoomFactor * MINE_X;
      output.top = zoomFactor * MINE_Y;
      output.right = zoomFactor * MINE_X + zoomFactor * TILE_SIZE_X;
      output.bottom = zoomFactor * MINE_Y + zoomFactor * TILE_SIZE_Y;
      lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, (zoomFactor * x * TILE_SIZE_X) , (zoomFactor * y * TILE_SIZE_Y), lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
    }

	
	/* Draw the pillNumber or base Number if required */
    if (isPill == TRUE && showPillLabels == TRUE) {
      labelNum = screenPillNumPos(x, y);
      sprintf(str, "%d", (labelNum-1));
      if (SUCCEEDED(lpDDSBackBuffer->lpVtbl->GetDC(lpDDSBackBuffer, &hDC))) {
        fontSelect(hDC);
        SetBkColor(hDC, RGB(0,0,0));
        SetTextColor(hDC, RGB(255,255,255));
        output.left = (zoomFactor * x * TILE_SIZE_X) + zoomFactor * LABEL_OFFSET_X;
        output.right = zoomFactor * output.left + zoomFactor * TILE_SIZE_X;
        output.top = (zoomFactor * y * TILE_SIZE_Y);
        output.bottom = output.top + zoomFactor * TILE_SIZE_Y;
          DrawTextA(hDC, str, (int) strlen(str), &output, (DT_TOP | DT_NOCLIP));
        lpDDSBackBuffer->lpVtbl->ReleaseDC(lpDDSBackBuffer, hDC);
      } 
    }
    if (isBase == TRUE && showBaseLabels == TRUE) {
      labelNum = screenBaseNumPos(x, y);
      sprintf(str, "%d", (labelNum-1));
      if (SUCCEEDED(lpDDSBackBuffer->lpVtbl->GetDC(lpDDSBackBuffer, &hDC))) {
        fontSelect(hDC);
        SetBkColor(hDC, RGB(0,0,0));
        SetTextColor(hDC, RGB(255,255,255));
        output.left = (zoomFactor * x * TILE_SIZE_X) + LABEL_OFFSET_X;
        output.right = output.left + zoomFactor * TILE_SIZE_X;
        output.top = (zoomFactor * y * TILE_SIZE_Y);
        output.bottom = output.top + zoomFactor * TILE_SIZE_Y;
          DrawTextA(hDC, str, (int) strlen(str), &output, (DT_TOP | DT_NOCLIP));
        lpDDSBackBuffer->lpVtbl->ReleaseDC(lpDDSBackBuffer, hDC);
      } 
    }

    /* Increment the variable */
    x++;
    if (x == MAIN_BACK_BUFFER_SIZE_X) {
      y++;
      x = 0;
      if (y == MAIN_BACK_BUFFER_SIZE_Y) {
        done = TRUE;
      }
    }
  }

  /* Draw Explosions if Required */
  drawShells(sBullets);

  /* Draw the tank */
  drawTanks(tks);

  drawLGMs(lgms);

  /* Draw Gunsight if Required */
  if (gs->mapX != NO_GUNSIGHT) {
    output.left = zoomFactor * GUNSIGHT_X;
    output.right = zoomFactor * GUNSIGHT_X + zoomFactor * TILE_SIZE_X;
    output.top = zoomFactor * GUNSIGHT_Y;
    output.bottom = zoomFactor * GUNSIGHT_Y + zoomFactor * TILE_SIZE_Y;
    outputX = (gs->mapX) * TILE_SIZE_X + (gs->pixelX);
    outputY = (gs->mapY) * TILE_SIZE_Y + (gs->pixelY);
    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, (zoomFactor * outputX) ,  (zoomFactor * outputY), lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
  }

  /* Draw the Cursor Square if required */
  if (useCursor == TRUE) {
    output.left = zoomFactor * MOUSE_SQUARE_X;
    output.right = zoomFactor * MOUSE_SQUARE_X + zoomFactor * TILE_SIZE_X;
    output.top = zoomFactor * MOUSE_SQUARE_Y;
    output.bottom = zoomFactor * MOUSE_SQUARE_Y + zoomFactor * TILE_SIZE_Y;
    outputX = cursorLeft * zoomFactor * TILE_SIZE_X;
    outputY = cursorTop * zoomFactor * TILE_SIZE_Y;
    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, outputX, outputY, lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
  }
  /*  if ((cursorPos(rcWindow, &x, &y)) == TRUE) {
    output.left = MOUSE_SQUARE_X;
    output.right = MOUSE_SQUARE_X + TILE_SIZE_X;
    output.top = MOUSE_SQUARE_Y;
    output.bottom = MOUSE_SQUARE_Y + TILE_SIZE_Y;
    x *= TILE_SIZE_X;
    y *= TILE_SIZE_Y;
    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
  } */
  /* Pillbox view */
  if (isPillView == TRUE) {
    /* we are in pillbox view - Write text here */
    drawPillInView();
  }


  if (netGetStatus() == netFailed) {
    drawNetFailed();
  }

  /* Copy the back buffer to the window */
  output.left = (zoomFactor * TILE_SIZE_X) + edgeX;
  output.top = (zoomFactor * TILE_SIZE_Y) + edgeY;
  output.right = (zoomFactor * MAIN_SCREEN_SIZE_X * TILE_SIZE_X) + (zoomFactor * TILE_SIZE_X) + edgeX;
  output.bottom = (zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y) + (zoomFactor * TILE_SIZE_Y) + edgeY;
  rcWindow->left = rcWindow->left + (zoomFactor * MAIN_OFFSET_X);
  rcWindow->top = rcWindow->top + (zoomFactor * MAIN_OFFSET_Y);
  rcWindow->right = rcWindow->left + (MAIN_SCREEN_SIZE_X * (zoomFactor * TILE_SIZE_X));
  rcWindow->bottom = rcWindow->top + (zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y);
  
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, rcWindow, lpDDSBackBuffer, &output, DDBLT_WAIT, NULL);

  }

  /* Frame rate counting stuff */
  g_dwFrameCount++;
  time = winboloTimer() - g_dwFrameTime;
  if( time > 1000) {
      g_dwFrameTotal = g_dwFrameCount;
      sprintf(str, "%d", g_dwFrameTotal);
/*      DDTextOut( lpDDSPrimary,str, RGB(0,0,0), RGB(255,255,0), rcWindow->left + (zoomFactor * 375), rcWindow->top + (zoomFactor * 50) ); */
      g_dwFrameTime = winboloTimer();
      g_dwFrameCount = 0;
  }
}

/*********************************************************
*NAME:          drawGetFrameRate
*AUTHOR:        John Morrison
*CREATION DATE: 16/12/98
*LAST MODIFIED: 16/12/98
*PURPOSE:
*  Returns the frame rate being achieved
*
*ARGUMENTS:
*
*********************************************************/
int drawGetFrameRate(void) {
  return g_dwFrameTotal;
}

/*********************************************************
*NAME:          drawBackground
*AUTHOR:        John Morrison
*CREATION DATE: 20/12/98
*LAST MODIFIED: 20/12/98
*PURPOSE:
*  Draws the background graphic. Returns if the operation
*  is successful or not.
*
*ARGUMENTS:
*  appInst - The application instance
*  appWnd  - The application Window 
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
bool drawBackground(HINSTANCE appInst, HWND appWnd, int xValue, int yValue) { 
/* NOTE: xValue & yValue UNUSED */
  bool returnValue;   /* Value to return */
  HBITMAP hBg = NULL; /* The background bitmap resource */
  HDC hBgDC = NULL;   /* The background resource DC */
  HDC hDC = NULL;     /* Temp DC of the window */
  BYTE zoomFactor;    /* Scaling Factor */
  char fileName[MAX_PATH]; /* Filename to load */

  hBg = NULL;
  returnValue = FALSE;
  zoomFactor = windowGetZoomFactor();


  /* Try to load from skins file First first */
  if (skinsIsLoaded() == TRUE) {
    skinsGetSkinDirectory(fileName);
    if (fileName[strlen(fileName)-1] != '\\') {
      strcat(fileName, "\\");
    }
    strcat(fileName, DRAW_SKINS_BGFILE);
    hBg = (HBITMAP) LoadImage(NULL, fileName, IMAGE_BITMAP, SCREEN_SIZE_X , SCREEN_SIZE_Y, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
  } 


  if (hBg == NULL) {
    hBg = (HBITMAP) LoadImage(appInst, MAKEINTRESOURCE(IDB_BACKGROUND), IMAGE_BITMAP, SCREEN_SIZE_X , SCREEN_SIZE_Y, LR_CREATEDIBSECTION);
  }

  if (hBg != NULL) {
    hBgDC = CreateCompatibleDC(NULL);
    SelectObject(hBgDC, hBg);
    hDC = GetDC(appWnd);
    if (hDC != NULL) {
      if (zoomFactor == ZOOM_FACTOR_NORMAL) {
        BitBlt(hDC, 0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y, hBgDC, 0, 0, SRCCOPY);
      } else {
        StretchBlt(hDC, 0, 0, (zoomFactor * SCREEN_SIZE_X), (zoomFactor * SCREEN_SIZE_Y), hBgDC, 0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y, SRCCOPY);
      }
      DeleteDC(hBgDC);
      DeleteObject(hBg);
      ReleaseDC(appWnd, hDC);
      returnValue = TRUE;
    }
  } 

  return returnValue;
}

/*********************************************************
*NAME:          drawBuildSelectRefresh
*AUTHOR:        John Morrison
*CREATION DATE: 31/01/09
*LAST MODIFIED: 31/01/09
*PURPOSE:
*  Redraws the build select icon on the main screen.
*  Required as Vista Aero no long supports transparent/
*  colour key blits onto the primary surface.
*
*  This is called after the drawBuildSelectIndents*
*  functions complete. It is suboptimal as it loads in
*  the bitmap file each time. This also happens twice:
*  once for the On oppeation and onces for the off.
*ARGUMENTS:
*  value - The build select item to redraw
*********************************************************/
void drawBuildSelectRefresh(buildSelect value) { 
  HBITMAP hBg = NULL; /* The background bitmap resource */
  HDC hBgDC = NULL;   /* The background resource DC */
  HDC hDC = NULL;     /* Temp DC of the window */
  BYTE zoomFactor;    /* Scaling Factor */
  char fileName[MAX_PATH]; /* Filename to load */
  HWND hWnd;          /* Main window handle */
  int top;            /* Top build select y co-ord to copy from */
  
  hWnd = windowWnd();
  hBg = NULL;
  zoomFactor = windowGetZoomFactor();
  
  switch (value) {
    case BsTrees:
      top = DRAW_BUILDSELECT_TREES_Y;
      break;
    case BsRoad:
      top = DRAW_BUILDSELECT_ROAD_Y;
      break;
    case BsBuilding:
      top = DRAW_BUILDING_ROAD_Y;
      break;
    case BsPillbox:
      top = DRAW_BUILDING_PILLBOX_Y;
      break;
    default: 
     /* BsMine:*/
      top = DRAW_BUILDING_MINE_Y;
      break;
  }

  /* Try to load from skins file First first */
  if (skinsIsLoaded() == TRUE) {
    skinsGetSkinDirectory(fileName);
    if (fileName[strlen(fileName)-1] != '\\') {
      strcat(fileName, "\\");
    }
    strcat(fileName, DRAW_SKINS_BGFILE);
    hBg = (HBITMAP) LoadImage(NULL, fileName, IMAGE_BITMAP, SCREEN_SIZE_X , SCREEN_SIZE_Y, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
  } 

  if (hBg == NULL) {
    hBg = (HBITMAP) LoadImage(windowGetInstance(), MAKEINTRESOURCE(IDB_BACKGROUND), IMAGE_BITMAP, SCREEN_SIZE_X , SCREEN_SIZE_Y, LR_CREATEDIBSECTION);
  }

  if (hBg != NULL) {
    hBgDC = CreateCompatibleDC(NULL);
    SelectObject(hBgDC, hBg);
    hDC = GetDC(hWnd);
    if (hDC != NULL) {
      if (zoomFactor == ZOOM_FACTOR_NORMAL) {
        BitBlt(hDC, DRAW_BUILDSELECT_X, top, DRAW_BUILDSELECT_WIDTH, DRAW_BUILDSELECT_HEIGHT, hBgDC, DRAW_BUILDSELECT_X, top, SRCCOPY);
      } else {
        StretchBlt(hDC, DRAW_BUILDSELECT_X * zoomFactor, top * zoomFactor, zoomFactor * DRAW_BUILDSELECT_WIDTH, (zoomFactor * DRAW_BUILDSELECT_HEIGHT), hBgDC, DRAW_BUILDSELECT_X, top, DRAW_BUILDSELECT_WIDTH, DRAW_BUILDSELECT_HEIGHT, SRCCOPY);
      }
      DeleteDC(hBgDC);
      DeleteObject(hBg);
      ReleaseDC(hWnd, hDC);
    }
  } 
}

/*********************************************************
*NAME:          drawSelectIndentsOn
*AUTHOR:        John Morrison
*CREATION DATE: 20/12/98
*LAST MODIFIED: 20/12/98
*PURPOSE:
*  Draws the indents around the five building selection 
*  graphics on the left based on the buildSelect value.
*  Draws the red dot as well.
*
*ARGUMENTS:
*  value   - The currently selected build icon
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawSelectIndentsOn(buildSelect value, int xValue, int yValue) {
  RECT src;        /* The src square on the tile file to retrieve */
  RECT dest;       /* The dest square to draw it */
  BYTE zoomFactor; /* Scaling factor */

  zoomFactor = windowGetZoomFactor();

  /* Set the co-ords of the tile file to get */
  src.left = zoomFactor * INDENT_ON_X;
  src.top = zoomFactor * INDENT_ON_Y;
  src.right = zoomFactor * INDENT_ON_X + zoomFactor * BS_ITEM_SIZE_X;
  src.bottom = zoomFactor * INDENT_ON_Y + zoomFactor * BS_ITEM_SIZE_Y;

  /* Modify the offset to allow for the indents */
  dest.left = xValue;
  dest.top = yValue;
  switch (value) {
  case BsTrees:
    dest.left += (zoomFactor * BS_TREE_OFFSET_X);
    dest.top += (zoomFactor * BS_TREE_OFFSET_Y);
    break;
  case BsRoad:
    dest.left += (zoomFactor * BS_ROAD_OFFSET_X);
    dest.top += (zoomFactor * BS_ROAD_OFFSET_Y);
    break;
  case BsBuilding:
    dest.left += (zoomFactor * BS_BUILDING_OFFSET_X);
    dest.top += (zoomFactor * BS_BUILDING_OFFSET_Y);
    break;
  case BsPillbox:
    dest.left += (zoomFactor * BS_PILLBOX_OFFSET_X);
    dest.top+= (zoomFactor * BS_PILLBOX_OFFSET_Y);
    break;
  default: 
   /* BsMine:*/
    dest.left += (zoomFactor * BS_MINE_OFFSET_X);
    dest.top += (zoomFactor * BS_MINE_OFFSET_Y);
    break;
  }
  dest.right = dest.left + (zoomFactor * BS_ITEM_SIZE_X);
  dest.bottom = dest.top + (zoomFactor * BS_ITEM_SIZE_Y);
  
  /* Perform the drawing */
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , lpDDSTiles ,&src, DDBLT_WAIT | DDBLT_KEYSRC, NULL);

  /* Set the co-ords of the tile file to get */
  src.left = zoomFactor * INDENT_DOT_ON_X;
  src.top = zoomFactor * INDENT_DOT_ON_Y;
  src.right = zoomFactor * INDENT_DOT_ON_X + zoomFactor * BS_DOT_ITEM_SIZE_X;
  src.bottom = zoomFactor * INDENT_DOT_ON_Y + zoomFactor * BS_DOT_ITEM_SIZE_Y;

  /* Draw the dot */
  /* Modify the offset to allow for the indents */
  dest.left = xValue;
  dest.top = yValue;
  switch (value) {
  case BsTrees:
    dest.left += (zoomFactor * BS_DOT_TREE_OFFSET_X);
    dest.top += (zoomFactor * BS_DOT_TREE_OFFSET_Y);
    break;
  case BsRoad:
    dest.left += (zoomFactor * BS_DOT_ROAD_OFFSET_X);
    dest.top += (zoomFactor * BS_DOT_ROAD_OFFSET_Y);
    break;
  case BsBuilding:
    dest.left += (zoomFactor * BS_DOT_BUILDING_OFFSET_X);
    dest.top += (zoomFactor * BS_DOT_BUILDING_OFFSET_Y);
    break;
  case BsPillbox:
    dest.left += (zoomFactor * BS_DOT_PILLBOX_OFFSET_X);
    dest.top+= (zoomFactor * BS_DOT_PILLBOX_OFFSET_Y);
    break;
  default: 
   /* BsMine:*/
    dest.left += (zoomFactor * BS_DOT_MINE_OFFSET_X);
    dest.top += (zoomFactor * BS_DOT_MINE_OFFSET_Y);
    break;
  }  
  
  dest.right = dest.left + (zoomFactor * BS_DOT_ITEM_SIZE_X);
  dest.bottom = dest.top + (zoomFactor * BS_DOT_ITEM_SIZE_Y);
  
  /* Perform the drawing */
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , lpDDSTiles ,&src, DDBLT_WAIT, NULL);
  drawBuildSelectRefresh(value);
}

/*********************************************************
*NAME:          drawSelectIndentsOff
*AUTHOR:        John Morrison
*CREATION DATE: 20/12/98
*LAST MODIFIED: 20/12/98
*PURPOSE:
*  Draws the indents around the five building selection 
*  graphics off the left based on the buildSelect value.
*  Draws the red dot as well.
*
*ARGUMENTS:
*  value   - The currently selected build icon
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawSelectIndentsOff(buildSelect value, int xValue, int yValue) {
  RECT src;        /* The src square on the tile file to retrieve */
  RECT dest;       /* The dest square to draw it */
  BYTE zoomFactor; /* Scaling factor */

  zoomFactor = windowGetZoomFactor();

  /* Set the co-ords of the tile file to get */
  src.left = zoomFactor * INDENT_OFF_X;
  src.top = zoomFactor * INDENT_OFF_Y;
  src.right = zoomFactor * INDENT_OFF_X + zoomFactor * BS_ITEM_SIZE_X;
  src.bottom = zoomFactor * INDENT_OFF_Y + zoomFactor * BS_ITEM_SIZE_Y;

  /* Modify the offset to allow for the indents */
  dest.left = xValue;
  dest.top = yValue;
  switch (value) {
  case BsTrees:
    dest.left += (zoomFactor * BS_TREE_OFFSET_X);
    dest.top += (zoomFactor * BS_TREE_OFFSET_Y);
    break;
  case BsRoad:
    dest.left += (zoomFactor * BS_ROAD_OFFSET_X);
    dest.top += (zoomFactor * BS_ROAD_OFFSET_Y);
    break;
  case BsBuilding:
    dest.left += (zoomFactor * BS_BUILDING_OFFSET_X);
    dest.top += (zoomFactor * BS_BUILDING_OFFSET_Y);
    break;
  case BsPillbox:
    dest.left += (zoomFactor * BS_PILLBOX_OFFSET_X);
    dest.top+= (zoomFactor * BS_PILLBOX_OFFSET_Y);
    break;
  default: 
   /* BsMine:*/
    dest.left += (zoomFactor * BS_MINE_OFFSET_X);
    dest.top += (zoomFactor * BS_MINE_OFFSET_Y);
    break;
  }
  dest.right = dest.left + (zoomFactor * BS_ITEM_SIZE_X);
  dest.bottom = dest.top + (zoomFactor * BS_ITEM_SIZE_Y);
  
  /* Perform the drawing */
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , lpDDSTiles ,&src, DDBLT_WAIT | DDBLT_KEYSRC, NULL);

  /* Set the co-ords of the tile file to get */
  src.left = zoomFactor * INDENT_DOT_OFF_X;
  src.top = zoomFactor * INDENT_DOT_OFF_Y;
  src.right = zoomFactor * INDENT_DOT_OFF_X + zoomFactor * BS_DOT_ITEM_SIZE_X;
  src.bottom = zoomFactor * INDENT_DOT_OFF_Y + zoomFactor * BS_DOT_ITEM_SIZE_Y;

  /* Draw the dot */
  /* Modify the offset to allow for the indents */
  dest.left = xValue;
  dest.top = yValue;
  switch (value) {
  case BsTrees:
    dest.left += (zoomFactor * BS_DOT_TREE_OFFSET_X);
    dest.top += (zoomFactor * BS_DOT_TREE_OFFSET_Y);
    break;
  case BsRoad:
    dest.left += (zoomFactor * BS_DOT_ROAD_OFFSET_X);
    dest.top += (zoomFactor * BS_DOT_ROAD_OFFSET_Y);
    break;
  case BsBuilding:
    dest.left += (zoomFactor * BS_DOT_BUILDING_OFFSET_X);
    dest.top += (zoomFactor * BS_DOT_BUILDING_OFFSET_Y);
    break;
  case BsPillbox:
    dest.left += (zoomFactor * BS_DOT_PILLBOX_OFFSET_X);
    dest.top += (zoomFactor * BS_DOT_PILLBOX_OFFSET_Y);
    break;
  default: 
   /* BsMine:*/
    dest.left += (zoomFactor * BS_DOT_MINE_OFFSET_X);
    dest.top += (zoomFactor * BS_DOT_MINE_OFFSET_Y);
    break;
  }  
  
  dest.right = dest.left + (zoomFactor * BS_DOT_ITEM_SIZE_X);
  dest.bottom = dest.top + (zoomFactor * BS_DOT_ITEM_SIZE_Y);
  
  /* Perform the drawing */
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , lpDDSTiles ,&src, DDBLT_WAIT, NULL);
  drawBuildSelectRefresh(value);
}

/*********************************************************
*NAME:          drawRedrawAll
*AUTHOR:        John Morrison
*CREATION DATE: 20/12/98
*LAST MODIFIED: 22/11/98
*PURPOSE:
*  Redraws everything (except the main view)
*
*ARGUMENTS:
*  appInst         - Application Instance
*  appWnd          - Application Window
*  value           - The currently selected build icon
*  rcWindow        - The window co-ords
*  showPillsStatus - Should the the pill status be shown
*  showBasesStatus - Should the the base status be shown
*********************************************************/
void drawRedrawAll(HINSTANCE appInst, HWND appWnd, buildSelect value, RECT *rcWindow, bool showPillsStatus, bool showBasesStatus) {
  BYTE total;      /* Total number of elements */
  BYTE count;      /* Looping Variable */
  BYTE ba;         /* Base, tanks and pill Alliences */
  BYTE shells;     /* Tank amounts */
  BYTE mines;
  BYTE armour;
  BYTE trees;
  char top[MESSAGE_STRING_SIZE]; /* Message lines */
  char bottom[MESSAGE_STRING_SIZE];
  int kills;      /* Number of kills and deaths the tank has */
  int deaths;
  /* LGM Status */
  bool lgmIsOut;
  bool lgmIsDead;
  TURNTYPE lgmAngle;

  drawBackground(appInst, appWnd, rcWindow->left, rcWindow->top);
  drawSelectIndentsOn(value, rcWindow->left, rcWindow->top);
  /* Draw Bases Status*/
  drawSetBasesStatusClear();
  clientMutexWaitFor();
  total = screenNumBases();
  for (count=1;count<=total;count++) {
    ba = screenBaseAlliance(count);
    drawStatusBase(count, ba, showBasesStatus);
  }
  clientMutexRelease();
  drawCopyBasesStatus(rcWindow->left, rcWindow->top);
  /* Draw Pillbox Status */
  clientMutexWaitFor();
  drawSetPillsStatusClear();
  total = screenNumPills();
  for (count=1;count<=total;count++) {
    ba = screenPillAlliance(count);
    drawStatusPillbox(count, ba, showPillsStatus);
  }
  clientMutexRelease();
  drawCopyPillsStatus(rcWindow->left, rcWindow->top);

  /* Draw Tanks Status */
  drawSetTanksStatusClear();
  clientMutexWaitFor();
  total = screenGetNumPlayers();
  for (count=1;count<=MAX_TANKS;count++) {
    ba = screenTankAlliance(count);
    drawStatusTank(count, ba);
  }
  clientMutexRelease();
  drawCopyTanksStatus(rcWindow->left, rcWindow->top);

  
  /* Draw tank Bars */
  clientMutexWaitFor();
  screenGetTankStats(&shells,&mines, &armour, &trees);
  drawStatusTankBars(rcWindow->left, rcWindow->top,shells,mines,armour,trees);
  screenGetMessages(top,bottom);
  drawMessages(rcWindow->left, rcWindow->top, top, bottom);
  screenGetKillsDeaths(&kills, &deaths);
  drawKillsDeaths(rcWindow->left, rcWindow->top, kills, deaths);
  drawStatusBaseBars(rcWindow->left, rcWindow->top, 0,0,0,TRUE);
  screenGetLgmStatus(&lgmIsOut, &lgmIsDead, &lgmAngle);
  if (lgmIsOut == FALSE) {
    drawSetManClear();
    drawCopyManStatus(rcWindow->left, rcWindow->top);
  } else {
    drawSetManStatus(rcWindow->left, rcWindow->top, lgmIsDead, lgmAngle);
  }
  clientMutexRelease();
}

/*********************************************************
*NAME:          drawStatusBase
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 23/1/99
*PURPOSE:
*  Draws the base status for a particular base
*
*ARGUMENTS:
*  baseNum - The base number to draw (1-16)
*  ba      - The allience of the base
*  labels  - Should the label be shown
*********************************************************/
void drawStatusBase(BYTE baseNum, baseAlliance ba, bool labels) {
  RECT src;  /* The src square on the tile file to retrieve */
  RECT dest; /* The dest square to draw it */
  BYTE zf;   /* Scaling factor */
  HDC hDC;     /* Temp DC of the surface */
  char str[3]; /* String to output if labels are on */

  zf = windowGetZoomFactor();
  str[0] = '\0';

  /* Set the co-ords of the tile file to get */
  switch (ba) {
  case baseDead:
    src.left = zf * STATUS_ITEM_DEAD_X;
    src.top = zf * STATUS_ITEM_DEAD_Y;
    src.right = zf * STATUS_ITEM_DEAD_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_ITEM_DEAD_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case baseNeutral:
    src.left = zf * STATUS_BASE_NEUTRAL_X;
    src.top = zf * STATUS_BASE_NEUTRAL_Y;
    src.right = zf * STATUS_BASE_NEUTRAL_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_BASE_NEUTRAL_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case baseOwnGood:
    src.left = zf * STATUS_BASE_GOOD_X;
    src.top = zf * STATUS_BASE_GOOD_Y;
    src.right = zf * STATUS_BASE_GOOD_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_BASE_GOOD_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case baseAllieGood:
    src.left = zf * STATUS_BASE_ALLIEGOOD_X;
    src.top = zf * STATUS_BASE_ALLIEGOOD_Y;
    src.right = zf * STATUS_BASE_ALLIEGOOD_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_BASE_ALLIEGOOD_Y + zf * STATUS_ITEM_SIZE_Y;
    break;

  default:
    /* Base Evil */
    src.left = zf * STATUS_BASE_EVIL_X;
    src.top = zf * STATUS_BASE_EVIL_Y;
    src.right = zf * STATUS_BASE_EVIL_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_BASE_EVIL_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  }
  /* Modify the offset to allow for the indents */
  switch (baseNum) {
  case BASE_1:
    dest.left = (zf * STATUS_BASE_1_X);
    dest.top = (zf * STATUS_BASE_1_Y);
    break;
  case BASE_2:
    dest.left = (zf * STATUS_BASE_2_X);
    dest.top = (zf * STATUS_BASE_2_Y);
    break;
  case BASE_3:
    dest.left = (zf * STATUS_BASE_3_X);
    dest.top = (zf * STATUS_BASE_3_Y);
    break;
  case BASE_4:
    dest.left = (zf * STATUS_BASE_4_X);
    dest.top = (zf * STATUS_BASE_4_Y);
    break;
  case BASE_5:
    dest.left = (zf * STATUS_BASE_5_X);
    dest.top = (zf * STATUS_BASE_5_Y);
    break;
  case BASE_6:
    dest.left = (zf * STATUS_BASE_6_X);
    dest.top = (zf * STATUS_BASE_6_Y);
    break;
  case BASE_7:
    dest.left = (zf * STATUS_BASE_7_X);
    dest.top = (zf * STATUS_BASE_7_Y);
    break;
  case BASE_8:
    dest.left = (zf * STATUS_BASE_8_X);
    dest.top = (zf * STATUS_BASE_8_Y);
    break;
  case BASE_9:
    dest.left = (zf * STATUS_BASE_9_X);
    dest.top = (zf * STATUS_BASE_9_Y);
    break;
  case BASE_10:
    dest.left = (zf * STATUS_BASE_10_X);
    dest.top = (zf * STATUS_BASE_10_Y);
    break;
  case BASE_11:
    dest.left = (zf * STATUS_BASE_11_X);
    dest.top = (zf * STATUS_BASE_11_Y);
    break;
  case BASE_12:
    dest.left = (zf * STATUS_BASE_12_X);
    dest.top = (zf * STATUS_BASE_12_Y);
    break;
  case BASE_13:
    dest.left = (zf * STATUS_BASE_13_X);
    dest.top = (zf * STATUS_BASE_13_Y);
    break;
  case BASE_14:
    dest.left = (zf * STATUS_BASE_14_X);
    dest.top = (zf * STATUS_BASE_14_Y);
    break;
  case BASE_15:
    dest.left = (zf * STATUS_BASE_15_X);
    dest.top = (zf * STATUS_BASE_15_Y);
    break;
  default: 
   /* BASE_16:*/
    dest.left = (zf * STATUS_BASE_16_X);
    dest.top = (zf * STATUS_BASE_16_Y);
  }

  dest.right = dest.left + (zf * STATUS_ITEM_SIZE_X);
  dest.bottom = dest.top + (zf * STATUS_ITEM_SIZE_Y);
  
  /* Perform the drawing */
  lpDDSBasesStatus->lpVtbl->Blt(lpDDSBasesStatus, &dest , lpDDSTiles ,&src, DDBLT_WAIT, NULL);
  if (labels == TRUE) {
    /* Must draw the label */
    sprintf(str, "%d", (baseNum-1));
    if (SUCCEEDED(lpDDSBasesStatus->lpVtbl->GetDC(lpDDSBasesStatus,&hDC))) {
      fontSelectTiny(hDC);
      SetBkColor(hDC, RGB(0,0,0));
      SetTextColor(hDC, RGB(255,255,255));
      DrawTextA(hDC, str, (int) strlen(str), &dest, (DT_TOP | DT_NOCLIP));
      lpDDSBasesStatus->lpVtbl->ReleaseDC(lpDDSBasesStatus, hDC);
    } 
  }
}

/*********************************************************
*NAME:          drawStatusPillbox
*AUTHOR:        John Morrison
*CREATION DATE: 21/12/98
*LAST MODIFIED: 23/1/99
*PURPOSE:
*  Draws the pillbox status for a particular pillbox
*
*ARGUMENTS:
*  pillNum - The tank number to draw (1-16)
*  pb      - The allience of the pillbox
*  labels  - Should labels be drawn?
*********************************************************/
void drawStatusPillbox(BYTE pillNum, pillAlliance pb, bool labels) {
  RECT src;    /* The src square on the tile file to retrieve */
  RECT dest;   /* The dest square to draw it */
  BYTE zf;     /* Scaling factor */
  HDC hDC;     /* Temp DC of the surface */
  char str[3]; /* String to output if labels are on */

  zf = windowGetZoomFactor();
  str[0] = '\0';

  /* Set the co-ords of the tile file to get */
  switch (pb) {
  case pillDead:
    src.left = zf * STATUS_ITEM_DEAD_X;
    src.top = zf * STATUS_ITEM_DEAD_Y;
    src.right = zf * STATUS_ITEM_DEAD_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_ITEM_DEAD_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case pillNeutral:
    src.left = zf * STATUS_PILLBOX_NEUTRAL_X;
    src.top = zf * STATUS_PILLBOX_NEUTRAL_Y;
    src.right = zf * STATUS_PILLBOX_NEUTRAL_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_PILLBOX_NEUTRAL_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case pillGood:
    src.left = zf * STATUS_PILLBOX_GOOD_X;
    src.top = zf * STATUS_PILLBOX_GOOD_Y;
    src.right = zf * STATUS_PILLBOX_GOOD_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_PILLBOX_GOOD_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case pillAllie:
    src.left = zf * STATUS_PILLBOX_ALLIEGOOD_X;
    src.top = zf * STATUS_PILLBOX_ALLIEGOOD_Y;
    src.right = zf * STATUS_PILLBOX_ALLIEGOOD_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_PILLBOX_ALLIEGOOD_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case pillTankGood:
    src.left = zf * STATUS_PILLBOX_TANKGOOD_X;
    src.top = zf * STATUS_PILLBOX_TANKGOOD_Y;
    src.right = zf * STATUS_PILLBOX_TANKGOOD_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_PILLBOX_TANKGOOD_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case pillTankAllie:
    src.left = zf * STATUS_PILLBOX_TANKALLIE_X;
    src.top = zf * STATUS_PILLBOX_TANKALLIE_Y;
    src.right = zf * STATUS_PILLBOX_TANKALLIE_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_PILLBOX_TANKALLIE_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case pillTankEvil:
    src.left = zf * STATUS_PILLBOX_TANKEVIL_X;
    src.top = zf * STATUS_PILLBOX_TANKEVIL_Y;
    src.right = zf * STATUS_PILLBOX_TANKEVIL_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_PILLBOX_TANKEVIL_Y + zf * STATUS_ITEM_SIZE_Y;
    break;

  default:
    /* PILLBOX Evil */
    src.left = zf * STATUS_PILLBOX_EVIL_X;
    src.top = zf * STATUS_PILLBOX_EVIL_Y;
    src.right = zf * STATUS_PILLBOX_EVIL_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_PILLBOX_EVIL_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  }
  /* Modify the offset to allow for the indents */
  switch (pillNum) {
  case PILLBOX_1:
    dest.left = (zf * STATUS_PILLBOX_1_X);
    dest.top = (zf * STATUS_PILLBOX_1_Y);
    break;
  case PILLBOX_2:
    dest.left = (zf * STATUS_PILLBOX_2_X);
    dest.top = (zf * STATUS_PILLBOX_2_Y);
    break;
  case PILLBOX_3:
    dest.left = (zf * STATUS_PILLBOX_3_X);
    dest.top = (zf * STATUS_PILLBOX_3_Y);
    break;
  case PILLBOX_4:
    dest.left = (zf * STATUS_PILLBOX_4_X);
    dest.top = (zf * STATUS_PILLBOX_4_Y);
    break;
  case PILLBOX_5:
    dest.left = (zf * STATUS_PILLBOX_5_X);
    dest.top = (zf * STATUS_PILLBOX_5_Y);
    break;
  case PILLBOX_6:
    dest.left = (zf * STATUS_PILLBOX_6_X);
    dest.top = (zf * STATUS_PILLBOX_6_Y);
    break;
  case PILLBOX_7:
    dest.left = (zf * STATUS_PILLBOX_7_X);
    dest.top = (zf * STATUS_PILLBOX_7_Y);
    break;
  case PILLBOX_8:
    dest.left = (zf * STATUS_PILLBOX_8_X);
    dest.top = (zf * STATUS_PILLBOX_8_Y);
    break;
  case PILLBOX_9:
    dest.left = (zf * STATUS_PILLBOX_9_X);
    dest.top = (zf * STATUS_PILLBOX_9_Y);
    break;
  case PILLBOX_10:
    dest.left = (zf * STATUS_PILLBOX_10_X);
    dest.top = (zf * STATUS_PILLBOX_10_Y);
    break;
  case PILLBOX_11:
    dest.left = (zf * STATUS_PILLBOX_11_X);
    dest.top = (zf * STATUS_PILLBOX_11_Y);
    break;
  case PILLBOX_12:
    dest.left = (zf * STATUS_PILLBOX_12_X);
    dest.top = (zf * STATUS_PILLBOX_12_Y);
    break;
  case PILLBOX_13:
    dest.left = (zf * STATUS_PILLBOX_13_X);
    dest.top = (zf * STATUS_PILLBOX_13_Y);
    break;
  case PILLBOX_14:
    dest.left = (zf * STATUS_PILLBOX_14_X);
    dest.top = (zf * STATUS_PILLBOX_14_Y);
    break;
  case PILLBOX_15:
    dest.left = (zf * STATUS_PILLBOX_15_X);
    dest.top = (zf * STATUS_PILLBOX_15_Y);
    break;
  default: 
   /* PILLBOX_16:*/
    dest.left = (zf * STATUS_PILLBOX_16_X);
    dest.top = (zf * STATUS_PILLBOX_16_Y);
  }

  dest.right = dest.left + (zf * STATUS_ITEM_SIZE_X);
  dest.bottom = dest.top + (zf * STATUS_ITEM_SIZE_Y);

  /* Perform the drawing */
  lpDDSPillsStatus->lpVtbl->Blt(lpDDSPillsStatus, &dest , lpDDSTiles ,&src, DDBLT_WAIT, NULL);
  if (labels == TRUE) {
    /* Must draw the label */
    sprintf(str, "%d", (pillNum-1));
    if (SUCCEEDED(lpDDSPillsStatus->lpVtbl->GetDC(lpDDSPillsStatus,&hDC))) {
      fontSelectTiny(hDC);
      SetBkColor(hDC, RGB(0,0,0));
      SetTextColor(hDC, RGB(255,255,255));
      DrawTextA(hDC, str, (int) strlen(str), &dest, (DT_TOP | DT_NOCLIP));
      lpDDSPillsStatus->lpVtbl->ReleaseDC(lpDDSPillsStatus, hDC);
    } 
  }
}


/*********************************************************
*NAME:          drawStatusTank
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/99
*LAST MODIFIED: 14/2/99
*PURPOSE:
*  Draws the tank status for a particular tank
*
*ARGUMENTS:
*  tankNum - The tank number to draw (1-16)
*  ta      - The allience of the pillbox
*********************************************************/
void drawStatusTank(BYTE tankNum, tankAlliance ta) {
  RECT src;    /* The src square on the tile file to retrieve */
  RECT dest;   /* The dest square to draw it */
  BYTE zf;     /* Scaling factor */

  zf = windowGetZoomFactor();

  /* Set the co-ords of the tile file to get */
  switch (ta) {
  case tankNone:
    src.left = zf * STATUS_TANK_NONE_X;
    src.top = zf * STATUS_TANK_NONE_Y;
    src.right = zf * STATUS_TANK_NONE_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_TANK_NONE_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case tankSelf:
    src.left = zf * STATUS_TANK_SELF_X;
    src.top = zf * STATUS_TANK_SELF_Y;
    src.right = zf * STATUS_TANK_SELF_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_TANK_SELF_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  case tankAllie:
    src.left = zf * STATUS_TANK_GOOD_X;
    src.top = zf * STATUS_TANK_GOOD_Y;
    src.right = zf * STATUS_TANK_GOOD_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_TANK_GOOD_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  default:
    /* tankEvil */
    src.left = zf * STATUS_TANK_EVIL_X;
    src.top = zf * STATUS_TANK_EVIL_Y;
    src.right = zf * STATUS_TANK_EVIL_X + zf * STATUS_ITEM_SIZE_X;
    src.bottom = zf * STATUS_TANK_EVIL_Y + zf * STATUS_ITEM_SIZE_Y;
    break;
  }
  /* Modify the offset to allow for the indents */
  switch (tankNum) {
  case TANK_1:
    dest.left = (zf * STATUS_TANKS_1_X);
    dest.top = (zf * STATUS_TANKS_1_Y);
    break;
  case TANK_2:
    dest.left = (zf * STATUS_TANKS_2_X);
    dest.top = (zf * STATUS_TANKS_2_Y);
    break;
  case TANK_3:
    dest.left = (zf * STATUS_TANKS_3_X);
    dest.top = (zf * STATUS_TANKS_3_Y);
    break;
  case TANK_4:
    dest.left = (zf * STATUS_TANKS_4_X);
    dest.top = (zf * STATUS_TANKS_4_Y);
    break;
  case TANK_5:
    dest.left = (zf * STATUS_TANKS_5_X);
    dest.top = (zf * STATUS_TANKS_5_Y);
    break;
  case TANK_6:
    dest.left = (zf * STATUS_TANKS_6_X);
    dest.top = (zf * STATUS_TANKS_6_Y);
    break;
  case TANK_7:
    dest.left = (zf * STATUS_TANKS_7_X);
    dest.top = (zf * STATUS_TANKS_7_Y);
    break;
  case TANK_8:
    dest.left = (zf * STATUS_TANKS_8_X);
    dest.top = (zf * STATUS_TANKS_8_Y);
    break;
  case TANK_9:
    dest.left = (zf * STATUS_TANKS_9_X);
    dest.top = (zf * STATUS_TANKS_9_Y);
    break;
  case TANK_10:
    dest.left = (zf * STATUS_TANKS_10_X);
    dest.top = (zf * STATUS_TANKS_10_Y);
    break;
  case TANK_11:
    dest.left = (zf * STATUS_TANKS_11_X);
    dest.top = (zf * STATUS_TANKS_11_Y);
    break;
  case TANK_12:
    dest.left = (zf * STATUS_TANKS_12_X);
    dest.top = (zf * STATUS_TANKS_12_Y);
    break;
  case TANK_13:
    dest.left = (zf * STATUS_TANKS_13_X);
    dest.top = (zf * STATUS_TANKS_13_Y);
    break;
  case TANK_14:
    dest.left = (zf * STATUS_TANKS_14_X);
    dest.top = (zf * STATUS_TANKS_14_Y);
    break;
  case TANK_15:
    dest.left = (zf * STATUS_TANKS_15_X);
    dest.top = (zf * STATUS_TANKS_15_Y);
    break;
  default: 
   /* TANK_16:*/
    dest.left = (zf * STATUS_TANKS_16_X);
    dest.top = (zf * STATUS_TANKS_16_Y);
  }

  dest.right = dest.left + (zf * STATUS_ITEM_SIZE_X);
  dest.bottom = dest.top + (zf * STATUS_ITEM_SIZE_Y);

  /* Perform the drawing */
  lpDDSTankStatus->lpVtbl->Blt(lpDDSTankStatus, &dest , lpDDSTiles ,&src, DDBLT_WAIT, NULL);
}


/*********************************************************
*NAME:          drawStatusTankBars
*AUTHOR:        John Morrison
*CREATION DATE: 22/12/98
*LAST MODIFIED: 22/12/98
*PURPOSE:
*  Draws the tanks armour, shells etc bars.
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  shells  - Number of shells
*  mines   - Number of mines
*  armour  - Amount of armour
*  trees   - Amount of trees
*********************************************************/
void drawStatusTankBars(int xValue, int yValue, BYTE shells, BYTE mines, BYTE armour, BYTE trees) {
  RECT dest;              /* The dest square to draw it */
  DDBLTFX fx;             /* FX for the colour fills */
  DDPIXELFORMAT ddpf;     /* DD Pixel Format to get green */
  BYTE zf;   /* Scaling factor */

  zf = windowGetZoomFactor();

  /* Get Pixel Format and set color*/
  ddpf.dwSize = sizeof(ddpf);
  lpDDSPrimary->lpVtbl->GetPixelFormat(lpDDSPrimary, &ddpf); 
  ZeroMemory(&fx, sizeof(fx));
  fx.dwSize = sizeof(fx);
  fx.dwFillColor =  0;

  /* Make the area black first */

  dest.top = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - ((zf * BAR_TANK_MULTIPLY) *  40);
  dest.bottom = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT);
  dest.left = xValue + (zf * STATUS_TANK_SHELLS);
  dest.right = xValue + (zf * STATUS_TANK_TREES) + (zf * STATUS_TANK_BARS_WIDTH);
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

  fx.dwFillColor =  ddpf.dwGBitMask;
  
  /* Shells */
  dest.top = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - ((zf * BAR_TANK_MULTIPLY) * shells);
  dest.left = xValue + (zf * STATUS_TANK_SHELLS);
  dest.bottom = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT);
  dest.right = xValue + (zf * STATUS_TANK_SHELLS) + (zf * STATUS_TANK_BARS_WIDTH);
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

  /* Mines */
  dest.top = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - ((zf * BAR_TANK_MULTIPLY) * mines);
  dest.left = xValue + (zf * STATUS_TANK_MINES);
  dest.right = xValue + (zf * STATUS_TANK_MINES) + (zf * STATUS_TANK_BARS_WIDTH);
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

  /* Armour */
  dest.top = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - ((zf * BAR_TANK_MULTIPLY) * armour);
  dest.left = xValue + (zf * STATUS_TANK_ARMOUR);
  dest.right = xValue + (zf * STATUS_TANK_ARMOUR) + (zf * STATUS_TANK_BARS_WIDTH);
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

  /* Trees */
  dest.top = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - ((zf * BAR_TANK_MULTIPLY) * trees);
  dest.left = xValue + (zf * STATUS_TANK_TREES);
  dest.right = xValue + (zf * STATUS_TANK_TREES) + (zf * STATUS_TANK_BARS_WIDTH);
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
}

/*********************************************************
*NAME:          drawShells
*AUTHOR:        John Morrison
*CREATION DATE: 26/12/98
*LAST MODIFIED: 26/12/98
*PURPOSE:
*  Draws shells and explosions on the backbuffer.
*
*ARGUMENTS:
*  sBullets - The screen Bullets data structure 
*********************************************************/
void drawShells(screenBullets *sBullets) {
  RECT output; /* Output Rectangle */
  int total;   /* Total shells to been drawn */
  int count;   /* Looping Variable */
  BYTE px;
  BYTE py;
  BYTE frame;
  BYTE mx;
  BYTE my;
  int x;
  int y;
  BYTE zf;

  total = screenBulletsGetNumEntries(sBullets);
  count = 1;
  zf = windowGetZoomFactor();
  while (count <= total) {
    screenBulletsGetItem(sBullets, count, &mx, &my,&px,&py, &frame);
    x = (mx * zf * TILE_SIZE_X) + zf * px;
    y = (my * zf * TILE_SIZE_Y) + zf * py;
    switch (frame) {
    case SHELL_EXPLOSION8:
      output.left = zf * EXPLOSION8_X;
      output.top = zf * EXPLOSION8_Y;
      output.right = (zf * EXPLOSION8_X + zf * TILE_SIZE_X);
      output.bottom = (zf * EXPLOSION8_Y + zf * TILE_SIZE_Y);
      break;
    case SHELL_EXPLOSION7:
      output.left = zf * EXPLOSION7_X;
      output.top = zf * EXPLOSION7_Y;
      output.right = (zf * EXPLOSION7_X + zf * TILE_SIZE_X);
      output.bottom = (zf * EXPLOSION7_Y + zf * TILE_SIZE_Y);
      break;
    case SHELL_EXPLOSION6:
      output.left = zf * EXPLOSION6_X;
      output.top = zf * EXPLOSION6_Y;
      output.right = (zf * EXPLOSION6_X + zf * TILE_SIZE_X);
      output.bottom = (zf * EXPLOSION6_Y + zf * TILE_SIZE_Y);
      break;
    case SHELL_EXPLOSION5:
      output.left = zf * EXPLOSION5_X;
      output.top = zf * EXPLOSION5_Y;
      output.right = (zf * EXPLOSION5_X + zf * TILE_SIZE_X);
      output.bottom = (zf * EXPLOSION5_Y + zf * TILE_SIZE_Y);
      break;
    case SHELL_EXPLOSION4:
      output.left = zf * EXPLOSION4_X;
      output.top = zf * EXPLOSION4_Y;
      output.right = (zf * EXPLOSION4_X + zf * TILE_SIZE_X);
      output.bottom = (zf * EXPLOSION4_Y + zf * TILE_SIZE_Y);
      break;
    case SHELL_EXPLOSION3:
      output.left = zf * EXPLOSION3_X;
      output.top = zf * EXPLOSION3_Y;
      output.right = (zf * EXPLOSION3_X + zf * TILE_SIZE_X);
      output.bottom = (zf * EXPLOSION3_Y + zf * TILE_SIZE_Y);
      break;
    case SHELL_EXPLOSION2:
      output.left = zf * EXPLOSION2_X;
      output.top = zf * EXPLOSION2_Y;
      output.right = (zf * EXPLOSION2_X + zf * TILE_SIZE_X);
      output.bottom = (zf * EXPLOSION2_Y + zf * TILE_SIZE_Y);
      break;
    case SHELL_EXPLOSION1:
      output.left = zf * EXPLOSION1_X;
      output.top = zf * EXPLOSION1_Y;
      output.right = (zf * EXPLOSION1_X + zf * TILE_SIZE_X);
      output.bottom = (zf * EXPLOSION1_Y + zf * TILE_SIZE_Y);
      break;
    case SHELL_DIR0:
      output.left = zf * SHELL_0_X;
      output.top = zf * SHELL_0_Y;
      output.right = (zf * SHELL_0_X + zf * SHELL_0_WIDTH);
      output.bottom = (zf * SHELL_0_Y + zf * SHELL_0_HEIGHT);
      break;
    case SHELL_DIR1:
      output.left = zf * SHELL_1_X;
      output.top = zf * SHELL_1_Y;
      output.right = (zf * SHELL_1_X + zf * SHELL_1_WIDTH);
      output.bottom = (zf * SHELL_1_Y + zf * SHELL_1_HEIGHT);
      break;
    case SHELL_DIR2:
      output.left = zf * SHELL_2_X;
      output.top = zf * SHELL_2_Y;
      output.right = (zf * SHELL_2_X + zf * SHELL_2_WIDTH);
      output.bottom = (zf * SHELL_2_Y + zf * SHELL_2_HEIGHT);
      break;
    case SHELL_DIR3:
      output.left = zf * SHELL_3_X;
      output.top = zf * SHELL_3_Y;
      output.right = (zf * SHELL_3_X + zf * SHELL_3_WIDTH);
      output.bottom = (zf * SHELL_3_Y + zf * SHELL_3_HEIGHT);
      break;
    case SHELL_DIR4:
      output.left = zf * SHELL_4_X;
      output.top = zf * SHELL_4_Y;
      output.right = (zf * SHELL_4_X + zf * SHELL_4_WIDTH);
      output.bottom = (zf * SHELL_4_Y + zf * SHELL_4_HEIGHT);
      break;
    case SHELL_DIR5:
      output.left = zf * SHELL_5_X;
      output.top = zf * SHELL_5_Y;
      output.right = (zf * SHELL_5_X + zf * SHELL_5_WIDTH);
      output.bottom = (zf * SHELL_5_Y + zf * SHELL_5_HEIGHT);
      break;
    case SHELL_DIR6:
      output.left = zf * SHELL_6_X;
      output.top = zf * SHELL_6_Y;
      output.right = (zf * SHELL_6_X + zf * SHELL_6_WIDTH);
      output.bottom = (zf * SHELL_6_Y + zf * SHELL_6_HEIGHT);
      break;
    case SHELL_DIR7:
      output.left = zf * SHELL_7_X;
      output.top = zf * SHELL_7_Y;
      output.right = (zf * SHELL_7_X + zf * SHELL_7_WIDTH);
      output.bottom = (zf * SHELL_7_Y + zf * SHELL_7_HEIGHT);
      break;
    case SHELL_DIR8:
      output.left = zf * SHELL_8_X;
      output.top = zf * SHELL_8_Y;
      output.right = (zf * SHELL_8_X + zf * SHELL_8_WIDTH);
      output.bottom = (zf * SHELL_8_Y + zf * SHELL_8_HEIGHT);
      break;
    case SHELL_DIR9:
      output.left = zf * SHELL_9_X;
      output.top = zf * SHELL_9_Y;
      output.right = (zf * SHELL_9_X + zf * SHELL_9_WIDTH);
      output.bottom = (zf * SHELL_9_Y + zf * SHELL_9_HEIGHT);
      break;
    case SHELL_DIR10:
      output.left = zf * SHELL_10_X;
      output.top = zf * SHELL_10_Y;
      output.right = (zf * SHELL_10_X + zf * SHELL_10_WIDTH);
      output.bottom = (zf * SHELL_10_Y + zf * SHELL_10_HEIGHT);
      break;
    case SHELL_DIR11:
      output.left = zf * SHELL_11_X;
      output.top = zf * SHELL_11_Y;
      output.right = (zf * SHELL_11_X + zf * SHELL_11_WIDTH);
      output.bottom = (zf * SHELL_11_Y + zf * SHELL_11_HEIGHT);
      break;
    case SHELL_DIR12:
      output.left = zf * SHELL_12_X;
      output.top = zf * SHELL_12_Y;
      output.right = (zf * SHELL_12_X + zf * SHELL_12_WIDTH);
      output.bottom = (zf * SHELL_12_Y + zf * SHELL_12_HEIGHT);
      break;
    case SHELL_DIR13:
      output.left = zf * SHELL_13_X;
      output.top = zf * SHELL_13_Y;
      output.right = (zf * SHELL_13_X + zf * SHELL_13_WIDTH);
      output.bottom = (zf * SHELL_13_Y + zf * SHELL_13_HEIGHT);
      break;
    case SHELL_DIR14:
      output.left = zf * SHELL_14_X;
      output.top = zf * SHELL_14_Y;
      output.right = (zf * SHELL_14_X + zf * SHELL_14_WIDTH);
      output.bottom = (zf * SHELL_14_Y + zf * SHELL_14_HEIGHT);
      break;
  default:
    /* SHELL_DIR15 */
      output.left = zf * SHELL_15_X;
      output.top = zf * SHELL_15_Y;
      output.right = (zf * SHELL_15_X + zf * SHELL_15_WIDTH);
      output.bottom = (zf * SHELL_15_Y + zf * SHELL_15_HEIGHT);
    break;

    }
    count++;
    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
  }
}

/*********************************************************
*NAME:          drawMessages
*AUTHOR:        John Morrison
*CREATION DATE: 3/1/99
*LAST MODIFIED: 3/1/99
*PURPOSE:
*  Draws the message window
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  top    - The top string to draw
*  bottom - The bottom string to draw
*********************************************************/
void drawMessages(int xValue, int yValue, char *top, char *bottom) {
  RECT dest;   /* The dest square to draw it */
  RECT src;    /* Source Rectangle */
  HRESULT res; /* DX Return Values */
  HDC hDC;     /* DC of the Primary Surface */
  BYTE zf;     /* Scaling factor */

  zf = windowGetZoomFactor();

  dest.top = 0;
  dest.bottom = (zf * MESSAGE_HEIGHT);
  dest.left = 0;
  dest.right = (zf * MESSAGE_WIDTH);

  /* Get the DC and write the text */
  res = lpDDSMessages->lpVtbl->GetDC(lpDDSMessages, &hDC);
  if (SUCCEEDED(res)) {
    fontSelect(hDC);
    SetBkColor(hDC, RGB(0,0,0));
    SetTextColor(hDC, RGB(255,255,255));
    DrawTextA(hDC, top, (int) strlen(top), &dest, (DT_TOP | DT_RIGHT | DT_NOCLIP | DT_NOPREFIX));
    dest.top = (zf * MESSAGE_TEXT_HEIGHT);
    dest.bottom = (zf * MESSAGE_HEIGHT);
    dest.left = 0;
    dest.right = (zf * MESSAGE_WIDTH);
    DrawTextA(hDC, bottom, (int) strlen(bottom), &dest, (DT_BOTTOM | DT_RIGHT | DT_NOCLIP | DT_NOPREFIX));
    lpDDSMessages->lpVtbl->ReleaseDC(lpDDSMessages, hDC);

    /* Blt the messages buffer to the primary window */
    dest.top = yValue + (zf * MESSAGE_TOP);
    dest.bottom = yValue + (zf * MESSAGE_TOP) + (zf * MESSAGE_HEIGHT);
    dest.left = xValue + (zf * MESSAGE_LEFT); 
    dest.right = xValue + (zf * MESSAGE_LEFT) + (zf * MESSAGE_WIDTH); 
    src.top = 0;
    src.bottom = (zf * MESSAGE_HEIGHT);
    src.left = 0;
    src.right = (zf * MESSAGE_WIDTH);
    lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest, lpDDSMessages, &src, DDBLT_WAIT, NULL);
    
  }
}


/*********************************************************
*NAME:          drawTanks
*AUTHOR:        John Morrison
*CREATION DATE: 06/01/99
*LAST MODIFIED: 26/01/02
*PURPOSE:
*  Draws tanks on the backbuffer.
*
*ARGUMENTS:
*  tks - The screen Tanks data structure 
*********************************************************/
void drawTanks(screenTanks *tks) {
  int x;       /* X and Y Co-ordinates */
  int y; 
  RECT output; /* Source Rectangle */
  BYTE count;  /* Looping Variable */
  BYTE total;  /* Total number of tanks on the screen */
  /* Current tank Stuff */
  char playerName[MAX_PATH];        /* Player name */
  BYTE frame;                       /* Frame id */
  BYTE mx;                          /* Map X and Y Co-ords */
  BYTE my;
  BYTE px;                          /* Pixel offsets */
  BYTE py;
  BYTE zoomFactor;                  /* Scaling Factor */
  BYTE playerNum;                   /* Tanks Player Number */
  
  count = 1;
  total = screenTanksGetNumEntries(tks);
  zoomFactor = windowGetZoomFactor();
  
  while (count <= total) {
    screenTanksGetItem(tks, count, &mx, &my, &px, &py, &frame, &playerNum, playerName);
    switch (frame) {
    case TANK_SELF_0:
      output.left = TANK_SELF_0_X;
      output.top = TANK_SELF_0_Y;
      break;
    case TANK_SELF_1:
      output.left = TANK_SELF_1_X;
      output.top = TANK_SELF_1_Y;
      break;
    case TANK_SELF_2:
      output.left = TANK_SELF_2_X;
      output.top = TANK_SELF_2_Y;
      break;
    case TANK_SELF_3:
      output.left = TANK_SELF_3_X;
      output.top = TANK_SELF_3_Y;
      break;
    case TANK_SELF_4:
      output.left = TANK_SELF_4_X;
      output.top = TANK_SELF_4_Y;
      break;
    case TANK_SELF_5:
      output.left = TANK_SELF_5_X;
      output.top = TANK_SELF_5_Y;
      break;
    case TANK_SELF_6:
      output.left = TANK_SELF_6_X;
      output.top = TANK_SELF_6_Y;
      break;
    case TANK_SELF_7:
      output.left = TANK_SELF_7_X;
      output.top = TANK_SELF_7_Y;
      break;
    case TANK_SELF_8:
      output.left = TANK_SELF_8_X;
      output.top = TANK_SELF_8_Y;
      break;
    case TANK_SELF_9:
      output.left = TANK_SELF_9_X;
      output.top = TANK_SELF_9_Y;
      break;
    case TANK_SELF_10:
      output.left = TANK_SELF_10_X;
      output.top = TANK_SELF_10_Y;
      break;
    case TANK_SELF_11:
      output.left = TANK_SELF_11_X;
      output.top = TANK_SELF_11_Y;
      break;
    case TANK_SELF_12:
      output.left = TANK_SELF_12_X;
      output.top = TANK_SELF_12_Y;
      break;
    case TANK_SELF_13:
      output.left = TANK_SELF_13_X;
      output.top = TANK_SELF_13_Y;
      break;
    case TANK_SELF_14:
      output.left = TANK_SELF_14_X;
      output.top = TANK_SELF_14_Y;
      break;
    case TANK_SELF_15:
      output.left = TANK_SELF_15_X;
      output.top = TANK_SELF_15_Y;
      break;
    case TANK_SELFBOAT_0:
      output.left = TANK_SELFBOAT_0_X;
      output.top = TANK_SELFBOAT_0_Y;
      break;
    case TANK_SELFBOAT_1:
      output.left = TANK_SELFBOAT_1_X;
      output.top = TANK_SELFBOAT_1_Y;
      break;
    case TANK_SELFBOAT_2:
      output.left = TANK_SELFBOAT_2_X;
      output.top = TANK_SELFBOAT_2_Y;
      break;
    case TANK_SELFBOAT_3:
      output.left = TANK_SELFBOAT_3_X;
      output.top = TANK_SELFBOAT_3_Y;
      break;
    case TANK_SELFBOAT_4:
      output.left = TANK_SELFBOAT_4_X;
      output.top = TANK_SELFBOAT_4_Y;
      break;
    case TANK_SELFBOAT_5:
      output.left = TANK_SELFBOAT_5_X;
      output.top = TANK_SELFBOAT_5_Y;
      break;
    case TANK_SELFBOAT_6:
      output.left = TANK_SELFBOAT_6_X;
      output.top = TANK_SELFBOAT_6_Y;
      break;
    case TANK_SELFBOAT_7:
      output.left = TANK_SELFBOAT_7_X;
      output.top = TANK_SELFBOAT_7_Y;
      break;
    case TANK_SELFBOAT_8:
      output.left = TANK_SELFBOAT_8_X;
      output.top = TANK_SELFBOAT_8_Y;
      break;
    case TANK_SELFBOAT_9:
      output.left = TANK_SELFBOAT_9_X;
      output.top = TANK_SELFBOAT_9_Y;
      break;
    case TANK_SELFBOAT_10:
      output.left = TANK_SELFBOAT_10_X;
      output.top = TANK_SELFBOAT_10_Y;
      break;
    case TANK_SELFBOAT_11:
      output.left = TANK_SELFBOAT_11_X;
      output.top = TANK_SELFBOAT_11_Y;
      break;
    case TANK_SELFBOAT_12:
      output.left = TANK_SELFBOAT_12_X;
      output.top = TANK_SELFBOAT_12_Y;
      break;
    case TANK_SELFBOAT_13:
      output.left = TANK_SELFBOAT_13_X;
      output.top = TANK_SELFBOAT_13_Y;
      break;
    case TANK_SELFBOAT_14:
      output.left = TANK_SELFBOAT_14_X;
      output.top = TANK_SELFBOAT_14_Y;
      break;
    case TANK_SELFBOAT_15: 
      output.left = TANK_SELFBOAT_15_X;
      output.top = TANK_SELFBOAT_15_Y;
      break;
    case TANK_GOOD_0:
      output.left = TANK_GOOD_0_X;
      output.top = TANK_GOOD_0_Y;
      break;
    case TANK_GOOD_1:
      output.left = TANK_GOOD_1_X;
      output.top = TANK_GOOD_1_Y;
      break;
    case TANK_GOOD_2:
      output.left = TANK_GOOD_2_X;
      output.top = TANK_GOOD_2_Y;
      break;
    case TANK_GOOD_3:
      output.left = TANK_GOOD_3_X;
      output.top = TANK_GOOD_3_Y;
      break;
    case TANK_GOOD_4:
      output.left = TANK_GOOD_4_X;
      output.top = TANK_GOOD_4_Y;
      break;
    case TANK_GOOD_5:
      output.left = TANK_GOOD_5_X;
      output.top = TANK_GOOD_5_Y;
      break;
    case TANK_GOOD_6:
      output.left = TANK_GOOD_6_X;
      output.top = TANK_GOOD_6_Y;
      break;
    case TANK_GOOD_7:
      output.left = TANK_GOOD_7_X;
      output.top = TANK_GOOD_7_Y;
      break;
    case TANK_GOOD_8:
      output.left = TANK_GOOD_8_X;
      output.top = TANK_GOOD_8_Y;
      break;
    case TANK_GOOD_9:
      output.left = TANK_GOOD_9_X;
      output.top = TANK_GOOD_9_Y;
      break;
    case TANK_GOOD_10:
      output.left = TANK_GOOD_10_X;
      output.top = TANK_GOOD_10_Y;
      break;
    case TANK_GOOD_11:
      output.left = TANK_GOOD_11_X;
      output.top = TANK_GOOD_11_Y;
      break;
    case TANK_GOOD_12:
      output.left = TANK_GOOD_12_X;
      output.top = TANK_GOOD_12_Y;
      break;
    case TANK_GOOD_13:
      output.left = TANK_GOOD_13_X;
      output.top = TANK_GOOD_13_Y;
      break;
    case TANK_GOOD_14:
      output.left = TANK_GOOD_14_X;
      output.top = TANK_GOOD_14_Y;
      break;
    case TANK_GOOD_15:
      output.left = TANK_GOOD_15_X;
      output.top = TANK_GOOD_15_Y;
      break;
    case TANK_GOODBOAT_0:
      output.left = TANK_GOODBOAT_0_X;
      output.top = TANK_GOODBOAT_0_Y;
      break;
    case TANK_GOODBOAT_1:
      output.left = TANK_GOODBOAT_1_X;
      output.top = TANK_GOODBOAT_1_Y;
      break;
    case TANK_GOODBOAT_2:
      output.left = TANK_GOODBOAT_2_X;
      output.top = TANK_GOODBOAT_2_Y;
      break;
    case TANK_GOODBOAT_3:
      output.left = TANK_GOODBOAT_3_X;
      output.top = TANK_GOODBOAT_3_Y;
      break;
    case TANK_GOODBOAT_4:
      output.left = TANK_GOODBOAT_4_X;
      output.top = TANK_GOODBOAT_4_Y;
      break;
    case TANK_GOODBOAT_5:
      output.left = TANK_GOODBOAT_5_X;
      output.top = TANK_GOODBOAT_5_Y;
      break;
    case TANK_GOODBOAT_6:
      output.left = TANK_GOODBOAT_6_X;
      output.top = TANK_GOODBOAT_6_Y;
      break;
    case TANK_GOODBOAT_7:
      output.left = TANK_GOODBOAT_7_X;
      output.top = TANK_GOODBOAT_7_Y;
      break;
    case TANK_GOODBOAT_8:
      output.left = TANK_GOODBOAT_8_X;
      output.top = TANK_GOODBOAT_8_Y;
      break;
    case TANK_GOODBOAT_9:
      output.left = TANK_GOODBOAT_9_X;
      output.top = TANK_GOODBOAT_9_Y;
      break;
    case TANK_GOODBOAT_10:
      output.left = TANK_GOODBOAT_10_X;
      output.top = TANK_GOODBOAT_10_Y;
      break;
    case TANK_GOODBOAT_11:
      output.left = TANK_GOODBOAT_11_X;
      output.top = TANK_GOODBOAT_11_Y;
      break;
    case TANK_GOODBOAT_12:
      output.left = TANK_GOODBOAT_12_X;
      output.top = TANK_GOODBOAT_12_Y;
      break;
    case TANK_GOODBOAT_13:
      output.left = TANK_GOODBOAT_13_X;
      output.top = TANK_GOODBOAT_13_Y;
      break;
    case TANK_GOODBOAT_14:
      output.left = TANK_GOODBOAT_14_X;
      output.top = TANK_GOODBOAT_14_Y;
      break;
    case TANK_GOODBOAT_15: 
      output.left = TANK_GOODBOAT_15_X;
      output.top = TANK_GOODBOAT_15_Y;
      break;
    case TANK_EVIL_0:
      output.left = TANK_EVIL_0_X;
      output.top = TANK_EVIL_0_Y;
      break;
    case TANK_EVIL_1:
      output.left = TANK_EVIL_1_X;
      output.top = TANK_EVIL_1_Y;
      break;
    case TANK_EVIL_2:
      output.left = TANK_EVIL_2_X;
      output.top = TANK_EVIL_2_Y;
      break;
    case TANK_EVIL_3:
      output.left = TANK_EVIL_3_X;
      output.top = TANK_EVIL_3_Y;
      break;
    case TANK_EVIL_4:
      output.left = TANK_EVIL_4_X;
      output.top = TANK_EVIL_4_Y;
      break;
    case TANK_EVIL_5:
      output.left = TANK_EVIL_5_X;
      output.top = TANK_EVIL_5_Y;
      break;
    case TANK_EVIL_6:
      output.left = TANK_EVIL_6_X;
      output.top = TANK_EVIL_6_Y;
      break;
    case TANK_EVIL_7:
      output.left = TANK_EVIL_7_X;
      output.top = TANK_EVIL_7_Y;
      break;
    case TANK_EVIL_8:
      output.left = TANK_EVIL_8_X;
      output.top = TANK_EVIL_8_Y;
      break;
    case TANK_EVIL_9:
      output.left = TANK_EVIL_9_X;
      output.top = TANK_EVIL_9_Y;
      break;
    case TANK_EVIL_10:
      output.left = TANK_EVIL_10_X;
      output.top = TANK_EVIL_10_Y;
      break;
    case TANK_EVIL_11:
      output.left = TANK_EVIL_11_X;
      output.top = TANK_EVIL_11_Y;
      break;
    case TANK_EVIL_12:
      output.left = TANK_EVIL_12_X;
      output.top = TANK_EVIL_12_Y;
      break;
    case TANK_EVIL_13:
      output.left = TANK_EVIL_13_X;
      output.top = TANK_EVIL_13_Y;
      break;
    case TANK_EVIL_14:
      output.left = TANK_EVIL_14_X;
      output.top = TANK_EVIL_14_Y;
      break;
    case TANK_EVIL_15:
      output.left = TANK_EVIL_15_X;
      output.top = TANK_EVIL_15_Y;
      break;
    case TANK_EVILBOAT_0:
      output.left = TANK_EVILBOAT_0_X;
      output.top = TANK_EVILBOAT_0_Y;
      break;
    case TANK_EVILBOAT_1:
      output.left = TANK_EVILBOAT_1_X;
      output.top = TANK_EVILBOAT_1_Y;
      break;
    case TANK_EVILBOAT_2:
      output.left = TANK_EVILBOAT_2_X;
      output.top = TANK_EVILBOAT_2_Y;
      break;
    case TANK_EVILBOAT_3:
      output.left = TANK_EVILBOAT_3_X;
      output.top = TANK_EVILBOAT_3_Y;
      break;
    case TANK_EVILBOAT_4:
      output.left = TANK_EVILBOAT_4_X;
      output.top = TANK_EVILBOAT_4_Y;
      break;
    case TANK_EVILBOAT_5:
      output.left = TANK_EVILBOAT_5_X;
      output.top = TANK_EVILBOAT_5_Y;
      break;
    case TANK_EVILBOAT_6:
      output.left = TANK_EVILBOAT_6_X;
      output.top = TANK_EVILBOAT_6_Y;
      break;
    case TANK_EVILBOAT_7:
      output.left = TANK_EVILBOAT_7_X;
      output.top = TANK_EVILBOAT_7_Y;
      break;
    case TANK_EVILBOAT_8:
      output.left = TANK_EVILBOAT_8_X;
      output.top = TANK_EVILBOAT_8_Y;
      break;
    case TANK_EVILBOAT_9:
      output.left = TANK_EVILBOAT_9_X;
      output.top = TANK_EVILBOAT_9_Y;
      break;
    case TANK_EVILBOAT_10:
      output.left = TANK_EVILBOAT_10_X;
      output.top = TANK_EVILBOAT_10_Y;
      break;
    case TANK_EVILBOAT_11:
      output.left = TANK_EVILBOAT_11_X;
      output.top = TANK_EVILBOAT_11_Y;
      break;
    case TANK_EVILBOAT_12:
      output.left = TANK_EVILBOAT_12_X;
      output.top = TANK_EVILBOAT_12_Y;
      break;
    case TANK_EVILBOAT_13:
      output.left = TANK_EVILBOAT_13_X;
      output.top = TANK_EVILBOAT_13_Y;
      break;
    case TANK_EVILBOAT_14:
      output.left = TANK_EVILBOAT_14_X;
      output.top = TANK_EVILBOAT_14_Y;
      break;
    case TANK_EVILBOAT_15: 
      output.left = TANK_EVILBOAT_15_X;
      output.top = TANK_EVILBOAT_15_Y;
      break;

    default:
      /* TANK_TRANSPARENT */
      output.left = TANK_TRANSPARENT_X;
      output.top = TANK_TRANSPARENT_Y;
    }


/*
  test for position of tank
  output.left = zoomFactor * BUILD_SINGLE_X;
	output.top = zoomFactor * BUILD_SINGLE_Y;
*/

    /* Output */
    output.left *= zoomFactor;
    output.top *= zoomFactor;
    output.right = output.left + (zoomFactor * TILE_SIZE_X);
    output.bottom = output.top + (zoomFactor * TILE_SIZE_Y);
    px += 2;
    py += 2;
    x = mx * (zoomFactor * TILE_SIZE_X) + (zoomFactor * px);
    y = my * (zoomFactor * TILE_SIZE_Y) + (zoomFactor * py);
    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY); 
    /* Output the label */
    drawTankLabel(playerName, playerNum, mx, my, px, py);

    count++;
  }

}

/*********************************************************
*NAME:          drawKillsDeaths
*AUTHOR:        John Morrison
*CREATION DATE:  8/1/99
*LAST MODIFIED:  8/1/99
*PURPOSE:
*  Draws the tanks kills/deaths
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  kills  - The number of kills the tank has.
*  deaths - The number of times the tank has died
*********************************************************/
void drawKillsDeaths(int xValue, int yValue, int kills, int deaths) {
  RECT textRect;         /* Text rectangle */
  RECT dest;             /* Destination rectangle */
  HRESULT res;           /* DX Return Values */
  HDC hDC;               /* DC of the Primary Surface */
  char str[STRING_SIZE]; /* Holds the charectors to print */
  BYTE zf;               /* Zoom Factor */

  zf = windowGetZoomFactor();
  
  textRect.left = 0;
  textRect.right = TANK_LABEL_WIDTH;
  textRect.top = 0;
  textRect.bottom = TANK_LABEL_HEIGHT;

  str[0] = EMPTY_CHAR;
  sprintf(str, "%d", kills);

  /* Get the DC and write the text */
  res = lpDDSTankLabels->lpVtbl->GetDC(lpDDSTankLabels, &hDC);
  if (SUCCEEDED(res)) {
    fontSelect(hDC);
    SetBkColor(hDC, RGB(0,0,0));
    SetTextColor(hDC, RGB(255,255,255));
    DrawTextA(hDC, str, (int) strlen(str), &textRect, (DT_CALCRECT | DT_TOP | DT_NOCLIP));
    DrawTextA(hDC, str, (int) strlen(str), &textRect, (DT_TOP | DT_NOCLIP));
    lpDDSTankLabels->lpVtbl->ReleaseDC(lpDDSTankLabels, hDC);
    dest.top = yValue + (zf * STATUS_KILLS_TOP);
    dest.bottom = dest.top + textRect.bottom;
    dest.left = xValue + (zf * STATUS_KILLS_LEFT);
    dest.right = dest.left + textRect.right;
    lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest, lpDDSTankLabels, &textRect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
  }

  res = lpDDSTankLabels->lpVtbl->GetDC(lpDDSTankLabels, &hDC);
  if (SUCCEEDED(res)) {
    textRect.left = 0;
    textRect.right = TANK_LABEL_WIDTH;
    textRect.top = 0;
    textRect.bottom = TANK_LABEL_HEIGHT;
    str[0] = '\0';
    sprintf(str, "%d", deaths);    
    fontSelect(hDC);
    SetBkColor(hDC, RGB(0,0,0));
    SetTextColor(hDC, RGB(255,255,255));
    DrawTextA(hDC, str, (int) strlen(str), &textRect, (DT_CALCRECT | DT_TOP | DT_NOCLIP));
    DrawTextA(hDC, str, (int) strlen(str), &textRect, (DT_TOP | DT_NOCLIP));
    lpDDSTankLabels->lpVtbl->ReleaseDC(lpDDSTankLabels, hDC);
    dest.top = yValue + (zf * STATUS_DEATHS_TOP);
    dest.bottom = dest.top + textRect.bottom;
    dest.left = xValue + (zf * STATUS_DEATHS_LEFT);
    dest.right = dest.left + textRect.right;

    lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest, lpDDSTankLabels, &textRect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
  }
}

/*********************************************************
*NAME:          drawStatusBaseBars
*AUTHOR:        John Morrison
*CREATION DATE: 11/1/98
*LAST MODIFIED: 11/1/98
*PURPOSE:
*  Draws the base armour, shells etc bars.
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  shells  - Number of shells
*  mines   - Number of mines
*  armour  - Amount of armour
*  redraw  - If set to true use the redraw last amounts
*********************************************************/
void drawStatusBaseBars(int xValue, int yValue, BYTE shells, BYTE mines, BYTE armour, bool redraw) {
  RECT dest;              /* The dest square to draw it */
  DDBLTFX fx;             /* FX for the colour fills */
  DDPIXELFORMAT ddpf;     /* DD Pixel Format to get green */
  static BYTE lastShells = 0;  /* Last amount of stuff to save on rendering and flicker */
  static BYTE lastMines = 0; 
  static BYTE lastArmour = 0;
  BYTE zf;                /* Zoom Factor */

  zf = windowGetZoomFactor();

  if (lastShells != shells || lastMines != mines || lastArmour != armour || redraw == TRUE) {
    if (redraw == FALSE) {
      lastShells = shells;
      lastMines = mines;
      lastArmour = armour;
    } else {
      shells = lastShells;
      mines = lastMines;
      armour = lastArmour;
    }
    /* Get Pixel Format and set color*/
    ddpf.dwSize = sizeof(ddpf);
    lpDDSPrimary->lpVtbl->GetPixelFormat(lpDDSPrimary, &ddpf); 
    ZeroMemory(&fx, sizeof(fx));
    fx.dwSize = sizeof(fx);
    fx.dwFillColor =  0;

    /* Make the area black first */

    dest.top = yValue + (zf * STATUS_BASE_SHELLS);
    dest.bottom = yValue + (zf * STATUS_BASE_MINES) + (zf * STATUS_BASE_BARS_HEIGHT);
    dest.left = xValue + (zf * STATUS_BASE_BARS_LEFT);
    dest.right = xValue + (zf * STATUS_BASE_BARS_LEFT) + (zf * STATUS_BASE_BARS_MAX_WIDTH);
    lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);


    if (shells != 0 || mines != 0 || armour != 0) {
      fx.dwFillColor =  ddpf.dwGBitMask;  
      /* Shells */
      dest.top = yValue + (zf * STATUS_BASE_SHELLS);
      dest.left = xValue + (zf * STATUS_BASE_BARS_LEFT);
      dest.bottom = yValue + (zf * STATUS_BASE_SHELLS) + (zf * STATUS_BASE_BARS_HEIGHT);
      dest.right = (long) (xValue + (zf * STATUS_BASE_BARS_LEFT) + (shells * (zf * BAR_BASE_MULTIPLY)));
      lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

      /* Mines */
      dest.top = yValue + (zf * STATUS_BASE_MINES);
      dest.bottom = yValue + (zf * STATUS_BASE_MINES) + (zf * STATUS_BASE_BARS_HEIGHT);
      dest.right = (long) (xValue + (zf * STATUS_BASE_BARS_LEFT) + (mines * (zf * BAR_BASE_MULTIPLY)));
      lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

      /* Armour */
      dest.top = yValue + (zf * STATUS_BASE_ARMOUR);
      dest.bottom = yValue + (zf * STATUS_BASE_ARMOUR) + (zf * STATUS_BASE_BARS_HEIGHT);
      dest.right = (long) (xValue + (zf * STATUS_BASE_BARS_LEFT) + (armour * (zf * BAR_BASE_MULTIPLY)));
      lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest , NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
    }
  }
}

/*********************************************************
*NAME:          drawTanks
*AUTHOR:        John Morrison
*CREATION DATE: 17/1/99
*LAST MODIFIED: 17/1/99
*PURPOSE:
*  Draws the builder
*
*ARGUMENTS:
*  lgms - The screenLgm data structure 
*********************************************************/
void drawLGMs(screenLgm *lgms) {
  BYTE total;   /* Total number to draw */
  BYTE frame;   /* Current LGM screen info */
  BYTE mx;
  BYTE my;
  BYTE px;
  BYTE py;
  int x;       /* X and Y Co-ordinates */
  int y; 
  RECT output; /* Source Rectangle */
  BYTE count;  /* Looping variable */
  BYTE zf;     /* Zoom factor */

  total = screenLgmGetNumEntries(lgms);
  zf = windowGetZoomFactor();

  for (count=1;count<=total;count++) {
    screenLgmGetItem(lgms, count, &mx, &my, &px, &py, &frame);
    switch (frame) {
      case LGM0:
        output.left = zf * LGM0_X;
        output.top = zf * LGM0_Y;
        output.right = zf * LGM0_X + zf * LGM_WIDTH;
        output.bottom = zf * LGM0_Y + zf * LGM_HEIGHT;
        break;
      case LGM1:
        output.left = zf * LGM1_X;
        output.top = zf * LGM1_Y;
        output.right = zf * LGM1_X + zf * LGM_WIDTH;
        output.bottom = zf * LGM1_Y + zf * LGM_HEIGHT;
        break;
      case LGM2:
        output.left = zf * LGM2_X;
        output.top = zf * LGM2_Y;
        output.right = zf * LGM2_X + zf * LGM_WIDTH;
        output.bottom = zf * LGM2_Y + zf * LGM_HEIGHT;
        break;
      default:
        /* LGM3 - Helicopter */
        output.left = zf * LGM_HELICOPTER_X;
        output.top = zf * LGM_HELICOPTER_Y;
        output.right = zf * LGM_HELICOPTER_X + zf * TILE_SIZE_X;
        output.bottom = zf * LGM_HELICOPTER_Y + zf * TILE_SIZE_Y;
        break;
    }
    x = (zf * mx * TILE_SIZE_X) + (zf * px);
    y = (zf * my * TILE_SIZE_Y) + (zf * py);
    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
  }
}

/*********************************************************
*NAME:          drawCopyManStatus
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/98
*LAST MODIFIED: 18/1/98
*PURPOSE:
*  Copys the LGM status on to the primary buffer
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawCopyManStatus(int xValue, int yValue) {
  RECT dest; /* Destination location */
  BYTE zf;   /* Zoom Factor */

  zf = windowGetZoomFactor();

  dest.left = xValue + (zf * MAN_STATUS_X);
  dest.top = yValue + (zf * MAN_STATUS_Y);
  dest.right = dest.left + (zf * MAN_STATUS_WIDTH);
  dest.bottom = dest.top + (zf * MAN_STATUS_HEIGHT);
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest, lpDDSManStatus, NULL, DDBLT_WAIT, NULL);
}

/*********************************************************
*NAME:          drawSetManStatus
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/98
*LAST MODIFIED: 18/1/98
*PURPOSE:
*  Draws the man arrow status. If isDead is set to true
*  then the angle is ignored.
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*  isDead - Is the man dead
*  angle  - The angle the item is facing
*********************************************************/
void drawSetManStatus(int xValue, int yValue, bool isDead, TURNTYPE angle) {
  TURNTYPE oldAngle; /* Copy of the angle parameter */
  HDC hDC;           /* Temp DC of the DDS */
  POINT dummy;       /* Ignored function parameter */
  HRESULT res;       /* Function return */
  double dbAngle;    /* Angle in radians */
  double dbTemp;
  BYTE zoomFactor;   /* Zooming factor */
  int addX;          /* X And and Y co-ordinates */
  int addY;

  zoomFactor = windowGetZoomFactor();
  drawSetManClear(); /* Clear the display */

  oldAngle = angle;
  angle += BRADIANS_SOUTH;
  if (angle >= BRADIANS_MAX) {
    angle -= BRADIANS_MAX;
  }

  if (angle >= BRADIANS_NORTH && angle < BRADIANS_EAST) {
    /* Convert bradians to degrees */
    dbAngle = (DEGREES_MAX / BRADIANS_MAX) * angle;
    /* Convert degrees to radians */
    dbAngle = (dbAngle / DEGREES_MAX) * RADIANS_MAX;      


    addX = (zoomFactor * MAN_STATUS_CENTER_X);
    addY = (zoomFactor * MAN_STATUS_CENTER_Y);
    dbTemp = MAN_STATUS_RADIUS * (sin(dbAngle));
    addX += (int) (zoomFactor * dbTemp);
    dbTemp = MAN_STATUS_RADIUS * (cos(dbAngle));
    addY -= (int) (zoomFactor * dbTemp);


  } else if (angle >= BRADIANS_EAST && angle < BRADIANS_SOUTH) {
    angle = BRADIANS_SOUTH - angle;
    /* Convert bradians to degrees */
    dbAngle = (DEGREES_MAX / BRADIANS_MAX) * angle;
    /* Convert degrees to radians */
    dbAngle = (dbAngle / DEGREES_MAX) * RADIANS_MAX;      


    addX = (zoomFactor * MAN_STATUS_CENTER_X);
    addY = (zoomFactor * MAN_STATUS_CENTER_Y);
    dbTemp = MAN_STATUS_RADIUS * (sin(dbAngle));
    addX += (int) (zoomFactor * dbTemp);
    dbTemp = MAN_STATUS_RADIUS * (cos(dbAngle));
    addY += (int) (zoomFactor * dbTemp);
  } else if (angle >= BRADIANS_SOUTH && angle < BRADIANS_WEST) {
    angle = BRADIANS_WEST - angle;
    angle = BRADIANS_EAST - angle;
    /* Convert bradians to degrees */
    dbAngle = (DEGREES_MAX / BRADIANS_MAX) * angle;
    /* Convert degrees to radians */
    dbAngle = (dbAngle / DEGREES_MAX) * RADIANS_MAX;      


    addX = (zoomFactor * MAN_STATUS_CENTER_X);
    addY = (zoomFactor * MAN_STATUS_CENTER_Y);
    dbTemp = MAN_STATUS_RADIUS * (sin(dbAngle));
    addX -= (int) (zoomFactor * dbTemp);
    dbTemp = MAN_STATUS_RADIUS * (cos(dbAngle));
    addY += (int) (zoomFactor * dbTemp);

  } else {
    angle = (float) BRADIANS_MAX - angle;
    /* Convert bradians to degrees */
    dbAngle = (DEGREES_MAX / BRADIANS_MAX) * angle;
    /* Convert degrees to radians */
    dbAngle = (dbAngle / DEGREES_MAX) * RADIANS_MAX;      

    addX = (zoomFactor * MAN_STATUS_CENTER_X);
    addY = (zoomFactor * MAN_STATUS_CENTER_Y);
    dbTemp = MAN_STATUS_RADIUS * (sin(dbAngle));
    addX -= (int) (zoomFactor * dbTemp);
    dbTemp = MAN_STATUS_RADIUS * (cos(dbAngle));
    addY -= (int) (zoomFactor * dbTemp);
  }


  res = lpDDSManStatus->lpVtbl->GetDC(lpDDSManStatus, &hDC);
  if (SUCCEEDED(res)) {
    if (isDead == TRUE) {
      /* Draw dead circle */
      SelectObject(hDC, hManBrush);
      Ellipse(hDC, 0, 0, (zoomFactor * MAN_STATUS_WIDTH), (zoomFactor * MAN_STATUS_HEIGHT));
    } else {
      /* Draw Arrow */
      SelectObject(hDC, hNarrowBrush);
      SelectObject(hDC, hManPen);
      Ellipse(hDC, 0, 0, (zoomFactor * MAN_STATUS_WIDTH), (zoomFactor * MAN_STATUS_HEIGHT));
      MoveToEx(hDC, (zoomFactor * MAN_STATUS_CENTER_X), (zoomFactor * MAN_STATUS_CENTER_Y), &dummy);
      LineTo(hDC, addX, addY);

    }
    lpDDSManStatus->lpVtbl->ReleaseDC(lpDDSManStatus, hDC);
  }



  /* Copy the surface */
  drawCopyManStatus(xValue, yValue);
}

/*********************************************************
*NAME:          drawSetManClear
*AUTHOR:        John Morrison
*CREATION DATE: 18/1/98
*LAST MODIFIED: 18/1/98
*PURPOSE:
*  Clears the lgm status display.
*
*ARGUMENTS:
*********************************************************/
void drawSetManClear(void) {
  DDBLTFX fx;  /* FX for the colour fills */

  ZeroMemory(&fx, sizeof(fx));
  fx.dwSize = sizeof(fx);
  fx.dwFillColor =  0;
  lpDDSManStatus->lpVtbl->Blt(lpDDSManStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx); 
}

/*********************************************************
*NAME:          drawSetBasesStatusClear
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Clears the bases status display.
*
*ARGUMENTS:
*********************************************************/
void drawSetBasesStatusClear(void) {
  DDBLTFX fx;       /* FX for the colour fills */
  RECT src;        /* Used for copying the bases & pills icon in */
  RECT dest;       /* Used for copying the bases & pills icon in */
  BYTE zoomFactor; /* Scaling Factor */

  zoomFactor = windowGetZoomFactor();
  ZeroMemory(&fx, sizeof(fx));
  fx.dwSize = sizeof(fx);
  fx.dwFillColor =  0;
  lpDDSBasesStatus->lpVtbl->Blt(lpDDSBasesStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
  src.left = zoomFactor * BASE_GOOD_X;
  src.top = zoomFactor * BASE_GOOD_Y;
  src.right = zoomFactor * BASE_GOOD_X + zoomFactor * TILE_SIZE_X;
  src.bottom = zoomFactor * BASE_GOOD_Y + zoomFactor * TILE_SIZE_Y;
  dest.top = (zoomFactor * STATUS_BASES_MIDDLE_ICON_Y);
  dest.left = (zoomFactor * STATUS_BASES_MIDDLE_ICON_X);
  dest.right = dest.left + (zoomFactor * TILE_SIZE_X);
  dest.bottom = dest.top + (zoomFactor * TILE_SIZE_Y);
  lpDDSBasesStatus->lpVtbl->Blt(lpDDSBasesStatus, &dest, lpDDSTiles, &src, DDBLT_WAIT, NULL);

}

/*********************************************************
*NAME:          drawSetPillsStatusClear
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Clears the pills status display.
*
*ARGUMENTS:
*********************************************************/
void drawSetPillsStatusClear(void) {
  DDBLTFX fx;  /* FX for the colour fills */
  RECT src;    /* Used for copying the bases & pills icon in */
  RECT dest;   /* Used for copying the bases & pills icon in */
  BYTE zoomFactor; /* Scaling Factor */

  zoomFactor = windowGetZoomFactor();
  ZeroMemory(&fx, sizeof(fx));
  fx.dwSize = sizeof(fx);
  fx.dwFillColor =  0;
  lpDDSPillsStatus->lpVtbl->Blt(lpDDSPillsStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
  src.left = zoomFactor * PILL_GOOD15_X;
  src.top = zoomFactor * PILL_GOOD15_Y;
  src.right = zoomFactor * PILL_GOOD15_X + zoomFactor * TILE_SIZE_X;
  src.bottom = zoomFactor * PILL_GOOD15_Y + zoomFactor * TILE_SIZE_Y;
  dest.top = (zoomFactor * STATUS_PILLS_MIDDLE_ICON_Y);
  dest.left = (zoomFactor * STATUS_PILLS_MIDDLE_ICON_X);
  dest.right = dest.left + (zoomFactor * TILE_SIZE_X);
  dest.bottom = dest.top + (zoomFactor * TILE_SIZE_Y);
  lpDDSPillsStatus->lpVtbl->Blt(lpDDSPillsStatus, &dest, lpDDSTiles, &src, DDBLT_WAIT, NULL);
}

/*********************************************************
*NAME:          drawSetTanksStatusClear
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/98
*LAST MODIFIED: 14/2/98
*PURPOSE:
*  Clears the tanks status display.
*
*ARGUMENTS:
*********************************************************/
void drawSetTanksStatusClear(void) {
  DDBLTFX fx;  /* FX for the colour fills */
  RECT src;    /* Used for copying the bases & pills icon in */
  RECT dest;   /* Used for copying the bases & pills icon in */
  BYTE zoomFactor; /* Scaling Factor */

  zoomFactor = windowGetZoomFactor();
  ZeroMemory(&fx, sizeof(fx));
  fx.dwSize = sizeof(fx);
  fx.dwFillColor =  0;
  lpDDSTankStatus->lpVtbl->Blt(lpDDSTankStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
  src.left = zoomFactor * TANK_SELF_0_X;
  src.top = zoomFactor * TANK_SELF_0_Y;
  src.right = TANK_SELF_0_X + zoomFactor * TILE_SIZE_X;
  src.bottom =zoomFactor *  TANK_SELF_0_Y + zoomFactor * TILE_SIZE_Y;
  dest.top = (zoomFactor * STATUS_TANKS_MIDDLE_ICON_Y);
  dest.left = (zoomFactor * STATUS_TANKS_MIDDLE_ICON_X);
  dest.right = dest.left + (zoomFactor * TILE_SIZE_X);
  dest.bottom = dest.top + (zoomFactor * TILE_SIZE_Y);
  lpDDSTankStatus->lpVtbl->Blt(lpDDSTankStatus, &dest, lpDDSTiles, &src, DDBLT_WAIT | DDBLT_KEYSRC, NULL);

}


/*********************************************************
*NAME:          drawCopyBasesStatus
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Copys the Bases status on to the primary buffer
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawCopyBasesStatus(int xValue, int yValue) {
  RECT dest; /* Destination location */
  BYTE zf;   /* Zoom Factor */

  zf = windowGetZoomFactor();

  dest.left = xValue + (zf * STATUS_BASES_LEFT);
  dest.top = yValue + (zf * STATUS_BASES_TOP);
  dest.right = dest.left + (zf * STATUS_BASES_WIDTH);
  dest.bottom = dest.top + (zf * STATUS_BASES_HEIGHT);
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest, lpDDSBasesStatus, NULL, DDBLT_WAIT, NULL);
}

/*********************************************************
*NAME:          drawCopyPillsStatus
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Copys the pills status on to the primary buffer
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawCopyPillsStatus(int xValue, int yValue) {
  RECT dest; /* Destination location */
  BYTE zf;   /* Zoom Factor */

  zf = windowGetZoomFactor();

  dest.left = xValue + (zf * STATUS_PILLS_LEFT);
  dest.top = yValue + (zf * STATUS_PILLS_TOP);
  dest.right = dest.left + (zf * STATUS_PILLS_WIDTH);
  dest.bottom = dest.top + (zf * STATUS_PILLS_HEIGHT);
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest, lpDDSPillsStatus, NULL, DDBLT_WAIT, NULL);
}

/*********************************************************
*NAME:          drawCopyTanksStatus
*AUTHOR:        John Morrison
*CREATION DATE: 14/2/98
*LAST MODIFIED: 14/2/98
*PURPOSE:
*  Copys the tanks status on to the primary buffer
*
*ARGUMENTS:
*  xValue  - The left position of the window
*  yValue  - The top position of the window
*********************************************************/
void drawCopyTanksStatus(int xValue, int yValue) {
  RECT dest; /* Destination location */
  BYTE zf;   /* Zoom Factor */

  zf = windowGetZoomFactor();

  dest.left = xValue + (zf * STATUS_TANKS_LEFT);
  dest.top = yValue + (zf * STATUS_TANKS_TOP);
  dest.right = dest.left + (zf * STATUS_TANKS_WIDTH);
  dest.bottom = dest.top + (zf * STATUS_TANKS_HEIGHT);
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, &dest, lpDDSTankStatus, NULL, DDBLT_WAIT, NULL);
}



/*********************************************************
*NAME:          drawStartDelay
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/98
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  The start delay is still counting down. Draw it here.
*
*ARGUMENTS:
*  rcWindow - Window rectangle
*  srtDelay - The start delay
*********************************************************/
void drawStartDelay(RECT *rcWindow, long srtDelay) {
  DDBLTFX fx;                /* FX for the colour fills */
  HDC hDC = NULL;            /* DC of the surface */
  RECT src;                  /* Used for copying the bases & pills icon in */
  BYTE zoomFactor;           /* Scaling Factor */
  char str[FILENAME_MAX];    /* Output String */
  char strNum[FILENAME_MAX]; /* Holds the start delay as a string */


  str[0] = '\0';
  zoomFactor = windowGetZoomFactor();
  src.left = TILE_SIZE_X;
  src.top = TILE_SIZE_Y;
  src.right = (MAIN_SCREEN_SIZE_X * TILE_SIZE_X) + TILE_SIZE_X;
  src.bottom = (MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y) + TILE_SIZE_Y;

  
  /* Fill the area black */
  ZeroMemory(&fx, sizeof(fx));
  fx.dwSize = sizeof(fx);
  fx.dwFillColor =  0;
  lpDDSBackBuffer->lpVtbl->Blt(lpDDSBackBuffer, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

  /* Prepare the string */
  srtDelay /= GAME_NUMGAMETICKS_SEC; /* Convert ticks back to seconds */
  sprintf(strNum, "%d", srtDelay);
  strcpy(str, langGetText(STR_DRAW_GAMESTARTSIN));
  strcat(str, strNum);

  /* Dispay the string */
  if (SUCCEEDED(lpDDSBackBuffer->lpVtbl->GetDC(lpDDSBackBuffer, &hDC))) {
    fontSelect(hDC);
    SetBkColor(hDC, RGB(0,0,0));
    SetTextColor(hDC, RGB(255,255,255));
    DrawTextA(hDC, str, (int) strlen(str), &src, (DT_CENTER | DT_VCENTER | DT_NOCLIP));
    lpDDSBackBuffer->lpVtbl->ReleaseDC(lpDDSBackBuffer, hDC);
  }

  /* Copy it to the primary buffer */
  rcWindow->left = rcWindow->left + (zoomFactor * MAIN_OFFSET_X);
  rcWindow->top = rcWindow->top + (zoomFactor * MAIN_OFFSET_Y);
  rcWindow->right = rcWindow->left + (MAIN_SCREEN_SIZE_X * (zoomFactor * TILE_SIZE_X));
  rcWindow->bottom = rcWindow->top + (zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y);
  
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, rcWindow, lpDDSBackBuffer, &src, DDBLT_WAIT, NULL);

}

void drawTankLabelsCalcSize(char *str, BYTE playerNum, BYTE zf) {
  HDC hDC = NULL; /* Temp DC of the surface */
  int maxWidth;   /* Max width of the item */
  RECT textRect;  /* Defines the text rectangle */

  maxWidth = zf * MAIN_BACK_BUFFER_SIZE_X * TILE_SIZE_X;
  drawPlayerLens[playerNum][0] = (int) strlen(str);
  drawPlayerLens[playerNum][2] = drawPlayerLens[playerNum][0];

  if (SUCCEEDED(lpDDSTankLabels->lpVtbl->GetDC(lpDDSTankLabels, &hDC))) {
    DrawTextA(hDC, str, drawPlayerLens[playerNum][0], &textRect, (DT_LEFT | DT_NOCLIP | DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE));
    while (textRect.right > maxWidth && drawPlayerLens[playerNum][0] > 0) {
      textRect.right = 0;
      str[drawPlayerLens[playerNum][0]-1] = '\0';
      drawPlayerLens[playerNum][0] -= 1;
      DrawTextA(hDC, str, drawPlayerLens[playerNum][0], &textRect, (DT_LEFT | DT_NOCLIP | DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE));
    }
    if (FAILED(lpDDSTankLabels->lpVtbl->ReleaseDC(lpDDSTankLabels, hDC))) {
      MessageBoxA(NULL, langGetText(STR_DRAWERROR_RELEASEDC), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    }
    drawPlayerLens[playerNum][1] = zf;
  }
  
}

/*********************************************************
*NAME:          drawTankLabel
*AUTHOR:        John Morrison
*CREATION DATE: 02/02/98
*LAST MODIFIED: 26/01/02
*PURPOSE:
*  Draws the tank label if required.
*
*ARGUMENTS:
*  str       - The string identifer of the tank
*  playerNum - Player number of the tank
*  mx        - Tank map X position
*  my        - Tank map Y position
*  px        - Tank pixel offset
*  py        - Tank pixel offset
*********************************************************/
void drawTankLabel(char *str, BYTE playerNum, int mx, int my, BYTE px, BYTE py) {
  HDC hDC = NULL; /* Temp DC of the surface */
  int len;        /* Length of the string */
  RECT textRect;  /* Defines the text rectangle */
  int x;          /* X And Y Locations on the back buffer to do the drawing */
  int y;
  BYTE zf;

  zf = windowGetZoomFactor();
  textRect.left = 0;
  textRect.right = zf * MAIN_BACK_BUFFER_SIZE_X * TILE_SIZE_X;
  textRect.top = 0;
  textRect.bottom = zf * TANK_LABEL_HEIGHT;
  len = (int) strlen(str);
  
  if (len > 0) {
    if (len != drawPlayerLens[playerNum][2] || zf != drawPlayerLens[playerNum][1]) {
      drawTankLabelsCalcSize(str, playerNum, zf);
    }

    if (SUCCEEDED(lpDDSTankLabels->lpVtbl->GetDC(lpDDSTankLabels, &hDC))) {
      /* Draw it on the back buffer */

      fontSelect(hDC);
      SetBkColor(hDC, RGB(0,255,0));
      SetTextColor(hDC, RGB(255, 255, 255));

      DrawTextA(hDC, str, drawPlayerLens[playerNum][0], &textRect, (DT_LEFT | DT_NOCLIP | DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE));    
      DrawTextA(hDC, str, drawPlayerLens[playerNum][0], &textRect, (DT_LEFT | DT_NOCLIP | DT_NOPREFIX | DT_SINGLELINE));
      if (FAILED(lpDDSTankLabels->lpVtbl->ReleaseDC(lpDDSTankLabels, hDC))) {
        MessageBoxA(NULL, langGetText(STR_DRAWERROR_RELEASEDC), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      }
      x = (mx+1) * zf * TILE_SIZE_X + zf * (px+1);
      y = my * zf * TILE_SIZE_Y + zf * py;
      textRect.left = 0;
      /* Fix displaying off the edge of the screen */ 
      if ((x + textRect.right) > zf * MAIN_BACK_BUFFER_SIZE_X * TILE_SIZE_X) {
        textRect.right = zf * MAIN_BACK_BUFFER_SIZE_X * TILE_SIZE_X - x;
      }
      if ((y + textRect.bottom) > (MAIN_BACK_BUFFER_SIZE_Y * (zf * TILE_SIZE_Y))) {
         textRect.bottom = zf * MAIN_BACK_BUFFER_SIZE_Y * TILE_SIZE_Y - y;
      }
      /* Output it */
      lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTankLabels, &textRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
    }
  }
}

/*********************************************************
*NAME:          drawPillInView
*AUTHOR:        John Morrison
*CREATION DATE:  3/2/98
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Draws the "Pillbox View" label
*
*ARGUMENTS:
*
*********************************************************/
void drawPillInView() {
  HDC hDC = NULL; /* Temp DC of the surface */
  RECT textRect;  /* Defines the text rectangle */
  int x;          /* X And Y Locations on the back buffer to do the drawing */
  int y;
  char *text;     /* Used in language getting */
  int textLen;    /* Length of the text */
  BYTE zoomFactor;

  zoomFactor = windowGetZoomFactor();
  textRect.left = 0;
  textRect.right = TANK_LABEL_WIDTH;
  textRect.top = 0;
  textRect.bottom = TANK_LABEL_HEIGHT;

  if (SUCCEEDED(lpDDSTankLabels->lpVtbl->GetDC(lpDDSTankLabels, &hDC))) {
    /* Draw it on the back buffer */
    fontSelect(hDC);
    SetBkColor(hDC, RGB(0,255,0));
    SetTextColor(hDC, RGB(255, 255, 255));
    text = langGetText(STR_DRAW_PILLBOXVIEW);
    textLen = (int) strlen(text);
    DrawTextA(hDC, text, textLen, &textRect, (DT_LEFT | DT_NOCLIP | DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE));
    DrawTextA(hDC, text, textLen, &textRect, (DT_LEFT | DT_NOCLIP | DT_NOPREFIX | DT_SINGLELINE));
    if (FAILED(lpDDSTankLabels->lpVtbl->ReleaseDC(lpDDSTankLabels, hDC))) {
      MessageBoxA(NULL, "This should never happen - Error Releasing DC for drawing Pillbox view", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    }
    x = zoomFactor * TILE_SIZE_X;
    y = zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y;
    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTankLabels, &textRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
  }
}

/*********************************************************
*NAME:          drawRestore
*AUTHOR:        John Morrison
*CREATION DATE: 23/3/99
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Restores all the DD objects and reloads graphics into
*  them
*
*ARGUMENTS:
*
*********************************************************/
void drawRestore(void) {
  BYTE zoomFactor;        /* scaling factor */
  HRESULT res;            /* Direct Draw Function returns */
  HBITMAP hTiles = NULL;  /* The tile file bitmap resource */
  HDC hTilesDC = NULL;    /* The tile file resource DC */
  HDC hDDSTilesDC = NULL; /* Temp DC of a DDS use to copy tile file into */
  DDBLTFX fx;             /* FX for the colour fills */
  RECT src;               /* Used for copying the bases & pills icon in */
  RECT dest;              /* Used for copying the bases & pills icon in */
  char fileName[MAX_PATH]; /* Used for loading skins */
  bool usingDoubleSkin;    /* Are we using the double sized skin */

  usingDoubleSkin = FALSE;
  zoomFactor = windowGetZoomFactor();
  res = lpDDSPrimary->lpVtbl->Restore(lpDDSPrimary);
  /* Create the primary surface */
  if (SUCCEEDED(res)) {
    lpDDSTiles->lpVtbl->Restore(lpDDSTiles);
    /* Back Buffer */
    /* Copy the bitmap into it */
    hTiles = NULL;
    /* Try to load from skins file First first */
    if (skinsIsLoaded() == TRUE) {
      /* Try 32x32 tile first */
      if (zoomFactor == 2) {
        skinsGetSkinDirectory(fileName);
        if (fileName[strlen(fileName)-1] != '\\') {
          strcat(fileName, "\\");
        }
        strcat(fileName, DRAW_SKINS_TILESFILE32);
        hTiles = (HBITMAP) LoadImage(NULL, fileName, IMAGE_BITMAP, 2 * TILE_FILE_X , 2 * TILE_FILE_Y, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        if (hTiles != NULL) {
          usingDoubleSkin = TRUE;
        }
      }
      if (hTiles == NULL) {
        skinsGetSkinDirectory(fileName);
        if (fileName[strlen(fileName)-1] != '\\') {
          strcat(fileName, "\\");
        }
        strcat(fileName, DRAW_SKINS_TILESFILE);
        hTiles = (HBITMAP) LoadImage(NULL, fileName, IMAGE_BITMAP, TILE_FILE_X , TILE_FILE_Y, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
      }
    } 
    if (hTiles == NULL) {
      hTiles = (HBITMAP) LoadImage(NULL, MAKEINTRESOURCE(IDB_TILE), IMAGE_BITMAP, TILE_FILE_X , TILE_FILE_Y, LR_CREATEDIBSECTION);
    }
    if (hTiles !=NULL) {     
      hTilesDC = CreateCompatibleDC(NULL);
      SelectObject(hTilesDC, hTiles);
      lpDDSTiles->lpVtbl->Restore(lpDDSTiles);
      res = lpDDSTiles->lpVtbl->GetDC(lpDDSTiles,&hDDSTilesDC);
      if (res != DD_OK) {
        MessageBoxA(NULL, langGetText(STR_DRAWERROR_GETDCFAILED), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      }
      if (usingDoubleSkin == TRUE) {
        BitBlt(hDDSTilesDC, 0, 0, 2 * TILE_FILE_X, 2 * TILE_FILE_Y, hTilesDC, 0, 0, SRCCOPY);
      } else {
        StretchBlt(hDDSTilesDC, 0, 0, zoomFactor * TILE_FILE_X, zoomFactor * TILE_FILE_Y, hTilesDC, 0, 0, TILE_FILE_X, TILE_FILE_Y, SRCCOPY);
      }

      res = lpDDSTiles->lpVtbl->ReleaseDC(lpDDSTiles, hDDSTilesDC);
      DeleteDC(hTilesDC);
      DeleteObject(hTiles);
    }

    /* Messages Buffer - Fill the surface black */
    ZeroMemory(&fx, sizeof(fx));
    fx.dwSize = sizeof(fx);
    fx.dwFillColor =  0;
    lpDDSMessages->lpVtbl->Restore(lpDDSMessages);
    lpDDSMessages->lpVtbl->Blt(lpDDSMessages, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

    /* Create the Man status window */
    lpDDSManStatus->lpVtbl->Restore(lpDDSManStatus);
    lpDDSManStatus->lpVtbl->Blt(lpDDSManStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);


    /* Create the Base status window */
    /* Fill the surface black */
    lpDDSBasesStatus->lpVtbl->Restore(lpDDSBasesStatus);
    lpDDSBasesStatus->lpVtbl->Blt(lpDDSBasesStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
    /* Copy the icon in */
    src.left = zoomFactor * BASE_GOOD_X;
    src.top = zoomFactor * BASE_GOOD_Y;
    src.right = zoomFactor * BASE_GOOD_X + zoomFactor * TILE_SIZE_X;
    src.bottom = zoomFactor * BASE_GOOD_Y + zoomFactor * TILE_SIZE_Y;
    dest.top = (zoomFactor * STATUS_BASES_MIDDLE_ICON_Y);
    dest.left = (zoomFactor * STATUS_BASES_MIDDLE_ICON_X);
    dest.right = dest.left + (zoomFactor * TILE_SIZE_X);
    dest.bottom = dest.top + (zoomFactor * TILE_SIZE_Y);
    lpDDSBasesStatus->lpVtbl->Blt(lpDDSBasesStatus, &dest, lpDDSTiles, &src, DDBLT_WAIT, NULL);

    /* Pill Status */
    /* Fill the surface black */
    lpDDSPillsStatus->lpVtbl->Restore(lpDDSPillsStatus);
    lpDDSPillsStatus->lpVtbl->Blt(lpDDSPillsStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
    /* Copy the icon in */
    src.left = zoomFactor * PILL_GOOD15_X;
    src.top = zoomFactor * PILL_GOOD15_Y;
    src.right = (zoomFactor * PILL_GOOD15_X) + (zoomFactor * TILE_SIZE_X);
    src.bottom = (zoomFactor * PILL_GOOD15_Y) + (zoomFactor * TILE_SIZE_Y);
    dest.top = (zoomFactor * STATUS_PILLS_MIDDLE_ICON_Y);
    dest.left = (zoomFactor * STATUS_PILLS_MIDDLE_ICON_X);
    dest.right = dest.left + (zoomFactor * TILE_SIZE_X);
    dest.bottom = dest.top + (zoomFactor * TILE_SIZE_Y);
    lpDDSPillsStatus->lpVtbl->Blt(lpDDSPillsStatus, &dest, lpDDSTiles, &src, DDBLT_WAIT, NULL);


    /* Tank Labels */
    lpDDSTankLabels->lpVtbl->Restore(lpDDSTankLabels);

    /* Tank Status */
    /* Fill the surface black */
    lpDDSTankStatus->lpVtbl->Restore(lpDDSTankStatus);
    lpDDSTankStatus->lpVtbl->Blt(lpDDSTankStatus, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
    /* Copy the icon in */
    src.left = zoomFactor * TANK_SELF_0_X;
    src.top = zoomFactor * TANK_SELF_0_Y;
    src.right = zoomFactor * TANK_SELF_0_X + zoomFactor * TILE_SIZE_X;
    src.bottom = zoomFactor * TANK_SELF_0_Y + zoomFactor * TILE_SIZE_Y;
    dest.top = (zoomFactor * STATUS_TANKS_MIDDLE_ICON_Y);
    dest.left = (zoomFactor * STATUS_TANKS_MIDDLE_ICON_X);
    dest.right = dest.left + (zoomFactor * TILE_SIZE_X);
    dest.bottom = dest.top + (zoomFactor * TILE_SIZE_Y);
    lpDDSTankStatus->lpVtbl->Blt(lpDDSTankStatus, &dest, lpDDSTiles, &src, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
     
    /* Need a redraw */
    windowRedrawAll();
  }
}

/*********************************************************
*NAME:          drawDownloadScreen
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/98
*LAST MODIFIED: 13/12/98
*PURPOSE:
*  Draws the download line on the screen
*
*ARGUMENTS:
*  rcWindow  - Window Co-ordinates
*  justBlack - TRUE if we want a black screen
*********************************************************/
void drawDownloadScreen(RECT *rcWindow, bool justBlack) {
  BYTE zoomFactor;        /* scaling factor */
  DDBLTFX fx;             /* FX for the colour fills */
  RECT output;            /* Output RECT */

  zoomFactor = windowGetZoomFactor();

  /* Fill the area black */
  ZeroMemory(&fx, sizeof(fx));
  fx.dwSize = sizeof(fx);
  fx.dwFillColor =  0;
  lpDDSBackBuffer->lpVtbl->Blt(lpDDSBackBuffer, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

  /* Fill the downloaded area white */
  output.top = 0;
  output.left = 0;
  if (justBlack == FALSE) {
    fx.dwFillColor = RGB(255,255,255);
    output.bottom = netGetDownloadPos();
    output.right = (MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y) + TILE_SIZE_Y;
    lpDDSBackBuffer->lpVtbl->Blt(lpDDSBackBuffer, &output, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
  }

  /* Copy the back buffer to the window */
  output.left = TILE_SIZE_X;
  output.top = TILE_SIZE_Y;
  output.right = (MAIN_SCREEN_SIZE_X * TILE_SIZE_X) + TILE_SIZE_X;
  output.bottom = (MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y) + TILE_SIZE_Y;
  rcWindow->left = rcWindow->left + (zoomFactor * MAIN_OFFSET_X);
  rcWindow->top = rcWindow->top + (zoomFactor * MAIN_OFFSET_Y);
  rcWindow->right = rcWindow->left + (MAIN_SCREEN_SIZE_X * (zoomFactor * TILE_SIZE_X));
  rcWindow->bottom = rcWindow->top + (zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y); 
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, rcWindow, lpDDSBackBuffer, &output, DDBLT_WAIT, NULL);
}

/*********************************************************
*NAME:          drawMainScreenBlack
*AUTHOR:        John Morrison
*CREATION DATE: 29/3/98
*LAST MODIFIED: 29/3/98
*PURPOSE:
*  Draws the main screen black
*
*ARGUMENTS:
*  rcWindow - Window Co-ordinates
*********************************************************/
void drawMainScreenBlack(RECT *rcWindow) {
  BYTE zoomFactor;        /* scaling factor */
  DDBLTFX fx;             /* FX for the colour fills */
  RECT output;            /* Output RECT */

  zoomFactor = windowGetZoomFactor();

  /* Fill the area black */
  ZeroMemory(&fx, sizeof(fx));
  fx.dwSize = sizeof(fx);
  fx.dwFillColor =  0;
  lpDDSBackBuffer->lpVtbl->Blt(lpDDSBackBuffer, NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

  /* Copy the back buffer to the window */
  output.left = TILE_SIZE_X;
  output.top = TILE_SIZE_Y;
  output.right = (MAIN_SCREEN_SIZE_X * TILE_SIZE_X) + TILE_SIZE_X;
  output.bottom = (MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y) + TILE_SIZE_Y;
  rcWindow->left = rcWindow->left + (zoomFactor * MAIN_OFFSET_X);
  rcWindow->top = rcWindow->top + (zoomFactor * MAIN_OFFSET_Y);
  rcWindow->right = rcWindow->left + (MAIN_SCREEN_SIZE_X * (zoomFactor * TILE_SIZE_X));
  rcWindow->bottom = rcWindow->top + (zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y); 
  lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, rcWindow, lpDDSBackBuffer, &output, DDBLT_WAIT, NULL);
}



/*********************************************************
*NAME:          drawSetupArrays
*AUTHOR:        John Morrison
*CREATION DATE: 28/5/00
*LAST MODIFIED: 28/5/00
*PURPOSE:
*  Sets up the drawing arrays to improve efficeny and
*  remove the giant switch statement every drawing loop
*
*ARGUMENTS:
*  zoomFactor - The scaling factor
*********************************************************/
void drawSetupArrays(BYTE zoomFactor) {
  drawPosX[DEEP_SEA_SOLID] = zoomFactor * DEEP_SEA_SOLID_X;
  drawPosY[DEEP_SEA_SOLID] = zoomFactor * DEEP_SEA_SOLID_Y;
  drawPosX[DEEP_SEA_CORN1] = zoomFactor * DEEP_SEA_CORN1_X;
  drawPosY[DEEP_SEA_CORN1] = zoomFactor * DEEP_SEA_CORN1_Y;
  drawPosX[DEEP_SEA_CORN2] = zoomFactor * DEEP_SEA_CORN2_X;
  drawPosY[DEEP_SEA_CORN2] = zoomFactor * DEEP_SEA_CORN2_Y;
  drawPosX[DEEP_SEA_CORN3] = zoomFactor * DEEP_SEA_CORN3_X;
  drawPosY[DEEP_SEA_CORN3] = zoomFactor * DEEP_SEA_CORN3_Y;
  drawPosX[DEEP_SEA_CORN4] = zoomFactor * DEEP_SEA_CORN4_X;
  drawPosY[DEEP_SEA_CORN4] = zoomFactor * DEEP_SEA_CORN4_Y;
  drawPosX[DEEP_SEA_SIDE1] = zoomFactor * DEEP_SEA_SIDE1_X;
  drawPosY[DEEP_SEA_SIDE1] = zoomFactor * DEEP_SEA_SIDE1_Y;
  drawPosX[DEEP_SEA_SIDE2] = zoomFactor * DEEP_SEA_SIDE2_X;
  drawPosY[DEEP_SEA_SIDE2] = zoomFactor * DEEP_SEA_SIDE2_Y;
  drawPosX[DEEP_SEA_SIDE3] = zoomFactor * DEEP_SEA_SIDE3_X;
  drawPosY[DEEP_SEA_SIDE3] = zoomFactor * DEEP_SEA_SIDE3_Y;
  drawPosX[DEEP_SEA_SIDE4] = zoomFactor * DEEP_SEA_SIDE4_X;
  drawPosY[DEEP_SEA_SIDE4] = zoomFactor * DEEP_SEA_SIDE4_Y;

  drawPosX[BUILD_SINGLE] = zoomFactor * BUILD_SINGLE_X;
  drawPosY[BUILD_SINGLE] = zoomFactor * BUILD_SINGLE_Y;
  drawPosX[BUILD_SOLID] = zoomFactor * BUILD_SOLID_X;
  drawPosY[BUILD_SOLID] = zoomFactor * BUILD_SOLID_Y;
  drawPosX[BUILD_CORNER1] = zoomFactor * BUILD_CORNER1_X;
  drawPosY[BUILD_CORNER1] = zoomFactor * BUILD_CORNER1_Y;
  drawPosX[BUILD_CORNER2] = zoomFactor * BUILD_CORNER2_X;
  drawPosY[BUILD_CORNER2] = zoomFactor * BUILD_CORNER2_Y;
  drawPosX[BUILD_CORNER3] = zoomFactor * BUILD_CORNER3_X;
  drawPosY[BUILD_CORNER3] = zoomFactor * BUILD_CORNER3_Y;
  drawPosX[BUILD_CORNER4] = zoomFactor * BUILD_CORNER4_X;
  drawPosY[BUILD_CORNER4] = zoomFactor * BUILD_CORNER4_Y;
  drawPosX[BUILD_L1] = zoomFactor * BUILD_L1_X;
  drawPosY[BUILD_L1] = zoomFactor * BUILD_L1_Y;
  drawPosX[BUILD_L2] = zoomFactor * BUILD_L2_X;
  drawPosY[BUILD_L2] = zoomFactor * BUILD_L2_Y;
  drawPosX[BUILD_L3] = zoomFactor * BUILD_L3_X;
  drawPosY[BUILD_L3] = zoomFactor * BUILD_L3_Y;
  drawPosX[BUILD_L4] = zoomFactor * BUILD_L4_X;
  drawPosY[BUILD_L4] = zoomFactor * BUILD_L4_Y;
  drawPosX[BUILD_T1] = zoomFactor * BUILD_T1_X;
  drawPosY[BUILD_T1] = zoomFactor * BUILD_T1_Y;
  drawPosX[BUILD_T2] = zoomFactor * BUILD_T2_X;
  drawPosY[BUILD_T2] = zoomFactor * BUILD_T2_Y;
  drawPosX[BUILD_T3] = zoomFactor * BUILD_T3_X;
  drawPosY[BUILD_T3] = zoomFactor * BUILD_T3_Y;
  drawPosX[BUILD_T4] = zoomFactor * BUILD_T4_X;
  drawPosY[BUILD_T4] = zoomFactor * BUILD_T4_Y;
  drawPosX[BUILD_HORZ] = zoomFactor * BUILD_HORZ_X;
  drawPosY[BUILD_HORZ] = zoomFactor * BUILD_HORZ_Y;
  drawPosX[BUILD_VERT] = zoomFactor * BUILD_VERT_X;
  drawPosY[BUILD_VERT] = zoomFactor * BUILD_VERT_Y;
  drawPosX[BUILD_VERTEND1] = zoomFactor * BUILD_VERTEND1_X;
  drawPosY[BUILD_VERTEND1] = zoomFactor * BUILD_VERTEND1_Y;
  drawPosX[BUILD_VERTEND2] = zoomFactor * BUILD_VERTEND2_X;
  drawPosY[BUILD_VERTEND2] = zoomFactor * BUILD_VERTEND2_Y;
  drawPosX[BUILD_HORZEND1] = zoomFactor * BUILD_HORZEND1_X;
  drawPosY[BUILD_HORZEND1] = zoomFactor * BUILD_HORZEND1_Y;
  drawPosX[BUILD_HORZEND2] = zoomFactor * BUILD_HORZEND2_X;
  drawPosY[BUILD_HORZEND2] = zoomFactor * BUILD_HORZEND2_Y;
  drawPosX[BUILD_CROSS] = zoomFactor * BUILD_CROSS_X;
  drawPosY[BUILD_CROSS] = zoomFactor * BUILD_CROSS_Y;
  drawPosX[BUILD_SIDE1] = zoomFactor * BUILD_SIDE1_X;
  drawPosY[BUILD_SIDE1] = zoomFactor * BUILD_SIDE1_Y;
  drawPosX[BUILD_SIDE2] = zoomFactor * BUILD_SIDE2_X;
  drawPosY[BUILD_SIDE2] = zoomFactor * BUILD_SIDE2_Y;
  drawPosX[BUILD_SIDE3] = zoomFactor * BUILD_SIDE3_X;
  drawPosY[BUILD_SIDE3] = zoomFactor * BUILD_SIDE3_Y;
  drawPosX[BUILD_SIDE4] = zoomFactor * BUILD_SIDE4_X;
  drawPosY[BUILD_SIDE4] = zoomFactor * BUILD_SIDE4_Y;
  drawPosX[BUILD_SIDECORN1] = zoomFactor * BUILD_SIDECORN1_X;
  drawPosY[BUILD_SIDECORN1] = zoomFactor * BUILD_SIDECORN1_Y;
  drawPosX[BUILD_SIDECORN2] = zoomFactor * BUILD_SIDECORN2_X;
  drawPosY[BUILD_SIDECORN2] = zoomFactor * BUILD_SIDECORN2_Y;
  drawPosX[BUILD_SIDECORN3] = zoomFactor * BUILD_SIDECORN3_X;
  drawPosY[BUILD_SIDECORN3] = zoomFactor * BUILD_SIDECORN3_Y;
  drawPosX[BUILD_SIDECORN4] = zoomFactor * BUILD_SIDECORN4_X;
  drawPosY[BUILD_SIDECORN4] = zoomFactor * BUILD_SIDECORN4_Y;
  drawPosX[BUILD_SIDECORN5] = zoomFactor * BUILD_SIDECORN5_X;
  drawPosY[BUILD_SIDECORN5] = zoomFactor * BUILD_SIDECORN5_Y;
  drawPosX[BUILD_SIDECORN6] = zoomFactor * BUILD_SIDECORN6_X;
  drawPosY[BUILD_SIDECORN6] = zoomFactor * BUILD_SIDECORN6_Y;
  drawPosX[BUILD_SIDECORN7] = zoomFactor * BUILD_SIDECORN7_X;
  drawPosY[BUILD_SIDECORN7] = zoomFactor * BUILD_SIDECORN7_Y;
  drawPosX[BUILD_SIDECORN8] = zoomFactor * BUILD_SIDECORN8_X;
  drawPosY[BUILD_SIDECORN8] = zoomFactor * BUILD_SIDECORN8_Y;
  drawPosX[BUILD_SIDECORN9] = zoomFactor * BUILD_SIDECORN9_X;
  drawPosY[BUILD_SIDECORN9] = zoomFactor * BUILD_SIDECORN9_Y;
  drawPosX[BUILD_SIDECORN10] = zoomFactor * BUILD_SIDECORN10_X;
  drawPosY[BUILD_SIDECORN10] = zoomFactor * BUILD_SIDECORN10_Y;
  drawPosX[BUILD_SIDECORN11] = zoomFactor * BUILD_SIDECORN11_X;
  drawPosY[BUILD_SIDECORN11] = zoomFactor * BUILD_SIDECORN11_Y;
  drawPosX[BUILD_SIDECORN12] = zoomFactor * BUILD_SIDECORN12_X;
  drawPosY[BUILD_SIDECORN12] = zoomFactor * BUILD_SIDECORN12_Y;
  drawPosX[BUILD_SIDECORN13] = zoomFactor * BUILD_SIDECORN13_X;
  drawPosY[BUILD_SIDECORN13] = zoomFactor * BUILD_SIDECORN13_Y;
  drawPosX[BUILD_SIDECORN14] = zoomFactor * BUILD_SIDECORN14_X;
  drawPosY[BUILD_SIDECORN14] = zoomFactor * BUILD_SIDECORN14_Y;
  drawPosX[BUILD_SIDECORN15] = zoomFactor * BUILD_SIDECORN15_X;
  drawPosY[BUILD_SIDECORN15] = zoomFactor * BUILD_SIDECORN15_Y;
  drawPosX[BUILD_SIDECORN16] = zoomFactor * BUILD_SIDECORN16_X;
  drawPosY[BUILD_SIDECORN16] = zoomFactor * BUILD_SIDECORN16_Y;
  drawPosX[BUILD_TWIST1] = zoomFactor * BUILD_TWIST1_X;
  drawPosY[BUILD_TWIST1] = zoomFactor * BUILD_TWIST1_Y;
  drawPosX[BUILD_TWIST2] = zoomFactor * BUILD_TWIST2_X;
  drawPosY[BUILD_TWIST2] = zoomFactor * BUILD_TWIST2_Y;
  drawPosX[BUILD_MOST1] = zoomFactor * BUILD_MOST1_X;
  drawPosY[BUILD_MOST1] = zoomFactor * BUILD_MOST1_Y;
  drawPosX[BUILD_MOST2] = zoomFactor * BUILD_MOST2_X;
  drawPosY[BUILD_MOST2] = zoomFactor * BUILD_MOST2_Y;
  drawPosX[BUILD_MOST3] = zoomFactor * BUILD_MOST3_X;
  drawPosY[BUILD_MOST3] = zoomFactor * BUILD_MOST3_Y;
  drawPosX[BUILD_MOST4] = zoomFactor * BUILD_MOST4_X;
  drawPosY[BUILD_MOST4] = zoomFactor * BUILD_MOST4_Y;

  drawPosX[RIVER_END1] = zoomFactor * RIVER_END1_X;
  drawPosY[RIVER_END1] = zoomFactor * RIVER_END1_Y;
  drawPosX[RIVER_END2] = zoomFactor * RIVER_END2_X;
  drawPosY[RIVER_END2] = zoomFactor * RIVER_END2_Y;
  drawPosX[RIVER_END3] = zoomFactor * RIVER_END3_X;
  drawPosY[RIVER_END3] = zoomFactor * RIVER_END3_Y;
  drawPosX[RIVER_END4] = zoomFactor * RIVER_END4_X;
  drawPosY[RIVER_END4] = zoomFactor * RIVER_END4_Y;
  drawPosX[RIVER_SOLID] = zoomFactor * RIVER_SOLID_X;
  drawPosY[RIVER_SOLID] = zoomFactor * RIVER_SOLID_Y;
  drawPosX[RIVER_SURROUND] = zoomFactor * RIVER_SURROUND_X;
  drawPosY[RIVER_SURROUND] = zoomFactor * RIVER_SURROUND_Y;
  drawPosX[RIVER_SIDE1] = zoomFactor * RIVER_SIDE1_X;
  drawPosY[RIVER_SIDE1] = zoomFactor * RIVER_SIDE1_Y;
  drawPosX[RIVER_SIDE2] = zoomFactor * RIVER_SIDE2_X;
  drawPosY[RIVER_SIDE2] = zoomFactor * RIVER_SIDE2_Y;
  drawPosX[RIVER_ONESIDE1] = zoomFactor * RIVER_ONESIDE1_X;
  drawPosY[RIVER_ONESIDE1] = zoomFactor * RIVER_ONESIDE1_Y;
  drawPosX[RIVER_ONESIDE2] = zoomFactor * RIVER_ONESIDE2_X;
  drawPosY[RIVER_ONESIDE2] = zoomFactor * RIVER_ONESIDE2_Y;
  drawPosX[RIVER_ONESIDE3] = zoomFactor * RIVER_ONESIDE3_X;
  drawPosY[RIVER_ONESIDE3] = zoomFactor * RIVER_ONESIDE3_Y;
  drawPosX[RIVER_ONESIDE4] = zoomFactor * RIVER_ONESIDE4_X;
  drawPosY[RIVER_ONESIDE4] = zoomFactor * RIVER_ONESIDE4_Y;
  drawPosX[RIVER_CORN1] = zoomFactor * RIVER_CORN1_X;
  drawPosY[RIVER_CORN1] = zoomFactor * RIVER_CORN1_Y;
  drawPosX[RIVER_CORN2] = zoomFactor * RIVER_CORN2_X;
  drawPosY[RIVER_CORN2] = zoomFactor * RIVER_CORN2_Y;
  drawPosX[RIVER_CORN3] = zoomFactor * RIVER_CORN3_X;
  drawPosY[RIVER_CORN3] = zoomFactor * RIVER_CORN3_Y;
  drawPosX[RIVER_CORN4] = zoomFactor * RIVER_CORN4_X;
  drawPosY[RIVER_CORN4] = zoomFactor * RIVER_CORN4_Y;

  drawPosX[SWAMP] = zoomFactor * SWAMP_X;
  drawPosY[SWAMP] = zoomFactor * SWAMP_Y;


  drawPosX[ROAD_CORNER1] = zoomFactor * ROAD_CORNER1_X;
  drawPosY[ROAD_CORNER1] = zoomFactor * ROAD_CORNER1_Y;
  drawPosX[ROAD_CORNER2] = zoomFactor * ROAD_CORNER2_X;
  drawPosY[ROAD_CORNER2] = zoomFactor * ROAD_CORNER2_Y;
  drawPosX[ROAD_CORNER3] = zoomFactor * ROAD_CORNER3_X;
  drawPosY[ROAD_CORNER3] = zoomFactor * ROAD_CORNER3_Y;
  drawPosX[ROAD_CORNER4] = zoomFactor * ROAD_CORNER4_X;
  drawPosY[ROAD_CORNER4] = zoomFactor * ROAD_CORNER4_Y;
  drawPosX[ROAD_CORNER5] = zoomFactor * ROAD_CORNER5_X;
  drawPosY[ROAD_CORNER5] = zoomFactor * ROAD_CORNER5_Y;
  drawPosX[ROAD_CORNER6] = zoomFactor * ROAD_CORNER6_X;
  drawPosY[ROAD_CORNER6] = zoomFactor * ROAD_CORNER6_Y;
  drawPosX[ROAD_CORNER7] = zoomFactor * ROAD_CORNER7_X;
  drawPosY[ROAD_CORNER7] = zoomFactor * ROAD_CORNER7_Y;
  drawPosX[ROAD_CORNER8] = zoomFactor * ROAD_CORNER8_X;
  drawPosY[ROAD_CORNER8] = zoomFactor * ROAD_CORNER8_Y;

  drawPosX[ROAD_SIDE1] = zoomFactor * ROAD_SIDE1_X;
  drawPosY[ROAD_SIDE1] = zoomFactor * ROAD_SIDE1_Y;
  drawPosX[ROAD_SIDE2] = zoomFactor * ROAD_SIDE2_X;
  drawPosY[ROAD_SIDE2] = zoomFactor * ROAD_SIDE2_Y;
  drawPosX[ROAD_SIDE3] = zoomFactor * ROAD_SIDE3_X;
  drawPosY[ROAD_SIDE3] = zoomFactor * ROAD_SIDE3_Y;
  drawPosX[ROAD_SIDE4] = zoomFactor * ROAD_SIDE4_X;
  drawPosY[ROAD_SIDE4] = zoomFactor * ROAD_SIDE4_Y;
  drawPosX[ROAD_SOLID] = zoomFactor * ROAD_SOLID_X;
  drawPosY[ROAD_SOLID] = zoomFactor * ROAD_SOLID_Y;
  drawPosX[ROAD_CROSS] = zoomFactor * ROAD_CROSS_X;
  drawPosY[ROAD_CROSS] = zoomFactor * ROAD_CROSS_Y;
  drawPosX[ROAD_T1] = zoomFactor * ROAD_T1_X;
  drawPosY[ROAD_T1] = zoomFactor * ROAD_T1_Y;
  drawPosX[ROAD_T2] = zoomFactor * ROAD_T2_X;
  drawPosY[ROAD_T2] = zoomFactor * ROAD_T2_Y;
  drawPosX[ROAD_T3] = zoomFactor * ROAD_T3_X;
  drawPosY[ROAD_T3] = zoomFactor * ROAD_T3_Y;
  drawPosX[ROAD_T4] = zoomFactor * ROAD_T4_X;
  drawPosY[ROAD_T4] = zoomFactor * ROAD_T4_Y;
  drawPosX[ROAD_HORZ] = zoomFactor * ROAD_HORZ_X;
  drawPosY[ROAD_HORZ] = zoomFactor * ROAD_HORZ_Y;
  drawPosX[ROAD_VERT] = zoomFactor * ROAD_VERT_X;
  drawPosY[ROAD_VERT] = zoomFactor * ROAD_VERT_Y;
  drawPosX[ROAD_WATER1] = zoomFactor * ROAD_WATER1_X;
  drawPosY[ROAD_WATER1] = zoomFactor * ROAD_WATER1_Y;
  drawPosX[ROAD_WATER2] = zoomFactor * ROAD_WATER2_X;
  drawPosY[ROAD_WATER2] = zoomFactor * ROAD_WATER2_Y;
  drawPosX[ROAD_WATER3] = zoomFactor * ROAD_WATER3_X;
  drawPosY[ROAD_WATER3] = zoomFactor * ROAD_WATER3_Y;
  drawPosX[ROAD_WATER4] = zoomFactor * ROAD_WATER4_X;
  drawPosY[ROAD_WATER4] = zoomFactor * ROAD_WATER4_Y;
  drawPosX[ROAD_WATER5] = zoomFactor * ROAD_WATER5_X;
  drawPosY[ROAD_WATER5] = zoomFactor * ROAD_WATER5_Y;
  drawPosX[ROAD_WATER6] = zoomFactor * ROAD_WATER6_X;
  drawPosY[ROAD_WATER6] = zoomFactor * ROAD_WATER6_Y;
  drawPosX[ROAD_WATER7] = zoomFactor * ROAD_WATER7_X;
  drawPosY[ROAD_WATER7] = zoomFactor * ROAD_WATER7_Y;
  drawPosX[ROAD_WATER8] = zoomFactor * ROAD_WATER8_X;
  drawPosY[ROAD_WATER8] = zoomFactor * ROAD_WATER8_Y;
  drawPosX[ROAD_WATER9] = zoomFactor * ROAD_WATER9_X;
  drawPosY[ROAD_WATER9] = zoomFactor * ROAD_WATER9_Y;
  drawPosX[ROAD_WATER10] = zoomFactor * ROAD_WATER10_X;
  drawPosY[ROAD_WATER10] = zoomFactor * ROAD_WATER10_Y;
  drawPosX[ROAD_WATER11] = zoomFactor * ROAD_WATER11_X;
  drawPosY[ROAD_WATER11] = zoomFactor * ROAD_WATER11_Y;

  drawPosX[PILL_EVIL_15] = zoomFactor * PILL_EVIL15_X;
  drawPosY[PILL_EVIL_15] = zoomFactor * PILL_EVIL15_Y;
  drawPosX[PILL_EVIL_14] = zoomFactor * PILL_EVIL14_X;
  drawPosY[PILL_EVIL_14] = zoomFactor * PILL_EVIL14_Y;
  drawPosX[PILL_EVIL_13] = zoomFactor * PILL_EVIL13_X;
  drawPosY[PILL_EVIL_13] = zoomFactor * PILL_EVIL13_Y;
  drawPosX[PILL_EVIL_12] = zoomFactor * PILL_EVIL12_X;
  drawPosY[PILL_EVIL_12] = zoomFactor * PILL_EVIL12_Y;
  drawPosX[PILL_EVIL_11] = zoomFactor * PILL_EVIL11_X;
  drawPosY[PILL_EVIL_11] = zoomFactor * PILL_EVIL11_Y;
  drawPosX[PILL_EVIL_10] = zoomFactor * PILL_EVIL10_X;
  drawPosY[PILL_EVIL_10] = zoomFactor * PILL_EVIL10_Y;
  drawPosX[PILL_EVIL_9] = zoomFactor * PILL_EVIL9_X;
  drawPosY[PILL_EVIL_9] = zoomFactor * PILL_EVIL9_Y;
  drawPosX[PILL_EVIL_8] = zoomFactor * PILL_EVIL8_X;
  drawPosY[PILL_EVIL_8] = zoomFactor * PILL_EVIL8_Y;
  drawPosX[PILL_EVIL_7] = zoomFactor * PILL_EVIL7_X;
  drawPosY[PILL_EVIL_7] = zoomFactor * PILL_EVIL7_Y;
  drawPosX[PILL_EVIL_6] = zoomFactor * PILL_EVIL6_X;
  drawPosY[PILL_EVIL_6] = zoomFactor * PILL_EVIL6_Y;
  drawPosX[PILL_EVIL_5] = zoomFactor * PILL_EVIL5_X;
  drawPosY[PILL_EVIL_5] = zoomFactor * PILL_EVIL5_Y;
  drawPosX[PILL_EVIL_4] = zoomFactor * PILL_EVIL4_X;
  drawPosY[PILL_EVIL_4] = zoomFactor * PILL_EVIL4_Y;
  drawPosX[PILL_EVIL_3] = zoomFactor * PILL_EVIL3_X;
  drawPosY[PILL_EVIL_3] = zoomFactor * PILL_EVIL3_Y;
  drawPosX[PILL_EVIL_2] = zoomFactor * PILL_EVIL2_X;
  drawPosY[PILL_EVIL_2] = zoomFactor * PILL_EVIL2_Y;
  drawPosX[PILL_EVIL_1] = zoomFactor * PILL_EVIL1_X;
  drawPosY[PILL_EVIL_1] = zoomFactor * PILL_EVIL1_Y;
  drawPosX[PILL_EVIL_0] = zoomFactor * PILL_EVIL0_X;
  drawPosY[PILL_EVIL_0] = zoomFactor * PILL_EVIL0_Y;

  drawPosX[PILL_GOOD_15] = zoomFactor * PILL_GOOD15_X;
  drawPosY[PILL_GOOD_15] = zoomFactor * PILL_GOOD15_Y;
  drawPosX[PILL_GOOD_14] = zoomFactor * PILL_GOOD14_X;
  drawPosY[PILL_GOOD_14] = zoomFactor * PILL_GOOD14_Y;
  drawPosX[PILL_GOOD_13] = zoomFactor * PILL_GOOD13_X;
  drawPosY[PILL_GOOD_13] = zoomFactor * PILL_GOOD13_Y;
  drawPosX[PILL_GOOD_12] = zoomFactor * PILL_GOOD12_X;
  drawPosY[PILL_GOOD_12] = zoomFactor * PILL_GOOD12_Y;
  drawPosX[PILL_GOOD_11] = zoomFactor * PILL_GOOD11_X;
  drawPosY[PILL_GOOD_11] = zoomFactor * PILL_GOOD11_Y;
  drawPosX[PILL_GOOD_10] = zoomFactor * PILL_GOOD10_X;
  drawPosY[PILL_GOOD_10] = zoomFactor * PILL_GOOD10_Y;
  drawPosX[PILL_GOOD_9] = zoomFactor * PILL_GOOD9_X;
  drawPosY[PILL_GOOD_9] = zoomFactor * PILL_GOOD9_Y;
  drawPosX[PILL_GOOD_8] = zoomFactor * PILL_GOOD8_X;
  drawPosY[PILL_GOOD_8] = zoomFactor * PILL_GOOD8_Y;
  drawPosX[PILL_GOOD_7] = zoomFactor * PILL_GOOD7_X;
  drawPosY[PILL_GOOD_7] = zoomFactor * PILL_GOOD7_Y;
  drawPosX[PILL_GOOD_6] = zoomFactor * PILL_GOOD6_X;
  drawPosY[PILL_GOOD_6] = zoomFactor * PILL_GOOD6_Y;
  drawPosX[PILL_GOOD_5] = zoomFactor * PILL_GOOD5_X;
  drawPosY[PILL_GOOD_5] = zoomFactor * PILL_GOOD5_Y;
  drawPosX[PILL_GOOD_4] = zoomFactor * PILL_GOOD4_X;
  drawPosY[PILL_GOOD_4] = zoomFactor * PILL_GOOD4_Y;
  drawPosX[PILL_GOOD_3] = zoomFactor * PILL_GOOD3_X;
  drawPosY[PILL_GOOD_3] = zoomFactor * PILL_GOOD3_Y;
  drawPosX[PILL_GOOD_2] = zoomFactor * PILL_GOOD2_X;
  drawPosY[PILL_GOOD_2] = zoomFactor * PILL_GOOD2_Y;
  drawPosX[PILL_GOOD_1] = zoomFactor * PILL_GOOD1_X;
  drawPosY[PILL_GOOD_1] = zoomFactor * PILL_GOOD1_Y;
  drawPosX[PILL_GOOD_0] = zoomFactor * PILL_GOOD0_X;
  drawPosY[PILL_GOOD_0] = zoomFactor * PILL_GOOD0_Y;

  drawPosX[BASE_GOOD] = zoomFactor * BASE_GOOD_X;
  drawPosY[BASE_GOOD] = zoomFactor * BASE_GOOD_Y;
  drawPosX[BASE_NEUTRAL] = zoomFactor * BASE_NEUTRAL_X;
  drawPosY[BASE_NEUTRAL] = zoomFactor * BASE_NEUTRAL_Y;
  drawPosX[BASE_EVIL] = zoomFactor * BASE_EVIL_X;
  drawPosY[BASE_EVIL] = zoomFactor * BASE_EVIL_Y;

  drawPosX[FOREST] = zoomFactor * FOREST_X;
  drawPosY[FOREST] = zoomFactor * FOREST_Y;
  drawPosX[FOREST_SINGLE] = zoomFactor * FOREST_SINGLE_X;
  drawPosY[FOREST_SINGLE] = zoomFactor * FOREST_SINGLE_Y;
  drawPosX[FOREST_BR] = zoomFactor * FOREST_BR_X;
  drawPosY[FOREST_BR] = zoomFactor * FOREST_BR_Y;
  drawPosX[FOREST_BL] = zoomFactor * FOREST_BL_X;
  drawPosY[FOREST_BL] = zoomFactor * FOREST_BL_Y;
  drawPosX[FOREST_AR] = zoomFactor * FOREST_AR_X;
  drawPosY[FOREST_AR] = zoomFactor * FOREST_AR_Y;
  drawPosX[FOREST_AL] = zoomFactor * FOREST_AL_X;
  drawPosY[FOREST_AL] = zoomFactor * FOREST_AL_Y;
  drawPosX[FOREST_ABOVE] = zoomFactor * FOREST_ABOVE_X;
  drawPosY[FOREST_ABOVE] = zoomFactor * FOREST_ABOVE_Y;
  drawPosX[FOREST_BELOW] = zoomFactor * FOREST_BELOW_X;
  drawPosY[FOREST_BELOW] = zoomFactor * FOREST_BELOW_Y;    
  drawPosX[FOREST_LEFT] = zoomFactor * FOREST_LEFT_X;
  drawPosY[FOREST_LEFT] = zoomFactor * FOREST_LEFT_Y;    
  drawPosX[FOREST_RIGHT] = zoomFactor * FOREST_RIGHT_X;
  drawPosY[FOREST_RIGHT] = zoomFactor * FOREST_RIGHT_Y;

  drawPosX[CRATER] = zoomFactor * CRATER_X;
  drawPosY[CRATER] = zoomFactor * CRATER_Y;
  drawPosX[CRATER_SINGLE] = zoomFactor * CRATER_SINGLE_X;
  drawPosY[CRATER_SINGLE] = zoomFactor * CRATER_SINGLE_Y;
  drawPosX[CRATER_BR] = zoomFactor * CRATER_BR_X;
  drawPosY[CRATER_BR] = zoomFactor * CRATER_BR_Y;
  drawPosX[CRATER_BL] = zoomFactor * CRATER_BL_X;
  drawPosY[CRATER_BL] = zoomFactor * CRATER_BL_Y;
  drawPosX[CRATER_AR] = zoomFactor * CRATER_AR_X;
  drawPosY[CRATER_AR] = zoomFactor * CRATER_AR_Y;
  drawPosX[CRATER_AL] = zoomFactor * CRATER_AL_X;
  drawPosY[CRATER_AL] = zoomFactor * CRATER_AL_Y;
  drawPosX[CRATER_ABOVE] = zoomFactor * CRATER_ABOVE_X;
  drawPosY[CRATER_ABOVE] = zoomFactor * CRATER_ABOVE_Y;
  drawPosX[CRATER_BELOW] = zoomFactor * CRATER_BELOW_X;
  drawPosY[CRATER_BELOW] = zoomFactor * CRATER_BELOW_Y;    
  drawPosX[CRATER_LEFT] = zoomFactor * CRATER_LEFT_X;
  drawPosY[CRATER_LEFT] = zoomFactor * CRATER_LEFT_Y;    
  drawPosX[CRATER_RIGHT] = zoomFactor * CRATER_RIGHT_X;
  drawPosY[CRATER_RIGHT] = zoomFactor * CRATER_RIGHT_Y;


  drawPosX[RUBBLE] = zoomFactor * RUBBLE_X;
  drawPosY[RUBBLE] = zoomFactor * RUBBLE_Y;
  drawPosX[GRASS] = zoomFactor * GRASS_X;
  drawPosY[GRASS] = zoomFactor * GRASS_Y;
  drawPosX[HALFBUILDING] = zoomFactor * SHOT_BUILDING_X;
  drawPosY[HALFBUILDING] = zoomFactor * SHOT_BUILDING_Y;

  drawPosX[BOAT_0] = zoomFactor * BOAT0_X;
  drawPosY[BOAT_0] = zoomFactor * BOAT0_Y;
  drawPosX[BOAT_1] = zoomFactor * BOAT1_X;
  drawPosY[BOAT_1] = zoomFactor * BOAT1_Y;
  drawPosX[BOAT_2] = zoomFactor * BOAT2_X;
  drawPosY[BOAT_2] = zoomFactor * BOAT2_Y;
  drawPosX[BOAT_3] = zoomFactor * BOAT3_X;
  drawPosY[BOAT_3] = zoomFactor * BOAT3_Y;
  drawPosX[BOAT_4] = zoomFactor * BOAT4_X;
  drawPosY[BOAT_4] = zoomFactor * BOAT4_Y;
  drawPosX[BOAT_5] = zoomFactor * BOAT5_X;
  drawPosY[BOAT_5] = zoomFactor * BOAT5_Y;
  drawPosX[BOAT_6] = zoomFactor * BOAT6_X;
  drawPosY[BOAT_6] = zoomFactor * BOAT6_Y;
  drawPosX[BOAT_7] = zoomFactor * BOAT7_X;
  drawPosY[BOAT_7] = zoomFactor * BOAT7_Y;

  /* Draw Tank frames */

  /* Do I want to do this? 
  drawTankPosX[TANK_SELF_0] = zoomFactor * TANK_SELF_0_X;
  drawTankPosY[TANK_SELF_0] = zoomFactor * TANK_SELF_0_Y;
  drawTankPosX[TANK_SELF_1] = zoomFactor * TANK_SELF_1_X;
  drawTankPosY[TANK_SELF_1] = zoomFactor * TANK_SELF_1_Y;
  drawTankPosX[TANK_SELF_2] = zoomFactor * TANK_SELF_2_X;
  drawTankPosY[TANK_SELF_2] = zoomFactor * TANK_SELF_2_Y;
  drawTankPosX[TANK_SELF_3] = zoomFactor * TANK_SELF_3_X;
  drawTankPosY[TANK_SELF_3] = zoomFactor * TANK_SELF_3_Y;
  drawTankPosX[TANK_SELF_4] = zoomFactor * TANK_SELF_4_X;
  drawTankPosY[TANK_SELF_4] = zoomFactor * TANK_SELF_4_Y;
  drawTankPosX[TANK_SELF_5] = zoomFactor * TANK_SELF_5_X;
  drawTankPosY[TANK_SELF_5] = zoomFactor * TANK_SELF_5_Y;
  drawTankPosX[TANK_SELF_6] = zoomFactor * TANK_SELF_6_X;
  drawTankPosY[TANK_SELF_6] = zoomFactor * TANK_SELF_6_Y;
  drawTankPosX[TANK_SELF_7] = zoomFactor * TANK_SELF_7_X;
  drawTankPosY[TANK_SELF_7] = zoomFactor * TANK_SELF_7_Y;
  drawTankPosX[TANK_SELF_8] = zoomFactor * TANK_SELF_8_X;
  drawTankPosY[TANK_SELF_8] = zoomFactor * TANK_SELF_8_Y;
  drawTankPosX[TANK_SELF_9] = zoomFactor * TANK_SELF_9_X;
  drawTankPosY[TANK_SELF_9] = zoomFactor * TANK_SELF_9_Y;
  drawTankPosX[TANK_SELF_10] = zoomFactor * TANK_SELF_10_X;
  drawTankPosY[TANK_SELF_10] = zoomFactor * TANK_SELF_10_Y;
  drawTankPosX[TANK_SELF_11] = zoomFactor * TANK_SELF_11_X;
  drawTankPosY[TANK_SELF_11] = zoomFactor * TANK_SELF_11_Y;
  drawTankPosX[TANK_SELF_12] = zoomFactor * TANK_SELF_12_X;
  drawTankPosY[TANK_SELF_12] = zoomFactor * TANK_SELF_12_Y;
  drawTankPosX[TANK_SELF_13] = zoomFactor * TANK_SELF_13_X;
  drawTankPosY[TANK_SELF_13] = zoomFactor * TANK_SELF_13_Y;
  drawTankPosX[TANK_SELF_14] = zoomFactor * TANK_SELF_14_X;
  drawTankPosY[TANK_SELF_14] = zoomFactor * TANK_SELF_14_Y;
  drawTankPosX[TANK_SELF_15] = zoomFactor * TANK_SELF_15_X;
  drawTankPosY[TANK_SELF_15] = zoomFactor * TANK_SELF_15_Y;
  
  drawTankPosX[TANK_SELFBOAT_0] = zoomFactor * TANK_SELFBOAT_0_X;
  drawTankPosY[TANK_SELFBOAT_0] = zoomFactor * TANK_SELFBOAT_0_Y;
  drawTankPosX[TANK_SELFBOAT_1] = zoomFactor * TANK_SELFBOAT_1_X;
  drawTankPosY[TANK_SELFBOAT_1] = zoomFactor * TANK_SELFBOAT_1_Y;
  drawTankPosX[TANK_SELFBOAT_2] = zoomFactor * TANK_SELFBOAT_2_X;
  drawTankPosY[TANK_SELFBOAT_2] = zoomFactor * TANK_SELFBOAT_2_Y;
  drawTankPosX[TANK_SELFBOAT_3] = zoomFactor * TANK_SELFBOAT_3_X;
  drawTankPosY[TANK_SELFBOAT_3] = zoomFactor * TANK_SELFBOAT_3_Y;
  drawTankPosX[TANK_SELFBOAT_4] = zoomFactor * TANK_SELFBOAT_4_X;
  drawTankPosY[TANK_SELFBOAT_4] = zoomFactor * TANK_SELFBOAT_4_Y;
  drawTankPosX[TANK_SELFBOAT_5] = zoomFactor * TANK_SELFBOAT_5_X;
  drawTankPosY[TANK_SELFBOAT_5] = zoomFactor * TANK_SELFBOAT_5_Y;
  drawTankPosX[TANK_SELFBOAT_6] = zoomFactor * TANK_SELFBOAT_6_X;
  drawTankPosY[TANK_SELFBOAT_6] = zoomFactor * TANK_SELFBOAT_6_Y;
  drawTankPosX[TANK_SELFBOAT_7] = zoomFactor * TANK_SELFBOAT_7_X;
  drawTankPosY[TANK_SELFBOAT_7] = zoomFactor * TANK_SELFBOAT_7_Y;
  drawTankPosX[TANK_SELFBOAT_8] = zoomFactor * TANK_SELFBOAT_8_X;
  drawTankPosY[TANK_SELFBOAT_8] = zoomFactor * TANK_SELFBOAT_8_Y;
  drawTankPosX[TANK_SELFBOAT_9] = zoomFactor * TANK_SELFBOAT_9_X;
  drawTankPosY[TANK_SELFBOAT_9] = zoomFactor * TANK_SELFBOAT_9_Y;
  drawTankPosX[TANK_SELFBOAT_10] = zoomFactor * TANK_SELFBOAT_10_X;
  drawTankPosY[TANK_SELFBOAT_10] = zoomFactor * TANK_SELFBOAT_10_Y;
  drawTankPosX[TANK_SELFBOAT_11] = zoomFactor * TANK_SELFBOAT_11_X;
  drawTankPosY[TANK_SELFBOAT_11] = zoomFactor * TANK_SELFBOAT_11_Y;
  drawTankPosX[TANK_SELFBOAT_12] = zoomFactor * TANK_SELFBOAT_12_X;
  drawTankPosY[TANK_SELFBOAT_12] = zoomFactor * TANK_SELFBOAT_12_Y;
  drawTankPosX[TANK_SELFBOAT_13] = zoomFactor * TANK_SELFBOAT_13_X;
  drawTankPosY[TANK_SELFBOAT_13] = zoomFactor * TANK_SELFBOAT_13_Y;
  drawTankPosX[TANK_SELFBOAT_14] = zoomFactor * TANK_SELFBOAT_14_X;
  drawTankPosY[TANK_SELFBOAT_14] = zoomFactor * TANK_SELFBOAT_14_Y;
  drawTankPosX[TANK_SELFBOAT_15] = zoomFactor * TANK_SELFBOAT_15_X;
  drawTankPosY[TANK_SELFBOAT_15] = zoomFactor * TANK_SELFBOAT_15_Y; */
}
