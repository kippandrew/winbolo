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
 
 //Header file for Brain writers. It is identical to Bolo's brain.h
//Except where marked by a 'jm' comment/
//Additionally all Mac specific #defines have been removed.

#ifndef _BRAIN_H //jm
#define _BRAIN_H //jm

#include <winsock2.h>
//jm - Winsock2 header must be included because WinBolo's GAMEID 
//struct contains the server details.

#define local static
#define export
#define import extern


typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned long  u_long;


typedef u_char  NIBBLE;	// to be interpreted as four bits
typedef u_char  BYTE;
typedef u_short WORD;
typedef struct { u_char c[36]; } u_char36;


//jm
typedef BYTE Boolean;
#define TRUE 1
#define FALSE 0
//endjm
typedef BYTE MAP_X,   MAP_Y;
typedef WORD WORLD_X, WORLD_Y;

// The various accessible tank control functions
enum
	{
	KEY_faster=0, KEY_slower, KEY_turnleft, KEY_turnright,
	KEY_morerange, KEY_lessrange, KEY_shoot, KEY_dropmine,
	KEY_TankView, KEY_PillView
	};
#define setkey(CONTROLVECTOR, COMMAND) CONTROLVECTOR |= (1<<COMMAND)
#define testkey(CONTROLVECTOR, COMMAND) ((CONTROLVECTOR & (1<<COMMAND)) != 0)

typedef BYTE TERRAIN;
enum
	{
	BUILDING=0, RIVER, SWAMP, CRATER, ROAD, FOREST, RUBBLE, GRASS,
	HALFBUILDING, BOAT, DEEPSEA, REFBASE_T, PILLBOX_T,
	TERRAIN_UNKNOWN,
	NUM_TERRAINS,
	TERRAIN_MASK     = 0x0F,
	TERRAIN_TANK_VIS = 0x10,
	TERRAIN_PILL_VIS = 0x20,
	TERRAIN_UNUSED   = 0x40,
	TERRAIN_MINE     = 0x80
	};

#define is_wet(A) ((A) == RIVER || (A) == BOAT || (A) == DEEPSEA)

typedef BYTE BUILDMODE;
enum
	{
	BUILDMODE_FARM=1, BUILDMODE_ROAD,
	BUILDMODE_BUILD, BUILDMODE_PBOX, BUILDMODE_MINE
	};

typedef struct
	{
	MAP_X x;
	MAP_Y y;
	BUILDMODE action;
	} BuildInfo;

// Farming gets you 4 tree units.
// Roads, bridges and buildings take 2 units, boats take 20
// Placing a pillbox takes 4 units, repairing takes proportionately less

#define NEUTRAL_PLAYER 0xFF
// player id used to identify neutral bases and pillboxes
#define FORESTVISUAL 0x30
#define MINRANGE 2
#define MAXRANGE 14		// In HALF map-squares
#define MAX_PILL_ARMOUR 15
#define MAX_BASE_SHELLS 90
#define MAX_BASE_MINES  90
#define MAX_BASE_ARMOUR 90
#define ARMOUR_COST 5
#define BASE_RESIST_SHELLS (ARMOUR_COST)
#define BASE_RESIST_TANKS (ARMOUR_COST*2)
#define MIN_BASE_ARMOUR (BASE_RESIST_TANKS + ARMOUR_COST - 1)
// A base bust have one armour unit or more to resist a shell --
// otherwise it is 'transparent' to shells.
// It must have two armour units to or more resist a tank.
// This is so that, after shooting a base down to 'transparency',
// there is some reasonble time window in which to drive onto it.

typedef u_long PlayerBitMap;


//jm Changed GAMEID struct to WinBolo's GAMEID struct.
typedef struct {
  struct in_addr serveraddress;
  unsigned short serverport;
  u_long start_time;
} GAMEID;

enum { GameType_open=1, GameType_tournament, GameType_strict_tment };

#define GAMEINFO_HIDDENMINES 0x80
#define GAMEINFO_ALLMINES_VISIBLE 0xC0

typedef struct
	{
	u_char36 mapname;
	GAMEID gameid;
	BYTE gametype;
	BYTE hidden_mines;
	// has the value GAMEINFO_HIDDENMINES or GAMEINFO_ALLMINES_VISIBLE
	BYTE allow_AI;
	BYTE assist_AI;
	long start_delay;
	long time_limit;
	} GAMEINFO;

// *********************************************************************

typedef u_short OBJECT;
enum
	{
	OBJECT_TANK=0,
	OBJECT_SHOT,
	OBJECT_PILLBOX,
	OBJECT_REFBASE,
	OBJECT_BUILDMAN,
	OBJECT_PARACHUTE
	};

