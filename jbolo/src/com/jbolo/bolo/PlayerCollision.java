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
 * PlayerCollision.java
 *
 * Created on 15 June 2004, 03:34
 */

package com.jbolo.bolo;

/**
 *
 * @author  John Morrison
 */
public class PlayerCollision extends Point {
    
    private boolean collision;
    
    /** Creates a new instance of PlayerCollision */
    public PlayerCollision() {
        super(0, 0);
        collision = false;
    }
    
    public void setCollision(boolean value) {
        collision = value;
    }
    
    public boolean getCollision() {
        return collision;
    }
    
}
