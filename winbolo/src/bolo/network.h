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
*Name:          Network
*Filename:      network.h
*Author:        John Morrison
*Creation Date: 21/02/99
*Last Modified: 09/12/03
*Purpose:
*  Backend network packet handler
*********************************************************/

#ifndef NETWORK_H
#define NETWORK_H

#include "global.h"
#ifdef _WIN32
#include <winsock.h>

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif

/* Defines */

/* Typdefs */
#pragma pack(push, enter_network_obj,1)


#ifndef _PACKETS_DEFINED
#define _PACKETS_DEFINED


/* The network type of game being played */
typedef enum {
  netNone,   /* Game hasn't Started */
  netSingle, /* Single-player (non-networked) game */
  netUdp     /* Networked game */
} netType;


/* Network status */
typedef enum {
  netJoining,
  netRunning,
  netStartDownload, /* First thing we do */
  netBaseDownload,  /* 2nd thing */
  netPillDownload,  /* 3rd thing */
  netMapDownload,   /* 4th - Map download */
  netTimeDownload,  /* Final. Get game times */
  netFailed
} netStatus;

/* Where to send the packet */
typedef enum {
  packetNext, /* The next machine in the ring */
  packetPrev, /* The prev machine in the ring */
  packetLast  /* The machine that just sent us a packet */
} packetType;

/* Bolo header packets */
typedef struct {
  BYTE signature[4];    /* 'Bolo'                */
  BYTE versionMajor;    /* BOLO_VERSION_MAJOR    */
  BYTE versionMinor;    /* BOLO_VERSION_MINOR    */
  BYTE versionRevision; /* BOLO_VERSION_REVISION */
  BYTE type;            /* Packet Type           */
} BOLOHEADER;

#pragma pack(8)
/* Bolo info packet Response */
#ifndef _BRAINS_H
/* Only define it if we need it */
typedef struct {
  struct in_addr serveraddress;
  unsigned short serverport;
  u_long start_time;
} GAMEID;
#endif

typedef struct {
  BOLOHEADER h;

  char mapname[MAP_STR_SIZE]; /* Pascal string (first byte is length)         */
  GAMEID gameid;       /* 8 byte unique ID for game (combination       */
                       /* of starting machine address & timestamp)     */
  BYTE gametype;       /* Game type (1, 2 or 3: open, tourn. & strict) */
  BYTE allow_mines;    /* 0x80 for normal hidden mines                 */
                       /* 0xC0 for all mines visible                   */
  BYTE allow_AI;       /* 0 for no AI tanks, 1 for AI tanks allowed    */
  BYTE spare1;         /* 0                                            */
  long start_delay;    /* if non zero, time until game starts, (50ths) */
  long time_limit;     /* if non zero, time until game ends, (50ths)   */

  WORD num_players;    /* number of players                            */
  WORD free_pills;     /* number of free (neutral) pillboxes           */
  WORD free_bases;     /* number of free (neutral) refuelling bases    */
  BYTE has_password;   /* non-zero if game has password set            */
  BYTE spare2;         /* 0                                            */
} INFO_PACKET;
#pragma pack(1)

/* Time response packet */
typedef struct {
  BOLOHEADER h;
  long start_delay;    /* if non zero, time until game starts, (50ths) */
  long time_limit;     /* if non zero, time until game ends, (50ths)   */
} TIME_PACKET;

/* Ping packet repsonse */
typedef struct {
  BOLOHEADER h;
  BYTE from;     /* Player that sent this */
  long sendTime; /* Time sent */
  BYTE inPacket; /* In packet Number */
  BYTE outPacket; /* Output packet Number */
} PING_PACKET;

/* Password test packet */
typedef struct {
  BOLOHEADER h;
  char password[MAP_STR_SIZE]; /* Test password - Pascal string (first byte is length)  */
} PASSWORD_PACKET;

/* RSA test packet */
typedef struct {
  BOLOHEADER h;
  char rsa[256]; /* Test rsa - Pascal string (first byte is length)  */
} RSA_PACKET;

