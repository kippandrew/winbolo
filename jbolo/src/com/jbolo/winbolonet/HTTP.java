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
 * HTTP.java
 *
 * Created on 16 May 2004, 20:23
 */

package com.jbolo.winbolonet;

import java.net.Socket;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import com.jbolo.gui.BoloPreferences;


/**
 * Responsable for sending/receiving http messages
 * @author  John Morrison
 */
public class HTTP {
    
    /** Is the http system operational */
    private boolean httpStarted;
    
    /* Host String Name Host: wbnHostString */
    private String wbnHostString;
    
    /** Host String Name Host: wbnHostString */
    private String hostString;
    
    /** Creates a new instance of HTTP */
    public HTTP() {
        httpStarted = false;        
        BoloPreferences bp = BoloPreferences.GetInstance();
        wbnHostString = bp.getPrivateProfileString("WINBOLO.NET", "Host", "wbn.winbolo.net");
        bp.writePriveteProfileString("WINBOLO.NET", "Host", wbnHostString);
        hostString = "Host: " + wbnHostString + "\n\n";
        httpStarted = true;
    }
    
    /**
     * Shutsdown the HTTP subsystem
     */
    public void shutdown() {
        httpStarted = false;
    }
    
    /**
     * Converts a byte array into a URL encoded byte array
     * 
     * @param src byte array to change
     * @param sourceLen Length of the byte array
     * @return encoded byte array
     */
    private byte[] encode(byte src[], int sourceLen) {
        ByteArrayOutputStream bos = new ByteArrayOutputStream(src.length*2);
        int count = 0;
        char hex[];
        
        while (count < sourceLen) {
            if ((src[count] >= '0' && src[count] <= '9') || (src[count] >= 'a' && src[count]<= 'z') || (src[count]>= 'A' && src[count]<= 'Z')) {
                /* Valid */
                bos.write(src[count]);
            } else {
                /* Encode it */
                hex = Integer.toHexString(src[count]).toCharArray();
                bos.write('%');
                if (src[count] <= 0xF) {
                    bos.write('0');
                    bos.write(hex[hex.length-1]);
                } else {
                    bos.write(hex[hex.length-2]);
                    bos.write(hex[hex.length-1]);
                }
            }
            count++;
        }
        
        return bos.toByteArray();
    }
    
    /**
     * Converts a hex value into a decimal value
     */
    private int hexToInt(byte val) {
        if (val >= '0' && val <='9') {
            return val;
        }
        return val - 30;
    }

    public byte[] decode(byte src[], int length) {
        ByteArrayOutputStream bos = new ByteArrayOutputStream(src.length);
        int count = 0;
        while (count < length) {
            if (src[count] == '%') {
                count++;
                bos.write(hexToInt(src[count]) * 16 + hexToInt(src[count+1]));
                count += 2;
            } else {
                bos.write(src[count]);
                count++;
            }
        }
        
        return bos.toByteArray();
    }
    
    private byte[] receiveData(Socket sock) throws Exception {
        InputStream is = sock.getInputStream();
        byte data[] = new byte[1024];
        byte rv[];
        int size;
        int pos = 0;
        int readLen;
        boolean found = false;
        int count = 0;
        
        // Read the data
        readLen = is.read(data);
        size = readLen;
        while (readLen != -1) {
            readLen = is.read(data, size, 1024-pos);
            if (readLen != -1) {
                size += readLen;
            }
        }

        // Check we read something
        if (pos == 0) {
            return null;
        }
        
        // Strip out the header
        while (count < size-3 && found == false) {
            if (data[count] == '\r' && data[count+1] == '\n' && data[count+2] == '\r' && data[count+3] == '\n') {
                found = true;
            } else {
                count++;
            }
        }
        
        if (found == false) {
            return null;
        }

        // Skip over \r\n\r\n
        count+=4;
        rv = new byte[size-count];

        while (count < size) {
            rv[pos] = data[count];
            pos++;
            count++;
        }
        
        return decode(rv, size-count);
        
    }
    
    public byte[] sendMessage(byte message[], int len) {
        byte rv[] = null;
        Socket sock;
        if (httpStarted == true) {
            try {
                sock = new Socket(wbnHostString, 80);
                sock.getOutputStream().write(encode(message, len));
                rv = receiveData(sock);
            } catch (Exception e) {
                //FIXME
                e.printStackTrace();
                rv = null;
            }
        }
        
        return rv;
    }
}
