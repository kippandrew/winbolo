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
*Name:          bolo_map
*Filename:      bolo_map.c
*Author:        John Morrison
*Creation Date: 21/10/98
*LAST MODIFIED: 11/11/00
*Purpose:
*  Provides operations for read and writing of
*  Bolo map files
*********************************************************/

#include <stdio.h>
#include <string.h>
#include "global.h"
#include "bolo_map.h"
#include "pillbox.h"
#include "starts.h"
#include "bases.h"

BYTE *mapOriginal;
long mapLength;

bool mapReadEOFCheck(BYTE *buff) {
  BYTE *test;

  test = mapOriginal + mapLength;
  if (buff < test) {
    return FALSE;
  }
  return TRUE;
}

/*********************************************************
*NAME:          mapCreate
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 21/10/98
*PURPOSE:
*  Creates and initilises the map structure. Sets all 
*  map squares to be deep ocean
*
*ARGUMENTS:
*  value - Pointer to the map file
*********************************************************/
void mapCreate(map *value) {
  int count;  /* Looping variable */
  int count2; /* Looping variable */
  New(*value);
  for (count=0;count<MAP_ARRAY_SIZE ;count++) {
    for (count2=0;count2<MAP_ARRAY_SIZE;count2++) {
      ((*value)->mapItem[count][count2]) = DEEP_SEA;
    }
  }
}

/*********************************************************
*NAME:          mapDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 21/10/98
*PURPOSE:
*  Destroys the map data structure. Also frees memory.
*
*ARGUMENTS:
*  value - Pointer to the map file
*********************************************************/
void mapDestroy(map *value) {
  Dispose(*value);
}

/*********************************************************
*NAME:          mapReadPills
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 11/11/00
*PURPOSE:
*  Reads the pill information from a map file
*  into the pill structure. Returns if the 
*  operation was successful or not
*
*ARGUMENTS:
*  buff  - Pointer to the read buffer
*  value - Pointer to the pillbox structure
*********************************************************/
bool mapReadPills(BYTE **buff, pillboxes *value) {
  int total;        /* Total number of pills to be read in */
  int count;        /* Looping variable */
  bool returnValue; /* Value to return */
  pillbox readInto; /* The pillbox being read into */
  
  count = 1;
  total = pillsGetNumPills(value);
  returnValue = TRUE;
  readInto.inTank = FALSE;

  while (count <= total && returnValue == TRUE && mapReadEOFCheck(*buff) == FALSE) {
    memcpy(&readInto, *buff, SIZEOFBMAP_PILL_INFO);
    *buff += SIZEOFBMAP_PILL_INFO;
    pillsSetPill(value,&readInto,(BYTE) count);
    count++;
  }
  return returnValue;
}

/*********************************************************
*NAME:          mapReadBases
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 11/11/00
*PURPOSE:
*  Reads the base information from a map file
*  into the base structure. Returns if the 
*  operation was successful or not
*
*ARGUMENTS:
*  buff  - Pointer to the read buffer
*  value - Pointer to the pillbox structure
*********************************************************/
bool mapReadBases(BYTE **buff, bases *value) {
  int count;        /* Looping variable */
  int total;        /* Total number of bases to read */
  bool returnValue; /* Value to return */
  base readInto;    /* The base structure to read into */

  count = 1;
  total = basesGetNumBases(value);
  returnValue = TRUE;

  while (count <= total && returnValue == TRUE && mapReadEOFCheck(*buff) == FALSE) {
    memcpy(&readInto, *buff, SIZEOFBAMP_BASE_INFO);
    *buff += SIZEOFBAMP_BASE_INFO;
    basesSetBase(value,&readInto,(BYTE) count);
    count++;
  }
  return returnValue;
}

/*********************************************************
*NAME:          mapReadStarts
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 11/11/00
*PURPOSE:
*  Reads the player start information from a map file
*  into the player starts structure. Returns if the 
*  operation was successful or not
*
*ARGUMENTS:
*  buff  - Pointer to the read buffer
*  value - Pointer to the pillbox structure
*********************************************************/
bool mapReadStarts(BYTE **buff, starts *value) {
  int total;        /* Total number of entries to read */
  int count;        /* Looping variable */
  bool returnValue; /* Value to return */
  start readInto;   /* Item to read into */

  count = 1;
  total = startsGetNumStarts(value);
  returnValue = TRUE;
  while (count <= total && returnValue == TRUE && mapReadEOFCheck(*buff) == FALSE) {
    memcpy(&readInto, *buff, SIZEOFBMAP_START_INFO);
    *buff += SIZEOFBMAP_START_INFO;
    startsSetStart(value,&readInto,(BYTE) count);
    count++;
  }
  return returnValue;
}

