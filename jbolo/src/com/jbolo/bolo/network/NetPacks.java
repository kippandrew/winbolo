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
 * NetPacks.java
 *
 * Created on 23 May 2004, 00:46
 */

package com.jbolo.bolo.network;

import com.jbolo.bolo.map.BoloMap; // For some map information
import com.jbolo.winbolonet.WBN; // For Key lengths

/**
 *
 * @author  John Morrison
 */
public class NetPacks {
    
    
    public final static int MAX_UDPPACKET_SIZE = 1024;
    public final static int MAX_TCPPACKET_SIZE = 1024;
    
    public final static byte BOLO_VERSION_MAJOR       = 0x01;
    public final static byte BOLO_VERSION_MAJORPOS    = 4;
    public final static byte BOLO_VERSION_MINOR       = 0x01;
    public final static byte BOLO_VERSION_MINORPOS    = 5;
    public final static byte BOLO_VERSION_REVISION    = 0x04;
    public final static byte BOLO_VERSION_REVISIONPOS = 6;
    
    /* Packet types */
    /* Info Packet */
    public final static byte BOLOPACKET_INFOREQUEST   = 13;
    public final static byte BOLOPACKET_INFORESPONSE  =14;
    /* Ping packet */
    public final static byte BOLOPACKET_PINGREQUEST =15;
    public final static byte BOLOPACKET_PINGRESPONSE =16;
    /* Password Packet */
    public final static byte BOLOPACKET_PASSWORDCHECK =17;  /* Check password  */
    public final static byte BOLOPACKET_PASSWORDACCEPT =18; /* Password OK     */
    public final static byte BOLOPACKET_PASSWORDFAIL =19;   /* Password failed */
    /* Check for player name availability */
    public final static byte BOLOPACKET_NAMECHECK =20;       /* Check name        */
    public final static byte BOLOPACKET_NAMEACCEPT =21;      /* Name check OK     */
    public final static byte BOLOPACKET_NAMEFAIL =22;        /* Name check failed */
    /* Player Data */
    public final static byte BOLOPACKET_PLAYERDATAREQUEST =23;  /* Check name      */
    public final static byte BOLOPACKET_PLAYERDATARESPONSE =24; /* Name check OK   */
    /* Player number */
    public final static byte BOLOPACKET_PLAYERNUMREQUEST =25;   /* Check name      */
    public final static byte BOLOPACKET_PLAYERNUMRESPONSE =26;  /* Name check OK   */
    /* New player packet info */
    public final static byte BOLOPACKET_PLAYERNEWPLAYER =27;    /* New Player data */
    /* Base Data */
    public final static byte BOLOPACKET_BASESDATAREQUEST =29;
    public final static byte BOLOPACKET_BASESDATARESPONSE =30;
    /* Starts Data */
    public final static byte BOLOPACKET_STARTSDATAREQUEST =31;
    public final static byte BOLOPACKET_STARTSDATARESPONSE =32;
    /* Pillbox Data */
    public final static byte BOLOPACKET_PILLSDATAREQUEST =33;
    public final static byte BOLOPACKET_PILLSDATARESPONSE =34;
    /* Map Download data */
    public final static byte BOLOPACKET_MAPDATAREQUEST =35;
    public final static byte BOLOPACKET_MAPDATARESPONSE =36;
    /* Message Packet to a single player packet */
    public final static byte BOLOPACKET_MESSAGE =37;
    /* Change player name Packet */
    public final static byte BOLOCHANGENAME_DATA =38;
    /* Packet OK/Fail */
    public final static byte BOLOPACKET_VALID =39;
    public final static byte BOLOPACKET_INVALID =40;
    public final static byte BOLOPACKET_PLAYERLEAVE =41;
    /* Game Time request/Response */
    public final static byte BOLOPACKET_TIMEREQUEST =42;
    public final static byte BOLOPACKET_TIMERESPONSE =43;
    /* Message to all players */
    public final static byte BOLOPACKET_MESSAGE_ALL_PLAYERS =44;
    
    /* Data Packet */
    public final static byte BOLOPACKET_TOKEN =50;
    public final static byte BOLOPACKET_DATA =28;
    /* Player Position/LGM/Shells etc */
    public final static byte BOLOPOSITION_DATA =29;
    /* Client Data - Shells etc */
    public final static byte BOLOCLIENT_DATA =45;
    
