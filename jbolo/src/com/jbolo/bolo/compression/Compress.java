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
 * Compress.java
 *
 * Created on 31 May 2004, 00:07
 */

/* Heavily based upon codrle1.c by David Bourgin released in the public domain */
/* This used to be a LZW implemtnation, hence the directory, file and function names but replaced by a RLE implementation as it was more effecient. Could probably be replaced with built in map RLE functions. */


package com.jbolo.bolo.compression;

import java.io.ByteArrayOutputStream;
import com.jbolo.bolo.Utils;

/**
 * Provides the internal RLE compression routines used Based on code by David
 * Bourgin
 * 
 * @author David Bourgin
 * @author John Morrison
 */
public class Compress {

	/** Byte Array Output Stream used for creating compressed data */
	private ByteArrayOutputStream bos;
	/** Compressed data */
	private byte data[];
	/** How many bytes we have read */
	private int upto;
	/** Length nof the source data */
	private int srcLen;
	/** The source data to compress */
	private byte srcData[];

	/**
	 * Creates a new compress object
	 * 
	 * @param data
	 *            Data to compress
	 * @param length
	 *            Length of the data
	 */
	public Compress(byte data[], int length) {
		bos = new ByteArrayOutputStream(length);

		srcData = data;
		upto = 0;
		srcLen = length;

		try {
			process();
			this.data = bos.toByteArray();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Gets the length of the compressed data
	 * 
	 * @return compressed data length
	 */
	public int getLength() {
		return data.length;
	}

	/**
	 * Gets the compressed data
	 * 
	 * @return byte array containing the data
	 */
	public byte[] getData() {
		return data;
	}

	/**
	 * Have we reached the end of the data to compress
	 * 
	 * @return boolean
	 */
	private boolean endOfData() {
		if (upto >= srcLen) {
			return true;
		}
		return false;
	}

	/**
	 * Gets a single byte from the uncompressed data array
	 * 
	 * @return byte converted to int to get around java's unsigned bytes
	 */
	private int readByte() {
		upto++;
		return Utils.ConvertByteToInt(srcData[upto - 1]);
	}

	/**
	 * Writes a single byte of compressed data
	 * 
	 * @param value
	 *            Byte to write
	 * @throws Exception
	 *             on IO error
	 */
	private void writeByte(byte value) {
		bos.write(value);
	}

	/**
	 * Writes an array of data into the compressed stream
	 * 
	 * @param data
	 *            Array of data to write
	 * @param length
	 *            Length of data to write
	 * @throws Exception
	 *             on IO and array out of bounds error
	 */
	private void writeArray(int data[], int length) throws Exception {
		int count = 0;
		while (count < length) {
			bos.write(data[count]);
			count++;
		}
	}

	/**
	 * Compresses the data
	 * 
	 * @throws Exception
	 *             on IO and array out of bounds error
	 */
	private void process() throws Exception {
		int byte1;
		int byte2;
		int frame_size;
		int array[] = new int[129];

		if (!endOfData()) {
			byte1 = readByte(); /* Is there at least a byte to analyze? */
			frame_size = 1;
			if (!endOfData())
			/* Are there at least two bytes to analyze? */
			{
				byte2 = readByte();
				frame_size = 2;
				do {
					if (byte1 == byte2)
					/* Is there a repetition? */
					{
						while ((!endOfData()) && (byte1 == byte2)
								&& (frame_size < 129)) {
							byte2 = readByte();
							frame_size++;
						}
						if (byte1 == byte2)
						/* Do we meet only a sequence of bytes? */
						{
							writeByte((byte) (126 + frame_size));
							writeByte((byte) byte1);
							if (!endOfData()) {
								byte1 = readByte();
								frame_size = 1;
							} else
								frame_size = 0;
						} else /* No, then don't handle the last byte */
						{
							writeByte((byte) (125 + frame_size));
							writeByte((byte) byte1);
							byte1 = byte2;
							frame_size = 1;
						}
						if (!endOfData()) {
							byte2 = readByte();
							frame_size = 2;
						}
					} else /*
							 * Prepare the array of comparisons where will be
							 * stored all the identical bytes
							 */
					{
						array[0] = byte1;
						array[1] = byte2;
						while ((!endOfData())
								&& (array[frame_size - 2] != array[frame_size - 1])
								&& (frame_size < 128)) {
							array[frame_size] = readByte();
							frame_size++;
						}
						if (array[frame_size - 2] == array[frame_size - 1])
						/*
						 * Do we meet a sequence of all different bytes followed
						 * by identical byte?
						 */
						{ /* Yes, then don't count the two last bytes */
							writeByte((byte) (frame_size - 3));
							writeArray(array, frame_size - 2);
							byte1 = array[frame_size - 2];
							byte2 = byte1;
							frame_size = 2;
						} else {
							writeByte((byte) (frame_size - 1));
							writeArray(array, frame_size);
							if (endOfData())
								frame_size = 0;
							else {
								byte1 = readByte();
								if (endOfData())
									frame_size = 1;
								else {
									byte2 = readByte();
									frame_size = 2;
								}
							}
						}
					}
				} while ((!endOfData()) || (frame_size >= 2));
			}
			if (frame_size == 1) {
				writeByte((byte) 0);
				writeByte((byte) byte1);
			}
		}
	}
}