/* Player Name check packet */
typedef struct {
  BOLOHEADER h;
  char playerName[PLAYER_NAME_LEN]; /* Test player name - Pascal string (first byte is length)  */
} PLAYERNAME_PACKET;

/* Player Name check packet */
typedef struct {
  BOLOHEADER h;
  char playerName[PLAYER_NAME_LEN]; /* Player name */
  unsigned short port;              /* UDP Port of this player */
  BYTE key[32];						/* WinBolo.net key */
  char password[MAP_STR_SIZE];      /* Test password - Pascal string (first byte is length)  */
  char rsaencrypted[256];			/* rsa encrypted string */
} PLAYERNUM_REQ_PACKET;

#define BOLOPACKET_PLAYERNUMREQPOS (BOLOPACKET_REQUEST_TYPEPOS + 1)

typedef struct {
  BYTE inUse;                        /* Is this slot in use */
  char playerName[PLAYER_NAME_LEN];  /* Players Name */
  BYTE numAllies;                    /* Number of allies  */
  BYTE allies[MAX_TANKS];            /* Filled with allie */
  struct in_addr addr;               /* This machines address and port */
  unsigned short port;
} PLAYERDATAITEM_PACKET;

typedef struct {
  BOLOHEADER h;                             /* Header */
  PLAYERDATAITEM_PACKET items[MAX_TANKS];   /* The players */
} PLAYERDATA_PACKET;

typedef struct {
  BOLOHEADER h;   /* Header */
  BYTE reqNumber; /* Player Number requested */
} PLAYERDATA_REQPACKET;


typedef struct {
  BOLOHEADER h;      /* Header */
  BYTE playerNumber; /* Player Number that has left */
} PLAYERLEAVE_PACKET;

#define BOLOPACKET_PLAYERNUMLEAVEPOS (BOLOPACKET_REQUEST_TYPEPOS + 1)


typedef struct {
  BOLOHEADER h;                      /* Header */
  BYTE playerNumber;                 /* Player Number of the new player  */
} PLAYERNUM_PACKET;

/* Position of platerNumber in the PLAYERNUM_PACKET packet */
#define BOLOPACKET_PLAYERNUMPOS (BOLOPACKET_REQUEST_TYPEPOS + 1)
#define BOLOPACKET_PLAYERNUMPOSLOW (BOLOPACKET_REQUEST_TYPEPOS + 2)
#define BOLOPACKET_PLAYERNUMPOSMAX (BOLOPACKET_REQUEST_TYPEPOS + 3)


/* New Player packet */
typedef struct {
  BOLOHEADER h;
  BYTE playerNumber; /* The new players player number */
  char playerName[PLAYER_NAME_LEN];  /* New Players Name */
  struct in_addr addr;               /* This machines address and port */
  unsigned short port;
} NEWPLAYER_PACKET;

/* Map Data download Request packet */
typedef struct {
  BOLOHEADER h;
  BYTE yPos; /* Y Position of the data */
} MAPDOWNLOADREQUEST_PACKET;

/* Rejoin request packet */
typedef struct {
  BOLOHEADER h;
  BYTE playerNumber; /* player reuesting the rejoin */
} REJOINREQUEST_PACKET;

/* Map Data download packet */
typedef struct {
  BOLOHEADER h;
  BYTE yPos; /* Y Position of the data */
  unsigned short dataLen;
  BYTE data[900]; /* X position data for the row - Excluding 40 mine squares */
} MAPDOWNLOAD_PACKET;

/* The LGM is moving */
typedef struct {
  BOLOHEADER h;
  BYTE mapX;
  BYTE mapY;
  BYTE pillNum;
  BYTE mines;
  BYTE trees;
} LGMMOVE_PACKET;

/* Positions in the map download packet of the stuff */
#define BOLOPACKET_MAPDDOWNLOAD_YPOS 8
#define BOLOPACKET_MAPDDOWNLOAD_DATASTART 9