/*********************************************************
*NAME:          mapProcessRun
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 11/11/00
*PURPOSE:
*  Process a single map run and puts values into map
*  data structure.
*  Returns whether operation was successful or not
*
*ARGUMENTS:
*  buff   - Pointer to the read buffer
*  value  - Pointer to the map data structure
*  elems  - Number of elements in the run
*  yValue - The y Map co-ordinate
*  startX - The start x co-ordinate
*  endX   - The end x co-ordinate
*********************************************************/
bool mapProcessRun(BYTE **buff,map *value,BYTE elems, MAP_Y yValue, BYTE startX, BYTE endX) {
  bool returnValue;  /* Value to return */
  bool needRead;     /* State variable - Do we need to read the next byte */
  mapRunState state; /* Current run state */  
  BYTE item;         /* Item being worked on */
  BYTE highNibble;   /* The low nibble of a byte */
  BYTE lowNibble;    /* The low nibble of a byte */
  BYTE len;          /* Length of the row */
  BYTE mapPos;       /* Possision on map */
  int count;         /* Looping variables */
  int count2;
  
  count = 0;
  returnValue = TRUE;
  state = highLen;
  mapPos = startX;

  item = **buff;
  (*buff)++;
  while (count < elems && mapReadEOFCheck(*buff) == FALSE) {
    needRead = FALSE;
    highNibble = lowNibble = item;
    /* Extract Nibbles */
    highNibble >>= MAP_SHIFT_SIZE;
    lowNibble <<= MAP_SHIFT_SIZE;
    lowNibble >>= MAP_SHIFT_SIZE;

    while (needRead == FALSE) {
      switch (state) {
      
      case highLen:
        len = highNibble;
        if (len < MAP_RUN_DIFF) {
          state = lowDiff;
          len++;
        } else {
          state = lowSame;
        }
        break;
      
      case lowLen:
        len = lowNibble;
        if (len < MAP_RUN_DIFF) {
          state = highDiff;
          len++;
        } else {
          state = highSame;
        }
        needRead = TRUE;
        break;
      
      case lowDiff:
        ((*value)->mapItem[mapPos][yValue]) = lowNibble;
        mapPos++;
        len--;
        if (len == 0) {
          state = highLen;
        } else {
          state = highDiff;
        }
        needRead = TRUE;
        break;

      case highDiff:
        ((*value)->mapItem[mapPos][yValue]) = highNibble;
        mapPos++;
        len--;
        if (len == 0) {
          state = lowLen;
        } else {
          state = lowDiff;
        }
        break;

      case lowSame:
        count2 = 0;
        while (count2 < (len-MAP_RUN_SAME)) {
          ((*value)->mapItem[mapPos+count2][yValue]) = lowNibble;
          count2++;
        }
        mapPos += count2;
        state = highLen;
        needRead = TRUE;
        break;

      case highSame:
        count2 = 0;
        while (count2 < (len-MAP_RUN_SAME)) {
          ((*value)->mapItem[mapPos+count2][yValue]) = highNibble;
          count2++;
        }
        mapPos += count2;
        state = lowLen;
        break;
      }
    }
    count++;
    item = **buff;
    (*buff)++;

  }
  (*buff)--;
  /* Check all read correctly */
  if (count != elems || mapPos != endX)  {
    returnValue = FALSE;
  }
  return returnValue;
}


