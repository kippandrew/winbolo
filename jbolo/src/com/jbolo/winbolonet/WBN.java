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
 * WBN.java
 *
 * Created on 18 May 2004, 21:17
 */

package com.jbolo.winbolonet;

import java.io.ByteArrayOutputStream;
import java.util.ArrayList;
import java.util.ListIterator;
import com.jbolo.bolo.GameClient;
import com.jbolo.bolo.Utils;


/**
 * Interface to WBN
 * http://www.winbolo.net/
 * @author John Morrison
 */
public class WBN {
    
    /** Size of keys */
    public static int WINBOLONET_KEY_LEN = 32;
    
    /** WBN Header - Major version number */
    private static byte WINBOLO_NET_VERSION_MAJOR = 0x01;
    /** WBN Header - Minor version number */
    private static byte WINBOLO_NET_VERSION_MINOR = 0x00;
    /** WBN Header - Revision version number */
    private static byte WINBOLO_NET_VERSION_REVISION = 0x0;
    
    /** WBN Message types - Version Request */
    private static byte WINBOLO_NET_MESSAGE_VERSION_REQ = 1;
    /** WBN Message types - Server Key Request */
    private static byte WINBOLO_NET_MESSAGE_SERVERKEY_REQ = 2;
    /** WBN Message types - Client Key Request */
    private static byte WINBOLO_NET_MESSAGE_CLIENTKEY_REQ = 3;
    /** WBN Message types - Verify Client Key Request */
    private static byte WINBOLO_NET_MESSAGE_VERIFYCLIENTKEY_REQ = 4;
    /** WBN Message types - Client Leave Message */
    private static byte WINBOLO_NET_MESSAGE_CLIENTLEAVE_REQ = 5;
    /** WBN Message types - Server Quit Message Request */
    private static byte WINBOLO_NET_MESSAGE_SERVERQUIT_REQ = 6;
    /** WBN Message types - Server Update Message */
    private static byte WINBOLO_NET_MESSAGE_SERVERUPDATE_REQ = 7;
    /** WBN Message types - Team Composition Message */
    private static byte WINBOLO_NET_MESSAGE_TEAMS = 8;
    /** WBN Message types - Version Number Submission */
    private static byte WINBOLO_NET_VERSION = 9;
    /** WBN Message types - Game Locked Status */
    private static byte WINBOLO_NET_LOCK = 10; /* Game lock message */
    
    /** WBN Event Type -  Player B has joined Alliance A */
    public static final byte WINBOLO_NET_EVENT_ALLY_JOIN = 0;
    /** WBN Event Type -  Player B has left Alliance A */
    public static final byte WINBOLO_NET_EVENT_ALLY_LEAVE = 1;
    /** WBN Event Type -  Player A has captured a base */
    public static final byte WINBOLO_NET_EVENT_BASE_CAPTURE = 2;
    /** WBN Event Type -  Player A has captured a pill */
    public static final byte WINBOLO_NET_EVENT_PILL_CAPTURE = 3;
    /** WBN Event Type -  Player A has killed a tank */
    public static final byte WINBOLO_NET_EVENT_TANK_KILL = 4;
    /** WBN Event Type -  Player A has killed a lgm */
    public static final byte WINBOLO_NET_EVENT_LGM_KILL = 5;
    /** WBN Event Type -  Player A has lost their lgm */
    public static final byte WINBOLO_NET_EVENT_LGM_LOST = 6;
    /** WBN Event Type -  Player A has stole a base from Player B*/
    public static final byte WINBOLO_NET_EVENT_BASE_STEAL = 7;
    /** WBN Event Type -  Player A has stole pill from Player B */
    public static final byte WINBOLO_NET_EVENT_PILL_STEAL = 8;
    /** WBN Event Type -  Player A has joined the game */
    public static final byte WINBOLO_NET_EVENT_PLAYER_JOIN = 9;
    /** WBN Event Type -  Player A has left the game */
    public static final byte WINBOLO_NET_EVENT_PLAYER_LEAVE = 10;
    /** WBN Event Type -  Player A has won the game */
    public static final byte WINBOLO_NET_EVENT_WIN = 11;
    /** WBN Event Type -  Player A is quitting the game */
    public static final byte WINBOLO_NET_EVENT_QUITTING = 13;
    /** WBN Event Type - No event Ignore Used by starts */
    public static final byte WINBOLO_NET_EVENT_NO_EVENT = 100;
    
