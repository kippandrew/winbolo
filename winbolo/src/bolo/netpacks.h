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
*Name:          Network Packet Types
*Filename:      netpacks.h
*Author:        John Morrison
*Creation Date: 30/10/99
*Last Modified: 20/09/02
*Purpose:
*  Defines all the different network packet types
*********************************************************/

#ifndef _NETPACKS_H
#define _NETPACKS_H


#define MAX_UDPPACKET_SIZE 1024
#define MAX_TCPPACKET_SIZE 1024

#define BOLO_VERSION_MAJOR       0x01
#define BOLO_VERSION_MAJORPOS    4
#define BOLO_VERSION_MINOR       0x01
#define BOLO_VERSION_MINORPOS    5
#define BOLO_VERSION_REVISION    0x05
#define BOLO_VERSION_REVISIONPOS 6

/* Packet types */
/* Info Packet */
#define BOLOPACKET_INFOREQUEST   13
#define BOLOPACKET_INFORESPONSE  14
/* Ping packet */
#define BOLOPACKET_PINGREQUEST 15
#define BOLOPACKET_PINGRESPONSE 16
/* Password Packet */
#define BOLOPACKET_PASSWORDCHECK 17  /* Check password  */
#define BOLOPACKET_PASSWORDACCEPT 18 /* Password OK     */
#define BOLOPACKET_PASSWORDFAIL 19   /* Password failed */
/* Check for player name availability */
#define BOLOPACKET_NAMECHECK 20       /* Check name        */
#define BOLOPACKET_NAMEACCEPT 21      /* Name check OK     */
#define BOLOPACKET_NAMEFAIL 22        /* Name check failed */
/* Player Data */
#define BOLOPACKET_PLAYERDATAREQUEST 23  /* Check name      */
#define BOLOPACKET_PLAYERDATARESPONSE 24 /* Name check OK   */
/* Player number */
#define BOLOPACKET_PLAYERNUMREQUEST 25   /* Check name      */
#define BOLOPACKET_PLAYERNUMRESPONSE 26  /* Name check OK   */
/* New player packet info */
#define BOLOPACKET_PLAYERNEWPLAYER 27    /* New Player data */
/* Base Data */
#define BOLOPACKET_BASESDATAREQUEST 29
#define BOLOPACKET_BASESDATARESPONSE 30
/* Starts Data */
#define BOLOPACKET_STARTSDATAREQUEST 31
#define BOLOPACKET_STARTSDATARESPONSE 32
/* Pillbox Data */
#define BOLOPACKET_PILLSDATAREQUEST 33
#define BOLOPACKET_PILLSDATARESPONSE 34
/* Map Download data */
#define BOLOPACKET_MAPDATAREQUEST 35
#define BOLOPACKET_MAPDATARESPONSE 36
/* Message Packet to a single player packet */
#define BOLOPACKET_MESSAGE 37
/* Change player name Packet */
#define BOLOCHANGENAME_DATA 38
/* Packet OK/Fail */
#define BOLOPACKET_VALID 39
#define BOLOPACKET_INVALID 40
#define BOLOPACKET_PLAYERLEAVE 41
/* Game Time request/Response */
#define BOLOPACKET_TIMEREQUEST 42
#define BOLOPACKET_TIMERESPONSE 43
/* Message to all players */
#define BOLOPACKET_MESSAGE_ALL_PLAYERS 44

/* Data Packet */
#define BOLOPACKET_TOKEN 50
#define BOLOPACKET_DATA 28
/* Player Position/LGM/Shells etc */
#define BOLOPOSITION_DATA 29
/* Client Data - Shells etc */
#define BOLOCLIENT_DATA 45

/* Alliance packets */
#define BOLOLEAVEALLIANCE_DATA 46
#define BOLOREQUESTALLIANCE_DATA 47
#define BOLOACCEPTALLIANCE_DATA 48

#define BOLOALLOWNEWPLAYERS 51
#define BOLONOALLOWNEWPLAYERS 52
#define BOLOREJOINREQUEST 53

#define BOLOLGMRETURN 54

#define BOLOREQUEST_STARTPOS 55
#define BOLORESPONSE_STARTPOS 56

#define BOLOPACKET_SERVERKEYREQUEST 57
#define BOLOPACKET_SERVERKEYRESPONSE 58

#define BOLOPACKET_CLIENTKEY 59

#define BOLOPACKET_PACKETREREQUEST 60
#define BOLOPACKET_PACKETQUIT 61



/* LGM Out working */
#define BOLOPACKET_LGM_OUTWORKING 62

/* Game is Locked */
#define BOLOPACKET_GAMELOCKED 63

/* Max Players reached */
#define BOLOPACKET_MAXPLAYERS 64

/* All your missing packets */
#define BOLOPACKET_RETRANSMITTED_PACKETS 65

/* Server message packet */
#define BOLOSERVERMESSAGE 49

#define INFOREQUESTHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_INFOREQUEST }
#define TOKENHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_TOKEN }
#define DATAHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_DATA }
#define GENERICHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION }
#define MESSAGEHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_MESSAGE }
#define MESSAGEHEADER_ALLPLAYERS { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_MESSAGE_ALL_PLAYERS }
#define POSHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPOSITION_DATA }
#define VALIDHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_VALID }
#define INVALIDHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_INVALID }
#define TIMEHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_TIMERESPONSE }
#define CLIENTHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOCLIENT_DATA }
#define SEVERMSGHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOSERVERMESSAGE }
#define ALLOWPLAYERSHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOALLOWNEWPLAYERS }
#define DISALLOWPLAYERSHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLONOALLOWNEWPLAYERS }
#define REQUESTHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOREJOINREQUEST };
#define LGMRETURNHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOLGMRETURN };
#define LGMWORKINGHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_LGM_OUTWORKING};

#define STARTREQUESTHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOREQUEST_STARTPOS };
#define STARTRESPONSEHEADER { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLORESPONSE_STARTPOS };

/* Size of the request packet */
#define BOLOPACKET_REQUEST_SIZE 8
/* position of the request/response part */
#define BOLOPACKET_REQUEST_TYPEPOS 7

/* Size of the mandatory token packet data */
#define BOLOPACKET_MAND_DATA 17

/* Optional Data in the data packets */
#define BOLO_PACKET_MAPDATA 10
#define BOLO_PACKET_SHELLDATA 11
#define BOLO_PACKET_TKDATA 12
#define BOLO_PACKET_PNBDATA 13
#define BOLO_PACKET_MNTDATA 14
#define BOLO_PACKET_SHELLNHDATA 15
/* Game Time request/Response */
#define BOLOPACKET_TIMEREQUEST 42
#define BOLOPACKET_TIMERESPONSE 43

/* Size of the option data header */
#define BOLO_PACKET_OPTDATAHEADER 2


#define BOLO_PACKET_CRC_SIZE 2

/* Mines */
#define ALL_MINES_VISIBLE 0xC0
#define HIDDEN_MINES 0x80
/* "Bolo" string and size of */
#define BOLO_SIGNITURE "Bolo"
#define BOLO_SIGNITURE_SIZE 4

/* String displayed in single player games rather then address */
#define NET_SINGLE_PLAYER_GAME "Single Player Game\0"



#endif /* _NETPACKS_H */

