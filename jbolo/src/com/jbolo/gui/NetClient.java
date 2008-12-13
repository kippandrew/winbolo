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
 * NetClient.java
 *
 * Created on 19 May 2004, 22:32
 */

package com.jbolo.gui;

import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.DatagramPacket;
import java.io.ByteArrayOutputStream;
import com.jbolo.gui.dialog.MessageBox;
import com.jbolo.bolo.network.Network;
import com.jbolo.bolo.network.UDPPackets;
import com.jbolo.bolo.CRC;

/**
 * UDP Packet transport mechanism
 * @author  John Morrison
 */
public class NetClient {
    
    /** Maximum size of a UDP packet */
    private static int MAX_UDPPACKET_SIZE = 1024;
    
    /** Port we are bound to */
    private int myPort;
    /** Our UDP Socket */
    private DatagramSocket myUdpSock;
    /** Server address */
    private InetAddress addrServer;
    /** Server Port */
    private int serverPort;
    /* Where the last UDP packet came from */
    private InetAddress addrLast;
    /** Port the last UDP packet came from */
    private int lastPort;
    /** Our own address */
    private InetAddress addrUs;
    /** Tracker machine address */
    private InetAddress addrTracker;
    /** Port of the tracker */
    private int trackerPort;
    
    private Network net;
    
    /** Creates a new instance of NetClient */
    public NetClient(Network net, int port) {
        myPort = port;
        this.net = net;
        myUdpSock = null;
    }
    
    /**
     * Trys to setup this NetClients socket
     * @return success. If failure socket is unbound
     */
    public boolean setup() {
        try {
            myUdpSock = new DatagramSocket(myPort);
            
        } catch (Exception e) {
            new MessageBox("Unable to Create a Bound Socket", MessageBox.TITLE);
            return false;
        }
        
        if (setAsyncMode(true) == false) {
            shutdown();
            return false;
        }
        
        return true;
    }
    
    /**
     * Shutsdown our socket
     */
    public void shutdown() {
        if (myUdpSock != null) {
            try {
                myUdpSock.close();
                myUdpSock = null;
            } catch (Exception e) {
                // Can ignore this exception as we are just shutting down anyway
            }
        }
    }
    
    /**
     * Sets the tracker
     *
     * @param address Tracker address
     * @param port Port of tracker
     * @return If the tracker address can be set
     */
    public boolean setTracker(String address, int port) {
        try {
            addrTracker = InetAddress.getByName(address);
            this.trackerPort = port;
        } catch (Exception e) {
            // Unable to lookup tracker address
            return false;
        }
        return true;
    }
    
    public int ping(String host, int port, byte data[], int length, boolean wantCRC, boolean addNonReliable) {
        DatagramPacket p;
        DatagramPacket recv;
        int crc;
        int crcA;
        int crcB;
        long timeOut;
        long tick;
        
        try {
            if (addNonReliable == true) {
                data[length] = (byte) UDPPackets.UDP_NON_RELIABLE_PACKET;
                length++;
            }
            if (wantCRC == true) {
                crc = CRC.CRCCalc(data, length);
                //FIXME  endian
                crcA = crc >> 8;
                crcB = crc & 0xFF;
                data[length] = (byte) crcA;
                length++;
                data[length] = (byte) crcB;
                length++;       
            }
            
            
            p = new DatagramPacket(data, length);
            p.setAddress(InetAddress.getByName(host));
            p.setPort(port);
            myUdpSock.send(p);
            p.setData(data);
            timeOut = 0;
            tick = System.currentTimeMillis();
            while (timeOut < 10000) {
                try {
                    myUdpSock.receive(p);
                    addrLast = p.getAddress();
                    return p.getLength();
                } catch (Exception e) {
                    
                }
                
                timeOut = System.currentTimeMillis() - tick;
            }
            
        } catch (Exception e) {
            return -1;
        }
        
        return -1;
    }
    
    /**
     * Sends a UDP packet to the last address we received a packet from.
     *
     * @param buff Buffer to send
     * @param len Length of buffer
     */
    public void sendLast(byte buff[], int len) {
        send(buff, len, addrLast, lastPort);
    }
    
    /**
     * Sends a UDP packet to the server.
     *
     * @param buff Buffer to send
     * @param len Length of buffer
     */
    public void sendServer(byte buff[], int len) {
        send(buff, len, addrServer, serverPort);
    }
    
    public void sendTracker(byte buff[], int len) {
        send(buff, len, addrTracker, trackerPort);
    }
    
    /**
     * Returns the server address
     * @return InetAddress
     */
    public InetAddress getServerAddress() {
        return addrServer;
    }
    
    /**
     * Returns the server port
     * @return port in local byte order
     */
    public int getServerPort() {
        return serverPort;
    }
    
    /**
     * Returns this server address as a host:port string
     * @return String
     */
    public String getServerAddressPortString() {
        return addrServer.getHostAddress() + ":" + serverPort;
    }
    
    /**
     * Returns this server address as a host:port string
     * @return String
     */
    public String getServerAddressString() {
        return addrServer.getHostAddress();
    }
    
    public byte[] getLastAddress() {
        return addrLast.getAddress();
    }
    
    /**
     * Sets the server port
     *
     * @param port Port to set
     */
    public void setServerPort(int port) {
        serverPort = port;
    }
    
    /**
     * Sets the server address
     *
     * @param addr Byte array containing the ip address to set
     * @param port Port of server to set
     */
    public void setServerAddress(byte addr[], int port) {
        try {
            addrServer = InetAddress.getByAddress(addr);
        } catch (Exception e) {
            // Wont happen as we are setting an IP address
        }
        serverPort = port;
    }
    
    /**
     * Checks for new data on the socket
     * Calls callback if found
     */
    public void check() {
        byte data[] = new byte[MAX_UDPPACKET_SIZE];
        DatagramPacket p = new DatagramPacket(data, MAX_UDPPACKET_SIZE);
        boolean finished = false;
        
        while (finished == false) {
            try {
                myUdpSock.receive(p);
                addrLast = p.getAddress();
                lastPort = p.getPort();
                net.packetArrive(p.getData(), p.getLength(), lastPort);
            } catch (Exception e) {
                // Exception thrown on no more data
                finished = true;
            }
        }
    }
    
    /**
     * Sets our socket asyncronous mode
     *
     * @return Operation succesfulness
     * @param mode Whether it is asyncronous or not
     */
    private boolean setAsyncMode(boolean mode) {
        int value = 0;
        if (mode == true) {
            value = 1;
        }
        try {
            myUdpSock.setSoTimeout(1);
        } catch (Exception e) {
            return false;
        }
        return true;
    }
    
    /**
     * Sends a UDP packet to an address and port
     *
     * @param buff Buffer to send
     * @param len Length of the buffer
     * @param address Address of the destination
     * @param port Port of the destination
     */
    private void send(byte buff[], int len, InetAddress address, int port) {
        DatagramPacket p = new DatagramPacket(buff, len, address, port);
        try {
            myUdpSock.send(p);
        } catch (Exception e) {
            // Send exception ignored
        }
    }
}