/*********************************************************
*NAME:          mapReadRuns
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 11/11/00
*PURPOSE:
*  Reads the map runs into the map data structure
*  Returns if the operation was successful or not
*
*ARGUMENTS:
*  buff  - Pointer to the read buffer
*  value - Pointer to the map data structure
*********************************************************/
bool mapReadRuns(BYTE **buff, map *value) {
  bmapRunHeader runHead; /* The header of each run */
  bool returnValue;      /* Value to return */
  bool done;             /* Is all the runs read */
  bool ret;              /* Function return Value */

  returnValue = TRUE;
  done = FALSE;

  memcpy(&runHead, *buff, SIZEOFBMAP_RUN_HEADER);
  *buff += SIZEOFBMAP_RUN_HEADER;

  while (mapReadEOFCheck(*buff) == FALSE && done == FALSE) {
    if (runHead.datalen == 4  &&  runHead.y == MAP_ARRAY_LAST && runHead.startx ==  MAP_ARRAY_LAST && runHead.endx == MAP_ARRAY_LAST) {
    /* Finished reading */
      done = TRUE;
      returnValue = TRUE;
    } else {
      ret = mapProcessRun(buff, value, (BYTE) ((runHead.datalen)- SIZEOFBMAP_RUN_HEADER), (BYTE) runHead.y, (BYTE) runHead.startx, (BYTE) runHead.endx);
      if (ret == FALSE) {
        /* Function return failed */
        done = TRUE;
      }
    }
    memcpy(&runHead, *buff, SIZEOFBMAP_RUN_HEADER);
    *buff += SIZEOFBMAP_RUN_HEADER;
  }

  return returnValue;
}



/*********************************************************
*NAME:          mapRead
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 11/11/00
*PURPOSE:
*  Reads a map in.
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  buff    - Pointer to the read buffer
*  value   - Pointer to the map data structure
*  ss      - Pointer to the starts structure
*  bs      - Pointer to the bases structure
*  pb      - Pointer to the pillbox structure
*  length  - Length of the map in bytes
*********************************************************/
bool mapRead(BYTE **buff, map *value, pillboxes *pb, bases *bs, starts *ss, long length) {
  bool returnValue;   /* Value to return */

  mapOriginal = *buff;
  mapLength = length;
  returnValue = TRUE;
  if (strncmp(*buff,MAP_HEADER, strlen(MAP_HEADER)) != 0) {
    returnValue = FALSE;
  }
  *buff = *buff + strlen(MAP_HEADER); 

  if (returnValue == TRUE) {
    if (**buff != CURRENT_MAP_VERSION) {
      returnValue = FALSE;
    }
    (*buff)++;
  }
  if (returnValue == TRUE) {
    pillsSetNumPills(pb,**buff);
    (*buff)++;
    basesSetNumBases(bs,**buff); 
    (*buff)++;
    startsSetNumStarts(ss,**buff);
    (*buff)++;
    if (pillsGetNumPills(pb) > MAX_PILLS || basesGetNumBases(bs) > MAX_BASES || startsGetNumStarts(ss) > MAX_STARTS) {
      returnValue = FALSE;
    }
  }
  if (returnValue == TRUE) {
    returnValue = mapReadPills(buff, pb);
  }
  if (returnValue == TRUE) {
    returnValue = mapReadBases(buff, bs);
  }
  if (returnValue == TRUE) {
    returnValue = mapReadStarts(buff, ss);
  }
  if (returnValue == TRUE) {
    returnValue = mapReadRuns(buff, value);
  }
  *buff = mapOriginal;
  return returnValue;   
}


/*********************************************************
*NAME:          mapGetPos
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 28/10/98
*PURPOSE:
* Returns The value of a square in a map. Return 
* DEEP_SEA if value out of range
*
*ARGUMENTS:
*  value  - Pointer to the map data structure
*  xValue - The x co-ordinate
*  yValue - The y co-ordinate 
*********************************************************/
BYTE mapGetPos(map *value, BYTE xValue, BYTE yValue) {
  BYTE returnValue = DEEP_SEA; /* Value to return */

  return (*value)->mapItem[xValue][yValue];
}


/*********************************************************
*NAME:          mapSetPos
*AUTHOR:        John Morrison
*CREATION DATE: 11/11/00
*LAST MODIFIED: 11/11/00
*PURPOSE:
* Returns The value of a square in a map. Return 
* DEEP_SEA if value out of range
*
*ARGUMENTS:
*  value   - Pointer to the map data structure
*  xValue  - The x co-ordinate
*  yValue  - The y co-ordinate 
*  terrain - Terrain to set to
*********************************************************/
void mapSetPos(map *value, BYTE xValue, BYTE yValue, BYTE terrain) {
  (*value)->mapItem[xValue][yValue] = terrain;
}

