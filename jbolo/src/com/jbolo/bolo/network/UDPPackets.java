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
 * UDPPackets.java
 *
 * Created on 23 May 2004, 23:13
 */

package com.jbolo.bolo.network;

/**
 * Handles keeping track of network packets for
 * retransmission on errors
 *
 *  Process: Client to Server
 * -------------------------
 * Client--->
 * 1. Build reliable packet and give it a sequence number
 * 2. Send
 * 3. Copy packet to sequence number position X of buffer.
 * 4. Increment next packet sequence number.
 * Server--->
 * 5. Server Receives packet
 * 6. Server checks sequence number. If valid increments to
 *    next available sequence number and process packet.
 * 7. If invalid sends back a packet request for missing
 *    sequence number(s) and stores packets at sequence
 *    position for processing.
 *
 *
 * Process: Server to Client
 * -------------------------
 * Server-->
 * 1. Build reliable packet.
 * 2. For each client makes the correct sequence number
 *    and sends it.
 * 3. Copy packet to sequence number position x of buffer
 * 4. Increment next packet sequence number
 * Client--->
 * 5. Client Receives packet
 * 6. Client checks sequence number. If valid increments
 *    to next available sequence number.
 * 7. If invalid sends back a packet request for missing
 *    sequence number(s) and stores packets at sequence
 *    position for processing.
 *
 * @author  John Morrison
 * @provides bolo/udppackets.c
 */
public class UDPPackets {
    
    /** Number of reliable sequence packets we have before they rotate */
    public static int MAX_UDP_SEQUENCE = 200; /* 64 */
    /** Packets are of maximum size of 1K */
    private static int SIZE_OF_PACKET = 1024;
    /** -1 indicates there is no packet at this sequence */
    private static int UDP_NO_PACKET = -1;
    /** A non reliable packet will have this marker */
    public static int UDP_NON_RELIABLE_PACKET = 254;
    
    
    /** The next packet sequence number to get for sending out */
    private int outSequenceNumber;
    /** Copy of the packets for retransmission */
    private byte outBuff[][];
    /** Size of each packet */
    private int outLens[];
    /** The next packet sequence number expected */
    private int inSequenceNumber;
    /** Sequence number to process up to  on error */
    private int inUpTo;
    /** Copy of the incoming packets for processing on missing packets */
    private byte inBuff[][];
    /** Size of each packet */
    private int inLens[];
    
    /** Creates a new instance of UDPPackets */
    public UDPPackets() {
        int count = 0; // Looping variable
        
        inUpTo = 0;
        inSequenceNumber = 1; /* In sequence number is always +1 */
        outSequenceNumber = 0;
        outBuff = new byte[MAX_UDP_SEQUENCE][SIZE_OF_PACKET];
        outLens = new int[MAX_UDP_SEQUENCE];
        inBuff = new byte[MAX_UDP_SEQUENCE][SIZE_OF_PACKET];
        inLens = new int[MAX_UDP_SEQUENCE];
        while (count < MAX_UDP_SEQUENCE) {
            outLens[count] = UDP_NO_PACKET;
            inLens[count] = UDP_NO_PACKET;
            count++;
        }
    }
    
    /**
     * Gets the outbound sequence number
     * @return int
     */
    public int getOutSequenceNumber() {
        return outSequenceNumber;
    }
    
    /**
     * Gets the inbound sequence number
     * @return int
     */
    public int getInSequenceNumber() {
        return inSequenceNumber;
    }
    
    /**
     * Incremements and returns the next inbound sequence number
     * @return int
     */
    public int getNextInSequenceNumber() {
        inSequenceNumber++;
        if (inSequenceNumber == MAX_UDP_SEQUENCE) {
            inSequenceNumber = 0;
        }
        return inSequenceNumber;
    }
    
    /**
     * Incremements and returns the next outbound sequence number
     * @return int
     */
    public int getNextOutSequenceNumber() {
        outSequenceNumber++;
        if (outSequenceNumber == MAX_UDP_SEQUENCE) {
            outSequenceNumber = 0;
        }
        return outSequenceNumber;
    }
    
    /**
     * Gets the inbound byte buffer at value
     * @return byte array
     * @param value Position to get at
     */
    public byte[] getInBuff(int value) {
        return inBuff[value];
    }

    /**
     * Gets the outbound byte buffer at value
     * @return byte array
     * @param value Position to get at
     */
    public byte[] getOutBuff(int value) {
        return outBuff[value];
    }

    /**
     * Gets the inbound buffer length at value
     * @return int
     * @param value to get length for
     */
    public int getInBuffLength(int value) {
        return inLens[value];
    }

    /**
     * Gets the outbound buffer length at value
     * @return int
     * @param value to get length for
     */
    public int getOutBuffLength(int value) {
        return outLens[value];
    }
    
    /**
     * Copies the packet to the inbound buffer
     * @param value Position to copy to 
     * @param data byte array of data
     * @param length Length of the data
     */
    public void setInBuff(int value, byte data[], int length) {
        int count = 0; // Looping variable
        
        inLens[value] = length;
        while (count < length) {
            inBuff[value][count] = data[count];
            count++;
        }
    }

    /**
     * Copies the packet to the outbound buffer
     * @param value Position to copy to 
     * @param data byte array of data
     * @param length Length of the data
     */
    public void setOutBuff(int value, byte data[], int length) {
        int count = 0; // Looping variable
        
        outLens[value] = length;
        while (count < length) {
            outBuff[value][count] = data[count];
            count++;
        }
    } 
}
