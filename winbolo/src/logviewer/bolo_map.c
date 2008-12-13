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
#include "blocks.h"

/* Prototypes */
bool logIsEOF();
int logReadBytes(BYTE *buff, int len);


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

  if (xValue < MAP_ARRAY_SIZE && yValue < MAP_ARRAY_SIZE) {
    returnValue = (*value)->mapItem[xValue][yValue];
  }
  return returnValue;
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
  BYTE returnValue = DEEP_SEA; /* Value to return */

  if (xValue < MAP_ARRAY_SIZE && yValue < MAP_ARRAY_SIZE) {
    (*value)->mapItem[xValue][yValue] = terrain;
  }
}

/*********************************************************
*NAME:          mapIsMine
*AUTHOR:        John Morrison
*CREATION DATE: 22/1/99
*LAST MODIFIED: 13/3/99
*PURPOSE:
* Returns whether a map square is mines or not
*
*ARGUMENTS:
*  value   - Pointer to the map data structure
*  xValue  - The x co-ordinate
*  yValue  - The y co-ordinate 
*********************************************************/
bool mapIsMine(map *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */
  
  returnValue = FALSE;
  
  if (xValue <= MAP_MINE_EDGE_LEFT || xValue >= MAP_MINE_EDGE_RIGHT || yValue <= MAP_MINE_EDGE_TOP || yValue >= MAP_MINE_EDGE_BOTTOM) {
    returnValue = TRUE;
  } else if ((*value)->mapItem[xValue][yValue] >= MINE_START && (*value)->mapItem[xValue][yValue] <= MINE_END) {
    returnValue = TRUE;
  }
  return returnValue;
}

/*********************************************************
*NAME:          mapProcessRun
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 21/10/98
*PURPOSE:
*  Process a single map run and puts values into map
*  data structure.
*  Returns whether operation was successful or not
*
*ARGUMENTS:
*  value  - Pointer to the map data structure
*  elems  - Number of elements in the run
*  yValue - The y Map co-ordinate
*  startX - The start x co-ordinate
*  endX   - The end x co-ordinate
*********************************************************/
bool mapProcessRun(map *value,BYTE elems, MAP_Y yValue, BYTE startX, BYTE endX) {
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
  len = 0;

  logReadBytes(&item, 1);
  while (count < elems && !logIsEOF()) {
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
        mapPos = (BYTE) (mapPos + count2);
        state = highLen;
        needRead = TRUE;
        break;
      case highSame:
        count2 = 0;
        while (count2 < (len-MAP_RUN_SAME)) {
          ((*value)->mapItem[mapPos+count2][yValue]) = highNibble;
          count2++;
        }
        mapPos = (BYTE) (mapPos + count2);
        state = lowLen;
        break;
      }
    }
    count++;
    if (count < elems) {
      logReadBytes(&item, 1);
    }
  }
  /* Check all read correctly */
  if (count != elems || mapPos != endX)  { /*  || mapPos != endX || count2 == EOF*/ 
    returnValue = FALSE;
  }
  return returnValue;
}

