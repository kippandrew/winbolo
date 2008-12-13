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
*Name:          Dialog - Game Info
*Filename:      dialoggameinfo.c
*Author:        John Morrison
*Creation Date: 26/1/99
*Last Modified: 28/4/00
*Purpose:
*  Looks after the game info dialog box.
*********************************************************/

#ifndef _DIALOG_GAME_INFO_H
#define _DIALOG_GAME_INFO_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "../lang.h"
#include "../../bolo/global.h"
#include "../../bolo/backend.h"
#include "../linresource.h"

/* There are 60 seconds in a minute */
#define NUM_SECONDS_MINUTE 60

/* There are 1000 miliseconds in a second */
#define SECOND 1000.0


/*********************************************************
*NAME:          dialogGameInfoUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 26/1/99
*LAST MODIFIED: 26/1/99
*PURPOSE:
* Called each time the dialog is supposed to be updated.
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
gint dialogGameInfoUpdate(gpointer data); 

gint dialogGameInfoClose(GtkWidget *widget, gpointer gdata); 

GtkWidget* dialogGameInfoCreate(void);

#endif

