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
 * Skin.java
 *
 * Created on 20 June 2004, 17:28
 */

package com.jbolo.gui;

import java.util.zip.*;
import java.io.InputStream;


/**
 *
 * @author  John Morrison
 */
public class Skin {
    
    /** Name of the skins directory */
    private String SKIN_DIR_STRING = "skins";
    
    /** Is this skin in use */
    private boolean inUse;
    
    /** Filename of the current skin */
    private String fileName;
    
    /** Loaded skin zip file */
    private ZipFile zf;
    
    
    /** Instance of the skin object in this game */
    protected static Skin _me = null;
    
    /** Creates a new instance of Skin */
    public Skin() {
        zf = null;
        inUse = false;
    }
    
    /**
     * Gets the instance of the skin item
     */
    public static Skin GetInstance() {
        if (_me == null) {
            _me = new Skin();
        }
        return _me;
    }
    
    /**
     * Gets the file name of the skin in use
     * @return String
     */
    public String getFileName() {
        return fileName;
    }
    
    /**
     * Gets the Skin bitmap from the zip file
     *
     * @return InputStream containing the bitmap file or null if not open
     */
    public InputStream getSkin() {
        try {
            return zf.getInputStream(zf.getEntry("skin.bmp"));
        } catch (Exception e) {
        }
        
        return null;
    }
    
    /**
     * Gets the screen bitmap from the zip file
     *
     * @return InputStream containing the bitmap file or null if not open
     */
    public InputStream getBackground() {
        try {
            return zf.getInputStream(zf.getEntry("background.bmp"));
        } catch (Exception e) {
        }
        
        return null;
    }

    /**
     * Gets the INI file from inside the skin zip file
     *
     * @return INIFile from the compressed file or null if not present or error
     */
    public INIFile getIniFile() {
        try {
            return new INIFile(zf.getInputStream(zf.getEntry("skin.ini")));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    /**
     * Returns if this skin object is in Use
     * @return boolean
     */
    public boolean isInUse() {
        return inUse;
    }
    
    public boolean loadSkin(String fileName) {
        try {
            inUse = true;
            this.fileName = fileName;
            zf = new ZipFile(fileName);
        } catch (Exception e) {
            inUse = false;
            return false;
        }
        return true;
    }
    
    /**
     * Unloads the current skin if one is in use
     */
    public void unload() {
        if (inUse == true) {
            try {
                zf.close();
            } catch (Exception e) {
                // Can ignore this
            }
        }
        inUse = false;
    }
    
    public boolean test() {
        ZipFile zf;
        INIFile ini;
        try {
            zf = new ZipFile("c:/jbolo/skin.zip");
            ini = new INIFile(zf.getInputStream(zf.getEntry("skin.ini")));
            zf.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        return true;
    }
}
