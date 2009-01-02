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


/********************************************************
*Name:          netClient
*Filename:      netClient.c
*Author:        John Morrison
*Creation Date: 20/2/99
*LAST MODIFIED: 28/5/00
*Purpose:
*  netClient TCP/UDP packet driver.
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>
#include "..\..\bolo\global.h"
#include "..\winbolo.h"
#include "..\..\bolo\network.h"
#include "..\..\bolo\netpacks.h"
#include "..\..\bolo\crc.h"
#include "..\currentGames.h"
#include "..\resource.h"
#include "..\lang.h"
#include "..\netClient.h"
#include "..\..\bolo\udppackets.h"

SOCKET myUdpSock = INVALID_SOCKET; /* Our Udp socket */
unsigned short myPort;             /* Our netClient Port number */
struct sockaddr_in addrServer;     /* Server address */
unsigned short serverPort;         /* Server Port */
struct sockaddr_in addrLast;       /* Where the last UDP packet came from */
unsigned short lastPort;           /* Port the last UDP packet came from */
struct sockaddr_in addrUs;         /* Our own machine */
struct sockaddr_in addrTracker;    /* Tracker machine */

/* Thread stuff */

#define BUFFER_PREFIX_SIZE 2

int lzwdecoding(char *src, char *dest, int len);
int lzwencoding(char *src, char *dest, int len);


