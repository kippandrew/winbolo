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

#ifndef __MEMORY_BLOCK_H
#define __MEMORY_BLOCK_H

#include "global.h"

/* Memory block structure */
typedef struct memoryblockObj *memoryblock;
struct memoryblockObj {
  unsigned long start; // Start is relative to overall position
  unsigned long end;   // End is relative to 0 start of array
  unsigned long size;  // Size is number of bytes allocated to this block
  BYTE *mem;
};

/* Memory block size is 512K */
#define MEMORY_BLOCK_SIZE (512 * 1024) 

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
memoryblock mbCreate(unsigned long size, unsigned long start);

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
void mbDestroy(memoryblock *value);

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
bool mbIsPosWithin(memoryblock *value, unsigned long pos);

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
bool mbIsPosWithinBounds(memoryblock *value, unsigned long pos);

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
unsigned long mbGetStart(memoryblock *value);

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
unsigned long mbGetEnd(memoryblock *value);

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
unsigned long mbGetData(memoryblock *value, BYTE *dest, unsigned long start, unsigned long len);

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
unsigned long mbPutData(memoryblock *value, BYTE *buff, unsigned long start, unsigned long len);

#endif /* __MEMORY_BLOCK_H */
