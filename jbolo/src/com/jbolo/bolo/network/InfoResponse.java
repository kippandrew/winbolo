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


/*
 * InfoResponse.java
 *
 * Created on 23 May 2004, 01:07
 */

package com.jbolo.bolo.network;

import com.jbolo.bolo.Utils;
import java.util.Date;

/**
 *
 *
 *
 *  typedef struct {
 *     BOLOHEADER h; 1-4
 *
 *      char mapname[MAP_STR_SIZE]; 5-41 /* Pascal string (first byte is length)         *
 *      GAMEID gameid;       42-52 /* 8 byte unique ID for game (combination       *
 *                           /* of starting machine address & timestamp)     *
 *      BYTE gametype;       53 /* Game type (1, 2 or 3: open, tourn. & strict) *
 *      BYTE allow_mines;    54 /* 0x80 for normal hidden mines                 *
 *                           /* 0xC0 for all mines visible                   *
 *      BYTE allow_AI;       55 /* 0 for no AI tanks, 1 for AI tanks allowed    *
 *      BYTE spare1;         56 /* 0                                            *
 *      long start_delay;    /* if non zero, time until game starts, (50ths) *
 *      long time_limit;     /* if non zero, time until game ends, (50ths)   *
 *
 *      WORD num_players;    /* number of players                            *
 *      WORD free_pills;     /* number of free (neutral) pillboxes           *
 *      WORD free_bases;     /* number of free (neutral) refuelling bases    *
 *      BYTE has_password;   /* non-zero if game has password set            *
 *      BYTE spare2;         /* 0                                            *
 *    } INFO_PACKET;
 *
 *
 *
 * @author  John Morrision
 */
public class InfoResponse {

    /** Server address */
    private String address;
    /** Server port */
    private int port;
    /** Game version */
    private String version;
    
    /** Name of the map */
    private String mapName;
    /** Type of game being played */
    private int gameType;
    /** Are hidden mines enabled */
    private boolean hiddenMines;

    /** Whether AI computer players allowed */
    private int ai;
    
    /** Start time of the game */
    private long startTime;
    
    /** Start delay of the game */
    private long startDelay;
    /** Time limit of the game */
    private long timeLimit;
    /** Number of players in the game */
    private int numPlayers;
    /** Number of free bases in the game */
    private int freeBases;
    /** Number of free pillboxes in the game */
    private int freePills;
    /** Has the game a password */
    private boolean password;

    /** Server address as 4 bytes */
    private byte[] addr;
    
    /** Creates a new instance of InfoResponse */
    public InfoResponse(byte data[], int len) throws Exception {
        int test;

        if (len != 76) {
            throw new Exception();
        }

        addr = new byte[4];
        
        if (data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] != NetPacks.BOLOPACKET_INFORESPONSE) {
            throw new Exception();
        }
        
        if (Utils.CompareArrays(data, NetPacks.HEADER, NetPacks.HEADER.length) == false) {
            throw new Exception();
        }
        
        // Version
        version = data[4] + "." + data[5] + data[6];
        
        // Map name
        mapName = Utils.PByteArrayToString(data, 8);

        //IP Address
        addr[0] = data[len-29];
        addr[1] = data[len-30];
        addr[2] = data[len-31];
        addr[3] = data[len-32];
        
        if (Utils.ConvertByteToInt(data[len-29]) == 0 && Utils.ConvertByteToInt(data[len-30]) == 0 && Utils.ConvertByteToInt(data[len-31]) == 0 && Utils.ConvertByteToInt(data[len-32]) == 0 ) {
            // Itourself because server address = 0
            address = "0.0.0.0";
        } else {
            address = Utils.ConvertByteToInt(data[len-29]) + "." + Utils.ConvertByteToInt(data[len-30]) + "." + Utils.ConvertByteToInt(data[len-31]) + "." + Utils.ConvertByteToInt(data[len-32]);
        }
        
        // Port
        port = this.readShort(data[len-27], data[len-28]);
        
        // 4 bytes are the start time
        startTime = getLongFromBytes(data[len-26], data[len-25], data[len-24], data[len-23]);
        Date d = new Date(startTime);
        