/*********************************************************
*NAME:          netClientCreate
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 29/4/00
*PURPOSE:
* Creates the netClient. Returns whether the operation
* was sucessful or not
*
*ARGUMENTS:
*  port - The port to create on (0== dont care)
*********************************************************/
bool netClientCreate(unsigned short port) {
  bool returnValue;        /* Value to return */
  WSADATA wsaData;         /* Winsock create data */
  int ret;                 /* Function returns */
  struct sockaddr_in addr; /* Socket structure */
  struct sockaddr_in name; /* Used to get port */
  int nameLen;             /* Used to set size of name */
  
  nameLen = sizeof(name);
  returnValue = TRUE;
  myPort = port;

  addrServer.sin_family = AF_INET;
  addrLast.sin_family = AF_INET;
  addrUs.sin_family = AF_INET;
  
  /* Startup Winsock */
  ret = WSAStartup(MAKEWORD(2,0), &wsaData);
  if (ret != 0) {
    MessageBoxA(NULL, langGetText(STR_NETCLIENTERR_WINSOCKFAILSTARTUP), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    returnValue = FALSE;
  }
  /* Create socket */
  if (returnValue == TRUE) {
    myUdpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (myUdpSock == INVALID_SOCKET) {
      returnValue = FALSE;
      MessageBoxA(NULL, langGetText(STR_NETCLIENTERR_CREATEUDPFAIL), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    } 
  }

  if (returnValue == TRUE) {
    returnValue = netClientSetUdpAsync(TRUE);
  }

  /* Bind to port */
  if (returnValue == TRUE) {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(myUdpSock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret != 0) {
      returnValue = FALSE;
      MessageBoxA(NULL, langGetText(STR_NETCLIENTERR_BINDUDPFAIL), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
    } else {
      /* Get what port we were assigned */
      if (getsockname(myUdpSock, (struct sockaddr*) &name, &nameLen) == 0) {
        myPort = ntohs(name.sin_port);
      }
    }
  }

  return returnValue;
}
  
/*********************************************************
*NAME:          netClientDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 30/8/99
*PURPOSE:
* Destroys the netClient.
*
*ARGUMENTS:
*
*********************************************************/
void netClientDestroy(void) {
  if (myUdpSock != INVALID_SOCKET) {
    shutdown(myUdpSock, SD_BOTH);
    closesocket(myUdpSock);
    myUdpSock = INVALID_SOCKET;
  }
  WSACleanup();
}

/*********************************************************
*NAME:          netClientSendUdpLast
*AUTHOR:        John Morrison
*CREATION DATE: 28/8/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sends a udp packet to the last address.
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*********************************************************/
void netClientSendUdpLast(BYTE *buff, int len) {
  sendto(myUdpSock, (char *) buff, len, 0, (struct sockaddr *)&addrLast, sizeof(addrLast));
}

/*********************************************************
*NAME:          netClientSendUdpServer
*AUTHOR:        John Morrison
*CREATION DATE: 28/8/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sends a udp packet to the server address.
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*********************************************************/
void netClientSendUdpServer(BYTE *buff, int len) {
  sendto(myUdpSock, (char *) buff, len, 0, (struct sockaddr *)&addrServer, sizeof(addrServer));
}


/*********************************************************
*NAME:          netClientGetServerAddress
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Copys the servers address and port
*
*ARGUMENTS:
*  dest - Destination address
*  port - Port of that corresponds to the address
*********************************************************/
void netClientGetServerAddress(struct in_addr *dest, unsigned short *port) {
  memcpy(dest, &(addrServer.sin_addr.s_addr), sizeof(dest));
  *port = ntohs(addrServer.sin_port);
}

/*********************************************************
*NAME:          netClientGetServerAddressString
*AUTHOR:        John Morrison
*CREATION DATE: 1/9/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Copys the servers address and port as a string.
*
*ARGUMENTS:
*  dest - Destination string
*********************************************************/
void netClientGetServerAddressString(char *dest) {
  char strPort[PLAYER_NAME_LAST]; /* Port as a string */
  char *ip;

  ip =inet_ntoa(addrServer.sin_addr);
  strcpy(dest, ip);
  sprintf(strPort, ":%d", ntohs(addrServer.sin_port));
  strcat(dest, strPort);
}


/*********************************************************
*NAME:          netClientSetServerAddress
*AUTHOR:        John Morrison
*CREATION DATE: 24/2/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Sets the servers address and port.
*
*ARGUMENTS:
*  src  - src address
*  port - Port of that corresponds to the address
*********************************************************/
void netClientSetServerAddress(struct in_addr *src, unsigned short port) {
  memcpy(&(addrServer.sin_addr.s_addr), src, sizeof(src)); 
  addrServer.sin_port = htons(port);
}

/*********************************************************
*NAME:          netClientSetServerPort
*AUTHOR:        John Morrison
*CREATION DATE: 12/1/00
*LAST MODIFIED: 12/1/00
*PURPOSE:
* Sets the servers port.
*
*ARGUMENTS:
*  port - Port of that corresponds to the address
*********************************************************/
void netClientSetServerPort(unsigned short port) {
  addrServer.sin_port = htons(port);
}

/*********************************************************
*NAME:          netClientSetUs
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sets up our IP address.
*
*ARGUMENTS:
*
*********************************************************/
void netClientSetUs(void) {
  char str[FILENAME_MAX]; /* Temp string to hold host name  & IP as string*/
  HOSTENT  *hd;           /* Host Data */
  int ipa;               /* Bytes for each IP address components */
  int ipb;
  int ipc;
  int ipd;

  ipa=ipb=ipc=ipd=0;

  gethostname(str, sizeof(str));
  hd = gethostbyname(str);
  if (hd != NULL) {
    ipa = hd->h_addr_list[IP_ARRAY0][IP_ARRAY0];
    ipb = hd->h_addr_list[IP_ARRAY0][IP_ARRAY1];
    ipc = hd->h_addr_list[IP_ARRAY0][IP_ARRAY2];
    ipd = hd->h_addr_list[IP_ARRAY0][IP_ARRAY3];
    sprintf(str, "%d.%d.%d.%d\0",ipa,ipb,ipc,ipd);
  } else {
    strcpy(str, "127.0.0.1\0");
  }
  addrUs.sin_addr.s_addr = inet_addr(str);
  addrUs.sin_port = myPort;
}

/*********************************************************
*NAME:          netClientGetAddress
*AUTHOR:        John Morrison
*CREATION DATE: 11/4/01
*LAST MODIFIED: 11/4/01
*PURPOSE:
* Reverse looks up an ip address
*
*ARGUMENTS:
*  ip   - IP Address to look up
*  dest - Buffer to hold returnm
*********************************************************/
void netClientGetAddress(char  *ip, char *dest) {
  HOSTENT  *hd;           /* Host Data */
  struct sockaddr_in  addr;
  
  addr.sin_addr.s_addr = inet_addr(ip);
  hd = gethostbyaddr((const void*)&addr.sin_addr, sizeof(struct in_addr), AF_INET);
   if (hd == NULL) {
    strcpy(dest, ip);
  } else {
    strcpy(dest, hd->h_name);
  }
}

/*********************************************************
*NAME:          netClientGetUs
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Gets our IP address and port.
*
*ARGUMENTS:
*  dest - Destination address
*  port - Destination port
*********************************************************/
void netClientGetUs(struct in_addr *dest, unsigned short *port) {
  memcpy(dest, &(addrUs.sin_addr), sizeof(*dest));
  *port = myPort;
}

/*********************************************************
*NAME:          netClientGetUsStr
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 28/8/993/3/99
*PURPOSE:
* Gets our IP address and port as a string
*
*ARGUMENTS:
*  dest - Destination string
*********************************************************/
void netClientGetUsStr(char *dest) {
  char port[32]; /* Temp pointer to string */
  char *ip;      /* Holds the ip address */
  ip = inet_ntoa(addrUs.sin_addr);
  strcpy(dest, ip);
  sprintf(port, ":%d", myPort);
  strcat(dest, port);
}

/* FIXME */
bool netClientUdpPingServer(BYTE *buff, int *len, bool wantCrc, bool addNonReliable) {
  bool returnValue;           /* Value to return */
  struct sockaddr_in from;    /* Socket returned from */
  int fromlen = sizeof(from); /* Size of socket */
  DWORD tick;                 /* Current tick used to determine time out */
  unsigned int timeOut;       /* Time out */
  BYTE crcA, crcB; /* CRC Bytes */

  returnValue = TRUE;
  if (addNonReliable == TRUE) {
    buff[(*len)] = UDP_NON_RELIABLE_PACKET;
    (*len)++;
  }
  if (wantCrc == TRUE) {
    CRCCalcBytes(buff, *len, &crcA, &crcB);
    buff[*len] = crcA;
    buff[(*len)+1] = crcB;
    (*len) += 2;
  }

  timeOut = 0;
  tick = winbolotimer();
  sendto(myUdpSock, (char *) buff, *len, 0, (struct sockaddr *)&addrServer, sizeof(addrServer));
  *len = 0;
  while (*len <= 0 && timeOut <= TIME_OUT) {
    *len = recvfrom(myUdpSock, (char *) buff, MAX_UDPPACKET_SIZE, 0, (struct sockaddr *)&from, &fromlen);
/*      if (*len == SOCKET_ERROR) {
      if (WSAGetLastError() != WSAEWOULDBLOCK ) { - Do we want to bail out? No
        MessageBoxA(NULL, "b", "bad socket error not WSAEWOULDBLOCK", MB_OK); (/
      } 
   } */
    timeOut = winbolotimer() - tick;
  }

  if (*len <= 0) {
    returnValue = FALSE;
  }
  return returnValue;
}

/*********************************************************
*NAME:          netClientUdpPing
*AUTHOR:        John Morrison
*CREATION DATE: 24/02/99
*LAST MODIFIED: 19/08/02
*PURPOSE:
* Sends a packet and waits till a repsonse packet arrives
* Returns FALSE if it times out.
*
*ARGUMENTS:
*  buff - Packet to send (also returned)
*  len  - Length of the packet (also returned)
*  dest - Destination address
*  port - Destination port
*  wantCrc - TRUE if we should add a CRC to this packet
*  addNoReliable - If true adds the non-reliable packet
*                  marker to the packet
*********************************************************/
bool netClientUdpPing(BYTE *buff, int *len, char *dest, unsigned short port, bool wantCrc, bool addNonReliable) {
  bool returnValue;           /* Value to return */
  struct sockaddr_in addr;    /* Socket to send to */
  struct sockaddr_in from;    /* Socket returned from */
  struct hostent *phe;        /* Used for DNS lookups */
  int fromlen = sizeof(from); /* Size of socket */
  DWORD tick;                 /* Current tick used to determine time out */
  unsigned int timeOut;       /* Time out */
  BYTE crcA, crcB; /* CRC Bytes */

  returnValue = TRUE;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(dest);
  if (addr.sin_addr.s_addr == INADDR_NONE) {
    /* Do hostname lookup */
    phe= gethostbyname(dest);
    if (phe == 0) {
      returnValue = FALSE;
    } else {
      addr.sin_addr.s_addr = *((u_long*)phe->h_addr_list[0]);
    }
  }
  if (returnValue == TRUE) {
    if (addNonReliable == TRUE) {
      buff[(*len)] = UDP_NON_RELIABLE_PACKET;
      (*len)++;
    }
    if (wantCrc == TRUE) {
      CRCCalcBytes(buff, (*len), &crcA, &crcB);
      buff[(*len)] = crcA;
      buff[(*len)+1] = crcB;
      (*len) += 2;
    }

    timeOut = 0;
    tick = winbolotimer();
    sendto(myUdpSock, (char *) buff, *len, 0, (struct sockaddr *)&addr, sizeof(addr));
    *len = 0;
    while (*len <= 0 && timeOut <= TIME_OUT) {
      *len = recvfrom(myUdpSock, (char *) buff, MAX_UDPPACKET_SIZE, 0, (struct sockaddr *)&from, &fromlen);
/*      if (*len == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK ) {
 Do we want to bail out? I don't think so
          MessageBoxA(NULL, "b", "bad", MB_OK); (/
        } 
     } */
      memcpy(&addrLast, &(from.sin_addr), sizeof(from.sin_addr));
      lastPort = from.sin_port;
      timeOut = winbolotimer() - tick;
    }

    if (*len <= 0) {
      returnValue = FALSE;
    }
  }
  return returnValue;
}

/*********************************************************
*NAME:          netClientUdpSendNoWait
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Sends a packet to the string address given
*
*ARGUMENTS:
*  buff - Packet to send
*  len  - Length of the packet
*  dest - Destination address
*  port - Destination port
*********************************************************/
void netClientSendUdpNoWait(BYTE *buff, int len, char *dest, unsigned short port) {
  struct sockaddr_in addr;    /* Socket to send to */

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(dest);
  sendto(myUdpSock, (char *) buff, len, 0, (struct sockaddr *)&addr, sizeof(addr));
}

/*********************************************************
*NAME:          netClientGetLastStr
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Copies the location of the last packet into a string
*
*ARGUMENTS:
*  dest - Destination string
*********************************************************/
void netClientGetLastStr(char *dest) {
  char *temp; /* Temp string pointer */

  temp = inet_ntoa(addrLast.sin_addr);
  strcpy(dest, temp);
}

/*********************************************************
*NAME:          netClientGetLast
*AUTHOR:        John Morrison
*CREATION DATE: 23/12/99
*LAST MODIFIED: 23/12/99
*PURPOSE:
* Copies the location of the last packet.
*
*ARGUMENTS:
*  dest - Destination.
*  port - Destination port.
*********************************************************/
void netClientGetLast(struct in_addr  *dest, unsigned short *port) {
  memcpy(dest, &addrLast, sizeof(addrLast));
  *port = lastPort;
}

/*********************************************************
*NAME:          netClientSetUseEvents
*AUTHOR:        John Morrison
*CREATION DATE: 5/6/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sets our netClient socket to use windows notificaion on 
* incoming packets. Returns sucess.
*
*ARGUMENTS:
*
*********************************************************/
bool netClientSetUseEvents(void) {
  bool returnValue; /* Value to return */
  int ret;          /* Function return */

  returnValue = TRUE;
  ret = 0;
/*  ret = WSAAsyncSelect( mySock, windowWnd(), WSA_READ, FD_READ); */
  if (ret < 0) {
    returnValue = FALSE;
    MessageBoxA(NULL, langGetText(STR_NETCLIENTERR_CHAINFAIL), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
  }
  return returnValue;
}

/*********************************************************
*NAME:          netClientUdpCheck
*AUTHOR:        John Morrison
*CREATION DATE: 28/5/00
*LAST MODIFIED: 28/5/00
*PURPOSE:
* Thread function responsible for checking for UDP Data.
*
*ARGUMENTS:
*
*********************************************************/
void netClientUdpCheck(void) {
  char info[MAX_UDPPACKET_SIZE];
  int packetLen;
  struct sockaddr_in from;
  int fromlen = sizeof(from);

  packetLen = recvfrom(myUdpSock, info, MAX_UDPPACKET_SIZE, 0, (struct sockaddr *)&from, &fromlen);
  while (packetLen > 0) {
    /* We have data - Yah! */
    memcpy(&addrLast, &from, (size_t) sizeof(from));
    lastPort = from.sin_port;
    netUdpPacketArrive((BYTE *) info, packetLen, lastPort);
    packetLen = recvfrom(myUdpSock, info, sizeof(info), 0, (struct sockaddr *)&from, &fromlen);
  }
}

/*********************************************************
*NAME:          netClientSetUdpAsync
*AUTHOR:        John Morrison
*CREATION DATE: 30/8/99
*LAST MODIFIED: 30/8/99
*PURPOSE:
* Sets the netClients UDP socket to be blocking/nonblocking
* Returns success.
*
*ARGUMENTS:
*  on - TRUE for non-block, FALSE for blocking
*********************************************************/
bool netClientSetUdpAsync(bool on) {
  u_long noBlock;   /* Used to set blocking state */
  int ret;          /* Function return value */
  bool returnValue; /* Value to return */
  
  returnValue = TRUE;
  if (on == TRUE) {
    noBlock = NO_BLOCK_SOCK;
  } else {
    noBlock = 0;
  }

  ret = ioctlsocket(myUdpSock, (long) FIONBIO, &noBlock);
  if (ret == SOCKET_ERROR) {
    returnValue = FALSE;
  }
  return returnValue;
}

/*********************************************************
*NAME:          netClientSetTracker
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Sets the trackers details up. Returns success.
*
*ARGUMENTS:
*  address - Address of the tracker
*  port    - Port of the tracker
*********************************************************/
bool netClientSetTracker(char *address, unsigned short port) {
  bool returnValue;    /* Value to return      */
  struct hostent *phe; /* Used for DNS lookups */
  
  returnValue = TRUE;
  addrTracker.sin_family = AF_INET;
  addrTracker.sin_port = htons(port);
  addrTracker.sin_addr.s_addr = inet_addr(address);
  if (addrTracker.sin_addr.s_addr == INADDR_NONE) {
    /* Not an IP Address. Do a hostname lookup */
    phe= gethostbyname(address);
    if (phe == 0) {
      returnValue = FALSE;
    } else {
      addrTracker.sin_addr.s_addr = *((u_long*)phe->h_addr_list[0]);
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          netClientSendUdpTracker
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
* Sends a udp packet to the tracker
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*********************************************************/
void netClientSendUdpTracker(BYTE *buff, int len) {
  sendto(myUdpSock, (char *) buff, len, 0, (struct sockaddr *)&addrTracker, sizeof(addrTracker));
}

//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------



int gameFinderGetParamFromLine(char *src, char *dest, char *pattern) {
  int returnValue; /* Value to return */
  char *ptr;
  int count;

  dest[0] = '\0';
  count = 0;
  returnValue = -1;
  if (strncmp(src, pattern, strlen(pattern)) == 0) {
    returnValue = (int) (strlen(pattern) + 1);
    ptr = src + returnValue;
    while (*ptr != '\n') {
      count++;
      ptr++;
    }
    strncat(dest, (char *) (src+returnValue), (size_t) count);
    strcat(dest, "\0");
    
  }

  return returnValue;
}


int gameFinderReplace(char *buff, int len) {
  int returnValue = -1;
  char *ptr = buff;
  while (strncmp(ptr, "\r\n", 2) != 0  && returnValue < len) {
    returnValue++;
    ptr++;
  }
  if (returnValue >= len) {
    returnValue = -1;
  } else {
    *ptr = '\0';
  }
  return returnValue+1;
}

char * gameFinderProcessMotd(char *buff, char *motd, int numLines) {
  int count;
  char *ptr;
  char param[256];

  ptr = buff;
  motd[0] = '\0';
  count = 0;
  while (count < numLines) {
    gameFinderGetParamFromLine(ptr, param, (char *) "MOTD");
    strcat(motd, param);
    strcat(motd, "\n");
    count++;
    ptr += gameFinderReplace(ptr, 20000);
    ptr++;
    ptr++;
  }
  return ptr;
}

unsigned short gameFrontGameLocation(char *argItem, char *trackerAddr) {
  char *tmp;
  tmp = strtok(argItem, ":");
  if (tmp != NULL) {
    strcpy(trackerAddr, tmp);
    tmp = strtok(NULL, ":");
    if (tmp != NULL) {
      return (unsigned short) atoi(tmp);
    }
  }
  return 0;
}


bool gameFinderYesNoToBool(char *str) {
  bool returnValue;
  returnValue = FALSE;
  if (str[0] == 'Y' || str[0] == 'y') {
    returnValue = TRUE;
  }
  return returnValue;
}

void gameFinderProcessV1(currentGames *cg, char *buff, int len, char *motd) {
  char param[256];
  char *ptr;
  char *ptr2;
  int numMotdLines = 0;
  int count = 0;
  int numGames;
  char gameNum[100];
  char gameId[100];
  char address[255];
  unsigned short port;
  char mapName[255];
  char version[255];
  gameType game;
  BYTE players;
  BYTE basesNum;
  BYTE pillsNum;
  bool password;
  bool hiddenMines;
  aiType ai;
  
  numGames = 0;  
  ptr = buff;
  ptr2 = buff;
  if (gameFinderGetParamFromLine(buff, param, (char *) "MOTDL") != -1) {
    /* Process Message of the day */
    numMotdLines = atoi(param);
    ptr += gameFinderReplace(ptr, 20000);
    ptr++;
    ptr++;
    ptr = gameFinderProcessMotd(ptr, motd, numMotdLines);
  }

  if (gameFinderGetParamFromLine(ptr, param, (char *) "NGAMES") != -1) {
    numGames = atoi(param);
  } else {
    numGames = 0;
  }
  if (numGames > 0) {
    ptr += gameFinderReplace(ptr, 20000);
    ptr++;
    ptr++;
    while (count < numGames) {
      sprintf(gameNum, "%.03d", count);
      strcpy(gameId, "GAME");
      strcat(gameId, gameNum);
      gameFinderGetParamFromLine(ptr, param, gameId);
      port = gameFrontGameLocation(param, address);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, version, (char *) "VERSION");
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, mapName, (char *) "MAP");
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, (char *) "TYPE");
      if (param[0] == 'o' || param[0] == 'O') {
        game = gameOpen;
      } else if (param[0] == 't' || param[0] == 'T') {
        game = gameTournament;
      } else {
	      game = gameStrictTournament;
      }
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, (char *) "PLAYERS");
      players = (BYTE) atoi(param);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, (char *) "BASES");
      basesNum = (BYTE) atoi(param);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, (char *) "PILLS");
      pillsNum = (BYTE) atoi(param);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;

      gameFinderGetParamFromLine(ptr, param, (char *) "HIDMINES");
      hiddenMines = gameFinderYesNoToBool(param);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, (char *) "PASSWORD");
      password = gameFinderYesNoToBool(param);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;

      gameFinderGetParamFromLine(ptr, param, (char *) "BRAINS");
      if (param[0] == 'n' || param[0] == 'N') {
        ai = aiNone;
      } else if (strlen(param) > 3) {
        ai = aiYesAdvantage;
      } else {
        ai = aiYes;
      }
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;

      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;

      /* Add it */
      currentGamesAddItem(cg, address, port, mapName, version, players, basesNum, pillsNum, hiddenMines, game, ai, password);
      count++;
    }
  }
  
}


