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


package com.jbolo.bolo.map;

import java.io.*;
import com.jbolo.bolo.GameWorld;
import com.jbolo.bolo.Starts;
import com.jbolo.bolo.Pillboxes;
import com.jbolo.bolo.Bases;
import com.jbolo.bolo.Start;
import com.jbolo.bolo.Pillbox;
import com.jbolo.bolo.Base;
import com.jbolo.bolo.Terrain;
import com.jbolo.bolo.Utils;
import com.jbolo.bolo.GameItems;
import com.jbolo.bolo.Log;
import com.jbolo.bolo.compression.*;

public class BoloMap {
    
    /** Version of map files read in by client */
    public static final byte MAP_VERSION = 1;
    
    
    /** Attributes */
    public static final int MAP_SQUARE_MIDDLE = 128;
    public static final int MAP_ARRAY_SIZE = 256;
    /** Map names are 36 charectors */
    public static final byte MAP_STR_SIZE = 36;
    public static final byte MAP_MINE_EDGE_LEFT = 20;
    public static final int MAP_MINE_EDGE_RIGHT = 236;
    public static final byte MAP_MINE_EDGE_TOP = 20;
    public static final int MAP_MINE_EDGE_BOTTOM = 236;
    public static final byte MAP_SHIFT_SIZE = 4;
    /** Speed of a tank in the sea */
    public static final byte MAP_SPEED_TDEEPSEA = 3;
    /** Speed of a tank on a building */
    public static final byte MAP_SPEED_TBUILDING = 0;
    /** Speed of a tank in a river */
    public static final byte MAP_SPEED_TRIVER = 3;
    /** Speed of a tank in a swamp */
    public static final byte MAP_SPEED_TSWAMP = 3;
    /** Speed of a tank in a crater */
    public static final byte MAP_SPEED_TCRATER = 3;
    /** Speed of a tank in a forest */
    public static final byte MAP_SPEED_TROAD = 16;
    /** Speed of a tank in a forest */
    public static final byte MAP_SPEED_TFOREST = 6;
    /** Speed of a tank on rubble */
    public static final byte MAP_SPEED_TRUBBLE = 3;
    /** Speed of a tank on grass */
    public static final byte MAP_SPEED_TGRASS = 12;
    /** Speed of a tank on a boat */
    public static final byte MAP_SPEED_TBOAT = 16;
    /** Speed of a tank on a half building */
    public static final byte MAP_SPEED_THALFBUILDING = 0;
    /** Speed of a tank on a base */
    public static final byte MAP_SPEED_TREFBASE = 16;
    /** Speed of a tank on a pillbox */
    public static final byte MAP_SPEED_TPILLBOX = 16;
    /** Speed of a LGM in the sea */
    public static final byte MAP_MANSPEED_TDEEPSEA = 0;
    /** Speed of a LGM on a building */
    public static final byte MAP_MANSPEED_TBUILDING = 0;
    /** Speed of a LGM in a river */
    public static final byte MAP_MANSPEED_TRIVER = 0;
    /** Speed of a LGM in a swamp */
    public static final byte MAP_MANSPEED_TSWAMP = 4;
    /** Speed of a LGM in a crater */
    public static final byte MAP_MANSPEED_TCRATER = 4;
    /** Speed of a LGM on road */
    public static final byte MAP_MANSPEED_TROAD = 16;
    /** Speed of a LGM in a forest */
    public static final byte MAP_MANSPEED_TFOREST = 8;
    /** Speed of a LGM on rubble */
    public static final byte MAP_MANSPEED_TRUBBLE = 4;
    /** Speed of a LGM on grass */
    public static final byte MAP_MANSPEED_TGRASS = 16;
    /** Speed of a LGM on a half building */
    public static final byte MAP_MANSPEED_THALFBUILDING = 0;
    /** Speed of a LGM on a boat */
    public static final byte MAP_MANSPEED_TBOAT = 16;
    /** Speed of a LGM on a base */
    public static final byte MAP_MANSPEED_TREFBASE = 16;
    /** Speed of a LGM on a pillbox */
    public static final byte MAP_MANSPEED_TPILLBOX = 0;
    
