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



#ifndef MAP_H
#define MAP_H

/* Defines */
#include "bases.h"
#include "global.h"
#include "pillbox.h"
#include "starts.h"
#include "types.h"

#define LENGTH_ID 8 /* Number of charecters in the "BMAPBOLO" id tag */
#define CURRENT_MAP_VERSION 1 /* The current Version is */

/* Number of bytes in structures */
#define SIZEOFBMAP_PILL_INFO 5
#define SIZEOFBAMP_BASE_INFO 6
#define SIZEOFBMAP_START_INFO 3
#define SIZEOFBMAP_RUN_HEADER 4

/* Map Edges for mines */
#define MAP_MINE_EDGE_LEFT  20
#define MAP_MINE_EDGE_RIGHT 236
#define MAP_MINE_EDGE_TOP  20
#define MAP_MINE_EDGE_BOTTOM 236

/* Maximums */
/* Not required
#define MAX_PILLS 16
#define MAX_BASES  16

*/

/* For identical terrain this is the first map "code" */
#define MAP_CODE_IDENTICAL_START 8
/* Last length */
#define MAP_CODE_IDENTICAL_END 15
/* We want to skip two items for identical map squres */
#define MAP_CODE_IDENTICAL_SKIP 2

#define MAP_CODE_DIFFERENT_END 7

#define MAP_ARRAY_SIZE 256 /* maps are 256x256 units square */
#define MAP_ARRAY_LAST 255 /* Last item in a map is 255 or 0xFF */

/* All map files should begin with BMAPBOLO */
#define MAP_HEADER "BMAPBOLO" 

/* Must shift 4 bits to get a nibble */
#define MAP_SHIFT_SIZE 4 

/* length of six for map to be same */
#define MAP_RUN_SAME 6 
#define MAP_RUN_DIFF 8

/* Speeds of tanks over various terrains */
#define MAP_SPEED_TDEEPSEA 3 /* Tank in deep sea kills tank */
#define MAP_SPEED_TBUILDING 0
#define MAP_SPEED_TRIVER 3
#define MAP_SPEED_TSWAMP 3
#define MAP_SPEED_TCRATER 3
#define MAP_SPEED_TROAD 16
#define MAP_SPEED_TFOREST 6
#define MAP_SPEED_TRUBBLE 3
#define MAP_SPEED_TGRASS 12
#define MAP_SPEED_THALFBUILDING 0
#define MAP_SPEED_TBOAT 16 /* Was 3 - check ?? */
#define MAP_SPEED_TREFBASE 16 /* Refueling base */
#define MAP_SPEED_TPILLBOX 0 /* Pillbox */


/* Speeds of man over various terrains */
#define MAP_MANSPEED_TDEEPSEA 0 /* Tank in deep sea kills tank */
#define MAP_MANSPEED_TBUILDING 0
#define MAP_MANSPEED_TRIVER 0
#define MAP_MANSPEED_TSWAMP 4
#define MAP_MANSPEED_TCRATER 4
#define MAP_MANSPEED_TROAD 16
#define MAP_MANSPEED_TFOREST 8
#define MAP_MANSPEED_TRUBBLE 4
#define MAP_MANSPEED_TGRASS 16
#define MAP_MANSPEED_THALFBUILDING 0
#define MAP_MANSPEED_TBOAT 16 /* Was 3 - check ?? */
#define MAP_MANSPEED_TREFBASE 16 /* Refueling base */
#define MAP_MANSPEED_TPILLBOX 0 /* Pillbox */


/* Speeds of tanks turn
Given in bradians - 256 bradians in a circle */
#define MAP_TURN_TDEEPSEA 0.5 /* Tank in deep sea kills tank */
#define MAP_TURN_TBUILDING 0
#define MAP_TURN_TRIVER 0.25
#define MAP_TURN_TSWAMP 0.25 
#define MAP_TURN_TCRATER 0.25 
#define MAP_TURN_TROAD 1 
#define MAP_TURN_TFOREST 0.5 
#define MAP_TURN_TRUBBLE 0.25 
#define MAP_TURN_TGRASS 1 
#define MAP_TURN_THALFBUILDING 0
#define MAP_TURN_TBOAT 1
#define MAP_TURN_TREFBASE  1 /* Refueling base */
#define MAP_TURN_TPILLBOX 0 /* Pillbox */