void gameFinderProcess(HWND hWnd, currentGames *cg, char *buff, int len, char *motd) {
  char *ptr;
  char *ptr2;
  char param[256];
  int amount;
  int version;
  ptr2 =ptr = buff;
  amount = gameFinderGetParamFromLine(buff, param, (char *) "TVERSION");
  ptr2 += amount;

  amount = gameFinderReplace(ptr, 20000);
  ptr += amount;
  ptr++;
  ptr++;
  
  version = atoi(ptr2);
  switch (version) {
    case 1:
      gameFinderProcessV1(cg, ptr, 20000, motd);
      break;
    default:
      MessageBoxA(hWnd, langGetText(STR_NETCLIENTERR_TRACKERVERSIONFAIL), DIALOG_BOX_TITLE, MB_OK);
  }
}



void gameFinderProcessBroadcast(currentGames *cg, INFO_PACKET *info, struct in_addr *pack) {
  char mapName[MAP_STR_SIZE];
  char version[MAP_STR_SIZE];
  bool password;
  bool mines;
  char address[FILENAME_MAX];

  utilPtoCString(info->mapname, mapName);
  password = TRUE;
  if (info->has_password == 0) {
    password = FALSE;
  }
  mines = FALSE;
  if (info->allow_mines & 0x80) {
    mines = TRUE;
  }
  if (info->gameid.serveraddress.s_addr == 0) {
    strcpy(address, inet_ntoa(*pack));
  } else {
    strcpy(address, inet_ntoa(info->gameid.serveraddress));
  }
  sprintf(version, "%d.%d%d", info->h.versionMajor, info->h.versionMinor, info->h.versionRevision);
  currentGamesAddItem(cg, address, ntohs(info->gameid.serverport), mapName, version, (BYTE) info->num_players, (BYTE) info->free_bases, (BYTE) info->free_pills, mines, (gameType) info->gametype, (aiType) info->allow_AI, password);

}