    /* Speeds of tanks turn
Given in bradians - 256 bradians in a circle */
    private static float MAP_TURN_TDEEPSEA = 0.5f; /* Tank in deep sea kills tank */
    private static float MAP_TURN_TBUILDING = 0f;
    private static float MAP_TURN_TRIVER = 0.25f;
    private static float MAP_TURN_TSWAMP =0.25f;
    private static float MAP_TURN_TCRATER = 0.25f;
    private static float MAP_TURN_TROAD = 1;
    private static float MAP_TURN_TFOREST = 0.5f;
    private static float MAP_TURN_TRUBBLE = 0.25f;
    private static float MAP_TURN_TGRASS = 1;
    private static float MAP_TURN_THALFBUILDING = 0;
    private static float MAP_TURN_TBOAT = 1;
    private static float MAP_TURN_TREFBASE = 1; /* Refueling base */
    private static float MAP_TURN_TPILLBOX = 0; /* Pillbox */
    
    private static int MAP_MAX_SERVER_WAIT = 200;
    private static byte MAP_CODE_IDENTICAL_START = 8;
    private static byte MAP_CODE_IDENTICAL_END = 15;
    private static byte MAP_CODE_IDENTICAL_SKIP = 2;
    private static byte MAP_CODE_DIFFERENT_END = 7;
    private static String MAP_HEADER = "BMAPBOLO";
    private static byte MAP_RUN_SAME = 6;
    private static byte MAP_RUN_DIFF = 8;
    private static int MAP_ARRAY_LAST = 255;
    
    
    /* Defines the state of map reading */
    private static final int RUN_STATE_HIGH_LEN = 0;  /* The next high nibble is a length */
    private static final int RUN_STATE_LOW_LEN = 1;   /* The next low nibble is a length */
    private static final int RUN_STATE_HIGH_DIFF = 2; /* The next high nibble is a item and the length indicates differnt items */
    private static final int RUN_STATE_LOW_DIFF = 3;  /* The next low nibble is a item and the length indicates differnt items */
    private static final int RUN_STATE_HIGH_SAME = 4; /* The next high nibble is a item and the same items for the length */
    private static final int RUN_STATE_LOW_SAME = 5;   /* The next low nibble is a item and the same items for the length */
    
    /** GameWorld that created this BoloMap */
    private GameWorld gw;
    
    /** Array of positions on the map */
    private int mapItem[][];
    
    /** MapNet for network games */
    private MapNet mapNet;
    
    /** Constructor
     * @param gw The game world that created this BoloMap
     */
    public BoloMap(GameWorld gw) {
        int count; // Looping variable
        int count2;
        mapItem = new int[256][256];
        
        mapNet = new MapNet(gw, this);
        this.gw = gw;
        
        for (count=0;count<256;count++) {
            for (count2=0;count2<256;count2++) {
                mapItem[count][count2] = Terrain.DEEP_SEA;
            }
        }
    }
    
    
    /**
     * reads a Bolo Map file in
     *
     * @param fileName The filename of the bolo map to load
     * @param sss Starts object to load starts into
     * @param bss Bases object to load starts into
     * @param pbs Pillboxes object to load starts into
     * @return if the map loaded successfully
     */
    public boolean read(String fileName, Starts sss, Bases bss, Pillboxes pbs) {
        boolean returnValue = true; /* Value to return */
        FileInputStream read; // Input stream used to load the file
        byte header[];
        int posRead;
        
        try {
            bss.setNumItems(0);
            pbs.setNumItems(0);
            sss.setNumItems(0);
            
            /* Read the header */
            header = new byte[8];
            read = new FileInputStream(fileName);
            posRead = read.read(header, 0, 8);
            if (posRead != 8) {
                read.close();
                return false;
            }
            
            if (BoloMap.MAP_HEADER.equals(new String(header)) == false) {
                read.close();
                return false;
            }
            
            posRead = read.read(header, 0, 1);
            if (posRead != MAP_VERSION) {
                read.close();
                return false;
            }
            
            if (header[0] != 1) {
                read.close();
                return false;
            }
            
            /* Read in Number of items */
            posRead = read.read(header, 0, 3);
            if (posRead != 3) {
                read.close();
                return false;
            }
            
            pbs.setNumItems(Utils.ConvertByteToInt(header[0]));
            bss.setNumItems(Utils.ConvertByteToInt(header[1]));
            sss.setNumItems(Utils.ConvertByteToInt(header[2]));
            
            
            if (sss.getNumItems() > GameItems.MAX_ITEMS || bss.getNumItems() > GameItems.MAX_ITEMS || pbs.getNumItems() > GameItems.MAX_ITEMS) {
                read.close();
                return false;
            }
            
            if (readPills((InputStream) read, pbs) == false) {
                read.close();
                return false;
            }
            
            if (readBases((InputStream) read, bss) == false) {
                read.close();
                return false;
            }
            
            if (readStarts((InputStream) read, sss) == false) {
                read.close();
                return false;
            }
            
            if (readRuns((InputStream) read) == false) {
                read.close();
                return false;
            }
            
            read.close();
            center(pbs, bss, sss);
            
        } catch (Exception e) {
            e.printStackTrace();
            returnValue = false;
            try {
                bss.setNumItems(0);
                pbs.setNumItems(0);
                sss.setNumItems(0);
            } catch (Exception  e1) {
                // Can ignore because its set by US and is an OK value
            }
        }
        return returnValue;
    }
    
