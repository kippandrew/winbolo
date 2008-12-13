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
 * CentredDialog.java
 *
 * Created on January 4, 2004, 3:59 PM
 */

package com.jbolo.gui.dialog;

import java.awt.*;

/**
 * CentredDialog extends a awt Dialog and centres it on the screen
 * 
 * @author John Morrison
 */
public class CentredDialog extends Dialog {

	/**
	 * Creates a new instance of CentredDialog
	 * 
	 * @param owner
	 *            The owner frame
	 * @param name
	 *            The name of the dialog
	 * @param modal
	 *            Is this dialog modal or not
	 */
	public CentredDialog(Frame owner, String name, boolean modal) {
		super(owner, name, modal);
	}

	public CentredDialog(Dialog owner, String name, boolean modal) {
		super(owner, name, modal);
	}

	/**
	 * Centres thi dialog on the screen
	 */
	public void centre() {
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
		setLocation(d.width / 2 - getWidth() / 2, d.height / 2 - getHeight()
				/ 2);
	}
}