//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------

/*********************************************************
*NAME:          netClientFindTrackedGames
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/00
*LAST MODIFIED: 23/1/00
*PURPOSE:
* Looks up the tracker address and ports and downloads a
* list of current games. Returns success
*
*ARGUMENTS:
*  hWnd           - Window handle of the calling dialog
*  cg             - Pointer to the currentGames structure
*  trackerAddress - Address of the tracker to use
*  port           - Port of the tracker
*  motd           - Buffer to hold the message of the day
*********************************************************/
bool netClientFindTrackedGames(HWND hWnd, currentGames *cg, char *trackerAddress, unsigned short port, char *motd) {
  bool returnValue;        /* Value to return */
  WSADATA wsaData;         /* Winsock create data */
  int ret;                 /* Function returns */
  struct sockaddr_in con;  /* Address to connect to */
  SOCKET sock;             /* Socket to use    */
  struct hostent *phe;     /* Used for DNS lookups */
  BYTE *buff;         /* Results buffer */
  BYTE *ptr;               /* Buffer pointer */
  int len = 0;
  int new_bytes_read;
  char strBuff[FILENAME_MAX];
  u_long noBlock;     /* Used to set blocking state */
  bool done;
  DWORD tick, timeOut;
  

  buff = malloc(1024 * 1024);
  sock = INVALID_SOCKET;
  returnValue = TRUE;
  ptr = buff;
  

  strcpy((char *) buff, langGetText(STR_NETCLIENT_TRACKERCONNECT));
  sprintf(strBuff, "%s:%d", trackerAddress, port);
  strcat((char *) buff, strBuff);
  SetDlgItemTextA(hWnd, IDC_STATUS, (LPCTSTR) buff); 

  /* Startup Winsock */
  ret = WSAStartup(MAKEWORD(2,0), &wsaData);
  if (ret != 0) {
    SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENTERR_WINSOCKFAILSTARTUP)); 
    returnValue = FALSE;
  }

  /* Create TCP Socket*/
  if (returnValue == TRUE) {
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
      returnValue = FALSE;
      SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENTERR_CREATETCPFAIL)); 
    }
  }

  if (returnValue == TRUE) {
    con.sin_family = AF_INET;
    con.sin_port = htons(port);
    con.sin_addr.s_addr = inet_addr(trackerAddress);
    if (con.sin_addr.s_addr == INADDR_NONE) {
      /* Not an IP Address. Do a hostname lookup */
      phe = gethostbyname(trackerAddress);
      if (phe == 0) {
        returnValue = FALSE;
        SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENTERR_TRACKERDNSFAIL)); 
      } else {
        con.sin_addr.s_addr = *((u_long*)phe->h_addr_list[0]);
      }
    }
  }

 

  /* Connect */
  if (returnValue == TRUE) {
    ret = connect(sock, (struct sockaddr *) &con, sizeof(con));
    if (ret == SOCKET_ERROR) {
      SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENTERR_TRACKERCONNECTFAIL)); 
      returnValue = FALSE;
    }
  }

  noBlock = NO_BLOCK_SOCK;
  if (returnValue == TRUE) {
    ret = ioctlsocket(sock, (long) FIONBIO, &noBlock);
    if (ret == SOCKET_ERROR) {
      SetDlgItemTextA(hWnd, IDC_STATUS, "Error setting socket to non blocking mode."); 
      returnValue = FALSE;
    }
  }


  if (returnValue == TRUE) {
    SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENT_TRACKERGETRESPONSE));
    len = 0;
    ptr = buff;
    new_bytes_read = recv(sock, (char *) (ptr+len), (int) ((int) ((1024 * 1024))-len), 0);
    tick = winbolotimer();
    timeOut = 0;
    done = FALSE;
    while (timeOut <= 10000 && done == FALSE) {
      if (new_bytes_read > 0) {
        len += new_bytes_read;
      } else if (new_bytes_read == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
          done = TRUE;
        }
      } else if (new_bytes_read == 0) {
        done = TRUE;
      }
      timeOut = winbolotimer() - tick;
      new_bytes_read = recv(sock, (char *) (ptr+len), (int) ((int) ((1024 * 1024))-len), (int) 0);
    } 

    /*new_bytes_read = recv(sock, ptr+len, sizeof(buff)-len, 0);
    while (new_bytes_read > 0) {
      len += new_bytes_read;
      new_bytes_read = recv(sock, ptr+len, sizeof(buff)-len, 0);
    } */
    /* Process it */
    if (len == 0) {
      SetDlgItemTextA(hWnd, IDC_STATUS, (LPCTSTR) langGetText(STR_NETCLIENTERR_TRACKERNODATA));
      returnValue = FALSE;
    } else {
      SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENT_TRACKERPROCESSRESPONSE));
      gameFinderProcess(hWnd, cg, (char *) buff, len, motd);
    }
  }

  free(buff);
  if (sock != INVALID_SOCKET) {
    shutdown(sock, SD_BOTH);  
    closesocket(sock);
  }
  WSACleanup();
  return returnValue;
}