    /**
     * Gets the map terrain at position x, y
     *
     * @param x X Map location to get for
     * @param y Y Map location to get for
     * @return Terrain item type
     */
    public int getPos(int x, int y){
        return mapItem[x][y];
    }
    
    /**
     *
     *
     * @paramRead pbs
     * @param bss
     * @param x
     * @param y
     * @return
     */
    public int getSpeed(Pillboxes pbs, Bases bss, int x, int y, int playerNum, boolean onBoat) {
        int returnValue;
        int terrain;
        boolean done;
        
        returnValue = MAP_SPEED_TDEEPSEA;
        done = false;
        if (pbs.existPos(x, y) == true) {
            if (pbs.deadPos(x, y) == false) {
                returnValue = MAP_SPEED_TPILLBOX;
                done = true;
            }
        } else if (bss.existPos(x, y) == true) {
            if (bss.canHit(x, y, playerNum) == false) {
                returnValue = MAP_SPEED_TREFBASE;
            } else {
                returnValue = 0;
            }
            done = true;
        }
        
        if (done == false) {
            terrain = Terrain.TerrainWithoutMine(mapItem[x][y]);
            switch (terrain) {
                case Terrain.DEEP_SEA:
                    returnValue = MAP_SPEED_TDEEPSEA;
                    if (onBoat == true) {
                        returnValue = MAP_SPEED_TBOAT;
                    }
                    break;
                case Terrain.BUILDING:
                    returnValue = MAP_SPEED_TBUILDING;
                    break;
                case Terrain.RIVER:
                    returnValue = MAP_SPEED_TRIVER;
                    if (onBoat == true) {
                        returnValue = MAP_SPEED_TBOAT;
                    }
                    break;
                case Terrain.SWAMP:
                    returnValue = MAP_SPEED_TSWAMP;
                    break;
                case Terrain.CRATER:
                    returnValue = MAP_SPEED_TCRATER;
                    break;
                case Terrain.ROAD:
                    returnValue = MAP_SPEED_TROAD;
                    break;
                case Terrain.FOREST:
                    returnValue = MAP_SPEED_TFOREST;
                    break;
                case Terrain.RUBBLE:
                    returnValue = MAP_SPEED_TRUBBLE;
                    break;
                case Terrain.GRASS:
                    returnValue = MAP_SPEED_TGRASS;
                    break;
                case Terrain.HALFBUILDING:
                    returnValue = MAP_SPEED_THALFBUILDING;
                    break;
                case Terrain.BOAT:
                    returnValue = MAP_SPEED_TBOAT;
                    break;
                default:
                    /* Fall through */
                    break;
            }
        }
        return returnValue;
    }
    
    /**
     * Operation
     *
     * @param pbs
     * @param bss
     * @param x
     * @param y
     * @return int
     */
    public int getManSpeed(Pillboxes pbs, Bases bss, int x, int y, int playerNum) {
        int returnValue; /* Value to return */
        int terrain;     /* The current Terrain */
        boolean done;        /* Are we done ? */
        
        returnValue = MAP_MANSPEED_TDEEPSEA;
        done = false;
        if ((pbs.existPos(x, y)) == true) {
            /* Check for PB */
            if (pbs.deadPos(x, y) == false) {
                returnValue = MAP_MANSPEED_TPILLBOX;
                done = true;
            }
        } else if ((bss.existPos(x, y)) == true) {
            /* Check for owned base */
            if (bss.canHit(x, y, playerNum) == false) {
                returnValue = MAP_MANSPEED_TREFBASE;
            } else {
                returnValue = 0;
            }
            done = true;
        }
        if (done == false) {
            terrain = Terrain.TerrainWithoutMine(mapItem[x][y]);
            switch (terrain) {
                case Terrain.DEEP_SEA:
                    returnValue = MAP_MANSPEED_TDEEPSEA;
                    break;
                case Terrain.BUILDING:
                    returnValue = MAP_MANSPEED_TBUILDING;
                    break;
                case Terrain.RIVER:
                    returnValue = MAP_MANSPEED_TRIVER;
                    break;
                case Terrain.SWAMP:
                    returnValue = MAP_MANSPEED_TSWAMP;
                    break;
                case Terrain.CRATER:
                    returnValue = MAP_MANSPEED_TCRATER;
                    break;
                case Terrain.ROAD:
                    returnValue = MAP_MANSPEED_TROAD;
                    break;
                case Terrain.FOREST:
                    returnValue = MAP_MANSPEED_TFOREST;
                    break;
                case Terrain.RUBBLE:
                    returnValue = MAP_MANSPEED_TRUBBLE;
                    break;
                case Terrain.GRASS:
                    returnValue = MAP_MANSPEED_TGRASS;
                    break;
                case Terrain.HALFBUILDING:
                    returnValue = MAP_MANSPEED_THALFBUILDING;
                    break;
                case Terrain.BOAT:
                    returnValue = MAP_MANSPEED_TBOAT;
                    break;
            }
        }
        return returnValue;
    }
    
    
    /**
     * Called once per game tick to update our map changes that are waiting on the network
     */
    public void update() {
        mapNet.update(gw.getPills(), gw.getBases());
    }
    
