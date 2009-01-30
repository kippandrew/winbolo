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
*Name:          WinUtil
*Filename:      winutil.c
*Author:        John Morrison
*Creation Date:  2/2/99
*Last Modified: 13/6/00
*Purpose:
*  Contains useful window functions
*********************************************************/

#include <windows.h>
#define DIRECTINPUT_VERSION 0x0300
#include <dinput.h>
#include "..\..\bolo\global.h"
#include "..\winbolo.h"
#include "..\lang.h"
#include "..\resource.h"
#include "..\winutil.h"

#define SLASH_CHAR '\\'

/*********************************************************
*NAME:          winUtilCenterWindow
*AUTHOR:        John Morrison
*CREATION DATE: 2/2/99
*LAST MODIFIED: 2/2/99
*PURPOSE:
*  Centers the given window on the screen
*
*ARGUMENTS:
*  hWnd   - Handle to the window to center
*********************************************************/
BOOL winUtilCenterWindow(HWND hWnd) {
  RECT    rect;
  int     w, h;
  int     wScreen, hScreen, xNew, yNew;
  HDC     hDC;

  GetWindowRect (hWnd, &rect);
  w = rect.right - rect.left;
  h = rect.bottom - rect.top;

  hDC = GetDC (hWnd);
  wScreen = GetDeviceCaps (hDC, HORZRES);
  hScreen = GetDeviceCaps (hDC, VERTRES);
  ReleaseDC (hWnd, hDC);

  xNew = wScreen/2 - w/2;
  yNew = hScreen/2 - h/2;

  return SetWindowPos (hWnd, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

/*********************************************************
*NAME:          winUtilVirtKeyToString
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* Takes a windows virtual key and places a string 
* identifier into str of it.
*
*ARGUMENTS:
*  value - Virtual key.
*  str   - String value of it.
*********************************************************/
void winUtilVirtKeyToString(int value, char *str) {

  switch (value) {
  case DKS_VK_ESC: 
    strcpy(str, DKS_STR_ESC);
    break;
  case DKS_VK_F1:
    strcpy(str, DKS_STR_F1);
    break;
  case DKS_VK_F2:
    strcpy(str, DKS_STR_F2);
    break;
  case DKS_VK_F3:
    strcpy(str, DKS_STR_F3);
    break;
  case DKS_VK_F4:
    strcpy(str, DKS_STR_F4);
    break;
  case DKS_VK_F5:
    strcpy(str, DKS_STR_F5);
    break;
  case DKS_VK_F6:
    strcpy(str, DKS_STR_F6);
    break;
  case DKS_VK_F7:
    strcpy(str, DKS_STR_F7);
    break;
  case DKS_VK_F8:
    strcpy(str, DKS_STR_F8);
    break;
  case DKS_VK_F9:
    strcpy(str, DKS_STR_F9);
    break;
  case DKS_VK_F10:
    strcpy(str, DKS_STR_F10);
    break;
  case DKS_VK_F11:
    strcpy(str, DKS_STR_F11);
    break;
  case DKS_VK_F12:
    strcpy(str, DKS_STR_F12);
    break;
  case DKS_VK_SCROLLOCK:
    strcpy(str, DKS_STR_SCROLLOCK);
    break;
  case DKS_VK_TILDE:
    strcpy(str, DKS_STR_TILDE);
    break;
  case DKS_VK_1: strcpy(str, DKS_STR_1);
    break;
  case DKS_VK_2:
    strcpy(str, DKS_STR_2);
    break;
  case DKS_VK_3:
    strcpy(str, DKS_STR_3);
    break;
  case DKS_VK_4:
    strcpy(str, DKS_STR_4);
    break;
  case DKS_VK_5:
    strcpy(str, DKS_STR_5);
    break;
  case DKS_VK_6: strcpy(str, DKS_STR_6); break;
  case DKS_VK_7: strcpy(str, DKS_STR_7); break;
  case DKS_VK_8: strcpy(str, DKS_STR_8); break;
  case DKS_VK_9: strcpy(str, DKS_STR_9); break;
  case DKS_VK_0: strcpy(str, DKS_STR_0); break;
  case DKS_VK_MINUS: strcpy(str, DKS_STR_MINUS); break;
  case DKS_VK_EQUALS: strcpy(str, DKS_STR_EQUALS); break;
  case DKS_VK_BACKSLASH: strcpy(str, DKS_STR_BACKSLASH); break;
  case DKS_VK_BACKSPACE: strcpy(str, DKS_STR_BACKSPACE); break;
  case DKS_VK_TAB: strcpy(str, DKS_STR_TAB); break;
  case DKS_VK_Q: strcpy(str, DKS_STR_Q); break;
  case DKS_VK_W: strcpy(str, DKS_STR_W); break;
  case DKS_VK_E: strcpy(str, DKS_STR_E); break;
  case DKS_VK_R: strcpy(str, DKS_STR_R); break;
  case DKS_VK_T: strcpy(str, DKS_STR_T); break;
  case DKS_VK_Y: strcpy(str, DKS_STR_Y); break;
  case DKS_VK_U: strcpy(str, DKS_STR_U); break;
  case DKS_VK_I: strcpy(str, DKS_STR_I); break;
  case DKS_VK_O: strcpy(str, DKS_STR_O); break;
  case DKS_VK_P: strcpy(str, DKS_STR_P); break;
  case DKS_VK_LBRACKET: strcpy(str, DKS_STR_LBRACKET); break;
  case DKS_VK_RBRACKET: strcpy(str, DKS_STR_RBRACKET); break;
  case DKS_VK_RET: strcpy(str, DKS_STR_RET); break;
  case DKS_VK_CAPS: strcpy(str, DKS_STR_CAPS); break;
  case DKS_VK_A: strcpy(str, DKS_STR_A); break;
  case DKS_VK_S: strcpy(str, DKS_STR_S); break;
  case DKS_VK_D: strcpy(str, DKS_STR_D); break;
  case DKS_VK_F: strcpy(str, DKS_STR_F); break;
  case DKS_VK_G: strcpy(str, DKS_STR_G); break;
  case DKS_VK_H: strcpy(str, DKS_STR_H); break;
  case DKS_VK_J: strcpy(str, DKS_STR_J); break;
  case DKS_VK_K: strcpy(str, DKS_STR_K); break;
  case DKS_VK_L: strcpy(str, DKS_STR_L); break;
  case DKS_VK_SEMICOLON: strcpy(str, DKS_STR_SEMICOLON); break;
  case DKS_VK_QUOTES: strcpy(str, DKS_STR_QUOTES); break;
  case DKS_VK_SHIFT: strcpy(str, DKS_STR_SHIFT); break;
  case DKS_VK_Z: strcpy(str, DKS_STR_Z); break;
  case DKS_VK_X: strcpy(str, DKS_STR_X); break;
  case DKS_VK_C: strcpy(str, DKS_STR_C); break;
  case DKS_VK_V: strcpy(str, DKS_STR_V); break;
  case DKS_VK_B: strcpy(str, DKS_STR_B); break;
  case DKS_VK_N: strcpy(str, DKS_STR_N); break;
  case DKS_VK_M: strcpy(str, DKS_STR_M); break;
  case DKS_VK_COMMA: strcpy(str, DKS_STR_COMMA); break;
  case DKS_VK_PERIOD: strcpy(str, DKS_STR_PERIOD); break;
  case DKS_VK_FORWARDSLASH: strcpy(str, DKS_STR_FORWARDSLASH); break;
  case DKS_VK_INS: strcpy(str, DKS_STR_INS); break;
  case DKS_VK_HOME: strcpy(str, DKS_STR_HOME); break;
  case DKS_VK_PGUP: strcpy(str, DKS_STR_PGUP); break;
  case DKS_VK_DEL: strcpy(str, DKS_STR_DEL); break;
  case DKS_VK_END: strcpy(str, DKS_STR_END); break;
  case DKS_VK_PGDN: strcpy(str, DKS_STR_PGDN); break;
  case DKS_VK_LEFT: strcpy(str, DKS_STR_LEFT); break;
  case DKS_VK_RIGHT: strcpy(str, DKS_STR_RIGHT); break;
  case DKS_VK_UP: strcpy(str, DKS_STR_UP); break;
  case DKS_VK_DOWN: strcpy(str, DKS_STR_DOWN); break;
  case DKS_VK_NUMLOCK: strcpy(str, DKS_STR_NUMLOCK); break;
  case DKS_VK_KPDIV: strcpy(str, DKS_STR_KPDIV); break;
  case DKS_VK_KPTIMES: strcpy(str, DKS_STR_KPTIMES); break;
  case DKS_VK_KPMINUS: strcpy(str, DKS_STR_KPMINUS); break;
  case DKS_VK_KPPLUS: strcpy(str, DKS_STR_KPPLUS); break;
  case DKS_VK_KP1: strcpy(str, DKS_STR_KP1); break;
  case DKS_VK_KP2: strcpy(str, DKS_STR_KP2); break;
  case DKS_VK_KP3: strcpy(str, DKS_STR_KP3); break;
  case DKS_VK_KP4: strcpy(str, DKS_STR_KP4); break;
  case DKS_VK_KP5: strcpy(str, DKS_STR_KP5); break;
  case DKS_VK_KP6: strcpy(str, DKS_STR_KP6); break;
  case DKS_VK_KP7: strcpy(str, DKS_STR_KP7); break;
  case DKS_VK_KP8: strcpy(str, DKS_STR_KP8); break;
  case DKS_VK_KP9: strcpy(str, DKS_STR_KP9); break;
  case DKS_VK_KP0: strcpy(str, DKS_STR_KP0); break;
  case DKS_VK_KPDEC: strcpy(str, DKS_STR_KPDEC); break;
  case DKS_VK_SPACE: strcpy(str, DKS_STR_SPACE); break;
  default:
    /* DKS_VK_CTRL: */
    strcpy(str, DKS_STR_CTRL);
    break;
  } 
}

/*********************************************************
*NAME:          winUtilDIKeyToString
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* Takes a Direct Input and key and places a string 
* identifier into str of it.
*
*ARGUMENTS:
*  value - DI Identifier.
*  str   - String value of it.
*********************************************************/
void winUtilDIKeyToString(int value, char *str) {
  switch (value) {
  case DIK_ESCAPE: 
    strcpy(str, DKS_STR_ESC);
    break;
  case DIK_F1:
    strcpy(str, DKS_STR_F1);
    break;
  case DIK_F2:
    strcpy(str, DKS_STR_F2);
    break;
  case DIK_F3:
    strcpy(str, DKS_STR_F3);
    break;
  case DIK_F4:
    strcpy(str, DKS_STR_F4);
    break;
  case DIK_F5:
    strcpy(str, DKS_STR_F5);
    break;
  case DIK_F6:
    strcpy(str, DKS_STR_F6);
    break;
  case DIK_F7:
    strcpy(str, DKS_STR_F7);
    break;
  case DIK_F8:
    strcpy(str, DKS_STR_F8);
    break;
  case DIK_F9:
    strcpy(str, DKS_STR_F9);
    break;
  case DIK_F10:
    strcpy(str, DKS_STR_F10);
    break;
  case DIK_F11:
    strcpy(str, DKS_STR_F11);
    break;
  case DIK_F12:
    strcpy(str, DKS_STR_F12);
    break;
  case DIK_SCROLL:
    strcpy(str, DKS_STR_SCROLLOCK);
    break;
  case DIK_GRAVE:
    strcpy(str, DKS_STR_TILDE);
    break;
  case DIK_1: strcpy(str, DKS_STR_1);
    break;
  case DIK_2:
    strcpy(str, DKS_STR_2);
    break;
  case DIK_3:
    strcpy(str, DKS_STR_3);
    break;
  case DIK_4:
    strcpy(str, DKS_STR_4);
    break;
  case DIK_5:
    strcpy(str, DKS_STR_5);
    break;
  case DIK_6: strcpy(str, DKS_STR_6); break;
  case DIK_7: strcpy(str, DKS_STR_7); break;
  case DIK_8: strcpy(str, DKS_STR_8); break;
  case DIK_9: strcpy(str, DKS_STR_9); break;
  case DIK_0: strcpy(str, DKS_STR_0); break;
  case DIK_MINUS: strcpy(str, DKS_STR_MINUS); break;
  case DIK_EQUALS: strcpy(str, DKS_STR_EQUALS); break;
  case DIK_BACKSLASH: strcpy(str, DKS_STR_BACKSLASH); break;
  case DIK_BACKSPACE: strcpy(str, DKS_STR_BACKSPACE); break;
  case DIK_TAB: strcpy(str, DKS_STR_TAB); break;
  case DIK_Q: strcpy(str, DKS_STR_Q); break;
  case DIK_W: strcpy(str, DKS_STR_W); break;
  case DIK_E: strcpy(str, DKS_STR_E); break;
  case DIK_R: strcpy(str, DKS_STR_R); break;
  case DIK_T: strcpy(str, DKS_STR_T); break;
  case DIK_Y: strcpy(str, DKS_STR_Y); break;
  case DIK_U: strcpy(str, DKS_STR_U); break;
  case DIK_I: strcpy(str, DKS_STR_I); break;
  case DIK_O: strcpy(str, DKS_STR_O); break;
  case DIK_P: strcpy(str, DKS_STR_P); break;
  case DIK_LBRACKET: strcpy(str, DKS_STR_LBRACKET); break;
  case DIK_RBRACKET: strcpy(str, DKS_STR_RBRACKET); break;
  case DIK_RETURN: strcpy(str, DKS_STR_RET); break;
  case DIK_CAPSLOCK: strcpy(str, DKS_STR_CAPS); break;
  case DIK_A: strcpy(str, DKS_STR_A); break;
  case DIK_S: strcpy(str, DKS_STR_S); break;
  case DIK_D: strcpy(str, DKS_STR_D); break;
  case DIK_F: strcpy(str, DKS_STR_F); break;
  case DIK_G: strcpy(str, DKS_STR_G); break;
  case DIK_H: strcpy(str, DKS_STR_H); break;
  case DIK_J: strcpy(str, DKS_STR_J); break;
  case DIK_K: strcpy(str, DKS_STR_K); break;
  case DIK_L: strcpy(str, DKS_STR_L); break;
  case DIK_SEMICOLON: strcpy(str, DKS_STR_SEMICOLON); break;
  case DIK_APOSTROPHE: strcpy(str, DKS_STR_QUOTES); break;
  case DIK_LSHIFT: strcpy(str, DKS_STR_SHIFT); break;
  case DIK_Z: strcpy(str, DKS_STR_Z); break;
  case DIK_X: strcpy(str, DKS_STR_X); break;
  case DIK_C: strcpy(str, DKS_STR_C); break;
  case DIK_V: strcpy(str, DKS_STR_V); break;
  case DIK_B: strcpy(str, DKS_STR_B); break;
  case DIK_N: strcpy(str, DKS_STR_N); break;
  case DIK_M: strcpy(str, DKS_STR_M); break;
  case DIK_COMMA: strcpy(str, DKS_STR_COMMA); break;
  case DIK_PERIOD: strcpy(str, DKS_STR_PERIOD); break;
  case DIK_SLASH: strcpy(str, DKS_STR_FORWARDSLASH); break;
  case DIK_INSERT: strcpy(str, DKS_STR_INS); break;
  case DIK_HOME: strcpy(str, DKS_STR_HOME); break;
  case DIK_PGUP: strcpy(str, DKS_STR_PGUP); break;
  case DIK_DELETE: strcpy(str, DKS_STR_DEL); break;
  case DIK_END: strcpy(str, DKS_STR_END); break;
  case DIK_PGDN: strcpy(str, DKS_STR_PGDN); break;
  case DIK_LEFT: strcpy(str, DKS_STR_LEFT); break;
  case DIK_RIGHT: strcpy(str, DKS_STR_RIGHT); break;
  case DIK_UP: strcpy(str, DKS_STR_UP); break;
  case DIK_DOWN: strcpy(str, DKS_STR_DOWN); break;
  case DIK_NUMLOCK: strcpy(str, DKS_STR_NUMLOCK); break;
  case DIK_NUMPADSLASH: strcpy(str, DKS_STR_KPDIV); break;
  case DIK_NUMPADSTAR: strcpy(str, DKS_STR_KPTIMES); break;
  case DIK_NUMPADMINUS: strcpy(str, DKS_STR_KPMINUS); break;
  case DIK_NUMPADPLUS: strcpy(str, DKS_STR_KPPLUS); break;
  case DIK_NUMPAD1: strcpy(str, DKS_STR_KP1); break;
  case DIK_NUMPAD2: strcpy(str, DKS_STR_KP2); break;
  case DIK_NUMPAD3: strcpy(str, DKS_STR_KP3); break;
  case DIK_NUMPAD4: strcpy(str, DKS_STR_KP4); break;
  case DIK_NUMPAD5: strcpy(str, DKS_STR_KP5); break;
  case DIK_NUMPAD6: strcpy(str, DKS_STR_KP6); break;
  case DIK_NUMPAD7: strcpy(str, DKS_STR_KP7); break;
  case DIK_NUMPAD8: strcpy(str, DKS_STR_KP8); break;
  case DIK_NUMPAD9: strcpy(str, DKS_STR_KP9); break;
  case DIK_NUMPAD0: strcpy(str, DKS_STR_KP0); break;
  case DIK_NUMPADPERIOD: strcpy(str, DKS_STR_KPDEC); break;
  case DIK_NUMPADENTER: strcpy(str, DKS_STR_KPENT); break;
  case DIK_SPACE: strcpy(str, DKS_STR_SPACE); break;
  default:
    /* DIK_CTRL: */
    strcpy(str, DKS_STR_CTRL);
    break;
  } 
}

/*********************************************************
*NAME:          winUtilVirtKeyToDI
*AUTHOR:        John Morrison
*CREATION DATE: 31/1/99
*LAST MODIFIED: 31/1/99
*PURPOSE:
* Returns the Direct Input equivilent of the windows 
* virtual key pressed.
*
*ARGUMENTS:
*  value - virtual key.
*********************************************************/
int winUtilVirtKeyToDI(int value) {
  int  returnValue; /* Value to return */

switch (value) {
  case DKS_VK_ESC: 
    (returnValue = DIK_ESCAPE);
    break;
  case DKS_VK_F1:
    (returnValue = DIK_F1);
    break;
  case DKS_VK_F2:
    (returnValue = DIK_F2);
    break;
  case DKS_VK_F3:
    (returnValue = DIK_F3);
    break;
  case DKS_VK_F4:
    (returnValue = DIK_F4);
    break;
  case DKS_VK_F5:
    (returnValue = DIK_F5);
    break;
  case DKS_VK_F6:
    (returnValue = DIK_F6);
    break;
  case DKS_VK_F7:
    (returnValue = DIK_F7);
    break;
  case DKS_VK_F8:
    (returnValue = DIK_F8);
    break;
  case DKS_VK_F9:
    (returnValue = DIK_F9);
    break;
  case DKS_VK_F10:
    (returnValue = DIK_F10);
    break;
  case DKS_VK_F11:
    (returnValue = DIK_F11);
    break;
  case DKS_VK_F12:
    (returnValue = DIK_F12);
    break;
  case DKS_VK_SCROLLOCK:
    (returnValue = DIK_SCROLL);
    break;
  case DKS_VK_TILDE:
    (returnValue = DIK_GRAVE);
    break;
  case DKS_VK_1: (returnValue = DIK_1);
    break;
  case DKS_VK_2:
    (returnValue = DIK_2);
    break;
  case DKS_VK_3:
    (returnValue = DIK_3);
    break;
  case DKS_VK_4:
    (returnValue = DIK_4);
    break;
  case DKS_VK_5:
    (returnValue = DIK_5);
    break;
  case DKS_VK_6: (returnValue = DIK_6); break;
  case DKS_VK_7: (returnValue = DIK_7); break;
  case DKS_VK_8: (returnValue = DIK_8); break;
  case DKS_VK_9: (returnValue = DIK_9); break;
  case DKS_VK_0: (returnValue = DIK_0); break;
  case DKS_VK_MINUS: (returnValue = DIK_MINUS); break;
  case DKS_VK_EQUALS: (returnValue = DIK_EQUALS); break;
  case DKS_VK_BACKSLASH: (returnValue = DIK_BACKSLASH); break;
  case DKS_VK_BACKSPACE: (returnValue = DIK_BACKSPACE); break;
  case DKS_VK_TAB: (returnValue = DIK_TAB); break;
  case DKS_VK_Q: (returnValue = DIK_Q); break;
  case DKS_VK_W: (returnValue = DIK_W); break;
  case DKS_VK_E: (returnValue = DIK_E); break;
  case DKS_VK_R: (returnValue = DIK_R); break;
  case DKS_VK_T: (returnValue = DIK_T); break;
  case DKS_VK_Y: (returnValue = DIK_Y); break;
  case DKS_VK_U: (returnValue = DIK_U); break;
  case DKS_VK_I: (returnValue = DIK_I); break;
  case DKS_VK_O: (returnValue = DIK_O); break;
  case DKS_VK_P: (returnValue = DIK_P); break;
  case DKS_VK_LBRACKET: (returnValue = DIK_LBRACKET); break;
  case DKS_VK_RBRACKET: (returnValue = DIK_RBRACKET); break;
  case DKS_VK_RET: (returnValue = DIK_RETURN); break;
  case DKS_VK_CAPS: (returnValue = DIK_CAPSLOCK); break;
  case DKS_VK_A: (returnValue = DIK_A); break;
  case DKS_VK_S: (returnValue = DIK_S); break;
  case DKS_VK_D: (returnValue = DIK_D); break;
  case DKS_VK_F: (returnValue = DIK_F); break;
  case DKS_VK_G: (returnValue = DIK_G); break;
  case DKS_VK_H: (returnValue = DIK_H); break;
  case DKS_VK_J: (returnValue = DIK_J); break;
  case DKS_VK_K: (returnValue = DIK_K); break;
  case DKS_VK_L: (returnValue = DIK_L); break;
  case DKS_VK_SEMICOLON: (returnValue = DIK_SEMICOLON); break;
  case DKS_VK_QUOTES: (returnValue = DIK_APOSTROPHE); break;
  case DKS_VK_SHIFT: (returnValue = DIK_LSHIFT); break;
  case DKS_VK_Z: (returnValue = DIK_Z); break;
  case DKS_VK_X: (returnValue = DIK_X); break;
  case DKS_VK_C: (returnValue = DIK_C); break;
  case DKS_VK_V: (returnValue = DIK_V); break;
  case DKS_VK_B: (returnValue = DIK_B); break;
  case DKS_VK_N: (returnValue = DIK_N); break;
  case DKS_VK_M: (returnValue = DIK_M); break;
  case DKS_VK_COMMA: (returnValue = DIK_COMMA); break;
  case DKS_VK_PERIOD: (returnValue = DIK_PERIOD); break;
  case DKS_VK_FORWARDSLASH: (returnValue = DIK_SLASH); break;
  case DKS_VK_INS: (returnValue = DIK_INSERT); break;
  case DKS_VK_HOME: (returnValue = DIK_HOME); break;
  case DKS_VK_PGUP: (returnValue = DIK_PGUP); break;
  case DKS_VK_DEL: (returnValue = DIK_DELETE); break;
  case DKS_VK_END: (returnValue = DIK_END); break;
  case DKS_VK_PGDN: (returnValue = DIK_PGDN); break;
  case DKS_VK_LEFT: (returnValue = DIK_LEFT); break;
  case DKS_VK_RIGHT: (returnValue = DIK_RIGHT); break;
  case DKS_VK_UP: (returnValue = DIK_UP); break;
  case DKS_VK_DOWN: (returnValue = DIK_DOWN); break;
  case DKS_VK_NUMLOCK: (returnValue = DIK_NUMLOCK); break;
  case DKS_VK_KPDIV: (returnValue = DIK_NUMPADSLASH); break;
  case DKS_VK_KPTIMES: (returnValue = DIK_NUMPADSTAR); break;
  case DKS_VK_KPMINUS: (returnValue = DIK_NUMPADMINUS); break;
  case DKS_VK_KPPLUS: (returnValue = DIK_NUMPADPLUS); break;
  case DKS_VK_KP1: (returnValue = DIK_NUMPAD1); break;
  case DKS_VK_KP2: (returnValue = DIK_NUMPAD2); break;
  case DKS_VK_KP3: (returnValue = DIK_NUMPAD3); break;
  case DKS_VK_KP4: (returnValue = DIK_NUMPAD4); break;
  case DKS_VK_KP5: (returnValue = DIK_NUMPAD5); break;
  case DKS_VK_KP6: (returnValue = DIK_NUMPAD6); break;
  case DKS_VK_KP7: (returnValue = DIK_NUMPAD7); break;
  case DKS_VK_KP8: (returnValue = DIK_NUMPAD8); break;
  case DKS_VK_KP9: (returnValue = DIK_NUMPAD9); break;
  case DKS_VK_KP0: (returnValue = DIK_NUMPAD0); break;
  case DKS_VK_KPDEC: (returnValue = DIK_NUMPADPERIOD); break;
  case DKS_VK_SPACE: (returnValue = DIK_SPACE); break;
  default:
    /* DKS_VK_CTRL: */
    (returnValue = DIK_LCONTROL);
    break;
  }

  return returnValue;
}

/*********************************************************
*NAME:          winUtilWBSubDirExist
*AUTHOR:        John Morrison
*CREATION DATE: 29/4/00
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Checks to see if a subdirectory exists off the WinBolo
*  Directory. Returns existance. Also modifies subDirName
*  to complete path of directory
*
*ARGUMENTS:
*  subDirName - The directory name to search for. Upon
*               successful find it also modifies this to
*               the complete path of the directory
*********************************************************/
bool winUtilWBSubDirExist(char *subDirName) {
  bool returnValue;            /* Value to return */
  char fileName[FILENAME_MAX]; /* Filename used in lookup */
  WIN32_FIND_DATA fd;          /* Find data */
  HANDLE findHandle;           /* The find handle  */
  unsigned int count;          /* Looping variable */

  returnValue = TRUE;
  /* Get the current directory */
  GetModuleFileName(windowGetInstance(), fileName, FILENAME_MAX);
  count = (unsigned int) strlen(fileName);
  while (fileName[count] != SLASH_CHAR) {
    count--;
  }


  if (count < FILENAME_MAX-1) {
    fileName[count+1] = EMPTY_CHAR;
  }
  strcat(fileName, subDirName);

  /* Look for the directory */
  findHandle = FindFirstFile(fileName, &fd);
  if (findHandle != INVALID_HANDLE_VALUE) {
    if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
      returnValue = FALSE;
    }
  } else {
    returnValue = FALSE;
  } 
  FindClose(findHandle);
  
  /* Copy the path into the parameter */
  if (returnValue == TRUE) {
    strcpy(subDirName, fileName);
  }
  return returnValue;
}

/*********************************************************
*NAME:          winUtilOpenHelpFile
*AUTHOR:        John Morrison
*CREATION DATE: 13/6/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  Opens the WinBolo help file.
*
*ARGUMENTS:
* hInst - Application Instance
* hWnd  - Parent Window handle (for error messages)
*********************************************************/
void winUtilOpenHelpFile(HINSTANCE appInst, HWND hWnd) {
  char path[MAX_PATH]; /* The path to open */
  char file[MAX_PATH]; /* The file to open */

  strcpy(path, "Manual");
  if (winUtilWBSubDirExist(path) == TRUE) {
    strcpy(file, "file://");
    strcat(file, path);
    strcat(file, "/Manual.pdf");
    if ((int) ShellExecute(NULL, "open", file, NULL, NULL, SW_SHOW) <= 32) {
      MessageBoxA(hWnd, langGetText(STRERR_HELPFILE), DIALOG_BOX_TITLE, MB_ICONINFORMATION); 
    }
  } else {
    /* Error */
    MessageBoxA(hWnd, langGetText(STRERR_HELPFILE), DIALOG_BOX_TITLE, MB_ICONINFORMATION); 
  }
}
