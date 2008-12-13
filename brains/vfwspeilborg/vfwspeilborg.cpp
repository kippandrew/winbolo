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
*Name:          vfwSpielborg
*Filename:      BrainTemplate.c
*Author:        John Morrison
*Creation Date: 12/8/00
*Last Modified: 12/8/00
*Purpose:
* Video for Windows version of Spielborg.
* Based on Spielborg for Bolo by David Xavier Clancy
* Includes vfw code by Tobias Johansson
*********************************************************/

/* Includes */

#include "brain.h"
#include  <windows.h>
#include <ddraw.h>
#define DIRECTINPUT_VERSION 0x0300
#include <dinput.h>
#include "resource.h"
#include "DirectXAVI.h"

/* Defines */

/* Input Defines*/
#define PRESSED 0x80
#define KEY_STATES 256
/* Default keys if loading preferences screws up */
#define DEFAULT_FORWARD DIK_Q
#define DEFAULT_BACKWARD DIK_A
#define DEFAULT_LEFT DIK_O
#define DEFAULT_RIGHT DIK_P
#define DEFAULT_SHOOT DIK_SPACE
#define DEFAULT_LAY_MINE DIK_TAB

/* Values of the arrow keys, return */
#define DEFAULT_SCROLLLEFT 37
#define DEFAULT_SCROLLUP 38
#define DEFAULT_SCROLLRIGHT 39
#define DEFAULT_SCROLLDOWN 40
#define DEFAULT_TANKVIEW 13
#define DEFAULT_PILLVIEW 222
/* + and - Keys */
#define DEFAULT_SCROLL_GUNINCREASE 187
#define DEFAULT_SCROLL_GUNDECREASE 189
#define DEFAULT_SCROLL_PILLVIEW 222

#define PREFERENCE_FILE "WinBolo.ini"


#define FILENAME_MAX 256



#define MAIN_SCREEN_SIZE_X 15 /*16 is bolo window size  times 16 pixels */
#define MAIN_SCREEN_SIZE_Y 15 /*18 is bolo window size  times 16 pixels */


/* Size of the back buffer */
#define MAIN_BACK_BUFFER_SIZE_X (MAIN_SCREEN_SIZE_X)
#define MAIN_BACK_BUFFER_SIZE_Y (MAIN_SCREEN_SIZE_Y)
#define TILE_SIZE_X 16
#define TILE_SIZE_Y 16

#define MAIN_OFFSET_X 81
#define MAIN_OFFSET_Y 18


/* Typestructure that holds the keys */
typedef struct {
  /* These are DIK_??? defines */
  int kiForward;    /* Tank accelerate */
  int kiBackward;   /* Tank decelerate */
  int kiLeft;       /* Tank left */
  int kiRight;      /* Tank right */
  int kiShoot;      /* Tank shooting */
  int kiLayMine;     /* Tank lay mine */
  /* The rest are all WM_KEYDOWN (virtual-key codes) */
  int kiGunIncrease; /* Increase gunsight length */
  int kiGunDecrease; /* Decrease gunsight length */
  int kiTankView;    /* Center on tank */
  int kiPillView;    /* Pill view */
  int kiScrollUp;    /* Scroll up */
  int kiScrollDown;  /* Scroll down */
  int kiScrollLeft;  /* Scroll left */
  int kiScrollRight; /* Scroll right */
} keyItems;


/* What we want to call our menu entry */
#define BRAIN_MENU_NAME "vfwSpielborg" 
/* Generic brain error */
#define BRAIN_ERROR -1

#define MENU_CHECK_WIN32(X) ((X==0) ? (MF_UNCHECKED) : (MF_CHECKED))


/* Globals */
HINSTANCE brainInst;      /* This Brains instance */
HMENU brainMenu = NULL;   /* Our Menu             */
HWND hAboutDialog = NULL; /* About Dialogbox      */
HWND hBoloWnd = NULL;     /* Bolo Window Handle   */
char fileName[FILENAME_MAX];  /* The filename and path that should saved */
Boolean saving, opt1, opt2;


/* Direct Input Stuff */
LPDIRECTINPUT lpDI = NULL;
LPDIRECTINPUTDEVICE lpDIDKeyboard = NULL;
keyItems keys;


/* Direct Draw Stuff */
LPDIRECTDRAW lpDD = NULL;
LPDIRECTDRAWSURFACE lpDDSPrimary = NULL;
LPDIRECTDRAWSURFACE lpDDSBackBuffer = NULL;
HDC hMemDC = NULL;


