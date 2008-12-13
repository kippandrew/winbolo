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
 * Utils.java
 *
 * Created on October 4, 2003, 5:27 PM
 */

package com.jbolo.bolo;
import java.io.File;
import java.io.ByteArrayOutputStream;
import com.jbolo.bolo.map.BoloMap;

/** Generic utilities. Static methods
 * @author John Morrison
 */
public class Utils {
    /** Maximum number of bradians in a circle */
    public static float BRADIANS_MAX = 256;
    /** Maximum number of degrees in a circle */
    public static float DEGREES_MAX = 360;
    /** Maximum number of radians in a circle */
    public static float RADIAN_MAX = (float) (2 * 3.14);
    public static float BRADIANS_NORTH = 0f;
    public static float BRADIANS_EAST = 64f;
    public static float BRADIANS_SOUTH = 128f;
    public static float BRADIANS_WEST = 192f;
    public static float BRADIANS_NNEAST = 16f;
    public static float BRADIANS_NEAST = 32f;
    public static float BRADIANS_NEASTE = 48f;
    public static float BRADIANS_SEASTE = 80f;
    public static float BRADIANS_SEAST = 96f;
    public static float BRADIANS_SSEAST = 112f;
    public static float BRADIANS_SSWEST = 144f;
    public static float BRADIANS_SWEST = 160f;
    public static float BRADIANS_SWESTW = 176f;
    public static float BRADIANS_NWESTW = 208f;
    public static float BRADIANS_NWEST = 224f;
    public static float BRADIANS_NNWEST = 240f;
    /** The gap between north to north north east to north east etc is 16 Bradians */
    public static float BRADIANS_GAP = 16;
    
    /** Worlds are 256 wide. The middle is 128 */
    public static int MAP_SQUARE_MIDDLE = 128;
    
    public static float BRADIAN_ADD8 = 8.0f;
    
    /** Empty string */
    public static String EMPTY_STRING = "";
    
    /** Amount to shift a nibble into a byte */
    private static int NIBBLE_SHIFT_SIZE = 4;
    
    
    
    /** Converts a Java Byte (-127/127) to a int (0-255)
     *
     * * Author's WebSite: http://tactika.com/realhome/realhome.html
     *
     * In JAVA, a byte always considered as signed when converted to another type. We must mask the sign bit to JAVA, cast to an integer and process the masked bit if needed. The following method implements this idea :
     *
     * public class UnsignedByte {
     *   public static void main (String args[]) {
     *     byte b1 = 127  // int 127;
     *     byte b2 = -128 // int 128;
     *     byte b3 = -1   // int 255;
     *
     *     System.out.println(unsignedByteToInt(b1));
     *     System.out.println(unsignedByteToInt(b2));
     *     System.out.println(unsignedByteToInt(b3));
     *     }
     *
     *   public static int unsignedByteToInt(byte b) {
     *     return (int) b & 0xFF;
     *     }
     *    }
     *
     *
     *
     *
     *
     * Therefore for an array of 4 bytes (buf[]), which represents an integer :
     *
     * int i = 0;
     * int pos = 0;
     * i += unsignedByteToInt(buf[pos++]) << 24;
     * i += unsignedByteToInt(buf[pos++]) << 16;
     * i += unsignedByteToInt(buf[pos++]) << 8;
     * i += unsignedByteToInt(buf[pos++]) << 0;
     * @param value byte to convert
     * @return unsigned representation of the byte
     */
    public static int ConvertByteToInt(byte value) {
        return (int) (value & 0xFF);
    }
    
    /** Calculates the distance travelled in X,Y vectors by an object
     * moving at speed and direction angle.
     * @param angle Angle object is travelling at
     * @param speed Speed item is travelling at
     * @return A point object.
     * @see com.jbolo.bolo.Point
     */
    public static Point CalcDistance(float angle, float speed) {
        double dbAngle;
        
        /* Take away 64 BRaidians to make correct sin/cos calculations */
        angle -= Utils.BRADIANS_EAST;
        if (angle < 0) {
            angle += Utils.BRADIANS_MAX;
        }
        /* Convert bradians to degrees */
        dbAngle = (Utils.DEGREES_MAX / Utils.BRADIANS_MAX) * angle;
        /* Convert degrees to radians */
        dbAngle = (dbAngle / Utils.DEGREES_MAX) * Utils.RADIAN_MAX;
        /* Perform calculation */
        return new Point((int) (speed * Math.cos(dbAngle)), (int) (speed * Math.sin(dbAngle)));
    }
    
