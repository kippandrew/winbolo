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
 

/* The size of the main window EXCLUDING Menus and Toolbar */
#define SCREEN_SIZE_X 515
#define SCREEN_SIZE_Y 325
#define MESSAGE_STRING_SIZE 68

#include <X11/keysym.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <X11/xpm.h>
#include <gdk/gdkx.h>
#include <math.h>
#include "SDL.h" 
#include "SDL_ttf.h"
#include <string.h>
#include "../../bolo/global.h"
#include "../../bolo/backend.h"
#include "../../bolo/network.h"
#include "../../bolo/screenlgm.h"
#include "../tiles.h"
#include "../../bolo/tilenum.h"
#include "../../gui/positions.h"
#include "../../gui/linresource.h"
#include "../../gui/lang.h"
#include "draw.h"
#include "messagebox.h"

#include "tiles.xph"
#include "background.xph"

/* #include "..\winbolo.h"
#include "..\cursor.h"
#include "..\font.h"
#include "..\clientmutex.h"
#include "..\lang.h"
#include "..\draw.h"
*/
/* SDL Surfaces */
SDL_Surface *lpScreen = NULL;
SDL_Surface *lpBackBuffer = NULL;
SDL_Surface *lpTiles = NULL;
SDL_Surface *lpPillsStatus = NULL;
SDL_Surface *lpBasesStatus = NULL;
SDL_Surface *lpTankStatus = NULL;
TTF_Font *lpFont = NULL;
SDL_Color white = {0xFF, 0xFF, 0xFF, 0};
SDL_Color green = {0, 0xFF, 0, 0};
SDL_Color black = {0,0,0,0};

typedef int DWORD;
/* Used for drawing the man status */
/* FIXME HBRUSH hManBrush = NULL;
HBRUSH hNarrowBrush = NULL;
HPEN hManPen = NULL;
*/
/* Used for storing time */
DWORD	g_dwFrameTime = 0;
/* Number of frames dispayed this second */
DWORD	g_dwFrameCount = 0;
/* The total frames per second for last second */
DWORD g_dwFrameTotal = 0;


extern GtkWidget* drawingarea1;

int drawPosX[255];
int drawPosY[255];

void drawSetupArrays(BYTE zoomFactor);
void drawDownloadScreen(bool justBlack);

int drawGetFrameRate() {
  return g_dwFrameTotal;
}

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
bool drawSetup(GtkWidget *appWnd) {
  bool returnValue;       /* Value to return */
  BYTE zoomFactor;        /* scaling factor */
  int ret;            /* Direct Draw Function returns */
  SDL_Rect in;               /* Used for copying the bases & pills icon in */
  SDL_Rect out;              /* Used for copying the bases & pills icon in */
  SDL_Rect fill;
  SDL_Surface *lpTemp;       /* Used temporarily before calling SDL_DisplayFormat() */
  BYTE *buff;
  char fileName[FILENAME_MAX];
  char *tf;
  FILE *fp;
  
  buff = malloc(80438);
  /* Get tmp file */
  strcpy(fileName, g_get_tmp_dir());
  tf = tempnam(fileName, "lb");
  strcpy(fileName, tf);
  ret = lzwdecoding(TILES_IMAGE, buff, 36499);
  if (ret != 80438) {
    free(buff);
    MessageBox("Can't load graphics file", DIALOG_BOX_TITLE);
    return FALSE;
  }

  returnValue = TRUE;
  zoomFactor = 1; //FIXME: windowGetZoomFactor();
  //lpScreen = SDL_SetVideoMode(SCREEN_SIZE_X, SCREEN_SIZE_Y , 0, 0);
  lpScreen = SDL_SetVideoMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, 0);
  if (lpScreen == NULL) {
    returnValue = FALSE;
    MessageBox("Can't build main surface", DIALOG_BOX_TITLE);
  }
  

  /* Create the back buffer surface */
  if (returnValue == TRUE) {
    lpTemp = SDL_CreateRGBSurface(SDL_HWSURFACE, zoomFactor * MAIN_BACK_BUFFER_SIZE_X * TILE_SIZE_X, zoomFactor * MAIN_BACK_BUFFER_SIZE_Y * TILE_SIZE_Y, 16, 0, 0, 0, 0);
    if (lpTemp == NULL) {
      returnValue = FALSE;
      MessageBox("Can't build a back buffer", DIALOG_BOX_TITLE);
    } else {
      lpBackBuffer = SDL_DisplayFormat(lpTemp);
      SDL_FreeSurface(lpTemp);
      if (lpBackBuffer == NULL) {
        returnValue = FALSE;
	MessageBox("Can't build a back buffer", DIALOG_BOX_TITLE);
      }
    }
  }

  /* Create the tile buffer and copy the bitmap into it */
  if (returnValue == TRUE) {
    /* Create the buffer */
    fp = fopen(fileName, "wb");
    fwrite(buff, 80438, 1, fp);
    fclose(fp);
    lpTiles = SDL_LoadBMP(fileName);
    unlink(fileName);
    if (lpTiles == NULL) {
      returnValue = FALSE;
      MessageBox("Can't load graphics file", DIALOG_BOX_TITLE);
    } else {
      /* Colour key */
      ret = SDL_SetColorKey(lpTiles, SDL_SRCCOLORKEY, SDL_MapRGB(lpTiles->format, 0, 0xFF, 0));
      if (ret == -1) {
        MessageBox("Couldn't map colour key", DIALOG_BOX_TITLE);
	returnValue = FALSE; 
      } else {
  //      lpTiles = SDL_DisplayFormat(lpTemp);
//	SDL_FreeSurface(lpTemp);
	if (lpTiles == NULL) {
          returnValue = FALSE;
	  MessageBox("Can't build a tile file", DIALOG_BOX_TITLE);
	}
      }
    }
  }
  
  out.w = zoomFactor * TILE_SIZE_X;
  out.h = zoomFactor * TILE_SIZE_Y;
  in.w = zoomFactor * TILE_SIZE_X;
  in.h = zoomFactor * TILE_SIZE_Y;
  
  /* Create the Base status window */
  if (returnValue == TRUE) {
    lpTemp = SDL_CreateRGBSurface(0, zoomFactor * STATUS_BASES_WIDTH, zoomFactor * STATUS_BASES_HEIGHT, 16, 0, 0, 0, 0);
     if (lpTemp == NULL) {
       returnValue = FALSE;
       MessageBox("Can't build a status base buffer", DIALOG_BOX_TITLE);
     } else {
       /* Fill the surface black */
       lpBasesStatus = SDL_DisplayFormat(lpTemp);
       SDL_FreeSurface(lpTemp);
       if (lpBasesStatus == NULL) {
         returnValue = FALSE;
         MessageBox("Can't build a status base buffer", DIALOG_BOX_TITLE);
       } else {
         fill.x = 0;
         fill.y = 0;
         fill.w = lpBasesStatus->w;
         fill.h = lpBasesStatus->h;
         SDL_FillRect(lpBasesStatus, &fill, SDL_MapRGB(lpBasesStatus->format, 0, 0, 0));
         /* Copy in the icon */
         in.x = zoomFactor * BASE_GOOD_X;
         in.y = zoomFactor * BASE_GOOD_Y;
         out.x = zoomFactor * STATUS_BASES_MIDDLE_ICON_X;
         out.y = zoomFactor * STATUS_BASES_MIDDLE_ICON_Y;
         SDL_BlitSurface(lpTiles, &in, lpBasesStatus, &out);
       }
     } 
  }
  /* Makes the pills status */
  if (returnValue == TRUE) {
    lpTemp = SDL_CreateRGBSurface(0, zoomFactor * STATUS_PILLS_WIDTH, zoomFactor * STATUS_PILLS_HEIGHT, 16, 0, 0, 0, 0);
     if (lpTemp == NULL) {
       returnValue = FALSE;
       MessageBox("Can't build a status pills buffer", DIALOG_BOX_TITLE);
     } else {
       lpPillsStatus = SDL_DisplayFormat(lpTemp);
       SDL_FreeSurface(lpTemp);
       if (lpTemp == FALSE) {
         returnValue = FALSE;
         MessageBox("Can't build a status pills buffer", DIALOG_BOX_TITLE);
       } else {	      
         /* Fill the surface black */
         fill.x = 0;
         fill.y = 0;
         fill.w = lpPillsStatus->w;
         fill.h = lpPillsStatus->h;
         SDL_FillRect(lpPillsStatus, &fill, SDL_MapRGB(lpPillsStatus ->format, 0, 0, 0));
         /* Copy in the icon */
         in.x = zoomFactor * PILL_GOOD15_X;
         in.y = zoomFactor * PILL_GOOD15_Y;
         out.x = zoomFactor * STATUS_PILLS_MIDDLE_ICON_X;
         out.y = zoomFactor * STATUS_PILLS_MIDDLE_ICON_Y;
         SDL_BlitSurface(lpTiles, &in, lpPillsStatus, &out);
       } 
     }
  }
 
   /* Makes the tanks status */
  if (returnValue == TRUE) {
    lpTemp = SDL_CreateRGBSurface(0, zoomFactor * STATUS_TANKS_WIDTH, zoomFactor * STATUS_TANKS_HEIGHT, 16, 0, 0, 0, 0);
     if (lpTemp == NULL) {
       returnValue = FALSE;
       MessageBox("Can't build a status tanks buffer", DIALOG_BOX_TITLE);
     } else {
       lpTankStatus = SDL_DisplayFormat(lpTemp);
       SDL_FreeSurface(lpTemp);
       if (lpTankStatus == NULL) {
         returnValue = FALSE;
         MessageBox("Can't build a status tanks buffer", DIALOG_BOX_TITLE);
       } else {
         /* Fill the surface black */
         fill.x = 0;
         fill.y = 0;
         fill.w = lpTankStatus->w;
         fill.h = lpTankStatus->h;
         SDL_FillRect(lpTankStatus, &fill, SDL_MapRGB(lpTankStatus ->format, 0, 0, 0));
         /* Copy in the icon */
         in.x = zoomFactor * TANK_SELF_0_X;
         in.y = zoomFactor * TANK_SELF_0_Y;
         out.x = zoomFactor * STATUS_TANKS_MIDDLE_ICON_X;
         out.y = zoomFactor * STATUS_TANKS_MIDDLE_ICON_Y;
         SDL_BlitSurface(lpTiles, &in, lpTankStatus, &out);
       } 
    }
  }
  if (returnValue == TRUE) {
    if (TTF_Init() < 0) {
      MessageBox("Couldn't init TTF rasteriser", DIALOG_BOX_TITLE);
      returnValue = FALSE;
    } else {
      lpFont = TTF_OpenFont("cour.ttf", 12);
      if (lpFont == NULL) {
        MessageBox("Couldn't open font file.\n Please place a courier font\ncalled \"cour.ttf\" in your\nLinBolo directory.", DIALOG_BOX_TITLE);
	returnValue = FALSE;
      }
    }
  }


  g_dwFrameTime = SDL_GetTicks();
  g_dwFrameCount = 0;
  drawSetupArrays(zoomFactor);

  free(buff);
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
  if (lpTiles != NULL) {
    SDL_FreeSurface(lpTiles);
    lpTiles = NULL;
  }
  if (lpBackBuffer != NULL) {
    SDL_FreeSurface(lpBackBuffer);
    lpBackBuffer = NULL;
  }
  if (lpBasesStatus != NULL) {
    SDL_FreeSurface(lpBasesStatus);
    lpBasesStatus = NULL;
  }
  if (lpPillsStatus != NULL) {
    SDL_FreeSurface(lpPillsStatus);
    lpPillsStatus = NULL;
  }
  if (lpTankStatus != NULL) {
    SDL_FreeSurface(lpTankStatus);
    lpTankStatus = NULL;
  }
  if (lpFont != NULL) {
    TTF_CloseFont(lpFont);
    TTF_Quit();
  }
  if (lpScreen != NULL) {
    SDL_FreeSurface(lpScreen);
    lpScreen = NULL;
  }

}

