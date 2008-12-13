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
*Name:          Server Transport
*Filename:      serverTransport.c
*Author:        John Morrison
*Creation Date: 11/8/99
*Last Modified: 12/1/00
*Purpose:
*  WinBolo Server Transport layer.
*********************************************************/

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
  #include <winsock2.h>
  #include <windows.h>
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define closesocket(X) close(X)
typedef struct hostent  HOSTENT;
typedef int SOCKET;
#define SD_BOTH 2

#endif
#include "../bolo/global.h"
#include "../bolo/crc.h"
#include "threads.h"
#include "servernet.h"
#include "servertransport.h"
#include "servercore.h"


SOCKET sockUdp = INVALID_SOCKET; /* Our UDP socket */

struct sockaddr_in addrUs;      /* Our own machine */
unsigned short myPort;          /* Our UDP Port number */
struct sockaddr_in addrLast;    /* Where the last packet came from */
struct sockaddr_in addrTracker; /* Tracker machine */


static unsigned long getaddrbyany(char *sp_name)  {               
  struct hostent *sp_he;
  int i; 

  if(isdigit(*sp_name))
    return inet_addr(sp_name);
  for(i=0;i<100;i++) {                       
    sp_he = gethostbyname(sp_name);
    if(!(sp_he)) {
      sleep(1);
     if(i>=3) {
       return(0);
     }
   } else break;   
  }

  return sp_he ? *(long*)*sp_he->h_addr_list : 0;
}   

/*********************************************************
*NAME:          serverTransportCreate
*AUTHOR:        John Morrison
*CREATION DATE: 11/8/99
*LAST MODIFIED: 10/9/00
*PURPOSE:
*  Creates the new servers transport layer. Returns FALSE 
*  if an error occured
*
*ARGUMENTS:
* port      - Port to run server on.
* addrToUse - The address to use if not NULL
*********************************************************/
bool serverTransportCreate(unsigned short port, char *addrToUse) {
  bool returnValue = TRUE; /* Value to return */
  int ret;                 /* Function returns */
  struct sockaddr_in addr; /* Socket structure */
  u_long noBlock;            /* Used to set blocking state */
  unsigned long ad;        /* Address */

  screenServerConsoleMessage((char *) "Server Transport Startup");

  if (returnValue == TRUE) {
    myPort = port;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (addrToUse != NULL) {
      ad = getaddrbyany(addrToUse);
      if (ad > 0) {
        addr.sin_addr.s_addr = ad;
      } 
    }
  }

  /* Create Sockets */
  if (returnValue == TRUE) {    
    sockUdp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockUdp == INVALID_SOCKET) {
      fprintf(stderr, "Error Creating UDP Socket\n");
      returnValue = FALSE;
    }
  }

  /* Binding */
  if (returnValue == TRUE) {
    ret = bind(sockUdp, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
      fprintf(stderr, "Error Binding UDP Socket\n");
      returnValue = FALSE;
    }
  }
  

  /* Set to non blocking */
  if (returnValue == TRUE) {
    noBlock = NO_BLOCK_SOCK;
    ret = fcntl(sockUdp, F_SETFL, O_NONBLOCK | fcntl(sockUdp, F_GETFL));
    if (ret == SOCKET_ERROR) {
      returnValue = FALSE;
    }

  }

  return returnValue;
}

/*********************************************************
*NAME:          serverTransportDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 11/8/99
*LAST MODIFIED: 11/8/99
*PURPOSE:
*  Shuts down the server transport subsystem.
*
*ARGUMENTS:
*
*********************************************************/
void serverTransportDestroy() {

  screenServerConsoleMessage((char *) "Server Transport Shutdown");
  shutdown(sockUdp, SD_BOTH);
  closesocket(sockUdp);
  sockUdp = INVALID_SOCKET;
}