    /** Converts a two byte Bolo World to a single byte Map by taking the
     * top byte.
     * @param world world to convert
     * @return The map part of the world item
     */
    public static int GetMapFromWorld(int world) {
        world = world & 0xFF00;
        return world >>> 8;
        
    }
    
    /** Converts a two byte Bolo World to a single pixel part by taking the
     * top nibble of the lower byte.
     * @param world world to convert
     * @return The map part of the world item
     */
    public static int GetPixelFromWorld(int world) {
        world = world & 0xF0;
        return world >>> 4;
    }
    
    /** Converts a map and pixel co-ordinate to a bolo world
     * @param map Map position to convert
     * @param pixel Pixel position to convert
     * @return The map part of the world item
     */
    public static int GetWorldFromMapPixel(int map, int pixel) {
        return (map >>> 8) + (pixel >>> 4);
    }
    
    
    /** GetWorldFromMap Converts a single byte map co-ordinate to a 2 byte
     * world co-ordinate
     * @param map Map to convert
     * @return The world position of this map item
     */
    public static int GetWorldFromMap(int map) {
        return map << 8;
    }
    
    /** Converts an angle to a 16 directions
     * @param value Angle of the item
     * @return The 16 direction
     */
    public static int GetDir(float value) {
        int returnValue; /* Value to return */
        
        if (value <= 8) {
            returnValue = 0;
        } else if (value <= 24) {
            returnValue = 1;
        } else if (value <= 40) {
            returnValue = 2;
        } else if (value <= 56) {
            returnValue = 3;
        } else if (value <= 72) {
            returnValue = 4;
        } else if (value <= 88) {
            returnValue = 5;
        } else if (value <= 104) {
            returnValue = 6;
        } else if (value <= 120) {
            returnValue = 7;
        } else if (value <= 136) {
            returnValue = 8;
        } else if (value <= 152) {
            returnValue = 9;
        } else if (value <= 168) {
            returnValue = 10;
        } else if (value <= 184) {
            returnValue = 11;
        } else if (value <= 200) {
            returnValue = 12;
        } else if (value <= 216) {
            returnValue = 13;
        } else if (value <= 232) {
            returnValue = 14;
        } else if (value <= 248) {
            returnValue = 15;
        } else {
            returnValue = 0;
        }
        
        return returnValue;
    }
    
    /**
     * Moves two nibbles into a byte
     * @return int that can be safely translated into a byte to send via the network
     * @param high High nibble
     * @param low Low nibble
     */
    public static int PutNibble(int high, int low) {
        int returnValue;
        
        returnValue = high;
        returnValue <<= NIBBLE_SHIFT_SIZE;
        returnValue += low;
        return returnValue;
    }
    
    /** Gets the high nibble of a byte
     * @return int containing the high nibble
     * @param value byte to get nibble from
     */
    public static int GetHighNibble(byte value) {
        int returnValue;
        
        returnValue = (int) value;
        returnValue >>= NIBBLE_SHIFT_SIZE;
        return returnValue;
    }
    
    /** Gets the low nibble of a byte
     * @return int containing the low nibble
     * @param value byte to get nibble from
     */
    public static int GetLowNibble(byte value) {
        int returnValue;
        
        returnValue = (int) value;
        returnValue <<= NIBBLE_SHIFT_SIZE;
        returnValue >>= NIBBLE_SHIFT_SIZE;
        
        return returnValue;
        
    }
    