    /**
     * Returns The turn rate of the tank for a given map square
     *
     * @param pbs Pillboxes on the map
     * @param bss Bases on the map
     * @param x X map position
     * @param y Y map position
     * @param playerNum Player number to get for
     * @param onBoat Is this item on a boat
     * @return The amount that should be turned
     */
    public float getTurnRate( Pillboxes pbs, Bases bss, int x, int y, int playerNum, boolean onBoat) {
        float returnValue; // Value to return
        int terrain;
        boolean done = false;
        
        returnValue = MAP_TURN_TDEEPSEA;
        done = false;
        if (pbs.existPos(x, y) == true) {
            if (pbs.deadPos(x, y) == false) {
                returnValue = MAP_TURN_TPILLBOX;
                done = true;
            }
        } else if (bss.existPos(x, y) == true) {
            if (bss.canHit(x ,y, playerNum) == false) {
                returnValue = MAP_TURN_TREFBASE;
            } else {
                returnValue = 0;
            }
            done = true;
        }
        
        if (done == false) {
            terrain = Terrain.TerrainWithoutMine(mapItem[x][y]);
            switch (terrain) {
                case Terrain.DEEP_SEA:
                    returnValue = MAP_TURN_TDEEPSEA;
                    if (onBoat == true) {
                        returnValue = MAP_TURN_TBOAT;
                    }
                    break;
                case Terrain.BUILDING:
                    returnValue = MAP_TURN_TBUILDING;
                    break;
                case Terrain.RIVER:
                    returnValue = MAP_TURN_TRIVER;
                    if (onBoat == true) {
                        returnValue = MAP_TURN_TBOAT;
                    }
                    break;
                case Terrain.SWAMP:
                    returnValue = MAP_TURN_TSWAMP;
                    break;
                case Terrain.CRATER:
                    returnValue = MAP_TURN_TCRATER;
                    break;
                case Terrain.ROAD:
                    returnValue = MAP_TURN_TROAD;
                    break;
                case Terrain.FOREST:
                    returnValue = MAP_TURN_TFOREST;
                    break;
                case Terrain.RUBBLE:
                    returnValue = MAP_TURN_TRUBBLE;
                    break;
                case Terrain.GRASS:
                    returnValue = MAP_TURN_TGRASS;
                    break;
                case Terrain.HALFBUILDING:
                    returnValue = MAP_TURN_THALFBUILDING;
                    break;
                case Terrain.BOAT:
                    returnValue = MAP_TURN_TBOAT;
                    break;
            }
        }
        return returnValue;
    }
    
    
    /** Returns if the terrain is passible
     * @param x x map position
     * @param y y map position
     * @param onBoat Is the player on a boat
     * @return booleanif the terrain is passible
     */
    public boolean isPassable(int x, int y, boolean onBoat) {
        boolean returnValue; /* Value to return */
        int pos;         /* The current position */
        
        returnValue = false;
        
        pos = Terrain.TerrainWithoutMine(mapItem[x][y]);
        switch (pos) {
            case Terrain.DEEP_SEA:
                returnValue = true;
                break;
            case Terrain.BUILDING:
                returnValue = false;
                break;
            case Terrain.RIVER:
                returnValue = true;
                break;
            case Terrain.SWAMP:
                if (onBoat == false) {
                    returnValue = true;
                }
                break;
            case Terrain.CRATER:
                returnValue = true;
                break;
            case Terrain.ROAD:
                if (onBoat == false) {
                    returnValue = true;
                }
                break;
            case Terrain.FOREST:
                returnValue = false;
                break;
            case Terrain.RUBBLE:
                if (onBoat == false) {
                    returnValue = true;
                }
                break;
            case Terrain.GRASS:
                if (onBoat == false) {
                    returnValue = true;
                }
                break;
            case Terrain.HALFBUILDING:
                returnValue = false;
                break;
            case Terrain.BOAT:
                returnValue = false;
                break;
        }
        return returnValue;
    }
    
