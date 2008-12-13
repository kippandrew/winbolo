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
 * NumberTextFieldListener.java
 *
 * Created on 22 May 2004, 14:16
 */

package com.jbolo.gui.dialog;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

/**
 *
 * @author  John Morrison
 */
public class NumberTextFieldListener implements KeyListener {
    
    
    /** Creates a new instance of NumberTextFieldListener */
    public NumberTextFieldListener() {
        super();
    }
    
    private void process(KeyEvent keyEvent) {
        int code = keyEvent.getKeyCode();
        char val = keyEvent.getKeyChar();
        
        if (val < '0' || val > '9') {
            if (code != KeyEvent.VK_BACK_SPACE && code != KeyEvent.VK_DELETE && code != KeyEvent.VK_LEFT && code != KeyEvent.VK_RIGHT) {
                keyEvent.consume();
            }
        }
    }
    
    /**
     * keyPressed function
     * @param key KeyEvent
     */
    public void keyPressed(java.awt.event.KeyEvent key) {
        process(key);
    }
    
    public void keyReleased(java.awt.event.KeyEvent keyEvent) {
    }
    
    public void keyTyped(java.awt.event.KeyEvent keyEvent) {
    }
    
}
