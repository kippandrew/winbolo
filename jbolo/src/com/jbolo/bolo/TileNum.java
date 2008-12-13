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


package com.jbolo.bolo;

/*
 * TileNum.java
 *
 * Created on October 5, 2003, 7:31 PM
 */

/** Static class containing tile position references
 * @author John MorrisonS
 */
public class TileNum extends Terrain {
    
    
    /* Defines the tile types */
    public static final int ROAD_HORZ = 17;
    public static final int ROAD_VERT = 18;
    public static final int ROAD_CORNER1 =19; /* The road corners */
    public static final int ROAD_CORNER2 =20;
    public static final int ROAD_CORNER3 =21;
    public static final int ROAD_CORNER4 =22;
    public static final int ROAD_CORNER5 =23; /* The 4 solid road corners */
    public static final int ROAD_CORNER6 =24;
    public static final int ROAD_CORNER7 =25;
    public static final int ROAD_CORNER8 =26;
    public static final int ROAD_SOLID= 27; /* Solid road */
    public static final int ROAD_CROSS= 28;
    public static final int ROAD_T1 =29;    /* T Junctions */
    public static final int ROAD_T2 =30;
    public static final int ROAD_T3 =31;
    public static final int ROAD_T4 =32;
    public static final int ROAD_WATER1 =33; /* 1 side over other normal */
    public static final int ROAD_WATER2 =34;
    public static final int ROAD_WATER3 =35;
    public static final int ROAD_WATER4 =36;
    public static final int ROAD_WATER5 =37; /* Corners on water */
    public static final int ROAD_WATER6 =38;
    public static final int ROAD_WATER7 =39;
    public static final int ROAD_WATER8 =40;
    public static final int ROAD_WATER9 =41; /* Horizontal water peice */
    public static final int ROAD_WATER10 =42; /* Vertical water peice */
    public static final int ROAD_WATER11 =43; /* Lone water peice */
    
    public static final int ROAD_SIDE1 =44; /* Single Side peices */
    public static final int ROAD_SIDE2 =45;
    public static final int ROAD_SIDE3 =46;
    public static final int ROAD_SIDE4 =47;
    
    public static final int BASE_GOOD =49;
    public static final int BASE_NEUTRAL= 122;
    public static final int BASE_EVIL =163;
    
