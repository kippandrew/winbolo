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
*Name:          Input
*Filename:      input.c
*Author:        John Morrison
*Creation Date: 16/12/98
*Last Modified:   1/5/00
*Purpose:
*  Keyboard and mouse routines (ie Direct Input routines)
*********************************************************/

#define DIRECTINPUT_VERSION 0x0300

#include <windows.h>
#include <dinput.h>
#include "..\..\bolo\global.h"
#include "..\..\bolo\backend.h"
#include "..\winbolo.h"
#include "..\lang.h"
#include "..\resource.h"
#include "..\input.h"

#define PRESSED 0x80
#define KEY_STATES 256


tankButton tb;
LPDIRECTINPUT lpDI = NULL;
LPDIRECTINPUTDEVICE lpDIDKeyboard = NULL;
static BYTE scrollKeyCount = 0; /* Used for screen scrolling */

/*********************************************************
*NAME:          inputSetup
*AUTHOR:        John Morrison
*CREATION DATE: 16/12/98
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Sets up input systems, direct draw structures etc.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
* appInst - Handle to the application
* appWnd  - Main Window Handle
*********************************************************/
bool inputSetup(HINSTANCE appInst, HWND appWnd) {
  bool returnValue; /* Value to return */

  returnValue = TRUE;
  scrollKeyCount = 0;

  if (FAILED(DirectInputCreate(appInst, DIRECTINPUT_VERSION, &lpDI, NULL))) {
    MessageBoxA(NULL, langGetText(STR_GAMEFRONT_INPUTERR_CREATE), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    returnValue = FALSE;
  }
  /* Create the Keyboard Device */
  if (returnValue == TRUE) {
    if (FAILED(lpDI->lpVtbl->CreateDevice(lpDI, &GUID_SysKeyboard, &lpDIDKeyboard, NULL))) {
      MessageBoxA(NULL, langGetText(STR_GAMEFRONT_INPUTERR_CREATE), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }


  /* Set the Data Format for the Keyboard */
  if (returnValue == TRUE) {
    if (FAILED(lpDIDKeyboard->lpVtbl->SetDataFormat(lpDIDKeyboard, &c_dfDIKeyboard))) {
      MessageBoxA(NULL, langGetText(STR_GAMEFRONT_INPUTERR_DATAFORMAT), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }

  /* Set the Data Format for the Keyboard */
  if (returnValue == TRUE) {
    if (FAILED(lpDIDKeyboard->lpVtbl->SetCooperativeLevel(lpDIDKeyboard, appWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))) {
      MessageBoxA(NULL, langGetText(STR_GAMEFRONT_INPUTERR_COOPLEVEL), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
      returnValue = FALSE;
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          inputCleanup
*AUTHOR:        John Morrison
*CREATION DATE: 13/12/98
*LAST MODIFIED: 13/12/98
*PURPOSE:
*  Destroys and cleans up input systems, direct draw 
*  structures etc.
*
*ARGUMENTS:
*
*********************************************************/
void inputCleanup(void) {
  if (lpDIDKeyboard != NULL) {
    lpDIDKeyboard->lpVtbl->Release(lpDIDKeyboard);
  }
  if (lpDI != NULL) {
    lpDI->lpVtbl->Release(lpDI);
  }
}

/*********************************************************
*NAME:          inputGetKeys
*AUTHOR:        John Morrison
*CREATION DATE: 17/12/98
*LAST MODIFIED:   1/5/00
*PURPOSE:
*  Gets the current Buttons that are being pressed.
*  Returns tank buttons being pressed.
*
*ARGUMENTS:
*     hWnd - The main window
*  setKeys - Structure that holds the key settings
*  isMenu  - True if we are in a menu
*********************************************************/
tankButton inputGetKeys(HWND hWnd, keyItems *setKeys, bool isMenu) {
  static BYTE gunsightKeyCount = 0; /* Used for gunsight movement */
  HRESULT res;            /* DI Function returns */
  char keys[KEY_STATES];  /* Keyboard states */
  bool proceed;           /* Ok to proceed */

  proceed = TRUE;

  /* Check message window doesn't have focus */
  if (GetForegroundWindow() != hWnd || isMenu == TRUE) {
    return TNONE;
  }

  /* Check the status of the device */
  lpDIDKeyboard->lpVtbl->Acquire(lpDIDKeyboard);
  res = lpDIDKeyboard->lpVtbl->GetDeviceState(lpDIDKeyboard, sizeof(keys), keys);
  if (res == DIERR_NOTACQUIRED || res == DIERR_INPUTLOST) {
    if (FAILED(lpDIDKeyboard->lpVtbl->Acquire(lpDIDKeyboard))) {
      proceed = FALSE;
    } else {
      if (FAILED(lpDIDKeyboard->lpVtbl->GetDeviceState(lpDIDKeyboard, sizeof(keys), keys))) {
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
    if (keys[(setKeys->kiForward)] & PRESSED &&  keys[(setKeys->kiRight)] & PRESSED) {
      tb = TRIGHTACCEL;
    } else if (keys[(setKeys->kiForward)] & PRESSED &&  keys[(setKeys->kiLeft)] & PRESSED) {
      tb = TLEFTACCEL;
    } else if (keys[(setKeys->kiBackward)] & PRESSED &&  keys[(setKeys->kiLeft)] & PRESSED) {
      tb = TLEFTDECEL;
    } else if (keys[(setKeys->kiBackward)] & PRESSED &&  keys[(setKeys->kiRight)] & PRESSED) {
      tb = TRIGHTDECEL;
    } else if (keys[(setKeys->kiForward)] & PRESSED) {
      tb = TACCEL;
    } else if (keys[(setKeys->kiBackward)] & PRESSED) {
      tb = TDECEL;
    } else if (keys[(setKeys->kiLeft)] & PRESSED) {
      tb = TLEFT;
    } else if (keys[(setKeys->kiRight)] & PRESSED) {
      tb = TRIGHT;
    } else {
      tb = TNONE;
    }

    /* Get whether the tank is lay a mine */
    if (keys[(setKeys->kiLayMine)] & PRESSED) {
      screenTankLayMine();
    }

    scrollKeyCount++;
    if (scrollKeyCount >= INPUT_SCROLL_WAIT_TIME && isMenu == FALSE) {
      scrollKeyCount = 0;
      /* Scroll Check */
      if (keys[(setKeys->kiScrollUp)] & PRESSED) {
        screenUpdate(up);
      }
      if (keys[(setKeys->kiScrollDown)] & PRESSED) {
        screenUpdate(down);
      }
      if (keys[(setKeys->kiScrollLeft)] & PRESSED) {
        screenUpdate(left);
      }
      if (keys[(setKeys->kiScrollRight)] & PRESSED) {
        screenUpdate(right);
      }
    }
    gunsightKeyCount++;
    if (gunsightKeyCount >= INPUT_GUNSIGHT_WAIT_TIME && isMenu == FALSE) {
      /* Gunsight Check */
      if (keys[(setKeys->kiGunIncrease)] & PRESSED) {
        screenGunsightRange(TRUE);
        gunsightKeyCount = 0;
      } else if (keys[(setKeys->kiGunDecrease)] & PRESSED) {
        screenGunsightRange(FALSE);
        gunsightKeyCount = 0;
      } else if (gunsightKeyCount > (INPUT_GUNSIGHT_WAIT_TIME + 1)) {
        gunsightKeyCount = INPUT_GUNSIGHT_WAIT_TIME;
      }
    }
  } else {
    tb = TNONE;
  }

  return tb;
}


/*********************************************************
*NAME:          inputScroll
*AUTHOR:        John Morrison
*CREATION DATE: 1/5/00
*LAST MODIFIED: 1/5/00
*PURPOSE:
*  Checks and does scrolling of the window
*
*ARGUMENTS:
*     hWnd - The main window
*  setKeys - Structure that holds the key settings
*  isMenu  - True if we are in a menu
*********************************************************/
void inputScroll(HWND hWnd, keyItems *setKeys, bool isMenu) {
  HRESULT res;            /* DI Function returns */
  char keys[KEY_STATES];  /* Keyboard states */
  bool proceed;           /* Ok to proceed */

  proceed = TRUE;
  /* Check message window doesn't have focus */
  if (GetForegroundWindow() != hWnd || isMenu == TRUE) {
    return;
  }

  /* Check the status of the device */
  res = lpDIDKeyboard->lpVtbl->GetDeviceState(lpDIDKeyboard, sizeof(keys), keys);
  if (res == DIERR_NOTACQUIRED || res == DIERR_INPUTLOST) {
    if (FAILED(lpDIDKeyboard->lpVtbl->Acquire(lpDIDKeyboard))) {
      proceed = FALSE;
    } else {
      if (FAILED(lpDIDKeyboard->lpVtbl->GetDeviceState(lpDIDKeyboard, sizeof(keys), keys))) {
        proceed = FALSE;
      }
    }
  } else if (res == DI_OK) {
    proceed = TRUE;
  }


  /* Get the tabkButtons here if it is OK to proceed */
  if (proceed == TRUE) {
    scrollKeyCount++;
    if (scrollKeyCount >= INPUT_SCROLL_WAIT_TIME && isMenu == FALSE) {
      scrollKeyCount = 0;
      /* Scroll Check */
      if (keys[(setKeys->kiScrollUp)] & PRESSED) {
        screenUpdate(up);
      }
      if (keys[(setKeys->kiScrollDown)] & PRESSED) {
        screenUpdate(down);
      }
      if (keys[(setKeys->kiScrollLeft)] & PRESSED) {
        screenUpdate(left);
      }
      if (keys[(setKeys->kiScrollRight)] & PRESSED) {
        screenUpdate(right);
      }
    }
  }
}


/*********************************************************
*NAME:          inputActivate
*AUTHOR:        John Morrison
*CREATION DATE: 17/12/98
*LAST MODIFIED: 17/12/98
*PURPOSE:
*  Application has just got the focus Aquire the input
*
*ARGUMENTS:
* 
*********************************************************/
void inputActivate(void) {
  lpDIDKeyboard->lpVtbl->Acquire(lpDIDKeyboard);
}

/*********************************************************
*NAME:          inputIsFireKeyPressed
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 14/11/99
*PURPOSE:
*  Returns whether the fire key is pressed
*  Old Comment dates before hWnd was added:
*      *CREATION DATE: 25/12/98
*      *LAST MODIFIED: 31/12/98
*
*ARGUMENTS:
*     hWnd - The main window
*  setKeys - Structure that holds the key settings
*   isMenu - TRUE if we are in a menu
*********************************************************/
bool inputIsFireKeyPressed(HWND hWnd, keyItems *setKeys, bool isMenu) {
  bool returnValue;       /* Value to return */
  HRESULT res;            /* DI Function returns */
  char keys[KEY_STATES];  /* Keyboard states */
  bool proceed;           /* Ok to proceed */
  
  returnValue = FALSE;
  proceed = TRUE;

  if (GetForegroundWindow() != hWnd || isMenu == TRUE) {
    return FALSE;
  }

  /* Check the status of the device */
  res = lpDIDKeyboard->lpVtbl->GetDeviceState(lpDIDKeyboard, sizeof(keys), keys);
  if (res == DIERR_NOTACQUIRED || res == DIERR_INPUTLOST) {
    if (FAILED(lpDIDKeyboard->lpVtbl->Acquire(lpDIDKeyboard))) {
      proceed = FALSE;
    } else {
      if (FAILED(lpDIDKeyboard->lpVtbl->GetDeviceState(lpDIDKeyboard, sizeof(keys), keys))) {
        proceed = FALSE;
      }
    } 
  } else if (res == DI_OK) {
    proceed = TRUE;
  }


  /* Get the fire button state here if it is OK to proceed */
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


    if (keys[(setKeys->kiShoot)] & PRESSED) {
      returnValue = TRUE;
    }
  }
  return returnValue;
}
