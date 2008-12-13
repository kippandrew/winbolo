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
*Name:          map preview
*Filename:      mappreview.h
*Author:        John Morrison
*Creation Date: 2/7/00
*Last Modified: 2/7/00
*Purpose:
*  Map previews in file open dialog box
*********************************************************/

#ifndef _MAPPREVIEW_H
#define _MAPPREVIEW_H

#include <windows.h>
#include "..\bolo\global.h"
#include "..\bolo\backend.h"


/*********************************************************
*NAME:          mapPreviewSetup
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  Sets up map preview.
*
*ARGUMENTS:
*
*********************************************************/
void mapPreviewSetup();

/*********************************************************
*NAME:          mapPreviewDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  Destroys map preview.
*
*ARGUMENTS:
*
*********************************************************/
void mapPreviewDestroy();

/*********************************************************
*NAME:          mapPreviewDraw
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  loads the map in filename and draws it in dest if it
*  is a valid map.
*
*ARGUMENTS:
*  fileName - Map to load
*  dest     - Destination device context
*********************************************************/
void mapPreviewDraw(char *fileName, HDC dest);

/*********************************************************
*NAME:          mapPreviewCopy
*AUTHOR:        John Morrison
*CREATION DATE: 2/7/00
*LAST MODIFIED: 2/7/00
*PURPOSE:
*  Copies a map preview into dest for redrawing purposes
*
*ARGUMENTS:
*  dest     - Destination device context
*********************************************************/
void mapPreviewCopy(HDC dest);

#endif /* _MAPPREVIEW_H */