/*********************************************************
*NAME:          mapReadRuns
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 21/10/98
*PURPOSE:
*  Reads the map runs into the map data structure
*  Returns if the operation was successful or not
*
*ARGUMENTS:
*  value - Pointer to the map data structure
*********************************************************/
bool mapReadRuns(map *value) {
  bmapRunHeader runHead; /* The header of each run */
  size_t bytesRead;         /* The number of bytes read from the header */
  bool returnValue;      /* Value to return */
  bool done;             /* Is all the runs read */
  bool ret;              /* Function return Value */

  returnValue = TRUE;
  done = FALSE;


  bytesRead = logReadBytes((BYTE *) &runHead, SIZEOFBMAP_RUN_HEADER);

  while (!logIsEOF()  && done == FALSE) {
    if (bytesRead != SIZEOFBMAP_RUN_HEADER) {
    /* Something bad happened reading */
      done = TRUE;
      returnValue = FALSE;
    } else if (runHead.datalen == SIZEOFBMAP_RUN_HEADER && runHead.y == MAP_ARRAY_LAST && runHead.startx == MAP_ARRAY_LAST && runHead.endx == MAP_ARRAY_LAST) {
    /* Finished reading */
      done = TRUE;
      returnValue = TRUE;
    } else {
      ret = mapProcessRun(value,(BYTE) ((runHead.datalen)- SIZEOFBMAP_RUN_HEADER),runHead.y, runHead.startx, runHead.endx);
      if (ret == FALSE) {
        /* Function return failed */
        done = TRUE;
        returnValue = FALSE;
      }
      bytesRead = logReadBytes((BYTE *) &runHead, SIZEOFBMAP_RUN_HEADER);

    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          mapWrite
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 9/2/99
*PURPOSE:
* Writes a map to the filename given
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  fileName - Pointer to a string containing the file name
*  value    - Pointer to the map data structure
*  ss       - Pointer to the starts structure
*  bs       - Pointer to the bases structure
*  pb       - Pointer to the pillbox structure
*  saveOwnerships - Save ownerships or not
*********************************************************/
bool mapWrite(char *fileName, map *value, pillboxes *pb, bases *bs, starts *ss, bool saveOwnerships) {
  FILE *fp;           /* File pointer */
  bool returnValue;   /* Value to return */
  int ret;            /* Function return value */
  BYTE numPills;      /* Number of pills on the map */
  BYTE numBases;      /* Number of bases on the map */
  BYTE numStarts;     /* Number of starts on the map */

  returnValue = TRUE;
  numPills = pillsGetNumPills(pb);
  numBases = basesGetNumBases(bs);
  numStarts = startsGetNumStarts(ss);

  fp = fopen(fileName,"wb");
  if (fp == NULL) {
    returnValue = FALSE;
  }
  /* Write header */
  if (returnValue == TRUE && fp) {
    ret = fputs(MAP_HEADER, fp);
    if (ret == EOF) {
      returnValue = FALSE;
    }
  }
  
  /* Write map version */
  if (returnValue == TRUE && fp) {
    ret = fputc(CURRENT_MAP_VERSION, fp);
    if (ret == EOF) {
      returnValue = FALSE;
    }
  }

  /* Put number of pills */
  if (returnValue == TRUE && fp) {
    ret = fputc(numPills, fp);
    if (ret == EOF) {
      returnValue = FALSE;
    }
  }

  /* Put number of bases */
  if (returnValue == TRUE && fp) {
    ret = fputc(numBases, fp);
    if (ret == EOF) {
      returnValue = FALSE;
    }
  }

  /* Put number of starts */
  if (returnValue == TRUE && fp) {
    ret = fputc(numStarts, fp);
    if (ret == EOF) {
      returnValue = FALSE;
    }
  }

  /* Write pill locations */
  if (returnValue == TRUE && fp) {
    returnValue = mapWritePills(fp, pb, numPills, saveOwnerships);
  }
  /* Write bases locations */
  if (returnValue == TRUE && fp) {
    returnValue = mapWriteBases(fp, bs, numBases, saveOwnerships);
  }
  /* Write starts locations */
  if (returnValue == TRUE && fp) {
    returnValue = mapWriteStarts(fp, ss, numStarts);
  }

  if (returnValue == TRUE && fp) {
    returnValue = mapWriteRuns(fp,value);
  }
  if (fp) {
    fclose(fp);
  }
  return returnValue;
   
}

/*********************************************************
*NAME:          mapWritePills
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 9/2/99
*PURPOSE:
* Writes the pillbox locations out
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  fp    - File pointer
*  pb    - Pointer to the pillbox structure
*  total - Total number of pills to write
*  saveOwnerships - Save ownerships or not
*********************************************************/
bool mapWritePills(FILE *fp, pillboxes *pb, BYTE total, bool saveOwnerships) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  pillbox item;     /* Pillbox information */
  int ret;          /* Function return Value */
  
  returnValue = TRUE;
  count = 1;
  while (count <= total && returnValue == TRUE) {
    pillsGetPill(pb, &item, count);
    /* Write each pill out */
    ret = fputc(item.x, fp);
    if (ret != item.x) {
      returnValue = FALSE;
    }
    if (returnValue == TRUE) {
      ret = fputc(item.y, fp);
      if (ret != item.y) {
        returnValue = FALSE;
      }
    }
    if (returnValue == TRUE) {
      if (saveOwnerships == TRUE) {
        ret = fputc(item.owner, fp);
        if (ret != item.owner) {
          returnValue = FALSE;
        }
      } else {
        ret = fputc(NEUTRAL, fp);
        if (ret != NEUTRAL) {
          returnValue = FALSE;
        }
      }
    }
    if (returnValue == TRUE) {
      ret = fputc(item.armour, fp);
      if (ret != item.armour) {
        returnValue = FALSE;
      }
    }
    if (returnValue == TRUE) {
      ret = fputc(item.speed, fp);
      if (ret != item.speed) {
        returnValue = FALSE;
      }
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          mapWriteBases
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 9/2/99
*PURPOSE:
* Writes the bases locations out
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  fp    - File pointer
*  bs    - Pointer to the pillbox structure
*  total - Total number of bases to write
*  saveOwnerships - Save ownerships or not
*********************************************************/
bool mapWriteBases(FILE *fp, bases *bs, BYTE total, bool saveOwnerships) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  base item;        /* The base item being saved */
  int ret;          /* Function return */

  returnValue = TRUE;
  count = 1;
  while (count <= total && returnValue == TRUE) {
    basesGetBase(bs, &item, count);
    /* Write each base out */
    ret = fputc(item.x, fp);
    if (ret != item.x) {
      returnValue = FALSE;
    }
    if (returnValue == TRUE) {
      ret = fputc(item.y, fp);
      if (ret != item.y) {
        returnValue = FALSE;
      }
    }
    if (returnValue == TRUE) {
      if (saveOwnerships == TRUE) {
        ret = fputc(item.owner, fp);
        if (ret != item.owner) {
          returnValue = FALSE;
        }
      } else {
        ret = fputc(NEUTRAL, fp);
        if (ret != NEUTRAL) {
          returnValue = FALSE;
        }
      }
    }
    if (returnValue == TRUE) {
      ret = fputc(item.armour, fp);
      if (ret != item.armour) {
        returnValue = FALSE;
      }
    }
    if (returnValue == TRUE) {
      ret = fputc(item.shells, fp);
      if (ret != item.shells) {
        returnValue = FALSE;
      }
    }
    if (returnValue == TRUE) {
      ret = fputc(item.mines, fp);
      if (ret != item.mines) {
        returnValue = FALSE;
      }
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
*NAME:          mapWriteStarts
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 9/2/99
*PURPOSE:
* Writes the starts locations out
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  fp    - File pointer
*  ss    - Pointer to the starts structure
*  total - Total number of starts to write
*********************************************************/
bool mapWriteStarts(FILE *fp, starts *ss, BYTE total) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  start item;       /* Item being saved */
  int ret;          /* Function return */

  returnValue = TRUE;
  count = 1;
  
  while (count <= total && returnValue == TRUE) {
    startsGetStartStruct(ss, &item, count);
    /* Write each start out */
    ret = fputc(item.x, fp);
    if (ret != item.x) {
      returnValue = FALSE;
    }
    if (returnValue == TRUE) {
      ret = fputc(item.y, fp);
      if (ret != item.y) {
        returnValue = FALSE;
      }
    }
    if (returnValue == TRUE) {
      ret = fputc(item.dir, fp);
      if (ret != item.dir) {
        returnValue = FALSE;
      }
    }
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
*  fp    - File pointer
*  value - Pointer to the map structure
*********************************************************/
bool mapWriteRuns(FILE *fp, map *value) {
  bool returnValue;         /* Value to return */
  bmapRun run;
  BYTE xPos;                /* Current position on the map */
  BYTE yPos;
  long len;                 /* Length of the run to write */
  size_t ret;                  /* Function return */

  returnValue = TRUE;
  xPos = 0;
  yPos = 0;
  while (yPos < 0xFF && returnValue == TRUE) {
    /* Process runs */
    len = mapPrepareRun(value, &run, &xPos, &yPos);
    /* Write the run out */
    ret = fwrite(&run, (size_t) len, 1, fp);
    if (ret !=  1) {
      returnValue = FALSE;
    }
  }

  return returnValue;
}

#define put_nibble(X) (!nibble_flag ? (nibble_flag = TRUE,  *nibble_data = (X)<<4) : (nibble_flag = FALSE, *nibble_data++ |= (X) & 0xF))

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
