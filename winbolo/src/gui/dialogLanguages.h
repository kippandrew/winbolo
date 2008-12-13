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
*Name:          Dialog - Languages
*Filename:      dialogLanguages.h
*Author:        John Morrison
*Creation Date: 29/4/00
*Last Modified: 29/4/00
*Purpose:
*  Looks after the languages dialog
*********************************************************/

#ifndef _DIALOG_LANGUAGES_H
#define _DIALOG_LANGUAGES_H


#include <windows.h>


#define SLASH_STRING "\\"
#define SLASH_CHAR '\\'
#define LANG_DIR_STRING "Languages"
#define LANG_FILTER_STRING "*.wbl"
#define LANG_EXTENSION ".wbl"

/*********************************************************
*NAME:          dialogLanguagesCallback
*AUTHOR:        John Morrison
*CREATION DATE: 29/4/00
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  The Languages Dialog call back function.
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*  msg    - The message
*  wParam - Message parameters
*  lParam - More Message parameters
*********************************************************/
BOOL CALLBACK dialogLanguagesCallback( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);

/*********************************************************
*NAME:          dialogLanguagesInit
*AUTHOR:        John Morrison
*CREATION DATE: 29/4/00
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  The Languages Dialog init function
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogLanguagesInit(HWND hWnd);

/*********************************************************
*NAME:          dialogLanguagesLoadList
*AUTHOR:        John Morrison
*CREATION DATE: 29/4/00
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Loads the list of available languages into the 
*  dialog box
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogLanguagesLoadList(HWND hWnd);

/*********************************************************
*NAME:          dialogLanguagesSetText
*AUTHOR:        John Morrison
*CREATION DATE: 29/4/00
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Sets the text in the dialog box
*
*ARGUMENTS:
*  hWnd   - Handle to the window
*********************************************************/
void dialogLanguagesSetText(HWND hWnd);

#endif /* _DIALOG_LANGUAGES_H */