/* Type definitions */

/* Defines the state of map reading */
typedef enum {
  highLen,  /* The next high nibble is a length */
  lowLen,   /* The next low nibble is a length */
  highDiff, /* The next high nibble is a item and the length indicates differnt items */
  lowDiff,  /* The next low nibble is a item and the length indicates differnt items */
  highSame, /* The next high nibble is a item and the same items for the length */
  lowSame   /* The next low nibble is a item and the same items for the length */
} mapRunState;


typedef struct {
  BYTE datalen;	/* length of the data for this run INCLUDING this 4 byte header */
  MAP_Y y;		/* y co-ordinate of this run. */
  MAP_X startx;	/* first square of the run */
  MAP_X endx;	/* last square of run + 1 (ie first deep sea square after run) */
} bmapRunHeader;


typedef struct {
	BYTE datalen;	/* length of the data for this run, INCLUDING this 4 byte header */
	MAP_Y y;		  /* y co-ordinate of this run. */
	BYTE  startx;	/* first square of the run */
	BYTE endx;		/* last square of run + 1 */
	BYTE data[0xFF]; /* actual length of data is always much less than 0xFF */
} bmapRun;


/* Net map structure */
#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define MapNetTail(list) ((list)->next);

/* The maximum amount of time to wait for the server to authorise this change */
#define MAP_MAX_SERVER_WAIT 3 



/* Prototypes */

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
void mapCreate(map *value);

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
void mapDestroy(map *value);

/*********************************************************
*NAME:          mapReadPills
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 21/10/98
*PURPOSE:
*  Reads the pill information from a map file
*  into the pill structure. Returns if the 
*  operation was successful or not
*
*ARGUMENTS:
*  fp    - Pointer to the file being read from
*  value - Pointer to the pillbox structure
*********************************************************/
bool mapReadPills(FILE *fp, pillboxes *value);

/*********************************************************
*NAME:          mapReadBases
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 21/10/98
*PURPOSE:
*  Reads the base information from a map file
*  into the base structure. Returns if the 
*  operation was successful or not
*
*ARGUMENTS:
*  fp    - Pointer to the file being read from
*  value - Pointer to the pillbox structure
*********************************************************/
bool mapReadBases(FILE *fp, bases *value);

/*********************************************************
*NAME:          mapReadStarts
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 21/10/98
*PURPOSE:
*  Reads the player start information from a map file
*  into the player starts structure. Returns if the 
*  operation was successful or not
*
*ARGUMENTS:
*  fp    - Pointer to the file being read from
*  value - Pointer to the pillbox structure
*********************************************************/
bool mapReadStarts(FILE *fp, starts *value);

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
*  fp     - Pointer to the file being read from
*  value  - Pointer to the map data structure
*  elems  - Number of elements in the run
*  yValue - The y Map co-ordinate
*  startX - The start x co-ordinate
*  endX   - The end x co-ordinate
*********************************************************/
bool mapProcessRun(FILE *fp,map *value,BYTE elems, MAP_Y yValue, BYTE startX, BYTE endX);

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
*  fp    - Pointer to the file being read from
*  value - Pointer to the map data structure
*********************************************************/
bool mapReadRuns(FILE *fp, map *value);


/*********************************************************
*NAME:          mapRead
*AUTHOR:        John Morrison
*CREATION DATE: 21/10/98
*LAST MODIFIED: 21/10/98
*PURPOSE:
*  Reads a map in.
* Returns if the operation was successful or not
*
*ARGUMENTS:
*  fileNam - Pointer to a string containing the file name
*  value   - Pointer to the map data structure
*  ss      - Pointer to the starts structure
*  bs      - Pointer to the bases structure
*  pb      - Pointer to the pillbox structure
*********************************************************/
bool mapRead(char *fileName, map *value, pillboxes *pb, bases *bs, starts *ss);

