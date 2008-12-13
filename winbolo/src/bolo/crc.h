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
*Filename:      crc.h
*Author:        John Morrison
*Creation Date: 7/3/98
*Last Modified: 7/3/99
*Purpose:
*  Provides CRC Operations
*********************************************************/

#ifndef CRC_H
#define CRC_H
/* Includes */
#include "global.h"

/* Magic CRC generating Divide */
#define CRC_NUM 42
/* Shifting to get the bits */
#define CRC_SHIFT_LEFT_1 16
#define CRC_SHIFT_RIGHT 24
#define CRC_SHIFT_LEFT_2 24


/*********************************************************
*NAME:          CRCCalc
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
*********************************************************/
int CRCCalc(BYTE *buff, int buffLen);

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
void CRCCalcBytes(BYTE *buff, int buffLen, BYTE *crcA, BYTE *crcB);

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
bool CRCCheck(BYTE *buff, int buffLen, BYTE crcA, BYTE crcB);

#endif /* CRC_H */
