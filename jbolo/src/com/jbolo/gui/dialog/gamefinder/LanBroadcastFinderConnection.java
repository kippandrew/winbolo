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
 * LanBroadcastFinderConnection.java
 *
 * Created on 21 May 2004, 22:45
 */

package com.jbolo.gui.dialog.gamefinder;

import java.awt.Label;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketTimeoutException;
import java.net.InetAddress;
import com.jbolo.bolo.network.NetPacks;
import com.jbolo.bolo.network.InfoResponse;

/**
 *
 * @author  John Morrison
 */
public class LanBroadcastFinderConnection extends GameFinderConnection {
    
    /** Creates a new instance of LanBroadcastFinderConnection */
    public LanBroadcastFinderConnection() {
        super();
    }
    
    /**
     * Searches for a list of games
     *
     * @param status Label to display messages
     */
    public void searchForGames(Label status) {
        DatagramSocket sock;
        DatagramPacket send;
        DatagramPacket recv;        
        byte sendBytes[] = NetPacks.INFOREQUESTHEADER;
        byte recvBytes[] = new byte[512];
        long startTime;
        
        games.clear();
        try {
            sock = new DatagramSocket(0);
            sock.setBroadcast(true);
            sock.setSoTimeout(10);
        } catch (Exception e) {
            status.setText("Unable to create a socket");
            return;
        }
        
        
        // Send the data
        try {
            send = new DatagramPacket(sendBytes, sendBytes.length, InetAddress.getLocalHost(), 27500);
            sock.send(send);
        } catch (Exception e) {
            e.printStackTrace();
        }
        startTime = System.currentTimeMillis();
        while (System.currentTimeMillis() - startTime < 10000) { // Process for 10 seconds
            try {
                recv = new DatagramPacket(recvBytes, recvBytes.length);
                sock.receive(recv);
                process(recv.getData(), recv.getLength(), recv.getAddress());
            } catch (SocketTimeoutException ste) {
                // OK to ignore
            } catch (Exception e) {
                // Bad exception
                e.printStackTrace();
            }
        }
        result = true;
    }
    
    /**
     * Process a response to look for info packets
     *
     * @param data Data received
     * @param len Length of the data received
     * @param addr InetAddress data originated from
     */
    private void process(byte data[], int len, InetAddress addr) {
        InfoResponse ir;
        CurrentGame cg;
        String address;
        
        try {
            ir = new InfoResponse(data, len);
            address = ir.getAddress();
            if (address.equals("0.0.0.0") == true) {
                ir.setAddress(addr.getHostAddress());
            }
            cg = new CurrentGame(ir.getAddress(), ir.getMapName(), ir.getVersion(), ir.getPort(), ir.getNumPlayers(), ir.getNumFreeBases(), ir.getNumFreePills(), ir.getHiddenMines(), ir.getGameType(), ir.getAI(), ir.hasPassword());
            games.add(cg);            
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
