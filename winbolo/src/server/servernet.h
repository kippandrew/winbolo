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
*Name:          Server Network
*Filename:      Server Net.h
*Author:        John Morrison
*Creation Date: 21/02/99
*Last Modified: 01/03/03
*Purpose:
*  Backend network packet handler
*********************************************************/

#ifndef _NETSERVER_H
#define _NETSERVER_H

#include "../bolo/global.h"
#include "../bolo/backend.h"
#include "rsaalgorithm.h"

#ifdef _WIN32
  #include <winsock.h>
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

/* Defines */
#define SERVER_NET_REMOVE_CHECKTIME (1) /* Check for removing every 1 seconds */

/* Typdefs */

#pragma pack(push, 1)


#ifndef _PACKETS_DEFINED
#define _PACKETS_DEFINED

/* The network type of game being played */
typedef enum {
  netNone, /* Game hasn't Started */
  netSingle,
  netUdp
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


/* Bolo info packet Response */
#pragma pack(8)

#ifndef _BRAINS_H
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
  BYTE key[32];                     /* WinBolo.net key */
  char password[MAP_STR_SIZE];      /* Test password - Pascal string (first byte is length)  */
  char rsaencrypted[RSA_DATA_SIZE];			/* rsa encrypted string */
} PLAYERNUM_REQ_PACKET;


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

#define BOLOPACKET_PLAYERNUMREQPOS (BOLOPACKET_REQUEST_TYPEPOS + 1)

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


/* Rejoin request packet */
typedef struct {
  BOLOHEADER h;
  BYTE playerNumber; /* player reuesting the rejoin */
} REJOINREQUEST_PACKET;

/* Map Data download Request packet */
typedef struct {
  BOLOHEADER h;
  BYTE yPos; /* Y Position of the data */
} MAPDOWNLOADREQUEST_PACKET;

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

/* Prototypes */

/*********************************************************
*NAME:          serverNetCreate
*AUTHOR:        John Morrison
*CREATION DATE: 15/08/99
*LAST MODIFIED: 27/02/03
*PURPOSE:
* Sets the network kind of game being played and sets up
* udp
*
*ARGUMENTS:
*  myPort   - UDP port on this machine
*  password - Password to use to verify joins
*  ai       - Ai Type of the game (allow/disallow/adv)
*  trackerAddr - Address of the tracker to use
*  trackerPort - Port of the tracker
*  useTracker  - Whether to use the tracker or not
*  useAddr     - Address to use for binding else NULL
*  maxPlayers  - Max players this server can handle (0 for
*                unlimited)
*********************************************************/
bool serverNetCreate(unsigned short myPort, char *password, aiType ai, char *trackerAddr, unsigned short trackerPort, bool useTracker, char *useAddr, BYTE maxPlayers);

/*********************************************************
*NAME:          serverNetDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Shuts down the network. Shutsdown transport as well.
*
*ARGUMENTS:
*
*********************************************************/
void serverNetDestroy(void);

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
*NAME:          netCheck
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED:  5/6/99
*PURPOSE:
* Checks for new packets and updates the backend if there
* is only one player in a network game.
*
*ARGUMENTS:
*  check - TRUE if to check for packets
*********************************************************/
void netCheck(bool check);

/*********************************************************
*NAME:          netCheckLast
*AUTHOR:        John Morrison
*CREATION DATE: 5/6/99
*LAST MODIFIED: 5/6/99
*PURPOSE:
* We are leaving a game. Call this. (waits for the next
* token before returning)
*
*ARGUMENTS:
*
*********************************************************/
void netCheckLast(void);

/*********************************************************
*NAME:          serverNetUDPPacketArrive
*AUTHOR:        John Morrison
*CREATION DATE: 15/08/99
*LAST MODIFIED: 19/08/02
*PURPOSE:
* A UDP packet has arrived. It is processed here.
*
*ARGUMENTS:
*  buff  - Buffer that has arrived.
*  len   - length of the packet
*  addr  - Address long
*  port  - The port the last packet came in on
*********************************************************/
void serverNetUDPPacketArrive(BYTE *buff, int len, unsigned long addr, unsigned short port);

