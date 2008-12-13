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
*Name:          tilenum
*Filename:      tilenum.h 
*Author:        John Morrison
*Creation Date: 28/10/98
*Last Modified: 26/3/99
*Purpose:
*  Defines tile peices with a unique number
*********************************************************/

#ifndef TILE_NUM_H
#define TILE_NUM_H

/* Defines */

/* Defines the tile types */
#define ROAD_HORZ 17
#define ROAD_VERT 18
#define ROAD_CORNER1 19 /* The road corners */
#define ROAD_CORNER2 20
#define ROAD_CORNER3 21
#define ROAD_CORNER4 22
#define ROAD_CORNER5 23 /* The 4 solid road corners */
#define ROAD_CORNER6 24
#define ROAD_CORNER7 25
#define ROAD_CORNER8 26
#define ROAD_SOLID 27 /* Solid road */
#define ROAD_CROSS 28
#define ROAD_T1 29    /* T Junctions */
#define ROAD_T2 30
#define ROAD_T3 31
#define ROAD_T4 32
#define ROAD_WATER1 33 /* 1 side over other normal */
#define ROAD_WATER2 34
#define ROAD_WATER3 35
#define ROAD_WATER4 36
#define ROAD_WATER5 37 /* Corners on water */
#define ROAD_WATER6 38
#define ROAD_WATER7 39
#define ROAD_WATER8 40
#define ROAD_WATER9 41 /* Horizontal water peice */
#define ROAD_WATER10 42 /* Vertical water peice */
#define ROAD_WATER11 43 /* Lone water peice */

#define ROAD_SIDE1 44 /* Single Side peices */
#define ROAD_SIDE2 45
#define ROAD_SIDE3 46
#define ROAD_SIDE4 47 

#define BASE_GOOD 49
#define BASE_NEUTRAL 122
#define BASE_EVIL 163

#define BUILD_SINGLE 50
#define BUILD_HORZ 51
#define BUILD_VERT 52
#define BUILD_HORZEND1 53 /* End peices */
#define BUILD_HORZEND2 54
#define BUILD_VERTEND1 55
#define BUILD_VERTEND2 56
#define BUILD_SOLID 57 /* Builing peices on all side */
#define BUILD_CORNER1 58 /* 4 corners */
#define BUILD_CORNER2 59
#define BUILD_CORNER3 60
#define BUILD_CORNER4 61
#define BUILD_L1 62 /* L - Shaped Peices */
#define BUILD_L2 63
#define BUILD_L3 64
#define BUILD_L4 65
#define BUILD_T1 66 /* T Shaped Peices  */
#define BUILD_T2 67
#define BUILD_T3 68
#define BUILD_T4 69
#define BUILD_CROSS 70
#define BUILD_SIDE1 71 /* Sides */
#define BUILD_SIDE2 72
#define BUILD_SIDE3 73
#define BUILD_SIDE4 74
#define BUILD_SIDECORN1 75 /* Sides and Corners */
#define BUILD_SIDECORN2 76
#define BUILD_SIDECORN3 77
#define BUILD_SIDECORN4 78
#define BUILD_SIDECORN5 79
#define BUILD_SIDECORN6 80
#define BUILD_SIDECORN7 81
#define BUILD_SIDECORN8 82
#define BUILD_SIDECORN9 83
#define BUILD_SIDECORN10 84
#define BUILD_SIDECORN11 85
#define BUILD_SIDECORN12 86
#define BUILD_SIDECORN13 87
#define BUILD_SIDECORN14 88
#define BUILD_SIDECORN15 89
#define BUILD_SIDECORN16 90
#define BUILD_TWIST1 91 /* Strange half peices */
#define BUILD_TWIST2 92
#define BUILD_MOST1 93 /* Most covered Also last 4 peices */
#define BUILD_MOST2 94
#define BUILD_MOST3 95
#define BUILD_MOST4 96
/* River Peices */
#define RIVER_END1 97  /* The four end U peices */
#define RIVER_END2 98
#define RIVER_END3 99
#define RIVER_END4 100
#define RIVER_SOLID 101 /* Solid River Peice */
#define RIVER_SURROUND 102 /* River surrounded on all sides by buildings */
#define RIVER_SIDE1 103 /* River Side Peices eg - | | */
#define RIVER_SIDE2 104 
#define RIVER_ONESIDE1 105 /* One side land others water */
#define RIVER_ONESIDE2 106
#define RIVER_ONESIDE3 107
#define RIVER_ONESIDE4 108
#define RIVER_CORN1 109 /* Four corner L shaped peices */
#define RIVER_CORN2 110
#define RIVER_CORN3 111
#define RIVER_CORN4 112
/* Deep Sea Peices */
#define DEEP_SEA_SOLID 113 
#define DEEP_SEA_CORN1 114 /* Corner peices L Shaped */
#define DEEP_SEA_CORN2 115
#define DEEP_SEA_CORN3 116
#define DEEP_SEA_CORN4 117
#define DEEP_SEA_SIDE1 118 /* Four Sides */
#define DEEP_SEA_SIDE2 119
#define DEEP_SEA_SIDE3 120
#define DEEP_SEA_SIDE4 121