#define OBJECT_HOSTILE 1	// Object is hostile to us
#define OBJECT_NEUTRAL 2	// Object is not loyal to any other player
// Note that being neutral means that an object has no particular loyalty
// to any player -- whether it is hostile or friendly to us is an orthogonal
// question. Currently, neutral refuelling bases are friendly to everyone
// and neutral pillboxes are hostile to everyone.

typedef struct
	{
	OBJECT object;
	WORLD_X x;
	WORLD_Y y;
	WORD idnum;
	BYTE direction;
	BYTE info;
	} ObjectInfo;

// For pillboxes and refuelling bases, the 'direction' field
// actually holds the armour strength
#define pillbox_strength direction
#define refbase_strength direction

typedef struct
	{
	u_short sender;
	PlayerBitMap *receivers;
	u_char *message;
	} MessageInfo;

// This header file describes Bolo BrainInfo structure version 3
#define CURRENT_BRAININFO_VERSION 3

enum { BRAIN_OPEN=0, BRAIN_CLOSE, BRAIN_THINK, BRAIN_MENU=200 }; //jm changed BRAIN_MENU = 200

typedef struct
	{
	u_short BoloVersion;	// two hex bytes, eg. 0x0098 means version 0.98
	u_short InfoVersion;	// current version of the BrainInfo structure is 1
	void *userdata;		// Initially points to address of your CODE resource
	u_short PrefsVRefNum;
	u_char *PrefsFileName;
	u_short operation;		// 0=OPEN, 1=CLOSE, 2=THINK, 200+ menu
	u_short menu_item;

	// Interface providing information about the Bolo world
	
	u_short max_players;	  // Players are numbered from 0 to max_players-1
	u_short max_pillboxes;	// Pillboxes are numbered from 0 to max_pillboxes-1
	u_short max_refbases;	  // Bases are numbered from 0 to max_refbases-1
	u_short player_number;	// Who Am I?
	u_short num_players;	  // How many players currently active in this game?
	u_char36 **playernames;	// Array of pointers to pascal strings
	PlayerBitMap *allies;	  // Who you are currently allied to

	WORLD_X tankx;
	WORLD_Y tanky;
	
	BYTE direction;
	BYTE speed;				// 64 is top speed on road, 48 on grass,
      							// 24 in forest, 12 on rubble, crater, water etc.
	BYTE inboat;			// non-zero means currently on boat
	BYTE hidden;			// non-zero means hidden inside forest

	BYTE shells;			// Range 0-40
	BYTE mines;				// Range 0-40
	BYTE armour;			// Range 0-8
	BYTE trees;				// Range 0-40

	BYTE carriedpills;		// Number of pillboxes the tank is carrying
	BYTE carriedbases;		// Number of refuelling bases (zero in current versions)
	WORD padding2;

	BYTE gunrange;			  // in units of half map squares
	BYTE reload;			    // non-zero means cannot fire shell immediately
	BYTE newtank;			    // Set initially, and each time tank is killed
	BYTE tankobstructed;	// Set if tank has hit obstacle
	
	ObjectInfo *base;		  // will be NULL if no friendly base nearby
	BYTE base_shells;
	BYTE base_mines;
	BYTE base_armour;
	BYTE padding3;			  // unused at present

	BYTE man_status;		  // 0 = in tank, 1 = dead, other = outside
	BYTE man_direction;
	WORLD_X man_x;
	WORLD_Y man_y;
	BYTE manobstructed;		// 0 = free, 1 = touching wall, 2 = completely stuck
	BYTE padding4;			  // unused at present

	WORD *pillview;			  // top bit set (0x8000) means view is from tank
	MAP_Y view_top;			  // coordinate of topmost square of current view
	MAP_X view_left;		  // coordinate of leftmost square of current view
	BYTE  view_height;		// height of the view, in map squares
	BYTE  view_width;	  	// width of the view, in map squares
	TERRAIN *viewdata;		// view_width*view_height bytes of terrain data

	WORD padding5;			  // unused at present
	u_short num_objects;	// number of moving objects visible
	ObjectInfo *objects;	// array information about those objects

	MessageInfo *message;	// Set if you recevied a message

	// Interface to control the tank
	u_long *holdkeys;
	u_long *tapkeys;
	BuildInfo *build;
	PlayerBitMap *wantallies; // Who you want to be allied to
  //jm - Please Note: the wantallies field is currently unsupported.
	PlayerBitMap *messagedest;
	u_char *sendmessage;
	
	// New additions for version 3
	const TERRAIN *theWorld;// Pointer to 256x256 map squares (arranged in rows)
	GAMEINFO gameinfo;		  // The game options
	
	} BrainInfo;


#endif // jm _BRAIN_H
