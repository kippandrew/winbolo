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
 * Log.java
 *
 * Created on December 2, 2003, 9:31 PM
 */

package com.jbolo.bolo;

import java.io.*;
import com.jbolo.bolo.map.BoloMap;

/**
 * Responsable for creating WinBolo log files.
 * NOTE: Not compatible with current current log file format... Was a draft protocol 
 * 
 * @author John Morrison
 */
public class Log {

	/** Log type: Player Joined game */
	public static final byte log_PlayerJoined = 0;
	/** Log type: Player quit game */
	public static final byte log_PlayerQuit = 1;
	/** Log type: Player location */
	public static final byte log_PlayerLocation = 2;
	/** Log type: LGM location */
	public static final byte log_LgmLocation = 3;
	/** Log type: Map change */
	public static final byte log_MapChange = 4;
	/** Log type: Shell information */
	public static final byte log_Shell = 5;
	/** Log type: Sound of building */
	public static final byte log_SoundBuild = 6;
	/** Log type: Sound of farming */
	public static final byte log_SoundFarm = 7;
	/** Log type: Sound of shooting */
	public static final byte log_SoundShoot = 8;
	/** Log type: Sound of a tank being hit */
	public static final byte log_SoundHitTank = 9;
	/** Log type: Sound of a tree being hit */
	public static final byte log_SoundHitTree = 10;
	/** Log type: Sound of a wall being hit */
	public static final byte log_SoundHitWall = 11;
	/** Log type: Sound of a mine being layed */
	public static final byte log_SoundMineLay = 12;
	/** Log type: Sound of a mine exploding */
	public static final byte log_SoundMineExplode = 13;
	/** Log type: Sound of a explosion */
	public static final byte log_SoundExplosion = 14;
	/** Log type: Sound of a big explosion */
	public static final byte log_SoundBigExplosion = 15;
	/** Log type: Sound of a lgm dying */
	public static final byte log_SoundManDie = 16;
	/** Log type: Message from the the server */
	public static final byte log_MessageServer = 17;
	/** Log type: Message to all */
	public static final byte log_MessageAll = 18;
	/** Log type: Message to certain player */
	public static final byte log_MessagePlayers = 19;
	/** Log type: Change player name */
	public static final byte log_ChangeName = 20;
	/** Log type: Ally request */
	public static final byte log_AllyRequest = 21;
	/** Log type: Ally accept */
	public static final byte log_AllyAccept = 22;
	/** Log type: Ally leave */
	public static final byte log_AllyLeave = 23;
	/** Log type: Base change owner */
	public static final byte log_BaseSetOwner = 24;
	/** Log type: Base set stock */
	public static final byte log_BaseSetStock = 25;
	/** Log type: Pillbox set owner */
	public static final byte log_PillSetOwner = 26;
	/** Log type: Pillbox set health */
	public static final byte log_PillSetHealth = 27;
	/** Log type: Pillbox set place */
	public static final byte log_PillSetPlace = 28;
	/** Log type: Pillbox set in tank */
	public static final byte log_PillSetInTank = 29;
	/** Log type: Map saved by player */
	public static final byte log_SaveMap = 30;
	/** Log type: LGM lost by player */
	public static final byte log_LostMan = 31;

	/** An empty string */
	public static final String EMPTY_STRING = "";

	/** Log file header name */
	private static String LOG_HEADER = "WBOLOMOV";
	/** Log file version header */
	private static byte LOG_VERSION = 0;

	/** Log items for a log quit */
	private static byte LOG_QUIT = 0;
	/** Log items for a log no event occurred */
	private static byte LOG_NOEVENTS = 1;
	/** Log items for a log no events for a long time */
	private static byte LOG_NOEVENTS_LONG = 2;
	/** Log items for logging an event */
	private static byte LOG_EVENT = 3;
	/** Log items for logging repeated series of events */
	private static byte LOG_EVENT_LONG = 4;

	/** The difference between big log and items */
	private static int LOG_SIZE_LONG_DIFF = 256;

	/** Last event logged. Increments each time there are no events */
	private short lastEvent;
	/** Last event logged. Increments each time there are no events */
	private short numEvents;
	/** Are we saving a log */
	private boolean isRunning;
	/** Log events are stored in memory so the file isn't written to all the time */
	private ByteArrayOutputStream mem;
	/** How much memory are we using */
	private int memSize;
	/** The file we are writing to */
	FileOutputStream logStream;