    public static final int BUILD_SINGLE= 50;
    public static final int BUILD_HORZ= 51;
    public static final int BUILD_VERT= 52;
    public static final int BUILD_HORZEND1= 53; /* End peices */
    public static final int BUILD_HORZEND2= 54;
    public static final int BUILD_VERTEND1= 55;
    public static final int BUILD_VERTEND2= 56;
    public static final int BUILD_SOLID =57; /* Builing peices on all side */
    public static final int BUILD_CORNER1= 58; /* 4 corners */
    public static final int BUILD_CORNER2= 59;
    public static final int BUILD_CORNER3= 60;
    public static final int BUILD_CORNER4= 61;
    public static final int BUILD_L1 =62; /* L - Shaped Peices */
    public static final int BUILD_L2 =63;
    public static final int BUILD_L3 =64;
    public static final int BUILD_L4 =65;
    public static final int BUILD_T1 =66; /* T Shaped Peices  */
    public static final int BUILD_T2 =67;
    public static final int BUILD_T3 =68;
    public static final int BUILD_T4 =69;
    public static final int BUILD_CROSS =70;
    public static final int BUILD_SIDE1 =71; /* Sides */
    public static final int BUILD_SIDE2 =72;
    public static final int BUILD_SIDE3 =73;
    public static final int BUILD_SIDE4 =74;
    public static final int BUILD_SIDECORN1 =75; /* Sides and Corners */
    public static final int BUILD_SIDECORN2 =76;
    public static final int BUILD_SIDECORN3 =77;
    public static final int BUILD_SIDECORN4 =78;
    public static final int BUILD_SIDECORN5 =79;
    public static final int BUILD_SIDECORN6 =80;
    public static final int BUILD_SIDECORN7 =81;
    public static final int BUILD_SIDECORN8 =82;
    public static final int BUILD_SIDECORN9 =83;
    public static final int BUILD_SIDECORN10 =84;
    public static final int BUILD_SIDECORN11 =85;
    public static final int BUILD_SIDECORN12 =86;
    public static final int BUILD_SIDECORN13 =87;
    public static final int BUILD_SIDECORN14 =88;
    public static final int BUILD_SIDECORN15 =89;
    public static final int BUILD_SIDECORN16 =90;
    public static final int BUILD_TWIST1 =91; /* Strange half peices */
    public static final int BUILD_TWIST2 =92;
    public static final int BUILD_MOST1 =93; /* Most covered Also last 4 peices */
    public static final int BUILD_MOST2 =94;
    public static final int BUILD_MOST3 =95;
    public static final int BUILD_MOST4 =96;
    /* River Peices */
    public static final int RIVER_END1 =97;  /* The four end U peices */
    public static final int RIVER_END2 =98;
    public static final int RIVER_END3 =99;
    public static final int RIVER_END4 =100;
    public static final int RIVER_SOLID =101; /* Solid River Peice */
    public static final int RIVER_SURROUND= 102; /* River surrounded on all sides by buildings */
    public static final int RIVER_SIDE1 =103; /* River Side Peices eg - | | */
    public static final int RIVER_SIDE2 =104;
    public static final int RIVER_ONESIDE1 =105; /* One side land others water */
    public static final int RIVER_ONESIDE2 =106;
    public static final int RIVER_ONESIDE3 =107;
    public static final int RIVER_ONESIDE4 =108;
    public static final int RIVER_CORN1 =109; /* Four corner L shaped peices */
    public static final int RIVER_CORN2 =110;
    public static final int RIVER_CORN3 =111;
    public static final int RIVER_CORN4 =112;
    /* Deep Sea Peices */
    public static final int DEEP_SEA_SOLID =113;
    public static final int DEEP_SEA_CORN1 =114; /* Corner peices L Shaped */
    public static final int DEEP_SEA_CORN2 =115;
    public static final int DEEP_SEA_CORN3 =116;
    public static final int DEEP_SEA_CORN4 =117;
    public static final int DEEP_SEA_SIDE1 =118; /* Four Sides */
    public static final int DEEP_SEA_SIDE2 =119;
    public static final int DEEP_SEA_SIDE3 =120;
    public static final int DEEP_SEA_SIDE4 =121;
    
