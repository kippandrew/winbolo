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

/* Heavily based upon dcodrle1.c by David Bourgin released in the public domain */

/* This used to be a LZW implemtnation, hence the directory, file and function names but replaced by a RLE implementation as it was more effecient. Could probably be replaced with built in map RLE functions. */


/* File: dcodrle1.c
   Author: David Bourgin
   Creation date: 1/2/94
   Last update: 24/7/95
   Purpose: Example of RLE type 1 decoding with a file source to decompress
*/

#include <stdio.h>
/* For routines printf,fgetc,fputc and fwrite */
#include <memory.h>
/* For routine memset */

/* Useful constants */
#define FALSE 0
#define TRUE  1

/* Global variables. */
char *compressSrc, *compressDest;

int compressLen; /* Length of the compressed array */
int uncompressLen; /* Length of the data to compress */
int compressUpto; /* Where we are up to in the compression */

/* Pseudo procedures */
__inline int decend_of_data() {
  if (compressUpto >= uncompressLen) {
    return TRUE;
  } else {
    return FALSE;
  }
}


__inline char decread_byte() {
  compressUpto++;
  return compressSrc[compressUpto-1];
}


__inline void decwrite_byte(char c) {
  compressDest[compressLen] = c;
  compressLen++;
}

void decwrite_array(char *c, int numBytes) {
  int count = 0;
  while (count < numBytes) {
    compressDest[compressLen] = c[count];
    compressLen++;
    count++;
  }
}



#define decwrite_block(byte,time_nb)  { unsigned char array_to_write[129];\
                                     (void)memset(array_to_write,(byte),(time_nb));\
                                     decwrite_array(array_to_write,(time_nb));\
                                   }


int lzwdecoding(char *src, char *dest, int len)
/* Returned parameters: None
   Action: Decompresses with RLE type 1 method all bytes read by the function read_byte
   Erreurs: An input/output error could disturb the running of the program
*/
{ unsigned char header;
  unsigned char i;

  compressSrc = src;
  compressDest = dest;
  compressLen = 0;
  compressUpto = 0;
  uncompressLen = len;
  while (!decend_of_data())
        { header=decread_byte();
          switch (header & 128)
          { case 0:for (i=0;i<=header;i++)
                       decwrite_byte(decread_byte());
                   break;
            case 128:decwrite_block(decread_byte(),(header & 127)+2);
          }
        }
  return compressLen;
}

