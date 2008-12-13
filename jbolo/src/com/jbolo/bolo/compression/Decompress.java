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
 * Decompress.java
 *
 * Created on 17 June 2004, 06:44
 */


/* Heavily based upon dcodrle1.c by David Bourgin released in the public domain */
/* This used to be a LZW implemtnation, hence the directory, file and function names but replaced by a RLE implementation as it was more effecient. Could probably be replaced with built in map RLE functions. */


package com.jbolo.bolo.compression;

import java.io.ByteArrayOutputStream;
import com.jbolo.bolo.Utils;

/**
 * Used to decompress RLE data for map transfer.
 * Example of RLE type 1 decoding with a file source to decompress.
 *
 * @author David Bourgin
 * @author John Morrison
 */
public class Decompress {
    
    /** Decompressed data */
    private byte data[];
    /** Where we are up to decompressing */
    private int upto;
    /** Compressed data length */
    private int srcLen;
    /** Source data byte array */
    private byte srcData[];
    /** Used to write data to */
    private ByteArrayOutputStream bos;
    
    /** Creates a new instance of Decompress */
    public Decompress(byte src[], int len) {
        bos = new ByteArrayOutputStream(len * 3); // Start off with a reasonable sized buffer;
        srcLen = len;
        srcData = src;
        
        try {
            process();
            data = bos.toByteArray();
            bos = null;
/*          
 *Debug map decompress should be this big  
            if (data.length != 1290) {
                System.exit(0);
            } 
 */
        } catch (Exception e) {
            // Shouldn't happen :)
        }
    }
    
    /**
     * Gets the uncompressed data
     * @return Byte array of uncompressed data
     */
    public byte[] getData() {
        return data;
    }
    
    /**
     * Gets the data length
     * @return length of decompressed data
     */
    public int getLength() {
        return data.length;
    }
    
    /**
     * Are we finished decompressing?
     * @return boolean
     */
    private boolean complete() {
        if (upto < srcLen) {
            return true;
        }
        return false;
    }
    
    /**
     * Reads a byte from the compressed data
     * @return int representing a byte to get around java's signed only typews
     */
    private int readByte() {
        upto++;
        return Utils.ConvertByteToInt(srcData[upto-1]);
    }
    
    /**
     * Writes a byte to the decompressed data
     * @param value Byte to write
     */
    private void writeByte(byte value) {
        bos.write(value);
    }
    

    /**
     * Writes a block of repeated data
     *
     * @param b Byte to write
     * @param num Number of times to repeat it
     */
    private void writeBlock(int b, int num) throws Exception {
        int count = 0;
        while (count < num) {
            bos.write(b);
            count++;
        }
    }
    
    /**
     * Performs the data decompression
     */
    private void process() throws Exception {
        int header;
        int count;
        
        
        while (upto < srcLen) {
            header = readByte();
            switch (header & 128) {
                case 0:
                    count = 0;
                    while (count <= header) {
                        writeByte((byte) readByte());
                        count++;
                    }
                    break;
                case 128:
                    writeBlock(readByte(), (header & 127)+2);
                    break;
            }
            
        }
    }
    
}