/*********************************************************
*NAME:          serverNetTCPPacketArrive
*AUTHOR:        John Morrison
*CREATION DATE: 27/08/99
*LAST MODIFIED: 19/08/02
*PURPOSE:
* A UDP packet has arrived. It is processed here.
*
*ARGUMENTS:
*  buff      - Buffer that has arrived.
*  len       - length of the packet
*  playerNum - The player number packet came from
*  addr  - Address long
*  port  - The port the last packet came in on
*********************************************************/
void serverNetTCPPacketArrive(BYTE *buff, int len, BYTE playerNum, unsigned long addr, unsigned short port);


/*********************************************************
*NAME:          netPacketTokenArrive
*AUTHOR:        John Morrison
*CREATION DATE: 5/6/99
*LAST MODIFIED: 5/6/99
*PURPOSE:
* Was the last packet we got the token packet?
*
*ARGUMENTS:
*  buff  - Buffer that has arrived.
*  len   - length of the packet
*********************************************************/
bool netPacketTokenArrive(void);

/*********************************************************
*NAME:          serverNetMakeInfoRespsonse
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* A request for an info packet has been made. Make a 
* response packet.
*
*ARGUMENTS:
*  buff - Buffer to hold the info packet.
*********************************************************/
void serverNetMakeInfoRespsonse(INFO_PACKET *buff);

/*********************************************************
*NAME:          serverNetMakePacketHeader
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Makes a packet header.
*
*ARGUMENTS:
*  hdr        - Pointer to the bolo header structure
*  packetType - The packet type to make
*********************************************************/
void serverNetMakePacketHeader(BOLOHEADER *hdr, BYTE packetType);

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
*NAME:          netJoinPing
*AUTHOR:        John Morrison
*CREATION DATE: 24/2/99
*LAST MODIFIED: 24/2/99
*PURPOSE:
* Pings each machine in the ring 3 times starting with the
* parameter given. The machine with the smallest ping time
* is copied back into the parameters if the operation is
* sucessful otherwise it returns FALSE
*
*ARGUMENTS:
*  ip   - String pointing to the IP of the first (returns 
*         best) machine
*  port - Port of the start (returns best) machine
*********************************************************/
bool netJoinPing(char *ip, unsigned short *port);

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
*NAME:          netChangeNextPrev
*AUTHOR:        John Morrison
*CREATION DATE: 282/99
*LAST MODIFIED: 28/2/99
*PURPOSE:
* Updates the next previous players in the ring.
*
*ARGUMENTS:
*
*********************************************************/
void netChangeNextPrev(void);

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
*NAME:          netGetNextAddressStr
*AUTHOR:        John Morrison
*CREATION DATE: 5/3/99
*LAST MODIFIED: 5/3/99
*PURPOSE:
* Copies next machine address into the dest if we are 
* playing a network game, else sets dest to an empty 
* string
*
*ARGUMENTS:
*  dest - string to hold address
*********************************************************/
void netGetNextAddressStr(char *dest);

