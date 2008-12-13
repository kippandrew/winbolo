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
*Name:          Skins
*Filename:      skins.h
*Author:        John Morrison
*Creation Date: 27/09/03
*Last Modified: 27/09/03
*Purpose:
*  Looks after loading/unloading skins
*  Code from zlib version 1.1.4
*********************************************************/

#ifndef __SKINS_H
#define __SKINS_H

#include "../bolo/global.h"

/* The default skins directory is called skins */
#define SKIN_DIR_STRING "skins"

/*********************************************************
*NAME:          skinsSetup
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Sets up the skins initially. Returns Success
*
*ARGUMENTS:
*
*********************************************************/
bool skinsSetup(void);

/*********************************************************
*NAME:          skinsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Cleans up after the skins. Remove temp files.
*
*ARGUMENTS:
*
*********************************************************/
void skinsDestroy(void);

/*********************************************************
*NAME:          skinsLoadSkin
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Attempts to load a skin set.
*
*ARGUMENTS:
*  fileName - Filename of skin to load.
*********************************************************/
bool skinsLoadSkin(char *fileName);

/*********************************************************
*NAME:          skinsIsLoaded
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Returns if a skin set is loaded
*
*ARGUMENTS:
*
*********************************************************/
bool skinsIsLoaded();

/*********************************************************
*NAME:          skinsIsLoaded
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Returns if a skin set is loaded
*
*ARGUMENTS:
* value - Char pointer to hold directory
*********************************************************/
void skinsGetSkinDirectory(char *value);

/*********************************************************
*NAME:          skinsRemoveFiles
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Removes the skin files and temp directory
*
*ARGUMENTS:
*
*********************************************************/
void skinsRemoveFiles(void);

/*********************************************************
*NAME:          skinsGetFileName
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Gets the skin file name of the current skin (excluding
*  extension) If no skin is loaded it returns NULL.
*
*ARGUMENTS:
* value - Char pointer to hold skin name
*********************************************************/
void skinsGetFileName(char *value);

#endif /* __SKINS_H */