    /* Alliance packets */
    public final static byte BOLOLEAVEALLIANCE_DATA =46;
    public final static byte BOLOREQUESTALLIANCE_DATA= 47;
    public final static byte BOLOACCEPTALLIANCE_DATA =48;
    
    public final static byte BOLOALLOWNEWPLAYERS =51;
    public final static byte BOLONOALLOWNEWPLAYERS =52;
    public final static byte BOLOREJOINREQUEST =53;
    
    public final static byte BOLOLGMRETURN =54;
    
    public final static byte BOLOREQUEST_STARTPOS =55;
    public final static byte BOLORESPONSE_STARTPOS =56;
    
    public final static byte BOLOPACKET_SERVERKEYREQUEST = 57;
    public final static byte BOLOPACKET_SERVERKEYRESPONSE = 58;
    public final static byte BOLOPACKET_CLIENTKEY = 59;
    
    public final static byte BOLOPACKET_PACKETREREQUEST = 60;
    public final static byte BOLOPACKET_PACKETQUIT = 61;
    
    
    
    /* LGM Out working */
    public final static byte BOLOPACKET_LGM_OUTWORKING =62;
    
    /* Game is Locked */
    public final static byte BOLOPACKET_GAMELOCKED =63;
    
    /* Max Players reached */
    public final static byte BOLOPACKET_MAXPLAYERS = 64;
    
    /* All your missing packets */
    public final static byte BOLOPACKET_RETRANSMITTED_PACKETS = 65;
    
    /* Server message packet */
    public final static byte BOLOSERVERMESSAGE = 49;
    
