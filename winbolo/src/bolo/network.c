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
*Filename:      network.c
*Author:        John Morrison
*Creation Date: 21/02/99
*Last Modified: 09/12/03
*Purpose:
*  Backend network packet handler
*********************************************************/

#ifdef _WIN32
#include <windows.h>
  HWND dlgAllianceWnd;
  #include "../gui/gamefront.h"
  #include "../gui/winbolo.h"
  #include "../gui/brainshandler.h"
  #include "../gui/dialogalliance.h"
#else
  #include "SDL.h"
  #include <gtk/gtk.h>
  #include "../gui/linux/messagebox.h"
  #include "../gui/linux/dialogalliance.h"
  GtkWidget *dlgAllianceWnd;

  extern char messageBody[16*1024];
  extern char messageTitle[256];
  extern bool hasMessage;
  typedef int DWORD;
#endif

#include "../gui/dnsLookups.h"
#include <time.h>
#include <string.h>
#include "global.h"
#include "../gui/netclient.h"
#include "screen.h"
#include "../gui/clientmutex.h"
#include "util.h"
#include "players.h"
#include "crc.h"
#include "netpnb.h"
#include "netmt.h"
#include "messages.h"
#include "netpacks.h"
#include "backend.h"
#include "log.h"
#include "../server/BigDigits/bigd.h"
#include "../server/rsaalgorithm.h"

#ifdef _WIN32
  #include "../gui/resource.h"
#else
  #include "../gui/linresource.h"
#endif

#include "../gui/lang.h"
#include "udppackets.h"
#include "../winbolonet/winbolonet.h"
#include "network.h"

/* The type of game we are playing */
netType networkGameType = netNone;
/* Password in netgames */
char netPassword[MAP_STR_SIZE];
// randomly generated string
char rsastring[256];

netStatus netStat = netRunning; /* Network status */
netStatus oldNetStatus = netRunning; /* Network status */
BYTE netFailedHigh = 0xFF;
BYTE netYPos; /* Y Position we want for data downloading */

/* Data for network status dialog box */
int netRingDelay = 0;           /* Total Ring delay */
int netDownStreamDelay = 0;     /* Downstream time delay */
int netPacketsPerSecond = 0;    /* Number of packets per second */
int netNumErrors = 0;           /* Number of network Errors */
int netRetransmissions = 0;     /* Number of network Retransmissions */

/* And Totals of for the last second */
int netTotPacketsPerSecond = 0; /* Number of packets per second */

long netLastTokenTime = 0;
long tknTime;                              /* Time since last token */

bool netUseTracker; /* Do we use the tracker or not */
bool inNetShutdown; /* Are we shutting down - Don't send packets */

time_t netLastHeard; /* Last time we got a packet from the server */

/* Udp packets */
udpPackets udpp;

/* Maximum retries for network things */
#define MAX_RETRIES 3
/* Network time */
DWORD dwSysNetTotal = 0;
DWORD dwSysNet = 0;

