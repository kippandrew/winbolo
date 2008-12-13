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


#ifndef _MESSAGE_BOX_H
#define _MESSAGE_BOX_H
#include "../../bolo/global.h"
#define DIALOG_BOX_TITLE "LinBolo"

void MessageBox(char *label, char *title);

void MessageBox2(char *label, char *title, char *label2, char *title2);

void MessageBox3(char *label, char *title, char *label2, char *title2, char *label3, char *title3); 

#endif