	/** Creates a new instance of Log */
	public Log() {
		isRunning = false;
		memSize = 0;
		numEvents = 0;
		lastEvent = 0;
	}

	/**
	 * Writes the nothing happened for X ticks to the log file
	 */
	private void writeEmpty() {
		try {
			if (isRunning == true) {
				if (lastEvent > 0 && lastEvent < LOG_SIZE_LONG_DIFF) {
					logStream.write(LOG_NOEVENTS);
					logStream.write(lastEvent);
				} else if (lastEvent > 0) {
					logStream.write(LOG_NOEVENTS_LONG);
					writeShortToFile(lastEvent);
				}
				lastEvent = 0;
			}
		} catch (Exception e) {
			System.err.println("Error writing log");
		}
	}

	/**
	 * Writes a two byte short to the log file taking care of endianess
	 * 
	 * @throws IOExceptions
	 *             on file write error
	 * @param value
	 *            Short to write
	 */
	private void writeShortToFile(short value) throws IOException {
		// FIXME Endian!
		logStream.write(value);
		logStream.write(value);
	}

	/**
	 * Called every tick. Logs stuff if required
	 */
	public void writeTick() {
		try {
			if (isRunning == true) {
				if (numEvents > 0) {
					writeEmpty();
					if (numEvents < LOG_SIZE_LONG_DIFF) {
						logStream.write(LOG_EVENT);
						logStream.write((byte) numEvents);
					} else {
						logStream.write(LOG_EVENT_LONG);
						writeShortToFile(numEvents);
					}
					logStream.write(mem.toByteArray());
					mem.reset();
					numEvents = 0;
				} else {
					lastEvent++;
					if (lastEvent == 63000) {
						writeEmpty();
					}
				}
			}
		} catch (Exception e) {
			System.err.println("Error writing log file");
		}
	}

	/**
	 * Stops logging if we are
	 */
	public void stop() {
		if (isRunning == true) {
			writeEmpty();
			try {
				logStream.write(LOG_QUIT);
				logStream.write(LOG_QUIT);
				logStream.close();
			} catch (Exception e) {
			}
			numEvents = 0;
		}
		isRunning = false;
	}

	/**
	 * Gets if we are currently logging
	 * 
	 * @return boolean indicating if we are logging
	 */
	public boolean isRecording() {
		return isRunning;
	}

	/**
	 * Adds a single byte to out internal byte array "add." This is done as the
	 * OutputStream classes work off byte arrays and its simplier to put it in
	 * an internal array first
	 * 
	 * @param value
	 *            byte to add
	 */
	private void add(byte value) {
		mem.write(value);
	}

	/**
	 * Adds a String to our internal byte array. Converts to a pascal string and
	 * puts the length of the string first
	 * 
	 * @param value
	 *            String to add
	 */
	private void add(String value) {
		byte strBytes[] = value.getBytes();
		int count = 0;

		mem.write((byte) strBytes.length);
		while (count < strBytes.length) {
			add(strBytes[count]);
			count++;
		}
	}

	/**
	 * Adds a short to our internal byte array.
	 * 
	 * @param value
	 *            short to add
	 */
	private void add(short value) {
		// FIXME!!! Endianisms
		mem.write(value);
		mem.write(value);
	}