/* Functions */


/*************************************************************************
 *
 * CopyScreenToBitmap()
 *
 * Parameter:
 *
 * LPRECT lpRect    - specifies the window
 *
 * Return Value:
 *
 * HDIB             - identifies the device-dependent bitmap
 *
 * Description:
 *
 * This function copies the specified part of the screen to a device-
 * dependent bitmap.
 *
 *
 ************************************************************************/
HBITMAP CopyScreenToBitmap(LPRECT lpRect) {
    HDC         hScrDC, hMemDC;         // screen DC and memory DC
    HBITMAP     hBitmap, hOldBitmap;    // handles to deice-dependent bitmaps
    int         nX, nY, nX2, nY2;       // coordinates of rectangle to grab
    int         nWidth, nHeight;        // DIB width and height
    int         xScrn, yScrn;           // screen resolution

    // check for an empty rectangle

    if (IsRectEmpty(lpRect))
      return NULL;

    // create a DC for the screen and create
    // a memory DC compatible to screen DC
    
    hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    hMemDC = CreateCompatibleDC(hScrDC);

    // get points of rectangle to grab

    nX = lpRect->left;
    nY = lpRect->top;
    nX2 = lpRect->right;
    nY2 = lpRect->bottom;

    // get screen resolution

    xScrn = GetDeviceCaps(hScrDC, HORZRES);
    yScrn = GetDeviceCaps(hScrDC, VERTRES);

    //make sure bitmap rectangle is visible

    if (nX < 0)
        nX = 0;
    if (nY < 0)
        nY = 0;
    if (nX2 > xScrn)
        nX2 = xScrn;
    if (nY2 > yScrn)
        nY2 = yScrn;

    nWidth = nX2 - nX;
    nHeight = nY2 - nY;

    // create a bitmap compatible with the screen DC
    hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

    // select new bitmap into memory DC
    hOldBitmap = (HBITMAP) SelectObject(hMemDC, hBitmap);

    // bitblt screen DC to memory DC
    BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

    // select old bitmap back into memory DC and get handle to
    // bitmap of the screen
    
    hBitmap = (HBITMAP) SelectObject(hMemDC, hOldBitmap);

    // clean up

    DeleteDC(hScrDC);
    DeleteDC(hMemDC);

    // return handle to the bitmap

    return hBitmap;
}


/* About dialog box callback. We use dialogbox rather then a simple
   MessageBox because a MessageBox stops WinBolo from continuing 
   until the user has clicked OK in the MessageBox. */
BOOL CALLBACK aboutDialogCallback(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_COMMAND && LOWORD(wParam) == IDOK) {
    /* OK Pressed. Hide the window till next time it is called */
    ShowWindow(hAboutDialog, SW_HIDE);
  }
  return FALSE;
}


#define OPEN_FILE_FILTERS "AVI Files\0*.AVI\0All Files\0*.*\0\0"
#define DEFAULT_FILE_EXTENSION "avi\0"

Boolean going = FALSE;

void setPath() {
  OPENFILENAME ofn;              /* File name to be saved */

  /* Fill the open file dialog box with stuff */
  memset( &ofn, 0, sizeof(OPENFILENAME));
  ofn.lStructSize	= sizeof(OPENFILENAME);
  ofn.hwndOwner	= NULL;
  ofn.hInstance	= brainInst;
  ofn.lpstrFilter	= OPEN_FILE_FILTERS;
  ofn.lpstrCustomFilter = NULL;
  ofn.nFilterIndex	= 0;
  ofn.nMaxCustFilter = 0;

  ofn.lpstrFile	= fileName;
  ofn.nMaxFile	= sizeof(fileName);
	ofn.lpstrFileTitle	= NULL;
	ofn.nMaxFileTitle	= 0;
 	ofn.lpstrInitialDir = NULL;
  ofn.lpstrTitle	= NULL;
  ofn.Flags		= OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
  ofn.lpstrDefExt = DEFAULT_FILE_EXTENSION;
  
  GetSaveFileName( &ofn ); /* Do the dialog box */
}