    /* Tank Positions */
    public static final int TANK_SELF_0 =0;
    public static final int TANK_SELF_1 =1;
    public static final int TANK_SELF_2 =2;
    public static final int TANK_SELF_3 =3;
    public static final int TANK_SELF_4 =4;
    public static final int TANK_SELF_5 =5;
    public static final int TANK_SELF_6 =6;
    public static final int TANK_SELF_7 =7;
    public static final int TANK_SELF_8 =8;
    public static final int TANK_SELF_9 =9;
    public static final int TANK_SELF_10 =10;
    public static final int TANK_SELF_11 =11;
    public static final int TANK_SELF_12 =12;
    public static final int TANK_SELF_13 =13;
    public static final int TANK_SELF_14 =14;
    public static final int TANK_SELF_15 =15;
    public static final int TANK_SELFBOAT_0 =16;
    public static final int TANK_SELFBOAT_1 =17;
    public static final int TANK_SELFBOAT_2 =18;
    public static final int TANK_SELFBOAT_3 =19;
    public static final int TANK_SELFBOAT_4 =20;
    public static final int TANK_SELFBOAT_5 =21;
    public static final int TANK_SELFBOAT_6 =22;
    public static final int TANK_SELFBOAT_7 =23;
    public static final int TANK_SELFBOAT_8 =24;
    public static final int TANK_SELFBOAT_9 =25;
    public static final int TANK_SELFBOAT_10= 26;
    public static final int TANK_SELFBOAT_11= 27;
    public static final int TANK_SELFBOAT_12= 28;
    public static final int TANK_SELFBOAT_13= 29;
    public static final int TANK_SELFBOAT_14= 30;
    public static final int TANK_SELFBOAT_15= 31;
    public static final int TANK_GOOD_0 =32;
    public static final int TANK_GOOD_1 =33;
    public static final int TANK_GOOD_2 =34;
    public static final int TANK_GOOD_3 =35;
    public static final int TANK_GOOD_4 =36;
    public static final int TANK_GOOD_5 =37;
    public static final int TANK_GOOD_6 =38;
    public static final int TANK_GOOD_7 =39;
    public static final int TANK_GOOD_8 =40;
    public static final int TANK_GOOD_9 =41;
    public static final int TANK_GOOD_10 =42;
    public static final int TANK_GOOD_11 =43;
    public static final int TANK_GOOD_12 =44;
    public static final int TANK_GOOD_13 =45;
    public static final int TANK_GOOD_14 =46;
    public static final int TANK_GOOD_15 =47;
    public static final int TANK_GOODBOAT_0 =48;
    public static final int TANK_GOODBOAT_1 =49;
    public static final int TANK_GOODBOAT_2 =50;
    public static final int TANK_GOODBOAT_3 =51;
    public static final int TANK_GOODBOAT_4 =52;
    public static final int TANK_GOODBOAT_5 =53;
    public static final int TANK_GOODBOAT_6 =54;
    public static final int TANK_GOODBOAT_7 =55;
    public static final int TANK_GOODBOAT_8 =56;
    public static final int TANK_GOODBOAT_9 =57;
    public static final int TANK_GOODBOAT_10 =58;
    public static final int TANK_GOODBOAT_11 =59;
    public static final int TANK_GOODBOAT_12 =60;
    public static final int TANK_GOODBOAT_13 =61;
    public static final int TANK_GOODBOAT_14 =62;
    public static final int TANK_GOODBOAT_15 =63;
    public static final int TANK_EVIL_0 =64;
    public static final int TANK_EVIL_1 =65;
    public static final int TANK_EVIL_2 =66;
    public static final int TANK_EVIL_3 =67;
    public static final int TANK_EVIL_4 =68;
    public static final int TANK_EVIL_5 =69;
    public static final int TANK_EVIL_6 =70;
    public static final int TANK_EVIL_7 =71;
    public static final int TANK_EVIL_8 =72;
    public static final int TANK_EVIL_9 =73;
    public static final int TANK_EVIL_10 =74;
    public static final int TANK_EVIL_11 =75;
    public static final int TANK_EVIL_12 =76;
    public static final int TANK_EVIL_13 =77;
    public static final int TANK_EVIL_14 =78;
    public static final int TANK_EVIL_15 =79;
    public static final int TANK_EVILBOAT_0 =80;
    public static final int TANK_EVILBOAT_1 =81;
    public static final int TANK_EVILBOAT_2 =82;
    public static final int TANK_EVILBOAT_3 =83;
    public static final int TANK_EVILBOAT_4 =84;
    public static final int TANK_EVILBOAT_5 =85;
    public static final int TANK_EVILBOAT_6 =86;
    public static final int TANK_EVILBOAT_7 =87;
    public static final int TANK_EVILBOAT_8 =88;
    public static final int TANK_EVILBOAT_9 =89;
    public static final int TANK_EVILBOAT_10= 90;
    public static final int TANK_EVILBOAT_11= 91;
    public static final int TANK_EVILBOAT_12= 92;
    public static final int TANK_EVILBOAT_13= 93;
    public static final int TANK_EVILBOAT_14= 94;
    public static final int TANK_EVILBOAT_15= 95;
    
    public static final int TANK_BOAT_ADD= 16;
    public static final int TANK_GOOD_ADD= 32; /* Add 32 to get to the begining of the allie tank tiles */
    public static final int TANK_EVIL_ADD= 64; /* Add 64 to get to the begining of the allie tank tiles */
    public static final int TANK_TRANSPARENT= 255;
    
