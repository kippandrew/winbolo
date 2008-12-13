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
 * Preferences.java
 *
 * Created on 16 May 2004, 20:27
 */

package com.jbolo.gui;

/**
 * Bolo Preferences
 * @author  John Morrison
 */
public class BoloPreferences extends INIFile {

    /** Default name of the preferences file */
    private static String DEFAULT_FILE = "c:/jbolo/jbolo.ini";
    
    /** Preferences object */
    private static BoloPreferences _me = null;
        
    /** Creates a new instance of Preferences 
     *
     * @param fileName fileName to read preferences from 
     */
    protected BoloPreferences(String fileName) {
        super(fileName);
    }
    
    /**
     * Gets the global preferences object
     * @return Global BoloPreferences
     */
    public static BoloPreferences GetInstance() {
        if (_me == null) {
            _me = new BoloPreferences(DEFAULT_FILE);
        }
        return _me;
    }
}