        // Game type
        gameType = data[len-20];
        if (gameType <1 || gameType > 3) {
            throw new Exception("bad game type");
        } 
        gameType--;
        
        // Hidden Mines
        test = Utils.ConvertByteToInt(data[len-19]);
        if (test == NetPacks.HIDDEN_MINES) {
            hiddenMines = true;
        } else {
            hiddenMines = false;
        }
        
        // AI
        ai = data[len-18];
        if (ai < 0 || ai > 2) {
            throw new Exception("bad ai " + ai);
        }

        // Start Delay
        // 4 byte long data[len-13], data[len-14], data[len-15], data[len-16]

        
        // Time limit
        // 4 byte long data[len-9], data[len-10], data[len-11], data[len-12]
        
        // Num Players
        numPlayers = checkValue(readShort(data[len-8], data[len-7]));

        // Free Pillboxes
        freePills = checkValue(readShort(data[len-6], data[len-5]));
        
        // Free bases
        freeBases = checkValue(readShort(data[len-4], data[len-3]));        
        
        // Password
        if (data[len-2] == 0) {
            password = false;
        } else {
            password = true;
        }
    }

    private long getLongFromBytes(byte a, byte b, byte c, byte d) {
        long ret;
        
        ret = Utils.ConvertByteToInt(a) << (3 * 8);
        ret += Utils.ConvertByteToInt(b) << (2 * 8);
        ret += Utils.ConvertByteToInt(c) << 8;
        ret += Utils.ConvertByteToInt(d);
        return ret;
    }
    
    /**
     * Reads a short from two bytes in network byte order
     * @return short as int
     * @param hi first byte
     * @param lo Second byte
     */
    private int readShort(byte hi, byte lo) {
        int ret = Utils.ConvertByteToInt(lo) << 8;
        ret += Utils.ConvertByteToInt(hi);
        
        return ret;
    }
    
    /**
     * Checks the passed int value to ensure it is in the range 0-16
     * @param value Value to check
     * @return original value passed
     * @throw Exception if value is out of bounds
     */
    private int checkValue(int value) throws Exception {
        if (value < 0 || value > 16) {
            throw new Exception("out o bounds = " + value);
        }
        return value;
    }
    
    /**
     * Gets the game version
     * @return String
     */
    public String getVersion() {
        return version;
    }
    
    /**
     * Gets game address
     * @return String
     */
    public String getAddress() {
        return address;
    }
   
    /**
     * Get game port
     * @return int
     */
    public int getPort() {
        return port;
    }
    
    /**
     * Return map name
     * @return String
     */
    public String getMapName() {
        return mapName;
    }
    
    /**
     * Get AI type
     * @return int
     */
    public int getAI() {
        return ai;
    }
     
    /**
     * Get if hidden mines are enabled
     * @return boolean
     */
    public boolean getHiddenMines() {
        return hiddenMines;
    }
    
    /**
     * Gets the game type
     * @return int
     */
    public int getGameType() {
        return gameType;
    }
    
    /**
     * Gets the number of players in the game
     * @return int
     */
    public int getNumPlayers() {
        return numPlayers;
    }
    
    /**
     * Gets the number of free pills in the game
     * @return int
     */
    public int getNumFreePills() {
        return freePills;
    }
    
    /**
     * Gets the number of free bases in the game
     * @return int
     */
    public int getNumFreeBases() {
        return freeBases;
    }

    /**
     * Gets the game start time
     * @return long
     */
    public long getStartTime() {
        return startTime;
    }
    
    /**
     * Gets the game start delay
     * @return long
     */
    public long getStartDelay() {
        return startDelay;
    }
    
    /**
     * Gets the game time limit
     * @return long
     */
    public long getTimelimit() {
        return timeLimit;
    }

    /**
     * Returns if the game has a password
     * @return boolean
     */
    public boolean hasPassword() {
        return password;
    }
    
    /**
     * Sets the address this information packet comes from. Does not overwrite the addr bytes field.
     * @param address Address to set.
     */
    public void setAddress(String address) {
    	this.address = address;
    }
}
