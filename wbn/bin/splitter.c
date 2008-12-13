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


/*
 * Splitter Used to get the first 200 bytes out of a log file
 */

#include <stdio.h>

int main(void) {
	int count = 0;
	int val;
	while (count < 200 && !feof(stdin)) {
		val = fgetc(stdin);
		fputc(val, stdout);
		count++;
	}
	return 0;
}