void doCapture() {
  static int e5 = 0;
  HBITMAP hBitmap;
  HDC hSurfDC;
  RECT rect;
  HRESULT res;

  if (going == FALSE || opt2 == 1) {
	  return;
  }
  e5++;
  if (e5 < 5) {
	return;
  }
  e5 = 0;
	  
  if (GetForegroundWindow() != hBoloWnd) {
	  return;
  }
  GetClientRect( hBoloWnd, &rect );
  /* Convert the coordinates from client relative to screen relative. */
  
  ClientToScreen(hBoloWnd, (LPPOINT) &rect );


  rect.left = rect.left + MAIN_OFFSET_X;
  rect.top = rect.top + MAIN_OFFSET_Y;
  rect.right = rect.left + (MAIN_SCREEN_SIZE_X * TILE_SIZE_X);
  rect.bottom = rect.top + (MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y);

  hBitmap = CopyScreenToBitmap(&rect);
  hMemDC = CreateCompatibleDC(NULL);
  SelectObject(hMemDC, hBitmap);
  lpDDSBackBuffer->Restore();

  res = lpDDSBackBuffer->GetDC(&hSurfDC);
  if (res == DD_OK) {
    BitBlt(hSurfDC, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);
    res = lpDDSBackBuffer->ReleaseDC(hSurfDC);
    res = WriteFrame(lpDDSBackBuffer);
  }
  if (hMemDC != NULL) {
    DeleteDC(hMemDC);
    hMemDC = NULL;
  }
  
  DeleteObject(hBitmap);  
}

/* Sample brain think call. Returns TRUE. 
   Add brain details here. */
Boolean brainThink(BrainInfo *info, keyItems *setKeys) {
  Boolean proceed;
  HRESULT res;            /* DI Function returns */
  char keys[KEY_STATES];  /* Keyboard states */

  
  *(info->holdkeys) = 0;
  proceed = TRUE;
	if (GetForegroundWindow() == hBoloWnd) {
	  if (saving == TRUE) {
		// Do Capture
	    doCapture();
	  }
	  lpDIDKeyboard->Acquire();
    res = lpDIDKeyboard->GetDeviceState(sizeof(keys), &keys);
    if (res == DIERR_NOTACQUIRED || res == DIERR_INPUTLOST) {
      if (FAILED(lpDIDKeyboard->Acquire())) {
        proceed = FALSE;
      } else {
        if (FAILED(lpDIDKeyboard->GetDeviceState(sizeof(keys), &keys))) {
          proceed = FALSE;
        }
      }
    } else if (res == DI_OK) {
      proceed = TRUE;
    }

     /* Get the tabkButtons here if it is OK to proceed */
    if (proceed == TRUE) {
      /* Add the Right Shift and control to left as well if required */
      if (keys[DIK_RSHIFT] & PRESSED) {
        keys[DIK_LSHIFT] = (char) PRESSED;
      }
      if (keys[DIK_RCONTROL] & PRESSED) {
        keys[DIK_LCONTROL] = (char) PRESSED;
      }
      if (keys[DIK_NUMPADENTER] & PRESSED) {
        keys[DIK_RETURN] = (char) PRESSED;
      }

      /* Now set up the tank buttons */
      if (keys[(setKeys->kiForward)] & PRESSED) {
        setkey(*(info->holdkeys), KEY_faster);
      }
      if (keys[(setKeys->kiBackward)] & PRESSED) {
        setkey(*(info->holdkeys), KEY_slower);
      }
      if (keys[(setKeys->kiLeft)] & PRESSED) {
        setkey(*(info->holdkeys), KEY_turnleft);
      }
      if (keys[(setKeys->kiRight)] & PRESSED) {
        setkey(*(info->holdkeys), KEY_turnright);
      }

      /* Get whether the tank is lay a mine */
      if (keys[(setKeys->kiLayMine)] & PRESSED) {
        setkey(*(info->holdkeys), KEY_dropmine);
      }

        /* Gunsight Check */
      if (keys[(setKeys->kiGunIncrease)] & PRESSED) {
        setkey(*(info->holdkeys), KEY_morerange);

      } 
      if (keys[(setKeys->kiGunDecrease)] & PRESSED) {
        setkey(*(info->holdkeys), KEY_lessrange);
      } 
      if (keys[(setKeys->kiShoot)] & PRESSED) {
        setkey(*(info->holdkeys), KEY_shoot);
      } 

      if (keys[(setKeys->kiTankView)] & PRESSED) {
        setkey(*(info->holdkeys), KEY_TankView);
      } 
      if (keys[(setKeys->kiPillView)] & PRESSED) {
        setkey(*(info->holdkeys), KEY_PillView);
      } 

    }
  }
  return TRUE;
}

