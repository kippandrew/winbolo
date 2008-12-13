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


#ifndef __BLOCKS_H
#define __BLOCKS_H

#include "global.h"
#include "memoryblock.h"

typedef struct blocksObj *blocks;

struct blocksObj {
  memoryblock item;
  blocks next;
};

#define logIsEOF() blocksIsEOF()
#define logReadBytes(X, Y) blocksReadBytes(X, Y)

// Size in MB
bool blocksCreate(char *fileName, int size);
void blocksDestroy();
int blocksReadBytes(BYTE *buff, int len);
bool blocksIsEOF();
unsigned long logGetCurrentPosition();

void logSetPosition(unsigned long pos);
BYTE blocksGetKey();
void blocksSetKey(BYTE key);

#endif /* __BLOCKS_H */