/* Tank Positions */
#define TANK_SELF_0 0
#define TANK_SELF_1 1
#define TANK_SELF_2 2
#define TANK_SELF_3 3
#define TANK_SELF_4 4
#define TANK_SELF_5 5
#define TANK_SELF_6 6
#define TANK_SELF_7 7
#define TANK_SELF_8 8
#define TANK_SELF_9 9
#define TANK_SELF_10 10
#define TANK_SELF_11 11
#define TANK_SELF_12 12
#define TANK_SELF_13 13
#define TANK_SELF_14 14
#define TANK_SELF_15 15
#define TANK_SELFBOAT_0 16
#define TANK_SELFBOAT_1 17
#define TANK_SELFBOAT_2 18
#define TANK_SELFBOAT_3 19
#define TANK_SELFBOAT_4 20
#define TANK_SELFBOAT_5 21
#define TANK_SELFBOAT_6 22
#define TANK_SELFBOAT_7 23
#define TANK_SELFBOAT_8 24
#define TANK_SELFBOAT_9 25
#define TANK_SELFBOAT_10 26
#define TANK_SELFBOAT_11 27
#define TANK_SELFBOAT_12 28
#define TANK_SELFBOAT_13 29
#define TANK_SELFBOAT_14 30
#define TANK_SELFBOAT_15 31
#define TANK_GOOD_0 32
#define TANK_GOOD_1 33
#define TANK_GOOD_2 34
#define TANK_GOOD_3 35
#define TANK_GOOD_4 36
#define TANK_GOOD_5 37
#define TANK_GOOD_6 38
#define TANK_GOOD_7 39
#define TANK_GOOD_8 40
#define TANK_GOOD_9 41
#define TANK_GOOD_10 42
#define TANK_GOOD_11 43
#define TANK_GOOD_12 44
#define TANK_GOOD_13 45
#define TANK_GOOD_14 46
#define TANK_GOOD_15 47
#define TANK_GOODBOAT_0 48
#define TANK_GOODBOAT_1 49
#define TANK_GOODBOAT_2 50
#define TANK_GOODBOAT_3 51
#define TANK_GOODBOAT_4 52
#define TANK_GOODBOAT_5 53
#define TANK_GOODBOAT_6 54
#define TANK_GOODBOAT_7 55
#define TANK_GOODBOAT_8 56
#define TANK_GOODBOAT_9 57
#define TANK_GOODBOAT_10 58
#define TANK_GOODBOAT_11 59
#define TANK_GOODBOAT_12 60
#define TANK_GOODBOAT_13 61
#define TANK_GOODBOAT_14 62
#define TANK_GOODBOAT_15 63
#define TANK_EVIL_0 64
#define TANK_EVIL_1 65
#define TANK_EVIL_2 66
#define TANK_EVIL_3 67
#define TANK_EVIL_4 68
#define TANK_EVIL_5 69
#define TANK_EVIL_6 70
#define TANK_EVIL_7 71
#define TANK_EVIL_8 72
#define TANK_EVIL_9 73
#define TANK_EVIL_10 74
#define TANK_EVIL_11 75
#define TANK_EVIL_12 76
#define TANK_EVIL_13 77
#define TANK_EVIL_14 78
#define TANK_EVIL_15 79
#define TANK_EVILBOAT_0 80
#define TANK_EVILBOAT_1 81
#define TANK_EVILBOAT_2 82
#define TANK_EVILBOAT_3 83
#define TANK_EVILBOAT_4 84
#define TANK_EVILBOAT_5 85
#define TANK_EVILBOAT_6 86
#define TANK_EVILBOAT_7 87
#define TANK_EVILBOAT_8 88
#define TANK_EVILBOAT_9 89
#define TANK_EVILBOAT_10 90
#define TANK_EVILBOAT_11 91
#define TANK_EVILBOAT_12 92
#define TANK_EVILBOAT_13 93
#define TANK_EVILBOAT_14 94
#define TANK_EVILBOAT_15 95

