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
*Filename:      winutil.h
*Author:        John Morrison
*Creation Date:  2/2/99
*Last Modified: 13/6/00
*Purpose:
*  Contains useful window functions
*********************************************************/

#ifndef WIN_UTIL_H
#define WIN_UTIL_H


#include <windows.h>
#include "..\bolo\global.h"

/* String identifiers for each key */
#define DKS_STR_SPACE "Space\0"
#define DKS_STR_ESC "Esc\0"
#define DKS_STR_F1 "F1\0"
#define DKS_STR_F2 "F2\0"
#define DKS_STR_F3 "F3\0"
#define DKS_STR_F4 "F4\0"
#define DKS_STR_F5 "F5\0"
#define DKS_STR_F6 "F6\0"
#define DKS_STR_F7 "F7\0"
#define DKS_STR_F8 "F8\0"
#define DKS_STR_F9 "F9\0"
#define DKS_STR_F10 "F10\0"
#define DKS_STR_F11 "F11\0"
#define DKS_STR_F12 "F12\0"
#define DKS_STR_PRINTSRC "PrtScr\0"
#define DKS_STR_SCROLLOCK "Scrlock\0"
#define DKS_STR_PAUSE "Pause\0"
#define DKS_STR_TILDE "`\0"
#define DKS_STR_1 "1\0"
#define DKS_STR_2 "2\0"
#define DKS_STR_3 "3\0"
#define DKS_STR_4 "4\0"
#define DKS_STR_5 "5\0"
#define DKS_STR_6 "6\0"
#define DKS_STR_7 "7\0"
#define DKS_STR_8 "8\0"
#define DKS_STR_9 "9\0"
#define DKS_STR_0 "0\0"
#define DKS_STR_MINUS "-\0"
#define DKS_STR_EQUALS "=\0"
#define DKS_STR_BACKSLASH "\\\0"
#define DKS_STR_BACKSPACE "BkSpc\0"
#define DKS_STR_TAB "Tab\0"
#define DKS_STR_Q "Q\0"
#define DKS_STR_W "W\0"
#define DKS_STR_E "E\0"
#define DKS_STR_R "R\0"
#define DKS_STR_T "T\0"
#define DKS_STR_Y "Y\0"
#define DKS_STR_U "U\0"
#define DKS_STR_I "I\0"
#define DKS_STR_O "O\0"
#define DKS_STR_P "P\0"
#define DKS_STR_LBRACKET "[\0"
#define DKS_STR_RBRACKET "]\0"
#define DKS_STR_RET "Ret\0"
#define DKS_STR_CAPS "CapsLock\0"
#define DKS_STR_A "A\0"
#define DKS_STR_S "S\0"
#define DKS_STR_D "D\0"
#define DKS_STR_F "F\0"
#define DKS_STR_G "G\0"
#define DKS_STR_H "H\0"
#define DKS_STR_J "J\0"
#define DKS_STR_K "K\0"
#define DKS_STR_L "L\0"
#define DKS_STR_SEMICOLON ";\0"
#define DKS_STR_QUOTES "'\0"
#define DKS_STR_SHIFT "Shift\0"
#define DKS_STR_Z "Z\0"
#define DKS_STR_X "X\0"
#define DKS_STR_C "C\0"
#define DKS_STR_V "V\0"
#define DKS_STR_B "B\0"
#define DKS_STR_N "N\0"
#define DKS_STR_M "M\0"
#define DKS_STR_COMMA ",\0"
#define DKS_STR_PERIOD ".\0"
#define DKS_STR_FORWARDSLASH "\\\0"
#define DKS_STR_INS "Ins\0"
#define DKS_STR_HOME "Home\0"
#define DKS_STR_PGUP "PgUp\0"
#define DKS_STR_DEL "DEL\0"
#define DKS_STR_END "END\0"
#define DKS_STR_PGDN "PgDn\0"
#define DKS_STR_LEFT "Left\0"
#define DKS_STR_RIGHT "Right\0"
#define DKS_STR_UP "Up\0"
#define DKS_STR_DOWN "Down\0"
#define DKS_STR_NUMLOCK "NumLck\0"
#define DKS_STR_KPDIV "KP\\\0"
#define DKS_STR_KPTIMES "KP*\0"
#define DKS_STR_KPMINUS "KP-\0"
#define DKS_STR_KPPLUS "KP+\0"
#define DKS_STR_KP1 "KP1\0"
#define DKS_STR_KP2 "KP2\0"
#define DKS_STR_KP3 "KP3\0"
#define DKS_STR_KP4 "KP4\0"
#define DKS_STR_KP5 "KP5\0"
#define DKS_STR_KP6 "KP6\0"
#define DKS_STR_KP7 "KP7\0"
#define DKS_STR_KP8 "KP8\0"
#define DKS_STR_KP9 "KP9\0"
#define DKS_STR_KP0 "KP0\0"
#define DKS_STR_KPDEC "KP.\0"
#define DKS_STR_KPENT "KPRet\0"
#define DKS_STR_CTRL "Ctrl\0"