    /**
     * Sets a positon on a map
     *
     * @param x X Co-ordinate
     * @param y Y Co-ordinate
     * @param terrain Terrain to set
     * @param needSend sed in network. True if we should request the server make this change False indicates the server should make it
     * @param mineClear Set to TRUE if we should just set the map to the terrain. This is to remove the mines from under bases on start up
     */
    public void setPos(int x, int y, int terrain, boolean needSend, boolean mineClear ){
        if (mineClear == true || true) { // FIXME: netGetType() == netsingle
            /* Single player game */
            mapItem[x][y] = terrain;
            gw.getBrainMap().setPos(x, y, terrain, gw.getMines().existPos(x,y));
        } else {
            /* Multiplayer game */
            mapNet.add(x, y, terrain, needSend);
        }
        gw.getLog().addEvent(Log.log_MapChange, (byte) x, (byte) y, (byte) terrain, (byte) 0, (short) 0, Log.EMPTY_STRING);
    }
    
    /**
     * Sets a positon on a map. Only to be called by MapNet.add()
     *
     * @param x X Co-ordinate
     * @param y Y Co-ordinate
     * @param terrain Terrain to set
     * @see MapNet
     */
    public void setPosFromNet(int x, int y, int terrain) {
        mapItem[x][y] = terrain;
    }
    
    public int getTerrainRaw(int x, int y) {
        return mapItem[x][y];
    }
       
    /**
     * Checks if the map position is on land or not. Also checks if a base is on the map square
     *
     * @param x X position to check
     * @param y Y position to check
     * @return If the map position is land or not
     */
    public boolean isLand(int x, int y) {
        boolean returnValue = true; // Value to return
        if (getPos(x, y) == Terrain.RIVER || getPos(x, y) == Terrain.DEEP_SEA) {
            if (gw.getBases().existPos(x, y) == false) {
                returnValue = false;
            }
        }
        return returnValue;
    }
    
    /**
     * Checks to see if the map position contains a mine regardless if the player can see it or not
     *
     * @param x X position to check
     * @param y Y position to check
     * @return If the X,Y map position contains a mine
     */
    public boolean isMine( int x, int y ){
        if (x <= MAP_MINE_EDGE_LEFT || x >= MAP_MINE_EDGE_RIGHT || y <= MAP_MINE_EDGE_TOP || y >= MAP_MINE_EDGE_BOTTOM) {
            return true;
        }
        return Terrain.IsMinedTerrain(this.mapItem[x][y]);
    }
    