/* Player Name check packet */
typedef struct {
  BOLOHEADER h;
  BYTE playerNum;                   /* Player number */
  char playerName[PLAYER_NAME_LEN]; /* New player name - Pascal string (first byte is length)  */
} PLAYERNAMECHANGE_PACKET;

/* Alliance packet */
typedef struct {
  BOLOHEADER h;
  BYTE playerNum;                   /* Player number this is coming from */
  BYTE to;                          /* Person requesting/accepting alliance to */
                                    /* Not Used for leaving alliance           */
} ALLIANCE_PACKET;

/* LGM Return packet */
typedef struct {
  BOLOHEADER h;
  BYTE numMines; /* Mines */
  BYTE numPills; /* Pill */
  BYTE numTrees; /* Trees */
} LGMRETURN_PACKET;

/* Start Position packet */
typedef struct {
  BOLOHEADER h;
  BYTE x;
  BYTE y;
  TURNTYPE angle;
  BYTE numShells; /* Shells */
  BYTE numMines; /* Mines */
} STARTPOSITION_PACKET;


/* Rerequest packet */
typedef struct {
  BOLOHEADER h;
  BYTE rerequestItem;
  BYTE nonReliable;
  BYTE crcA;
  BYTE crcB;
} REREQUEST_PACKET;


#endif /* _PACKETS_DEFINED */

/* Maximum time before */
#define NET_MAX_TIMEOUT (30)

/* Prototypes */

void netSend(BYTE *buff, int len);

/*********************************************************
*NAME:          netSetup
*AUTHOR:        John Morrison
*CREATION DATE: 21/02/99
*LAST MODIFIED: 01/04/02
*PURPOSE:
* Sets the network kind of game being played and sets up
* netClient
*
*ARGUMENTS:
*  value       - The network type of game being played
*  myPort      - netClient port on this machine
*  targetIP    - Target IP on a machine to join
*  targetPort  - Target port on that machine
*  password    - Password for the net game (NULL for none)
*  usCreate    - TRUE if we started the game, FALSE if we
*                joined
*  trackerAddr - Address of the tracker to use
*  trackerPort - Port of the tracker
*  useTracker  - Whether to use the tracker or not
*  wantRejoin  - TRUE if we want to rejoin the game else 
*                just join
*  useWinboloNet - TRUE if we want to participate in
*                  WinBolo.net
*  wbnPassword   - Our WinBolo.net password
*********************************************************/
bool netSetup(netType value, unsigned short myPort, char *targetIp, unsigned short targetPort, char *password, bool usCreate, char *trackerAddr, unsigned short trackerPort, bool useTracker, bool wantRejoin, bool useWinboloNet, char *wbnPassword);

/*********************************************************
*NAME:          netDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Shuts down the network. Calls low-level packet drivers
* and shuts them down as required.
*
*ARGUMENTS:
*
*********************************************************/
void netDestroy(void);

/*********************************************************
*NAME:          netGetType
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Returns the network game type being played
*
*ARGUMENTS:
*
*********************************************************/
netType netGetType(void);

/*********************************************************
*NAME:          netUdpPacketArrive
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 12/1/00
*PURPOSE:
* A UDP packet has arrived. It is processed here.
*
*ARGUMENTS:
*  buff  - Buffer that has arrived.
*  len   - length of the packet
*  port  - The port this packet arrived on
*********************************************************/
void netUdpPacketArrive(BYTE *buff, int len, unsigned short port);

/*********************************************************
*NAME:          netTcpPacketArrive
*AUTHOR:        John Morrison
*CREATION DATE: 29/9/99
*LAST MODIFIED: 29/9/99
*PURPOSE:
* A TCP packet has arrived. It is processed here.
*
*ARGUMENTS:
*  buff  - Buffer that has arrived.
*  len   - length of the packet
*********************************************************/
void netTcpPacketArrive(BYTE *buff, int len);

/*********************************************************
*NAME:          netMakeInfoRespsonse
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* A request for an info packet has been made. Make a 
* response packet.
*
*ARGUMENTS:
*  buff - Buffer to hold the info packet.
*********************************************************/
void netMakeInfoRespsonse(INFO_PACKET *buff);