    /** WBN - If no player use this holder */
    public static final byte WINBOLO_NET_NO_PLAYER = 100;
    /** Maximum time between sending to WBN in ms */
    public static final int WINBOLO_NET_MAX_NOSEND = 60 * 1000;
    
    /** Team marker for sending WBN string */
    private static byte WINBOLO_NET_TEAM_MARKER = 0; // FIXME (254);
    
    
    
    /** The WBN singleton */
    private static WBN _me = null;
    
    /** Number of free pillboxes last time sendServerUpdate was called */
    private int wbnFreePills;
    /** Number of free bases last time sendServerUpdate was called */
    private int wbnFreeBases;
    /** Number of players last time sendServerUpdate was called */
    private int wbnNumPlayers;
    
    /** Thread for sending WBN data */
    private WBNThread wbnThread;
    
    /** List of events waiting to send to WBN - Arraylist of WBNEvent */
    private ArrayList<WBNEvent> events;
    
    /** Is WBN running */
    private boolean winboloNetRunning = false;
    
    /** HTTP object used to send data */
    private HTTP http;
    
    /** Header bytes */
    private byte[] header;
    
    /** Server Key */
    private byte[] serverKey;
    
    /** No player key */
    private byte[] noPlayerArray;
    
    /** Array of Player Keys */
    private byte[][] winboloNetPlayerKey;
    
    /** Time last updated winbolo.net */
    private long winboloNetLastSent;
    
    /** Creates a new instance of WBN */
    protected WBN() {
        int count; // Looping variable
        
        // FIXME screenServerConsoleMessage((char *) "WinBolo.net Startup");
        winboloNetRunning = false;
        serverKey = new byte[WINBOLONET_KEY_LEN];
        winboloNetPlayerKey = new byte[WINBOLONET_KEY_LEN][WINBOLONET_KEY_LEN];
        serverKey[0] = 0;
        
        // Set these initial to values that can't be set so they invalidate the first time they are checked
        wbnFreePills = 255;
        wbnFreeBases = 255;
        wbnNumPlayers = 255;
        
        header = new byte[3];
        header[0] = WINBOLO_NET_VERSION_MAJOR;
        header[1] = WINBOLO_NET_VERSION_MINOR;
        header[2] = WINBOLO_NET_VERSION_REVISION;
        
        count = 0;
        while (count < GameClient.MAX_TANKS) {
            winboloNetPlayerKey[count][0] = 0;
            count++;
        }
        
        noPlayerArray = new byte[WINBOLONET_KEY_LEN];
        count = 0;
        while (count < WINBOLONET_KEY_LEN) {
            noPlayerArray[count] = 0;
            count++;
        }
        
        http = new HTTP();
        events = new ArrayList<WBNEvent>();
        wbnThread = new WBNThread(http);
    }
    
    
    /**
     * Returns the WBN object for this game.
     * Creates a new one if required. createServer() or
     * createClient() must be called first to setup object type
     * @return WBN Object.
     */    
    public static WBN GetInstance() {
        if (_me == null) {
            _me = new WBN();
        }
        return _me;
    }
    
    
    /**
     *
     * @param mapName
     * @param port
     * @param gameType
     * @param ai
     * @param mines
     * @param password
     * @param numBases
     * @param numPills
     * @param freeBases
     * @param freePills
     * @param numPlayers
     * @param startTime
     * @return
     */    
    public boolean createServer(String mapName, int port, int gameType, int ai, boolean mines, boolean password, int numBases, int numPills, int freeBases, int freePills, int numPlayers, long startTime) {
        ByteArrayOutputStream bos = new ByteArrayOutputStream(64);
        byte results[];
        byte val[];
        String errorMessage;
        
        try {
            bos.write(header);
            bos.write(WINBOLO_NET_MESSAGE_VERSION_REQ);
            val = bos.toByteArray();
            val = http.sendMessage(val, val.length);
            if (val.length > 0) {
                if (val[0] == 0) {
                    /* Error */
                    errorMessage = new String(val, 1, val.length-1);
                    System.err.println("Error: " + errorMessage);
                    shutdown();
                } else if (val[0] == 1) {
                    // FIXME screenServerConsoleMessage("\tWinBolo.net Available: Version: " + val[1] + "." + val[2] + "." + val[3]);
                    if (val.length > 4) {
                        //FIXME screenServerConsoleMessage("MOTD: " + new String(val, 4, val.length - 4));
                        /* Get a server key */
                        if (requestServerKey(mapName, port, gameType, ai, mines, password, numBases, numPills, freeBases, freePills, numPlayers, startTime) == false) {
                            /* Error can't get a key */
                            shutdown();
                        } else {
                            winboloNetRunning = true;
                            wbnThread.start();
                            winboloNetLastSent = System.currentTimeMillis();
                            sendVersion();
                        }
                        
                    }
                } else {
                    /* Unexpected error */
                    // FIXME screenServerConsoleMessage("Error: WinBolo.net unavailable\n");
                    shutdown();
                    
                }
            }
        } catch (Exception e) {
            winboloNetRunning = false;
        }
        
        return winboloNetRunning;
    }
    