    /**
     * * Sets the map to the network run at yPos. A network run
     * is an array of the bytes from 20 to 236 compressed
     *
     * @param data Byte array containing data
     * @param offset Offset into the byte array to read from
     * @param yPos Y Map position
     * @param dataLen Length of the data
     */
    public void setMapNetRun(byte data[], int offset, int yPos, int dataLen) {
        Decompress dec;
        byte newData[];
        int xPos;
        int count = 0;
        int dataPos = 0;
        
        dec = new Decompress(Utils.ArraySubstring(data, dataLen, offset), dataLen);
        newData = dec.getData();
        while (count < 6) {
            for (xPos=MAP_MINE_EDGE_LEFT+1;xPos < MAP_MINE_EDGE_RIGHT;xPos++) {
                mapItem[xPos][yPos+count] = Utils.ConvertByteToInt(newData[dataPos]);
                dataPos++;
            }
            count++;
        }
    }
    
    
    /**
     * Operation
     *
     * @param fileName
     * @param sss
     * @param bss
     * @param pbs
     * @return boolean
     */
    public boolean write( String fileName, Starts sss, Bases bss, Pillboxes pbs ) {
        FileOutputStream fos;
        
        try {
            fos = new FileOutputStream(fileName);
            fos.write(MAP_HEADER.getBytes());
            fos.write(MAP_VERSION);
            fos.write(pbs.getNumItems());
            fos.write(bss.getNumItems());
            fos.write(sss.getNumItems());
            writePills(fos, pbs, pbs.getNumItems());
            writeBases(fos, bss, bss.getNumItems());
            writeStarts(fos, sss, sss.getNumItems());
            writeRuns(fos);
        } catch (Exception e) {
            e.printStackTrace(); //FIXME
            return false;
        }
        
        
        return true;
    }
    /**
     * Operation
     *
     * @param read
     * @param pbs
     * @return boolean
     */
    private boolean readPills(InputStream read, Pillboxes pbs) throws IOException {
        int total;
        int count = 0;
        int posRead;
        byte[] buffer = new byte[5];
        
        total = pbs.getNumItems();
        
        while (count < total) {
            posRead = read.read(buffer, 0, 5);
            if (posRead != 5) {
                return false;
            }
            
            pbs.set(count, Utils.ConvertByteToInt(buffer[0]), Utils.ConvertByteToInt(buffer[1]), Utils.ConvertByteToInt(buffer[2]), Utils.ConvertByteToInt(buffer[3]), Utils.ConvertByteToInt(buffer[4]), false, false );
            count++;
        }
        
        return true;
    }
    /**
     * Operation
     *
     * @param read
     * @param bss
     * @return boolean
     */
    private boolean readBases(InputStream read, Bases bss) throws IOException {
        int total;
        int count = 0;
        int posRead;
        byte[] buffer = new byte[6];
        
        total = bss.getNumItems();
        
        while (count < total) {
            posRead = read.read(buffer, 0, 6);
            if (posRead != 6) {
                return false;
            }
            bss.set(count, Utils.ConvertByteToInt(buffer[0]), Utils.ConvertByteToInt(buffer[1]), Utils.ConvertByteToInt(buffer[2]), Utils.ConvertByteToInt(buffer[3]), Utils.ConvertByteToInt(buffer[4]), Utils.ConvertByteToInt(buffer[5]));
            count++;
        }
        return true;
    }
    