/* A sample brainOpen function. It shows how to add a menu entry 
   onto WinBolo's menu and creates an about dialogbox. Returns 
   success of the opertaion. */
Boolean brainOpen(keyItems *keys) {
  Boolean returnValue; /* Value to return          */
  HWND hMainWnd;       /* Handle to WinBolo window */
  HMENU hMenu;         /* Handle to WinBolo's menu */
  char buff[FILENAME_MAX]; /* Read Buffer               */
  char def[FILENAME_MAX];  /* The default value      */
  HRESULT res;            /* Direct Draw Function returns */
  DDSURFACEDESC primDesc; /* Surface description */
  DDPIXELFORMAT ddpf;     /* DD Pixel Format to get green */
  /* Palette Checking */
  DDSURFACEDESC   ddsd;


  returnValue = TRUE;
  strcpy(fileName, "Spielborg.avi");
  brainMenu = NULL;
  hMainWnd = GetActiveWindow();
  hMenu = GetMenu(hMainWnd);


    /* Check we could get handles to the WinBolo window */
  if (hMenu == NULL || hMainWnd == NULL) {
    returnValue = FALSE;
  }
  
  /* Find the window */
  if (returnValue == TRUE) {
    hBoloWnd = GetActiveWindow();
	if (hBoloWnd == NULL) {
	  returnValue = FALSE;
	}
  }

  /* Load our resouce */
  if (returnValue == TRUE) {
    brainMenu = LoadMenu(brainInst, MAKEINTRESOURCE(IDR_BRAIN_MENU));
    if (brainMenu == NULL) {
      returnValue = FALSE;
    }
  }

  /* Added our menu */
  if (returnValue == TRUE) {
    AppendMenu(hMenu, MF_POPUP, (UINT) brainMenu, BRAIN_MENU_NAME);
    DrawMenuBar(hMainWnd);
  }

  /* Load about box */
  if (returnValue == TRUE) {
    hAboutDialog = CreateDialog(brainInst, MAKEINTRESOURCE(IDD_ABOUT), NULL, aboutDialogCallback);
    if (hAboutDialog == NULL) {
      returnValue = FALSE;
    }
  }
  

  /* Direct Input Setup */
  if (returnValue == TRUE) {
    if (FAILED(DirectInputCreate(brainInst, DIRECTINPUT_VERSION, &lpDI, NULL))) {
	    returnValue = FALSE;
  	}
  }
  /* Create the Keyboard Device */
  if (returnValue == TRUE) {
    if (FAILED(lpDI->CreateDevice(GUID_SysKeyboard, &lpDIDKeyboard, NULL))) {
      returnValue = FALSE;
    }
  }


  /* Set the Data Format for the Keyboard */
  if (returnValue == TRUE) {
    if (FAILED(lpDIDKeyboard->SetDataFormat(&c_dfDIKeyboard))) {
      returnValue = FALSE;
    }
  }


  /* Set the Data Format for the Keyboard */
  if (returnValue == TRUE) {
    if (FAILED(lpDIDKeyboard->SetCooperativeLevel(hBoloWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))) {
      returnValue = FALSE;
    }
  }

  saving = FALSE;
  opt1 = 0;
  opt2 = 0;
  going = FALSE;
  if (returnValue == TRUE) {
    /* Load key preferences */
    itoa(DEFAULT_FORWARD, def, 10);
    GetPrivateProfileString("KEYS", "Forward", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiForward = atoi(buff);
    itoa(DEFAULT_BACKWARD, def, 10);
    GetPrivateProfileString("KEYS", "Backwards", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiBackward = atoi(buff);
    itoa(DEFAULT_LEFT, def, 10);
    GetPrivateProfileString("KEYS", "Left", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiLeft = atoi(buff);
    itoa(DEFAULT_RIGHT, def, 10);
    GetPrivateProfileString("KEYS", "Right", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiRight = atoi(buff);
    itoa(DEFAULT_SHOOT, def, 10);
    GetPrivateProfileString("KEYS", "Shoot", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiShoot = atoi(buff);
    itoa(DEFAULT_LAY_MINE, def, 10);
    GetPrivateProfileString("KEYS", "Lay Mine", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiLayMine = atoi(buff);
    itoa(DEFAULT_SCROLL_GUNINCREASE, def, 10);
    GetPrivateProfileString("KEYS", "Increase Range", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiGunIncrease = atoi(buff);
    itoa(DEFAULT_SCROLL_GUNDECREASE, def, 10);
    GetPrivateProfileString("KEYS", "Decrease Range", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiGunDecrease = atoi(buff);
    itoa(DEFAULT_TANKVIEW, def, 10);
    GetPrivateProfileString("KEYS", "Tank View", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiTankView = atoi(buff);
    itoa(DEFAULT_PILLVIEW, def, 10);
    GetPrivateProfileString("KEYS", "Pill View", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiPillView = atoi(buff);
    itoa(DEFAULT_SCROLLUP, def, 10);
    GetPrivateProfileString("KEYS", "Scroll Up", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiScrollUp = atoi(buff);
    itoa(DEFAULT_SCROLLDOWN, def, 10);
    GetPrivateProfileString("KEYS", "Scroll Down", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiScrollDown = atoi(buff);
    itoa(DEFAULT_SCROLLLEFT, def, 10);
    GetPrivateProfileString("KEYS", "Scroll Left", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiScrollLeft = atoi(buff);
    itoa(DEFAULT_SCROLLRIGHT, def, 10);
    GetPrivateProfileString("KEYS", "Scroll Right", def, buff, FILENAME_MAX, PREFERENCE_FILE);
    keys->kiScrollRight = atoi(buff);
  }


  /* Direct Draw Stuff */
  if (returnValue == TRUE) {
    res = DirectDrawCreate(NULL, &lpDD,NULL);
    if (FAILED(res)) {
      returnValue = FALSE;
	  }
  }
  /* Set the co-op level */
  if (returnValue == TRUE) {
    res = lpDD->SetCooperativeLevel(NULL,DDSCL_NORMAL);
    if (FAILED(res)) {
      returnValue = FALSE;
    }
  }

  /* Create the primary surface */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS;
    primDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    res = lpDD->CreateSurface(&primDesc, &lpDDSPrimary, NULL);
    if (FAILED(res)) {
      returnValue = FALSE;
    }
  }

  /* Get the pixel format */
  ddpf.dwSize = sizeof(ddpf);
  res = lpDDSPrimary->GetPixelFormat(&ddpf);
  if (FAILED(res)) {
    returnValue = FALSE;
  }

  /* Palette Checking Stuff */
  ZeroMemory( &ddsd, sizeof( ddsd ) );
  ddsd.dwSize = sizeof( ddsd );
  if (FAILED(lpDDSPrimary->GetSurfaceDesc(&ddsd))) {
    returnValue = FALSE;
  }
  
  if (( ddsd.ddpfPixelFormat.dwRGBBitCount < 16 )) { /* FIXME: Was also (ddsd.ddpfPixelFormat.dwFlags != DDPF_RGB) || but taken out as it stops matrox G4's from working in 32bit colour mode */
    returnValue = FALSE;
  }

  /* Create the back buffer surface */
  if (returnValue == TRUE) {
    ZeroMemory(&primDesc, sizeof (primDesc));
    primDesc.dwSize = sizeof (primDesc);
    primDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    primDesc.dwWidth = MAIN_BACK_BUFFER_SIZE_X * TILE_SIZE_X;
    primDesc.dwHeight = MAIN_BACK_BUFFER_SIZE_Y * TILE_SIZE_Y;
    primDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    res = lpDD->CreateSurface(&primDesc, &lpDDSBackBuffer, NULL);
    if (FAILED(res)) {
      returnValue = FALSE;
    }
  }


  return returnValue;
}

/* This is the brains closing function. It should free all 
   resources it uses as well as remove and menus it creates */
void brainClose(void) {
  HWND hMainWnd; /* Handle to WinBolo window */
  HMENU hMenu;   /* Handle to WinBolo's menu */

  if (brainMenu != NULL) {
    /* We added a menu so we shoudl remove it */
    hMainWnd = GetActiveWindow();
    hMenu = GetMenu(hMainWnd);
    DestroyMenu(brainMenu);
    RemoveMenu(hMenu, GetMenuItemCount(hMenu)-1, MF_BYPOSITION);
    DrawMenuBar(hMainWnd);
    brainMenu = NULL;
  }

  if (hAboutDialog != NULL) {
    /* Remove the about dialog box */
    EndDialog(hAboutDialog, TRUE);
    DestroyWindow(hAboutDialog);
  }
  
  /* Direct Input Shutdown */
  if (lpDIDKeyboard != NULL) {
    lpDIDKeyboard->Release();
	  lpDIDKeyboard = NULL;
  }
  if (lpDI != NULL) {
    lpDI->Release();
  	lpDI = NULL;
  }
  if (saving == TRUE) {
  	saving = FALSE;
	  CloseAVIFile();
  }

  /* Direct Draw Shutdown */
  if (lpDDSBackBuffer != NULL) {
    lpDDSBackBuffer->Release();
    lpDDSBackBuffer = NULL;
  }
  if (lpDDSPrimary != NULL) {
    lpDDSPrimary->Release();
    lpDDSPrimary = NULL;
  }
  if (lpDD != NULL) {
    lpDD->Release();
    lpDD = NULL;
  }
}

/* This is a sample menu handler function. */
void brainMenuHandler(const BrainInfo *brainInfo) {
  HRESULT res;
  switch (brainInfo->menu_item) {
  case ID_BRAINTEMPLATE_ABOUT:
    /* About box */
    ShowWindow(hAboutDialog, SW_SHOW);
    SetForegroundWindow(hAboutDialog);
    break;
  case ID_BRAINTEMPLATE_OPTION1:
    // Start/Stop Capturing 
    opt1 ^= 1;
    CheckMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION1, MENU_CHECK_WIN32(opt1));
    if (saving == TRUE) {
    	saving = FALSE;
	    CloseAVIFile();
      EnableMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION2, MF_ENABLED);
      EnableMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION3, MF_GRAYED);
    } else {
      saving = TRUE;
      opt2 = 0;
      CheckMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION3, MENU_CHECK_WIN32(opt2));
      EnableMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION2, MF_GRAYED);
      EnableMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION3, MF_ENABLED);
      res = WriteAVIFile(fileName, lpDDSBackBuffer ,6);
      if (res != 1) {
        saving = FALSE;
        opt1 ^= 1;
        CheckMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION1, MENU_CHECK_WIN32(opt1));
        EnableMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION2, MF_ENABLED);
        EnableMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION3, MF_GRAYED);
      } 
    }
    break;
  case ID_BRAINTEMPLATE_OPTION2:
    // Set Path
    EnableMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION2, MF_GRAYED);
  	setPath();
    EnableMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION2, MF_ENABLED);
    break;
  case ID_BRAINTEMPLATE_OPTION3:
    // Pause
    opt2 ^= 1;
    CheckMenuItem(brainMenu, ID_BRAINTEMPLATE_OPTION3, MENU_CHECK_WIN32(opt2));
    break;
  }
}


/* The exported brain function that gets called.
   Returns 0 if OK or anything else on an error. 
   Remember if you return an error you will not be called again with 
   the BRAIN_CLOSE message */
extern "C"{ __declspec( dllexport ) short BrainMain(const BrainInfo *brainInfo) {
  short returnValue; /* Value to return */
    
  returnValue = 0;
  if (brainInfo->InfoVersion != CURRENT_BRAININFO_VERSION) {
    returnValue = BRAIN_ERROR;
  } else {
    switch (brainInfo->operation) {
    case BRAIN_OPEN:
      /* Do brain startup. */
      if (brainOpen(&keys) == FALSE) {
        returnValue = BRAIN_ERROR;
      }
      break;
    case BRAIN_CLOSE:
      /* Do brain shutdown. */
      brainClose();
      break;
    case BRAIN_THINK:
      /* Do brain think call */
      if (brainThink((BrainInfo *) brainInfo, &keys) == FALSE) {
        returnValue = BRAIN_ERROR;
      }
      break;
    case BRAIN_MENU:
      /* Do brain menu action. */
      brainMenuHandler(brainInfo);
      break;
    default:
      /* This should never be reached so it must be an unexpected error.
         Shutdown and return an error. */
      returnValue = BRAIN_ERROR;
    }
  }

  if (returnValue != 0) {
    /* We must of encounted an error so shutdown */
    brainClose();
  }
  return returnValue;
}
}

/* Standard Win32 DLL Main entry point. It is useful to 
   save hInst for resource loading */
BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved) {
  brainInst = (HINSTANCE) hInst;
  return TRUE;
}