/*********************************************************
*NAME:          netMakePacketHeader
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* Makes a packet header.
*
*ARGUMENTS:
*  hdr        - Pointer to the bolo header structure
*  packetType - The packet type to make
*********************************************************/
void netMakePacketHeader(BOLOHEADER *hdr, BYTE packetType);

/*********************************************************
*NAME:          netMakeInfoRespsonse
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 21/2/99
*PURPOSE:
* A request for an ping packet has been made. Make a 
* response packet.
*
*ARGUMENTS:
*  buff - Buffer to hold the ping packet.
*********************************************************/
void netMakePingRespsonse(PING_PACKET *buff);

/*********************************************************
*NAME:          netJoin
*AUTHOR:        John Morrison
*CREATION DATE: 24/02/99
*LAST MODIFIED: 09/12/03
*PURPOSE:
* Attempts to join a game. Returns whether the operation
* was sucessful or not
*
*ARGUMENTS:
*  ip         - String IP of the contact machine
*  port       - Port of the contact machine
*  wantRejoin - TRUE if we want to rejoin the game else 
*               just join
*  userName   - Account name we wish to use
*  password   - Account password
*  useWbn     - Do we want to try and use WBN?              
*  usCreate   - Is this client hosting the server
*  gamePassword - The game password (if we started it)
*********************************************************/
bool netJoin(char *ip, unsigned short port, bool wantRejoin, char *userName, char *password, bool useWbn, bool usCreate, char *gamePassword);

/*********************************************************
*NAME:          netJoinInit
*AUTHOR:        John Morrison
*CREATION DATE: 24/02/99
*LAST MODIFIED: 09/12/03
*PURPOSE:
* The first part. Varifies the contact machine exist.
* Makes sure the password is OK (if required) and player
* name is not in use. Returns whether the operation is
* sucessful or not. Also stores ai type, mapname 
*
*ARGUMENTS:
*  ip           - String IP of the contact machine
*  port         - Port of the contact machine
*  usCreate     - True if we created the game
*  gamePassword - The game password (if we started it)
*********************************************************/
bool netJoinInit(char *ip, unsigned short port, bool usCreate, char *gamePassword);

/*********************************************************
*NAME:          netJoinFinalise
*AUTHOR:        John Morrison
*CREATION DATE: 24/2/99
*LAST MODIFIED: 22/6/00
*PURPOSE:
* Finalising join process. Get list of all players and
* a player number. Returns success.
*
*ARGUMENTS:
*  targetip   - String IP of the contact machine
*  port       - Port of the contact machine
*
*  wantRejoin - TRUE if we want to rejoin the game else 
*               just join
*  userName   - Account name we wish to use
*  password   - Account password
*  useWbn     - Do we want to try and use WBN?
*********************************************************/
bool netJoinFinalise(char *targetip, unsigned short port, bool wantRejoin, char *userName, char *password, bool useWbn);

/*********************************************************
*NAME:          netPlayerJoined
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 26/2/99
*PURPOSE:
* A player has joined. Add data to players structure and
* add to netPlayers
*
*ARGUMENTS:
*  buff - Byte buffer of data
*********************************************************/
void netPlayerJoined(BYTE *buff);

/*********************************************************
*NAME:          netGetStatus
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 26/2/99
*PURPOSE:
* Returns the status of the network
*
*ARGUMENTS:
*
*********************************************************/
netStatus netGetStatus(void);

/*********************************************************
*NAME:          netMakeTokenPacket
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 28/2/99
*PURPOSE:
* Makes and sends the token packet.
*
*ARGUMENTS:
*
*********************************************************/
void netMakeTokenPacket(void);

/*********************************************************
*NAME:          netDataPacket
*AUTHOR:        John Morrison
*CREATION DATE: 28/2/99
*LAST MODIFIED: 28/2/99
*PURPOSE:
* A data packet has arrived. Look after it here.
*
*ARGUMENTS:
*   buff - Data buffer
*   len  - Length of the buffer
*********************************************************/
void netDataPacket(BYTE *buff, int len);

