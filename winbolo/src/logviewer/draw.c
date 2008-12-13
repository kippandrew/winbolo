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
*Last Modified:  29/4/00
*Purpose:
*  System Specific Drawing routines (Uses Direct Draw)
*********************************************************/

#include <math.h>


#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "resource.h"
#include "ddraw.h"



#include <commdlg.h>
#include <string.h>
#include <ddraw.h>
#include "global.h"
#include "screenlgm.h"
#include "tiles.h"
#include "tilenum.h"
#include "positions.h"
#include "font.h"
#include "resource.h"
#include "dialogfullscreensetup.h"
#include "draw.h"

#define TEAM_IMAGE_SIZE_X (16 * 16)
#define TEAM_IMAGE_SIZE_Y (17 * 16)
#define ITEM_IMAGE_SIZE_X (17 * 16)
#define ITEM_IMAGE_SIZE_Y (17 * 16)


/* Direct Draw Surfaces */
LPDIRECTDRAW lpDD = NULL;
LPDIRECTDRAWSURFACE lpDDSPrimary = NULL;
LPDIRECTDRAWSURFACE lpDDSBackBuffer = NULL;
LPDIRECTDRAWSURFACE lpDDSTiles = NULL;
LPDIRECTDRAWSURFACE lpDDSTanks = NULL;
LPDIRECTDRAWSURFACE lpDDSBoats = NULL;
LPDIRECTDRAWSURFACE lpDDSItems = NULL;

LPDIRECTDRAWSURFACE lpDDSTankLabels = NULL;
LPDIRECTDRAWCLIPPER lpDDClipper = NULL;
WNDCLASS drawWC;      /* Window Class to create */
HWND drawWnd = NULL;

/* Used for storing time */
DWORD	g_dwFrameTime = 0;
/* Number of frames dispayed this second */
DWORD	g_dwFrameCount = 0;
/* The total frames per second for last second */
DWORD g_dwFrameTotal = 0;


int drawPosX[255];
int drawPosY[255];

int drawLast[100][100];

extern BYTE tc[17];
extern bool useTeamColours;
extern bool useFullscreen;

BYTE screenGetPillTeam(BYTE x, BYTE y, BYTE *pillHealth);
BYTE screenGetBaseTeam(BYTE x, BYTE y);


BYTE windowGetZoomFactor() {
  return 1;
}

HINSTANCE windowGetInstance() {
  return NULL;
}

// Dirtys the entire screen forcing a redraw next update
void drawDirtyScreen() {
  int count;
  int count2;
  /* Fix the back buffer */
  count = 0;
  while (count < 100) {
    count2 = 0;
    while (count2 < 100) {
      drawLast[count][count2] = 10000;
      count2++;
    }
    count++;
  }

}

