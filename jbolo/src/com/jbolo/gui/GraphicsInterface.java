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
 * GraphicsInterface.java
 *
 * Created on December 16, 2003, 11:12 PM
 */

package com.jbolo.gui;

import java.awt.Graphics2D;

/**
 *
 * @author John Morrison
 */
public interface GraphicsInterface {
    
    /** 
     * Gets the graphics instanace for this object
     * @return Graphics2D
     */
    public Graphics2D getGraphicsObject();
    
    
}