/*********************************************************
*NAME:          mapWrite
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 11/11/00
*PURPOSE:
* Writes a map to the filename given
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  buff     - Pointer to data buffer
*  buffLen  - Pointer to the length of the buffer used
*  value    - Pointer to the map data structure
*  ss       - Pointer to the starts structure
*  bs       - Pointer to the bases structure
*  pb       - Pointer to the pillbox structure
*********************************************************/
bool mapWrite(BYTE *buff, long *buffLen, map *value, pillboxes *pb, bases *bs, starts *ss) {
  bool returnValue;   /* Value to return */
  BYTE numPills;      /* Number of pills on the map */
  BYTE numBases;      /* Number of bases on the map */
  BYTE numStarts;     /* Number of starts on the map */

  numPills = pillsGetNumPills(pb);
  numBases = basesGetNumBases(bs);
  numStarts = startsGetNumStarts(ss);

  /* Write header */
  strncpy(buff, MAP_HEADER, strlen(MAP_HEADER));
  *buffLen = strlen(MAP_HEADER);

  buff[*buffLen] = CURRENT_MAP_VERSION;
  (*buffLen)++;

  /* Put number of pills */
  buff[*buffLen] = numPills;
  (*buffLen)++;
  /* Put number of bases */
  buff[*buffLen] = numBases;
  (*buffLen)++;
  /* Put number of starts */
  buff[*buffLen] = numStarts;
  (*buffLen)++;

  /* Write pill locations */
  returnValue = mapWritePills(buff, buffLen, pb, numPills);

  /* Write bases locations */
  if (returnValue == TRUE) {
    returnValue = mapWriteBases(buff, buffLen, bs, numBases);
  }
  /* Write starts locations */
  if (returnValue == TRUE) {
    returnValue = mapWriteStarts(buff, buffLen, ss, numStarts);
  }

  if (returnValue == TRUE) {
    returnValue = mapWriteRuns(buff, buffLen, value);
  }

  return returnValue;   
}

/*********************************************************
*NAME:          mapWritePills
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 11/11/00
*PURPOSE:
* Writes the pillbox locations out
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  buff    - Pointer to the buffer to write to
*  buffLen - Pointer to the buffer length
*  pb      - Pointer to the pillbox structure
*  total   - Total number of pills to write
*********************************************************/
bool mapWritePills(BYTE *buff, long *buffLen, pillboxes *pb, BYTE total) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  pillbox item;     /* Pillbox information */
  
  returnValue = TRUE;
  count = 1;
  while (count <= total && returnValue == TRUE) {
    pillsGetPill(pb, &item, count);
    /* Write each pill out */
    buff[*buffLen] = item.x;
    (*buffLen)++;
    buff[*buffLen] = item.y;
    (*buffLen)++;
    buff[*buffLen] = item.owner;
    (*buffLen)++;
    buff[*buffLen] = item.armour;
    (*buffLen)++;
    buff[*buffLen] = item.speed;
    (*buffLen)++;
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          mapWriteBases
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 11/11/00
*PURPOSE:
* Writes the bases locations out
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  buff    - Pointer to the buffer to write to
*  buffLen - Pointer to the buffer length
*  bs      - Pointer to the pillbox structure
*  total   - Total number of bases to write
*********************************************************/
bool mapWriteBases(BYTE *buff, long *buffLen, bases *bs, BYTE total) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  base item;        /* The base item being saved */

  returnValue = TRUE;
  count = 1;
  while (count <= total && returnValue == TRUE) {
    basesGetBase(bs, &item, count);
    /* Write each base out */
    buff[*buffLen] = item.x;
    (*buffLen)++;
    buff[*buffLen] = item.y;
    (*buffLen)++;
    buff[*buffLen] = item.owner;
    (*buffLen)++;
    buff[*buffLen] = item.armour;
    (*buffLen)++;
    buff[*buffLen] = item.shells;
    (*buffLen)++;
    buff[*buffLen] = item.mines;
    (*buffLen)++;
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          mapWriteStarts
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 11/11/00
*PURPOSE:
* Writes the starts locations out
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  buff    - Pointer to the buffer to write to
*  buffLen - Pointer to the buffer length
*  ss      - Pointer to the starts structure
*  total   - Total number of starts to write
*********************************************************/
bool mapWriteStarts(BYTE *buff, long *buffLen, starts *ss, BYTE total) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  start item;       /* Item being saved */

  returnValue = TRUE;
  count = 1;
  
  while (count <= total && returnValue == TRUE) {
    startsGetStartStruct(ss, &item, count);
    /* Write each start out */
    buff[*buffLen] = item.x;
    (*buffLen)++;
    buff[*buffLen] = item.y;
    (*buffLen)++;
    buff[*buffLen] = item.dir;
    (*buffLen)++;
    count++;
  } 

  return returnValue;
}


/*********************************************************
*NAME:          mapWriteRuns
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 9/2/99
*PURPOSE:
* Writes out the series of map runs
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  buff    - Pointer to the buffer to write to
*  buffLen - Pointer to the buffer length
*  value   - Pointer to the map structure
*********************************************************/
bool mapWriteRuns(BYTE *buff, long *buffLen, map *value) {
  bool returnValue;         /* Value to return */
  bmapRun run;
  BYTE xPos;                /* Current position on the map */
  BYTE yPos;
  long len;                 /* Length of the run to write */

  returnValue = TRUE;
  xPos = 0;
  yPos = 0;
  while (yPos < 0xFF && returnValue == TRUE) {
    /* Process runs */
    len = mapPrepareRun(value, &run, &xPos, &yPos);
    if (yPos > 176) {
      yPos = yPos;
    }
    /* Write the run out */
    memcpy(buff+(*buffLen), &run, len);
    *buffLen += len;
  }

  return returnValue;
}

#define put_nibble(X) (!nibble_flag ? (nibble_flag = TRUE,  *nibble_data = (X)<<4) :\
                                      (nibble_flag = FALSE, *nibble_data++ |= (X) & 0xF))