long FAR PASCAL windowFullScreenProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

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
  HRESULT res;            /* Direct Draw Function returns */
  DDSURFACEDESC primDesc; /* Surface description */
  HBITMAP hTiles = NULL;  /* The tile file bitmap resource */
  HDC hTilesDC = NULL;    /* The tile file resource DC */
  HDC hDDSTilesDC = NULL; /* Temp DC of a DDS use to copy tile file into */
  DDPIXELFORMAT ddpf;     /* DD Pixel Format to get green */


  /* Palette Checking */
  DDSURFACEDESC   ddsd;

  returnValue = TRUE;

  /* Set up the Window Class */
  drawWC.hCursor        = NULL;;
  drawWC.hIcon          = NULL;
  drawWC.lpszMenuName   = NULL;
  drawWC.lpszClassName  = "LOGVIEWERFS";
  drawWC.hbrBackground  = 0;
  drawWC.hInstance      = appInst;
  drawWC.style          = 0;
  drawWC.lpfnWndProc    = windowFullScreenProc;
  drawWC.cbClsExtra     = 0;
  drawWC.cbWndExtra     = 0;
  drawWC.hbrBackground  = 0;
  res = RegisterClass( &drawWC );

  drawDirtyScreen();

  /* Create the Direct Draw Object */
  
  res = DirectDrawCreate(NULL, &lpDD, NULL);
  if (FAILED(res)) {
    returnValue = FALSE;
    drawCleanup();
    MessageBox(NULL, "Error Starting DirectDraw", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
  }
  
  if (useFullscreen == TRUE) {
    drawWnd = CreateWindowEx(
        WS_EX_TOPMOST,
        "LOGVIEWERFS",
        "WinBolo LogViewer Full Screen",
        WS_POPUP,
        0, 100,
        GetSystemMetrics( SM_CXSCREEN ),
        GetSystemMetrics( SM_CYSCREEN ),
        NULL,
        NULL,
        appInst,
        NULL );
    ShowWindow(drawWnd, SW_HIDE);
//      UpdateWindow( drawWnd);
  }
  if (drawWnd == NULL) {
    DWORD d;
    d = GetLastError();
    d = d;
  }


  /* Set the co-op level */
  if (returnValue == TRUE) {
    if (useFullscreen == TRUE) {
      res = lpDD->lpVtbl->SetCooperativeLevel(lpDD, drawWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
    } else {
      res = lpDD->lpVtbl->SetCooperativeLevel(lpDD,NULL,DDSCL_NORMAL);
    }
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Error Starting DirectDraw (Co-op!)", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }  
  
  // Set the Video Mode
  if (returnValue == TRUE && useFullscreen == TRUE) {
    res = lpDD->lpVtbl->SetDisplayMode(lpDD, 800, 600, 16);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Unable to set display mode", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }


  /* Create the primary surface */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    if (useFullscreen == TRUE) {
      primDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
      primDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
      primDesc.dwBackBufferCount = 1;
    } else {
      primDesc.dwFlags = DDSD_CAPS;
      primDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    }
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSPrimary, NULL);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Error Starting DirectDraw", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }
  
  /* Get the pixel format */
  if (returnValue == TRUE) {
    ddpf.dwSize = sizeof(ddpf);
    res = lpDDSPrimary->lpVtbl->GetPixelFormat(lpDDSPrimary, &ddpf);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Error Starting DirectDraw", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }

  /* Palette Checking Stuff */
  if (returnValue == TRUE) {
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    if (FAILED(lpDDSPrimary->lpVtbl->GetSurfaceDesc(lpDDSPrimary, &ddsd))) {
      returnValue = FALSE;
      drawCleanup();
      MessageBox(NULL, "Error Starting DirectDraw", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    }

    if (( ddsd.ddpfPixelFormat.dwRGBBitCount < 16 )) { /* FIXME: Was also (ddsd.ddpfPixelFormat.dwFlags != DDPF_RGB) || but taken out as it stops matrox G4's from working in 32bit colour mode */
      returnValue = FALSE;
      drawCleanup();
      MessageBox(NULL, "Error Starting DirectDraw", DIALOG_BOX_TITLE, MB_ICONINFORMATION);
    }
  }
  /* End Palette Checking Stuff */



  /* Create the back buffer surface */
  if (returnValue == TRUE)
    
    if (useFullscreen == FALSE) {
      ZeroMemory(&primDesc, sizeof (primDesc));
      primDesc.dwSize = sizeof (primDesc);
      primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
      primDesc.dwWidth = screenGetSizeX() * TILE_SIZE_X;
      primDesc.dwHeight = screenGetSizeY() * TILE_SIZE_Y;
      primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
      res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSBackBuffer, NULL);
    } else {
      DDSCAPS ddscaps;
      ZeroMemory(&ddscaps, sizeof(ddscaps));
      ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
      res = lpDDSPrimary->lpVtbl->GetAttachedSurface(lpDDSPrimary, &ddscaps, &lpDDSBackBuffer);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Error Starting DirectDraw BB", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
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
    primDesc.dwWidth = TILE_FILE_X;
    primDesc.dwHeight =TILE_FILE_Y;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSTiles, NULL);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Creating DD Tile buffer and copying resource into it Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Copy the bitmap into it */
      hTiles = (HBITMAP) LoadImage(appInst, MAKEINTRESOURCE(IDB_TILE), IMAGE_BITMAP, TILE_FILE_X , TILE_FILE_Y, LR_CREATEDIBSECTION);
      if (hTiles !=NULL) {
        hTilesDC = CreateCompatibleDC(NULL);
        SelectObject(hTilesDC, hTiles);
        lpDDSTiles->lpVtbl->Restore(lpDDSTiles);
        res = lpDDSTiles->lpVtbl->GetDC(lpDDSTiles,&hDDSTilesDC);
        if (res != DD_OK) {
          drawCleanup();
          MessageBox(NULL, "Error Starting DirectDraw CP", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
        }
        StretchBlt(hDDSTilesDC, 0, 0, TILE_FILE_X, TILE_FILE_Y, hTilesDC, 0, 0, TILE_FILE_X, TILE_FILE_Y, SRCCOPY);
        res = lpDDSTiles->lpVtbl->ReleaseDC(lpDDSTiles, hDDSTilesDC);
        DeleteDC(hTilesDC);
        DeleteObject(hTiles);
      } else {
        drawCleanup();
        MessageBox(NULL, "Creating DD Tile buffer and copying resource into it Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
        returnValue = FALSE;
      }
    }
  }

  /* Create the tanks buffer and copy the bitmap into it */
  if (returnValue == TRUE) {
    /* Create the buffer */
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
    primDesc.ddckCKSrcBlt.dwColorSpaceLowValue = ddpf.dwGBitMask;
    primDesc.ddckCKSrcBlt.dwColorSpaceHighValue = ddpf.dwGBitMask;
    primDesc.dwWidth = TEAM_IMAGE_SIZE_X;
    primDesc.dwHeight = TEAM_IMAGE_SIZE_Y;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSTanks, NULL);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Creating DD Tank buffer and copying resource into it Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Copy the bitmap into it */

      hTiles = (HBITMAP) LoadImage(appInst, MAKEINTRESOURCE(IDB_TANKS), IMAGE_BITMAP, TEAM_IMAGE_SIZE_X, TEAM_IMAGE_SIZE_Y, LR_CREATEDIBSECTION);
      if (hTiles != NULL) {
        hTilesDC = CreateCompatibleDC(NULL);
        SelectObject(hTilesDC, hTiles);
        lpDDSTanks->lpVtbl->Restore(lpDDSTiles);
        res = lpDDSTanks->lpVtbl->GetDC(lpDDSTanks, &hDDSTilesDC);
        if (res != DD_OK) {
          drawCleanup();
          MessageBox(NULL, "Error Starting DirectDraw LI", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
        }
        StretchBlt(hDDSTilesDC, 0, 0, TEAM_IMAGE_SIZE_X, TEAM_IMAGE_SIZE_Y, hTilesDC, 0, 0, TEAM_IMAGE_SIZE_X, TEAM_IMAGE_SIZE_Y, SRCCOPY);
        res = lpDDSTanks->lpVtbl->ReleaseDC(lpDDSTanks, hDDSTilesDC);
        DeleteDC(hTilesDC);
        DeleteObject(hTiles);
      } else {
        drawCleanup();
        MessageBox(NULL, "Creating DD Tanks buffer and copying resource into it Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
        returnValue = FALSE;
      }
    }
  }

  /* Create the boats buffer and copy the bitmap into it */
  if (returnValue == TRUE) {
    /* Create the buffer */
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
    primDesc.ddckCKSrcBlt.dwColorSpaceLowValue = ddpf.dwGBitMask;
    primDesc.ddckCKSrcBlt.dwColorSpaceHighValue = ddpf.dwGBitMask;
    primDesc.dwWidth = TEAM_IMAGE_SIZE_X;
    primDesc.dwHeight = TEAM_IMAGE_SIZE_Y;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSBoats, NULL);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Creating DD boat buffer and copying resource into it Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Copy the bitmap into it */

      hTiles = (HBITMAP) LoadImage(appInst, MAKEINTRESOURCE(IDB_BOATS), IMAGE_BITMAP, TEAM_IMAGE_SIZE_X, TEAM_IMAGE_SIZE_Y, LR_CREATEDIBSECTION);
      if (hTiles != NULL) {
        hTilesDC = CreateCompatibleDC(NULL);
        SelectObject(hTilesDC, hTiles);
        lpDDSBoats->lpVtbl->Restore(lpDDSBoats);
        res = lpDDSBoats->lpVtbl->GetDC(lpDDSBoats, &hDDSTilesDC);
        if (res != DD_OK) {
          drawCleanup();
          MessageBox(NULL, "Error Starting DirectDraw BB", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
        }
        StretchBlt(hDDSTilesDC, 0, 0, TEAM_IMAGE_SIZE_X, TEAM_IMAGE_SIZE_Y, hTilesDC, 0, 0, TEAM_IMAGE_SIZE_X, TEAM_IMAGE_SIZE_Y, SRCCOPY);
        res = lpDDSBoats->lpVtbl->ReleaseDC(lpDDSBoats, hDDSTilesDC);
        DeleteDC(hTilesDC);
        DeleteObject(hTiles);
      } else {
        drawCleanup();
        MessageBox(NULL, "Creating DD boats buffer and copying resource into it Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
        returnValue = FALSE;
      }
    }
  }

  /* Create the items buffer and copy the bitmap into it */
  if (returnValue == TRUE) {
    /* Create the buffer */
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
    primDesc.ddckCKSrcBlt.dwColorSpaceLowValue = ddpf.dwGBitMask;
    primDesc.ddckCKSrcBlt.dwColorSpaceHighValue = ddpf.dwGBitMask;
    primDesc.dwWidth = ITEM_IMAGE_SIZE_X;
    primDesc.dwHeight = ITEM_IMAGE_SIZE_Y;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSItems, NULL);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Creating DD items buffer and copying resource into it Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    } else {
      /* Copy the bitmap into it */

      hTiles = (HBITMAP) LoadImage(appInst, MAKEINTRESOURCE(IDB_ITEMS), IMAGE_BITMAP, ITEM_IMAGE_SIZE_X, ITEM_IMAGE_SIZE_Y, LR_CREATEDIBSECTION);
      if (hTiles != NULL) {
        hTilesDC = CreateCompatibleDC(NULL);
        SelectObject(hTilesDC, hTiles);
        lpDDSItems->lpVtbl->Restore(lpDDSItems);
        res = lpDDSItems->lpVtbl->GetDC(lpDDSItems, &hDDSTilesDC);
        if (res != DD_OK) {
          drawCleanup();
          MessageBox(NULL, "Error Starting DirectDraw", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
        }
        StretchBlt(hDDSTilesDC, 0, 0, ITEM_IMAGE_SIZE_X, ITEM_IMAGE_SIZE_Y, hTilesDC, 0, 0, ITEM_IMAGE_SIZE_X, ITEM_IMAGE_SIZE_Y, SRCCOPY);
        res = lpDDSItems->lpVtbl->ReleaseDC(lpDDSItems, hDDSTilesDC);
        DeleteDC(hTilesDC);
        DeleteObject(hTiles);
      } else {
        drawCleanup();
        MessageBox(NULL, "Creating DD items buffer and copying resource into it Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
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
    primDesc.dwWidth = screenGetSizeX() * TILE_SIZE_X; //zoomFactor * TANK_LABEL_WIDTH;
    primDesc.dwHeight = (2* TANK_LABEL_HEIGHT - 20);
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    res = lpDD->lpVtbl->CreateSurface(lpDD, &primDesc, &lpDDSTankLabels, NULL);
    if (FAILED(res)) {
      drawCleanup();
      returnValue = FALSE;
      MessageBox(NULL, "Creating DD Tank Label Back buffer Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    }
  }


  /* Create the clipper */
  if (returnValue == TRUE && useFullscreen == FALSE) {
    res = lpDD->lpVtbl->CreateClipper(lpDD, 0, &lpDDClipper, 0);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Error Starting DirectDraw CLI", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }

  /* Set the clipper to the window */
  if (returnValue == TRUE && useFullscreen == FALSE) {
    res = lpDDClipper->lpVtbl->SetHWnd(lpDDClipper, 0, appWnd);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Error Starting DirectDraw CLI2", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }

  /* Set the Primary Surface to use the clipper */
  if (returnValue == TRUE && useFullscreen == FALSE) {
    res = lpDDSPrimary->lpVtbl->SetClipper(lpDDSPrimary, lpDDClipper);
    if (FAILED(res)) {
      drawCleanup();
      MessageBox(NULL, "Associating DD Clipper with surface Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
      lpDDClipper = NULL;
    }
  }

  /* Release the clipper */
  if (lpDDClipper != NULL && useFullscreen == FALSE) {  
    lpDDClipper->lpVtbl->Release(lpDDClipper);
    lpDDClipper = NULL;
  }

  drawSetupArrays(1);
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

  if (lpDDSTankLabels != NULL) {
    lpDDSTankLabels->lpVtbl->Release(lpDDSTankLabels);
    lpDDSTankLabels = NULL;
  }
  if (lpDDSTiles != NULL) {
    lpDDSTiles->lpVtbl->Release(lpDDSTiles);
    lpDDSTiles = NULL;
  }
  if (lpDDSTanks != NULL) {
    lpDDSTanks->lpVtbl->Release(lpDDSTanks);
    lpDDSTanks = NULL;
  }
  if (lpDDSBoats != NULL) {
    lpDDSBoats->lpVtbl->Release(lpDDSBoats);
    lpDDSBoats = NULL;
  }
  if (lpDDSItems != NULL) {
    lpDDSItems->lpVtbl->Release(lpDDSItems);
    lpDDSItems = NULL;
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

  if (drawWnd != NULL) {
    DestroyWindow(drawWnd);
    drawWnd = NULL;
  }
}

void drawLogo(HINSTANCE appInst, HWND appWnd, RECT *rcWindow) {
  BYTE zoomFactor;
  HBITMAP hBg = NULL; /* The background bitmap resource */
  HDC hBgDC = NULL;   /* The background resource DC */
  HDC hDC;
  int width = rcWindow->right - rcWindow->left;
  int height = rcWindow->bottom - rcWindow->top;
  int x = 0;
  int y = 0;
  
  hBg = (HBITMAP) LoadImage(appInst, MAKEINTRESOURCE(IDB_SPLASH), IMAGE_BITMAP, 800, 600, LR_CREATEDIBSECTION);

  zoomFactor = windowGetZoomFactor();
  if (hBg != NULL) {
    hBgDC = CreateCompatibleDC(NULL);
    SelectObject(hBgDC, hBg);
    hDC = GetDC(appWnd);
    if (hDC != NULL) {
      BitBlt(hDC, 0, 0, width, height, NULL, 0, 0, BLACKNESS);
      if (width > 800) {
        x = (width - 800) / 2;
      }
      if (height > 600){
		  y = ((height - 600) / 2) - rcWindow->top - 10;
      }
      BitBlt(hDC, x, y, x+800, y+600, hBgDC, 0, 0, SRCCOPY);
    } 
    DeleteDC(hBgDC);
    DeleteObject(hBg);
    ReleaseDC(appWnd, hDC);
  } 

}

bool testDB = FALSE;

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
*********************************************************/
void drawMainScreen(screen *value, screenMines *mineView, screenTanks *tks, screenGunsight *gs, screenBullets *sBullets, screenLgm *lgms, RECT *rcWindow, bool showPillLabels, bool showBaseLabels, long srtDelay, bool isPillView, int edgeX, int edgeY, bool useCursor, BYTE cursorLeft, BYTE cursorTop) {
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
  bool isPill;     /* Is the square a pill */
  bool isBase;     /* Is the square a base */
  bool shouldDraw; /* Do we need to draw */
  HRESULT res;
  BYTE itc;        /* Item team colour */
  BYTE pillHealth; /* Pillbox health */


  if (testDB == TRUE) {
//DEBUG    windowAddEvent(0, "Bad");
    return;
  }

  testDB = TRUE;

  if (lpDDSPrimary->lpVtbl->IsLost(lpDDSPrimary) == DDERR_SURFACELOST) {
    drawRestore();
  }
  if (lpDDSBackBuffer->lpVtbl->IsLost(lpDDSBackBuffer) == DDERR_SURFACELOST) {
    lpDDSBackBuffer->lpVtbl->Restore(lpDDSBackBuffer);
  }
 
  count = 0;
  x = 0;
  y = 0;
  done = FALSE;
  zoomFactor = windowGetZoomFactor();
  str[0] = '\0';
  

  while (done == FALSE) {
    pos = screenGetPos(value,x,y);
	  shouldDraw = FALSE;
    if (drawLast[x][y] == 10000) {
      shouldDraw = TRUE;
	  } else if (screenIsMine(mineView, x, y) == TRUE) {
      if (drawLast[x][y] != (0 - pos)) {
		    shouldDraw = TRUE;
      }
    } else if (drawLast[x][y] != pos) {
		  shouldDraw = TRUE;
	  }
    if (shouldDraw == TRUE) {
  		drawLast[x][y] = pos;
      isPill = FALSE;
      isBase = FALSE;
      if (ROAD_HORZ == pos) {
        pos = ROAD_HORZ;
      }
      if (pos == PILL_EVIL_15 || pos == PILL_EVIL_14 || pos == PILL_EVIL_13 || pos == PILL_EVIL_12 || pos == PILL_EVIL_11 || pos == PILL_EVIL_10 || pos == PILL_EVIL_9 || pos == PILL_EVIL_8 || pos == PILL_EVIL_7 || pos == PILL_EVIL_6 || pos == PILL_EVIL_5 || pos == PILL_EVIL_4 || pos == PILL_EVIL_3 || pos == PILL_EVIL_2 || pos == PILL_EVIL_1 || pos == PILL_EVIL_0) {
        isPill = TRUE;
        drawLast[x][y] = 10000;
      }
      if (pos == PILL_GOOD_15 || pos == PILL_GOOD_14 || pos == PILL_GOOD_13 || pos == PILL_GOOD_12 || pos == PILL_GOOD_11 || pos == PILL_GOOD_10 || pos == PILL_GOOD_9 || pos == PILL_GOOD_8 || pos == PILL_GOOD_7 || pos == PILL_GOOD_6 || pos == PILL_GOOD_5 || pos == PILL_GOOD_4 || pos == PILL_GOOD_3 || pos == PILL_GOOD_2 || pos == PILL_GOOD_1 || pos == PILL_GOOD_0) {
        isPill = TRUE;
        drawLast[x][y] = 10000;
      }
      if (pos == BASE_GOOD || pos == BASE_NEUTRAL || pos == BASE_EVIL) {
        isBase = TRUE;
        drawLast[x][y] = 10000;
      }

      /* Drawing */
    
      if (isPill == TRUE && useTeamColours == TRUE) {
        itc = screenGetPillTeam(x, y, &pillHealth);
        output.left = pillHealth * zoomFactor * TILE_SIZE_X;
        output.right = output.left + zoomFactor * TILE_SIZE_X;
        output.top = tc[itc] * zoomFactor * TILE_SIZE_Y;
        output.bottom = output.top + zoomFactor * TILE_SIZE_Y;
        lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, zoomFactor * (x * TILE_SIZE_X), (zoomFactor * y * TILE_SIZE_Y), lpDDSItems ,&output, DDBLTFAST_WAIT);
      } else if (isBase == TRUE && useTeamColours == TRUE) {
        itc = screenGetBaseTeam(x, y);
        output.left = 16 * zoomFactor * TILE_SIZE_X;
        output.right = output.left + zoomFactor * TILE_SIZE_X;
        output.top = tc[itc] * zoomFactor * TILE_SIZE_Y;
        output.bottom = output.top + zoomFactor * TILE_SIZE_Y;
        lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, zoomFactor * (x * TILE_SIZE_X), (zoomFactor * y * TILE_SIZE_Y), lpDDSItems ,&output, DDBLTFAST_WAIT);
      } else {
        /* Draw the map block */
        outputX = drawPosX[pos];
        outputY = drawPosY[pos];
        output.left = outputX;
        output.right = outputX + zoomFactor * TILE_SIZE_X;
        output.top = outputY;
        output.bottom = outputY + zoomFactor * TILE_SIZE_Y;
        lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, zoomFactor * (x * TILE_SIZE_X), (zoomFactor * y * TILE_SIZE_Y), lpDDSTiles ,&output, DDBLTFAST_WAIT);
      }
      /* Draw Mines */
      if ((screenIsMine(mineView,x,y)) == TRUE) { 
        output.left = zoomFactor * MINE_X;
        output.top = zoomFactor * MINE_Y;
        output.right = zoomFactor * MINE_X + zoomFactor * TILE_SIZE_X;
        output.bottom = zoomFactor * MINE_Y + zoomFactor * TILE_SIZE_Y;
        if (lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, (zoomFactor * x * TILE_SIZE_X) , (zoomFactor * y * TILE_SIZE_Y), lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY) != DD_OK) {
          output.bottom = 0;
        }
        drawLast[x][y] = -pos;

      }
    }

    /* Increment the variable */
    x++;
    if (x == screenGetSizeX()) {
      y++;
      x = 0;
      if (y == screenGetSizeY()) {
        done = TRUE;
      }
    }
  }

  /* Draw Explosions if Required */
  drawShells(sBullets);


  /* Draw the tank */
  drawTanks(tks);
  drawLGMs(lgms);

  /* Copy the back buffer to the window */
  output.left = 0;
  output.top = 0;
  output.right = (zoomFactor * screenGetSizeX() * TILE_SIZE_X);
  output.bottom = (zoomFactor * screenGetSizeY() * TILE_SIZE_Y);
  rcWindow->right = rcWindow->left  + (output.right - output.left);
  rcWindow->bottom = rcWindow->top + (output.bottom - output.top);


  /* Item Test *
  output.left = 0;
  output.right = ITEM_IMAGE_SIZE_X;
  output.top  = 0;
  output.bottom = ITEM_IMAGE_SIZE_Y;
  rcWindow->right = rcWindow->left  + (output.right - output.left);
  rcWindow->bottom = rcWindow->top + (output.bottom - output.top);
  res = lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, rcWindow, lpDDSItems, &output, DDBLT_WAIT, NULL);

  /* Boats Test *
  output.left = 0;
  output.right = TEAM_IMAGE_SIZE_X;
  output.top  = 0;
  output.bottom = TEAM_IMAGE_SIZE_Y;
  rcWindow->right = rcWindow->left  + (output.right - output.left);
  rcWindow->bottom = rcWindow->top + (output.bottom - output.top);
  res = lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, rcWindow, lpDDSBoats, &output, DDBLT_WAIT, NULL);
*/

  if (useFullscreen == FALSE) {
    res = lpDDSPrimary->lpVtbl->Blt(lpDDSPrimary, rcWindow, lpDDSBackBuffer, &output, DDBLT_WAIT, NULL);
  } else {
    lpDDSPrimary->lpVtbl->Flip( lpDDSPrimary, NULL, DDFLIP_WAIT );
  }
  if (res != DD_OK) {
    switch (res) {
    case DDERR_GENERIC:
    output.left = 0;
  break;
    case DDERR_INVALIDCLIPLIST :
    output.left = 0;
  break;
    case DDERR_INVALIDOBJECT :
    output.left = 0;
  break;
    case DDERR_INVALIDPARAMS :
    output.left = 0;
  break;
    case DDERR_INVALIDRECT :
    output.left = 0;
  break;
    case DDERR_NOALPHAHW :
    output.left = 0;
  break;
    case DDERR_NOBLTHW :
    output.left = 0;
  break;
    case DDERR_NOCLIPLIST :
    output.left = 0;
  break;
    case DDERR_NODDROPSHW: 
    output.left = 0;
  break;
    case DDERR_NOMIRRORHW :
    output.left = 0;
  break;
          
    case DDERR_NORASTEROPHW :
    output.left = 0;
  break;
    case DDERR_NOROTATIONHW :
    output.left = 0;
  break;
    case DDERR_NOSTRETCHHW :
    output.left = 0;
  break;
    case DDERR_NOZBUFFERHW :
    output.left = 0;
  break;
    case DDERR_SURFACEBUSY :
                output.left = 0;
  break;
    case DDERR_SURFACELOST :
    output.left = 0;
  break;
    case DDERR_UNSUPPORTED :
    output.left = 0;
  break;
    case DDERR_WASSTILLDRAWING:
    output.left = 0;
  break;
    }
  }
  testDB = FALSE;

}


void drawMarkRedraw(int mx, int my, int px, int py, int itemSize) {
	drawLast[mx][my] = 10000;
	if (px > itemSize && mx < 100) {
	    drawLast[mx+1][my] = 10000;
	    if (my < 100) {
			drawLast[mx+1][my+1] = 10000;
	    }

	}
	if (py > itemSize && my < 100) {
		drawLast[mx][my+1] = 10000;
	    if (mx < 100) {
			drawLast[mx+1][my+1] = 10000;
	    }
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
	if (frame >= SHELL_EXPLOSION8 && frame <= SHELL_EXPLOSION1) {
      drawMarkRedraw(mx, my, px, py, 0);
	} else {
      drawMarkRedraw(mx, my, px, py, 10);
	}

    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
  }
}

/*********************************************************
*NAME:          drawTanks
*AUTHOR:        John Morrison
*CREATION DATE: 6/1/99
*LAST MODIFIED: 2/2/99
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
  char playerName[PLAYER_NAME_LEN]; /* Player name */
  BYTE frame;                       /* Frame id */
  BYTE mx;                          /* Map X and Y Co-ords */
  BYTE my;
  BYTE px;                          /* Pixel offsets */
  BYTE py;
  BYTE team;
  BYTE zoomFactor;                  /* Scaling Factor */
  BYTE dir;
  BYTE onBoat;
  
  count = 1;
  total = screenTanksGetNumEntries(tks);
  zoomFactor = windowGetZoomFactor();
  
  while (count <= total) {
    screenTanksGetItem(tks, count, &mx, &my, &px, &py, &frame, &team, &dir, &onBoat, playerName);
    px += 2;
    py += 2;
    x = mx * (zoomFactor * TILE_SIZE_X) + (zoomFactor * px);
    y = my * (zoomFactor * TILE_SIZE_Y) + (zoomFactor * py);

    
    if (useTeamColours == FALSE) {
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
        /* Output */
        output.left *= zoomFactor;
        output.top *= zoomFactor;
        output.right = output.left + (zoomFactor * TILE_SIZE_X);
        output.bottom = output.top + (zoomFactor * TILE_SIZE_Y);
        lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTiles, &output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

      } else {
        /* Using team colours */
        output.left = zoomFactor * TILE_SIZE_X * dir;
        output.top = zoomFactor * TILE_SIZE_Y * tc[team];
        output.right = output.left + (zoomFactor * TILE_SIZE_X);
        output.bottom = output.top + (zoomFactor * TILE_SIZE_Y);
        if (onBoat == TRUE) {
          lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSBoats, &output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
        } else{
          lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTanks, &output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
        }

      }
    /* Output the label */
    drawTankLabel(playerName, mx, my, px, py);
    drawMarkRedraw(mx, my, px, py, 0);
    count++;
  }

}

/*********************************************************
*NAME:          drawLGMs
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
        drawMarkRedraw(mx, my, px, py, 12);
        break;
      case LGM1:
        output.left = zf * LGM1_X;
        output.top = zf * LGM1_Y;
        output.right = zf * LGM1_X + zf * LGM_WIDTH;
        output.bottom = zf * LGM1_Y + zf * LGM_HEIGHT;
        drawMarkRedraw(mx, my, px, py, 12);
        break;
      case LGM2:
        output.left = zf * LGM2_X;
        output.top = zf * LGM2_Y;
        output.right = zf * LGM2_X + zf * LGM_WIDTH;
        output.bottom = zf * LGM2_Y + zf * LGM_HEIGHT;
        drawMarkRedraw(mx, my, px, py, 12);
        break;
      default:
        /* LGM3 - Helicopter */
        output.left = zf * LGM_HELICOPTER_X;
        output.top = zf * LGM_HELICOPTER_Y;
        output.right = zf * LGM_HELICOPTER_X + zf * TILE_SIZE_X;
        output.bottom = zf * LGM_HELICOPTER_Y + zf * TILE_SIZE_Y;
        drawMarkRedraw(mx, my, px, py, 1);
        break;
    }
    x = (zf * mx * TILE_SIZE_X) + (zf * px);
    y = (zf * my * TILE_SIZE_Y) + (zf * py);
    lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTiles ,&output, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
  }
}
/*********************************************************
*NAME:          drawTankLabel
*AUTHOR:        John Morrison
*CREATION DATE: 2/2/98
*LAST MODIFIED: 2/2/98
*PURPOSE:
*  Draws the tank label if required.
*
*ARGUMENTS:
*  str - The string identifer of the tank
*  mx  - Tank map X position
*  my  - Tank map Y position
*  px  - Tank pixel offset
*  py  - Tank pixel offset
*********************************************************/
void drawTankLabel(char *str, int mx, int my, BYTE px, BYTE py) {
  HDC hDC = NULL; /* Temp DC of the surface */
  int len;        /* Length of the string */
  RECT textRect;  /* Defines the text rectangle */
  int x;          /* X And Y Locations on the back buffer to do the drawing */
  int y;
  BYTE zf;
  int count1;
  int count2;
  int width = 5;

  zf = windowGetZoomFactor();
  textRect.left = 0;
  textRect.right = zf * TANK_LABEL_WIDTH;
  textRect.top = 0;
  textRect.bottom = zf * TANK_LABEL_HEIGHT;
  len = strlen(str);

  if (len > 0) {
    if (SUCCEEDED(lpDDSTankLabels->lpVtbl->GetDC(lpDDSTankLabels, &hDC))) {
      /* Draw it on the back buffer */
      fontSelect(hDC);
      SetBkColor(hDC, RGB(0,255,0));
      SetTextColor(hDC, RGB(255, 255, 255));
      DrawText(hDC, str, len, &textRect, (DT_LEFT | DT_NOCLIP | DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE));
      DrawText(hDC, str, len, &textRect, (DT_LEFT | DT_NOCLIP | DT_NOPREFIX | DT_SINGLELINE));
      if (FAILED(lpDDSTankLabels->lpVtbl->ReleaseDC(lpDDSTankLabels, hDC))) {
        MessageBox(NULL, "Release DC Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      }
      x = (mx+1) * zf * TILE_SIZE_X + zf * (px+1);
      y = my * zf * TILE_SIZE_Y + zf * py;
      textRect.left = 0;
      /* Fix displaying off the edge of the screen */ 
      if ((x + textRect.right) > zf * screenGetSizeX() * TILE_SIZE_X) {
        textRect.right = zf * screenGetSizeX() * TILE_SIZE_X - x;
      }
      if ((y + textRect.bottom) > (screenGetSizeY() * (zf * TILE_SIZE_Y))) {
         textRect.bottom = zf * ((screenGetSizeY() * TILE_SIZE_Y) - y);
      }
      /* Output it */
      lpDDSBackBuffer->lpVtbl->BltFast(lpDDSBackBuffer, x, y, lpDDSTankLabels, &textRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
      /* Mark Redraw for the squares */
      count1 =0;
	  if (len > 10) {
		  width = 16;
	  }

      while (count1 < 2) {
        count2 = 0;
        while (count2 < width) {
          drawMarkRedraw(mx+count2, my+count1, px, py, 10);
          count2++;
        }
        count1++;
      }
    }
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


  if (useFullscreen == TRUE) {
    return;
  }

  zoomFactor = windowGetZoomFactor();
  res = lpDDSPrimary->lpVtbl->Restore(lpDDSPrimary);
  /* Create the primary surface */
  if (SUCCEEDED(res)) {
    lpDDSTiles->lpVtbl->Restore(lpDDSTiles);
    /* Back Buffer */
    /* Copy the bitmap into it */
    hTiles = (HBITMAP) LoadImage(windowGetInstance(), MAKEINTRESOURCE(IDB_TILE), IMAGE_BITMAP, TILE_FILE_X , TILE_FILE_Y, LR_CREATEDIBSECTION);
    if (hTiles !=NULL) {
      hTilesDC = CreateCompatibleDC(NULL);
      SelectObject(hTilesDC, hTiles);
      lpDDSTiles->lpVtbl->Restore(lpDDSTiles);
      res = lpDDSTiles->lpVtbl->GetDC(lpDDSTiles,&hDDSTilesDC);
      if (res != DD_OK) {
        MessageBox(NULL, "Get DC Failed", DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      }
      StretchBlt(hDDSTilesDC, 0, 0, zoomFactor * TILE_FILE_X, zoomFactor * TILE_FILE_Y, hTilesDC, 0, 0, TILE_FILE_X, TILE_FILE_Y, SRCCOPY);
      res = lpDDSTiles->lpVtbl->ReleaseDC(lpDDSTiles, hDDSTilesDC);
      DeleteDC(hTilesDC);
      DeleteObject(hTiles);
    }
    
    /* Need a redraw */
  }
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
  drawPosX[CRATER] = zoomFactor * CRATER_X;
  drawPosY[CRATER] = zoomFactor * CRATER_Y;


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

void drawEnumarateDisplays(HWND hItem) {
  lpDD->lpVtbl->EnumDisplayModes(lpDD, 0, NULL, (LPVOID) hItem, (LPDDENUMMODESCALLBACK) dialogFullScreenEnumCallback);
}
