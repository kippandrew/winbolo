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
*Name:          Dialog - System Info
*Filename:      dialogsysteminfo.c
*Author:        John Morrison
*Creation Date: 23/1/99
*Last Modified: 29/4/00
*Purpose:
*  Looks after the system info dialog box.
**********************************************************/

#ifndef _DIALOG_SYS_INFO
#define _DIALOG_SYS_INFO

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include "../../bolo/global.h"


/* There are 1000 miliseconds in a second */
#define SECOND 1000.0


/*********************************************************
*NAME:          dialogSysInfoUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 29/1/99
*LAST MODIFIED: 23/1/99
*PURPOSE:
* Updates the system info dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
gint dialogSysInfoUpdate(gpointer data);

GtkWidget* dialogSystemInfoCreate(void);

#endif