	/**
	 * Adds a event to be logged
	 * 
	 * @param itemNum -
	 *            Item number to add
	 * @param opt1
	 *            Option argument 1
	 * @param opt2
	 *            Option argument 2
	 * @param opt3
	 *            Option argument 3
	 * @param opt4
	 *            Option argument 4
	 * @param short1
	 *            short optional argument
	 * @param words
	 *            String optional argument
	 */
	public void addEvent(byte itemNum, byte opt1, byte opt2, byte opt3,
			byte opt4, short short1, String words) {
		if (isRunning == true) {
			switch (itemNum) {
			case log_BaseSetOwner:
				add(log_BaseSetOwner);
				add(opt1);
				add(opt2);
				break;
			case log_BaseSetStock:
				add(log_BaseSetStock);
				add(opt1);
				add(opt2);
				add(opt3);
				add(opt4);
			case log_PlayerJoined:
				add(log_PlayerJoined);
				add(opt1);
				add(opt2);
				add(opt3);
				add(opt4);
				add(short1);
				add(words);
			case log_PlayerQuit:
				add(log_PlayerQuit);
				add(opt1);
				break;
			case log_LostMan:
				add(log_LostMan);
				add(opt1);
				break;
			case log_MapChange:
				add(log_MapChange);
				add(opt1);
				add(opt2);
				add(opt3);
				break;
			case log_ChangeName:
				add(log_ChangeName);
				add(opt1);
				add(words);
				break;
			case log_AllyRequest:
				add(log_AllyRequest);
				add(opt1);
				add(opt2);
				break;
			case log_AllyAccept:
				add(log_AllyAccept);
				add(opt1);
				add(opt2);
				break;
			case log_AllyLeave:
				add(log_AllyLeave);
				add(opt1);
				break;
			case log_PillSetOwner:
				add(log_PillSetOwner);
				add(opt1);
				add(opt2);
				break;
			case log_PillSetPlace:
				add(log_PillSetPlace);
				add(opt1);
				add(opt2);
				add(opt3);
				break;
			case log_PillSetInTank:
			case log_PillSetHealth:
				add(itemNum);
				add(opt1);
				break;
			case log_SoundBuild:
			case log_SoundFarm:
			case log_SoundShoot:
			case log_SoundHitWall:
			case log_SoundHitTank:
			case log_SoundHitTree:
			case log_SoundMineLay:
			case log_SoundMineExplode:
			case log_SoundExplosion:
			case log_SoundBigExplosion:
			case log_SoundManDie:
				add(itemNum);
				add(opt1);
				add(opt2);
				break;
			case log_PlayerLocation:
				add(log_PlayerLocation);
				add(opt1);
				add(opt2);
				add(opt3);
				add(opt4);
				add(short1);
				break;
			case log_Shell:
			case log_LgmLocation:
				add(itemNum);
				add(opt1);
				add(opt2);
				add(opt3);
				add(opt4);
				break;
			case log_MessageAll:
				add(log_MessageAll);
				add(opt1);
				add(words);
				break;
			case log_MessageServer:
				add(log_MessageServer);
				add(words);
				break;
			default:
				// FIXME: Error case
				numEvents--;
				break;

			}
			numEvents++;
		}
	}

	/**
	 * Overrides java.lang.object finalize to cleanup logging if we are still
	 * logging and we are garbage collected. Throws no exception
	 */
	protected void finalize() throws Throwable {
		stop();
	}

	/**
	 * Starts logging
	 * 
	 * @return Logging success
	 * @param fileName
	 *            File to write log to
	 * @param map
	 *            Map for thie game
	 * @param bas
	 *            Bases for this game
	 * @param pbs
	 *            Pillboxes for this game
	 * @param ss
	 *            Starts for this game
	 */
	public boolean start(BoloMap map, Bases bss, Pillboxes pbs, Starts ss) {
		boolean returnValue = true; // Value to return

		stop(); // Stops old log if it is running

		try {
			mem = new ByteArrayOutputStream(64 * 1024);
			// FIXME open file
			numEvents = 0;
			add(LOG_HEADER);
			add(LOG_VERSION);
			add(BoloMap.MAP_VERSION);
			add((byte) pbs.getNumItems());
			add((byte) bss.getNumItems());
			add((byte) ss.getNumItems());
			logStream.write(mem.toByteArray());
			mem.reset();
			/* Write pill, base and start locations */
			returnValue = map.writePills((OutputStream) logStream, pbs, pbs
					.getNumItems());
			if (returnValue == true) {
				returnValue = map.writeBases((OutputStream) logStream, bss, bss
						.getNumItems());
			}
			if (returnValue == true) {
				returnValue = map.writeStarts((OutputStream) logStream, ss, ss
						.getNumItems());
			}
			if (returnValue == true) {
				returnValue = map.writeRuns((OutputStream) logStream);
			}
		} catch (Exception e) {
			returnValue = false;
		}
		if (returnValue == true) {
			isRunning = true;
		}

		return returnValue;
	}
}
