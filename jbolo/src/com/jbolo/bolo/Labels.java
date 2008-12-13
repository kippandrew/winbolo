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
 * Labels.java
 *
 * Created on 7 October 2003, 20:58
 */

package com.jbolo.bolo;

/**
 * Label Settings for the client.
 * 
 * @provides bolo/labels.c
 * @author John Morrison
 */
public class Labels {

	/** Display no labels. */
	public static final int LABEL_NONE = 0;
	public static final int LABEL_SHORT = 1;
	/** Display <player name @ host > for label. */
	public static final int LABEL_LONG = 2;
	/** At symbol (@) */
	private static String AT_SYMBOL = "@";

	/** Are we to label our own tank. */
	private boolean labelOwnTank;

	/** What label type to use for messages. */
	private int messages;

	/** What label type to use for tank names on screen. */
	private int tankLabels;

	/** Creates a new instance of Labels */
	public Labels() {
		labelOwnTank = false;
		tankLabels = LABEL_SHORT;
		messages = LABEL_SHORT;

	}

	/**
	 * Set the Label Length for messages.
	 * 
	 * @param length
	 *            Length to set.
	 */
	public void setSenderLength(int length) {
		this.messages = length;

	}

	/**
	 * Gets the Label Length for messages.
	 * 
	 * @return The label length
	 */
	public int getSenderLength() {
		return messages;
	}

	/**
	 * Set the Label Length for tank drawing.
	 * 
	 * @param length
	 *            Length to set to.
	 */
	public void setTankLength(int length) {
		this.tankLabels = length;
	}

	/**
	 * Gets the Label Length for tank drawing.
	 * 
	 * @return The label length.
	 */
	public int getTankLength() {
		return tankLabels;
	}

	/**
	 * Sets whether we should label our own tank.
	 * 
	 * @param label
	 *            boolean
	 */
	public void setLabelOwnTank(boolean label) {
		this.labelOwnTank = label;
	}

	/** Gets whether we should label our own tank. */
	public boolean getLabelOwnTank() {
		return labelOwnTank;
	}

	/**
	 * Constructs a new label for messages based on the label settings.
	 * 
	 * @param name
	 *            Player name of the label to create
	 * @param location
	 *            Player location of the label to create
	 * @return The generated label.
	 */
	public String makeMessageLabel(java.lang.String name,
			java.lang.String location) {
		switch (messages) {
		case LABEL_SHORT:
			return name;
		case LABEL_LONG:
			return name + AT_SYMBOL + location;
		}
		return Utils.EMPTY_STRING;
	}

	/**
	 * Constructs a new label for tank drawing based on the label settings.
	 * 
	 * @param name
	 *            Player name of the label to create
	 * @param location
	 *            Player location of the label to create
	 * @param isOwnTank
	 *            true if this label is for our own tank
	 * @return The generated label.
	 */
	String makeTankLabel(java.lang.String name, java.lang.String location,
			boolean isOwnTank) {
		if (this.tankLabels != LABEL_NONE && (isOwnTank == false || (isOwnTank == true && labelOwnTank == true))) {
			switch (tankLabels) {
			case LABEL_SHORT:
				return name;
			case LABEL_LONG:
				return name + AT_SYMBOL + location;
			}

		}
		return Utils.EMPTY_STRING;
	}

}
