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
*Name:          Lang
*Filename:      lang.c
*Author:        John Morrison
*Creation Date: 28/4/00
*Last Modified: 13/6/00
*Purpose:
*  Handles languages loading from resource
*********************************************************/

#include "../lang.h"
#include "../linresource.h"

char langBuff[16*1024];
char langBuff2[16*1024];
char langFileName[FILENAME_MAX];

/*********************************************************
*NAME:          langSetup
*AUTHOR:        John Morrison
*Creation Date: 28/4/00
*Last Modified: 28/4/00
*PURPOSE:
*  Sets up language system
*
*ARGUMENTS:
*
*********************************************************/
bool langSetup() {
  return TRUE;
}


/*********************************************************
*NAME:          langCleanup
*AUTHOR:        John Morrison
*Creation Date: 28/4/00
*Last Modified: 28/4/00
*PURPOSE:
*  Cleans up the language systems
*
*ARGUMENTS:
*
*********************************************************/
void langCleanup() {
  langFileName[0] = '\0';
}

/*********************************************************
*NAME:          langGetMenu
*AUTHOR:        John Morrison
*Creation Date: 28/4/00
*Last Modified: 28/4/00
*PURPOSE:
*  Loads in a menu resource
*
*ARGUMENTS:
*
*********************************************************/
HMENU langGetMenu() {
  return 0;
}

/*********************************************************
*NAME:          langLoadFile
*AUTHOR:        John Morrison
*Creation Date: 28/4/00
*Last Modified: 13/6/00
*PURPOSE:
*  Loads in a language file to use. If the parameter is 
*  NULL then it defaults to inbuilt english
*
*ARGUMENTS:
*  fileName - File name to use
*  langName - The raw name of the file not including path
*********************************************************/
bool langLoadFile(char *filename, char *langName) {
  bool returnValue; /* Value to return */

  returnValue = FALSE;
  return returnValue;
}

/*********************************************************
*NAME:          langGetFileName
*AUTHOR:        John Morrison
*Creation Date: 29/4/00
*Last Modified: 29/4/00
*PURPOSE:
*  Gets a copy of the filename used to load the current
*  library
*
*ARGUMENTS:
*  fileName - File name to use
*********************************************************/
void langGetFileName(char *fileName) {
  strcpy(fileName, langFileName);
}

/*********************************************************
*NAME:          langGetText
*AUTHOR:        John Morrison
*Creation Date: 28/4/00
*Last Modified: 28/4/00
*PURPOSE:
*  Returns the text of this items ID
*
*ARGUMENTS:
*  id - Id to load
*********************************************************/
char* langGetText(langid id) {
  strcpy(langBuff, id);
  return langBuff;
}

/*********************************************************
*NAME:          langGetText2
*AUTHOR:        John Morrison
*Creation Date: 28/4/00
*Last Modified: 28/4/00
*PURPOSE:
*  Returns the text of this items ID into buffer 2
*
*ARGUMENTS:
*  id - Id to load
*********************************************************/
char* langGetText2(langid id) {
  strcpy(langBuff2, id);
  return langBuff2;
}

