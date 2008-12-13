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


#include "global.h"
#include "memoryblock.h"
#include "blocks.h"
#include "/winbolo/src/zlib/unzip.h"

/* typedef struct blocksObj *blocks;

struct blocksObj {
  memoryblock item;
  blocks *next;
}; */

char logFileName[256];
unzFile logFile;
unsigned long zipFilePosition; /* Log position in the zip file */
unsigned long logPosition; /* Position in the log file we are */
unsigned long logMinBlockPosition; /* Minimum log position stored in memory blocks */
unsigned long logMaxBlockPosition; /* Maximum log position stored in memory blocks */
blocks block = NULL;
int logMaxMemory; /* Maximum memory to use */
BYTE blockKey;

// Number of blocks we have 
int blocksGetNumber() {
  blocks q;
  int returnValue = 0;

  q = block;
  while (q != NULL) {
    returnValue++;
    q = q->next;
  }
  return returnValue;
}

// Size in MB
bool blocksCreate(char *fileName, int size) {
  bool returnValue = TRUE;
  blockKey = 0;
  
  logMaxMemory = size * 2;
  if (logMaxMemory < 2) {
    logMaxMemory = 2;
  }

  block = NULL;
  logFile = NULL;
  logMinBlockPosition = 0;
  logMaxBlockPosition = 0;

  logPosition = 0;
  zipFilePosition = 0;
  logFile = unzOpen(fileName);
  if (logFile == NULL) {
    returnValue = FALSE;
  }

  if (returnValue == TRUE) {
    strcpy(logFileName, fileName);
    if (unzLocateFile(logFile, "log.dat", 0) != UNZ_OK) {
      returnValue = FALSE;
    }
  }

  if (returnValue == TRUE) {
    if (unzOpenCurrentFile(logFile) != UNZ_OK) {
      returnValue = FALSE;
    }
  }

  return returnValue;
}

// Destroy blocks
void blocksDestroy() {
  blocks q;

  while (block != NULL) {
    q = block;
    block = block->next;
    mbDestroy(&(q->item));
  }
  if (logFile != NULL) {
    unzCloseCurrentFile(logFile);
    unzClose(logFile);
    logFile = NULL;
  }
}

/* Get the block containing */
memoryblock blocksGetBlockContaining(unsigned long pos) {
  blocks q;
  memoryblock mb = NULL;

  q = block;
  while (mb == NULL && q != NULL) {
    if (mbIsPosWithin(&(q->item), pos) == TRUE) {
      mb = q->item;
      mbIsPosWithin(&(q->item), pos);
    }
    q = q->next;
  }

  return mb;
}

memoryblock blocksGetBlockFitsIn(unsigned long pos) {
  blocks q;
  memoryblock mb = NULL;

  q = block;
  while (mb == NULL && q != NULL) {
    if (mbIsPosWithinBounds(&(q->item), pos) == TRUE) {
      mb = q->item;
    }
    q = q->next;
  }

  return mb;
}


/* Add new block */
void blocksAddNewBlock(BYTE *buff, unsigned long pos, int length) {
  unsigned long amountStored;
  unsigned long longLength = length;
  blocks q;
  blocks prev = NULL;
  blocks newmb;

  // Check for max blocks, remove if required
  if (blocksGetNumber() > logMaxMemory) {
    q = block;
    block = block->next;
    mbDestroy(&(q->item));
    
    if (block != NULL) {
      logMinBlockPosition = mbGetStart(&(block->item));
    } else {
      logMinBlockPosition = 0;
      logMaxBlockPosition = 0;
    }
  }

  // Add new block
  
  New(newmb);
  newmb->next = NULL;
  newmb->item = mbCreate(MEMORY_BLOCK_SIZE, pos);
  amountStored = mbPutData(&(newmb->item), buff, pos, length);
  
  q = block;
  while (q != NULL) {
    prev = q;
    q = q->next;
  }
  if (prev == NULL) {
    block = newmb;
  } else {
    prev->next = newmb;
  }

  if (amountStored < longLength) {
    blocksAddNewBlock(buff+amountStored, pos+amountStored, length-amountStored);
  }
}

/* Add bytes */
int blocksAddBytes(BYTE *buff, int length) {
  memoryblock mb;
  unsigned long amountStored;
  unsigned long longLength = length;

  mb = blocksGetBlockFitsIn(logPosition);
  if (mb != NULL) {
    amountStored = mbPutData(&mb, buff, logPosition, length);
    if (amountStored < longLength) {
      // Need to store more bytes
      // Need to add in new memory block
      blocksAddNewBlock(buff+amountStored, logPosition+amountStored, length-amountStored);
    }
  } else {
    blocksAddNewBlock(buff, logPosition, length);
  }
  logMaxBlockPosition = logPosition+length;
  return 0;
}


/* Are we EOF */
bool blocksIsEOF() {
  if (logPosition < zipFilePosition) {
    return FALSE;
  }
  if (logFile != NULL) {
    return unzeof(logFile);
  }
  return TRUE;
}

int blocksReadFromBlocks(BYTE *buff, int len) {
  memoryblock mb;
  int returnValue = 0;

  mb = blocksGetBlockContaining(logPosition);
  while (returnValue < len && mb != NULL) {
    returnValue += mbGetData(&mb, buff+returnValue, logPosition+returnValue, len-returnValue);
    if (returnValue < len) {
      mb = blocksGetBlockContaining(logPosition+returnValue);
    }
  }

  return returnValue;
}

/* Read Bytes from the file */
int blocksReadBytes(BYTE *buff, int len) {
  int readLength = 0;
  int zipReadLength;
  int count = 0;

  if (logFile != NULL) {
    /* Try to read from memory blocks first */
    if (logPosition >= logMinBlockPosition && logPosition < logMaxBlockPosition) {
      readLength = blocksReadFromBlocks(buff, len);
      logPosition += readLength;
    }
    if (readLength < len) {
      /* Failing that read from the file */
      zipReadLength = unzReadCurrentFile(logFile, buff+readLength, len-readLength);
      if (zipReadLength > 0) {
        zipFilePosition += zipReadLength;
        blocksAddBytes(buff, zipReadLength);
        logPosition += zipReadLength;
        readLength += zipReadLength;
      }
    }
  } else {
    readLength = -1;
  }

  while (count < readLength) {
    buff[count] = buff[count] ^ blockKey;
    count++;
  }

  return readLength;
}

void logSetPosition(unsigned long pos) {
  blocks q;
  unsigned long count = 0;
  BYTE temp;

  if (pos >= logMinBlockPosition && pos < logMaxBlockPosition) {
    logPosition = pos;
  } else if (pos < logMinBlockPosition) {
    // Delete all blocks and re-read the file back to our current position
    while (block != NULL) {
      q = block;
      block = block->next;
      mbDestroy(&(q->item));
    }
    logMinBlockPosition = pos;
    logMaxBlockPosition = pos;
    logPosition = pos;
    unzCloseCurrentFile(logFile);
    unzClose(logFile);
    logFile = unzOpen(logFileName);
    unzLocateFile(logFile, "log.dat", 0);
    unzOpenCurrentFile(logFile);
    while (count < pos) {
      unzReadCurrentFile(logFile, &temp, 1);
      count++;
    }
    zipFilePosition = pos;
  } else {
    temp = 1;
  }
}

unsigned long logGetCurrentPosition() {
  return logPosition;
}

BYTE blocksGetKey() {
  return blockKey;
}

void blocksSetKey(BYTE key) {
  blockKey = key;
}
