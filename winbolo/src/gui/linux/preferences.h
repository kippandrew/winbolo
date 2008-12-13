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
*Name:          Preferences
*Filename:      preferences.h
*Author:        John Morrison
*Creation Date: 26/11/99
*Last Modified: 26/11/99
*Purpose:
* Implements the Win32 ini file functions required. Also
* includes function to preference file name and path.
*********************************************************/


#ifndef __PREFERENCES_H
#define __PREFERENCES_H

#define PREFERENCE_FILE "linbolo.ini"

/*********************************************************
*NAME:          GetPrivateProfileString 
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Implements the Win32 GetPrivateProfileString function
*
*ARGUMENTS:
* section  - The section name excluding square [ ] 
* item     - The item name to read
* def      - The default item to replace with if missing
* output   - The output variable to store in
* filename - Filename and path to read file from
*********************************************************/
void GetPrivateProfileString(char *section, char *item, char *def, char *output, int outlen, char *filename);

/*********************************************************
*NAME:          WritePrivateProfileString 
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Implements the Win32 WritePrivateProfileString function
*
*ARGUMENTS:
* section - The section name excluding square [ ] 
* item    - The item name to write
* value   - The value to set the item name too
* filename - Filename and path to write too
*********************************************************/
void WritePrivateProfileString(char *section, char *item, char *value, char *filename);

/*********************************************************
*NAME:          preferencesGetPreferenceFile 
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
* Returns the preference path and file name. Under linux
* this is $HOME/.linbolo/linbolo.ini
*
*ARGUMENTS:
* value - Pointer to hold path returned 
*********************************************************/
void preferencesGetPreferenceFile(char *value);

#endif /* __PREFERENCES_H */