/*********************************************************
*NAME:          serverTransportListenUDP
*AUTHOR:        John Morrison
*CREATION DATE: 13/8/99
*LAST MODIFIED: 13/8/99
*PURPOSE:
*  Listens for incoming UDP packets and processes them. 
*  Function exits upon an error caused by the sockUdp 
*  being closed.
*
*ARGUMENTS:
*
*********************************************************/
void serverTransportListenUDP(void) {
  BYTE info[512 /*MAX_PACKET_SIZE */];  /* The packet                 */
  int packetLen;               /* Size of the packet         */
  struct sockaddr_in from;     /* Where the packet came from */
  int fromlen;                 /* size of the from struct    */
  
  fromlen = sizeof(from);
  packetLen = recvfrom(sockUdp, info, 512 /* MAX_PACKET_SIZE*/ , 0, (struct sockaddr *)&from, &fromlen);
  while (packetLen != SOCKET_ERROR) {
     /* We have data - Yah! */
    memcpy(&addrLast, &from, (size_t) sizeof(from));
    serverNetUDPPacketArrive(info, packetLen, from.sin_addr.s_addr,  from.sin_port);
    /* Process it and await more data */
    fromlen = sizeof(from);
    packetLen = recvfrom(sockUdp, info, 512 /*MAX_PACKET_SIZE*/, 0, (struct sockaddr *)&from, &fromlen);
  }
}

/*********************************************************
*NAME:          serverTransportSetUs
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Sets up our IP address.
*
*ARGUMENTS:
*
*********************************************************/
void serverTransportSetUs(void) {
  char str[FILENAME_MAX]; /* Temp string to hold host name  & IP as string*/
  HOSTENT  *hd;           /* Host Data */
  int ipa;               /* Bytes for each IP address components */
  int  ipb;
  int  ipc;
  int  ipd;

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
  sprintf(str, "%d.%d.%d.%d\0",ipa,ipb,ipc,ipd);
  addrUs.sin_addr.s_addr = inet_addr(str);
  addrUs.sin_port = myPort;
}

/*********************************************************
*NAME:          serverTransportGetUs
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 15/8/99
*PURPOSE:
* Gets our IP address and port.
*
*ARGUMENTS:
*  dest - Destination address
*  port - Destination port
*********************************************************/
void serverTransportGetUs(struct in_addr *dest, unsigned short *port) {
  memcpy(dest, &(addrUs.sin_addr), (size_t) sizeof(*dest));
  *port = myPort;
}

/*********************************************************
*NAME:          serverTransportSendUDPLast
*AUTHOR:        John Morrison
*CREATION DATE: 15/08/99
*LAST MODIFIED: 19/08/02
*PURPOSE:
* Sends a packet to the last UDP connection made.
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*  wantCrc - Should we add a CRC to this?
*********************************************************/
void serverTransportSendUDPLast(BYTE *buff, int len, bool wantCrc) {
  BYTE crcA, crcB;
  int i;

  if (wantCrc == TRUE) {
    CRCCalcBytes(buff, len, &crcA, &crcB);
    buff[len] = crcA;
    buff[len+1] = crcB;
    len+=2;
  }
  i = sendto(sockUdp, (char *) buff, len, 0, (struct sockaddr *)&addrLast, sizeof(addrLast));
}

// FIXME: Prototype
void serverTransportSendUDP(BYTE *buff, int len, struct sockaddr_in *addr) {
  sendto(sockUdp, (char *) buff, len, 0, (struct sockaddr *) addr, sizeof(*addr));

}


/*********************************************************
*NAME:          serverTransportSetTracker
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
bool serverTransportSetTracker(char *address, unsigned short port) {
  bool returnValue;    /* Value to return      */
  struct hostent *phe; /* Used for DNS lookups */
  
  returnValue = TRUE;
  addrTracker.sin_family = AF_INET;
  addrTracker.sin_port = htons(port);
  addrTracker.sin_addr.s_addr = inet_addr(address);
  if (addrTracker.sin_addr.s_addr == INADDR_NONE) {
    /* Not an IP Address. Do a hostname lookup */
    phe = gethostbyname(address);
    if (phe == 0) {
      returnValue = FALSE;
    } else {
      addrTracker.sin_addr.s_addr = *((u_long*)phe->h_addr_list[0]);
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          serverTransportSendUdpTracker
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
void serverTransportSendUdpTracker(BYTE *buff, int len) {
  sendto(sockUdp, (char *) buff, len, 0, (struct sockaddr *)&addrTracker, sizeof(addrTracker));
}


void serverTransportDoChecks() {
  serverTransportListenUDP();
  return;
}