/*********************************************************
*NAME:          netDataPacket
*AUTHOR:        John Morrison
*CREATION DATE: 28/2/99
*LAST MODIFIED: 28/2/99
*PURPOSE:
* If we need to do any downloading request do them here
*
*ARGUMENTS:
*
*********************************************************/
void netJoinDataRequests(void);

/*********************************************************
*NAME:          netGetOurAddressStr
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 3/3/99
*PURPOSE:
* Copies our address into the dest if we are playing a 
* network game, else sets dest to an empty string
*
*ARGUMENTS:
*  dest - string to hold address
*********************************************************/
void netGetOurAddressStr(char *dest);

/*********************************************************
*NAME:          netGetServerAddressStr
*AUTHOR:        John Morrison
*CREATION DATE: 5/3/99
*LAST MODIFIED: 1/9/99
*PURPOSE:
* Copies server machine address into the dest if we are 
* playing a network game, else sets dest to the string
* "SINGLE PLAYER GAME"
*
*ARGUMENTS:
*  dest - string to hold address
*********************************************************/
void netGetServerAddressStr(char *dest);

/*********************************************************
*NAME:          netGetStats
*AUTHOR:        John Morrison
*CREATION DATE: 5/3/99
*LAST MODIFIED: 1/9/99
*PURPOSE:
* Gets copies of all the network game statistics.
*
*ARGUMENTS:
*  status  - String to hold status
*  ping    - Holds ring delay
*  ppsec   - Packets per second
*  retrans - Number of retransmissions required
*********************************************************/
void netGetStats(char *status, int *ping, int *ppsec, int *retrans);

/*********************************************************
*NAME:          netSecond
*AUTHOR:        John Morrison
*CREATION DATE: 6/3/99
*LAST MODIFIED: 6/3/99
*PURPOSE:
* Called once a second to reset packet per second counter
*
*ARGUMENTS:
*
*********************************************************/
void netSecond(void);

/*********************************************************
*NAME:          netDataPosPacket
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED: 12/1/00
*PURPOSE:
* A data position packet has arrived. Look after it here.
*
*ARGUMENTS:
*   buff - Data buffer
*   len  - Length of the buffer
*   port - Port the data came in on
*********************************************************/
void netDataPosPacket(BYTE *buff, int len, unsigned short port);

/*********************************************************
*NAME:          netMakeDataPosPacket
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED: 20/3/99
*PURPOSE:
* Makes and sends our player location and shells stuff
* and sends it to all players
*
*ARGUMENTS:
*
*********************************************************/
void netMakeDataPosPacket(void);

/*********************************************************
*NAME:          netGetDownloadPos
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/99
*LAST MODIFIED: 27/3/99
*PURPOSE:
* Returns the map download position
*
*ARGUMENTS:
*
*********************************************************/
BYTE netGetDownloadPos(void);

/*********************************************************
*NAME:          netGetDownloadPos
*AUTHOR:        John Morrison
*CREATION DATE: 30/3/99
*LAST MODIFIED: 30/3/99
*PURPOSE:
* A message has arived. Process it here
*
*ARGUMENTS:
*
*********************************************************/
void netMessagePacket(BYTE *data, int dataLen);

void netMessageSendAllPlayers(BYTE playerNum, char *message);

void netMessageSendPlayer(BYTE playerNum, BYTE destPlayer, char *message);

/*********************************************************
*NAME:          netSendChangePlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 17/4/99
*LAST MODIFIED: 17/4/99
*PURPOSE:
* You have changed your player name. Inform the other
* network players
*
*ARGUMENTS:
*  playerNum - Your player number
*  newName   - Your new player Name
*********************************************************/
void netSendChangePlayerName(BYTE playerNum, char *newName);

/*********************************************************
*NAME:          netGetChangePlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 17/4/99
*LAST MODIFIED: 17/4/99
*PURPOSE:
* A player name changed packet has arrived. Process it.
*
*ARGUMENTS:
*  info - The packet
*********************************************************/
void netGetChangePlayerName(PLAYERNAMECHANGE_PACKET *info);

