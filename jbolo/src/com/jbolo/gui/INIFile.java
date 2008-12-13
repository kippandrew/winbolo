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
 * INIFile.java
 *
 * Created on 20 June 2004, 17:18
 */

package com.jbolo.gui;

import java.util.Hashtable;
import java.util.Enumeration;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * INIFile represents a ini file. Implements windows style ini files
 * 
 * @author John Morrison
 */
public class INIFile {

	
	/** Comments are started with a # character */
	private static final String COMMENT_MARKER_STRING = "#";
	
	/** Comments are started with a # character */
	private static final char COMMENT_MARKER_CHAR = '#';
	
	/**
	 * Hashtable containing each section. Key is the section name in upper case.
	 * Value is a hashtable of key value pairs as strings
	 */
	private Hashtable<String, Hashtable<String, String>> sections;
	/** Filename to save preferences too */
	private String fileName;
	/** Newline string */
	private String newLine;
	/** Is this INI File read only and can't be modified? */
	private boolean readOnly;

	/**
	 * Creates a new instance of INIFile
	 * 
	 * @param fileName
	 *            Filename to read/write from/to
	 */
	public INIFile(String fileName) {
		this.fileName = fileName;
		readOnly = false;
		sections = new Hashtable<String, Hashtable<String,String>>();
		newLine = System.getProperty("line.separator");
		read();
	}

	/**
	 * Creates a new instance of INIFile
	 * 
	 * @param fileName
	 *            Filename to read
	 * @param readOnly
	 *            Is this a readonly ini file?
	 */
	public INIFile(String fileName, boolean readOnly) {
		this.fileName = fileName;
		this.readOnly = readOnly;
		sections = new Hashtable<String,Hashtable<String,String>>();
		newLine = System.getProperty("line.separator");
		read();
	}

	/**
	 * Creates a new instance of INIFile
	 * 
	 * @param is
	 *            Input Stream to read from
	 */
	public INIFile(InputStream is) {
		readOnly = true;
		sections = new Hashtable<String,Hashtable<String,String>>();
		newLine = System.getProperty("line.separator");
		read(is);
	}

	/**
	 * Gets a preference setting from the global preferences file.
	 * 
	 * @param section
	 *            Preference setion to retrieve from
	 * @param key
	 *            Preference name to get the key for
	 * @param defaultValue
	 *            Default to get if the key is not found.
	 */
	public String getPrivateProfileString(String section, String key,
			String defaultValue) {
		Hashtable<String,String> sectionTable; // Hashtable that is the section containing this
								// key
		String returnValue = defaultValue; // Value to return

		sectionTable = sections.get(section.toUpperCase());
		if (sectionTable != null) {
			returnValue = sectionTable.get(key.toUpperCase());
			if (returnValue == null) {
				returnValue = defaultValue;
			}
		}

		return returnValue;
	}

	/**
	 * Writes a preference to the preference file
	 * 
	 * @param section
	 *            Section to write to (Case insensitive)
	 * @param key
	 *            Key to write to (Case insensitive)
	 * @para value Value to write (null replaced with empty string)
	 */
	public void writePriveteProfileString(String section, String key,
			String value) {
		Hashtable<String,String> sectionTable;

		if (value == null) {
			value = "";
		}

		section = section.toUpperCase();
		key = key.toUpperCase();
		sectionTable = sections.get(section);
		if (sectionTable == null) {
			sectionTable = new Hashtable<String,String>();
			sectionTable.put(key, value);
			sections.put(section, sectionTable);
		} else {
			sectionTable.put(key, value);
		}

		write();
	}

	/**
	 * Attempts to read the ini file from the stored filename.
	 */
	private void read() {
		try {
			read(new FileInputStream(fileName));
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Reads the preferences file into the internal hashtable
	 * 
	 * @param is
	 *            InputStream to read from
	 */
	private void read(InputStream is) {
		BufferedReader br;
		String line;
		String sectionName;

		try {
			br = new BufferedReader(new InputStreamReader(is));
			line = br.readLine();
			while (line != null) {
				
				if (line.startsWith(COMMENT_MARKER_STRING) == false) {
					sectionName = getSectionName(line);
					if (sectionName != null) {
						line = readSection(br, sectionName);
					} else {
						line = br.readLine();
					}
				} else {
					line = br.readLine();
				}
			}
			br.close();
		} catch (FileNotFoundException fnfe) {
			// Its fine for the file not to exist

		} catch (Exception e) {
			// FIXME
			e.printStackTrace();
		}
	}

	/**
	 * Gets a section name. A section name is defined of being inside [ ]
	 * 
	 * @return Section name without brackets or null if not found
	 */
	private String getSectionName(String line) throws Exception {
		String sectionName = null;

		if (line.startsWith("[") == true && line.endsWith("]") == true) {
			sectionName = line.substring(1, line.length() - 1);
		}

		return sectionName;

	}

	/**
	 * Reads in a section from the preferences file
	 * 
	 * @return The next sections complete line or null if EOF is reached
	 * @param br
	 *            BufferedReader to read from
	 * @param sectionName
	 *            section name we are reading
	 * @throw Exception on I/O error
	 */
	private String readSection(BufferedReader br, String sectionName)
			throws Exception {
		Hashtable<String,String> section = new Hashtable<String,String>();
		String nextSection = null;
		String line;
		String key;
		String value;
		int equalsIndex;

		line = br.readLine();
		while (line != null && nextSection == null) {
			if (line.trim().length() > 0) {
				if (line.charAt(0) != COMMENT_MARKER_CHAR) {
					nextSection = getSectionName(line);
					if (nextSection == null) {
						equalsIndex = line.indexOf('=');
						if (equalsIndex > 0) {
							key = line.substring(0, equalsIndex);
							value = line.substring(equalsIndex + 1);
							section.put(key.toUpperCase(), value);
						}
					} else {
						nextSection = line;
					}
				}
			}
			if (nextSection == null) {
				line = br.readLine();
			}
		}

		sections.put(sectionName.toUpperCase(), section);
		return nextSection;
	}

	/**
	 * Writes the hashtable back to disk
	 */
	private void write() {
		BufferedWriter bw;
		Enumeration<String> enums = sections.keys();
		String key;
		Hashtable<String,String> current; // Current section

		if (readOnly == false) {
			try {
				bw = new BufferedWriter(new FileWriter(fileName));
				while (enums.hasMoreElements()) {
					key = enums.nextElement();
					current = sections.get(key);
					writeSection(bw, key, current);
					if (enums.hasMoreElements()) {
						bw.write(newLine);
					}
				}
				bw.close();
			} catch (Exception e) {
				// FIXME
				e.printStackTrace();
			}
		}
	}

	/**
	 * Writes the section to the file
	 * 
	 * @throw Exception on file writing error
	 * @param bw
	 *            BufferedWriter to write to
	 * @param name
	 *            Name of the section to write to
	 * @param section
	 *            Hashtable of the section
	 */
	private void writeSection(BufferedWriter bw, String name, Hashtable<String,String> section)
			throws Exception {
		Enumeration<String> enums = section.keys();
		String key;
		String value;

		bw.write("[" + name + "]" + newLine);
		while (enums.hasMoreElements()) {
			key = enums.nextElement();
			value = section.get(key);
			bw.write(key + "=" + value + newLine);
		}
	}
}