    /* shells and explosions */
    public static final int SHELL_EXPLOSION8= 1;
    public static final int SHELL_EXPLOSION7= 2;
    public static final int SHELL_EXPLOSION6= 3;
    public static final int SHELL_EXPLOSION5= 4;
    public static final int SHELL_EXPLOSION4= 5;
    public static final int SHELL_EXPLOSION3= 6;
    public static final int SHELL_EXPLOSION2= 7;
    public static final int SHELL_EXPLOSION1= 8;
    public static final int SHELL_DIR0= 9;
    public static final int SHELL_DIR1= 10;
    public static final int SHELL_DIR2= 11;
    public static final int SHELL_DIR3= 12;
    public static final int SHELL_DIR4= 13;
    public static final int SHELL_DIR5= 14;
    public static final int SHELL_DIR6= 15;
    public static final int SHELL_DIR7= 16;
    public static final int SHELL_DIR8= 17;
    public static final int SHELL_DIR9= 18;
    public static final int SHELL_DIR10= 19;
    public static final int SHELL_DIR11= 20;
    public static final int SHELL_DIR12= 21;
    public static final int SHELL_DIR13= 22;
    public static final int SHELL_DIR14= 23;
    public static final int SHELL_DIR15= 24;
    
    /* Evil Pillbox Numbers */
    public static final int PILL_EVIL_15= 48;
    public static final int PILL_EVIL_14= 123;
    public static final int PILL_EVIL_13= 124;
    public static final int PILL_EVIL_12= 125;
    public static final int PILL_EVIL_11= 126;
    public static final int PILL_EVIL_10= 127;
    public static final int PILL_EVIL_9= 128;
    public static final int PILL_EVIL_8= 129;
    public static final int PILL_EVIL_7= 130;
    public static final int PILL_EVIL_6= 131;
    public static final int PILL_EVIL_5= 132;
    public static final int PILL_EVIL_4= 133;
    public static final int PILL_EVIL_3= 134;
    public static final int PILL_EVIL_2= 135;
    public static final int PILL_EVIL_1= 136;
    public static final int PILL_EVIL_0= 137;
    public static final int PILL_GOOD_15= 147;
    public static final int PILL_GOOD_14= 148;
    public static final int PILL_GOOD_13= 149;
    public static final int PILL_GOOD_12= 150;
    public static final int PILL_GOOD_11= 151;
    public static final int PILL_GOOD_10= 152;
    public static final int PILL_GOOD_9= 153;
    public static final int PILL_GOOD_8= 154;
    public static final int PILL_GOOD_7= 155;
    public static final int PILL_GOOD_6= 156;
    public static final int PILL_GOOD_5= 157;
    public static final int PILL_GOOD_4= 158;
    public static final int PILL_GOOD_3= 159;
    public static final int PILL_GOOD_2= 160;
    public static final int PILL_GOOD_1= 161;
    public static final int PILL_GOOD_0= 162;
    
    
    /* Forests */
    public static final int FOREST_SINGLE =164;
    public static final int FOREST_BR =165;
    public static final int FOREST_BL =166;
    public static final int FOREST_AL =167;
    public static final int FOREST_AR =168;
    public static final int FOREST_ABOVE =169;
    public static final int FOREST_BELOW =170;
    public static final int FOREST_LEFT =171;
    public static final int FOREST_RIGHT =172;
    
    /* Craters */
    public static final int CRATER_SINGLE =173;
    public static final int CRATER_BR =174;
    public static final int CRATER_BL =175;
    public static final int CRATER_AL =176;
    public static final int CRATER_AR =177;
    public static final int CRATER_ABOVE= 178;
    public static final int CRATER_BELOW= 179;
    public static final int CRATER_LEFT =180;
    public static final int CRATER_RIGHT= 181;
    
    /* Boats */
    public static final int BOAT_0 =138;
    public static final int BOAT_1 =139;
    public static final int BOAT_2 =140;
    public static final int BOAT_3 =141;
    public static final int BOAT_4 = 142;
    public static final int BOAT_5 = 143;
    public static final int BOAT_6 = 144;
    public static final int BOAT_7 = 145;
    public static final int BOAT_8 = 146;
    
    
    /* LGM */
    public static final int LGM0 = 0;
    public static final int LGM1 = 1;
    public static final int LGM2 = 2;
    public static final int LGM3 = 3;
    
    
    
    /** Creates a new instance of TileNum */
    public TileNum() {
    }
    
}