    /**
     * Operation
     *
     * @param is
     * @param sts
     * @return boolean
     */
    private boolean readStarts( InputStream read, Starts sts ) throws IOException {
        int total;
        int count = 0;
        int posRead;
        byte[] buffer = new byte[3];
        
        total = sts.getNumItems();
        
        while (count < total) {
            posRead = read.read(buffer, 0, 3);
            if (posRead != 3) {
                return false;
            }
            sts.set(count, Utils.ConvertByteToInt(buffer[0]), Utils.ConvertByteToInt(buffer[1]), buffer[2]);
            count++;
        }
        
        return true;
    }
    /**
     * Operation
     *
     * @param is
     * @param elems
     * @param yValue
     * @param startx
     * @param endx
     * @return int length of next run to read or -1 for error
     */
    private int processRun(InputStream read, int elems, int yValue, int startx, int endx ) throws IOException {
        boolean needRead;     /* State variable - Do we need to read the next byte */
        int state; /* Current run state */
        byte[] item = new byte[1];         /* Item being worked on */
        int highNibble;   /* The low nibble of a byte */
        int lowNibble;    /* The low nibble of a byte */
        int len;          /* Length of the row */
        int mapPos;       /* Possision on map */
        int count;         /* Looping variables */
        int count2;
        int readLen;
        
        count = 0;
        state = RUN_STATE_HIGH_LEN;
        mapPos = startx;
        len = 0;
        
        readLen = read.read(item, 0, 1);
        while (count < elems && readLen == 1) {
            needRead = false;
            highNibble = lowNibble = item[0];
            /* Extract nibbles */
            highNibble = Utils.ConvertByteToInt(item[0]) >>> 4;
            lowNibble = item[0] & ( 0xF);
            while (needRead == false) {
                switch (state) {
                    case RUN_STATE_HIGH_LEN:
                        len = highNibble;
                        if (len < MAP_RUN_DIFF) {
                            state = RUN_STATE_LOW_DIFF;
                            len++;
                        } else {
                            state = RUN_STATE_LOW_SAME;
                        }
                        break;
                    case RUN_STATE_LOW_LEN:
                        len = lowNibble;
                        if (len < MAP_RUN_DIFF) {
                            state = RUN_STATE_HIGH_DIFF;
                            len++;
                        } else {
                            state = RUN_STATE_HIGH_SAME;
                        }
                        needRead = true;
                        break;
                    case RUN_STATE_LOW_DIFF:
                        mapItem[mapPos][yValue] = lowNibble;
                        mapPos++;
                        len--;
                        if (len == 0) {
                            state = RUN_STATE_HIGH_LEN;
                        } else {
                            state = RUN_STATE_HIGH_DIFF;
                        }
                        needRead = true;
                        break;
                    case RUN_STATE_HIGH_DIFF:
                        mapItem[mapPos][yValue] = highNibble;
                        mapPos++;
                        len--;
                        if (len == 0) {
                            state = RUN_STATE_LOW_LEN;
                        } else {
                            state = RUN_STATE_LOW_DIFF;
                        }
                        break;
                    case RUN_STATE_LOW_SAME:
                        count2 = 0;
                        while (count2 < (len - MAP_RUN_SAME)) {
                            mapItem[mapPos+count2][yValue] = lowNibble;
                            count2++;
                        }
                        mapPos = mapPos + count2;
                        state = RUN_STATE_HIGH_LEN;
                        needRead = true;
                        break;
                    case RUN_STATE_HIGH_SAME:
                        count2 = 0;
                        while (count2 < (len - MAP_RUN_SAME)) {
                            mapItem[mapPos + count2][yValue] = highNibble;
                            count2++;
                        }
                        mapPos = mapPos + count2;
                        state = RUN_STATE_LOW_LEN;
                        break;
                }
            }
            readLen = read.read(item, 0, 1);
            count++;
        }
        
        /* Check all read correctly */
        if (count != elems || mapPos != endx) {
            return -1;
        }
        return Utils.ConvertByteToInt(item[0]);
    }
    /**
     * Operation
     *
     * @param is
     * @return boolean
     */
    private boolean readRuns(InputStream read) throws IOException {
        boolean done;
        boolean returnValue; /* Value to return */
        int dataLen = 0; /* Length of the run */
        int y;
        int startX;
        int endX;
        int posRead;
        int runGetLenOffset;
        byte[] runHeader = new byte[4];
        
        returnValue = true;
        done = false;
        runGetLenOffset = 0;
        while (done == false) {
            posRead = read.read(runHeader, 0, 4-runGetLenOffset);
            if (posRead != 4-runGetLenOffset) {
                return false;
            }
            if (runGetLenOffset == 0) {
                dataLen = Utils.ConvertByteToInt(runHeader[0-runGetLenOffset]);
            }
            y = Utils.ConvertByteToInt(runHeader[1-runGetLenOffset]);
            startX = Utils.ConvertByteToInt(runHeader[2-runGetLenOffset]);
            endX = Utils.ConvertByteToInt(runHeader[3-runGetLenOffset]);
            if (dataLen == 4 && y == MAP_ARRAY_LAST && startX == MAP_ARRAY_LAST && endX == MAP_ARRAY_LAST) {
                done = true;
            } else {
                dataLen = processRun(read, dataLen - 4, y, startX, endX);
                if (dataLen == -1) {
                    returnValue = false;
                    done = true;
                } else {
                    runGetLenOffset = 1;
                }
            }
        }
        return returnValue;
    }
    /**
     * Operation
     *
     * @param os
     * @param pbs
     * @param total
     * @return boolean
     */
    public boolean writePills( OutputStream os, Pillboxes pbs, int total) throws Exception {
        int count = 0;
        Pillbox p;
        
        while (count < total) {
            p = (Pillbox) pbs.getItem(count);
            os.write((byte) p.getX());
            os.write((byte) p.getY());
            os.write((byte) p.getOwner());
            os.write((byte) p.getArmour());
            os.write((byte) p.getAttackSpeed());
            count++;
        }
        return true;
    }
    /**
     * Operation
     *
     * @param os
     * @param bss
     * @param total
     * @return boolean
     */
    public  boolean writeBases( OutputStream os, Bases bss, int total ) throws Exception {
        int count = 0;
        Base b;
        
        while (count < total) {
            b = (Base) bss.getItem(count);
            os.write((byte) b.getX());
            os.write((byte) b.getY());
            os.write((byte) b.getOwner());
            os.write((byte) b.getArmour());
            os.write((byte) b.getShells());
            os.write((byte) b.getMines());
            count++;
        }
        return true;
    }
    /**
     * Operation
     *
     * @param os
     * @param sss
     * @param total
     * @return boolean
     */
    public boolean writeStarts( OutputStream os, Starts sss, int total ) throws Exception {
        int count = 0;
        Start s;
        
        while (count < total) {
            s = (Start) sss.getItem(count);
            os.write(s.getX());
            os.write(s.getY());
            os.write(s.getDir());
            count++;
        }
        return true;
    }
    
