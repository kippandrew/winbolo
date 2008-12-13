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
*Name:          Memory Block
*Filename:      memoryblock.c
*Author:        John Morrison
*Purpose:
*  Defines a memory block used for caching uncompressed
*  data from log files
*********************************************************/

#include <stdlib.h>
#include "global.h"
#include "memoryblock.h"

/*
struct memoryblockObj {
  unsigned long start; // Start is relative to overall position
  unsigned long end;   // End is relative to 0 start of array
  unsigned long size;  // Size is number of bytes allocated to this block
  BYTE *mem;
}; */

/*********************************************************
*NAME:          mbCreate
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Creates a new memory block. Returns new memory block
*
*ARGUMENTS:
* size - Size of the memory block to create
* start - byte offset from the start of the log file this
*         memory block starts
*********************************************************/
memoryblock mbCreate(unsigned long size, unsigned long start) {
  memoryblock mb;
  mb = New(mb);
  mb->size = size;
  mb->end = 0;
  mb->start = start;
  mb->mem = malloc(size);
  memset(mb->mem, 0x0F, size);
  return mb;
}

/*********************************************************
*NAME:          mbDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Destroys this memory block
*
*ARGUMENTS:
* value - Memory block to destroy
*********************************************************/
void mbDestroy(memoryblock *value) {
  free((*value)->mem);
  Dispose(value);
}

/*********************************************************
*NAME:          mbIsPosWithin
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Returns whether pos is located within this memory block
*
*ARGUMENTS:
* value - Memeory block to check
* pos   - Log file position to check
*********************************************************/
bool mbIsPosWithin(memoryblock *value, unsigned long pos) {
  if (pos != (*value)->start + (*value)->size && pos >= (*value)->start && pos <= (*value)->start + (*value)->end) {
    return TRUE;
  }
  return FALSE;
}

/*********************************************************
*NAME:          mbIsPosWithinBounds
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Returns whether pos is located within this memory block
*  and has been allocated
*
*ARGUMENTS:
* value - Memeory block to check
* pos   - Log file position to check
*********************************************************/
bool mbIsPosWithinBounds(memoryblock *value, unsigned long pos) {
  if (pos != (*value)->start + (*value)->size && pos >= (*value)->start && pos <= (*value)->start + (*value)->size) {
    return TRUE;
  }
  return FALSE;
}

/*********************************************************
*NAME:          mbGetStart
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Returns the log file position at the start of this
*  memory block
*
*ARGUMENTS:
* value - Memeory block to check
*********************************************************/
unsigned long mbGetStart(memoryblock *value) {
  return (*value)->start;
}

/*********************************************************
*NAME:          mbGetEnd
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Returns the end position of the memory block
*
*ARGUMENTS:
* value - Memeory block to check
*********************************************************/
unsigned long mbGetEnd(memoryblock *value) {
  return (*value)->end;
}

/*********************************************************
*NAME:          mbGetData
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Retrieves the data located in this memory block. Returns
*  the amount of data copied
*  
*
*ARGUMENTS:
* value - Memeory block to retrieve from
* dest  - Byte buffere to hold data retrieved
* start - Start position to retrieve data from
* len   - Length of the data to return
*********************************************************/
unsigned long mbGetData(memoryblock *value, BYTE *dest, unsigned long start, unsigned long len) {
  unsigned long count = 0;
  unsigned long end;
  unsigned long pos = start;

  end = (*value)->start + (*value)->end;
  if (start >= (*value)->start && start <= end) {
    while (count < len && pos < end) {
      dest[count] = (*value)->mem[start-(*value)->start+count];
      count++;
      pos++;
    }
  }
  return count;
}

/*********************************************************
*NAME:          mbPutData
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Adds data into this memory block. Returns the amount
*  of data added.
*
*ARGUMENTS:
* value - Memeory block to put data into
* buff  - Byte byffer to add data from
* start - Start position to add data from
* len   - Length of the data to add
*********************************************************/
unsigned long mbPutData(memoryblock *value, BYTE *buff, unsigned long start, unsigned long len) {
  unsigned long count = 0;
  unsigned long startOffset = start - (*value)->start;

  if (start - (*value)->start >= 0 && startOffset < (*value)->start + (*value)->size) {
    while (count < len && (*value)->end < (*value)->size) {
      (*value)->mem[count+startOffset] = buff[count];
      (*value)->end++;
      count++;
    }
  }
  return count;
}
