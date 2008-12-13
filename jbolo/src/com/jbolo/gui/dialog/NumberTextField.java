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
 * NumberTextField.java
 *
 * Created on January 11, 2004, 11:10 PM
 */

package com.jbolo.gui.dialog;

import java.awt.TextField;

/**
 * Extends a java.awt.TextField to make it accept numbers only
 * @author  John Morrison
 */
public class NumberTextField extends TextField {
    
    /** Creates a new instance of NumberTextField */
    public NumberTextField() {
	super();
	addKeyListener(new NumberTextFieldListener());
    }
    
    public NumberTextField(int columns) {
	super(columns);
	addKeyListener(new NumberTextFieldListener());
    }
    
    public NumberTextField(String text) {
	super(text);
	addKeyListener(new NumberTextFieldListener());
    }
    
    public NumberTextField(String text, int columns) {
	super(text, columns);
	addKeyListener(new NumberTextFieldListener());
    }
    
    
}