    public final static byte HEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION };
    public final static byte INFOREQUESTHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_INFOREQUEST };
    public final static byte TOKENHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_TOKEN };
    public final static byte DATAHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_DATA };
    public final static byte GENERICHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION , 0};
    public final static byte MESSAGEHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_MESSAGE };
    public final static byte MESSAGEHEADER_ALLPLAYERS[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_MESSAGE_ALL_PLAYERS };
    public final static byte POSHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPOSITION_DATA };
    public final static byte VALIDHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_VALID };
    public final static byte INVALIDHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_INVALID };
    public final static byte TIMEHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_TIMERESPONSE };
    public final static byte CLIENTHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOCLIENT_DATA };
    public final static byte SEVERMSGHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOSERVERMESSAGE };
    public final static byte ALLOWPLAYERSHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOALLOWNEWPLAYERS };
    public final static byte DISALLOWPLAYERSHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLONOALLOWNEWPLAYERS };
    public final static byte REQUESTHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOREJOINREQUEST };
    public final static byte LGMRETURNHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOLGMRETURN };
    public final static byte LGMWORKINGHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOPACKET_LGM_OUTWORKING};
    
    public final static byte STARTREQUESTHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLOREQUEST_STARTPOS };
    public final static byte STARTRESPONSEHEADER[] = { 'B','o','l','o', BOLO_VERSION_MAJOR, BOLO_VERSION_MINOR, BOLO_VERSION_REVISION, BOLORESPONSE_STARTPOS };
    
    /* Size of the request packet */
    public final static byte BOLOPACKET_REQUEST_SIZE = 8;
    /* position of the request/response part */
    public final static byte BOLOPACKET_REQUEST_TYPEPOS = 7;
    
    /** Where the data starts in the packet - Right after the header */
    public final static byte DATA_START = (byte) (BOLOPACKET_REQUEST_TYPEPOS + 1);
    
    /** Location in the packet of the player number position */
    public final static byte BOLOPACKET_PLAYERNUMPOS  = (BOLOPACKET_REQUEST_TYPEPOS + 1);
    
    public final static byte BOLOPACKET_MAPDDOWNLOAD_YPOS = DATA_START;
    
    /* Size of the mandatory token packet data */
    public final static byte BOLOPACKET_MAND_DATA = 17;
    
    /* Optional Data in the data packets */
    public final static byte BOLO_PACKET_MAPDATA =10;
    public final static byte BOLO_PACKET_SHELLDATA =11;
    public final static byte BOLO_PACKET_TKDATA =12;
    public final static byte BOLO_PACKET_PNBDATA =13;
    public final static byte BOLO_PACKET_MNTDATA =14;
    public final static byte BOLO_PACKET_SHELLNHDATA = 15;
    
    /* Size of the option data header */
    public final static byte BOLO_PACKET_OPTDATAHEADER = 2;
    
    /** Size of CRC */
    public final static byte BOLO_PACKET_CRC_SIZE = 2;
    
    /** Size of CRC */
    public final static byte CRC_SIZE = 2;
    
    /* Mines */
    public final static int ALL_MINES_VISIBLE = 0xC0;
    public final static int HIDDEN_MINES = 0x80;
    /* "Bolo" string and size of */
    public static String BOLO_SIGNITURE = "Bolo";
    public final static byte BOLO_SIGNITURE_SIZE = 4;
    
    /** Size of a player name */
    public static final byte PLAYER_NAME_LEN = 33;
    
    
    /** String displayed in single player games rather then server address */
    public static String NET_SINGLE_PLAYER_GAME = "Single Player Game";
    
    /** Size of a password packet to send */
    public final static int SIZE_PASSWORD_PACKET = DATA_START + BoloMap.MAP_STR_SIZE;
    
    /** Size of a password packet to send */
    public final static int SIZE_NAMECHECK_PACKET = DATA_START + PLAYER_NAME_LEN;
    
    
    /** Size of a response packet (Header + 1 byte) */
    public final static int SIZE_PACKETRESPONSE = 8;
    
    /** Size of a response packet with CRC (Header + 1 byte) */
    public final static int SIZE_PACKETRESPONSE_WITHCRC = SIZE_PACKETRESPONSE + BOLO_PACKET_CRC_SIZE;
    
    /** Size of a player number request packet */
    public final static int SIZE_PACKET_PLAYERNUM_REQ = (NetPacks.DATA_START + 2 + WBN.WINBOLONET_KEY_LEN + NetPacks.PLAYER_NAME_LEN + BoloMap.MAP_STR_SIZE);
    
    /** Size of a player number response packet. Is a response packet with CRC with 1 data byte of the player number */
    public final static int SIZE_PACKET_PLAYERNUMRESPONSE = SIZE_PACKETRESPONSE_WITHCRC + 1;
    
    /** Size of a request packet. Header + 1 byte */
    public final static int SIZE_PACKET_REQUEST = DATA_START;
    
    /** Ping packet size<br>
     * typedef struct {<br>
     *   BOLOHEADER h;<br>
     *   BYTE from;     // Player that sent this<br>
     *   long sendTime; // Time sent<br>
     *   BYTE inPacket; // In packet Number<br>
     *   BYTE outPacket; // Output packet Number<br>
     * } PING_PACKET;<br>
     */
    public final static int SIZE_PACKET_PING = NetPacks.HEADER.length + 1 + 1 + 2+8+1;
    
    /**
     * The player data response packet. It consists of 8 byte header,
     * followed by the 16 players data (60 bytes each) and finally a 2 byte CRC
     *<br>
     *<br>
     * typedef struct {<br
     * BYTE inUse;                        // Is this slot in use<br>
     * char playerName[PLAYER_NAME_LEN];  // Players Name<br>
     * BYTE numAllies;                    // Number of allies<br>
     * BYTE allies[MAX_TANKS];            // Filled with allie<br>
     * struct in_addr addr;               // This machines address and port<br>
     * unsigned short port;<br>
     * } PLAYERDATAITEM_PACKET;<br>
     *<br>
     * typedef struct {<br>
     * BOLOHEADER h;                             // Header<br>
     * PLAYERDATAITEM_PACKET items[MAX_TANKS];   // The players<br>
     * } PLAYERDATA_PACKET;<br>
     */
    public final static int SIZE_PLAYERDATA_PACKET = HEADER.length + 1 + (16 * 60) + BOLO_PACKET_CRC_SIZE;
    
    /**
     * Size of a re-request data packet. This packet is sent when
     *reliable data is lost and we want it back. CRC are not included as java code does this <pre>
     * typedef struct {
     * BOLOHEADER h;
     * BYTE rerequestItem;
     * BYTE nonReliable;
     * BYTE crcA;
     * BYTE crcB;
     * } REREQUEST_PACKET;</pre>
     *
     */
    public final static int SIZE_PACKET_REREQUEST = 10;
    
    /**
     * Size of Time response packet<br>
     * typedef struct {<br>
     * BOLOHEADER h;<br>
     * long start_delay;    // if non zero, time until game starts, (50ths)<br>
     * long time_limit;     // if non zero, time until game ends, (50ths)<br>
     * } TIME_PACKET;<br>
     */
    public final static int SIZE_PACKET_TIME = HEADER.length + 1 + 8;
    
    /** Creates a new instance of NetPacks */
    public NetPacks() {
    }
    
}
