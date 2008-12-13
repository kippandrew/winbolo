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
*Name:          UDP Packets
*Filename:      udppackets.c
*Author:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*Purpose:
* Handles keeping track of network packets for
* retransmission on errors
*
*  Process: Client to Server
* -------------------------
* Client--->
* 1. Build reliable packet and give it a sequence number
* 2. Send
* 3. Copy packet to sequence number position X of buffer.
* 4. Increment next packet sequence number.
* Server--->
* 5. Server Receives packet
* 6. Server checks sequence number. If valid increments to 
*    next available sequence number and process packet.
* 7. If invalid sends back a packet request for missing 
*    sequence number(s) and stores packets at sequence 
*    position for processing.
*
*
* Process: Server to Client
* -------------------------
* Server-->
* 1. Build reliable packet.
* 2. For each client makes the correct sequence number
*    and sends it.
* 3. Copy packet to sequence number position x of buffer
* 4. Increment next packet sequence number
* Client--->
* 5. Client Receives packet
* 6. Client checks sequence number. If valid increments
*    to next available sequence number.
* 7. If invalid sends back a packet request for missing 
*    sequence number(s) and stores packets at sequence
*    position for processing.
*********************************************************/

#include "global.h"
#include "udppackets.h"

/*********************************************************
*NAME:          udpPacketsCreate
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Creates an udpPackets struncture. Returns NULL on error
*
*ARGUMENTS:
*
*********************************************************/
udpPackets udpPacketsCreate(void) {
  udpPackets returnValue; /* Value to return */
  BYTE count;             /* Looping variable */

  New(returnValue);
  if (returnValue != NULL) {
    returnValue->inUpTo = 0;
    returnValue->inSequenceNumber = 1; /* In sequence number is always +1 */
    returnValue->outSequenceNumber = 0;
    count = 0;

    while (count < MAX_UDP_SEQUENCE) {
      returnValue->outLens[count] = UDP_NO_PACKET;
      returnValue->inLens[count] = UDP_NO_PACKET;
      count++;
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          udpPacketsDestroy
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Destroys a udpPackets structure
*
*ARGUMENTS:
* value - UdpPackets item
*********************************************************/
void udpPacketsDestroy(udpPackets *value) {
  if (*value != NULL) {
    Dispose(*value);
    *value = NULL;
  }
}

/*********************************************************
*NAME:          udpPacketsGetOutSequenceNumber
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Returns the current outbound sequence number
*
*ARGUMENTS:
* value - UdpPackets item
*********************************************************/
BYTE udpPacketsGetOutSequenceNumber(udpPackets *value) {
  return (*value)->outSequenceNumber;
}

/*********************************************************
*NAME:          udpPacketsGetInSequenceNumber
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Returns the current inbound sequence number
*
*ARGUMENTS:
* value - UdpPackets item
*********************************************************/
BYTE udpPacketsGetInSequenceNumber(udpPackets *value) {
  return (*value)->inSequenceNumber;
}

/*********************************************************
*NAME:          udpPacketsGetNextInSequenceNumber
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Returns the next inbound sequence number and increments
* it
*
*ARGUMENTS:
* value - UdpPackets item
*********************************************************/
BYTE udpPacketsGetNextInSequenceNumber(udpPackets *value) {
  (*value)->inSequenceNumber = (*value)->inSequenceNumber + 1;
  if ((*value)->inSequenceNumber == MAX_UDP_SEQUENCE) {
    (*value)->inSequenceNumber = 0;
  }
  return (*value)->inSequenceNumber;
}


/*********************************************************
*NAME:          udpPacketsGetNextOutSequenceNumber
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Returns the next outbound sequence number and increments
* it
*
*ARGUMENTS:
* value - UdpPackets item
*********************************************************/
BYTE udpPacketsGetNextOutSequenceNumber(udpPackets *value) {
  (*value)->outSequenceNumber = (*value)->outSequenceNumber + 1;
  if ((*value)->outSequenceNumber == MAX_UDP_SEQUENCE) {
    (*value)->outSequenceNumber = 0;
  }
  return (*value)->outSequenceNumber;
}

/*********************************************************
*NAME:          udpPacketsGetInBuff
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Returns a pointer to the inbound byte array at 
* sequenceNumber
*
*ARGUMENTS:
* value          - UdpPackets item
* sequenceNumber - Sequence number position
*********************************************************/
BYTE* udpPacketsGetInBuff(udpPackets *value, BYTE sequenceNumber) {
  return (*value)->inBuff[sequenceNumber];
}

/*********************************************************
*NAME:          udpPacketsGetOutBuff
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Returns a pointer to the outbound pointer at 
* sequenceNumber
*
*ARGUMENTS:
* value          - UdpPackets item
* sequenceNumber - Sequence number position
*********************************************************/
BYTE* udpPacketsGetOutBuff(udpPackets *value, BYTE sequenceNumber) {
  return (*value)->outBuff[sequenceNumber];
}

/*********************************************************
*NAME:          udpPacketsGetInBuffLength
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Returns the size of the buffer at sequenceNumber in the
* incoming packet stream
*
*ARGUMENTS:
* value          - UdpPackets item
* sequenceNumber - Sequence number position
*********************************************************/
int udpPacketsGetInBuffLength(udpPackets *value, BYTE sequenceNumber) {
  return (*value)->inLens[sequenceNumber];
}

/*********************************************************
*NAME:          udpPacketsGetOutBuffLength
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Returns the size of the buffer at sequenceNumber in the
* outgoing packet stream
*
*ARGUMENTS:
* value          - UdpPackets item
* sequenceNumber - Sequence number position
*********************************************************/
int udpPacketsGetOutBuffLength(udpPackets *value, BYTE sequenceNumber) {
  return (*value)->outLens[sequenceNumber];
}

/*********************************************************
*NAME:          udpPacketsSetInBuff
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Sets the packet at position sequence number to buff in
* the inbound items
*
*ARGUMENTS:
* value          - UdpPackets item
* sequenceNumber - Sequence number position
* buff           - Buffer to set
* length         - Lenght of the buffer
*********************************************************/
void udpPacketsSetInBuff(udpPackets *value, BYTE sequenceNumber, BYTE *buff, int length) {
  memcpy((*value)->inBuff[sequenceNumber], buff, (unsigned int) length);
  (*value)->inLens[sequenceNumber] = length;
}

/*********************************************************
*NAME:          udpPacketsSetOutBuff
*AUTHOR:        John Morrison
*Creation Date: 24/02/02
*Last Modified: 24/02/02
*PURPOSE:
* Sets the packet at position sequence number to buff in
* the output items
*
*ARGUMENTS:
* value          - UdpPackets item
* sequenceNumber - Sequence number position
* buff           - Buffer to set
* length         - Lenght of the buffer
*********************************************************/
void udpPacketsSetOutBuff(udpPackets *value, BYTE sequenceNumber, BYTE *buff, int length) {
  memcpy((*value)->outBuff[sequenceNumber], buff, (unsigned int) length);
  (*value)->outLens[sequenceNumber] = length;
}

