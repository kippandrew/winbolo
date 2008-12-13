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


#ifndef _DIALOG_NETWORK_INFO_H
#define _DIALOG_NETWORK_INFO_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "../../bolo/global.h"
#include "../../bolo/network.h"
#include "dialognetworkinfo.h"

/* There are 1000 miliseconds in a second */
#define SECOND 1000.0

/*********************************************************
*NAME:          dialogNetInfoUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 1/9/99
*PURPOSE:
* Updates the network info dialog box
*
*ARGUMENTS:
*  hWnd - The dialog window handle
*********************************************************/
gint dialogNetInfoUpdate(gpointer data); 

gint dialogNetworkInfoClose(GtkWidget *widget, gpointer gdata);

GtkWidget* dialogNetworkInformationCreate(void);

#endif
