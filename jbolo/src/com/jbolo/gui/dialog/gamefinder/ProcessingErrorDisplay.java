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
 * ProcessingErrorNoDisplay.java
 *
 * Created on 20 May 2004, 21:26
 */

package com.jbolo.gui.dialog.gamefinder;

/**
 * 
 * @author John Morrison
 */
public class ProcessingErrorDisplay extends Exception {

	/**
	 * Serial UID.
	 */
	private static final long serialVersionUID = -7435468895509125347L;

	/** Creates a new instance of ProcessingErrorNoDisplay */
	public ProcessingErrorDisplay() {
		super();
	}

	/**
	 * Creates a new ProcessingErrorNoDisplay Exception
	 * 
	 * @param error
	 *            Error text
	 */
	public ProcessingErrorDisplay(String error) {
		super(error);
	}
}