    /** Converts BRadians to nearest 16 direction
     * @return The direction as one of 16 options
     * @param value The angle in brandians
     */
    public static int Get16Dir(float value) {
        int returnValue; /* Value to return */
        if (value <= Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_NORTH;
        } else if (value <= Utils.BRADIANS_NNEAST+ Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_NNEAST;
        } else if (value <= Utils.BRADIANS_NEAST + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_NEAST;
        } else if (value <= Utils.BRADIANS_NEASTE + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_NEASTE ;
        } else if (value <= Utils.BRADIANS_EAST + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_EAST;
        } else if (value <= Utils.BRADIANS_SEASTE + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_SEASTE;
        } else if (value <= Utils.BRADIANS_SEAST + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_SEAST;
        } else if (value <= Utils.BRADIANS_SSEAST + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_SSEAST;
        } else if (value <= Utils.BRADIANS_SOUTH + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_SOUTH;
        } else if (value <= Utils.BRADIANS_SSWEST + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_SSWEST;
        } else if (value <= Utils.BRADIANS_SWEST + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_SWEST;
        } else if (value <= Utils.BRADIANS_SWESTW + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_SWESTW;
        } else if (value <= Utils.BRADIANS_WEST + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_WEST;
        } else if (value <= Utils.BRADIANS_NWESTW + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_NWESTW;
        } else if (value <= Utils.BRADIANS_NWEST + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_NWEST;
        } else if (value <= Utils.BRADIANS_NNWEST + Utils.BRADIAN_ADD8) {
            returnValue = (int) Utils.BRADIANS_NNWEST;
        } else {
            returnValue = 0;
        }
        return returnValue;
    }
    
    /**
     * Gets the gap between 2 values ensuring it is positive
     * @param value1 The first value
     * @param value2 The second value
     * @return The gap between the two values
     */
    public static int GetGap(int value1, int value2) {
        if (value1 > value2) {
            return value1 - value2;
        }
        return value2 - value1;
    }
    
    /** Gets whether two items are in a range of diatance
     * @param x1 Item 1's X position
     * @param x2 Item 1's Y position
     * @param y1 Item 2's X position
     * @param y2 Item 2's Y position
     * @param distance The distance to check if the items are under
     * @return If the two items are in range
     */
    public static boolean IsItemInRange(int x1, int y1, int x2, int y2, double distance) {
        if (GetItemRange(x1,y1,x2,y2) <= distance) {
            return true;
        }
        return false;
    }
    
    /** Gets the range of one item to another
     * @return Items distance from each other
     * @param x1 Item 1's X position
     * @param x2 Item 1's Y position
     * @param y1 Item 2's X position
     * @param y2 Item 2's Y position
     */
    public static double GetItemRange(int x1, int y1, int x2, int y2) {
        int gapX = GetGap(x1, x2);
        int gapY = GetGap(y1, y2);
        double distance;
        
        distance = gapX * gapX + gapY * gapY;
        
        return Math.sqrt(distance);
    }
    
    /**
     * Calculates the angle from object 1 to object 2
     * @return the angle from object 1 to object 2
     * @param object1X X location of object1
     * @param object1Y Y location of object2
     * @param object2X X location of object1
     * @param object2Y Y location of object2
     */
    public static float CalcAngle(int object1X, int object1Y, int object2X, int object2Y) {
        float returnValue;
        double angle;
        double gapX = GetGap(object1X, object2X);
        double gapY = GetGap(object1Y, object2Y);
        
        angle = Math.atan(gapX/gapY);
        angle = (angle / Utils.RADIAN_MAX) * Utils.DEGREES_MAX;
        
        returnValue = (float) ((Utils.BRADIANS_MAX / Utils.DEGREES_MAX) * angle);
        if (object2X - object1X <= 0 && object2Y - object1Y <= 0) {
            returnValue = (float) (Utils.BRADIANS_MAX - returnValue);
        } else if (object2X - object1X > 0 && object2Y - object1Y >= 0) {
            returnValue = (float) (Utils.BRADIANS_SOUTH - returnValue);
        } else if (object2X - object1X <= 0 && object2Y - object1Y >= 0) {
            returnValue += Utils.BRADIANS_SOUTH;
        }
        
        return returnValue;
    }
    