/*********************************************************
*NAME:          mapGetPos
*AUTHOR:        John Morrison
*CREATION DATE: 28/10/98
*LAST MODIFIED: 30/12/2008
*PURPOSE:
* Returns The value of a square in a map. Return 
* DEEP_SEA if value out of range
*
*ARGUMENTS:
*  value  - Pointer to the map data structure
*  xValue - The x co-ordinate
*  yValue - The y co-ordinate 
*********************************************************/
BYTE mapGetPos(map *value, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          mapGetSpeed
*AUTHOR:        John Morrison
*CREATION DATE:  7/11/98
*LAST MODIFIED: 15/01/02
*PURPOSE:
* Returns the speed of the tank for a given map square
*
*ARGUMENTS:
*  value     - Pointer to the map data structure
*  pb        - Pointer to the pillboxes structure
*  bs        - Pointer to the bases structures
*  xValue    - The x co-ordinate
*  yValue    - The y co-ordinate 
*  onBoat    - Is the tank on a boat or not?
*  playerNum - Player Number of this player 
*********************************************************/
BYTE mapGetSpeed(map *value, pillboxes *pb, bases *bs, BYTE xValue, BYTE yValue, bool onBoat, BYTE playerNum);

/*********************************************************
*NAME:          mapGetManSpeed
*AUTHOR:        John Morrison
*CREATION DATE:  7/11/98
*LAST MODIFIED: 15/01/02
*PURPOSE:
* Returns The speed of the tank for a given map square
*
*ARGUMENTS:
*  value     - Pointer to the map data structure
*  pb        - Pointer to the pillboxes structure
*  bs        - Pointer to the bases structures
*  xValue    - The x co-ordinate
*  yValue    - The y co-ordinate 
*  playerNum - Player Number of this player
*********************************************************/
BYTE mapGetManSpeed(map *value, pillboxes *pb, bases *bs, BYTE xValue, BYTE yValue, BYTE playerNum);

/*********************************************************
*NAME:          mapGetTurnRate
*AUTHOR:        John Morrison
*CREATION DATE:  7/11/98
*LAST MODIFIED: 15/1/02
*PURPOSE:
* Returns The turn rate of the tank for a given 
* map square
*
*ARGUMENTS:
*  value  - Pointer to the map data structure
*  pb     - Pointer to the pillboxes structure
*  bs     - Pointer to the bases structures
*  xValue - The x co-ordinate
*  yValue - The y co-ordinate 
*  onBoat - Is the tank on a boat or not?
*  playerNum - Player Number who is requesting the turn
*********************************************************/
TURNTYPE mapGetTurnRate(map *value, pillboxes *pb, bases *bs, BYTE xValue, BYTE yValue, bool onBoat, BYTE playerNum);

/*********************************************************
*NAME:          mapIsPassable
*AUTHOR:        John Morrison
*CREATION DATE:  29/12/98
*LAST MODIFIED:  29/12/98
*PURPOSE:
* Returns whether the map square is passable or not
*
*ARGUMENTS:
*  value  - Pointer to the map data structure
*  xValue - The x co-ordinate
*  yValue - The y co-ordinate 
*  onBoat - Is the item on a boat 
*********************************************************/
bool mapIsPassable(map *value, BYTE xValue, BYTE yValue, bool onBoat);

/*********************************************************
*NAME:          mapSetPos
*AUTHOR:        John Morrison
*CREATION DATE: 30/12/98
*LAST MODIFIED:  31/7/00
*PURPOSE:
* Sets a position on the map
*
*ARGUMENTS:
*  value    - Pointer to the map data structure
*  xValue   - The x co-ordinate
*  yValue   - The y co-ordinate 
*  terrain  - The new terrain type
*  needSend - Used in network. True if we should
*             request the server make this change
*             False indicates the server should make it
* mineClear - Set to TRUE if we should just set the map
*             to the terrain. This is to remove the mines
*             from under bases on start up
*********************************************************/
void mapSetPos(map *value, BYTE xValue, BYTE yValue, BYTE terrain, bool needSend, bool mineClear);

/*********************************************************
*NAME:          mapIsLand
*AUTHOR:        John Morrison
*CREATION DATE: 6/1/99
*LAST MODIFIED: 6/1/99
*PURPOSE:
* Returns whether a map square is land or not 
*
*ARGUMENTS:
*  value   - Pointer to the map data structure
*  pb      - Pointer to the pillboxs structure
*  bs      - Pointer to the bases structure
*  xValue  - The x co-ordinate
*  yValue  - The y co-ordinate 
*********************************************************/
bool mapIsLand(map *value, pillboxes *pb, bases *bs, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          mapIsMine
*AUTHOR:        John Morrison
*CREATION DATE: 22/1/99
*LAST MODIFIED: 22/1/99
*PURPOSE:
* Returns whether a map square is mines or not
*
*ARGUMENTS:
*  value   - Pointer to the map data structure
*  xValue  - The x co-ordinate
*  yValue  - The y co-ordinate 
*********************************************************/
bool mapIsMine(map *value, BYTE xValue, BYTE yValue);

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
*********************************************************/
bool mapWrite(char *fileName, map *value, pillboxes *pb, bases *bs, starts *ss);

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
*  fileNam - Pointer to a string containing the file name
*  pb      - Pointer to the pillbox structure
*  total   - Total number of pills to write
*********************************************************/
bool mapWritePills(FILE *fp, pillboxes *pb, BYTE total);

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
*********************************************************/
bool mapWriteBases(FILE *fp, bases *bs, BYTE total);

/*********************************************************
*NAME:          mapWriteBases
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
bool mapWriteStarts(FILE *fp, starts *ss, BYTE total);

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
bool mapWriteRuns(FILE *fp, map *value);

/*********************************************************
*NAME:          mapPrepareRun
*AUTHOR:        John Morrison
*CREATION DATE: 9/2/99
*LAST MODIFIED: 9/2/99
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
long mapPrepareRun(map *value, bmapRun *run, BYTE *xPos, BYTE *yPos);

/*********************************************************
*NAME:          mapNetAdd
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED: 30/10/99
*PURPOSE:
* Adds a item to the mapNet structure. If an item already
* exists at that position it repaces it with the new
* terrain.
*
*ARGUMENTS:
*  value    - Pointer to the map structure
*  mx       - Current X position
*  my       - Current Y position
*  terrain  - Terrain to place
*  needSend - Should we send this update?
*********************************************************/
void mapNetAdd(map *value, BYTE mx, BYTE my, BYTE terrain, bool needSend);

/*********************************************************
*NAME:          mapNetUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED: 19/11/99
*PURPOSE:
* Updates the time the items have been waiting for the 
* server to authenticate them. If it reaches the expiry
* date then it is restored.
*
*ARGUMENTS:
*  value   - Pointer to map structure
*  pb      - Pointer to the pillboxes strucuture
*  bs      - Pointer to the bases strucuture
*********************************************************/
void mapNetUpdate(map *value, pillboxes *pb, bases *bs);

/*********************************************************
*NAME:          mapNetPacket
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED: 23/2/99
*PURPOSE:
* A packet has arrived. Here is a peice of map info in it.
*
*ARGUMENTS:
*  value   - Pointer to map structure
*  mx      - X position to add
*  my      - Y position
*  terrain - Terrain to place
*********************************************************/
void mapNetPacket(map *value, BYTE mx, BYTE my, BYTE terrain);

/*********************************************************
*NAME:          mapNetMakePacket
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Make the map part of the packet. Returns the data length.
* Is destructive on the data
*
*ARGUMENTS:
*  map  - Pointer to the map structure
*  buff - Buffer to hold data
*********************************************************/
BYTE mapNetMakePacket(map *value, BYTE *buff);

/*********************************************************
*NAME:          mapNetClientPacket
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* Make the client map part of the packet. Returns the 
* data length.
*
*ARGUMENTS:
*  map  - Pointer to the map structure
*  buff - Buffer to hold data
*********************************************************/
BYTE mapNetClientPacket(map *value, BYTE *buff);

/*********************************************************
*NAME:          mapMakeNetRun
*AUTHOR:        John Morrison
*CREATION DATE: 28/2/99
*LAST MODIFIED: 24/4/99
*PURPOSE:
* Makes a map network run at yPos. A network run is an 
* compress array of the bytes from 20 to 236 for the next
* 5 vertical map rows. Returns the size of the compressed
* data.
*
*ARGUMENTS:
*  value - Pointer to the map structure
*  buff  - Buffer to hold data
*  yPos  - Y position of the run
*********************************************************/
int mapMakeNetRun(map *value, BYTE *buff, BYTE yPos);

/*********************************************************
*NAME:          mapSetNetRun
*AUTHOR:        John Morrison
*CREATION DATE: 28/2/99
*LAST MODIFIED:  7/1/00
*PURPOSE:
* Sets the map to the network run at yPos. A network run 
* is an array of the bytes from 20 to 236
*
*ARGUMENTS:
*  value   - Pointer to the map structure
*  buff    - Buffer that contains data
*  yPos    - Y position of the run
*  dataLen - Length of the data
*********************************************************/
void mapSetNetRun(map *value, BYTE *buff, BYTE yPos, int dataLen);

/*********************************************************
*NAME:          mapNetIncomingItem
*AUTHOR:        John Morrison
*CREATION DATE: 3/11/99
*LAST MODIFIED: 9/11/99
*PURPOSE:
* A incoming map item has come from the server. If it is
* in the waitinf for confirmation buffer remove it, else
* if it exists in the incoming buffer replace it with the
* new value otherwise add it to the incoming buffer
*
*ARGUMENTS:
*  value   - Pointer to the map structure
*  mx      - X position to add
*  my      - Y position
*  terrain - Terrain to place
*********************************************************/
void mapNetIncomingItem(map *value, BYTE mx, BYTE my, BYTE terrain);

/*********************************************************
*NAME:          mapNetCheckWater
*AUTHOR:        John Morrison
*CREATION DATE: 19/11/99
*LAST MODIFIED: 19/11/99
*PURPOSE:
* Checks an square updated through mapNetUpdate to see if
* it should be filled to overcome the mines problem.
*
*ARGUMENTS:
*  value  - Pointer to map structure
*  pb     - Pointer to the pillboxes strucuture
*  bs     - Pointer to the bases strucuture
*  xValue - X Value to check
*  yValue - Y Value to check
*********************************************************/
void mapNetCheckWater(map *value, pillboxes *pb, bases *bs, BYTE xValue, BYTE yValue);

/*********************************************************
*NAME:          mapLoadCompressedMap
*AUTHOR:        John Morrison
*CREATION DATE: 1/5/99
*LAST MODIFIED: 1/5/99
*PURPOSE:
*  Reads a map in via a compressed map structure. Returns 
*  if the operation was successful or not
*
*ARGUMENTS:
*  value    - Pointer to the map data structure
*  ss       - Pointer to the starts structure
*  bs       - Pointer to the bases structure
*  pb       - Pointer to the pillbox structure
*  input    - Pointer to the data buffer
*  inputLen - Size of the buffer
*********************************************************/
bool mapLoadCompressedMap(map *value, pillboxes *pb, bases *bs, starts *ss, BYTE *input, int inputLen);

/*********************************************************
*NAME:          mapSaveCompressedMap
*AUTHOR:        John Morrison
*CREATION DATE: 1/5/99
*LAST MODIFIED: 1/5/99
*PURPOSE:
*  Saves a map to a compressed map structure. Returns 
*  compressed data length
*
*ARGUMENTS:
*  value    - Pointer to the map data structure
*  ss       - Pointer to the starts structure
*  bs       - Pointer to the bases structure
*  pb       - Pointer to the pillbox structure
*  output   - Pointer to the data buffer
*********************************************************/
int mapSaveCompressedMap(map *value, pillboxes *pb, bases *bs, starts *ss, BYTE *output);

/*********************************************************
*NAME:          mapCenter
*AUTHOR:        John Morrison
*CREATION DATE: 13/6/00
*LAST MODIFIED: 13/6/00
*PURPOSE:
*  Centers the map file and everything on it.
*
*ARGUMENTS:
*  value   - Pointer to the map data structure
*  ss      - Pointer to the starts structure
*  bs      - Pointer to the bases structure
*  pb      - Pointer to the pillbox structure
*********************************************************/
void mapCenter(map *value, pillboxes *pb, bases *bs, starts *ss);

#endif /* MAP_H */
