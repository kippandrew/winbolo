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
 * TrackerFinderConnection.java
 *
 * Created on 20 May 2004, 20:30
 */

package com.jbolo.gui.dialog.gamefinder;

import java.util.ArrayList;
import java.awt.Label;
import java.net.Socket;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.InputStreamReader;
import java.io.InputStream;
import java.io.ByteArrayInputStream;
import java.util.Properties;
import java.util.StringTokenizer;
import com.jbolo.gui.BoloPreferences;
import com.jbolo.gui.dialog.MessageBox;
import com.jbolo.bolo.GameType;
import com.jbolo.bolo.Utils;


import java.io.FileInputStream;

/**
 * Provides a way to download game list from the tracker
 * @author John Morrison
 */
public class TrackerFinderConnection extends GameFinderConnection {
    
    /** Creates a new instance of TrackerFinderConnection */
    public TrackerFinderConnection() {
        super();
    }

    
    /**
     * Searches for a list of games
     *
     * @param status Status label to display messages
     */
    public void searchForGames(Label status) {
        Socket sock;
        String address;
        String portString;
        int port;
        
        games.clear();
        address = BoloPreferences.GetInstance().getPrivateProfileString("TRACKER", "Address", "tracker.winbolo.com");
        
        try {
            port = Integer.parseInt(BoloPreferences.GetInstance().getPrivateProfileString("TRACKER", "Port", "50000"));
        } catch (Exception e) {
            port = 50000;
        }
        
        
        status.setText("Connectiing to " + address + ":" + port);
        
        try {
            sock = new Socket(address, port);
        } catch (Exception e) {
            status.setText("Error: Unable to connect to tracker");
            result = false;
            return;
        }
        
        try {
            processList(status, sock);
        } catch (ProcessingErrorDisplay pe) {
            status.setText("Error: " + pe.getMessage());
            result = false;
            return;
        } catch (Exception e) {
            status.setText("Error: Error processing results");
            result = false;
            return;
        }
        
        result = true;
    }
    
    /**
     * Process the tracker connection
     * @throws Exception on invalid data format
     * @param status Status field to update
     * @param sock Socket connection to the tracker
     */
    private void processList(Label status, Socket sock) throws Exception {
        ByteArrayInputStream bis;
        int numGames;
        int motdLines;
        String line;
        Properties p = new Properties();
        byte buff[] = new byte[32 * 1024];
        int len;
        int size = 0;
        InputStream is = sock.getInputStream();


        
        
        len = is.read(buff);
        while (len != -1) {
            size += len;
            len = is.read(buff, size, buff.length-size);
        }
        
        
        p.load(new ByteArrayInputStream(buff));
        
        
        
        if (Integer.parseInt(p.getProperty("TVERSION")) != 1) {
            throw new ProcessingErrorDisplay("Error: Invalid tracker protocol version");
        }
        
        processMOTD(Integer.parseInt(p.getProperty("MOTDL")), buff);
        processGames(Integer.parseInt(p.getProperty("NGAMES")), buff);
        result = true;
    }
    

    /**
     * Process the list of games from the tracker input
     *
     * @throw Exception on invalid data format
     * @param numGames Number of games the tracker reported
     * @param buff Byte array of entire data returned from the tracker
     */
    private void processGames(int numGames, byte buff[]) throws Exception {
        String data = new String(buff);
        CurrentGame game;
        int count = 0;
        int startPos = 0;
        int endPos = 0;
        
        while (count < numGames) {
            if (count == 1) {
                startPos = data.indexOf("GAME" + formatGameId(count), startPos);;
            } else {
                startPos = endPos;
            }
                
            if (count == (numGames-1)) {
                endPos = buff.length;
            } else {
                endPos = data.indexOf("GAME" + formatGameId(count+1), startPos);
            }
            game = getGame(new ByteArrayInputStream(buff, startPos, endPos-startPos), count);
            games.add(game);
            count++;
        }
        
    }
    
    /**
     * Formats the game number into a 3 digit straing prefixed by zeros
     *
     * @param gameNum Integer number to convert
     * @return String
     */
    private String formatGameId(int gameNum) {
        if (gameNum < 10) {
            return "00" + gameNum;
        } else if (gameNum < 99) {
            return "0" + gameNum;
        }
        return Integer.toString(gameNum);
    }
    
    /**
     * GetGame reads a tracker input format for a game
     *
     * @param is InputStream containing just the game
     * @param gameNum Game Number we are processing
     * @throw Exception on data format error
     */
    private CurrentGame getGame(InputStream is, int gameNum) throws Exception {
        Properties p = new Properties();
        String address;
        int port;
        String addrLine;
        String gameId;
        int colonPos;
        
        gameId = "GAME"+ formatGameId(gameNum);
        p.load(is);
        addrLine = p.getProperty(gameId);
        colonPos = addrLine.indexOf(':');
        address = addrLine.substring(0, colonPos);
        port = Integer.parseInt(addrLine.substring(colonPos+1));
        return new CurrentGame(address, p.getProperty("MAP"), p.getProperty("VERSION"), port, Integer.parseInt(p.getProperty("PLAYERS")), Integer.parseInt(p.getProperty("BASES")), Integer.parseInt(p.getProperty("PILLS")), Utils.ConvertYesNoToBool(p.getProperty("HIDMINES")), convertGameTypeString(p.getProperty("TYPE")), convertAI(p.getProperty("BRAINS")), Utils.ConvertYesNoToBool(p.getProperty("PASSWORD")));
    }
    

    /**
     * processMOTD reads in the MOTD= lines from the tracker
     *
     * @param numLines Number of MOTD lines
     * @param buff Byte array input from the tracker
     * @throw Exception on data format error, invalid number of MOTD lines
     */
    private void processMOTD(int numLines, byte buff[]) throws Exception {
        int count = 0;
        String data;
        String line;
        StringTokenizer toks;
        
        if (numLines < 0) {
            throw new Exception("invalid number");
        }
        
        if (numLines > 0) {
            data = new String(buff).trim();
            toks = new StringTokenizer(data, "\r\n");
            
            
            while (count < numLines) {
                line = toks.nextToken();
                while (line.startsWith("MOTD=") == false) {
                    line = toks.nextToken();
                }
                /* Process the mod line */
                if (motd.length() > 0) {
                    motd += '\n';
                }
                motd += line.substring(5);
                count++;
            }
        }
    }    
    
    
    /**
     * ConvertGameTypeString converts the game type string into a GameType value
     *
     * @param value Value to convert
     * @return int
     * @throw Exception on invalid value
     */
    private int convertGameTypeString(String value) throws Exception {
        value = value.toLowerCase();
        if (value.equals("open")) {
            return GameType.OPEN_GAME_TYPE;
        } else if (value.equals("tournament")) {
            return GameType.TOURNAMENT_GAME_TYPE;
        } else if (value.equals("strict")) {
            return GameType.STRICT_GAME_TYPE;
        }
        
        throw new Exception("Invalid format");
    }
    
    /**
     * Convert AI converts the AI no,yes,yesAdv string into a value 0,1,2
     *
     * @param value Value to convert
     * @return int
     * @throw Exception on invalid value
     */
    private int convertAI(String value) throws Exception {
        value = value.toLowerCase();
        
        if (value.equals("no")) {
            return 0;
        } else if (value.equals("yes")) {
            return 1;
        } else if (value.equals("yesadv")) {
            return 2;
        }
        
        throw new Exception("Invalid format");
    }
        
}