/*********************************************************
*NAME:          netClientFindBroadcastGames
*AUTHOR:        John Morrison
*CREATION DATE: 4/6/00
*LAST MODIFIED: 4/6/00
*PURPOSE:
* Gets a list of current games on you by using your 
* broadcast address.
*
*ARGUMENTS:
*  hWnd           - Window handle of the calling dialog
*  cg             - Pointer to the currentGames structure
*********************************************************/
bool netClientFindBroadcastGames(HWND hWnd, currentGames *cg) {
  bool returnValue;        /* Value to return */
  WSADATA wsaData;         /* Winsock create data */
  int ret;                 /* Function returns */
  struct sockaddr_in con;  /* Address to connect to */
  struct sockaddr_in from; /* Address the packet came from .*/
  struct sockaddr_in addr; /* Address to bind to  */
  SOCKET sock;             /* Socket to use    */
  BYTE *ptr;               /* Buffer pointer */
  int len = 0;
  char buff[MAX_UDPPACKET_SIZE] = INFOREQUESTHEADER; /* Data Buffer */
  u_long noBlock;     /* Used to set blocking state */
  unsigned int timeOut;
  DWORD tick;
  int fromlen;
  struct sockaddr_in last;
  int szlast;

  szlast = sizeof(last);
 
  
  sock = INVALID_SOCKET;
  returnValue = TRUE;
  ptr = buff;
  

  /* Startup Winsock */
  ret = WSAStartup(MAKEWORD(2,0), &wsaData);
  if (ret != 0) {
    SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENTERR_WINSOCKFAILSTARTUP)); 
    returnValue = FALSE;
  }


  /* Create UDP Socket*/
  if (returnValue == TRUE) {
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
      returnValue = FALSE;
      SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENTERR_CREATEUDPFAIL)); 
    }
  }

  /* Bind to port */
  if (returnValue == TRUE) {
    addr.sin_family = AF_INET;
    addr.sin_port = INADDR_ANY;
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret != 0) {
      returnValue = FALSE;
      MessageBoxA(NULL, langGetText(STR_NETCLIENTERR_BINDUDPFAIL), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
    }
  }

  /* Set to non blocking */
  if (returnValue == TRUE) {
    noBlock = NO_BLOCK_SOCK;
    ret = ioctlsocket(sock, (long) FIONBIO, &noBlock);
    if (ret == SOCKET_ERROR) {
      returnValue = FALSE;
      SetDlgItemTextA(hWnd, IDC_STATUS, "Error setting socket options"); 
    }
  }
  /* Broadcast socket option */
  if (returnValue == TRUE) {
    char c[5];
    strcpy(c, "true");
    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, c ,sizeof(c));
    if (ret != 0) {
      returnValue = FALSE;
      SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENTERR_TRACKERNOBLOCK)); 
    }
  }

  /* Do Broadcast */
  if (returnValue == TRUE) {

    con.sin_family = AF_INET;
    con.sin_port = htons(27500); /* Fixme: Run time settable? */
    con.sin_addr.s_addr = INADDR_BROADCAST;
    ret = sendto(sock, buff, BOLOPACKET_REQUEST_SIZE, 0, (struct sockaddr *)&con, sizeof(con));
    if (ret == SOCKET_ERROR) {
      SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENTERR_BROADCAST)); 
      returnValue = FALSE;
    }
  }

  Sleep(50);
  if (returnValue == TRUE) {
    /* Get responses */
    SetDlgItemTextA(hWnd, IDC_STATUS, langGetText(STR_NETCLIENT_GETRESPONSES));
    szlast = sizeof(last);
    len = recvfrom(sock, (char *) (ptr+len), (int) ((int) (sizeof(buff))-len), (int)(0), (struct sockaddr *) &last, &szlast);
    timeOut = 0;
    tick = winbolotimer();

    while (timeOut <= 10000) { //TIME_OUT
      if (len == (int) sizeof(INFO_PACKET)) {
        if (strncmp(buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0 && buff[BOLO_VERSION_MAJORPOS] == BOLO_VERSION_MAJOR && buff[BOLO_VERSION_MINORPOS] == BOLO_VERSION_MINOR && buff[BOLO_VERSION_REVISIONPOS] == BOLO_VERSION_REVISION && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_INFORESPONSE) {
        /* Process response */
          gameFinderProcessBroadcast(cg, (INFO_PACKET *) buff, &(last.sin_addr));
        }
      }
      Sleep(50);
      timeOut = winbolotimer() - tick;
      fromlen = sizeof(from); 
      len = recvfrom(sock, buff, MAX_UDPPACKET_SIZE, 0, (struct sockaddr *)&from, &fromlen);
    }
  }

  if (sock != INVALID_SOCKET) {
    shutdown(sock, SD_BOTH);  
    closesocket(sock);
  }
  WSACleanup();
  return returnValue;
}