    /**
     * Initialises this WBN object to be a client.
     *
     * @return String containing the error message or null if succesful
     * @param userName Players name to use
     * @param password Players password to authenticate with
     * @param key Byte array containing the server WBN key
     */
    public String createClient(String userName, String password, byte key[]) {
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        String returnValue = null;
        byte buff[];
        byte ret[];
        
        winboloNetRunning = true;
        try {
            bos.write(header);
            bos.write(WINBOLO_NET_MESSAGE_VERSION_REQ);
            buff = bos.toByteArray();
            ret = http.sendMessage(buff, buff.length);
            if (ret.length > 0) {
                if (ret[0] == 0) {
                    returnValue = new String(ret, 1, ret.length - 1);
                    shutdown();
                } else {
                    returnValue = requestClientKey(userName, password, key);
                    if (returnValue != null) {
                        shutdown();
                    }
                }
            }
        } catch (Exception e) {
            shutdown();
            returnValue = "Unknown Error";
        }
        
        return returnValue;
    }
    
    
    /**
     * Returns the server key of this server
     *
     * @return byte array
     */
    public byte[] getServerKey() {
        return serverKey;
    }

    /**
     * Gets this WBN clients key. Called only by clients
     * @return Client key or first value = 0 if non participant
     */
    public byte[] getMyClientKey() {
        return winboloNetPlayerKey[0];
    }
    

    /**
     * Shutsdown all WBN communcation and prevents and further messages from being sent.
     * Sends goodbye message if connected.
     */
    public void shutdown() {
        //FIXME  screenServerConsoleMessage((char *) "WinBolo.net Shutdown");
        if (winboloNetRunning == true) {
            wbnThread.stop();
            /* We need to say goodbye */
            if (serverKey[0] != 0) { // FIXME threadsGetContext() == TRUE &&
                /* Note only servers have to say goodbye */
                goodbye();
            }
        }
        events.clear();
        winboloNetRunning = false;
    }
    
    /**
     * Returns if a player is participating in WBN
     *
     * @return If player is participating
     * @param playerNum to check for
     */
    public boolean isPlayerParticipant(int playerNum) {
        if (winboloNetPlayerKey[playerNum][0] == 0) {
            return false;
        }
        return true;
    }
    