/* Virtual Key identifiers */

#define DKS_VK_SPACE 32
#define DKS_VK_ESC 27
#define DKS_VK_F1 112
#define DKS_VK_F2 113
#define DKS_VK_F3 114
#define DKS_VK_F4 115
#define DKS_VK_F5 116
#define DKS_VK_F6 117
#define DKS_VK_F7 118
#define DKS_VK_F8 119
#define DKS_VK_F9  120
#define DKS_VK_F10 121
#define DKS_VK_F11 122
#define DKS_VK_F12 123
#define DKS_VK_SCROLLOCK 145
#define DKS_VK_TILDE 192
#define DKS_VK_1 49
#define DKS_VK_2 50
#define DKS_VK_3 51
#define DKS_VK_4 52
#define DKS_VK_5 53
#define DKS_VK_6 54
#define DKS_VK_7 55
#define DKS_VK_8 56
#define DKS_VK_9 57
#define DKS_VK_0 48
#define DKS_VK_MINUS 189
#define DKS_VK_EQUALS 187
#define DKS_VK_BACKSLASH 220
#define DKS_VK_BACKSPACE 8
#define DKS_VK_TAB 9
#define DKS_VK_Q 81
#define DKS_VK_W 87
#define DKS_VK_E 69
#define DKS_VK_R 82
#define DKS_VK_T 84
#define DKS_VK_Y 89
#define DKS_VK_U 85
#define DKS_VK_I 73
#define DKS_VK_O 79
#define DKS_VK_P 80
#define DKS_VK_LBRACKET 219
#define DKS_VK_RBRACKET 221
#define DKS_VK_RET 13
#define DKS_VK_CAPS 20
#define DKS_VK_A 65
#define DKS_VK_S 83
#define DKS_VK_D 68
#define DKS_VK_F 70
#define DKS_VK_G 71
#define DKS_VK_H 72
#define DKS_VK_J 74
#define DKS_VK_K 75
#define DKS_VK_L 76
#define DKS_VK_SEMICOLON 186
#define DKS_VK_QUOTES 222
#define DKS_VK_SHIFT 16
#define DKS_VK_Z 90
#define DKS_VK_X 88
#define DKS_VK_C 67
#define DKS_VK_V 86
#define DKS_VK_B 66
#define DKS_VK_N 78
#define DKS_VK_M 77
#define DKS_VK_COMMA 188
#define DKS_VK_PERIOD 190
#define DKS_VK_FORWARDSLASH 191
#define DKS_VK_RSHIFT 16
#define DKS_VK_INS 45
#define DKS_VK_HOME 36
#define DKS_VK_PGUP 33
#define DKS_VK_DEL 46
#define DKS_VK_END 35
#define DKS_VK_PGDN 34
#define DKS_VK_LEFT 37
#define DKS_VK_RIGHT 39
#define DKS_VK_UP 38
#define DKS_VK_DOWN 40
#define DKS_VK_NUMLOCK 144
#define DKS_VK_KPDIV 111
#define DKS_VK_KPTIMES 106
#define DKS_VK_KPMINUS 109
#define DKS_VK_KPPLUS 107
#define DKS_VK_KP1 97
#define DKS_VK_KP2 98
#define DKS_VK_KP3 99
#define DKS_VK_KP4 100
#define DKS_VK_KP5 101
#define DKS_VK_KP6 102
#define DKS_VK_KP7 103
#define DKS_VK_KP8 104
#define DKS_VK_KP9 105
#define DKS_VK_KP0 96
#define DKS_VK_KPDEC 110
#define DKS_VK_KPENT 13
#define DKS_VK_CTRL 17

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
BOOL winUtilCenterWindow(HWND hWnd);

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
void winUtilVirtKeyToString(int value, char *str);

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
void winUtilDIKeyToString(int value, char *str);

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
int winUtilVirtKeyToDI(int value);

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
bool winUtilWBSubDirExist(char *subDirName);

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
void winUtilOpenHelpFile(HINSTANCE appInst, HWND hWnd);

#endif /* WIN_UTIL_H */
