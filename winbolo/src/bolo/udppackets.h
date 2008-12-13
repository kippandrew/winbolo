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
*Filename:      udppackets.h
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

#ifndef _UDP_PACKETS_H
#define _UDP_PACKETS_H

#include "global.h"

/* Defines */


/* We track a maximum of 64 reliable packets */
#define MAX_UDP_SEQUENCE 200 /* 64 */
/* Packets are of maximum size of 1K */
#define SIZE_OF_PACKET 1024
/* -1 indicates there is no packet at this sequence */
#define UDP_NO_PACKET (-1)

/* A non reliable packet will have this marker */
#define UDP_NON_RELIABLE_PACKET 254

typedef struct udpPacketsObj *udpPackets;

struct udpPacketsObj {
  BYTE outSequenceNumber;                     /* The next packet sequence number to get for sending out */
  BYTE outBuff[MAX_UDP_SEQUENCE][SIZE_OF_PACKET]; /* Copy of the packets for retransmission */
  int outLens[MAX_UDP_SEQUENCE];                 /* Size of each packet */
  BYTE inSequenceNumber; 	                    /* The next packet sequence number expected */
  BYTE inUpTo;                                /* Sequence number to process up to  on error */
  BYTE inBuff[MAX_UDP_SEQUENCE][SIZE_OF_PACKET];  /* Copy of the incoming packets for processing on missing packets */
  int inLens[MAX_UDP_SEQUENCE];                 /* Size of each packet */
};

/* Prototypes */

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
udpPackets udpPacketsCreate(void);

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
void udpPacketsDestroy(udpPackets *value);

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
BYTE udpPacketsGetOutSequenceNumber(udpPackets *value);

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
BYTE udpPacketsGetInSequenceNumber(udpPackets *value);

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
BYTE udpPacketsGetNextInSequenceNumber(udpPackets *value);

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
BYTE udpPacketsGetNextOutSequenceNumber(udpPackets *value);

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
BYTE* udpPacketsGetInBuff(udpPackets *value, BYTE sequenceNumber);

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
BYTE* udpPacketsGetOutBuff(udpPackets *value, BYTE sequenceNumber);

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
int udpPacketsGetInBuffLength(udpPackets *value, BYTE sequenceNumber);

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
int udpPacketsGetOutBuffLength(udpPackets *value, BYTE sequenceNumber);

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
void udpPacketsSetInBuff(udpPackets *value, BYTE sequenceNumber, BYTE *buff, int length);

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
********************************************************/
void udpPacketsSetOutBuff(udpPackets *value, BYTE sequenceNumber, BYTE *buff, int length);

#endif /* _UDP_PACKETS_H */