/*********************************************************
*NAME:          netLeaveAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Sends a leave alliance message.
*
*ARGUMENTS:
*  playerNum - Player who is leaving alliance
*********************************************************/
void netLeaveAlliance(BYTE playerNum);

/*********************************************************
*NAME:          netRequestAlliance
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Sends a request alliance message.
*
*ARGUMENTS:
*  playerNum - Player who is leaving alliance
*  requestTo - Who we are requesting to
*********************************************************/
void netRequestAlliance(BYTE playerNum, BYTE reqestTo);

/*********************************************************
*NAME:          netLostConnection
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED: 1/11/99
*PURPOSE:
* Called when we lose our connection to the current 
* server.
*
*ARGUMENTS:
*
*********************************************************/
void netLostConnection();

/*********************************************************
*NAME:          netSendTrackerUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
* Called when it is time to send the tracker an updated
* game snapshot
*
*ARGUMENTS:
*
*********************************************************/
void netSendTrackerUpdate();

/*********************************************************
*NAME:          netGetNetTime
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/00
*LAST MODIFIED: 8/1/00
*PURPOSE:
*  Returns number of miliseconds spent doing network work
*  last second
*
*ARGUMENTS:
*
*********************************************************/
int netGetNetTime(void);

/*********************************************************
*NAME:          netSetAllowNewPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 1/6/00
*LAST MODIFIED: 1/6/00
*PURPOSE:
*  Sets the allow new players flag and send this clients
*  vote to the server
*
*ARGUMENTS:
*  allow - Whether we are allowing or disallowing
*********************************************************/
void netSetAllowNewPlayers(bool allow);

/*********************************************************
*NAME:          netAllianceAccept
*AUTHOR:        John Morrison
*CREATION DATE: 11/6/00
*LAST MODIFIED: 11/6/00
*PURPOSE:
*  We have accepted this player into our alliance. Send
*  to server.
*
*ARGUMENTS:
*  playerNum - Player we just accepted
*********************************************************/
void netAllianceAccept(BYTE playerNum);

/*********************************************************
*NAME:          netRequestStartPosition
*AUTHOR:        John Morrison
*CREATION DATE: 2/4/01
*LAST MODIFIED: 2/4/01
*PURPOSE:
*  Request a new starting position off the server.
*
*ARGUMENTS:
*
*********************************************************/
void netRequestStartPosition();

/*********************************************************
*NAME:          netProcessedDnsLookup
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
* Function called when the DNS lookup is completed
*
*ARGUMENTS:
*  ip   - IP Address passed to the lookup
*  host - Hostname returned
*********************************************************/
void netProcessedDnsLookup(char *ip, char *host);

/*********************************************************
*NAME:          netWinBoloNetSetup
*AUTHOR:        John Morrison
*CREATION DATE: 01/04/02
*LAST MODIFIED: 12/02/03
*PURPOSE:
* The first part. Varifies the contact machine exist.
* Makes sure the password is OK (if required) and player
* name is not in use. Returns whether the operation is
* sucessful or not. Also stores ai type, mapname 
*
*ARGUMENTS:
*  userName   - Account name we wish to use
*  password   - Account password
*********************************************************/
bool netWinBoloNetSetup(char *userName, char *password);

/*********************************************************
*NAME:          netSend
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/02
*PURPOSE:
*  Sends a packet to the server using reliable packets
*
*ARGUMENTS:
*  buff - Packet to send
*  len  - Buffer length
*********************************************************/
void netSend(BYTE *buff, int len);

/*********************************************************
*NAME:          netSendQuitMessage
*AUTHOR:        John Morrison
*CREATION DATE: 31/08/02
*LAST MODIFIED: 31/08/02
*PURPOSE:
*  Sends a quit message to the server
*
*ARGUMENTS:
*
*********************************************************/
void netSendQuitMessage();

#pragma pack(pop, enter_network_obj,1)

#endif /* NETWORK_H */