    /**
     * Called when a player leaves the game. Tells the WBN so.
     *
     * @param playerNum  Player position Number
     * @param numPlayers Number of players now in the game
     * @param freeBases  Number of free bases
     * @param freePills  Number of free pills
     */
    public void clientLeaveGame(int playerNum, int numPlayers, int freeBases, int freePills) {
        ByteArrayOutputStream bos;
        byte buff[];
        
        addEvent(WINBOLO_NET_EVENT_PLAYER_LEAVE, true, playerNum, WINBOLO_NET_NO_PLAYER);
        if (winboloNetPlayerKey[playerNum][0] != 0 && winboloNetRunning == true) {
            try {
                bos = new ByteArrayOutputStream(64);
                bos.write(header);
                bos.write(WINBOLO_NET_MESSAGE_CLIENTLEAVE_REQ);
                bos.write(numPlayers);
                bos.write(freeBases);
                bos.write(freePills);
                bos.write(winboloNetPlayerKey[playerNum]);
                buff = bos.toByteArray();
                http.sendMessage(buff, buff.length);
            } catch (Exception e) {
                // Ignore excecption as its only caused by a byte array writting error
            }
            winboloNetPlayerKey[playerNum][0] = 0;
        }
    }
    
    /**
     * Verifies a client key by sending it to the server for
     * authentication. Returns if its a valid key for this
     * session
     *
     * @return if user can be authenticated
     * @param keyBuff Buffer containing key
     * @param userName Player name to check for
     * @param playerNum Player number
     */
    public boolean verifyClientKey(byte keyBuff[], String userName, int playerNum) {
        ByteArrayOutputStream bos;
        boolean returnValue = false;
        byte buff[];
        byte retBuff[];
        
        if (winboloNetRunning == false || winboloNetPlayerKey[playerNum][0] != 0) {
            returnValue = false;
        } else {
            bos = new ByteArrayOutputStream(128);
            try {
                bos.write(header);
                bos.write(WINBOLO_NET_MESSAGE_VERIFYCLIENTKEY_REQ);
                bos.write(serverKey);
                bos.write(Utils.StringToPByteArray(userName));
                bos.write(keyBuff);
                buff = bos.toByteArray();
                
                retBuff = http.sendMessage(buff, buff.length);
                if (retBuff.length == 2) {
                    if (retBuff[1] == 1) {
                        returnValue = true;
                    }
                }
            } catch (Exception e) {
                returnValue = false;
            }
        }
        return returnValue;
    }
    
    /**
     * Adds a WinBolo.net Event for sending to the server
     * @param eventType Type of event this is
     * @param isServer Are we the server for this and not a client
     * @param playerA Player A player Number
     * @param playerB Player B player Number
     */
    public void addEvent(byte eventType, boolean isServer, int playerA, int playerB) {
        WBNEvent add;
        byte keyB[];
        
        if (winboloNetRunning == true && isServer == true && eventType != WINBOLO_NET_EVENT_NO_EVENT) {
            if (playerB == WINBOLO_NET_NO_PLAYER) {
                keyB = noPlayerArray;
            } else {
                keyB = winboloNetPlayerKey[playerB];
            }
            
            add = new WBNEvent(eventType, winboloNetPlayerKey[playerA], keyB);
            events.add(add);
        }
    }
    
    /*
     * Returns if we are running and participating in winbolo.net
     */
    /**
     * Returns if we are participating in WBN
     * @return boolean indicating if we are running
     */    
    public boolean isRunning() {
        return winboloNetRunning;
    }
  
    /**
     * Sends the list of teams at the end of the game.
     *
     * @param value byte array of data
     * @param numTeams Number of teams present in this array. 
     */
    public void serverSendTeams(byte value[], int numTeams) {
         ByteArrayOutputStream bos;
         int count = 0;
         byte buff[];
         
         try {
             bos = new ByteArrayOutputStream(256);
             bos.write(header);
             bos.write(WINBOLO_NET_MESSAGE_TEAMS);
             bos.write(serverKey);
             bos.write((byte) numTeams);
             
             while (count < value.length) {
                 if (value[count] == WINBOLO_NET_TEAM_MARKER) {
                     bos.write(WINBOLO_NET_TEAM_MARKER);
                 } else if (winboloNetPlayerKey[value[count]][0] == 0) {
                     bos.write(0);
                 } else {
                     bos.write(winboloNetPlayerKey[value[count]]);
                 }
                 count++;
             }
             
             /* Send it */
             buff = bos.toByteArray();
             http.sendMessage(buff, buff.length);
         } catch (Exception e) {
             
         }
    }