    /** Returns whether a tank is hidden (ie surrounded) by trees
     * @param map The BoloMap that is in this game world
     * @param pbs The Pillboxes that are in this game world
     * @param bss The Bases that are in this game world
     * @param wx World X position of the tank to check
     * @param wy World Y position of the tank to check
     * @return If the tank is hidden in trees
     */
    public static boolean IsTankInTrees(BoloMap map, Pillboxes pbs, Bases bss, int wx, int wy) {
        boolean returnValue = true; // Value to return
        int terrain;
        int terrain2;
        int bmx = GetMapFromWorld(wx);
        int bmy = GetMapFromWorld(wy);
        
        wx = wx >> 8;
        wy = wy >> 8;
        
        terrain = Terrain.TerrainWithoutMine(map.getPos(bmx, bmy));
        if (terrain != Terrain.FOREST) {
            returnValue = false;
        } else if (pbs.existPos(bmx, bmy) == true || bss.existPos(bmx, bmy) == true) {
            returnValue = false;
        }
        
        if (returnValue == true) {
            if (wx >= MAP_SQUARE_MIDDLE) {
                /* wx > 128 */
                terrain = Terrain.TerrainWithoutMine(map.getPos(bmx+1, bmy));
                if (terrain != Terrain.FOREST || pbs.existPos(bmx+1, bmy) == true || bss.existPos(bmx+1, bmy) == true) {
                    returnValue = false;
                }
                if (returnValue == true) {
                    if (wy > MAP_SQUARE_MIDDLE) {
                        /* wx > 128 && wy > 128 */
                        terrain = Terrain.TerrainWithoutMine(map.getPos(bmx+1, bmy+1));
                        terrain2 = Terrain.TerrainWithoutMine(map.getPos(bmx, bmy+1));
                        if (pbs.existPos(bmx+1, bmy+1) == true || bss.existPos(bmx+1, bmy+1) == true) {
                            returnValue = false;
                        }
                        if (pbs.existPos(bmx, bmy+1) == true || bss.existPos(bmx, bmy+1) == true) {
                            returnValue = false;
                        }
                    } else {
                        /* wx > 128 && wy < 128 */
                        terrain = Terrain.TerrainWithoutMine(map.getPos(bmx+1, bmy-1));
                        terrain2 = Terrain.TerrainWithoutMine(map.getPos(bmx, bmy-1));
                    }
                    if (returnValue == true && terrain != Terrain.FOREST && terrain2 != Terrain.FOREST) {
                        returnValue = false;
                    }
                }
            } else {
                /* wx < 128 */
                terrain = Terrain.TerrainWithoutMine(map.getPos((bmx-1), bmy));
                if (terrain != Terrain.FOREST) {
                    returnValue = false;
                }
                
                if (returnValue == true) {
                    if (wy >= MAP_SQUARE_MIDDLE) {
                        /* wx < 128 && wy > 128 */
                        terrain2 = Terrain.TerrainWithoutMine(map.getPos(bmx, (bmy+1)));
                        terrain = Terrain.TerrainWithoutMine(map.getPos((bmx-1), (bmy+1)));
                    } else {
                        /* wx < 128 && wy < 128 */
                        terrain2 = Terrain.TerrainWithoutMine(map.getPos(bmx, (bmy-1)));
                        terrain = Terrain.TerrainWithoutMine(map.getPos((bmx-1), (bmy-1)));
                    }
                    if (terrain != Terrain.FOREST  || terrain2 != Terrain.FOREST) {
                        returnValue = false;
                    }
                }
            }
        }
        return returnValue;
        
        
    }
    /**
     * Returns whether the tank has been hit or not.
     * @return Whether the tank has been hit or not.
     * @param xTank X Co-ordinate of the tank
     * @param yTank Y Co-ordinate of the tank
     * @param tankAngle Angle of the tank
     * @param x X co-ord of shell
     * @param y Y co-ord of shell
     * @param angle The direction the shell came from
     */
    public static boolean IsTankHit(int xTank, int yTank, float tankAngle, int x, int y, float angle) {
        boolean returnValue = false; // Value to return
        int top;
        int bottom;
        int leftPos;
        int rightPos;
        
        bottom = yTank - Tank.TANK_SUBTRACT_SHELL_HIT_Y+72;
        top = yTank + Tank.TANK_SUBTRACT_SHELL_HIT_Y-8;
        leftPos = Tank.TANK_SUBTRACT_SHELL_HIT_X2 - 80;
        rightPos = Tank.TANK_SUBTRACT_SHELL_HIT_X1+8;
        leftPos = xTank - leftPos;
        rightPos = xTank + rightPos;
        
        if (x >= leftPos && x <= rightPos && y <= top && y >= bottom) {
            returnValue = true;
        }
        
        return returnValue;
    }
    
