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
 * CRC.java
 *
 * Created on 19 May 2004, 22:18
 */

package com.jbolo.bolo;

/**
 * Generates and validates 16 bit CRCs on data
 * @author  John Morrison
 */
public class CRC {
    
    
    private static int crc_16_table[] = {
        0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
        0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400 };
        
        
        /**
         * Calculates a 16 bit CRC for a byte array
         *
         * @return Calculated CRC
         * @param buff Byte array to generated CRC for
         * @param length Length of the byte array
         */
        public static int CRCCalc(byte buff[], int length) {
            int crc = 0;
            int r;
            int count = 0;
            while (count < length) {
                /* compute checksum of lower four bits of *p */
                r = crc_16_table[crc & 0xF];
                crc = (crc >> 4) & 0x0FFF;
                crc = crc ^ r ^ crc_16_table[buff[count] & 0xF];
                /* now compute checksum of upper four bits of *p */
                r = crc_16_table[crc & 0xF];
                crc = (crc >> 4) & 0x0FFF;
                crc = crc ^ r ^ crc_16_table[(buff[count] >> 4) & 0xF];
                /* next... */
                count++;
            }
            
            return crc;
        }
        
        public static void CRCCalcAppendArray(byte buff[], int length) {
            int crc;
            int crcA;
            int crcB;
            
            crc = CRCCalc(buff, length);
            crcA = crc >> 8;
            crcB = crc & 0xFF;
            buff[length] = (byte) crcA;
            buff[length+1] = (byte) crcB;
        }
        
        /**
         * Checks a byte array to see if it matches a CRC
         *
         * @return whether the CRC matches
         * @param buff Byte array to check
         * @param length Length of the byte array
         * @param crc CRC to check
         */
        public static boolean CRCCheck(byte buff[], int length, int crc) {
            if(crc == CRCCalc(buff, length)) {
                return true;
            }
            return false;
        }
        
        public static boolean CRCCheckArray(byte buff[], int length) {
            int crc;
            int crcA;
            int crcB;
            
            
            if (length < 3) {
                return false;
            }
            
            crc = CRCCalc(buff, length-2);
            crcA = crc >>> 8;
            crcB = crc & 0xFF;
            
            if (crcA == Utils.ConvertByteToInt(buff[length-2]) && crcB == Utils.ConvertByteToInt(buff[length-1])) {
                return true;
            }

            return false;
        }
}