/*********************************************************
*NAME:          netGetPrevAddressStr
*AUTHOR:        John Morrison
*CREATION DATE: 5/3/99
*LAST MODIFIED: 5/3/99
*PURPOSE:
* Copies previous machine address into the dest if we are 
* playing a network game, else sets dest to an empty 
* string
*
*ARGUMENTS:
*  dest - string to hold address
*********************************************************/
void netGetPrevAddressStr(char *dest);

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
*NAME:          netSendAllPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 18/3/99
*LAST MODIFIED: 18/3/99
*PURPOSE:
* Sends the data to all players in a game
*
*ARGUMENTS:
*  buff    - Buffer to send
*  buffLen - Length of the buffer 
*********************************************************/
void netSendAllPlayers(BYTE *buff, int buffLen);

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
*NAME:          netRemovePlayer
*AUTHOR:        John Morrison
*CREATION DATE: 21/3/99
*LAST MODIFIED: 21/3/99
*PURPOSE:
* Removes a player from the netplayers structure
*
*ARGUMENTS:
*  playerNum - The player number to remove.
*********************************************************/
void netRemovePlayer(BYTE playerNum);

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
*NAME:          netSeenBefore
*AUTHOR:        John Morrison
*CREATION DATE: 19/6/99
*LAST MODIFIED: 19/6/99
*PURPOSE:
* We have seen this packet before. Determine if it is
* corrupt or not and send the appropriate reply
*
*ARGUMENTS:
*  buff - The packet
*  len  - Packet Length
*********************************************************/
void netSeenBefore(BYTE *buff, int len);

/*********************************************************
*NAME:          serverNetPlayerNumReq
*AUTHOR:        John Morrison
*CREATION DATE: 29/08/99
*LAST MODIFIED: 19/08/02
*PURPOSE:
* Request for a player number. Checks name isn't in use and
* returns a new player number for the player. Sends out
* this new players details to the rest of the players in
* the game
*
*ARGUMENTS:
*  buff    - Buffer that has arrived.
*  len     - length of the packet
*  addr    - Address this packet came form
*  port    - Port this packet came from
*********************************************************/
void serverNetPlayerNumReq(BYTE *buff, int len, unsigned long addr, unsigned short port);

/*********************************************************
*NAME:          serverNetPlayerLeave
*AUTHOR:        John Morrison
*CREATION DATE: 29/08/99
*LAST MODIFIED: 18/12/02
*PURPOSE:
* Called when a TCP socket is closed indicating a player
* has left the game. Tells all the clients about it and 
* make slot available.
*
*ARGUMENTS:
*  playerNum - The player number that quit
*  graceful  - True if the player choose to quit, false
*              if timed out
*********************************************************/
void serverNetPlayerLeave(BYTE playerNum, bool graceful);

/*********************************************************
*NAME:          serverNetPlayerDataReq
*AUTHOR:        John Morrison
*CREATION DATE: 29/08/99
*LAST MODIFIED: 19/19/02
*PURPOSE:
* A request has been made for player data packet. Process
* it and send the reply.
*
*ARGUMENTS:
*  buff      - Buffer that has arrived.
*  len       - length of the packet
*********************************************************/
void serverNetPlayerDataReq();

/*********************************************************
*NAME:          serverNetChangePlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 29/8/99
*LAST MODIFIED: 29/8/99
*PURPOSE:
* A change of player name packet has been recieved. Send
* it out the make the change.
*
*ARGUMENTS:
*  sockNum - The socket it came from
*  buff    - Buffer that has arrived.
*********************************************************/
void serverNetChangePlayerName(BYTE sockNum, BYTE *buff);

/*********************************************************
*NAME:          serverNetMakePosPackets
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 31/8/99
*PURPOSE:
* Makes and send out the positions of every person in the
* game.
*
*ARGUMENTS:
*
*********************************************************/
void serverNetMakePosPackets(void);

/*********************************************************
*NAME:          severNetProcessClientData
*AUTHOR:        John Morrison
*CREATION DATE: 30/10/99
*LAST MODIFIED:  21/8/00
*PURPOSE:
* Client data has arrived. Client data consists of:
* - Shells that have been fired.
*
*ARGUMENTS:
*  buff      - Buffer that has arrived.
*  len       - Length of the packet.
*  playerNum - Player number this packet belongs with
*********************************************************/
void severNetProcessClientData(BYTE *buff, int len, BYTE playerNum);

/*********************************************************
*NAME:          severNetProcessClientData
*AUTHOR:        John Morrison
*CREATION DATE: 30/10/99
*LAST MODIFIED: 30/10/99
*PURPOSE:
* Makes the server data to be sent to all clients.
* Includes:
* - Map Data
*
*ARGUMENTS:
*
*********************************************************/
void serverNetMakeData();