    /**
     * Gets the map name from a filename and path.
     * @param fileName The map name to get
     * @return the map name without the .map extension if present
     */
    public static String GetMapNameFromFileName(String fileName) {
        String returnValue;
        File file = new File(fileName);
        
        returnValue = file.getName();
        if (returnValue.toLowerCase().endsWith(".map") == true) {
            returnValue = returnValue.substring(0, returnValue.length()-4);
        }
        return returnValue;
    }
    
    
    /*
     * Returns whether a tank is hidden (ie surrounded) by trees
     *
     * @return boolean indicating if the tank is hidden in trees
     * @param gw Gameworld to search in
     * @param wx Tank X world position
     * @param wy Tank Y world position
     */
    public static boolean IsTankInTrees(GameWorld gw, int wx, int wy) {
        return Utils.IsTankInTrees(gw.getBoloMap(), gw.getPills(), gw.getBases(), wx, wy);
    }
    
    /**
     * Converts a boolean into a single byte
     *
     * @return byte (1 == true, 0 == false)
     * @param boolean to convert
     */
    public static byte BooleanToByte(boolean value) {
        if (value == true) {
            return 1;
        }
        return 0;
    }
    
    
    /**
     * Converts a single byte value to a boolean
     *
     * @return boolean (0 == false, anything else = true)
     * @param value Value to convert
     */
    public static boolean ByteToBoolean(byte value) {
        if (value == 0) {
            return false;
        }
        return true;
    }
    
    /**
     * Converts a char array into a byte array by typecasting each char to a byte
     *
     * @return byte array
     * @param value char array to convert
     */
    public static byte[] ConvertCharArrayToByteArray(char value[]) {
        byte returnValue[] = new byte[value.length];
        int count = 0;
        while (count < value.length) {
            returnValue[count] = (byte) value[count];
            count++;
        }
        return returnValue;
    }
    
    /**
     * Converts a String into a Byte Array in Pascal format. Pascat format is
     * the length of the string followed by the string itself
     *
     * @return pascal string of length 1+stringlength
     * @param value String to convert. Strings are truncated to 255 letters if larger
     */
    public static byte[] StringToPByteArray(String value) {
        byte returnValue[];
        ByteArrayOutputStream bos = new ByteArrayOutputStream(value.length()+1);
        if (value.length() > 255) {
            value = value.substring(0, 254);
        }
        try {
            bos.write(value.length());
            bos.write(ConvertCharArrayToByteArray(value.toCharArray()));
            returnValue = bos.toByteArray();
        } catch (Exception e) {
            returnValue = new byte[1];
            returnValue[0] = 0;
        }
        return returnValue;
    }
    
    /**
     * Converts a String into a Byte Array in Pascal format. Pascat format is
     * the length of the string followed by the string itself
     *
     * @return pascal string of length 1+stringlength
     * @param value String to convert. Strings are truncated to 255 letters if larger
     */
    public static void StringToPByteArrayExisting(String value, byte data[], int offset) {
        int count = 0;
        char c[];
        
        if (value.length() > 255) {
            value = value.substring(0, 254);
        }
        try {
            data[offset] = (byte) value.length();
            c = value.toCharArray();
            while (count < value.length()) {
                data[offset+count+1] = (byte) c[count];
                count++;
            }
        } catch (Exception e) {
            data[offset] = 0;
        }
    }
    
    
    /**
     * Compares two byte arrays
     *
     * @return if the arrays are identical
     * @param testA Array A
     * @param testB Array B
     * @param length of the array to test
     */
    public static boolean CompareArrays(byte testA[], byte testB[], int length) {
        return CompareArrays(testA, 0, testB, 0, length);
    }
    
    
    /**
     * Compares two byte arrays
     *
     * @return if the arrays are identical
     * @param testA Array A
     * @param testAOffset Offset of the array
     * @param testB Array B
     * @param testBOffset Offset of the array
     * @param length of the array to test
     */
    public static boolean CompareArrays(byte testA[], int testAOffset, byte testB[], int testBOffset, int length) {
        int count = 0;
        try {
            while (count < length) {
                if (testA[count+testAOffset] != testB[count+testBOffset]) {
                    return false;
                }
                count++;
            }
        } catch (Exception e) {
            // Could get an index out of bounds exception
            e.printStackTrace();
            return false;
        }
        return true;
    }
    