    /**
     * Requests a client key for the game from WBN. Copies serverkey and assigns player key
     * to winboloNetPlayerKey[0] (This function is only called by clients
     *
     * @return String containing the error message or null if succesful
     * @param userName Players name to use
     * @param password Players password to authenticate with
     * @param key Byte array containing the server WBN key
     */
    private String requestClientKey(String userName, String password, byte key[]) throws Exception {
        String returnValue = null;
        ByteArrayOutputStream bos = new ByteArrayOutputStream(64);
        byte buff[];
        byte ret[];
        int count = 0;
        
        bos.write(header);
        bos.write(WINBOLO_NET_MESSAGE_CLIENTKEY_REQ);
        bos.write(key);
        bos.write(Utils.StringToPByteArray(userName));
        bos.write(Utils.StringToPByteArray(password));        
        buff = bos.toByteArray();
        ret = http.sendMessage(buff, buff.length);
        if (ret.length > 0) {
            if (ret[0] == 0) {
                returnValue = new String(ret, 1, ret.length - 1);
            } else if (ret.length == 33) {
                serverKey = key;
                while (count < WINBOLONET_KEY_LEN) {
                    winboloNetPlayerKey[0][count] = ret[count+1];
                    count++;
                }
            } else {
                returnValue = "Error: No response from WinBolo.net - Winbolo.net disabled";            
            }
        } else {
            returnValue = "Error: No response from WinBolo.net - Winbolo.net disabled";
        }
        
        return returnValue;
    }
    
    /**
     * Sends a server WBN update
     *
     * @param numPlayer    - Number of pleyers in the game
     * @param numFreePills - Number of free bases in the game
     * @param numFreePills - Number of free pills in the game
     * @param sendNow      - If TRUE the data should not be queued
     */
    private void sendServerUpdate(int numPlayers, int numFreeBases, int numFreePills, boolean sendNow) {
        ByteArrayOutputStream bos;
        ListIterator<WBNEvent> li;
        WBNEvent current;
        byte keyA[];
        byte keyB[];
        
        if (winboloNetRunning == true) {
            if (events.size() > 0 || numFreePills != wbnFreePills || numFreeBases != wbnFreeBases || numPlayers != wbnNumPlayers || System.currentTimeMillis() - winboloNetLastSent > WINBOLO_NET_MAX_NOSEND) {
                wbnFreePills = numFreePills;
                wbnFreeBases = numFreeBases;
                wbnNumPlayers = numPlayers;
                try {
                    bos = new ByteArrayOutputStream(128);
                    bos.write(header);
                    bos.write(WINBOLO_NET_MESSAGE_SERVERUPDATE_REQ);
                    bos.write((byte) numPlayers);
                    bos.write((byte) numFreeBases);
                    bos.write((byte) numFreePills);
                    bos.write(serverKey);
                    
                    li = events.listIterator();
                    while (li.hasNext()) {
                        current = li.next();
                        keyA = current.getKeyA();
                        keyB = current.getKeyB();
                        bos.write(current.getEventType());
                        if (keyA[0] == 0) {
                            bos.write(0);
                        } else {
                            bos.write(keyA);
                        }
                        if (keyB[0] == 0) {
                            bos.write(0);
                        } else {
                            bos.write(keyB);
                        }
                    }
                    events.clear();
                    if (sendNow == false) {
                        wbnThread.addRequest(new WBNThreadItem(bos.toByteArray()));
                    } else {
                        keyA = bos.toByteArray();
                        keyB = http.sendMessage(keyA, keyA.length);
                        if (keyB.length > 0) {
                            if (keyB[0] == 0) {
                                /* Error */
                            }
                        } else {
                            // Commented on purpose         fprintf(stderr, "Error: %s\n", winboloNetBuff+1);
                        }
                    }
                } catch (Exception e) {
                    // Ignore error
                }
                winboloNetLastSent = System.currentTimeMillis();
            }
        }
    }
    
