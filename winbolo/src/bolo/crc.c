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


/*********************************************************
*Name:          crc
*Filename:      crc.c 
*Author:        John Morrison
*Creation Date:  7/3/98
*Last Modified: 23/5/99
*Purpose:
*  Provides CRC Operations
*********************************************************/

/* Code based upon code in the public domain. */

/* Includes */
#include <stdlib.h>
#include "global.h"
#include "crc.h"

unsigned int crc_16_table[16] = {
  0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
  0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400 };

/*********************************************************
*NAME:          CRCCalc
*AUTHOR:        John Morrison
*CREATION DATE:  7/3/98
*LAST MODIFIED: 23/5/98
*PURPOSE:
*  Generates a CRC for a buffer buff of length buffLen
*  Sets number of bases to zero.
*
*ARGUMENTS:
*  buff    - Pointer to the buffer
*  buffLen - Length of the buffer
*********************************************************/
int CRCCalc(BYTE *buff, int buffLen) {
  unsigned short int crc = 0;
  int r;
  /* while there is more data to process */
  while (buffLen-- > 0) {
    /* compute checksum of lower four bits of *p */
    r = crc_16_table[crc & 0xF];
    crc = (crc >> 4) & 0x0FFF;
    crc = crc ^ r ^ crc_16_table[*buff & 0xF];
    /* now compute checksum of upper four bits of *p */
    r = crc_16_table[crc & 0xF];
    crc = (crc >> 4) & 0x0FFF;
    crc = crc ^ r ^ crc_16_table[(*buff >> 4) & 0xF];    
    /* next... */    
    buff++;
  } 
  return(crc);
}

/*********************************************************
*NAME:          CRCCalcBytes
*AUTHOR:        John Morrison
*CREATION DATE: 7/3/98
*LAST MODIFIED: 7/3/98
*PURPOSE:
*  Generates a CRC for a buffer buff of length buffLen
*  Sets number of bases to zero
*
*ARGUMENTS:
*  buff    - Pointer to the buffer
*  buffLen - Length of the buffer
*  crcA    - Pointer to hold CRC Byte 1
*  crcB    - Pointer to hold CRC Byte 2
*********************************************************/
void CRCCalcBytes(BYTE *buff, int buffLen, BYTE *crcA, BYTE *crcB) {
  int newCRC; /* CRC Calculated */
  int conv;   /* Used in the conversion */

  newCRC = CRCCalc(buff, buffLen);
  conv = newCRC;
  conv <<= CRC_SHIFT_LEFT_1;
  conv >>= CRC_SHIFT_RIGHT;
  *crcA = (BYTE) conv;
  conv = newCRC;
  conv <<= CRC_SHIFT_LEFT_2;
  conv >>= CRC_SHIFT_RIGHT;
  *crcB = (BYTE) conv;
}

/*********************************************************
*NAME:          CRCCheck
*AUTHOR:        John Morrison
*CREATION DATE: 7/3/98
*LAST MODIFIED: 7/3/98
*PURPOSE:
*  Checks a CRC known CRC against a buffer to see if they
*  match
*
*ARGUMENTS:
*  buff    - Pointer to the buffer
*  buffLen - Length of the buffer
*  crcA    - CRC Byte 1
*  crcB    - CRC Byte 2
*********************************************************/
bool CRCCheck(BYTE *buff, int buffLen, BYTE crcA, BYTE crcB) {
  bool returnValue; /* Value to return */
  BYTE testA;       /* Test CRC Byte A */
  BYTE testB;       /* Test CRC Byte B */

  returnValue = TRUE;
  /* Generate the CRC */
  CRCCalcBytes(buff, buffLen, &testA, &testB);

  /* Perform the check */
  if (testA != crcA || testB != crcB) {
    returnValue = FALSE;
  }

  return returnValue;

}