/*********************************************************
*NAME:          mapPrepareRun
*AUTHOR:        John Morrison
*CREATION DATE:  9/2/99
*LAST MODIFIED: 10/2/99
*PURPOSE:
* Prepares a map run to write out. Returns the length of
* the run
*
*ARGUMENTS:
*  value - Pointer to the map structure
*  run   - Pointer to the map run structure
*  xPos  - Pointer to our current X position
*  yPos  - Pointer to our current Y position
*********************************************************/
long mapPrepareRun(map *value, bmapRun *run, BYTE *xPos, BYTE *yPos) {
	BYTE terrain;     /* Terrain under current Position */
	BYTE code;        /* Map code (ie identical/differnt etc) */
  BYTE x;           /* Temp variables to hold xPos and yPos */
	BYTE y;
	bool nibble_flag; /* High/Low nibble filter */
	BYTE *nibble_data; /* Holds run->data for nibble calculations */
  
  nibble_flag = FALSE;
  x = *xPos;
  y = *yPos;
  nibble_data = run->data;
	
	/* Search for non-DEEPSEA terrain */
  while (mapGetPos(value, x, y) == DEEP_SEA) {
    if (x < 0xFF) {
      x++; 
    } else if (y < MAP_ARRAY_LAST) { 
      x = 0; 
      y++; 
      if (y > 177) {
        y = y;
      }
    } else {
      break;
    }
  }
	run->startx = x;
	if (y < MAP_ARRAY_LAST) {
    terrain = mapGetPos(value, x, y);
		while(terrain != DEEP_SEA) {
			if (terrain == mapGetPos(value, (BYTE) (x+1), y)){
        /* Two squares are the same */
				code = MAP_CODE_IDENTICAL_START;
				x += MAP_CODE_IDENTICAL_SKIP;			/* skip over the two squares we have found */
				while (code < MAP_CODE_IDENTICAL_END && mapGetPos(value, x, y) == terrain) {
          code++;
          x++;
        }
				put_nibble(code);
				put_nibble(terrain);
			}	else {
				BYTE code = 0;	/* code 0 means 1 individual square */
				MAP_X ds = x++;	/* record where the difference run starts */
				while (code < MAP_CODE_DIFFERENT_END  && mapGetPos(value, x, y) != DEEP_SEA && mapGetPos(value, x, y) != mapGetPos(value, (BYTE) (x+1), y)) { 
          code++; 
          x++;
        }
				put_nibble(code);
        while (ds<x) {
          put_nibble(mapGetPos(value, ds++, y)); 
        }
      }
      terrain = mapGetPos(value, x, y);
		}
    if (nibble_flag == TRUE) {
      put_nibble(0);	/* round it up to whole number of bytes */
    }
  }
  

  *xPos = run->endx = x;
	*yPos = run->y    = y;
	return(run->datalen = nibble_data - (BYTE*)run);
}