int lzwdecoding(char *src, char *dest, int len);

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
bool netSetup(netType value, unsigned short myPort, char *targetIp, unsigned short targetPort, char *password, bool usCreate, char *trackerAddr, unsigned short trackerPort, bool useTracker, bool wantRejoin, bool useWinboloNet, char *wbnPassword) {
  bool returnValue; /* Value to return */
  time_t startTime; /* Start Time if we created the game */

  returnValue = TRUE;
  inNetShutdown = FALSE;
  netRetransmissions = 0;
  netStat = netRunning;
  startTime = 0;
  networkGameType = value;
  strcpy(netPassword, password);
  udpp = udpPacketsCreate();
  #ifdef _WIN32
  dlgAllianceWnd = CreateDialog(windowGetInstance(), MAKEINTRESOURCE(IDD_ALLIANCE), windowWnd(), dialogAllianceCallback);
  #else
  dlgAllianceWnd = dialogAllianceCreate();
  #endif
  if (networkGameType == netUdp) {
    netStat = netJoining;
    returnValue = netClientCreate(myPort);
    if (returnValue == TRUE) {
      returnValue = dnsLookupsCreate();
    }
    if (returnValue == TRUE) {
      netClientSetUs();

      /* Try and set the tracker */
      netUseTracker = useTracker;
      if (useTracker == TRUE) {
        if (netClientSetTracker(trackerAddr, trackerPort) == FALSE) {
          #ifdef _WIN32
          MessageBox(NULL, langGetText(NETERR_TRACKERDNS), DIALOG_BOX_TITLE, MB_OK);
          #else 
          MessageBox(langGetText(NETERR_TRACKERDNS), DIALOG_BOX_TITLE);
          #endif
          netUseTracker = FALSE;
        }
      }

/*      if (usCreate == TRUE) {
        netYPos = 241;
        startTime = time(NULL);
        screenSetTimeGameCreated(startTime);
        /*FIXME: netClientSetServerUs(); *
        netStat = netRunning;
      } else { */
       /* Try and join the game */
      {
        char playerName[FILENAME_MAX];     /* Player Name Storage */
	
        gameFrontGetPlayerName(playerName);
        netYPos = 21;
        returnValue = netJoin(targetIp, targetPort, wantRejoin, playerName, wbnPassword, useWinboloNet, usCreate, password);
	      if (returnValue == TRUE) {
          screenNetSetupTank(TRUE);
          netStat = netStartDownload;
	      }
      }
    }
  }

  if (returnValue == FALSE) {
    netDestroy();
  } else {
    /* Do the final setup */
    if (networkGameType == netUdp) {
      returnValue = netClientSetUdpAsync(TRUE);
      netJoinDataRequests();
      netLastHeard = time(NULL);
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          netDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 19/4/99
*PURPOSE:
* Shuts down the network. Calls low-level packet drivers
* and shuts them down as required.
*
*ARGUMENTS:
*
*********************************************************/
void netDestroy(void) {
  bool windowDestroyed = FALSE; /* Variable to see if the window has been closed properly */
  WPARAM wParam = MAKEWPARAM(IDCANCEL,IDCANCEL); /* Create a WPARAM variable that will tell the window to close */
  LPARAM lParam = MAKELPARAM(0,0); /* Create a WPARAM variable that will tell the window to close */
  if (dlgAllianceWnd != NULL) {
#ifdef _WIN32
	/* Should return nonzero if successful.  Was not working so let's just close it instead. */
    /* windowDestroyed = DestroyWindow(dlgAllianceWnd); */
	windowDestroyed = dialogAllianceCallback(dlgAllianceWnd, WM_COMMAND, wParam, lParam);
#else 
    gtk_widget_destroy(dlgAllianceWnd);
#endif
    dlgAllianceWnd = NULL;
  }
  if (networkGameType == netUdp) {
    inNetShutdown = TRUE;
    netSendQuitMessage();
    networkGameType = netSingle;
    dnsLookupsDestroy();
    netClientDestroy();
  }
  networkGameType = netSingle;
  netStat = netRunning;
  udpPacketsDestroy(&udpp);
}

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
netType netGetType(void) {
  return networkGameType;
}

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
void netUdpPacketArrive(BYTE *buff, int len, unsigned short port) {
  static BYTE info[MAX_UDPPACKET_SIZE] = GENERICHEADER; /* Buffer to send */
  static bool inFix = FALSE;
  static BYTE crcA;  /* First CRC Byte */
  static BYTE crcB;  /* Second CRC Byte */
  static BYTE sequenceNumber; /* Sequence number */
  static int delete = 0;
  DWORD tick;     /* Number of ticks passed */

  
  tick = winboloTimer();
  
  if ((strncmp((char *) buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) != 0) || buff[BOLO_VERSION_MAJORPOS] != BOLO_VERSION_MAJOR || buff[BOLO_VERSION_MINORPOS] != BOLO_VERSION_MINOR || buff[BOLO_VERSION_REVISIONPOS] != BOLO_VERSION_REVISION) {
    /* error */
    netNumErrors++;
    dwSysNet += winboloTimer() - tick;
    return;
  }

  if (len == (int) sizeof(PING_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PINGRESPONSE) {
    /* Ping packet */
    PING_PACKET *pp;
    int pos;
	  bool okFix = TRUE;

    netLastHeard = time(NULL);
    pp = (PING_PACKET *) buff;
    if (pp->from == playersGetSelf(screenGetPlayers())) {
      if ((long) (winboloTimer() - tknTime) >= 0) {
        netRingDelay = (int) (winboloTimer() - tknTime);
      }
	    /* Recovery Checks */
      pos = 1 + udpPacketsGetOutSequenceNumber(&udpp);
      if (pos == MAX_UDP_SEQUENCE) {
        pos = 0;
      }

      if (pp->inPacket != pos) {
        /* Recover - Send server missing packets */
        BYTE upto;
        BYTE high;
        upto = pp->inPacket;
        high = udpPacketsGetOutSequenceNumber(&udpp);
        high++;
        if (high == MAX_UDP_SEQUENCE) {
          high = 0;
        }
		    okFix = FALSE;
/*          while (upto != high) {
            netClientSendUdpServer(udpPacketsGetOutBuff(&udpp, upto), udpPacketsGetOutBuffLength(&udpp, upto));
	    printf("Sending %d of %d size = %d", upto, high, udpPacketsGetOutBuffLength(&udpp, upto));
            upto++;
            if (upto == MAX_UDP_SEQUENCE) {
              upto = 0;
            }
          }*/
      }
    	pos = 1 + pp->outPacket;
      if (pos == MAX_UDP_SEQUENCE) {
        pos = 0;
      }
      if (udpPacketsGetInSequenceNumber(&udpp) != pos) {
        /* Recover */
        REREQUEST_PACKET rrp;
		    okFix = FALSE;
        if (netStat != netFailed) {
          oldNetStatus = netStat;
          netStat = netFailed;
          netMakePacketHeader(&(rrp.h), BOLOPACKET_PACKETREREQUEST);
  	      netFailedHigh = pp->outPacket;
          rrp.nonReliable = UDP_NON_RELIABLE_PACKET;
          rrp.rerequestItem = udpPacketsGetInSequenceNumber(&udpp); 
          CRCCalcBytes((BYTE *) &rrp, sizeof(REREQUEST_PACKET)-2 , &(rrp.crcA), &(rrp.crcB));
          netClientSendUdpServer((BYTE *) &rrp, sizeof(rrp));
        }
      }
    }
	  if (okFix == TRUE && netStat == netFailed) {
        netStat = oldNetStatus;
	  }
  } else if (len == BOLOPACKET_REQUEST_SIZE && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_INFOREQUEST) {
   /* Make info response packet */
    INFO_PACKET h;
    memset(&h, 0, sizeof(h));
    netMakeInfoRespsonse(&h);
    memcpy(info, &h, sizeof(h));
    netClientSendUdpLast((BYTE *) info, sizeof(h));
    netPacketsPerSecond++;
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPOSITION_DATA) {
    /* It must be an position and stuff packet */
    /* Set last to be server */
    netDataPosPacket(buff, len, port);
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PACKETREREQUEST && len == sizeof(REREQUEST_PACKET)) {
    REREQUEST_PACKET rrp;

    memcpy(&rrp, buff, sizeof(rrp));
    if (CRCCheck(buff, sizeof(REREQUEST_PACKET)-2, rrp.crcA, rrp.crcB) == TRUE) {
      BYTE upto;
      BYTE high;
      BYTE count = 0;
      unsigned short us;
      upto = buff[BOLOPACKET_REQUEST_TYPEPOS+1];
      high = udpPacketsGetOutSequenceNumber(&udpp);
      high++;
	    len = BOLOPACKET_REQUEST_TYPEPOS+2 ;
      if (high == MAX_UDP_SEQUENCE) {
        high = 0;
      }
      info[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_RETRANSMITTED_PACKETS;
      while (upto != high && len < 1000) {
        if (len + udpPacketsGetOutBuffLength(&udpp, upto) > 1000) {
          upto = high;
        } else {
          /* Add it to our packet */
          us = udpPacketsGetOutBuffLength(&udpp, upto);
          memcpy(info+len, &us, 2);
          len += 2;
          memcpy(info+len, udpPacketsGetOutBuff(&udpp, upto), us);
          len += us;
          upto++;
          if (upto == MAX_UDP_SEQUENCE) {
            upto = 0;
          }
	      count++;
        }
      }
      info[BOLOPACKET_REQUEST_TYPEPOS+1] = count;
	    CRCCalcBytes(info, len, (info+len), (info+len+1));
      netClientSendUdpServer(info, len+2);
    }
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_RETRANSMITTED_PACKETS) {
    BYTE count;
    unsigned short us;
    int pos;
    if (netStat == netFailed) {
	 	  if (CRCCheck(buff, len-2, buff[len-2], buff[len-1]) == TRUE) {
        count = 0;
        pos = BOLOPACKET_REQUEST_TYPEPOS+2;
        inFix = TRUE;
        while (count < buff[BOLOPACKET_REQUEST_TYPEPOS+1]) {
          memcpy(&us, buff+pos, 2);
          pos += 2;
          netUdpPacketArrive(buff+pos, us, 0);
          pos += us;
          count++;
        }
        inFix = FALSE;
      } 
    }
  } else {
    netLastHeard = time(NULL);
    crcA = buff[len-2];
    crcB = buff[len-1];
    if (CRCCheck(buff, len - BOLO_PACKET_CRC_SIZE, crcA, crcB) == TRUE) {
      len -= 2;
      sequenceNumber = buff[len-1];
      if (netStat != netFailed) {
        oldNetStatus = netStat;
      }
      if (sequenceNumber != udpPacketsGetInSequenceNumber(&udpp)) {
        netStat = netFailed;
        netNumErrors++;
    	  netFailedHigh = sequenceNumber;
        if (inFix == FALSE && (sequenceNumber > udpPacketsGetInSequenceNumber(&udpp) || (udpPacketsGetInSequenceNumber(&udpp) > 180 && (udpPacketsGetInSequenceNumber(&udpp) - sequenceNumber > 10)))) {
          /* We want to ignore resends if the packet reappears */
          REREQUEST_PACKET rrp;
          netMakePacketHeader(&(rrp.h), BOLOPACKET_PACKETREREQUEST);
          rrp.nonReliable = UDP_NON_RELIABLE_PACKET;
          rrp.rerequestItem = udpPacketsGetInSequenceNumber(&udpp);           
          CRCCalcBytes((BYTE *) &rrp, sizeof(REREQUEST_PACKET)-2 , &(rrp.crcA), &(rrp.crcB));
          netClientSendUdpServer((BYTE *) &rrp, sizeof(rrp)); 
        }
      } else {
        if (netStat == netFailed) {
          if (netFailedHigh == sequenceNumber) {
            /* Back up to speed */	
            netStat = oldNetStatus;
    	    } else if (inFix == FALSE) {
            /* Request Next */
            REREQUEST_PACKET rrp;
            netMakePacketHeader(&(rrp.h), BOLOPACKET_PACKETREREQUEST);
            rrp.nonReliable = UDP_NON_RELIABLE_PACKET;
            rrp.rerequestItem = udpPacketsGetInSequenceNumber(&udpp);        
            CRCCalcBytes((BYTE *) &rrp, sizeof(REREQUEST_PACKET)-2 , &(rrp.crcA), &(rrp.crcB));
            netClientSendUdpServer((BYTE *) &rrp, sizeof(rrp));
	        }
        }
        udpPacketsGetNextInSequenceNumber(&udpp);
        netTcpPacketArrive(buff, len-1);
      }
    }
  }
  dwSysNet += winboloTimer() - tick;
}

/*********************************************************
*NAME:          netTcpPacketArrive
*AUTHOR:        John Morrison
*CREATION DATE: 29/09/99
*LAST MODIFIED: 05/05/01
*PURPOSE:
* A tcp packet has arrived. It is processed here.
*
*ARGUMENTS:
*  buff  - Buffer that has arrived.
*  len   - length of the packet
*********************************************************/
void netTcpPacketArrive(BYTE *buff, int len) {
  BYTE *pnt;                                  /* Pointer to the buffer */
  
  clientMutexWaitFor();
  if (len == (int) sizeof(PLAYERLEAVE_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PLAYERLEAVE) {
    /* Player Leave */
    playersLeaveGame(screenGetPlayers(), buff[BOLOPACKET_PLAYERNUMLEAVEPOS]);
  } else if (len == (int) sizeof(LGMRETURN_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOLGMRETURN) {
    /* FIXME: len == sizeof(NEWPLAYER_PACKET) + BOLO_PACKET_CRC_SIZE */
    LGMRETURN_PACKET *p = (LGMRETURN_PACKET *) buff;
    screenNetLgmReturn(p->numTrees, p->numMines, p->numPills);
  } else if (len == (int) sizeof(NEWPLAYER_PACKET)  && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PLAYERNEWPLAYER) { 
    /* New player has joined */
    netPlayerJoined(buff);
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_BASESDATARESPONSE) { 
    /* Here is the bases data */
    pnt = buff;
    pnt += BOLOPACKET_REQUEST_TYPEPOS+1;
    screenSetBaseNetData(pnt, len - BOLOPACKET_REQUEST_TYPEPOS);
    netStat = netPillDownload;
    netPacketsPerSecond++;
    netJoinDataRequests();
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_STARTSDATARESPONSE) { 
    /* Here is the Starts data */
    pnt = buff;
    pnt += BOLOPACKET_REQUEST_TYPEPOS+1;
    screenSetStartsNetData(pnt, (BYTE) (len - BOLOPACKET_REQUEST_TYPEPOS));
    netStat = netBaseDownload;
    netPacketsPerSecond++;
    netJoinDataRequests();
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PILLSDATARESPONSE) { 
    /* Here is the Pillbox data */
    pnt = buff;
    pnt += BOLOPACKET_REQUEST_TYPEPOS+1;
    screenSetPillNetData(pnt, (BYTE) (len - BOLOPACKET_REQUEST_TYPEPOS));
    netStat = netMapDownload;
    netPacketsPerSecond++;
    netJoinDataRequests();
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_MAPDATARESPONSE && buff[BOLOPACKET_MAPDDOWNLOAD_YPOS] == netYPos) { 
    /* Here is the map download data */
    unsigned short dataLen;

    pnt = buff;
    pnt += BOLOPACKET_MAPDDOWNLOAD_DATASTART;
    memcpy(&dataLen, pnt, sizeof(dataLen));
    pnt += 2;
    screenSetMapNetRun(pnt, netYPos, dataLen);
    netYPos +=5;
    if (netYPos == 241) {
      netStat = netTimeDownload;
    }/* else {
      netStat = netMapDownload;
    } */
    netPacketsPerSecond++;
    netJoinDataRequests();
  } else if (len == (int) sizeof(TIME_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_TIMERESPONSE) { 
    /* Here is the time data */
    TIME_PACKET tp;
    memcpy(&tp, buff, sizeof(TIME_PACKET));
    screenSetTimeLengths(tp.start_delay, tp.time_limit);
    netStat = netRunning;
    clientMutexWaitFor();
    screenNetSetupTankGo();
    clientMutexRelease();
  } else if (len == (int) sizeof(PLAYERNAMECHANGE_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOCHANGENAME_DATA) {
    /* Change Player Name packet */
    netGetChangePlayerName((PLAYERNAMECHANGE_PACKET *) buff);
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_MESSAGE || buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_MESSAGE_ALL_PLAYERS) {
    /* A Message packet */
    netMessagePacket(buff, len);
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_DATA) { //|| buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_MESSAGE_ALL_PLAYERS
    /* A Data packet */
    netDataPacket(buff, len);
  } else if (len == (int) sizeof(ALLIANCE_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOLEAVEALLIANCE_DATA) {
    /* Player leave alliance */
    playersLeaveAlliance(screenGetPlayers(), buff[BOLOPACKET_REQUEST_TYPEPOS+1]);
    logAddEvent(log_AllyLeave, buff[BOLOPACKET_REQUEST_TYPEPOS+1], 0, 0, 0, 0, NULL);
  } else if (len == (int) sizeof(ALLIANCE_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOREQUESTALLIANCE_DATA) {
    /* Player Request alliance */
    char pName[FILENAME_MAX];
    playersGetPlayerName(screenGetPlayers(), buff[BOLOPACKET_REQUEST_TYPEPOS+1], pName);
    if (windowShowAllianceRequest() == TRUE) { 
      #ifdef _WIN32
      dialogAllianceSetName(dlgAllianceWnd, windowWnd(), pName, buff[BOLOPACKET_REQUEST_TYPEPOS+1]);
      #else 
      dialogAllianceSetName(pName, buff[BOLOPACKET_REQUEST_TYPEPOS+1]);
      #endif
    } else {
      char str[255];
      sprintf(str, "You have ignored alliance request from %s", pName);
      messageAdd(networkStatus, "Alliance Request", str);
      playersGetPlayerName(screenGetPlayers(), playersGetSelf(screenGetPlayers()), pName);
      sprintf(str, "%s has ignored your alliance request", pName);
      netMessageSendPlayer(playersGetSelf(screenGetPlayers()), buff[BOLOPACKET_REQUEST_TYPEPOS+1], str);
    }
    logAddEvent(log_AllyRequest, buff[BOLOPACKET_REQUEST_TYPEPOS+1], buff[BOLOPACKET_REQUEST_TYPEPOS+2], 0, 0, 0, NULL);
  } else if (len == (int) sizeof(ALLIANCE_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOACCEPTALLIANCE_DATA) {
    /* Player is accepted into an alliance */
    playersAcceptAlliance(screenGetPlayers(), buff[BOLOPACKET_REQUEST_TYPEPOS+1], buff[BOLOPACKET_REQUEST_TYPEPOS+2]);
    logAddEvent(log_AllyAccept, buff[BOLOPACKET_REQUEST_TYPEPOS+1], buff[BOLOPACKET_REQUEST_TYPEPOS+2], 0, 0, 0, NULL);
  } else if (len == (int) (sizeof(STARTPOSITION_PACKET)) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLORESPONSE_STARTPOS) {
    STARTPOSITION_PACKET *spp = (STARTPOSITION_PACKET *) buff;
    screenSetTankStartPosition(spp->x, spp->y, spp->angle,spp->numShells, spp->numMines);
  } else if (len == (int) (sizeof(LGMMOVE_PACKET)) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_LGM_OUTWORKING) {
    /* Lgm out working packet */
    LGMMOVE_PACKET *p;
    p = (LGMMOVE_PACKET *) buff;
    screenNetManWorking(p->mapX, p->mapY, p->mines, p->pillNum, p->trees);
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PACKETQUIT) {
    /* Server has quit */
    clientMutexRelease();
    netLostConnection();
    clientMutexWaitFor();
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOSERVERMESSAGE) {
    /* Server Message */
    BYTE *ptr;
    char msg[FILENAME_MAX];
    ptr = buff;
    ptr += BOLOPACKET_REQUEST_TYPEPOS+1;
    utilPtoCString((char *) ptr, msg);
    screenNetStatusMessage(msg);
  } else {
    /* Packet corrupt */
    netNumErrors++;
  }
  clientMutexRelease();
}

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
void netMakeInfoRespsonse(INFO_PACKET *buff) {
  char dummy[MAP_STR_SIZE]; /* Used to get the map name before conversion to pascal string */

  /* Make header */
  netMakePacketHeader(&(buff->h), BOLOPACKET_INFORESPONSE);
  /* Make game ID */
  buff->gameid.start_time = htonl(screenGetTimeGameCreated());
  netClientGetServerAddress(&(buff->gameid.serveraddress), &(buff->gameid.serverport));
  /* Get the map name and convert to a pascal string */
  screenGetMapName(dummy);
  utilCtoPString(dummy, buff->mapname);
  
  /* Make game variables */
  buff->gametype = screenGetGameType();
  if (screenGetAllowHiddenMines() == TRUE) {
    buff->allow_mines = HIDDEN_MINES;
  } else {
    buff->allow_mines = ALL_MINES_VISIBLE;
  }
  buff->allow_AI = screenGetAiType();
  buff->spare1 = 0;
  buff->start_delay = htonl(screenGetGameStartDelay());
  buff->time_limit = htonl(screenGetGameTimeLeft());
  buff->num_players = screenGetNumPlayers();
  buff->free_pills = screenGetNumNeutralPills();
  buff->free_bases = screenGetNumNeutralBases();
  buff->has_password = netPassword[0];
  buff->spare2 = 0;
}

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
void netMakePacketHeader(BOLOHEADER *hdr, BYTE packetType) {
  strncpy(hdr->signature, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE);
  hdr->versionMajor = BOLO_VERSION_MAJOR;
  hdr->versionMinor = BOLO_VERSION_MINOR;
  hdr->versionRevision = BOLO_VERSION_REVISION;
  hdr->type = packetType;
}

/*********************************************************
*NAME:          netMakeInfoRespsonse
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 22/11/99
*PURPOSE:
* A request for an ping packet has been made. Make a 
* response packet.
*
*ARGUMENTS:
*  buff - Buffer to hold the ping packet.
*********************************************************/
void netMakePingRespsonse(PING_PACKET *buff) {
  netMakePacketHeader(&(buff->h), BOLOPACKET_PINGREQUEST);
  buff->from = playersGetSelf(screenGetPlayers());
  buff->inPacket = udpPacketsGetInSequenceNumber(&udpp);
  buff->outPacket = udpPacketsGetOutSequenceNumber(&udpp);
  buff->sendTime = 0;
}

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
bool netWinBoloNetSetup(char *userName, char *password) {
  bool returnValue;              /* Value to return */
  bool done;
  bool done2;
  BYTE buff[MAX_UDPPACKET_SIZE]; /* Buffer that is sent */
  BOLOHEADER bh;                 /* Header packet   */
  int packetLen;                 /* Length of the packet */
  BYTE numTries;                 /* Number of tries for reading TCP data */
  BYTE key[WINBOLONET_KEY_LEN];  /* Server key */
  char errorMsg[FILENAME_MAX];   /* Error Message Storage */

  returnValue = TRUE;
  /* Send initial request */
  packetLen = BOLOPACKET_REQUEST_SIZE;
  netMakePacketHeader(&bh, BOLOPACKET_SERVERKEYREQUEST);
  memcpy(buff, &bh, sizeof(bh));
  packetLen = sizeof(bh);
  numTries = 0;
  done2 = FALSE;

  done  = netClientUdpPingServer(buff, &packetLen, TRUE, TRUE);
  while (done == TRUE && numTries < MAX_RETRIES) {
    if (packetLen == sizeof(bh)+32 && (strncmp(buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0) && buff[BOLO_VERSION_MAJORPOS] == BOLO_VERSION_MAJOR && buff[BOLO_VERSION_MINORPOS] == BOLO_VERSION_MINOR && buff[BOLO_VERSION_REVISIONPOS] == BOLO_VERSION_REVISION && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_SERVERKEYRESPONSE) {
      /* Password is correct */
      numTries = MAX_RETRIES;
      done2 = TRUE;
    } else {
      memcpy(buff, &bh, sizeof(bh));
      packetLen = sizeof(bh);
      done  = netClientUdpPingServer(buff, &packetLen, TRUE, TRUE);
    }
    numTries++;
  }

  if (done2 == TRUE) {
    if (buff[sizeof(bh)] == '\0') {
      #ifdef _WIN32	    
      MessageBox(NULL, "Sorry, this server is not participating in WinBolo.net", "WinBolo.net Error", MB_OK);
      #else
      MessageBox("Sorry, this server is not participating in WinBolo.net", "WinBolo.net Error");
      #endif
    } else {
      memcpy(key, buff+sizeof(bh), 32);
      if (winbolonetCreateClient(userName, password, key, errorMsg) == FALSE) {
        returnValue = FALSE;
	strcat(errorMsg, "\n\nPlease correct the problem or disable WinBolo.net and try to join again");
#ifdef _WIN32
        MessageBox(NULL, errorMsg, "WinBolo.net Error", MB_OK);
#else
        MessageBox(errorMsg, "WinBolo.net Error");
#endif
      }
    }
  } 

  return returnValue;
}

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
bool netJoinInit(char *ip, unsigned short port, bool usCreate, char *gamePassword) {
  bool returnValue;   /* Value to return */
  char buff[MAX_UDPPACKET_SIZE]; /* Data Buffer */
  char sendBuff[MAX_UDPPACKET_SIZE]; /* Buffer that is sent */
  BOLOHEADER bh;        /* Header packet   */
  INFO_PACKET inf;      /* Info packet */
  PASSWORD_PACKET pp;   /* Password packet */
  RSA_PACKET rsap;		/* RSA Packet */
  PLAYERNAME_PACKET pn; /* Player name packet */
  int packetLen;        /* Length of the packet */
  BYTE numTries;        /* Number of tries for reading TCP data */
  char *msg;            /* Pointer to message */

  /* Send initial request */
  packetLen = BOLOPACKET_REQUEST_SIZE;
  memset(buff, 0, MAX_UDPPACKET_SIZE);
  memset(sendBuff, 0, MAX_UDPPACKET_SIZE);
  memset(&pn, 0, sizeof(pn));
  netMakePacketHeader(&bh, BOLOPACKET_INFOREQUEST);
  memcpy(buff, &bh, sizeof(bh));
  returnValue = netClientUdpPing(buff, &packetLen, ip, port, FALSE, FALSE);
  if (packetLen != sizeof(inf) || (strncmp((char *) buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) != 0) || buff[BOLO_VERSION_MAJORPOS] != BOLO_VERSION_MAJOR || buff[BOLO_VERSION_MINORPOS] != BOLO_VERSION_MINOR || buff[BOLO_VERSION_REVISIONPOS] != BOLO_VERSION_REVISION || buff[BOLOPACKET_REQUEST_TYPEPOS] != BOLOPACKET_INFORESPONSE) {
#ifdef _WIN32
	  int i = sizeof(inf);
    MessageBox(NULL, langGetText(NETERR_JOININIT), DIALOG_BOX_TITLE, MB_OK);
#else 
    MessageBox(langGetText(NETERR_JOININIT), DIALOG_BOX_TITLE);
#endif
    returnValue = FALSE;
  }

  /* Check there is an availble player slot */
  if (returnValue == TRUE) {
    memcpy(&inf, buff, sizeof(inf));
    if (inf.num_players == MAX_TANKS) {
      returnValue = FALSE;
#ifdef _WIN32
     MessageBox(NULL, langGetText(NETERR_GAMEFULL), DIALOG_BOX_TITLE, MB_OK);
#else
     MessageBox(langGetText(NETERR_GAMEFULL), DIALOG_BOX_TITLE);
#endif
    } else {
      /* Extract map name, AI Type and hidden mines and game start and originator */
      utilPtoCString(inf.mapname, buff);
      screenSetMapName(buff);
      gameFrontSetAIType((aiType) (inf.allow_AI));
      if (inf.allow_mines == HIDDEN_MINES) {
        screenSetAllowHiddenMines(TRUE);
      } else {
        screenSetAllowHiddenMines(FALSE);
      }
      screenSetGameType(inf.gametype);
      screenSetTimeGameCreated(ntohl(inf.gameid.start_time));
      if (inf.gameid.serveraddress.s_addr == 0) {
        /* Last packet must be from server. */
        netClientGetLast(&(inf.gameid.serveraddress), &(inf.gameid.serverport));
        inf.gameid.serverport = ntohs(inf.gameid.serverport);
      }
      if (strcmp("127.0.0.1", inet_ntoa(inf.gameid.serveraddress)) == 0) {
        /* Set localhost address to our IP address */
        //unsigned short dummy;
        //netClientGetUs(&(inf.gameid.serveraddress), &dummy);
      }
      netClientSetServerAddress(&(inf.gameid.serveraddress), (inf.gameid.serverport));
    }
  }

  /* Password if required */
  if (returnValue == TRUE) {
    if (inf.has_password != 0) {
      if (usCreate == FALSE) {
        gameFrontGetPassword(netPassword);
      } else {
        strcpy(netPassword, gamePassword);
      }
      netMakePacketHeader(&(pp.h), BOLOPACKET_PASSWORDCHECK);
      utilCtoPString(netPassword, pp.password);
      packetLen = sizeof(pp);
      memcpy(sendBuff, &pp, packetLen);
      memcpy(buff, sendBuff, sizeof(pp));
      numTries = 0;
      returnValue = netClientUdpPing(buff, &packetLen, ip, port, TRUE, TRUE);
      while (returnValue == TRUE && numTries < MAX_RETRIES) {
        if (packetLen == sizeof(bh) + BOLO_PACKET_CRC_SIZE && (strncmp(buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0) && buff[BOLO_VERSION_MAJORPOS] == BOLO_VERSION_MAJOR && buff[BOLO_VERSION_MINORPOS] == BOLO_VERSION_MINOR && buff[BOLO_VERSION_REVISIONPOS] == BOLO_VERSION_REVISION && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PASSWORDACCEPT) {
          /* Password is correct */
          numTries = MAX_RETRIES;
        } else if (packetLen == sizeof(bh) + BOLO_PACKET_CRC_SIZE && (strncmp(buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0) && buff[BOLO_VERSION_MAJORPOS] == BOLO_VERSION_MAJOR && buff[BOLO_VERSION_MINORPOS] == BOLO_VERSION_MINOR && buff[BOLO_VERSION_REVISIONPOS] == BOLO_VERSION_REVISION && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PASSWORDFAIL) {
          returnValue = FALSE;
          #ifdef _WIN32
          MessageBox(NULL, langGetText(NETERR_PASSWORDWRONG), DIALOG_BOX_TITLE, MB_OK);
          #else
          MessageBox(langGetText(NETERR_PASSWORDWRONG), DIALOG_BOX_TITLE);
          #endif
        } else {
          packetLen = sizeof(pp);
          memcpy(buff, sendBuff, sizeof(pp));
          returnValue = netClientUdpPing(buff, &packetLen, ip, port, TRUE, TRUE);
        }
        numTries++;
      }
      if (returnValue == FALSE && numTries == 0) {
        #ifdef _WIN32
        MessageBox(NULL, langGetText(NETERR_NONEWPLAYERS), DIALOG_BOX_TITLE, MB_OK);
        #else 
        MessageBox(langGetText(NETERR_NONEWPLAYERS), DIALOG_BOX_TITLE);
        #endif
      }

    }
  }
  /* RSA Check */
  if (returnValue == TRUE){
	netMakePacketHeader(&(rsap.h), BOLOPACKET_RSACHECK);
	packetLen = sizeof(rsap);
    memcpy(sendBuff, &rsap, packetLen);
    memcpy(buff, sendBuff, packetLen);
    numTries = 0;
    returnValue = netClientUdpPing(buff, &packetLen, ip, port, TRUE, TRUE);
    // the while loop is just in case this message got lost in the mail.
	while (returnValue == TRUE && numTries < MAX_RETRIES) {
		if (packetLen == sizeof(rsap) || (strncmp((char *) buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) != 0) || buff[BOLO_VERSION_MAJORPOS] != BOLO_VERSION_MAJOR || buff[BOLO_VERSION_MINORPOS] != BOLO_VERSION_MINOR || buff[BOLO_VERSION_REVISIONPOS] != BOLO_VERSION_REVISION || buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_RSARESPONSE) {
			//Ok, we should have the randomly generated message from the server here, and now we can encrypt it and send it back.
			memcpy(&rsap, buff, sizeof(rsap));
			numTries = MAX_RETRIES;
			strcpy(rsastring, rsap.rsa);
		}
		else
		{
			packetLen = sizeof(rsap);
			memcpy(buff, sendBuff, packetLen);
			returnValue = netClientUdpPing(buff, &packetLen, ip, port, TRUE, TRUE);
		}
		numTries++;
	}
  }
 
  /* Check for player name availability */
  if (returnValue == TRUE) {
    netMakePacketHeader(&(pn.h), BOLOPACKET_NAMECHECK);
    gameFrontGetPlayerName(buff);
    utilStripNameReplace(buff);
    utilCtoPString(buff, pn.playerName);
    packetLen = sizeof(pn);
    memcpy(sendBuff, &pn, packetLen);
    memcpy(buff, sendBuff, packetLen);
    numTries = 0;
    returnValue = netClientUdpPing(buff, &packetLen, ip, port, TRUE, TRUE);
    while (returnValue == TRUE && numTries < MAX_RETRIES) {
      if (packetLen == sizeof(bh) + BOLO_PACKET_CRC_SIZE && (strncmp(buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0) && buff[BOLO_VERSION_MAJORPOS] == BOLO_VERSION_MAJOR && buff[BOLO_VERSION_MINORPOS] == BOLO_VERSION_MINOR && buff[BOLO_VERSION_REVISIONPOS] == BOLO_VERSION_REVISION && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_NAMEACCEPT) {
        numTries = MAX_RETRIES;
      } else if (packetLen == sizeof(bh) + BOLO_PACKET_CRC_SIZE && (strncmp(buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0) && buff[BOLO_VERSION_MAJORPOS] == BOLO_VERSION_MAJOR && buff[BOLO_VERSION_MINORPOS] == BOLO_VERSION_MINOR && buff[BOLO_VERSION_REVISIONPOS] == BOLO_VERSION_REVISION && (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_NAMEFAIL || buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_GAMELOCKED || buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_MAXPLAYERS)) {
        /* Player Name is incorrect */
        returnValue = FALSE;
        if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_GAMELOCKED) {
          msg = langGetText(NETERR_NONEWPLAYERS);
        } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] ==  BOLOPACKET_NAMEFAIL) {
          msg = langGetText(NETERR_PLAYERNAMEINUSE);
        } else {
	        strcpy(buff, "Sorry, the server player limit has been reached");
          msg = buff;
	      }
        #ifdef _WIN32
          MessageBox(NULL, msg, DIALOG_BOX_TITLE, MB_OK);
        #else
          MessageBox(msg, DIALOG_BOX_TITLE);
        #endif
      } else {
        packetLen = sizeof(pn);
        memcpy(buff, sendBuff, packetLen);
        returnValue = netClientUdpPing(buff, &packetLen, ip, port, TRUE, TRUE);
      }
      numTries++;
    }
  }
  return returnValue;
}

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
bool netJoin(char *ip, unsigned short port, bool wantRejoin, char *userName, char *password, bool useWbn, bool usCreate, char *gamePassword) {
  bool returnValue; /* Value to return */

  returnValue = netJoinInit(ip, port, usCreate, gamePassword);
  if (returnValue == TRUE) {
    /* We have a valid game. Finalise all player data */
    returnValue = netJoinFinalise(ip, port, wantRejoin, userName, password, useWbn);
  }
  return returnValue;
}

/*********************************************************
*NAME:          netJoinFinalise
*AUTHOR:        John Morrison
*CREATION DATE: 24/02/99
*LAST MODIFIED: 02/02/03
*PURPOSE:
* Finalising join process. Get list of all players and
* a player number. Returns success.
*
*ARGUMENTS:
*  targetip   - String IP of the contact machine
*  port       - Port of the contact machine
*  wantRejoin - TRUE if we want to rejoin the game else 
*               just join
*  userName   - Account name we wish to use
*  password   - Account password
*  useWbn     - Do we want to try and use WBN?              
*********************************************************/
bool netJoinFinalise(char *targetip, unsigned short port, bool wantRejoin, char *userName, char *password, bool useWbn) {
  bool returnValue;           /* Value to return              */
  struct in_addr dummy;       /* Dummy IP address             */
  BYTE buff[MAX_UDPPACKET_SIZE]; /* Data buffer                  */
  BYTE sendBuff[MAX_UDPPACKET_SIZE]; /* Send Buffer              */
  char name[MAX_UDPPACKET_SIZE]; /* Player Name */
  char *ip;                   /* IP address of a player       */    
  int buffLen;                /* Length of the buffer         */
  PLAYERNUM_REQ_PACKET prp;   /* Player Number request packet */
  PLAYERDATA_REQPACKET pdr;   /* Player data request packet   */
  PLAYERDATA_PACKET pdres;    /* Player data response packet  */
  BYTE maxPlayers;            /* Maximum players in the game  */
  BYTE lowPlayers;            /* Lowest playernumber          */
  BYTE count;                 /* Looping variable             */
  BYTE crcA, crcB;            /* CRC Bytes                    */
  BYTE numTries;              /* Number of tries              */
  BIGD c,m,e,n;				  /* temporary holder for encrytion */
  char newName[256]; /* Our new name - Prefix with * if WBN participant */
  char rsaencryptedholder[512];
  REJOINREQUEST_PACKET rrp = REQUESTHEADER; /* Rejoin packet */

  returnValue = TRUE;
  memset(rsaencryptedholder, 0, 258);
  /* WinBolo.net check */
  if (useWbn == TRUE) {
    returnValue = netWinBoloNetSetup(userName, password);
  }

  if (returnValue == TRUE) {
    memset(&prp, 0, sizeof(PLAYERNUM_REQ_PACKET));
    memset(&pdr, 0, sizeof(PLAYERDATA_REQPACKET));
    memset(&pdres, 0, sizeof(PLAYERDATA_PACKET));
    netMakePacketHeader(&(prp.h), BOLOPACKET_PLAYERNUMREQUEST);
    netClientGetUs(&dummy, &(prp.port));
    gameFrontGetPlayerName(buff);
    utilCtoPString(buff, prp.playerName);
    if (useWbn == TRUE) {
      winboloNetGetMyClientKey((BYTE *) &(prp.key));
    } else {
      prp.key[0] = EMPTY_CHAR;
    }
    utilCtoPString(netPassword, prp.password);
	// setup encryption, encrypt the random string we were sent earlier, and send it with the player number request so it can be processed by the server.
	c = bdNew();
	m = bdNew();
	e = bdNew();
	n = bdNew();
	bdConvFromHex(m,rsastring);
	bdSetShort(e, 00000003);
	bdConvFromHex(n,"b0c9dd3085dc9a0a4183957d1d25d59edbe9c72930962a171c17e5456959b154edd51289425136e53c925ed73d55163a575774151e2eb7f740830ca93f3da7b19e30514b1ee09246d7bf56c1a8e2e9c5c7c197c60849e5f7ce3a9691b6d17c825741341904b316a98bd8d160f939300ee0c4042d5428f22de095f606f120e1d7");
	// rsa encryption here
	/* Encrypt c = m^e mod n */
	bdModExp(c, m, e, n);
	pr_msg("c= ", c);
	pr_msg("m= ", m);
	pr_msg("e= ", e);
	pr_msg("n= ", n);
	bdConvToHex(c,rsaencryptedholder,258);
	memcpy(prp.rsaencrypted,rsaencryptedholder,sizeof(rsaencryptedholder));
	bdFree(&c);
	bdFree(&m);
	bdFree(&e);
	bdFree(&n);

    memcpy(sendBuff, &prp, sizeof(prp));

  
    numTries = 0;
    buffLen = sizeof(prp);
    memcpy(buff, sendBuff, sizeof(prp));
    returnValue = netClientUdpPingServer(buff, &buffLen, TRUE, TRUE);
    while (returnValue == TRUE && numTries < MAX_RETRIES) {
      if (buffLen == sizeof(PLAYERNUM_PACKET) + BOLO_PACKET_CRC_SIZE && (strncmp(buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0) && buff[BOLO_VERSION_MAJORPOS] == BOLO_VERSION_MAJOR && buff[BOLO_VERSION_MINORPOS] == BOLO_VERSION_MINOR && buff[BOLO_VERSION_REVISIONPOS] == BOLO_VERSION_REVISION && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PLAYERNUMRESPONSE && buff[BOLOPACKET_PLAYERNUMPOS] < MAX_TANKS) {
        crcA = buff[sizeof(PLAYERNUM_PACKET)];
        crcB = buff[sizeof(PLAYERNUM_PACKET)+1];
        if (CRCCheck(buff, sizeof(PLAYERNUM_PACKET), crcA, crcB) == TRUE) {
          gameFrontGetPlayerName(name);
	        newName[0] = EMPTY_CHAR;
	        if (winbolonetIsRunning() == TRUE && useWbn == TRUE) {
	          strcat(newName, "*");
            windowAllowPlayerNameChange(FALSE);
          }
          strcat(newName, name);
          playersSetSelf(screenGetPlayers(), buff[BOLOPACKET_PLAYERNUMPOS], newName);
		  basesUpdateTimer(buff[BOLOPACKET_PLAYERNUMPOS]);
          lowPlayers = buff[BOLOPACKET_PLAYERNUMPOSLOW];
          maxPlayers = buff[BOLOPACKET_PLAYERNUMPOSMAX];
          numTries = MAX_RETRIES;
        } else {
          returnValue = FALSE;
        }
      } else {
        numTries++;
        if (numTries == MAX_RETRIES) {
          returnValue = FALSE;
          #ifdef _WIN32
          MessageBox(NULL, langGetText(NETERR_CONNECTNOJOIN), DIALOG_BOX_TITLE, MB_OK);
          #else
          MessageBox(langGetText(NETERR_CONNECTNOJOIN), DIALOG_BOX_TITLE);
          #endif
        } else {
          buffLen = sizeof(prp);
          memcpy(buff, sendBuff, sizeof(prp));
          returnValue = netClientUdpPingServer(buff, &buffLen, TRUE, TRUE);
        }
      }
    } 
  }

  /* Rejoin if required */
  if (returnValue == TRUE) {
    if (wantRejoin == TRUE) {
      netMakePacketHeader(&(rrp.h), BOLOREJOINREQUEST);
      rrp.playerNumber = buff[BOLOPACKET_PLAYERNUMPOS];
      memcpy(buff, &rrp, sizeof(rrp));
      buff[sizeof(rrp)] = UDP_NON_RELIABLE_PACKET;
      CRCCalcBytes(buff, sizeof(rrp)+1, buff+sizeof(rrp)+1, buff+sizeof(rrp)+2);
      netClientSendUdpServer(buff, sizeof(rrp)+3); 
    }
  }
  
  /* Point of no return */
  if (returnValue == TRUE) {
    /* Get each players details */
    netMakePacketHeader(&(pdr.h), BOLOPACKET_PLAYERDATAREQUEST);
    memcpy(buff, &pdr, sizeof(pdr));
    buffLen = sizeof(pdr);
    numTries = 0;
    returnValue = netClientUdpPingServer(buff, &buffLen, TRUE, TRUE); 
    while (returnValue == TRUE && numTries < MAX_RETRIES) {
      if (returnValue == TRUE && buffLen == sizeof(PLAYERDATA_PACKET) + BOLO_PACKET_CRC_SIZE && (strncmp(buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0) && buff[BOLO_VERSION_MAJORPOS] == BOLO_VERSION_MAJOR && buff[BOLO_VERSION_MINORPOS] == BOLO_VERSION_MINOR && buff[BOLO_VERSION_REVISIONPOS] == BOLO_VERSION_REVISION && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PLAYERDATARESPONSE) {
        /* Get the info from it */
        memcpy(&pdres, buff, sizeof(PLAYERDATA_PACKET));
        for (count=0;count<MAX_TANKS;count++) {
          if (pdres.items[count].inUse == TRUE) {      
            utilPtoCString(pdres.items[count].playerName ,buff);
            ip = inet_ntoa(pdres.items[count].addr);
            playersSetPlayer(screenGetPlayers(), count, buff, ip, 0, 0, 0, 0, 0, 0, pdres.items[count].numAllies, pdres.items[count].allies);
            dnsLookupsAddRequest(ip, netProcessedDnsLookup);
            /* One day add stuff to do with each players address */
          }
        } 
        numTries = MAX_RETRIES;
      } else if (returnValue == TRUE) {
        memcpy(buff, &pdr, sizeof(pdr));
        buffLen = sizeof(pdr);
        returnValue = netClientUdpPingServer(buff, &buffLen, TRUE, TRUE); 
      }
    }
  }
  return returnValue;
}

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
void netProcessedDnsLookup(char *ip, char *host) {
  if (inNetShutdown == FALSE) {
    clientMutexWaitFor();
    playerSetLocation(screenGetPlayers(), ip, host);
	  clientMutexRelease();
  }
}

/*********************************************************
*NAME:          netPlayerJoined
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 27/3/99
*PURPOSE:
* A player has joined. Add data to players structure and
* add to netPlayers
*
*ARGUMENTS:
*  buff - Byte buffer of data
*********************************************************/
void netPlayerJoined(BYTE *buff) {
  NEWPLAYER_PACKET npp;       /* New player packet */
  char name[PLAYER_NAME_LEN]; /* New players name */
  char *ip;                   /* New players IP */

  memcpy(&npp, buff, sizeof(NEWPLAYER_PACKET));
  utilPtoCString(npp.playerName, name);
  ip = inet_ntoa(npp.addr);
  dnsLookupsAddRequest(ip, netProcessedDnsLookup);
  playersSetPlayer(screenGetPlayers(), npp.playerNumber, name, ip, 0, 0, 0, 0, 0, FALSE, 0, NULL);
  /* Do something with the address and port */
}

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
netStatus netGetStatus(void) {
  return netStat;
}

/*********************************************************
*NAME:          netMakeTokenPacket
*AUTHOR:        John Morrison
*CREATION DATE: 27/2/99
*LAST MODIFIED: 20/3/99
*PURPOSE:
* Makes and sends the token packet.
*
*ARGUMENTS:
*
*********************************************************/
void netMakeTokenPacket(void) {
  BYTE info[MAX_UDPPACKET_SIZE] = CLIENTHEADER; /* Packet that is sent */
  BYTE *ptr;                                 /* Pointer to buffer for memcopies */
  BYTE buff[MAX_UDPPACKET_SIZE];                /* Passed to functions to get backend packet data */
  int packetLen;                             /* Length of the packet */
  BYTE len;                                  /* length of data bits */
  bool shouldSend;                           /* Should this packet be sent */
  DWORD tick;     /* Number of ticks passed */
  
  tick = winboloTimer();
  if (inNetShutdown == TRUE) {
    return;
  }

  packetLen = BOLOPACKET_REQUEST_SIZE;
  ptr = info;
  ptr += BOLOPACKET_REQUEST_TYPEPOS+1;
  shouldSend = FALSE;

  /* Make the shell data */
  clientMutexWaitFor();
  len = screenMakeShellData(buff);
  clientMutexRelease();
  if (len > 0) {
    shouldSend = TRUE;
    *ptr = BOLO_PACKET_SHELLDATA;
    ptr++;
    *ptr = len;
    ptr++;
    memcpy(ptr, buff, len);
    ptr += len;
    packetLen += len + BOLO_PACKET_OPTDATAHEADER;
  }

  /* Make the map data */
  clientMutexWaitFor();
  len = screenMakeMapTokenData(buff);
  clientMutexRelease();
  if (len > 0) {
    shouldSend = TRUE;
    *ptr = BOLO_PACKET_MAPDATA;
    ptr++;
    *ptr = len;
    ptr++;
    memcpy(ptr, buff, len);
    ptr += len;
    packetLen += len + BOLO_PACKET_OPTDATAHEADER;
  }
 
  /* Make the tk data */
  clientMutexWaitFor();
  len = screenMakeTKData(buff);
  clientMutexRelease();
  if (len > 0) {
    shouldSend = TRUE;
    *ptr = BOLO_PACKET_TKDATA;
    ptr++;
    *ptr = len;
    ptr++;
    memcpy(ptr, buff, len);
    ptr += len;
    packetLen += len + BOLO_PACKET_OPTDATAHEADER;
  }
 
  /* Make the pnb data */
  clientMutexWaitFor();
  len = netPNBMake(screenGetNetPnb(), buff);
  clientMutexRelease();
  if (len > 0) {
    shouldSend = TRUE;
    *ptr = BOLO_PACKET_PNBDATA;
    ptr++;
    *ptr = len;
    ptr++;
    memcpy(ptr, buff, len);
    ptr += len;
    packetLen += len + BOLO_PACKET_OPTDATAHEADER; 
  }

  /* Make the NMT Data */
  clientMutexWaitFor();
  len = netMNTMake(screenGetNetMnt(), buff);
  clientMutexRelease();
  if (len > 0) {
    shouldSend = TRUE;
    *ptr = BOLO_PACKET_MNTDATA;
    ptr++;
    *ptr = len;
    ptr++;
    memcpy(ptr, buff, len);
    ptr += len;
    packetLen += len + BOLO_PACKET_OPTDATAHEADER;
  }
  
  if (shouldSend == TRUE) {
    netSend(info, packetLen);
    /* Make the token time */
    time(&tknTime);
    /* Update the screen */
    screenNetToken();
  }
  dwSysNet += winboloTimer() - tick;
}

/*********************************************************
*NAME:          netDataPacket
*AUTHOR:        John Morrison
*CREATION DATE: 28/2/99
*LAST MODIFIED: 31/10/99
*PURPOSE:
* A data packet has arrived. Look after it here.
*
*ARGUMENTS:
*   buff - Data buffer
*   len  - Length of the buffer
*********************************************************/
void netDataPacket(BYTE *buff, int len) {
  BYTE *ptr;       /* Pointer to the buffer */
  BYTE data;       /* Data Type */
  BYTE dataLen;    /* Data length */
  int upto;

  ptr = buff;
  upto = 0;


  ptr += (BOLOPACKET_REQUEST_TYPEPOS + 1);
  upto += (BOLOPACKET_REQUEST_TYPEPOS + 1);
//  len--;
 
  while (upto < len) {
    data = *ptr;
    upto++;
    ptr++;
    dataLen = *ptr;
    ptr++;
    upto++;
    if (dataLen > 0) {
      switch (data) {
      case BOLO_PACKET_MAPDATA:
      /* Extract Map Data */
        clientMutexWaitFor();
        screenExtractMapData(ptr, dataLen, netYPos);
        clientMutexRelease();
        break;
      case BOLO_PACKET_PNBDATA:
        clientMutexWaitFor();
        netNumErrors += !(screenExtractPNBData(ptr, dataLen, TRUE));
        clientMutexRelease();
        break;
      case BOLO_PACKET_TKDATA:
        clientMutexWaitFor();
        screenExtractTKData(ptr, dataLen);
        clientMutexRelease();
        break;
      case BOLO_PACKET_MNTDATA:
        clientMutexWaitFor();
        netNumErrors += !(screenExtractMNTData(ptr, dataLen, TRUE));
        clientMutexRelease();
      }
      ptr += dataLen;
      upto += dataLen;
    }
  }
  netPacketsPerSecond++;
}

/*********************************************************
*NAME:          netJoinDataRequests
*AUTHOR:        John Morrison
*CREATION DATE: 28/2/99
*LAST MODIFIED: 29/3/99
*PURPOSE:
* If we need to do any downloading request do them here
*
*ARGUMENTS:
*
*********************************************************/
void netJoinDataRequests(void) {
  BYTE joinReq;
//  static BYTE lastDone = 0;
  BYTE data[MAX_UDPPACKET_SIZE] = GENERICHEADER;
  
/*  if (lastDone < 10) {
    lastDone++;
    return;
  }
  lastDone = 0; */
  joinReq = netStat;
  if (netStat == netFailed) {
    joinReq = oldNetStatus;
  }
  switch (joinReq) {
  case netStartDownload:
    data[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_STARTSDATAREQUEST;
    netSend(data, BOLOPACKET_REQUEST_SIZE);
    netPacketsPerSecond++;
    break;
  case netBaseDownload:
    data[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_BASESDATAREQUEST;
    netSend(data, BOLOPACKET_REQUEST_SIZE);
    netPacketsPerSecond++;
    break;
  case netPillDownload:
    data[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_PILLSDATAREQUEST;
    netSend(data, BOLOPACKET_REQUEST_SIZE);
    netPacketsPerSecond++;
    break;
  case netMapDownload:
    data[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_MAPDATAREQUEST;
    data[BOLOPACKET_MAPDDOWNLOAD_YPOS] = netYPos;
    netSend(data, sizeof(MAPDOWNLOADREQUEST_PACKET));
    netPacketsPerSecond++;
    break;
  case netTimeDownload:
    data[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_TIMEREQUEST;
    netSend(data, BOLOPACKET_REQUEST_SIZE);
    netPacketsPerSecond++;
    break;
  case netJoining:
  case netRunning:
  case netFailed:
  default:
    break;
  }
}

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
void netGetOurAddressStr(char *dest) {
  dest[0] = '\0';
  if (networkGameType == netUdp) {
    netClientGetUsStr(dest);
  } else {
    strcpy(dest, NET_SINGLE_PLAYER_GAME);
  }
}

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
void netGetServerAddressStr(char *dest) {
  if (networkGameType == netUdp) {
    netClientGetServerAddressString(dest);
  } else {
    strcpy(dest, NET_SINGLE_PLAYER_GAME);
  }
}

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
void netGetStats(char *status, int *ping, int *ppsec, int *retrans) {
  switch (netStat) {
  case netJoining:
  case netStartDownload:
  case netBaseDownload:
  case netPillDownload:
  case netMapDownload:
  case netTimeDownload:
    strcpy(status, langGetText(NET_STATUS_JOINING));
    break;
  case netRunning:
    strcpy(status, langGetText(NET_STATUS_OK));
    break;
  default: /* netFailed */
    strcpy(status, langGetText(NET_STATUS_FAILED));
    break;
  }
  
  *ping = netRingDelay;
  *ppsec = netTotPacketsPerSecond;
  if (netTotPacketsPerSecond > 0 && netTotPacketsPerSecond < 5) {
    *ppsec += 5;
  }

  *retrans = netNumErrors;
}

/*********************************************************
*NAME:          netSecond
*AUTHOR:        John Morrison
*CREATION DATE: 6/3/99
*LAST MODIFIED: 22/11/99
*PURPOSE:
* Called once a second to reset packet per second counter
*
*ARGUMENTS:
*
*********************************************************/
void netSecond(void) {
  static BYTE count = 0; /* Count to hold every 2 seconds */
  PING_PACKET pp;        /* Ping packet to send */

  dwSysNetTotal = dwSysNet;
  dwSysNet = 0;
  netTotPacketsPerSecond = netPacketsPerSecond;
  netPacketsPerSecond = 0;
  count++;
  if (count == 2 && networkGameType == netUdp) {
    count = 0;
    netMakePingRespsonse(&pp);
    tknTime = winboloTimer();
    netClientSendUdpServer((char *) &pp, sizeof(pp));
    netPacketsPerSecond++;
  }
}

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
void netDataPosPacket(BYTE *buff, int len, unsigned short port) {
  BYTE *ptr; /* Pointer to the buffer */
  BYTE sectionType; /* Type of the section */
  BYTE sectionLen; /* Length of the section */
  int pos;        /* Position we are through the data */
  BYTE crcA, crcB; /* CRC Bytes */
  
  ptr = buff;
  pos = 0;

  /* Check CRC */
  crcA = buff[len-2];
  crcB = buff[len-1];
  if (CRCCheck(buff, len - BOLO_PACKET_CRC_SIZE, crcA, crcB) == FALSE) {
    netNumErrors++;
  } else {
    /* Process player location data */
    netClientSetServerPort(ntohs(port));
    ptr = buff;
    pos = BOLOPACKET_REQUEST_TYPEPOS+1;
    ptr += pos;
    len -= BOLO_PACKET_CRC_SIZE;
    len--;
    while (pos < len) {
      sectionType = *ptr;
      pos++;
      ptr++;
      sectionLen = *ptr;
      pos++;
      ptr++;
      if (sectionLen > 0) {
        switch (sectionType) {
        case BOLOPACKET_MAND_DATA:
          screenExtractPlayerData(ptr, sectionLen);
          break;
        case BOLO_PACKET_PNBDATA:
          screenExtractPNBData(ptr, sectionLen, FALSE);
          break;
        case BOLO_PACKET_MNTDATA:
          screenExtractMNTData(ptr, sectionLen, FALSE);
          break;
      	case BOLO_PACKET_SHELLDATA: 
           screenExtractShellData(ptr, sectionLen);
          break;
	      default:
          break;
        }
      }
      ptr += sectionLen;
      pos += sectionLen;
    }
  }

  netPacketsPerSecond++;
}


//bool lgmIsOut();
bool netSendNow = FALSE;
time_t windowsGetTicks();


/*********************************************************
*NAME:          netMakeDataPosPacket
*AUTHOR:        John Morrison
*CREATION DATE: 20/3/99
*LAST MODIFIED: 30/8/99
*PURPOSE:
* Makes and sends our player location and shells stuff
* and sends it to all players
*
*ARGUMENTS:
*
*********************************************************/
void netMakeDataPosPacket(void) {
  static BYTE countHalf = 0;
  static BYTE countHalfAmount = 6;
  static BYTE changed = 0;
  static BYTE last[BOLOPACKET_MAND_DATA];
  BYTE info[MAX_UDPPACKET_SIZE] = POSHEADER; /* Packet that is sent */
  BYTE *ptr;                              /* Pointer to buffer for memcopies */
  BYTE crcA, crcB;                        /* CRC Bytes */
  DWORD tick;     /* Number of ticks passed */
  BYTE posLen;
  

  time_t t;

  tick = winboloTimer();
  if (inNetShutdown == TRUE) {
    return;
  }

  
  if (networkGameType == netUdp) {
    netClientUdpCheck();
    if (time(NULL) - netLastHeard >= NET_MAX_TIMEOUT) {
      netLostConnection();
      return;
    }
  }
  if (netStat == netFailed) {
    return;
  }
  countHalf++;
  if (netSendNow == TRUE) {
    countHalf = countHalfAmount;
/*  } else if (lgmIsOut(screenGetLgmFromPlayerNum(playersGetSelf()))) {
This doesn't matter
    countHalfAmount = 3; */
  } else {
    countHalfAmount = 6;
  }
  if (countHalf < countHalfAmount) {
    return;
  }
  countHalf = 0;

  if (netYPos >= 241) {
    ptr = info;
    ptr += BOLOPACKET_REQUEST_TYPEPOS+1;
    clientMutexWaitFor();
    posLen = screenMakePosInfo(ptr);
    clientMutexRelease();
    /* Make the CRC */
    if (memcmp(ptr, last, posLen) != 0 || changed > 2) {
      memcpy(last, ptr, posLen);
      ptr += posLen;
      changed = 0;
      /*NEW CODE */
          t = windowsGetTicks();
          memcpy(ptr, &t, sizeof(time_t));
          posLen += sizeof(time_t);
          ptr += sizeof(time_t);
    /*END NEW CODE */

      *ptr = UDP_NON_RELIABLE_PACKET;
      ptr++;
      posLen++;
      CRCCalcBytes(info, BOLOPACKET_REQUEST_SIZE + posLen, &crcA, &crcB);
      *ptr = crcA;
      ptr++;
      *ptr = crcB;
      /* Send it */
      netClientSendUdpServer(info, (BOLOPACKET_REQUEST_SIZE + posLen + BOLO_PACKET_CRC_SIZE));
      netPacketsPerSecond++;
    } else {
      changed++;
    }
  }
  
  dwSysNet += winboloTimer() - tick;
}

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
BYTE netGetDownloadPos(void) {
  return netYPos;
}

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
void netMessagePacket(BYTE *data, int dataLen) {
  char message[512];
  BYTE *ptr;

  ptr = data;
  ptr += BOLOPACKET_MAPDDOWNLOAD_DATASTART + 1;
  utilPtoCString((char *) ptr, message);
  ptr -= 2;
  screenIncomingMessage(*ptr, message);
}

void netMessageSendAllPlayers(BYTE playerNum, char *message) {
  BYTE info[MAX_UDPPACKET_SIZE] = MESSAGEHEADER_ALLPLAYERS; /* Packet that is sent */
  BYTE *ptr;

  if (networkGameType == netUdp) {
    ptr = info;
    info[BOLOPACKET_MAPDDOWNLOAD_YPOS] = playerNum;
    ptr += BOLOPACKET_MAPDDOWNLOAD_DATASTART;
    *ptr = NEUTRAL;
    ptr++;
    utilCtoPString(message, (char *) ptr);
    ptr = info + BOLOPACKET_MAPDDOWNLOAD_DATASTART;
    netSend(info, (*ptr) + 11);
  }
}

void netMessageSendPlayer(BYTE playerNum, BYTE destPlayer, char *message) {
  BYTE info[MAX_UDPPACKET_SIZE] = MESSAGEHEADER; /* Packet that is sent */
  BYTE *ptr;

  if (networkGameType == netUdp) {
    ptr = info;
    info[BOLOPACKET_MAPDDOWNLOAD_YPOS] = playerNum;
    ptr += BOLOPACKET_MAPDDOWNLOAD_DATASTART;
    *ptr = destPlayer;
    ptr++;
    utilCtoPString(message, (char *) ptr);
    ptr = info + BOLOPACKET_MAPDDOWNLOAD_DATASTART+1;
    /* Send it */
    netSend(info, (*ptr) + 11);
  }
}

/*********************************************************
*NAME:          netSendChangePlayerName
*AUTHOR:        John Morrison
*CREATION DATE: 17/4/99
*LAST MODIFIED: 29/8/99
*PURPOSE:
* You have changed your player name. Inform the other
* network players
*
*ARGUMENTS:
*  playerNum - Your player number
*  newName   - Your new player Name
*********************************************************/
void netSendChangePlayerName(BYTE playerNum, char *newName) {
  PLAYERNAMECHANGE_PACKET info; /* Packet to send */
  BYTE buff[128];

  netMakePacketHeader(&(info.h), BOLOCHANGENAME_DATA);
  info.playerNum = playerNum;
  utilCtoPString(newName, info.playerName);
  memcpy(buff, &info, sizeof(info));
  netSend(buff, sizeof(info));
}

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
void netGetChangePlayerName(PLAYERNAMECHANGE_PACKET *info) {
  char newName[PLAYER_NAME_LEN]; /* The New Player Name */

  utilPtoCString(info->playerName, newName);
  playersSetPlayerName(screenGetPlayers(), info->playerNum, newName);
}


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
void netLeaveAlliance(BYTE playerNum) {
  ALLIANCE_PACKET info; /* Packet that is sent */
  BYTE buff[128];

  netMakePacketHeader(&(info.h), BOLOLEAVEALLIANCE_DATA);
  info.playerNum = playerNum;
  memcpy(buff, &info, sizeof(ALLIANCE_PACKET));
  netSend(buff, sizeof(info));
}

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
void netRequestAlliance(BYTE playerNum, BYTE reqestTo) {
  ALLIANCE_PACKET info; /* Packet that is sent */
  BYTE buff[128];

  netMakePacketHeader(&(info.h), BOLOREQUESTALLIANCE_DATA);
  info.playerNum = playerNum;
  info.to = reqestTo;
  memcpy(buff, &info, sizeof(ALLIANCE_PACKET));
  netSend(buff, sizeof(info));
}

/*********************************************************
*NAME:          netLostConnection
*AUTHOR:        John Morrison
*CREATION DATE: 1/11/99
*LAST MODIFIED:  8/1/00
*PURPOSE:
* Called when we lose our connection to the current 
* server.
*
*ARGUMENTS:
*
*********************************************************/
void netLostConnection() {
  clientMutexWaitFor();
  screenConnectionLost();
  clientMutexRelease();
#ifdef _WIN32
  MessageBox(windowWnd(), langGetText(NETERR_LOSTCONNECTION), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
#else
  strcpy(messageTitle, DIALOG_BOX_TITLE);
  strcpy(messageBody, langGetText(NETERR_LOSTCONNECTION));
  hasMessage = TRUE;
#endif
  
  netDestroy();
}

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
void netSendTrackerUpdate() {
  INFO_PACKET h; /* Packet to send */

  if (networkGameType == netUdp && netUseTracker == TRUE) {
    memset(&h, 0, sizeof(INFO_PACKET));
    netMakeInfoRespsonse(&h);
    netClientSendUdpTracker((char *) &h, sizeof(h));
    netPacketsPerSecond++;
  }
}

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
int netGetNetTime(void) {
  return dwSysNetTotal;
}

void netErrorOccured(void) {
  netNumErrors++;
}

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
void netSetAllowNewPlayers(bool allow) {
  if (networkGameType == netUdp) {
    if (allow == TRUE) {
      /* We are now allowing players */
      BYTE data[BOLOPACKET_REQUEST_SIZE + 5] = ALLOWPLAYERSHEADER; /* Buffer to send */

      netSend(data, BOLOPACKET_REQUEST_SIZE );
    } else {
      /* We are not */
      BYTE data[BOLOPACKET_REQUEST_SIZE + 5] = DISALLOWPLAYERSHEADER; /* Buffer to send */
      netSend((BYTE *) data, BOLOPACKET_REQUEST_SIZE);
    }
  }
}

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
void netAllianceAccept(BYTE playerNum) {
  ALLIANCE_PACKET ap;
  BYTE buff[128];
  if (playersIsInUse(screenGetPlayers(), playerNum) == TRUE) {
    /* If still valid */
    netMakePacketHeader(&(ap.h), BOLOACCEPTALLIANCE_DATA);
    ap.playerNum = playersGetSelf(screenGetPlayers());
    ap.to = playerNum;
    memcpy(buff, &ap, sizeof(ap));
    netSend(buff, sizeof(ap));
  }
}

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
void netRequestStartPosition() {
  /* We are now allowing players */
  BYTE data[BOLOPACKET_REQUEST_SIZE + 5] = STARTREQUESTHEADER; /* Buffer to send */
  netSend(data, BOLOPACKET_REQUEST_SIZE );
}

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
void netSend(BYTE *buff, int len) {
  BYTE crcA; /* CRC Bytes */
  BYTE crcB;

  buff[len] = udpPacketsGetNextOutSequenceNumber(&udpp);
  CRCCalcBytes(buff, len+1, &crcA, &crcB);

  buff[len+1] = crcA;
  buff[len+2] = crcB;
  udpPacketsSetOutBuff(&udpp, buff[len], buff, len+3);
  netClientSendUdpServer(buff, len+3);
}


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
void netSendQuitMessage() {
  if (netStat != netJoining) {
    BYTE h[sizeof(BOLOHEADER) + BOLO_PACKET_CRC_SIZE + 1];
    netMakePacketHeader((BOLOHEADER *) &h, BOLOPACKET_PACKETQUIT);
    netSend(h, sizeof(BOLOHEADER));
  }
}