    /**
     * Sends the goodbye message to WBN
     */
    private void goodbye() {
        ByteArrayOutputStream bos = new ByteArrayOutputStream(64);
        byte output[];
        byte ret[];
        
        sendServerUpdate(0,0,0, true);
        
        try {
            bos.write(header);
            bos.write(WINBOLO_NET_MESSAGE_SERVERQUIT_REQ);
            bos.write(serverKey);
            output = bos.toByteArray();
            ret = http.sendMessage(output, output.length);
            if (ret.length > 0) {
                if (ret[0] == 0) {
                    /* Error */
                }
            } else {
                System.err.println("Error: No response from WinBolo.net");
            }
        } catch (Exception e) {
            // Ignore
        }
        
    }
    
    
    /**
     * Requests a server key from WBN and set member variable key
     *
     * @return if key is succesfully retrieved
     * @param mapName Map name being played on this server
     * @param port Port of this server
     * @param gameType Game type of this server
     * @param ai AI/Computer brains allowed in this server
     * @param mines Hidden mines enabled in this server
     * @param numBases Number of bases in this game
     * @param numPills Number of pills in this game
     * @param freeBases Number of free bases in this game
     * @param freePills Number of free pills in this game
     * @param numPlayers Number of players in this game
     * @param startTime Time this game was started
     */
    private boolean requestServerKey(String mapName, int port, int gameType, int ai, boolean mines, boolean password, int numBases, int numPills, int freeBases, int freePills, int numPlayers, long startTime) throws Exception {
        boolean returnValue = false;        /* Value to return */
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        byte val[];
        byte output[];
        
        returnValue = false;
        //FIXME        startTime = htonl(startTime);
        //FIXME        port = htons(port);
        
        bos.write(header);
        bos.write(WINBOLO_NET_MESSAGE_SERVERKEY_REQ);
        bos.write((byte) gameType);
        bos.write((byte) ai);
        bos.write((byte) Utils.BooleanToByte(mines));
        bos.write((byte) Utils.BooleanToByte(password));
        bos.write((byte) freeBases);
        bos.write((byte) freePills);
        bos.write((byte) numPlayers);
        bos.write((byte) numBases);
        bos.write((byte) numPills);
        
        bos.write(Utils.StringToPByteArray(mapName));
        //FIXME        memcpy(buff+buffPos, &port, sizeof(port));
        //FIXME        memcpy(buff+buffPos, &startTime, sizeof(startTime));
        output = bos.toByteArray();
        val = http.sendMessage(output, output.length);
        if (val.length > 0) {
            if (val[0] == 0) {
                /* Error */
            } else if (val.length == 33) {
                // FIXME                memcpy(winboloNetServerKey, winboloNetBuff+1, WINBOLONET_KEY_LEN);
                returnValue = true;
            }
        } else {
            System.err.println("Error: No response from WinBolo.net - Winbolo.net disabled\n");
        }
        
        return returnValue;
        
    }
    
    
    /**
     * Sends the game version number to WBN
     */
    private void sendVersion() {
        byte buff[];
        ByteArrayOutputStream bos;
        
        if (winboloNetRunning == true) {
            try {
                bos = new ByteArrayOutputStream(64);
                bos.write(header);
                bos.write(WINBOLO_NET_MESSAGE_VERSION_REQ);
                bos.write(serverKey);
                //FIXME
                //bos.write("VERSION NUMBER GOES HERE FIXME")
                buff = bos.toByteArray();
                http.sendMessage(buff, buff.length);
            } catch (Exception e) {
                winboloNetRunning = false;
            }
        }
    }
    
    /**
     * Sends the game lock status to WBN
     * @param isLocked Whether this game is locked or not
     */
    public void sendLocked(boolean isLocked) {
        byte buff[];
        ByteArrayOutputStream bos;
        
        if (winboloNetRunning == true) {
            try {
                bos = new ByteArrayOutputStream(64);
                bos.write(header);
                bos.write(WINBOLO_NET_MESSAGE_VERSION_REQ);
                bos.write(serverKey);
                bos.write(Utils.BooleanToByte(isLocked));
                buff = bos.toByteArray();
                http.sendMessage(buff, buff.length);
            } catch (Exception e) {
                winboloNetRunning = false;
            }
        }
    }
    
}