    /**
     * Operation
     *
     * @param OutputStream
     * @return boolean
     */
    public boolean writeRuns(OutputStream os) throws Exception {
        BoloMapRun run;
        int x = 0;
        int y = 0;
        
        while (y < 0xFF) {
            run = prepareRun(x, y);
            os.write(run.getHeader());
            os.write(run.getData(), 0, run.getDataLen());
            x = run.getX();
            y = run.getY();
        }
        return true;
        
    }
    
    /**
     * Operation
     *
     * @return long
     */
    private BoloMapRun prepareRun(int x, int y){
        BoloMapRun run = new BoloMapRun(x, y);
        int terrain;
        int code;
        
        /* Search for non-DEEPSEA terrain */
        while (getPos(x, y) == Terrain.DEEP_SEA) {
            if (x < 0xFF) {
                x++;
            } else if (y < MAP_ARRAY_LAST) {
                x = 0;
                y++;
            } else {
                break;
            }
        }
        run.setStartX(x);
        if (y < MAP_ARRAY_LAST) {
            terrain = getPos(x, y);
            while(terrain != Terrain.DEEP_SEA) {
                if (terrain == getPos((x+1), y)){
                    /* Two squares are the same */
                    code = MAP_CODE_IDENTICAL_START;
                    x += MAP_CODE_IDENTICAL_SKIP;			/* skip over the two squares we have found */
                    while (code < MAP_CODE_IDENTICAL_END && getPos(x, y) == terrain) {
                        code++;
                        x++;
                    }
                    run.putNibble(code);
                    run.putNibble(terrain);
                } else {
                    int code1 = 0;	/* code 0 means 1 individual square */
                    int ds = x++;	/* record where the difference run starts */
                    while (code1 < MAP_CODE_DIFFERENT_END  && getPos(x, y) != Terrain.DEEP_SEA && getPos(x, y) != getPos((x+1), y)) {
                        code1++;
                        x++;
                    }
                    run.putNibble(code1);
                    while (ds<x) {
                        run.putNibble(getPos(ds++, y));
                    }
                }
                terrain = getPos(x, y);
            }
            run.finish();
        }
        
        run.set(x, y);
        return run;
    }
    
    /**
     * Returns the extents of this map (left,right,top bottom)
     *
     * @return GameItemRect containing the extents of the map
     * @see GameItemRect
     */
    private GameItemRect getExtents() {
        int top = -1;
        int bottom = 255;
        int left = -1;
        int right = 255;
        int count = 0;
        int count2 = 0;
        
        for (count=0;count<MAP_ARRAY_SIZE;count++) {
            for (count2=0;count2<MAP_ARRAY_SIZE;count2++) {
                if (mapItem[count][count2] != Terrain.DEEP_SEA) {
                    if (count2 > top) {
                        top = count2;
                    }
                    if (count2 < bottom) {
                        bottom = count2;
                    }
                    if (count < left) {
                        left = count;
                    }
                    if (count > right) {
                        right = count;
                    }
                }
            }
        }
        return new GameItemRect(top, bottom, left, right);
    }
    
    /**
     * centers the map and bases/pillboxes/starts
     *
     * @param pbs Pillboxes on this map
     * @param bss Bases on this map
     * @param sss Starts on this map
     */
    private void center(Pillboxes pbs, Bases bss, Starts sss) {
        GameItemRect best;
        GameItemRect test;
        int addX;
        int addY;
        int[][] newMap;
        int count;
        int count2;
        
        /* Get the largest extends */
        best = pbs.getGameItemRect();
        test = bss.getGameItemRect();
        best.setLargerExtends(test);
        test = sss.getGameItemRect();
        best.setLargerExtends(test);
        test = this.getExtents();
        best.setLargerExtends(test);
        
        if (best.getTop() <= best.getBottom() && best.getLeft() <= best.getRight()) {
            addX = (255/2) - ((best.getLeft() - best.getRight())/2) - 1;
            addY = (255/2) - ((best.getTop() - best.getBottom())/2) - 1;
            if (addX != 0 || addY != 0) {
                bss.moveAll(addX, addY);
                sss.moveAll(addX, addY);
                pbs.moveAll(addX, addY);
                
                /* Move the map items */
                newMap = new int[256][256];
                for (count=0;count<256;count++) {
                    for (count2=0;count2<256;count2++) {
                        newMap[count][count2] = mapItem[count][count2];
                    }
                }
                
                for (count=best.getLeft();count<best.getRight();count++) {
                    for (count2=best.getTop();count2<best.getBottom();count2++) {
                        mapItem[count+addX][count2+addY] = newMap[count][count2];
                    }
                }
            }
        }
    }
}