int lastManX = 0;
int lastManY = 0;

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
void drawSetManClear() {
  int zoomFactor, left, top, width, height;

//  jm removed today gdk_threads_enter(); 
  zoomFactor = 1; //FIXME
  left = zoomFactor * MAN_STATUS_X;
  top = (zoomFactor * MAN_STATUS_Y);
  width = zoomFactor * MAN_STATUS_WIDTH + 5;
  height =  zoomFactor * MAN_STATUS_HEIGHT + 5;
  gdk_draw_rectangle (drawingarea1->window, drawingarea1->style->black_gc, TRUE, left, top, width, height);
  lastManX = 0;
  lastManY = 0;
//  gdk_threads_leave(); 
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
void drawSetManStatus(bool isDead, TURNTYPE angle, bool needLocking) {
  TURNTYPE oldAngle; /* Copy of the angle parameter */
  double dbAngle;    /* Angle in radians */
  double dbTemp;
  BYTE zoomFactor;   /* Zooming factor */
  int addX;          /* X And and Y co-ordinates */
  int addY;
  int midx, midy;
  int left, top;
  int width, height;
  zoomFactor = 1; //FIXME windowGetZoomFactor();

  
  //drawSetManClear(menuHeight); /* Clear the display */

  oldAngle = angle;
  angle += BRADIANS_SOUTH;
  if (angle >= BRADIANS_MAX) {
    angle -= BRADIANS_MAX;
  }

  if (angle >= BRADIANS_NORTH && angle < BRADIANS_EAST) {
    /* Convert bradians to degrees */
    dbAngle = (DEGREES_MAX / BRADIANS_MAX) * angle;
    /* Convert degrees to radians */
    dbAngle = (dbAngle / DEGREES_MAX) * RADIAN_MAX;

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
    dbAngle = (dbAngle / DEGREES_MAX) * RADIAN_MAX;

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
    dbAngle = (dbAngle / DEGREES_MAX) * RADIAN_MAX;

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
    dbAngle = (dbAngle / DEGREES_MAX) * RADIAN_MAX;

    addX = (zoomFactor * MAN_STATUS_CENTER_X);
    addY = (zoomFactor * MAN_STATUS_CENTER_Y);
    dbTemp = MAN_STATUS_RADIUS * (sin(dbAngle));
    addX -= (int) (zoomFactor * dbTemp);
    dbTemp = MAN_STATUS_RADIUS * (cos(dbAngle));
    addY -= (int) (zoomFactor * dbTemp);
  }
  
 
  if (needLocking == TRUE) { 
    gdk_threads_enter(); 
  }
  left = zoomFactor * MAN_STATUS_X;
  top = (zoomFactor * MAN_STATUS_Y);
  width = zoomFactor * MAN_STATUS_WIDTH;
  height =  zoomFactor * MAN_STATUS_HEIGHT;
  if (lastManX != 0) {
    gdk_draw_line (drawingarea1->window, drawingarea1->style->black_gc, zoomFactor * MAN_STATUS_CENTER_X + left , top + zoomFactor * MAN_STATUS_CENTER_Y, lastManX, lastManY);
  } else {
     gdk_draw_rectangle (drawingarea1->window, drawingarea1->style->black_gc, TRUE, left, top, width, height);
  }
  addY += top;
  addX += left;
  if (isDead == TRUE) {
     /* Draw dead circle */
     gdk_draw_arc(drawingarea1->window, drawingarea1->style->white_gc, TRUE, left, top, width, height, 0, 360 * 64);
     lastManX = 0;
  } else {
    gdk_draw_arc(drawingarea1->window, drawingarea1->style->white_gc, FALSE, left, top, width, height, 0, 360 * 64);
   gdk_draw_line(drawingarea1->window, drawingarea1->style->white_gc, zoomFactor * MAN_STATUS_CENTER_X + left , top + zoomFactor * MAN_STATUS_CENTER_Y, addX, addY);

    lastManX = addX;
    lastManY = addY;
  }
  if (needLocking == TRUE) {
    gdk_threads_leave(); 
  }
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
  SDL_Rect output; /* Output Rectangle */
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
  SDL_Rect dest;

  total = screenBulletsGetNumEntries(sBullets);
  count = 1;
  zf = 1; //FIXME: windowGetZoomFactor();
  while (count <= total) {
    screenBulletsGetItem(sBullets, count, &mx, &my,&px,&py, &frame);
    dest.x = (mx * zf * TILE_SIZE_X) + zf * px;
    dest.y = (my * zf * TILE_SIZE_Y) + zf * py;
    switch (frame) {
    case SHELL_EXPLOSION8:
      output.x = zf * EXPLOSION8_X;
      output.y = zf * EXPLOSION8_Y;
      output.w = zf * TILE_SIZE_X;
      output.h = zf * TILE_SIZE_Y;
      break;
   case SHELL_EXPLOSION7:
      output.x = zf * EXPLOSION7_X;
      output.y = zf * EXPLOSION7_Y;
      output.w = zf * TILE_SIZE_X;
      output.h = zf * TILE_SIZE_Y;
      break;
   case SHELL_EXPLOSION6:
     output.x = zf * EXPLOSION6_X;
     output.y = zf * EXPLOSION6_Y;
     output.w = zf * TILE_SIZE_X;
     output.h = zf * TILE_SIZE_Y;
     break;
   case SHELL_EXPLOSION5:
     output.x = zf * EXPLOSION5_X;
     output.y = zf * EXPLOSION5_Y;
     output.w = zf * TILE_SIZE_X;
     output.h = zf * TILE_SIZE_Y;
     break;
   case SHELL_EXPLOSION4:
     output.x = zf * EXPLOSION4_X;
     output.y = zf * EXPLOSION4_Y;
     output.w = zf * TILE_SIZE_X;
     output.h = zf * TILE_SIZE_Y;
     break;
   case SHELL_EXPLOSION3:
     output.x = zf * EXPLOSION3_X;
     output.y = zf * EXPLOSION3_Y;
     output.w = zf * TILE_SIZE_X;
     output.h = zf * TILE_SIZE_Y;
     break;
   case SHELL_EXPLOSION2:
     output.x = zf * EXPLOSION2_X;
     output.y = zf * EXPLOSION2_Y;
     output.w = zf * TILE_SIZE_X;
     output.h = zf * TILE_SIZE_Y;
     break; 
    case SHELL_EXPLOSION1:
      output.x = zf * EXPLOSION1_X;
      output.y = zf * EXPLOSION1_Y;
      output.w = zf * TILE_SIZE_X;
      output.h = zf * TILE_SIZE_Y;
      break;
    case SHELL_DIR0:
      output.x = zf * SHELL_0_X;
      output.y = zf * SHELL_0_Y;
      output.w = zf * SHELL_0_WIDTH;
      output.h = zf * SHELL_0_HEIGHT;
      break;
    case SHELL_DIR1:
      output.x = zf * SHELL_1_X;
      output.y = zf * SHELL_1_Y;
      output.w = zf * SHELL_1_WIDTH;
      output.h = zf * SHELL_1_HEIGHT;
      break;
    case SHELL_DIR2:
      output.x = zf * SHELL_2_X;
      output.y = zf * SHELL_2_Y;
      output.w = zf * SHELL_2_WIDTH;
      output.h = zf * SHELL_2_HEIGHT;
      break;
    case SHELL_DIR3:
      output.x = zf * SHELL_3_X;
      output.y = zf * SHELL_3_Y;
      output.w = zf * SHELL_3_WIDTH;
      output.h = zf * SHELL_3_HEIGHT;
      break;
    case SHELL_DIR4:
      output.x = zf * SHELL_4_X;
      output.y = zf * SHELL_4_Y;
      output.w = zf * SHELL_4_WIDTH;
      output.h = zf * SHELL_4_HEIGHT;
      break;
    case SHELL_DIR5:
      output.x = zf * SHELL_5_X;
      output.y = zf * SHELL_5_Y;
      output.w = zf * SHELL_5_WIDTH;
      output.h = zf * SHELL_5_HEIGHT;
      break;
    case SHELL_DIR6:
      output.x = zf * SHELL_6_X;
      output.y = zf * SHELL_6_Y;
      output.w = zf * SHELL_6_WIDTH;
      output.h = zf * SHELL_6_HEIGHT;
      break;
    case SHELL_DIR7:
      output.x = zf * SHELL_7_X;
      output.y = zf * SHELL_7_Y;
      output.w = zf * SHELL_7_WIDTH;
      output.h = zf * SHELL_7_HEIGHT;
      break;
    case SHELL_DIR8:
      output.x = zf * SHELL_8_X;
      output.y = zf * SHELL_8_Y;
      output.w = zf * SHELL_8_WIDTH;
      output.h = zf * SHELL_8_HEIGHT;
      break;
    case SHELL_DIR9:
      output.x = zf * SHELL_9_X;
      output.y = zf * SHELL_9_Y;
      output.w = zf * SHELL_9_WIDTH;
      output.h = zf * SHELL_9_HEIGHT;
      break;
    case SHELL_DIR10:
      output.x = zf * SHELL_10_X;
      output.y = zf * SHELL_10_Y;
      output.w = zf * SHELL_10_WIDTH;
      output.h = zf * SHELL_10_HEIGHT;
      break;
    case SHELL_DIR11:
      output.x = zf * SHELL_11_X;
      output.y = zf * SHELL_11_Y;
      output.w = zf * SHELL_11_WIDTH;
      output.h = zf * SHELL_11_HEIGHT;
      break;
    case SHELL_DIR12:
      output.x = zf * SHELL_12_X;
      output.y = zf * SHELL_12_Y;
      output.w = zf * SHELL_12_WIDTH;
      output.h = zf * SHELL_12_HEIGHT;
      break;
    case SHELL_DIR13:
      output.x = zf * SHELL_13_X;
      output.y = zf * SHELL_13_Y;
      output.w = zf * SHELL_13_WIDTH;
      output.h = zf * SHELL_13_HEIGHT;
      break;
    case SHELL_DIR14:
      output.x = zf * SHELL_14_X;
      output.y = zf * SHELL_14_Y;
      output.w = zf * SHELL_14_WIDTH;
      output.h = zf * SHELL_14_HEIGHT;
      break;
  default:
    /* SHELL_DIR15 */
      output.x = zf * SHELL_15_X;
      output.y = zf * SHELL_15_Y;
      output.w = zf * SHELL_15_WIDTH;
      output.h = zf * SHELL_15_HEIGHT;
    break;

    }
    count++;
    dest.w = output.w;
    dest.h = output.h;
    SDL_BlitSurface(lpTiles, &output, lpBackBuffer, &dest);
  }
}

/*********************************************************
* *NAME:          drawTankLabel
* *AUTHOR:        John Morrison
*
*CREATION DATE: 2/2/98
*LAST MODIFIED: 2/2/98
*PURPOSE:
Draws the tank label if required.
*
*ARGUMENTS:
*  str - The string identifer of the tank
*  mx  - Tank map X position
*  my  - Tank map Y position
*  px  - Tank pixel offset
*  py  - Tank pixel offset
*********************************************************/
void drawTankLabel(char *str, int mx, int my, BYTE px, BYTE py) {
  int len;        /* Length of the string */
  SDL_Rect dest;  /* Defines the text rectangle */
  int x;          /* X And Y Locations on the back buffer to do the drawing */
  int y;
  BYTE zf;
  SDL_Surface *lpTextSurface;
   

  zf = 1; //FIXME: windowGetZoomFactor();
/*  textRect.left = 0;
  textRect.right = zf * TANK_LABEL_WIDTH;
  textRect.top = 0;
  textRect.bottom = zf * TANK_LABEL_HEIGHT; */
  len = strlen(str);

  if (len > 0) {
    /* Draw it on the back buffer */
    lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
    if (lpTextSurface) {
      dest.x = ((mx +1) * zf * TILE_SIZE_X) + zf * (px + 1);
      dest.y = my * zf * TILE_SIZE_Y + zf * py;
      dest.w = lpTextSurface->w;
      dest.h = lpTextSurface->h;
/* Fix displaying off the edge of the screen */
/* FIXME: Obsolete if ((x + textRect.right) > (MAIN_BACK_BUFFER_SIZE_X * (zf * TILE_SIZE_X))) {
textRect.right = zf * ((MAIN_BACK_BUFFER_SIZE_X * TILE_SIZE_X) - x);
}
if ((y + textRect.bottom) > (MAIN_BACK_BUFFER_SIZE_Y * (zf * TILE_SIZE_Y))) {
textRect.bottom = zf * ((MAIN_BACK_BUFFER_SIZE_Y * TILE_SIZE_Y) - y);
} */
      /* Make it transparent */
      SDL_SetColorKey(lpTextSurface, SDL_SRCCOLORKEY, SDL_MapRGB(lpTextSurface->format, 0, 0, 0));
      /* Output it */
      SDL_BlitSurface(lpTextSurface, NULL, lpBackBuffer, &dest);
      SDL_UpdateRects(lpBackBuffer, 1, &dest);
      SDL_FreeSurface(lpTextSurface);
    }
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
  SDL_Rect output; /* Source Rectangle */
  SDL_Rect dest;
  BYTE count;  /* Looping Variable */
  BYTE total;  /* Total number of tanks on the screen */
  /* Current tank Stuff */
  char playerName[PLAYER_NAME_LEN]; /* Player name */
  BYTE frame;                       /* Frame id */
  BYTE mx;                          /* Map X and Y Co-ords */
  BYTE my;
  BYTE px;                          /* Pixel offsets */
  BYTE py;
  BYTE zoomFactor;                  /* Scaling Factor */
  BYTE playerNum;                   /* Player Number */

  count = 1;
  total = screenTanksGetNumEntries(tks);
  zoomFactor = 1; //FIXME: windowGetZoomFactor();
  output.w = zoomFactor * TILE_SIZE_X;
  output.h = zoomFactor * TILE_SIZE_Y;
  dest.w = output.w;
  dest.h = output.h;
  while (count <= total) {
    screenTanksGetItem(tks, count, &mx, &my, &px, &py, &frame, &playerNum, playerName);
    switch (frame) {
    case TANK_SELF_0:
      output.x = TANK_SELF_0_X;
      output.y = TANK_SELF_0_Y;
      break;
    case TANK_SELF_1:
      output.x = TANK_SELF_1_X;
      output.y = TANK_SELF_1_Y;
      break;
    case TANK_SELF_2:
      output.x = TANK_SELF_2_X;
      output.y = TANK_SELF_2_Y;
      break;
    case TANK_SELF_3:
      output.x = TANK_SELF_3_X;
      output.y = TANK_SELF_3_Y;
      break;
    case TANK_SELF_4:
      output.x = TANK_SELF_4_X;
      output.y = TANK_SELF_4_Y;
      break;
    case TANK_SELF_5:
      output.x = TANK_SELF_5_X;
      output.y = TANK_SELF_5_Y;
      break;
    case TANK_SELF_6:
      output.x = TANK_SELF_6_X;
      output.y = TANK_SELF_6_Y;
      break;
    case TANK_SELF_7:
      output.x = TANK_SELF_7_X;
      output.y = TANK_SELF_7_Y;
      break;
    case TANK_SELF_8:
      output.x = TANK_SELF_8_X;
      output.y = TANK_SELF_8_Y;
      break;
    case TANK_SELF_9:
      output.x = TANK_SELF_9_X;
      output.y = TANK_SELF_9_Y;
      break;
    case TANK_SELF_10:
      output.x = TANK_SELF_10_X;
      output.y = TANK_SELF_10_Y;
      break;
    case TANK_SELF_11:
      output.x = TANK_SELF_11_X;
      output.y = TANK_SELF_11_Y;
      break;
    case TANK_SELF_12:
      output.x = TANK_SELF_12_X;
      output.y = TANK_SELF_12_Y;
      break;
    case TANK_SELF_13:
      output.x = TANK_SELF_13_X;
      output.y = TANK_SELF_13_Y;
      break;
    case TANK_SELF_14:
      output.x = TANK_SELF_14_X;
      output.y = TANK_SELF_14_Y;
      break;
    case TANK_SELF_15:
      output.x = TANK_SELF_15_X;
      output.y = TANK_SELF_15_Y;
      break;
    case TANK_SELFBOAT_0:
      output.x = TANK_SELFBOAT_0_X;
      output.y = TANK_SELFBOAT_0_Y;
      break;
    case TANK_SELFBOAT_1:
      output.x = TANK_SELFBOAT_1_X;
      output.y = TANK_SELFBOAT_1_Y;
      break;
    case TANK_SELFBOAT_2:
      output.x = TANK_SELFBOAT_2_X;
      output.y = TANK_SELFBOAT_2_Y;
      break;
    case TANK_SELFBOAT_3:
      output.x = TANK_SELFBOAT_3_X;
      output.y = TANK_SELFBOAT_3_Y;
      break;
    case TANK_SELFBOAT_4:
      output.x = TANK_SELFBOAT_4_X;
      output.y = TANK_SELFBOAT_4_Y;
      break;
    case TANK_SELFBOAT_5:
      output.x = TANK_SELFBOAT_5_X;
      output.y = TANK_SELFBOAT_5_Y;
      break;
    case TANK_SELFBOAT_6:
      output.x = TANK_SELFBOAT_6_X;
      output.y = TANK_SELFBOAT_6_Y;
      break;
    case TANK_SELFBOAT_7:
      output.x = TANK_SELFBOAT_7_X;
      output.y = TANK_SELFBOAT_7_Y;
      break;
    case TANK_SELFBOAT_8:
      output.x = TANK_SELFBOAT_8_X;
      output.y = TANK_SELFBOAT_8_Y;
      break;
    case TANK_SELFBOAT_9:
      output.x = TANK_SELFBOAT_9_X;
      output.y = TANK_SELFBOAT_9_Y;
      break;
    case TANK_SELFBOAT_10:
      output.x = TANK_SELFBOAT_10_X;
      output.y = TANK_SELFBOAT_10_Y;
      break;
    case TANK_SELFBOAT_11:
      output.x = TANK_SELFBOAT_11_X;
      output.y = TANK_SELFBOAT_11_Y;
      break;
    case TANK_SELFBOAT_12:
      output.x = TANK_SELFBOAT_12_X;
      output.y = TANK_SELFBOAT_12_Y;
      break;
    case TANK_SELFBOAT_13:
      output.x = TANK_SELFBOAT_13_X;
      output.y = TANK_SELFBOAT_13_Y;
      break;
    case TANK_SELFBOAT_14:
      output.x = TANK_SELFBOAT_14_X;
      output.y = TANK_SELFBOAT_14_Y;
      break;
    case TANK_SELFBOAT_15:
      output.x = TANK_SELFBOAT_15_X;
      output.y = TANK_SELFBOAT_15_Y;
      break;
    case TANK_GOOD_0:
      output.x = TANK_GOOD_0_X;
      output.y = TANK_GOOD_0_Y;
      break;
    case TANK_GOOD_1:
      output.x = TANK_GOOD_1_X;
      output.y = TANK_GOOD_1_Y;
      break;
    case TANK_GOOD_2:
      output.x = TANK_GOOD_2_X;
      output.y = TANK_GOOD_2_Y;
      break;
    case TANK_GOOD_3:
      output.x = TANK_GOOD_3_X;
      output.y = TANK_GOOD_3_Y;
      break;
    case TANK_GOOD_4:
      output.x = TANK_GOOD_4_X;
      output.y = TANK_GOOD_4_Y;
      break;
    case TANK_GOOD_5:
      output.x = TANK_GOOD_5_X;
      output.y = TANK_GOOD_5_Y;
      break;
    case TANK_GOOD_6:
      output.x = TANK_GOOD_6_X;
      output.y = TANK_GOOD_6_Y;
      break;
    case TANK_GOOD_7:
      output.x = TANK_GOOD_7_X;
      output.y = TANK_GOOD_7_Y;
      break;
    case TANK_GOOD_8:
      output.x = TANK_GOOD_8_X;
      output.y = TANK_GOOD_8_Y;
      break;
    case TANK_GOOD_9:
      output.x = TANK_GOOD_9_X;
      output.y = TANK_GOOD_9_Y;
      break;
    case TANK_GOOD_10:
      output.x = TANK_GOOD_10_X;
      output.y = TANK_GOOD_10_Y;
      break;
    case TANK_GOOD_11:
      output.x = TANK_GOOD_11_X;
      output.y = TANK_GOOD_11_Y;
      break;
    case TANK_GOOD_12:
      output.x = TANK_GOOD_12_X;
      output.y = TANK_GOOD_12_Y;
      break;
    case TANK_GOOD_13:
      output.x = TANK_GOOD_13_X;
      output.y = TANK_GOOD_13_Y;
      break;
    case TANK_GOOD_14:
      output.x = TANK_GOOD_14_X;
      output.y = TANK_GOOD_14_Y;
      break;
    case TANK_GOOD_15:
      output.x = TANK_GOOD_15_X;
      output.y = TANK_GOOD_15_Y;
      break;
    case TANK_GOODBOAT_0:
      output.x = TANK_GOODBOAT_0_X;
      output.y = TANK_GOODBOAT_0_Y;
      break;
    case TANK_GOODBOAT_1:
      output.x = TANK_GOODBOAT_1_X;
      output.y = TANK_GOODBOAT_1_Y;
      break;
    case TANK_GOODBOAT_2:
      output.x = TANK_GOODBOAT_2_X;
      output.y = TANK_GOODBOAT_2_Y;
      break;
    case TANK_GOODBOAT_3:
      output.x = TANK_GOODBOAT_3_X;
      output.y = TANK_GOODBOAT_3_Y;
      break;
    case TANK_GOODBOAT_4:
      output.x = TANK_GOODBOAT_4_X;
      output.y = TANK_GOODBOAT_4_Y;
      break;
    case TANK_GOODBOAT_5:
      output.x = TANK_GOODBOAT_5_X;
      output.y = TANK_GOODBOAT_5_Y;
      break;
    case TANK_GOODBOAT_6:
      output.x = TANK_GOODBOAT_6_X;
      output.y = TANK_GOODBOAT_6_Y;
      break;
    case TANK_GOODBOAT_7:
      output.x = TANK_GOODBOAT_7_X;
      output.y = TANK_GOODBOAT_7_Y;
      break;
    case TANK_GOODBOAT_8:
      output.x = TANK_GOODBOAT_8_X;
      output.y = TANK_GOODBOAT_8_Y;
      break;
    case TANK_GOODBOAT_9:
      output.x = TANK_GOODBOAT_9_X;
      output.y = TANK_GOODBOAT_9_Y;
      break;
    case TANK_GOODBOAT_10:
      output.x = TANK_GOODBOAT_10_X;
      output.y = TANK_GOODBOAT_10_Y;
      break;
    case TANK_GOODBOAT_11:
      output.x = TANK_GOODBOAT_11_X;
      output.y = TANK_GOODBOAT_11_Y;
      break;
    case TANK_GOODBOAT_12:
      output.x = TANK_GOODBOAT_12_X;
      output.y = TANK_GOODBOAT_12_Y;
      break;
    case TANK_GOODBOAT_13:
      output.x = TANK_GOODBOAT_13_X;
      output.y = TANK_GOODBOAT_13_Y;
      break;
    case TANK_GOODBOAT_14:
      output.x = TANK_GOODBOAT_14_X;
      output.y = TANK_GOODBOAT_14_Y;
      break;
    case TANK_GOODBOAT_15:
      output.x = TANK_GOODBOAT_15_X;
      output.y = TANK_GOODBOAT_15_Y;
      break;
    case TANK_EVIL_0:
      output.x = TANK_EVIL_0_X;
      output.y = TANK_EVIL_0_Y;
      break;
    case TANK_EVIL_1:
      output.x = TANK_EVIL_1_X;
      output.y = TANK_EVIL_1_Y;
      break;
    case TANK_EVIL_2:
      output.x = TANK_EVIL_2_X;
      output.y = TANK_EVIL_2_Y;
      break;
    case TANK_EVIL_3:
      output.x = TANK_EVIL_3_X;
      output.y = TANK_EVIL_3_Y;
      break;
    case TANK_EVIL_4:
      output.x = TANK_EVIL_4_X;
      output.y = TANK_EVIL_4_Y;
      break;
    case TANK_EVIL_5:
      output.x = TANK_EVIL_5_X;
      output.y = TANK_EVIL_5_Y;
      break;
    case TANK_EVIL_6:
      output.x = TANK_EVIL_6_X;
      output.y = TANK_EVIL_6_Y;
      break;
    case TANK_EVIL_7:
      output.x = TANK_EVIL_7_X;
      output.y = TANK_EVIL_7_Y;
      break;
    case TANK_EVIL_8:
      output.x = TANK_EVIL_8_X;
      output.y = TANK_EVIL_8_Y;
      break;
    case TANK_EVIL_9:
      output.x = TANK_EVIL_9_X;
      output.y = TANK_EVIL_9_Y;
      break;
    case TANK_EVIL_10:
      output.x = TANK_EVIL_10_X;
      output.y = TANK_EVIL_10_Y;
      break;
    case TANK_EVIL_11:
      output.x = TANK_EVIL_11_X;
      output.y = TANK_EVIL_11_Y;
      break;
    case TANK_EVIL_12:
      output.x = TANK_EVIL_12_X;
      output.y = TANK_EVIL_12_Y;
      break;
    case TANK_EVIL_13:
      output.x = TANK_EVIL_13_X;
      output.y = TANK_EVIL_13_Y;
      break;
    case TANK_EVIL_14:
      output.x = TANK_EVIL_14_X;
      output.y = TANK_EVIL_14_Y;
      break;
    case TANK_EVIL_15:
      output.x = TANK_EVIL_15_X;
      output.y = TANK_EVIL_15_Y;
      break;
    case TANK_EVILBOAT_0:
      output.x = TANK_EVILBOAT_0_X;
      output.y = TANK_EVILBOAT_0_Y;
      break;
    case TANK_EVILBOAT_1:
      output.x = TANK_EVILBOAT_1_X;
      output.y = TANK_EVILBOAT_1_Y;
      break;
    case TANK_EVILBOAT_2:
      output.x = TANK_EVILBOAT_2_X;
      output.y = TANK_EVILBOAT_2_Y;
      break;
    case TANK_EVILBOAT_3:
      output.x = TANK_EVILBOAT_3_X;
      output.y = TANK_EVILBOAT_3_Y;
      break;
    case TANK_EVILBOAT_4:
      output.x = TANK_EVILBOAT_4_X;
      output.y = TANK_EVILBOAT_4_Y;
      break;
    case TANK_EVILBOAT_5:
      output.x = TANK_EVILBOAT_5_X;
      output.y = TANK_EVILBOAT_5_Y;
      break;
    case TANK_EVILBOAT_6:
      output.x = TANK_EVILBOAT_6_X;
      output.y = TANK_EVILBOAT_6_Y;
      break;
    case TANK_EVILBOAT_7:
      output.x = TANK_EVILBOAT_7_X;
      output.y = TANK_EVILBOAT_7_Y;
      break;
    case TANK_EVILBOAT_8:
      output.x = TANK_EVILBOAT_8_X;
      output.y = TANK_EVILBOAT_8_Y;
      break;
    case TANK_EVILBOAT_9:
      output.x = TANK_EVILBOAT_9_X;
      output.y = TANK_EVILBOAT_9_Y;
      break;
    case TANK_EVILBOAT_10:
      output.x = TANK_EVILBOAT_10_X;
      output.y = TANK_EVILBOAT_10_Y;
      break;
    case TANK_EVILBOAT_11:
      output.x = TANK_EVILBOAT_11_X;
      output.y = TANK_EVILBOAT_11_Y;
      break;
    case TANK_EVILBOAT_12:
      output.x = TANK_EVILBOAT_12_X;
      output.y = TANK_EVILBOAT_12_Y;
      break;
    case TANK_EVILBOAT_13:
      output.x = TANK_EVILBOAT_13_X;
      output.y = TANK_EVILBOAT_13_Y;
      break;
    case TANK_EVILBOAT_14:
      output.x = TANK_EVILBOAT_14_X;
      output.y = TANK_EVILBOAT_14_Y;
      break;
    case TANK_EVILBOAT_15:
      output.x = TANK_EVILBOAT_15_X;
      output.y = TANK_EVILBOAT_15_Y;
      break;

    default:
      /* TANK_TRANSPARENT */
      output.x = TANK_TRANSPARENT_X;
      output.y = TANK_TRANSPARENT_Y;
    }

    /* Output */
    output.x *= zoomFactor;
    output.y *= zoomFactor;
    dest.x = mx * (zoomFactor * TILE_SIZE_X) + (zoomFactor * px);
    dest.y = my * (zoomFactor * TILE_SIZE_Y) + (zoomFactor * py);
    px += 2;
    py += 2;
    SDL_BlitSurface(lpTiles, &output, lpBackBuffer, &dest);
    /* Output the label */
    drawTankLabel(playerName, mx, my, px, py);
    count++;
  }

}




/*********************************************************
*NAME:          drawLgms
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
  SDL_Rect output; /* Source Rectangle */
  SDL_Rect dest;   /* Destination rect */
  BYTE count;  /* Looping variable */
  BYTE zf;     /* Zoom factor */

  total = screenLgmGetNumEntries(lgms);
  zf = 1; //FIXME: windowGetZoomFactor();

  for (count=1;count<=total;count++) {
    screenLgmGetItem(lgms, count, &mx, &my, &px, &py, &frame);
    switch (frame) {
      case LGM0:
        output.x = zf * LGM0_X;
        output.y = zf * LGM0_Y;
        output.w = zf * LGM_WIDTH;
        output.h = zf * LGM_HEIGHT;
        break;
      case LGM1:
        output.x = zf * LGM1_X;
        output.y = zf * LGM1_Y;
        output.w = zf * LGM_WIDTH;
        output.h = zf * LGM_HEIGHT;
        break;
      case LGM2:
        output.x = zf * LGM2_X;
        output.y = zf * LGM2_Y;
        output.w = zf * LGM_WIDTH;
        output.h = zf * LGM_HEIGHT;
        break;
      default:
        /* LGM3 - Helicopter */
        output.x = zf * LGM_HELICOPTER_X;
        output.y = zf * LGM_HELICOPTER_Y;
        output.w = zf * TILE_SIZE_X;
        output.h = zf * TILE_SIZE_Y;
        break;
    }
    dest.x = (zf * mx * TILE_SIZE_X) + (zf * px);
    dest.y = (zf * my * TILE_SIZE_Y) + (zf * py);
    dest.w = output.w;
    dest.h = output.h;
    SDL_BlitSurface(lpTiles, &output, lpBackBuffer, &dest);
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
  SDL_Rect dest;  /* Defines the text rectangle */
  BYTE zoomFactor;
  SDL_Surface *lpTextSurface;

  zoomFactor = 1; //FIXME windowGetZoomFactor();
  dest.x = zoomFactor * 3 * TILE_SIZE_X;
  dest.y = zoomFactor * 8 * TILE_SIZE_Y;
  lpTextSurface = TTF_RenderText_Shaded(lpFont, "Network Failed -  Resyncing", white, black);  
  SDL_SetColorKey(lpTextSurface, SDL_SRCCOLORKEY, SDL_MapRGB(lpTextSurface->format, 0, 0, 0));
  dest.w = lpTextSurface->w;
  dest.h = lpTextSurface->h;
  /* Output it */
  SDL_BlitSurface(lpTextSurface, NULL, lpBackBuffer, &dest);
  SDL_FreeSurface(lpTextSurface);
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
  SDL_Rect dest;  /* Defines the text rectangle */
  BYTE zoomFactor;
  SDL_Surface *lpTextSurface;

  zoomFactor = 1; //FIXME windowGetZoomFactor();
  dest.x = zoomFactor * TILE_SIZE_X;
  dest.y = zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y;
  lpTextSurface = TTF_RenderText_Shaded(lpFont, langGetText(STR_DRAW_PILLBOXVIEW), white, black);  
  SDL_SetColorKey(lpTextSurface, SDL_SRCCOLORKEY, SDL_MapRGB(lpTextSurface->format, 0, 0, 0));
  dest.w = lpTextSurface->w;
  dest.h = lpTextSurface->h;
  /* Output it */
  SDL_BlitSurface(lpTextSurface, NULL, lpBackBuffer, &dest);
  SDL_FreeSurface(lpTextSurface);
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
void drawStartDelay(long srtDelay) {
  SDL_Rect src;                  /* Used for copying the bases & pills icon in */
  BYTE zoomFactor;           /* Scaling Factor */
  char str[FILENAME_MAX];    /* Output String */
  char strNum[FILENAME_MAX]; /* Holds the start delay as a string */
  SDL_Surface *lpTextSurface;
  SDL_Rect in;

  zoomFactor = 1; //FIXME
  SDL_FillRect(lpBackBuffer, NULL, SDL_MapRGB(lpBackBuffer->format, 0, 0, 0)); 
  /* Prepare the string */
  srtDelay /= GAME_NUMGAMETICKS_SEC; /* Convert ticks back to seconds */
  sprintf(strNum, "%d", srtDelay);
  strcpy(str, langGetText(STR_DRAW_GAMESTARTSIN));
  strcat(str, strNum);
  lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
  if (lpTextSurface) {
    src.x = zoomFactor * TILE_SIZE_X + 5;
    src.y = zoomFactor * TILE_SIZE_Y + 5; 
    src.w = lpTextSurface->w;
    src.h = lpTextSurface->h;
    SDL_BlitSurface(lpTextSurface, NULL, lpBackBuffer, &src);
    SDL_UpdateRect(lpBackBuffer, 0, 0, 0, 0);
    in.x = (zoomFactor * TILE_SIZE_X);
    in.y = (zoomFactor * TILE_SIZE_Y);
    in.w = zoomFactor * MAIN_SCREEN_SIZE_X * TILE_SIZE_X;
    in.h = zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y;
    src.x = zoomFactor * MAIN_OFFSET_X;
    src.y = zoomFactor * MAIN_OFFSET_Y;
    src.w = in.w;
    src.h = in.h;
    SDL_BlitSurface(lpBackBuffer, &in, lpScreen, &src);
    SDL_UpdateRect(lpScreen, src.x, src.y, src.w, src.h);
    SDL_FreeSurface(lpTextSurface);
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
*********************************************************/
void drawMainScreen(screen *value, screenMines *mineView, screenTanks *tks, screenGunsight *gs, screenBullets *sBullets, screenLgm *lgms, bool showPillLabels, bool showBaseLabels, long srtDelay, bool isPillView, int edgeX, int edgeY, bool useCursor, BYTE cursorLeft, BYTE cursorTop) {
  SDL_Rect output;     /* Output Rectangle */
  SDL_Rect textOutput; /* Text Output Rect */
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
  SDL_Rect in;
  int  ret;
  SDL_Surface *lpTextSurface;

  count = 0;
  x = 0;
  y = 0;
  done = FALSE;
  zoomFactor = 1; //FIXME: windowGetZoomFactor();
  in.w = zoomFactor * TILE_SIZE_X;
  in.h = zoomFactor * TILE_SIZE_Y;
  output.w = zoomFactor * TILE_SIZE_X;
  output.h = zoomFactor * TILE_SIZE_Y;
  str[0] = '\0';
  if (srtDelay > 0) {
    /* Waiting for game to start. Draw coutdown */
    drawStartDelay(srtDelay);
    return;
  }
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
    in.x = outputX;
    in.y = outputY;
    output.x = x * zoomFactor * TILE_SIZE_X;
    output.y = y * zoomFactor * TILE_SIZE_Y;
    ret = SDL_BlitSurface(lpTiles, &in, lpBackBuffer, &output);
    
    /* Draw Mines */
    if ((screenIsMine(mineView,x,y)) == TRUE) {
      in.x = zoomFactor * MINE_X;
      in.y = zoomFactor * MINE_Y;
      SDL_BlitSurface(lpTiles, &in, lpBackBuffer, &output);
    }

    /* Draw the pillNumber or base Number if required */
    if (isPill == TRUE && showPillLabels == TRUE) {
      labelNum = screenPillNumPos(x, y);
      sprintf(str, "%d", (labelNum-1));
      lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
      textOutput.x = (zoomFactor * x * TILE_SIZE_X) + zoomFactor * LABEL_OFFSET_X;
      textOutput.y =  (zoomFactor * y * TILE_SIZE_Y);
      textOutput.w = lpTextSurface->w;
      textOutput.h = lpTextSurface->h;
      SDL_BlitSurface(lpTextSurface, NULL, lpBackBuffer, &textOutput);
      SDL_FreeSurface(lpTextSurface);
    }

    if (isBase == TRUE && showBaseLabels == TRUE) {
      labelNum = screenBaseNumPos(x, y);
      sprintf(str, "%d", (labelNum-1));
      lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
      textOutput.x = (zoomFactor * x * TILE_SIZE_X) + zoomFactor * LABEL_OFFSET_X;
      textOutput.y =  (zoomFactor * y * TILE_SIZE_Y);
      textOutput.w = lpTextSurface->w;
      textOutput.h = lpTextSurface->h;
      SDL_BlitSurface(lpTextSurface, NULL, lpBackBuffer, &textOutput);
      SDL_FreeSurface(lpTextSurface);
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

  /* Draw Gunsight */
  if (gs->mapX != NO_GUNSIGHT) {
    in.x = zoomFactor * GUNSIGHT_X;
    in.w = zoomFactor * TILE_SIZE_X;
    in.y = zoomFactor * GUNSIGHT_Y;
    in.h = zoomFactor * TILE_SIZE_Y;
    output.x = (gs->mapX) * TILE_SIZE_X + (gs->pixelX);
    output.y = (gs->mapY) * TILE_SIZE_Y + (gs->pixelY);
    output.w = zoomFactor * TILE_SIZE_X;
    output.h = zoomFactor * TILE_SIZE_Y;
    SDL_BlitSurface(lpTiles, &in, lpBackBuffer, &output);
  }

  /* Draw the Cursor Square if required */
  if (useCursor == TRUE) {
    in.x = zoomFactor * MOUSE_SQUARE_X;
    in.w = zoomFactor * TILE_SIZE_X;
    in.y = zoomFactor * MOUSE_SQUARE_Y;
    in.h = zoomFactor * TILE_SIZE_Y;
    output.x = cursorLeft * zoomFactor * TILE_SIZE_X;
    output.y = cursorTop * zoomFactor * TILE_SIZE_Y;
    output.w = zoomFactor * TILE_SIZE_X;
    output.h = zoomFactor * TILE_SIZE_X;
    SDL_BlitSurface(lpTiles, &in, lpBackBuffer, &output);
  }

  /* Copy the back buffer to the window */
  SDL_UpdateRect(lpBackBuffer, 0, 0, 0, 0);
  in.x = (zoomFactor * TILE_SIZE_X) + edgeX;
  in.y = (zoomFactor * TILE_SIZE_Y) + edgeY;
  in.w = zoomFactor * MAIN_SCREEN_SIZE_X * TILE_SIZE_X;
  in.h = zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y;
  output.x =  (zoomFactor * MAIN_OFFSET_X);
  output.y = (zoomFactor * MAIN_OFFSET_Y);
  output.w = in.w;
  output.h = in.h;
 
  if (isPillView == TRUE) {
    /* we are in pillbox view - Write text here */
    drawPillInView();
  }

  if (netGetStatus() == netFailed) {
    drawNetFailed();
  }

  ret = SDL_BlitSurface(lpBackBuffer, &in, lpScreen, &output);
  SDL_UpdateRect(lpScreen, output.x, output.y, output.w, output.h);

  /* Frame rate counting stuff */
    g_dwFrameCount++;
time = SDL_GetTicks() - g_dwFrameTime;
if( time > 1000) {
g_dwFrameTotal = g_dwFrameCount;
sprintf(str, "%d", g_dwFrameTotal);
g_dwFrameTime = SDL_GetTicks();
g_dwFrameCount = 0;
}

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
bool drawBackground(int width, int height) {
  bool returnValue;   /* Value to return */
  BYTE zoomFactor;    /* Scaling Factor */
  SDL_Surface *bg;    /* Background */
  SDL_Rect destRect;  /* Copying rect */
  BYTE *buff;
  char fileName[FILENAME_MAX];
  char *tf;
  FILE *fp;
  int ret;
  
  buff = malloc(168778);
  /* Get tmp file */
  strcpy(fileName, g_get_tmp_dir());
  tf = tempnam(fileName, "lb");
  strcpy(fileName, tf);
  ret = lzwdecoding(B_IMAGE, buff, 17099);
  if (ret != 168778) {
    free(buff);
    return FALSE;
  }
  returnValue = FALSE;
  zoomFactor = 1; //FIXME: windowGetZoomFactor();
  fp = fopen(fileName, "wb");
  fwrite(buff, 168778, 1, fp);
  fflush(fp);
  fclose(fp);
  bg = SDL_LoadBMP(fileName);
  unlink(fileName);
  if (bg != NULL) {
    destRect.x = 0;
    destRect.y = 0;
    destRect.w = bg->w;
    destRect.h = bg->h;
    if (SDL_BlitSurface(bg, NULL, lpScreen, &destRect) == 0) {
      returnValue = TRUE;
      SDL_UpdateRect(lpScreen, 0,0,0,0);
    }  
    SDL_FreeSurface(bg);
  }
  free(buff);
  return returnValue;
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
  SDL_Rect src;        /* Used for copying the bases & pills icon in */
  SDL_Rect dest;       /* Used for copying the bases & pills icon in */
  BYTE zoomFactor; /* Scaling Factor */

  zoomFactor = 1; //FIXME: windowGetZoomFactor();
  src.x = zoomFactor * BASE_GOOD_X;
  src.y = zoomFactor * BASE_GOOD_Y;
  src.w = zoomFactor * TILE_SIZE_X;
  src.h = zoomFactor * TILE_SIZE_Y;
  dest.x = zoomFactor * STATUS_BASES_MIDDLE_ICON_X;
  dest.y = zoomFactor * STATUS_BASES_MIDDLE_ICON_Y;
  dest.w = zoomFactor * TILE_SIZE_X;
  dest.h = zoomFactor * TILE_SIZE_Y;
  SDL_FillRect(lpBasesStatus, NULL, SDL_MapRGB(lpBasesStatus->format, 0, 0, 0));
  SDL_BlitSurface(lpTiles, &src, lpBasesStatus, &dest);
  SDL_UpdateRect(lpBasesStatus, 0, 0, 0, 0);
}

/*********************************************************
* *NAME:          drawSetPillsStatusClear
*AUTHOR:        John Morrison
*CREATION DATE: 23/1/98
*LAST MODIFIED: 23/1/98
*PURPOSE:
*  Clears the pills status display.
*
*ARGUMENTS:
*********************************************************/
void drawSetPillsStatusClear(void) {
  SDL_Rect src;    /* Used for copying the bases & pills icon in */
  SDL_Rect dest;   /* Used for copying the bases & pills icon in */
  BYTE zoomFactor; /* Scaling Factor */

  zoomFactor = 1; //FIXME: windowGetZoomFactor();
  src.x = zoomFactor * PILL_GOOD15_X;
  src.y = zoomFactor * PILL_GOOD15_Y;
  src.w = zoomFactor * TILE_SIZE_X;
  src.h = zoomFactor * TILE_SIZE_Y;
  dest.y = zoomFactor * STATUS_PILLS_MIDDLE_ICON_Y;
  dest.x = zoomFactor * STATUS_PILLS_MIDDLE_ICON_X;
  dest.w = zoomFactor * TILE_SIZE_X;
  dest.h = zoomFactor * TILE_SIZE_Y;
  SDL_FillRect(lpPillsStatus, NULL, SDL_MapRGB(lpPillsStatus->format, 0, 0, 0));
  SDL_BlitSurface(lpTiles, &src, lpPillsStatus, &dest);
  SDL_UpdateRect(lpPillsStatus, 0, 0, 0, 0);
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
  SDL_Rect src;    /* Used for copying the bases & pills icon in */
  SDL_Rect dest;   /* Used for copying the bases & pills icon in */
  BYTE zoomFactor; /* Scaling Factor */

  zoomFactor = 1; //FIXME: windowGetZoomFactor();
  src.x = zoomFactor * TANK_SELF_0_X;
  src.y = zoomFactor * TANK_SELF_0_Y;
  src.w = zoomFactor * TILE_SIZE_X;
  src.h = zoomFactor * TILE_SIZE_Y;
  dest.y = zoomFactor * STATUS_TANKS_MIDDLE_ICON_Y;
  dest.x = zoomFactor * STATUS_TANKS_MIDDLE_ICON_X;
  dest.w = zoomFactor * TILE_SIZE_X;
  dest.h = zoomFactor * TILE_SIZE_Y;
  SDL_FillRect(lpTankStatus, NULL, SDL_MapRGB(lpTankStatus->format, 0, 0, 0));
  SDL_BlitSurface(lpTiles, &src, lpTankStatus, &dest);
  SDL_UpdateRect(lpTankStatus, 0, 0, 0, 0);
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
void drawCopyBasesStatus() {
  SDL_Rect dest; /* Destination location */
  BYTE zf;   /* Zoom Factor */

  zf = 1; //FIXME: windowGetZoomFactor();

  dest.x = zf * STATUS_BASES_LEFT;
  dest.y = zf * STATUS_BASES_TOP;
  dest.w = zf * STATUS_BASES_WIDTH;
  dest.h = zf * STATUS_BASES_HEIGHT;
  SDL_BlitSurface(lpBasesStatus, NULL, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
}

/*********************************************************
 * *NAME:          drawCopyPillsStatus
 * *AUTHOR:        John Morrison
 * *CREATION DATE: 23/1/98
 * *LAST MODIFIED: 23/1/98
 * *PURPOSE:
 * *  Copys the pills status on to the primary buffer
 * *
 * *ARGUMENTS:
 * *  xValue  - The left position of the window
 * *  yValue  - The top position of the window
 * *********************************************************/
void drawCopyPillsStatus() {
  SDL_Rect dest; /* Destination location */
  BYTE zf;   /* Zoom Factor */

  zf = 1; //FIXME: windowGetZoomFactor();

  dest.x = zf * STATUS_PILLS_LEFT;
  dest.y = zf * STATUS_PILLS_TOP;
  dest.w = zf * STATUS_PILLS_WIDTH;
  dest.h = zf * STATUS_PILLS_HEIGHT;
  SDL_BlitSurface(lpPillsStatus, NULL, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
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
void drawCopyTanksStatus() {
  SDL_Rect dest; /* Destination location */
  BYTE zf;   /* Zoom Factor */

  zf = 1; //FIXME: windowGetZoomFactor();

  dest.x = zf * STATUS_TANKS_LEFT;
  dest.y = zf * STATUS_TANKS_TOP;
  dest.w = zf * STATUS_TANKS_WIDTH;
  dest.h = zf * STATUS_TANKS_HEIGHT;
  SDL_BlitSurface(lpTankStatus, NULL, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
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
  SDL_Rect src;  /* The src square on the tile file to retrieve */
  SDL_Rect dest; /* The dest square to draw it */
  BYTE zf;   /* Scaling factor */
  char str[3]; /* String to output if labels are on */

  zf = 1; //FIXME: windowGetZoomFactor();
  str[0] = '\0';

  src.w = zf * STATUS_ITEM_SIZE_X;
  src.h = zf * STATUS_ITEM_SIZE_Y;
  
  /* Set the co-ords of the tile file to get */
  switch (ba) {
  case baseDead:
    src.x = zf * STATUS_ITEM_DEAD_X;
    src.y = zf * STATUS_ITEM_DEAD_Y;
    break;
  case baseNeutral:
    src.x = zf * STATUS_BASE_NEUTRAL_X;
    src.y = zf * STATUS_BASE_NEUTRAL_Y;
    break;
  case baseOwnGood:
    src.x = zf * STATUS_BASE_GOOD_X;
    src.y = zf * STATUS_BASE_GOOD_Y;
    break;
  case baseAllieGood:
    src.x = zf * STATUS_BASE_ALLIEGOOD_X;
    src.y = zf * STATUS_BASE_ALLIEGOOD_Y;
    break;
  default:
    /* Base Evil */
    src.x = zf * STATUS_BASE_EVIL_X;
    src.y = zf * STATUS_BASE_EVIL_Y;
    break;
  }
  /* Modify the offset to allow for the indents */
  switch (baseNum) {
  case BASE_1:
    dest.x = (zf * STATUS_BASE_1_X);
    dest.y = (zf * STATUS_BASE_1_Y);
    break;
  case BASE_2:
    dest.x = (zf * STATUS_BASE_2_X);
    dest.y = (zf * STATUS_BASE_2_Y);
    break;
  case BASE_3:
    dest.x = (zf * STATUS_BASE_3_X);
    dest.y = (zf * STATUS_BASE_3_Y);
    break;
  case BASE_4:
    dest.x = (zf * STATUS_BASE_4_X);
    dest.y = (zf * STATUS_BASE_4_Y);
    break;
  case BASE_5:
    dest.x = (zf * STATUS_BASE_5_X);
    dest.y = (zf * STATUS_BASE_5_Y);
    break;
  case BASE_6:
    dest.x = (zf * STATUS_BASE_6_X);
    dest.y = (zf * STATUS_BASE_6_Y);
    break;
  case BASE_7:
    dest.x = (zf * STATUS_BASE_7_X);
    dest.y = (zf * STATUS_BASE_7_Y);
    break;
  case BASE_8:
    dest.x = (zf * STATUS_BASE_8_X);
    dest.y = (zf * STATUS_BASE_8_Y);
    break;
  case BASE_9:
    dest.x = (zf * STATUS_BASE_9_X);
    dest.y = (zf * STATUS_BASE_9_Y);
    break;
  case BASE_10:
    dest.x = (zf * STATUS_BASE_10_X);
    dest.y = (zf * STATUS_BASE_10_Y);
    break;
  case BASE_11:
    dest.x = (zf * STATUS_BASE_11_X);
    dest.y = (zf * STATUS_BASE_11_Y);
    break;
  case BASE_12:
    dest.x = (zf * STATUS_BASE_12_X);
    dest.y = (zf * STATUS_BASE_12_Y);
    break;
  case BASE_13:
    dest.x = (zf * STATUS_BASE_13_X);
    dest.y = (zf * STATUS_BASE_13_Y);
    break;
  case BASE_14:
    dest.x = (zf * STATUS_BASE_14_X);
    dest.y = (zf * STATUS_BASE_14_Y);
    break;
  case BASE_15:
    dest.x = (zf * STATUS_BASE_15_X);
    dest.y = (zf * STATUS_BASE_15_Y);
    break;
  default:
   /* BASE_16:*/
    dest.x = (zf * STATUS_BASE_16_X);
    dest.y = (zf * STATUS_BASE_16_Y);
  }

  dest.w = zf * STATUS_ITEM_SIZE_X;
  dest.h = zf * STATUS_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpBasesStatus, &dest);
  if (labels == TRUE) {
    /* Must draw the label */
    sprintf(str, "%d", (baseNum-1));
/* FIXME    if (SUCCEEDED(lpDDSBasesStatus->lpVtbl->GetDC(lpDDSBasesStatus,&hDC))) {
      fontSelectTiny(hDC);
      SetBkColor(hDC, RGB(0,0,0));
      SetTextColor(hDC, RGB(255,255,255));
      DrawText(hDC, str, strlen(str), &dest, (DT_TOP | DT_NOCLIP));
      lpDDSBasesStatus->lpVtbl->ReleaseDC(lpDDSBasesStatus, &hDC);
    } */
  }
  SDL_UpdateRects(lpBasesStatus, 1, &dest);
  gdk_threads_leave();
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
  SDL_Rect src;    /* The src square on the tile file to retrieve */
  SDL_Rect dest;   /* The dest square to draw it */
  BYTE zf;     /* Scaling factor */
  char str[3]; /* String to output if labels are on */

  zf = 1; //FIXME: windowGetZoomFactor();
  str[0] = '\0';
  src.w = zf * STATUS_ITEM_SIZE_X;
  src.h = zf * STATUS_ITEM_SIZE_Y;
 
  /* Set the co-ords of the tile file to get */
  switch (pb) {
  case pillDead:
    src.x = zf * STATUS_ITEM_DEAD_X;
    src.y = zf * STATUS_ITEM_DEAD_Y;
   break;
  case pillNeutral:
    src.x = zf * STATUS_PILLBOX_NEUTRAL_X;
    src.y = zf * STATUS_PILLBOX_NEUTRAL_Y;
    break;
  case pillGood:
    src.x = zf * STATUS_PILLBOX_GOOD_X;
    src.y = zf * STATUS_PILLBOX_GOOD_Y;
    break;
  case pillAllie:
    src.x = zf * STATUS_PILLBOX_ALLIEGOOD_X;
    src.y = zf * STATUS_PILLBOX_ALLIEGOOD_Y;
    break;
  case pillTankGood:
    src.x = zf * STATUS_PILLBOX_TANKGOOD_X;
    src.y = zf * STATUS_PILLBOX_TANKGOOD_Y;
    break;
  case pillTankAllie:
    src.x = zf * 272; //STATUS_PILLBOX_TANKALLIE_X;
    src.y = zf * 144; //STATUS_PILLBOX_TANKALLIE_Y +5 ;
    break;
  case pillTankEvil:
    src.x = zf * STATUS_PILLBOX_TANKEVIL_X;
    src.y = zf * STATUS_PILLBOX_TANKEVIL_Y;
    break;
  default:
    /* PILLBOX Evil */
    src.x = zf * STATUS_PILLBOX_EVIL_X;
    src.y = zf * STATUS_PILLBOX_EVIL_Y;
    break;
  }
  /* Modify the offset to allow for the indents */
  switch (pillNum) {
  case PILLBOX_1:
    dest.x = (zf * STATUS_PILLBOX_1_X);
    dest.y = (zf * STATUS_PILLBOX_1_Y);
    break;
  case PILLBOX_2:
    dest.x = (zf * STATUS_PILLBOX_2_X);
    dest.y = (zf * STATUS_PILLBOX_2_Y);
    break;
  case PILLBOX_3:
    dest.x = (zf * STATUS_PILLBOX_3_X);
    dest.y = (zf * STATUS_PILLBOX_3_Y);
    break;
  case PILLBOX_4:
    dest.x = (zf * STATUS_PILLBOX_4_X);
    dest.y = (zf * STATUS_PILLBOX_4_Y);
    break;
  case PILLBOX_5:
    dest.x = (zf * STATUS_PILLBOX_5_X);
    dest.y = (zf * STATUS_PILLBOX_5_Y);
    break;
  case PILLBOX_6:
    dest.x = (zf * STATUS_PILLBOX_6_X);
    dest.y = (zf * STATUS_PILLBOX_6_Y);
    break;
  case PILLBOX_7:
    dest.x = (zf * STATUS_PILLBOX_7_X);
    dest.y = (zf * STATUS_PILLBOX_7_Y);
    break;
  case PILLBOX_8:
    dest.x = (zf * STATUS_PILLBOX_8_X);
    dest.y = (zf * STATUS_PILLBOX_8_Y);
    break;
  case PILLBOX_9:
    dest.x = (zf * STATUS_PILLBOX_9_X);
    dest.y = (zf * STATUS_PILLBOX_9_Y);
    break;
  case PILLBOX_10:
    dest.x = (zf * STATUS_PILLBOX_10_X);
    dest.y = (zf * STATUS_PILLBOX_10_Y);
    break;
  case PILLBOX_11:
    dest.x = (zf * STATUS_PILLBOX_11_X);
    dest.y = (zf * STATUS_PILLBOX_11_Y);
    break;
  case PILLBOX_12:
    dest.x = (zf * STATUS_PILLBOX_12_X);
    dest.y = (zf * STATUS_PILLBOX_12_Y);
    break;
  case PILLBOX_13:
    dest.x = (zf * STATUS_PILLBOX_13_X);
    dest.y = (zf * STATUS_PILLBOX_13_Y);
    break;
  case PILLBOX_14:
    dest.x = (zf * STATUS_PILLBOX_14_X);
    dest.y = (zf * STATUS_PILLBOX_14_Y);
    break;
  case PILLBOX_15:
    dest.x = (zf * STATUS_PILLBOX_15_X);
    dest.y = (zf * STATUS_PILLBOX_15_Y);
    break;
  default:
   /* PILLBOX_16:*/
    dest.x = (zf * STATUS_PILLBOX_16_X);
    dest.y = (zf * STATUS_PILLBOX_16_Y);
  }

  dest.w = zf * STATUS_ITEM_SIZE_X;
  dest.h = zf * STATUS_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpPillsStatus, &dest);
  if (labels == TRUE) {
    /* Must draw the label */
    sprintf(str, "%d", (pillNum-1));
/* FIXME:    if (SUCCEEDED(lpDDSPillsStatus->lpVtbl->GetDC(lpDDSPillsStatus,&hDC))) {
      fontSelectTiny(hDC);
      SetBkColor(hDC, RGB(0,0,0));
      SetTextColor(hDC, RGB(255,255,255));
      DrawText(hDC, str, strlen(str), &dest, (DT_TOP | DT_NOCLIP));
      lpDDSPillsStatus->lpVtbl->ReleaseDC(lpDDSPillsStatus,&hDC);
    } */
  }
  SDL_UpdateRects(lpPillsStatus, 1, &dest);
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
  SDL_Rect src;    /* The src square on the tile file to retrieve */
  SDL_Rect dest;   /* The dest square to draw it */
  BYTE zf;     /* Scaling factor */

  zf = 1; //FIXME: windowGetZoomFactor();
  src.w = zf * STATUS_ITEM_SIZE_X;
  src.h = zf * STATUS_ITEM_SIZE_Y;
 
  /* Set the co-ords of the tile file to get */
  switch (ta) {
  case tankNone:
    src.x = zf * STATUS_TANK_NONE_X;
    src.y = zf * STATUS_TANK_NONE_Y;
    break;
  case tankSelf:
    src.x = zf * STATUS_TANK_SELF_X;
    src.y = zf * STATUS_TANK_SELF_Y;
   break;
  case tankAllie:
    src.x = zf * STATUS_TANK_GOOD_X;
    src.y = zf * STATUS_TANK_GOOD_Y;
    break;
  default:
    /* tankEvil */
    src.x = zf * STATUS_TANK_EVIL_X;
    src.y = zf * STATUS_TANK_EVIL_Y;
    break;
  }
  /* Modify the offset to allow for the indents */
  switch (tankNum) {
  case TANK_1:
    dest.x = (zf * STATUS_TANKS_1_X);
    dest.y = (zf * STATUS_TANKS_1_Y);
    break;
  case TANK_2:
    dest.x = (zf * STATUS_TANKS_2_X);
    dest.y = (zf * STATUS_TANKS_2_Y);
    break;
  case TANK_3:
    dest.x = (zf * STATUS_TANKS_3_X);
    dest.y = (zf * STATUS_TANKS_3_Y);
    break;
  case TANK_4:
    dest.x = (zf * STATUS_TANKS_4_X);
    dest.y = (zf * STATUS_TANKS_4_Y);
    break;
  case TANK_5:
    dest.x = (zf * STATUS_TANKS_5_X);
    dest.y = (zf * STATUS_TANKS_5_Y);
    break;
  case TANK_6:
    dest.x = (zf * STATUS_TANKS_6_X);
    dest.y = (zf * STATUS_TANKS_6_Y);
    break;
  case TANK_7:
    dest.x = (zf * STATUS_TANKS_7_X);
    dest.y = (zf * STATUS_TANKS_7_Y);
    break;
  case TANK_8:
    dest.x = (zf * STATUS_TANKS_8_X);
    dest.y = (zf * STATUS_TANKS_8_Y);
    break;
  case TANK_9:
    dest.x = (zf * STATUS_TANKS_9_X);
    dest.y = (zf * STATUS_TANKS_9_Y);
    break;
  case TANK_10:
    dest.x = (zf * STATUS_TANKS_10_X);
    dest.y = (zf * STATUS_TANKS_10_Y);
    break;
  case TANK_11:
    dest.x = (zf * STATUS_TANKS_11_X);
    dest.y = (zf * STATUS_TANKS_11_Y);
    break;
  case TANK_12:
    dest.x = (zf * STATUS_TANKS_12_X);
    dest.y = (zf * STATUS_TANKS_12_Y);
    break;
  case TANK_13:
    dest.x = (zf * STATUS_TANKS_13_X);
    dest.y = (zf * STATUS_TANKS_13_Y);
    break;
  case TANK_14:
    dest.x = (zf * STATUS_TANKS_14_X);
    dest.y = (zf * STATUS_TANKS_14_Y);
    break;
  case TANK_15:
    dest.x = (zf * STATUS_TANKS_15_X);
    dest.y = (zf * STATUS_TANKS_15_Y);
    break;
  default:
   /* TANK_16:*/
    dest.x = (zf * STATUS_TANKS_16_X);
    dest.y = (zf * STATUS_TANKS_16_Y);
  }

  dest.w = zf * STATUS_ITEM_SIZE_X;
  dest.h = zf * STATUS_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpTankStatus, &dest);
  SDL_UpdateRects(lpTankStatus, 1, &dest);
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
  SDL_Rect dest;              /* The dest square to draw it */
  SDL_Rect fill;
  BYTE zf;   /* Scaling factor */
  Uint32 color; /* Fill green colour */
  
  zf = 1; //FIXME: windowGetZoomFactor();
  dest.w = zf * STATUS_TANK_BARS_WIDTH;
  color = SDL_MapRGB(lpScreen->format, 0, 0xFF, 0);
  
  /* Make the area black first */
  fill.y = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - ((zf * BAR_TANK_MULTIPLY) *  40);
  fill.h = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - fill.y;
  fill.x = xValue + (zf * STATUS_TANK_SHELLS);
  fill.w = xValue + (zf * STATUS_TANK_TREES) + (zf * STATUS_TANK_BARS_WIDTH) - fill.x;
  SDL_FillRect(lpScreen, &fill, SDL_MapRGB(lpScreen->format, 0, 0, 0));

  /* Shells */
  dest.y = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - ((zf * BAR_TANK_MULTIPLY) * shells);
  dest.x = xValue + (zf * STATUS_TANK_SHELLS);
  dest.h = zf * BAR_TANK_MULTIPLY * shells;
  SDL_FillRect(lpScreen, &dest, color);
  
  /* Mines */
  dest.y = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - ((zf * BAR_TANK_MULTIPLY) * mines);
  dest.x = xValue + (zf * STATUS_TANK_MINES);
  dest.h = zf * BAR_TANK_MULTIPLY * mines;
  SDL_FillRect(lpScreen, &dest, color);

  /* Armour */
  dest.y = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - ((zf * BAR_TANK_MULTIPLY) * armour);
  dest.x = xValue + (zf * STATUS_TANK_ARMOUR);
  dest.h = zf * BAR_TANK_MULTIPLY * armour;
  SDL_FillRect(lpScreen, &dest, color);
  
/* Trees */
  dest.y = yValue + (zf * STATUS_TANK_BARS_TOP) + (zf * STATUS_TANK_BARS_HEIGHT) - ((zf * BAR_TANK_MULTIPLY) * trees);
  dest.x = xValue + (zf * STATUS_TANK_TREES);
  dest.h = zf * BAR_TANK_MULTIPLY * trees;
  SDL_FillRect(lpScreen, &dest, color);

  SDL_UpdateRects(lpScreen, 1, &fill);
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
  SDL_Rect dest;              /* The dest square to draw it */
  SDL_Rect fill;
  static BYTE lastShells = 0;  /* Last amount of stuff to save on rendering and flicker */
  static BYTE lastMines = 0;
  static BYTE lastArmour = 0;
  BYTE zf;                /* Zoom Factor */
  Uint32 color; /* Fill green colour */

  zf = 1; //FIXME: windowGetZoomFactor();

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
    /* Make the area black first */
    fill.y = yValue + (zf * STATUS_BASE_SHELLS);
    fill.x = xValue + (zf * STATUS_BASE_BARS_LEFT);
    fill.h = yValue + (zf * STATUS_BASE_MINES) + (zf * STATUS_BASE_BARS_HEIGHT) - fill.y;
    fill.w = zf * STATUS_BASE_BARS_MAX_WIDTH;
    SDL_FillRect(lpScreen, &fill, SDL_MapRGB(lpScreen->format, 0, 0, 0));
    if (shells != 0 || mines != 0 || armour != 0) {
      color = SDL_MapRGB(lpScreen->format, 0, 0xFF, 0);
      dest.x = xValue + (zf * STATUS_BASE_BARS_LEFT);
      dest.h = zf * STATUS_BASE_BARS_HEIGHT;
      /* Shells */
      dest.y = yValue + (zf * STATUS_BASE_SHELLS);
      dest.w = shells * zf * BAR_BASE_MULTIPLY;
      SDL_FillRect(lpScreen, &dest, color);
      /* Mines */
      dest.y = yValue + (zf * STATUS_BASE_MINES);
      dest.w = mines * zf * BAR_BASE_MULTIPLY;
      SDL_FillRect(lpScreen, &dest, color);
      /* Armour */
      dest.y = yValue + (zf * STATUS_BASE_ARMOUR);
      dest.w = armour * zf * BAR_BASE_MULTIPLY;
      SDL_FillRect(lpScreen, &dest, color);
    }
    SDL_UpdateRects(lpScreen, 1, &fill);
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
  SDL_Rect src;        /* The src square on the tile file to retrieve */
  SDL_Rect dest;       /* The dest square to draw it */
  BYTE zoomFactor; /* Scaling factor */

  zoomFactor = 1; //FIXME windowGetZoomFactor();

  /* Set the co-ords of the tile file to get */
  src.x = zoomFactor * INDENT_ON_X;
  src.y = zoomFactor * INDENT_ON_Y;
  src.w = zoomFactor * BS_ITEM_SIZE_X;
  src.h = zoomFactor * BS_ITEM_SIZE_Y;

  /* Modify the offset to allow for the indents */
  dest.x = xValue;
  dest.y = yValue;
  switch (value) {
  case BsTrees:
    dest.x += (zoomFactor * BS_TREE_OFFSET_X);
    dest.y += (zoomFactor * BS_TREE_OFFSET_Y);
    break;
  case BsRoad:
    dest.x += (zoomFactor * BS_ROAD_OFFSET_X);
    dest.y += (zoomFactor * BS_ROAD_OFFSET_Y);
    break;
  case BsBuilding:
    dest.x += (zoomFactor * BS_BUILDING_OFFSET_X);
    dest.y += (zoomFactor * BS_BUILDING_OFFSET_Y);
    break;
  case BsPillbox:
    dest.x += (zoomFactor * BS_PILLBOX_OFFSET_X);
    dest.y+= (zoomFactor * BS_PILLBOX_OFFSET_Y);
    break;
  default:
   /* BsMine:*/
    dest.x += (zoomFactor * BS_MINE_OFFSET_X);
    dest.y += (zoomFactor * BS_MINE_OFFSET_Y);
    break;
  }
  dest.w = zoomFactor * BS_ITEM_SIZE_X;
  dest.h = zoomFactor * BS_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);

 /* Set the co-ords of the tile file to get */
  src.x = zoomFactor * INDENT_DOT_ON_X;
  src.y = zoomFactor * INDENT_DOT_ON_Y;
  src.w = zoomFactor * BS_DOT_ITEM_SIZE_X;
  src.h = zoomFactor * BS_DOT_ITEM_SIZE_Y;

  /* Draw the dot */
  /* Modify the offset to allow for the indents */
  dest.x = xValue;
  dest.y = yValue;
  switch (value) {
  case BsTrees:
    dest.x += (zoomFactor * BS_DOT_TREE_OFFSET_X);
    dest.y += (zoomFactor * BS_DOT_TREE_OFFSET_Y);
    break;
  case BsRoad:
    dest.x += (zoomFactor * BS_DOT_ROAD_OFFSET_X);
    dest.y += (zoomFactor * BS_DOT_ROAD_OFFSET_Y);
    break;
  case BsBuilding:
    dest.x += (zoomFactor * BS_DOT_BUILDING_OFFSET_X);
    dest.y += (zoomFactor * BS_DOT_BUILDING_OFFSET_Y);
    break;
  case BsPillbox:
    dest.x += (zoomFactor * BS_DOT_PILLBOX_OFFSET_X);
    dest.y += (zoomFactor * BS_DOT_PILLBOX_OFFSET_Y);
    break;
  default:
   /* BsMine:*/
    dest.x += (zoomFactor * BS_DOT_MINE_OFFSET_X);
    dest.y += (zoomFactor * BS_DOT_MINE_OFFSET_Y);
    break;
  }

  dest.w = zoomFactor * BS_DOT_ITEM_SIZE_X;
  dest.h = zoomFactor * BS_DOT_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
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
  SDL_Rect src;        /* The src square on the tile file to retrieve */
  SDL_Rect dest;       /* The dest square to draw it */
  BYTE zoomFactor; /* Scaling factor */

  zoomFactor = 1; //FIXME windowGetZoomFactor();

  /* Set the co-ords of the tile file to get */
  src.x = zoomFactor * INDENT_OFF_X;
  src.y = zoomFactor * INDENT_OFF_Y;
  src.w = zoomFactor * BS_ITEM_SIZE_X;
  src.h = zoomFactor * BS_ITEM_SIZE_Y;

  /* Modify the offset to allow for the indents */
  dest.x= xValue;
  dest.y = yValue;
  switch (value) {
  case BsTrees:
    dest.x += (zoomFactor * BS_TREE_OFFSET_X);
    dest.y += (zoomFactor * BS_TREE_OFFSET_Y);
    break;
  case BsRoad:
    dest.x += (zoomFactor * BS_ROAD_OFFSET_X);
    dest.y += (zoomFactor * BS_ROAD_OFFSET_Y);
    break;
  case BsBuilding:
    dest.x += (zoomFactor * BS_BUILDING_OFFSET_X);
    dest.y += (zoomFactor * BS_BUILDING_OFFSET_Y);
    break;
  case BsPillbox:
    dest.x += (zoomFactor * BS_PILLBOX_OFFSET_X);
    dest.y += (zoomFactor * BS_PILLBOX_OFFSET_Y);
    break;
  default:
   /* BsMine:*/
    dest.x += (zoomFactor * BS_MINE_OFFSET_X);
    dest.y += (zoomFactor * BS_MINE_OFFSET_Y);
    break;
  }
  dest.w = zoomFactor * BS_ITEM_SIZE_X;
  dest.h = zoomFactor * BS_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
    
  /* Set the co-ords of the tile file to get */
  src.x = zoomFactor * INDENT_DOT_OFF_X;
  src.y = zoomFactor * INDENT_DOT_OFF_Y;
  src.w = zoomFactor * BS_DOT_ITEM_SIZE_X;
  src.h = zoomFactor * BS_DOT_ITEM_SIZE_Y;

  /* Draw the dot */
  /* Modify the offset to allow for the indents */
  dest.x = xValue;
  dest.y = yValue;
  switch (value) {
  case BsTrees:
    dest.x += (zoomFactor * BS_DOT_TREE_OFFSET_X);
    dest.y += (zoomFactor * BS_DOT_TREE_OFFSET_Y);
    break;
  case BsRoad:
    dest.x += (zoomFactor * BS_DOT_ROAD_OFFSET_X);
    dest.y += (zoomFactor * BS_DOT_ROAD_OFFSET_Y);
    break;
  case BsBuilding:
    dest.x += (zoomFactor * BS_DOT_BUILDING_OFFSET_X);
    dest.y += (zoomFactor * BS_DOT_BUILDING_OFFSET_Y);
    break;
  case BsPillbox:
    dest.x += (zoomFactor * BS_DOT_PILLBOX_OFFSET_X);
    dest.y += (zoomFactor * BS_DOT_PILLBOX_OFFSET_Y);
    break;
  default:
   /* BsMine:*/
    dest.x += (zoomFactor * BS_DOT_MINE_OFFSET_X);
    dest.y += (zoomFactor * BS_DOT_MINE_OFFSET_Y);
    break;
  }

  dest.w = zoomFactor * BS_DOT_ITEM_SIZE_X;
  dest.h = zoomFactor * BS_DOT_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
    
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
void drawRedrawAll(int width, int height, buildSelect value, bool showPillsStatus, bool showBasesStatus) {
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

  lpScreen = SDL_SetVideoMode(width, height, 0, 0);
  drawBackground(width, height);
  drawSelectIndentsOn(value, 0, 0);
  drawSetBasesStatusClear();
  clientMutexWaitFor();
  total = screenNumBases();
  for (count=1;count<=total;count++) {
    ba = screenBaseAlliance(count);
    drawStatusBase(count, ba, showBasesStatus);
  }
  clientMutexRelease();
  drawCopyBasesStatus();	  
  /* Draw Pillbox Status */
  clientMutexWaitFor();
  drawSetPillsStatusClear();
  total = screenNumPills();
  for (count=1;count<=total;count++) {
    ba = screenPillAlliance(count);
    drawStatusPillbox(count, ba, showPillsStatus);
  }
  clientMutexRelease();
  drawCopyPillsStatus();

    /* Draw Tanks Status */
  drawSetTanksStatusClear();
  clientMutexWaitFor();
  total = screenGetNumPlayers();
  for (count=1;count<=MAX_TANKS;count++) {
    ba = screenTankAlliance(count);
    drawStatusTank(count, ba);
  }
  clientMutexRelease();
  drawCopyTanksStatus();

  /* Draw tank Bars */
  clientMutexWaitFor();
  screenGetTankStats(&shells,&mines, &armour, &trees);
  drawStatusTankBars(0, 0, shells, mines, armour, trees);
  screenGetMessages(top,bottom);
  drawMessages(0, 0, top, bottom);
  screenGetKillsDeaths(&kills, &deaths);
  drawKillsDeaths(0, 0, kills, deaths);
  drawStatusBaseBars(0, 0, 0, 0, 0, TRUE);
  screenGetLgmStatus(&lgmIsOut, &lgmIsDead, &lgmAngle);
  if (lgmIsOut == TRUE) {
    drawSetManStatus(lgmIsDead, lgmAngle, FALSE);
  } 
  clientMutexRelease();
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
  SDL_Surface *lpTextSurface;
  SDL_Rect dest;   /* The dest square to draw it */
  BYTE zf;     /* Scaling factor */
  
  zf = 1; //FIXME: windowGetZoomFactor();
  lpTextSurface = TTF_RenderText_Shaded(lpFont, top, white, black);
  if (lpTextSurface) {
    dest.x = xValue + zf * MESSAGE_LEFT;
    dest.y = yValue + zf * MESSAGE_TOP;
    dest.w = lpTextSurface->w;
    dest.h = lpTextSurface->h;
    SDL_BlitSurface(lpTextSurface, NULL, lpScreen, &dest);
    SDL_UpdateRects(lpScreen, 1, &dest);
    SDL_FreeSurface(lpTextSurface);
  }
  lpTextSurface = TTF_RenderText_Shaded(lpFont, bottom, white, black);
  if (lpTextSurface) {
    dest.x = xValue + zf * MESSAGE_LEFT;
    dest.y = yValue + zf * MESSAGE_TOP + (zf * MESSAGE_TEXT_HEIGHT);
    dest.w = lpTextSurface->w;
    dest.h = lpTextSurface->h;
    SDL_BlitSurface(lpTextSurface, NULL, lpScreen, &dest);
    SDL_UpdateRects(lpScreen, 1, &dest);
    SDL_FreeSurface(lpTextSurface);
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
void drawDownloadScreen(bool justBlack) {
  BYTE zoomFactor;        /* scaling factor */
  SDL_Rect output;        /* Output RECT */
  SDL_Rect in;
  
  zoomFactor = 1; //FIXME windowGetZoomFactor();
  /* Fill the area black */
  SDL_FillRect(lpBackBuffer, NULL, SDL_MapRGB(lpBackBuffer->format, 0, 0, 0)); 
  /* Fill the downloaded area white */
  if (justBlack == FALSE) {
    output.x = 0;
    output.y = 0;
    output.h = netGetDownloadPos();
    output.w = (MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y) + TILE_SIZE_Y;
    SDL_FillRect(lpBackBuffer, &output, SDL_MapRGB(lpBackBuffer->format, 255, 255, 255));
}

  /* Copy the back buffer to the window */
  SDL_UpdateRect(lpBackBuffer, 0, 0, 0, 0);
  in.x = (zoomFactor * TILE_SIZE_X) + 2;
  in.y = (zoomFactor * TILE_SIZE_Y) + 1;
  in.w = zoomFactor * MAIN_SCREEN_SIZE_X * TILE_SIZE_X;
  in.h = zoomFactor * MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y;
  output.x =  (zoomFactor * MAIN_OFFSET_X);
  output.y = zoomFactor * MAIN_OFFSET_Y;
  output.w = in.w;
  output.h = in.h;

  SDL_BlitSurface(lpBackBuffer, &in, lpScreen, &output);
  SDL_UpdateRect(lpScreen, output.x, output.y, output.w, output.h);
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
  SDL_Surface *lpTextSurface;
  SDL_Rect dest;   /* The dest square to draw it */
  BYTE zf;     /* Scaling factor */
  char str[16]; /* Holds the charectors to print */

  zf = 1; //FIXME: windowGetZoomFactor();
  sprintf(str, "%d", kills);

  lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
  if (lpTextSurface) {
    dest.x = xValue + zf * STATUS_KILLS_LEFT;
    dest.y = yValue + zf * STATUS_KILLS_TOP + zf;
    dest.w = lpTextSurface->w;
    dest.h = lpTextSurface->h;
    SDL_BlitSurface(lpTextSurface, NULL, lpScreen, &dest);
    SDL_UpdateRects(lpScreen, 1, &dest);
    SDL_FreeSurface(lpTextSurface);
  }
  sprintf(str, "%d", deaths);
  lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
  if (lpTextSurface) {
    dest.x = xValue + zf * STATUS_DEATHS_LEFT;
    dest.y = yValue + zf * STATUS_DEATHS_TOP + zf;
    dest.w = lpTextSurface->w;
    dest.h = lpTextSurface->h;
    SDL_BlitSurface(lpTextSurface, NULL, lpScreen, &dest);
    SDL_UpdateRects(lpScreen, 1, &dest);
    SDL_FreeSurface(lpTextSurface);
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
