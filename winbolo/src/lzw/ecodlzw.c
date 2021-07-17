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


/* Heavily based upon codrle1.c by David Bourgin released in the public domain */

/* This used to be a LZW implemtnation, hence the directory, file and function names but replaced by a RLE implementation as it was more effecient. Could probably be replaced with built in map RLE functions. */

/* Error codes sent to the caller */
/* Useful constants. */
#define FALSE 0
#define TRUE  1

/* Global variables. */
char *compressSrc, *compressDest;

int compressLen; /* Length of the compressed array */
int uncompressLen; /* Length of the data to compress */
int compressUpto; /* Where we are up to in the compression */

/* Pseudo procedures */
int conend_of_data() {
  if (compressUpto >= uncompressLen) {
    return TRUE;
  } else {
    return FALSE;
  }
}


char conread_byte() {
  compressUpto++;
  return compressSrc[compressUpto-1];
}


void conwrite_byte(char c) {
  compressDest[compressLen] = c;
  compressLen++;
}

void conwrite_array(char *c, int numBytes) {
  int count = 0;
  while (count < numBytes) {
    compressDest[compressLen] = c[count];
    compressLen++;
    count++;
  }
}

  
int lzwencoding(char *src, char *dest, int len)
/* Returned parameters: None
   Action: Compresses with RLE type 1 method all bytes read by the function 'read_byte'
   Errors: An input/output error could disturb the running of the program
*/
{  unsigned char byte1,byte2,frame_size,
                         array[129];

  compressSrc = src;
  compressDest = dest;
  compressLen = 0;
  compressUpto = 0;
  uncompressLen = len;

  if (!conend_of_data())
     { byte1=conread_byte();    /* Is there at least a byte to analyze? */
       frame_size=1;
       if (!conend_of_data())
                             /* Are there at least two bytes to analyze? */
          { byte2=conread_byte();
            frame_size=2;
            do { if (byte1==byte2)
                             /* Is there a repetition? */
                    { while ((!conend_of_data())&&(byte1==byte2)&&(frame_size<129))
                            { byte2=conread_byte();
                              frame_size++;
                            }
                      if (byte1==byte2)
                             /* Do we meet only a sequence of bytes? */
                        { conwrite_byte((char) (126+frame_size));
                          conwrite_byte(byte1);
                          if (!conend_of_data())
                             { byte1=conread_byte();
                               frame_size=1;
                             }
                          else frame_size=0;
                        }
                      else   /* No, then don't handle the last byte */
                           { conwrite_byte((char) (125+frame_size));
                             conwrite_byte(byte1);
                             byte1=byte2;
                             frame_size=1;
                           }
                      if (!conend_of_data())
                         { byte2=conread_byte();
                           frame_size=2;
                         }
                    }
                 else        /* Prepare the array of comparisons
                                where will be stored all the identical bytes */
                      { *array = byte1;
                        array[1]=byte2;
                        while ((!conend_of_data())&&(array[frame_size-2]!=array[frame_size-1])&&(frame_size<128))
                              { array[frame_size]=conread_byte();
                                frame_size++;
                              }
                        if (array[frame_size-2]==array[frame_size-1])
                             /* Do we meet a sequence of all different bytes followed by identical byte? */
                           { /* Yes, then don't count the two last bytes */
                             conwrite_byte((char) (frame_size-3));
                             conwrite_array(array,frame_size-2);
                             byte1=array[frame_size-2];
                             byte2=byte1;
                             frame_size=2;
                           }
                        else { conwrite_byte((char) (frame_size-1));
                               conwrite_array(array,frame_size);
                               if (conend_of_data())
                                  frame_size=0;
                               else { byte1=conread_byte();
                                      if (conend_of_data())
                                         frame_size=1;
                                      else { byte2=conread_byte();
                                             frame_size=2;
                                           }
                                    }
                             }
                      }
               }
            while ((!conend_of_data())||(frame_size>=2));
          }
       if (frame_size==1)
          { conwrite_byte(0);
            conwrite_byte(byte1);
          }
     }
  return compressLen;
}