/*********************************************************
*NAME:          serverNetSendTrackerUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 22/12/99
*LAST MODIFIED: 22/12/99
*PURPOSE:
* Called when it is time to send the tracker an updated
* game snapshot
*
*ARGUMENTS:
*
*********************************************************/
void serverNetSendTrackerUpdate();

/*********************************************************
*NAME:          serverNetSetLock
*AUTHOR:        John Morrison
*CREATION DATE: 1/6/00
*LAST MODIFIED: 1/6/00
*PURPOSE:
* Sets whether the server lock is on or off
*
*ARGUMENTS:
*  lockState - The state to set to 
*********************************************************/
void serverNetSetLock(bool lockState);

/*********************************************************
*NAME:          serverNetIsGameLocked
*AUTHOR:        John Morrison
*CREATION DATE: 1/6/00
*LAST MODIFIED: 1/6/00
*PURPOSE:
* Returns whether the game is locked or not
*
*ARGUMENTS:
*
*********************************************************/
bool serverNetIsGameLocked();

/*********************************************************
*NAME:          serverNetSendQuitMessage
*AUTHOR:        John Morrison
*CREATION DATE: 1/6/00
*LAST MODIFIED: 1/6/00
*PURPOSE:
* Sends the quit message to all clients
*
*ARGUMENTS:
*
*********************************************************/
void serverNetSendQuitMessage();

void serverNetSendAllExceptPlayer(BYTE playerNum, BYTE *buff, int len);
void serverNetSendPlayer(BYTE playerNum, BYTE *buff, int len);
void serverNetSendAll(BYTE *buff, int len);

/*********************************************************
*NAME:          serverNetCheckRemovePlayers
*AUTHOR:        John Morrison
*CREATION DATE: 2/08/02
*LAST MODIFIED: 2/08/02
*PURPOSE:
* Sends the man has return packet to playerNum
*
*ARGUMENTS:
*  playerNum - Player number to send to.
*  numTrees  - Amount of trees lgm is carrying
*  numMines  - Amount of mines the lgm is carrying
*  pillNum   - Pillbox being carries
*********************************************************/
void serverNetCheckRemovePlayers();

void serverNetSendManWorkingMessage(BYTE playerNum, BYTE mapX, BYTE mapY, BYTE numMines, BYTE numTrees, BYTE pillNum);

/*********************************************************
*NAME:          serverNetSendserverMessageAllPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 2/08/02
*LAST MODIFIED: 2/08/02
*PURPOSE:
* Sends a server message to all players 
*
*ARGUMENTS:
*  msg - Message to send 
*********************************************************/
void serverNetSendServerMessageAllPlayers(char *msg);

/*********************************************************
*NAME:          serverNetGetMaxPlayers
*AUTHOR:        John Morrison
*CREATION DATE: 19/11/03
*LAST MODIFIED: 19/11/03
*PURPOSE:
* Returns the maximum number of players allowed on this
* server
*
*ARGUMENTS:
*
*********************************************************/
BYTE serverNetGetMaxPlayers();

/*********************************************************
*NAME:          serverNetGetIsLocked
*AUTHOR:        John Morrison
*CREATION DATE: 19/11/03
*LAST MODIFIED: 19/11/03
*PURPOSE:
* Returns if the game is locked or not.
*
*ARGUMENTS:
*
*********************************************************/
bool serverNetGetIsLocked();

/*********************************************************
*NAME:          serverNetGetUs
*AUTHOR:        John Morrison
*CREATION DATE: 16/08/04
*LAST MODIFIED: 16/08/04
*PURPOSE:
* Gets the servers IP address and port
*
*ARGUMENTS:
* buff - 4 byte buffer for IP bytes
* port - Port to copy to
*********************************************************/
void serverNetGetUs(BYTE *buff, unsigned short *port);

#pragma pack(pop)

#endif /* _NETSERVER_H */