    /**
     * Converts a byte array containing a pascal string into a java String
     *
     * @return Converted string
     * @param value Byte array to convert
     */
    public static String PByteArrayToString(byte value[]) {
        return PByteArrayToString(value, 0);
    }
    
    /**
     * Converts a byte array containing a pascal string into a java String
     *
     * @return Converted string
     * @param value Byte array to convert
     */
    public static String PByteArrayToString(byte value[], int offset) {
        return new String(value, 1+offset, ConvertByteToInt(value[0+offset]));
    }
    
    
    /**
     * Converts a boolean value to the string Yes or No
     *
     * @return String
     * @param value Value to convert
     */
    public static String ConvertBoolToYesNoTo(boolean value) {
        
        if (value == true) {
            return "Yes";
        }
        return "No";
    }
    
    /**
     * Converts the case insensitive string yes/no to a boolean value
     *
     * @throw Exception if string is not yes or no
     * @return boolean
     * @param value Value to convert
     */
    public static boolean ConvertYesNoToBool(String value) throws Exception {
        value = value.toLowerCase();
        
        if (value.equals("yes")) {
            return true;
        } else if (value.equals("no")) {
            return false;
        }
        throw new Exception("Invalid conversion");
    }
    
    /**
     * Copies byte array memory from src to dest
     *
     * @param src Source byte array to copy from
     * @param dest Dest byte array to copy to
     * @param length Length of data to copy
     */
    public static void MemCopy(byte src[], byte dest[], int length) {
        MemCopy(src, dest, length, 0, 0);
    }
    
    /**
     * Copies byte array memory from src to dest with src offset
     *
     * @param src Source byte array to copy from
     * @param dest Dest byte array to copy to
     * @param length Length of data to copy
     * @param srcOffset Source array offset
     */
    public static void MemCopy(byte src[], byte dest[], int length, int srcOffset) {
        MemCopy(src, dest, length, srcOffset, 0);
    }
    
    /**
     * Copies byte array memory from src to dest with src and dest offsets
     *
     * @param src Source byte array to copy from
     * @param dest Dest byte array to copy to
     * @param length Length of data to copy
     * @param srcOffset Source array offset
     */
    public static void MemCopy(byte src[], byte dest[], int length, int srcOffset, int destOffset) {
        int count = 0;
        while (count < length) {
            dest[count+destOffset] = src[count+srcOffset];
            count++;
        }
    }

    public static void MemSet(byte data[], byte value) {
        MemSet(data, value, data.length, 0);
    }
    
    
    public static void MemSet(byte data[], byte value, int length) {
        MemSet(data, value, length, 0);
    }
    
    public static void MemSet(byte data[], byte value, int length, int offset) {
        int count = 0;
        while (count < length)  {
            data[count+offset] = value;
            count++;
        }
    }
    
    /**
     * Adds a short into an array in network byte order
     *
     * @param data Array to write short into
     * @param value Short to write
     * @param offset Offset into array to write
     */
    public static void AddShortToArray(byte data[], int value, int offset) {
        data[offset] = (byte) (value >>> 8);
        data[offset+1] = (byte) (value & 0xFF);;
    }
    
    /**
     * Gets a short out of an array when it is stored in network byte order
     *
     * @param data Byte array to get short from
     * @param offset Offset to get short from
     */
    public static int GetShortFromArray(byte data[], int offset) {
        //FIXME endian
        int returnValue = ConvertByteToInt(data[offset+1]);
        returnValue = returnValue << 8;
        returnValue += ConvertByteToInt(data[offset]);
        return returnValue;
    }
    
    public static byte[] ArraySubstring(byte data[], int length, int offset) {
        byte rv[] = null;
        int count = 0;

        try {
            rv = new byte[length];
            while (count < length) {
                rv[count] = data[count+offset];
                count++;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return rv;
        
    }
}