#define TANK_BOAT_ADD 16
#define TANK_GOOD_ADD 32 /* Add 32 to get to the begining of the allie tank tiles */
#define TANK_EVIL_ADD 64 /* Add 64 to get to the begining of the allie tank tiles */
#define TANK_TRANSPARENT 255

/* shells and explosions */
#define SHELL_EXPLOSION8 1
#define SHELL_EXPLOSION7 2
#define SHELL_EXPLOSION6 3
#define SHELL_EXPLOSION5 4
#define SHELL_EXPLOSION4 5
#define SHELL_EXPLOSION3 6
#define SHELL_EXPLOSION2 7
#define SHELL_EXPLOSION1 8
#define SHELL_DIR0 9
#define SHELL_DIR1 10
#define SHELL_DIR2 11
#define SHELL_DIR3 12
#define SHELL_DIR4 13
#define SHELL_DIR5 14
#define SHELL_DIR6 15
#define SHELL_DIR7 16
#define SHELL_DIR8 17
#define SHELL_DIR9 18
#define SHELL_DIR10 19
#define SHELL_DIR11 20
#define SHELL_DIR12 21
#define SHELL_DIR13 22
#define SHELL_DIR14 23
#define SHELL_DIR15 24

/* Evil Pillbox Numbers */
#define PILL_EVIL_15 48
#define PILL_EVIL_14 123
#define PILL_EVIL_13 124
#define PILL_EVIL_12 125
#define PILL_EVIL_11 126
#define PILL_EVIL_10 127
#define PILL_EVIL_9 128
#define PILL_EVIL_8 129
#define PILL_EVIL_7 130
#define PILL_EVIL_6 131
#define PILL_EVIL_5 132
#define PILL_EVIL_4 133
#define PILL_EVIL_3 134
#define PILL_EVIL_2 135
#define PILL_EVIL_1 136
#define PILL_EVIL_0 137
#define PILL_GOOD_15 147
#define PILL_GOOD_14 148
#define PILL_GOOD_13 149
#define PILL_GOOD_12 150
#define PILL_GOOD_11 151
#define PILL_GOOD_10 152
#define PILL_GOOD_9 153
#define PILL_GOOD_8 154
#define PILL_GOOD_7 155
#define PILL_GOOD_6 156
#define PILL_GOOD_5 157
#define PILL_GOOD_4 158
#define PILL_GOOD_3 159
#define PILL_GOOD_2 160
#define PILL_GOOD_1 161
#define PILL_GOOD_0 162


/* Forests */
#define FOREST_SINGLE 164
#define FOREST_BR 165
#define FOREST_BL 166
#define FOREST_AL 167
#define FOREST_AR 168
#define FOREST_ABOVE 169
#define FOREST_BELOW 170
#define FOREST_LEFT 171
#define FOREST_RIGHT 172

/* Boats */
#define BOAT_0 138
#define BOAT_1 139
#define BOAT_2 140
#define BOAT_3 141
#define BOAT_4 142
#define BOAT_5 143
#define BOAT_6 144
#define BOAT_7 145
#define BOAT_8 146


/* LGM */
#define LGM0 0
#define LGM1 1
#define LGM2 2
#define LGM3 3

#endif /* TILE_NUM_H */
