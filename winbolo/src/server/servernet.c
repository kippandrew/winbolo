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
*Name:          serverNet
*Filename:      serverNet.c
*Author:        John Morrison
*Creation Date: 21/02/99
*Last Modified: 16/08/04
*Purpose:
*  Backend network packet handler
*********************************************************/

#include <time.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <netinet/in.h>
#endif
#include "../bolo/global.h"
#include "../bolo/util.h"
#include "../bolo/players.h"
#include "../bolo/netplayers.h"
#include "../bolo/crc.h"
#include "../bolo/netpnb.h"
#include "../bolo/netmt.h"
#include "../bolo/netpnb.h"
#include "../bolo/messages.h"
#include "../bolo/backend.h"
#include "servertransport.h"
#include "servercore.h"
#include "threads.h"
#include "../bolo/netpacks.h"
#include "../bolo/log.h"
#include "../winbolonet/winbolonet.h"
#include "servernet.h"
#include "rsaalgorithm.h"

/* Password in netgames */
char netPassword[MAP_STR_SIZE];
// holder for random string
char rmsg[256];
aiType allowAi;
bool netUseTracker; /* Do we use the tracker or not */
bool serverLock;    /* Is the game locked by server? */
bool hasPass;       /* Do we have a password         */
BYTE snMaxPlayers;  /* Max Players */

netPlayers np; /* Network players status */

int lzwencoding(char *src, char *dest, int len);

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
*  myPort      - UDP port on this machine
*  password    - Password to use to verify joins
*  ai          - Ai Type of the game (allow/disallow/adv)
*  trackerAddr - Address of the tracker to use
*  trackerPort - Port of the tracker
*  useTracker  - Whether to use the tracker or not
*  useAddr     - Address to use for binding else NULL
*  maxPlayers  - Max players this server can handle (0 for
*                unlimited)
**********************************************************/
bool serverNetCreate(unsigned short myPort, char *password, aiType ai, char *trackerAddr, unsigned short trackerPort, bool useTracker, char *useAddr, BYTE maxPlayers) {
  bool returnValue; /* Value to return */
  time_t startTime; /* Start Time if we created the game */

  returnValue = TRUE;
  snMaxPlayers = maxPlayers;
  startTime = 0;
  hasPass = FALSE;
  strcpy(netPassword, password);
  if (netPassword[0] != '\0') {
    hasPass = TRUE;
  }
  netPlayersCreate(&np);
  allowAi = ai;
  serverLock = FALSE;

  returnValue = serverTransportCreate(myPort, useAddr);
  if (returnValue == TRUE) {
    serverTransportSetUs();
    time(&startTime);
    serverCoreSetTimeGameCreated((long) startTime);
    /* Try and set the tracker */
    netUseTracker = useTracker;
    if (useTracker == TRUE) {
      if (serverTransportSetTracker(trackerAddr, trackerPort) == FALSE) {
        screenServerConsoleMessage((char *) "The tracker hostname lookup failed.\n Tracker notification disabled");
        netUseTracker = FALSE;
      }
    }
  }

  if (returnValue == FALSE) {
    serverNetDestroy();
  }

  return returnValue;
}

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
void serverNetDestroy(void) {
  serverTransportDestroy();
  netPlayersDestroy(&np);
}


time_t serverMainGetTicks();

void serverNetCheck(BYTE *ptr, int len) {
  BYTE playerNum;
  BYTE frame;
  time_t t;
  memcpy(&t, ptr+len-sizeof(time_t), sizeof(time_t));
  utilGetNibbles(*(ptr+BOLOPACKET_REQUEST_SIZE), &playerNum, &frame);
  if (netPlayersCheck(&np, playerNum, t, serverMainGetTicks()) == TRUE) {
    serverNetPlayerLeave(playerNum, FALSE);
  }
}


netPlayers *serverNetGetNetPlayers() {
  return &np;
}

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
void serverNetUDPPacketArrive(BYTE *buff, int len, unsigned long addr, unsigned short port) {
  static char info[MAX_UDPPACKET_SIZE] = GENERICHEADER; /* Buffer to send */
  static bool inFix = FALSE;
  static BYTE playerNum; /* Player number that sent us this packet. Used only in PosData */
  static BYTE dummy; /* Used to hold unused nibble to get player Number. Used only in PosData */
  static BYTE crcA;  /* First CRC Byte */
  static BYTE crcB;  /* Second CRC Byte */
  static BYTE sequenceNumber; /* Sequence number */
  udpPackets udpp;
  char *ip; /* Variable 'addr' in character form */
  struct sockaddr_in infoPacketAddress; /* Address in octet format */
  char serverConsoleMessage[255]; /* Store the message to be put to the console */

/*
  printf("Received pack of type: %d \r\n", buff[BOLOPACKET_REQUEST_TYPEPOS]);
  printf("Received pack of length: %i \r\n", len);
  printf("Received pack of rsa_packet: %i \r\n", sizeof(RSA_PACKET));
*/

  if ((strncmp(buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0) && buff[BOLO_VERSION_MAJORPOS] == BOLO_VERSION_MAJOR && buff[BOLO_VERSION_MINORPOS] == BOLO_VERSION_MINOR && buff[BOLO_VERSION_REVISIONPOS] == BOLO_VERSION_REVISION) {
    if (len == sizeof(PING_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PINGREQUEST) {
      /* Ping packet */
      PING_PACKET *pp;
      int pos;
      playerNum = netPlayersGetPlayerNumber(&np, addr, port);
      if (playerNum < MAX_TANKS && playersIsInUse(screenGetPlayers(), playerNum)) {
        pp = (PING_PACKET *) buff;
        udpp = netPlayersGetUdpPackets(&np, playerNum);
      	pos = 1 + pp->outPacket;;
	      if (pos == MAX_UDP_SEQUENCE) {
          pos = 0;
        }

        if (udpPacketsGetInSequenceNumber(&udpp) != pos && inFix == FALSE) {
          REREQUEST_PACKET rrp;
          serverNetMakePacketHeader(&(rrp.h), BOLOPACKET_PACKETREREQUEST);
          rrp.nonReliable = UDP_NON_RELIABLE_PACKET;
          rrp.rerequestItem = udpPacketsGetInSequenceNumber(&udpp);
          CRCCalcBytes((BYTE *) &rrp, sizeof(REREQUEST_PACKET)-2 , &(rrp.crcA), &(rrp.crcB));
          serverTransportSendUDPLast((BYTE *) &rrp, sizeof(rrp), FALSE);
        }
        pp->h.type  = BOLOPACKET_PINGRESPONSE;
        pp->inPacket = udpPacketsGetInSequenceNumber(&udpp);
        pp->outPacket = udpPacketsGetOutSequenceNumber(&udpp);
        serverTransportSendUDPLast(buff, len, FALSE);
      }
    } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_RETRANSMITTED_PACKETS) {
       BYTE count;
       unsigned short us;
       int pos;
       count = 0;
       pos = BOLOPACKET_REQUEST_TYPEPOS+2;
       inFix = TRUE;
       while (count < buff[BOLOPACKET_REQUEST_TYPEPOS+1]) {
         memcpy(&us, buff+pos, 2);
         pos += 2;
         serverNetUDPPacketArrive(buff+pos, us, addr, port);
         pos += us;
         count++;
       }
       inFix = FALSE;

    } else if (len == BOLOPACKET_REQUEST_SIZE && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_INFOREQUEST) {
     /* Make info response packet */
      INFO_PACKET h;
      infoPacketAddress.sin_family = AF_INET;
#ifdef _WIN32
      infoPacketAddress.sin_addr.S_un.S_addr = addr;
#else
      infoPacketAddress.sin_addr.s_addr = addr;
#endif
      ip = inet_ntoa(infoPacketAddress.sin_addr); /* Store the IP of the requester in string format */
      strcpy(serverConsoleMessage,(char *)"Info packet request from ");
      strcat(serverConsoleMessage, ip);
      screenServerConsoleMessage(serverConsoleMessage);

      memset(&h, 0, sizeof(h));
      serverNetMakeInfoRespsonse(&h);
      memcpy(info, &h, sizeof(h));
      serverTransportSendUDPLast(info, sizeof(h), FALSE);
    } else {
      /* Person must be in game 
         Check CRC
         Check packet position
         then process
      */
      BYTE playerNum;

      playerNum = netPlayersGetPlayerNumber(&np, addr, port);

      if (playerNum != NET_PLAYERS_NOT_FOUND || buff[len-3] == UDP_NON_RELIABLE_PACKET) { // || buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PLAYERDATAREQUEST || buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PASSWORDCHECK  || buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_NAMECHECK || buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PLAYERNUMREQUEST || buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_SERVERKEYREQUEST || buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOREJOINREQUEST) {

        crcA = buff[len-2];
        crcB = buff[len-1];
        if (CRCCheck(buff, len - BOLO_PACKET_CRC_SIZE, crcA, crcB) == TRUE) {
          /* Valid CRC */
          len -= 2; /* Remove CRC bytes */
          sequenceNumber = buff[len-1];
          if (sequenceNumber == UDP_NON_RELIABLE_PACKET) {
            /* Non reliable marker */
            len--;
            if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PACKETREREQUEST && netPlayersGetInUse(&np, playerNum) == TRUE) {
              BYTE upto;
              BYTE high;
      	      BYTE count=0;
              unsigned short us;
              udpp = netPlayersGetUdpPackets(&np, playerNum);
              upto = buff[BOLOPACKET_REQUEST_TYPEPOS+1];
              high = udpPacketsGetOutSequenceNumber(&udpp);
              high++;
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
              serverTransportSendUDPLast(info, len, TRUE);
            } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_SERVERKEYREQUEST) {
              info[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_SERVERKEYRESPONSE;
              winboloNetGetServerKey(info + sizeof(BOLOHEADER));
              serverTransportSendUDPLast(info, sizeof(BOLOHEADER) + WINBOLONET_KEY_LEN, FALSE);
            } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPOSITION_DATA) {
              /* Position packet */
              BYTE *ptr;
              struct sockaddr_in sAddr;
              ptr = buff + BOLOPACKET_REQUEST_SIZE;
              serverNetCheck(buff, len);

              threadsWaitForMutex();
              
              serverCoreSetPosData(ptr);
              
              /* Reset the players address and port for routers that change them */
              utilGetNibbles(*ptr, &playerNum, &dummy);
              sAddr.sin_family = AF_INET;
              #ifdef _WIN32
                sAddr.sin_addr.S_un.S_addr = addr;
              #else
                sAddr.sin_addr.s_addr = addr;
              #endif
              sAddr.sin_port=port;
              netPlayersSetAddr(&np, playerNum, &sAddr);
              threadsReleaseMutex();
            } else if (len == sizeof(REJOINREQUEST_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOREJOINREQUEST) {
              serverCoreRequestRejoin(buff[BOLOPACKET_REQUEST_TYPEPOS+1]);
			} else if (len == sizeof(RSA_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_RSACHECK) {
			  /* RSA check packet */
			  RSA_PACKET rsap;
			  BIGD randommessage;
			  memcpy(&rsap, buff, sizeof(rsap));
			  // make a packet of type rsaresponse
			  serverNetMakePacketHeader(&(rsap.h), BOLOPACKET_RSARESPONSE);
			  // initalize the random message variable
			  randommessage = bdNew();
			  // generate random message the second number is in bits, not bytes.
			  bdRandomBits(randommessage, 512);
		  	  //pr_msg("random=\n", randommessage);
			  // convert the message over to a hexidecimal string
			  bdConvToHex(randommessage, rmsg, 128);
			  bdFree(&randommessage);
			  strcpy(rsap.rsa,rmsg);
			  memcpy(info,&rsap,sizeof(rsap));
			  // send the random hex string off to the client
			  serverTransportSendUDPLast(info, sizeof(rsap), TRUE);
		
            } else if (len == sizeof(PASSWORD_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PASSWORDCHECK) { 
             /* Password check packet */
			  PASSWORD_PACKET pp;
              BOLOHEADER h;
              memcpy(&pp, buff, sizeof(pp));
              utilPtoCString(pp.password, info);
              if (strcmp(info, netPassword) != 0) {
                serverNetMakePacketHeader(&h, BOLOPACKET_PASSWORDFAIL);
				screenServerConsoleMessage((char *)"Password Failed");
              } else {
                serverNetMakePacketHeader(&h, BOLOPACKET_PASSWORDACCEPT);
				screenServerConsoleMessage((char *)"Password Accepted");
              } 
              memcpy(info, &h, sizeof(h));
              serverTransportSendUDPLast(info, sizeof(h), TRUE);
              } else if (len == sizeof(PLAYERNAME_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_NAMECHECK) { 
               /* Player name check packet */
                PLAYERNAME_PACKET pn;
                BOLOHEADER h;
            		BYTE numPlayers;

                memcpy(&pn, buff, sizeof(pn));
                utilPtoCString(pn.playerName, info);
                threadsWaitForMutex();
            		numPlayers = playersGetNumPlayers(screenGetPlayers());
            		if (serverLock == TRUE || (numPlayers > 0 && netPlayersIsLocked(&np) == TRUE)) {
                  serverNetMakePacketHeader(&h, BOLOPACKET_GAMELOCKED);
            		} else if (snMaxPlayers > 0 && numPlayers >= snMaxPlayers) {
                  serverNetMakePacketHeader(&h, BOLOPACKET_MAXPLAYERS);
		            } else if (playersNameTaken(screenGetPlayers(), info) == TRUE) {
                  serverNetMakePacketHeader(&h, BOLOPACKET_NAMEFAIL);
                } else {
                  serverNetMakePacketHeader(&h, BOLOPACKET_NAMEACCEPT);
                }
                threadsReleaseMutex();
                memcpy(info, &h, sizeof(h));
                serverTransportSendUDPLast(info, sizeof(h), TRUE);
              } else if (len == sizeof(PLAYERNUM_REQ_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PLAYERNUMREQUEST) { 
                /* Player Number request */
                threadsWaitForMutex();
                serverNetPlayerNumReq(buff, len, addr, port);
                threadsReleaseMutex();
              } else if (len == sizeof(PLAYERDATA_REQPACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PLAYERDATAREQUEST) {
                /* Player data request */
                threadsWaitForMutex();
                serverNetPlayerDataReq();
                threadsReleaseMutex();

              } else {
                screenServerConsoleMessage((char *) "Bad unknown unreliable packet ");
              }

          } else {
            udpp = netPlayersGetUdpPackets(&np, playerNum);
            if (sequenceNumber != udpPacketsGetInSequenceNumber(&udpp)) {
              /* Rerequest *
              REREQUEST_PACKET rrp;
              serverNetMakePacketHeader(&(rrp.h), BOLOPACKET_PACKETREREQUEST);
              rrp.nonReliable = UDP_NON_RELIABLE_PACKET;
              rrp.rerequestItem = udpPacketsGetInSequenceNumber(&udpp);           
              CRCCalcBytes((BYTE *) &rrp, sizeof(REREQUEST_PACKET)-2 , &(rrp.crcA), &(rrp.crcB));
              serverTransportSendUDPLast((BYTE *) &rrp, sizeof(rrp), FALSE);
//              screenServerConsoleMessage((char *) "Bad sequence number - Rerequesting"); */
            } else {
              udpPacketsGetNextInSequenceNumber(&udpp);
              serverNetTCPPacketArrive(buff, len-1, playerNum, addr, port);
            }
          }
        } else {
           printf("CRC Error %d, %d\n", crcA, crcB); 
        }
      } 
    }
  }
}

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
void serverNetTCPPacketArrive(BYTE *buff, int len, BYTE playerNum, unsigned long addr, unsigned short port) {
  static char info[MAX_UDPPACKET_SIZE] = GENERICHEADER; /* Buffer to send */
  BYTE *pnt;                                  /* Data Pointer   */

   /* Server password check */
  if (hasPass == TRUE) {
    if (netPlayersHasPassed(&np, playerNum) == FALSE) {
      /* If this isn't a password packet drop them */
      if (len != sizeof(PASSWORD_PACKET) || buff[BOLOPACKET_REQUEST_TYPEPOS] != BOLOPACKET_PASSWORDCHECK) {
        serverNetPlayerLeave(playerNum, FALSE);
        return;
      }
    }
  }

  /* Server RSA check */
  if (netPlayersHasPassedRsa(&np, playerNum) == FALSE){
	  /* If this isn't a RSA packet drop them */
	  if (len != sizeof(RSA_PACKET)||buff[BOLOPACKET_REQUEST_TYPEPOS] != BOLOPACKET_RSACHECK){
		  serverNetPlayerLeave(playerNum, FALSE);
		  return;
	  }
  }

  memset(info, 0, MAX_TCPPACKET_SIZE);
  serverNetMakePacketHeader((BOLOHEADER *) info, 0);
  threadsWaitForMutex();

  if (len == sizeof(BOLOHEADER) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PACKETQUIT) {
    /* This player has quit */
    serverNetPlayerLeave(playerNum, TRUE);
  } else if (len == BOLOPACKET_REQUEST_SIZE && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_BASESDATAREQUEST) { 
    /* Wants bases data */
    BYTE len;
    info[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_BASESDATARESPONSE;
    pnt = info;
    pnt += BOLOPACKET_REQUEST_TYPEPOS+1;
    len = serverCoreGetBaseNetData(pnt);
    serverNetSendPlayer(playerNum, info, (BOLOPACKET_REQUEST_SIZE + len));
  } else if (len == BOLOPACKET_REQUEST_SIZE && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_STARTSDATAREQUEST) { 
    /* Wants Starts data */
    BYTE len;
    info[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_STARTSDATARESPONSE;
    pnt = info;
    pnt += BOLOPACKET_REQUEST_TYPEPOS+1;
    len = serverCoreGetStartsNetData(pnt);
    serverNetSendPlayer(playerNum, info, (BOLOPACKET_REQUEST_SIZE + len));
    netPlayersSetInGame(&np, playerNum, TRUE);
  } else if (len == BOLOPACKET_REQUEST_SIZE && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_PILLSDATAREQUEST) { 
    /* Wants Pillbox data */
    BYTE len;
    info[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_PILLSDATARESPONSE;
    pnt = info;
    pnt += BOLOPACKET_REQUEST_TYPEPOS+1;
    len = serverCoreGetPillNetData(pnt);
    serverNetSendPlayer(playerNum, info, (BOLOPACKET_REQUEST_SIZE + len));
  } else if (len == sizeof(MAPDOWNLOADREQUEST_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_MAPDATAREQUEST) { 
    /* Wants Map download data */
    unsigned short dataLen;
    BYTE *savePtr;

    pnt = info;
    info[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_MAPDATARESPONSE;
    info[BOLOPACKET_MAPDDOWNLOAD_YPOS] = buff[BOLOPACKET_MAPDDOWNLOAD_YPOS];
    pnt += BOLOPACKET_MAPDDOWNLOAD_YPOS+1;
    savePtr = pnt;
    pnt += 2;
    memset(pnt, 0, MAX_UDPPACKET_SIZE-20);
    dataLen = serverCoreMakeMapNetRun(pnt, buff[BOLOPACKET_MAPDDOWNLOAD_YPOS]);
    memcpy(savePtr, &dataLen, sizeof(dataLen));
    serverNetSendPlayer(playerNum, info, sizeof(BOLOHEADER) + sizeof(BYTE) + sizeof(unsigned short) + dataLen);
  } else if (len == BOLOPACKET_REQUEST_SIZE && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_TIMEREQUEST) { 
    /* Wants Time data */
    TIME_PACKET tp = TIMEHEADER;
    tp.start_delay = serverCoreGetGameStartDelay();
    tp.time_limit = serverCoreGetGameTimeLeft();
    memcpy(info, &tp, sizeof(TIME_PACKET));
    serverNetSendPlayer(playerNum, info, sizeof(TIME_PACKET));
  } else if (len == sizeof(PLAYERNAMECHANGE_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOCHANGENAME_DATA) {
    /* Change Player Name packet */
    serverNetChangePlayerName(playerNum, buff);
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_MESSAGE_ALL_PLAYERS) {
    /* Message to all players */
    serverNetSendAllExceptPlayer(playerNum, buff, len);
    logAddEvent(log_MessageAll, playerNum, 0, 0, 0, 0, (char *) (buff + BOLOPACKET_REQUEST_TYPEPOS + 3));
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_MESSAGE) {
    /* Message to a certain player */
    serverNetSendPlayer(buff[BOLOPACKET_REQUEST_TYPEPOS+2], buff, len);
    logAddEvent(log_MessagePlayers, playerNum, buff[BOLOPACKET_REQUEST_TYPEPOS+2],0, 0, 0, buff+BOLOPACKET_REQUEST_TYPEPOS+3);
    //serverTransportSendTCPData(netPlayersGetSockNum(&np, buff[BOLOPACKET_REQUEST_TYPEPOS+2]), buff, len);
  } else if (buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOCLIENT_DATA) {
    /* Client Data */
    severNetProcessClientData(buff, len, playerNum);
  } else if (len == sizeof(ALLIANCE_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOLEAVEALLIANCE_DATA) {
    /* Player leave alliance */
    playersLeaveAlliance(screenGetPlayers(), buff[BOLOPACKET_REQUEST_TYPEPOS+1]);
    winbolonetAddEvent(WINBOLO_NET_EVENT_ALLY_LEAVE, TRUE, buff[BOLOPACKET_REQUEST_TYPEPOS+1], WINBOLO_NET_NO_PLAYER);
    logAddEvent(log_AllyLeave, buff[BOLOPACKET_REQUEST_TYPEPOS+1], 0, 0, 0, 0, NULL);
    serverNetSendAll(buff, len);
  } else if (len == BOLOPACKET_REQUEST_SIZE && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOALLOWNEWPLAYERS) {
    /* Client is allowing new players */
    BYTE *ptr;
    char str[FILENAME_MAX];
    info[BOLOPACKET_REQUEST_TYPEPOS] = BOLOSERVERMESSAGE; 
    ptr = info;
    ptr += BOLOPACKET_REQUEST_TYPEPOS+1;

    playersGetPlayerName(screenGetPlayers(), playerNum, str);
    strcat(str, (char *) " is now allowing players to join.");
    utilCtoPString(str, ptr);
    logAddEvent(log_MessageServer, 0, 0, 0, 0, 0, (char *) ptr);
    serverNetSendAll(info, BOLOPACKET_REQUEST_SIZE +(*ptr)+1);
    if (netPlayersIsLocked(&np) == TRUE) {
      /* Send out game is unlocked message */
      utilCtoPString((char *) "This game is now unlocked to new players (client unlock)", ptr);
      logAddEvent(log_MessageServer, 0, 0, 0, 0, 0, (char *) ptr);
      serverNetSendAll(info, BOLOPACKET_REQUEST_SIZE +(*ptr)+1);  
      if (serverLock == FALSE) {
        winboloNetSendLock(FALSE);
      }
    }
    netPlayersSetLock(&np, playerNum, FALSE);
  } else if (len == BOLOPACKET_REQUEST_SIZE && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLONOALLOWNEWPLAYERS) {
    /* Client is not allowing new players */
    BYTE *ptr;
    char str[FILENAME_MAX];
    info[BOLOPACKET_REQUEST_TYPEPOS] = BOLOSERVERMESSAGE; 
    ptr = info;
    ptr += BOLOPACKET_REQUEST_TYPEPOS +1;

    playersGetPlayerName(screenGetPlayers(), playerNum, str);
    strcat(str, (char *) " is now not allowing players to join.");
    utilCtoPString(str, ptr);
    logAddEvent(log_MessageServer, 0, 0, 0, 0, 0, (char *) ptr);
    serverNetSendAll(info, BOLOPACKET_REQUEST_SIZE +(*ptr)+1);
    netPlayersSetLock(&np, playerNum, TRUE);
    if (netPlayersIsLocked(&np) == TRUE) {
      /* Send out game is locked message */
      utilCtoPString((char *) "This game is now locked to new players (client lock)", ptr);
      logAddEvent(log_MessageServer, 0, 0, 0, 0, 0, (char *) ptr);
      serverNetSendAll(info, BOLOPACKET_REQUEST_SIZE +(*ptr)+1);   
      winboloNetSendLock(TRUE);
    }

  } else if (len == sizeof(ALLIANCE_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOREQUESTALLIANCE_DATA) {
    /* Player Request alliance */
    serverNetSendPlayer(buff[BOLOPACKET_REQUEST_TYPEPOS+2], buff, len);
    logAddEvent(log_AllyRequest, buff[BOLOPACKET_REQUEST_TYPEPOS+1], buff[BOLOPACKET_REQUEST_TYPEPOS+2], 0, 0, 0, NULL);
  } else if (len == sizeof(ALLIANCE_PACKET) && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOACCEPTALLIANCE_DATA) {
    /* Player is accepted into an alliance */
    playersAcceptAlliance(screenGetPlayers(), buff[BOLOPACKET_REQUEST_TYPEPOS+1], buff[BOLOPACKET_REQUEST_TYPEPOS+2]);
    winbolonetAddEvent(WINBOLO_NET_EVENT_ALLY_JOIN, TRUE, buff[BOLOPACKET_REQUEST_TYPEPOS+1], buff[BOLOPACKET_REQUEST_TYPEPOS+2]);
    logAddEvent(log_AllyAccept, buff[BOLOPACKET_REQUEST_TYPEPOS+1], buff[BOLOPACKET_REQUEST_TYPEPOS+2], 0, 0, 0, NULL);
    serverNetSendAll(buff, len);   
  } else if (len == BOLOPACKET_REQUEST_SIZE && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOREQUEST_STARTPOS) {
    /* Start position request */
    BYTE *ptr;
    STARTPOSITION_PACKET spp = STARTRESPONSEHEADER;
    
    serverCoreGetStartPosition(playerNum, &(spp.x), &(spp.y),  &(spp.angle), &(spp.numShells), &(spp.numMines));
    ptr = info;
    ptr += BOLOPACKET_REQUEST_TYPEPOS +1;
    memcpy(info, &spp, sizeof(STARTPOSITION_PACKET));
    serverNetSendPlayer(playerNum, info, sizeof(STARTPOSITION_PACKET));
  }
  threadsReleaseMutex();
}

/*********************************************************
*NAME:          serverNetMakeInfoRespsonse
*AUTHOR:        John Morrison
*CREATION DATE: 15/8/99
*LAST MODIFIED: 24/12/99
*PURPOSE:
* A request for an info packet has been made. Make a 
* response packet.
*
*ARGUMENTS:
*  buff - Buffer to hold the info packet.
*********************************************************/
void serverNetMakeInfoRespsonse(INFO_PACKET *buff) {
  char dummy[MAP_STR_SIZE]; /* Used to get the map name before conversion to pascal string */

  /* Make header */
  serverNetMakePacketHeader(&(buff->h), BOLOPACKET_INFORESPONSE);
  /* Make game ID */
  buff->gameid.start_time = htonl(serverCoreGetTimeGameCreated());
  serverTransportGetUs(&(buff->gameid.serveraddress), &(buff->gameid.serverport));
  buff->gameid.serverport = htons(buff->gameid.serverport);
  buff->gameid.serveraddress.s_addr = 0;
  /* Get the map name and convert to a pascal string */
  serverCoreGetMapName(dummy);
  utilCtoPString(dummy, buff->mapname);
  
  /* Make game variables */
  buff->gametype = serverCoreGetActualGameType();
  if (serverCoreGetAllowHiddenMines() == TRUE) {
    buff->allow_mines = HIDDEN_MINES;
  } else {
    buff->allow_mines = ALL_MINES_VISIBLE;
  }
  buff->allow_AI = allowAi;
  buff->spare1 = 0;
  buff->start_delay = htonl(serverCoreGetGameStartDelay());
  buff->time_limit = htonl(serverCoreGetGameTimeLeft());
  buff->num_players = serverCoreGetNumPlayers();
  buff->free_pills = serverCoreGetNumNeutralPills();
  buff->free_bases = serverCoreGetNumNeutralBases();
  buff->has_password = netPassword[0];
  buff->spare2 = 0;
}

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
void serverNetMakePacketHeader(BOLOHEADER *hdr, BYTE packetType) {
  strncpy(hdr->signature, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE);
  hdr->versionMajor = BOLO_VERSION_MAJOR;
  hdr->versionMinor = BOLO_VERSION_MINOR;
  hdr->versionRevision = BOLO_VERSION_REVISION;
  hdr->type = packetType;
}

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
void serverNetPlayerNumReq(BYTE *buff, int len, unsigned long addr, unsigned short port) {
  PLAYERNUM_PACKET pnp;                       /* Packet to send back to the requesting player */
  PLAYERNUM_REQ_PACKET prp;                   /* The request player number packet */
  NEWPLAYER_PACKET npp;						  /* New Player Packet */
  RSA_PACKET rsap;							  /* RSA Packet */
  char info[MAX_UDPPACKET_SIZE] = GENERICHEADER; /* Buffer to send */
  char ipStr[FILENAME_MAX];                   /* IP Address as a string */
  char *ip;                                   /* String representing IP of player */
  struct in_addr dummyAddr;                   /* Dummy variables so we can get the player address */
  unsigned short dummyPort;                   /* if connected via localhost */
  char str[FILENAME_MAX];                     /* Used for message printing */
  struct sockaddr_in sAddr;
  char *tok;
  char rsadecryptedholder[RSA_DATA_SIZE];
  BIGD m1,c,d,n;
  BYTE ip1;
  BYTE ip2;
  BYTE ip3;
  BYTE ip4;
  
  memset(rsadecryptedholder, 0, RSA_DATA_SIZE);
  /* Check Name not in use */
  memset(&prp, 0, sizeof(prp));
  memcpy(&prp, buff, sizeof(prp));
  utilPtoCString(prp.password, info);
  if (strcmp(info, netPassword) != 0) {
    /* Passwords do not match */
  	return;
  }

  // check the decrypted random string is correct
  // decrypt the string here
  // test the string to match rmsg here
	m1 = bdNew();
	c = bdNew();
	d = bdNew();
	n = bdNew();
	strcpy(rsadecryptedholder,prp.rsaencrypted);
//	printf("rsaencrypted: %s \r\n", rsadecryptedholder);
//	rsadecryptedholder[256]='\0';
	bdConvFromHex(c,rsadecryptedholder);
	bdConvFromHex(d,"75dbe8cb03e866b1810263a8be193914929bda1b75b97164bd6543839b9120e34938b7062c362498d30c3f3a28e36426e4e4f80e141f254f80575dc62a291a754d5112ee6c043d83cfe291370a3c630ad65f7c1c619eb09198883c32d32ae2e8b658a21909f20fd61ff29c65797d0ad752eaf4102ebac63ed56c8c50bd98a9ab");
	bdConvFromHex(n,"b0c9dd3085dc9a0a4183957d1d25d59edbe9c72930962a171c17e5456959b154edd51289425136e53c925ed73d55163a575774151e2eb7f740830ca93f3da7b19e30514b1ee09246d7bf56c1a8e2e9c5c7c197c60849e5f7ce3a9691b6d17c825741341904b316a98bd8d160f939300ee0c4042d5428f22de095f606f120e1d7");
	// This is the decryption
	/* Check decrypt m1 = c^d mod n */
	bdModExp(m1, c, d, n);
//	below is commented out debugging code stuff for the rsa code.
//	pr_msg("m1= ", m1);
//	pr_msg("c= ", c);
//	pr_msg("d= ", d);
//	pr_msg("n= ", n);
	memset(rsadecryptedholder, 0, RSA_DATA_SIZE);
	bdConvToHex(m1,rsadecryptedholder,RSA_DATA_SIZE);
	bdFree(&m1);
	bdFree(&c);
	bdFree(&d);
	bdFree(&n);

//	below is commented out debugging code stuff for the rsa code.
//  printf("rmsg: %s \r\n", rmsg);
//  printf("decrypted: %s \r\n", rsadecryptedholder);

  if (strcmp(rsadecryptedholder, rmsg) != 0) {
    /* randomstring doesn't match so, disconnect them*/
	/* Name in use  - Just send back a incorrect packet */
	serverNetMakePacketHeader(&(rsap.h), BOLOPACKET_RSAFAIL);
	printf("RSA Authorization Failed\r\n");
	/* Send reply */
    memcpy(info, &rsap, sizeof(rsap));
    serverTransportSendUDPLast(info, sizeof(rsap), TRUE);
	return;
  }

  utilPtoCString(prp.playerName, info);
	                      
  if (playersNameTaken(screenGetPlayers(), info) == FALSE && prp.playerName[0] != '*') {
    /* Name not taken - Add to players*/
    serverNetMakePacketHeader(&(pnp.h), BOLOPACKET_PLAYERNUMRESPONSE);
    memset(&npp, 0, sizeof(npp));
#ifdef _WIN32    
    npp.addr.S_un.S_addr = addr;
#else
    npp.addr.s_addr = addr;
#endif
    npp.port = port;
    pnp.playerNumber = playersGetFirstNotUsed(screenGetPlayers());
    sAddr.sin_port=port;
#ifdef _WIN32
    sAddr.sin_addr.S_un.S_addr = addr;
#else
    sAddr.sin_addr.s_addr = addr;
#endif
    netPlayersSetPlayer(&np, pnp.playerNumber, &sAddr); 
    netPlayersDonePassword(&np, pnp.playerNumber);
	netPlayersDoneRsa(&np,pnp.playerNumber);
	basesUpdateTimer(pnp.playerNumber);
    /* Verify with wbn if playing */
    if (prp.key[0] != EMPTY_CHAR) {
      if (winboloNetVerifyClientKey(prp.key, info, pnp.playerNumber) == TRUE) {
        /* We could verify their key */
	      info[0] = '*';
        utilPtoCString(prp.playerName, info+1);
      } else {
        utilPtoCString(prp.playerName, info);
      }
    }

    
    ip = inet_ntoa(npp.addr);
    strcpy(ipStr, ip);
    if (strcmp(ipStr, "127.0.0.1") == 0) {
      /* The joined from localhost */
      serverTransportGetUs(&dummyAddr, &dummyPort);
      ip = inet_ntoa(dummyAddr);
    }
    playersSetPlayer(screenGetPlayers(), pnp.playerNumber, info, ip, 0, 0, 0, 0, 0, 0, 0, NULL);
    serverCorePlayerJoin(pnp.playerNumber);
    sprintf(str, (char *) "New Player %s accepted into game.\n", info);
    screenServerConsoleMessage(str);

    /* Log it */
    tok = strtok(ipStr, ".");
    ip1 = (BYTE) atoi(tok);
    tok = strtok(NULL, ".");
    ip2 = (BYTE) atoi(tok);
    tok = strtok(NULL, ".");
    ip3 = (BYTE) atoi(tok);
    tok = strtok(NULL, ".");
    ip4 = (BYTE) atoi(tok);
    tok = strtok(NULL, ".");
    logAddEvent(log_PlayerJoined, pnp.playerNumber, ip1, ip2, ip3, ip4, prp.playerName);
    /* Inform other players */   
    serverNetMakePacketHeader(&(npp.h), BOLOPACKET_PLAYERNEWPLAYER);
    
    npp.playerNumber = pnp.playerNumber;
    utilCtoPString(info, npp.playerName);
    memcpy(info, &npp, sizeof(npp));
    serverNetSendAllExceptPlayer(npp.playerNumber, info, sizeof(npp));
    winbolonetAddEvent(WINBOLO_NET_EVENT_PLAYER_JOIN, TRUE, npp.playerNumber, WINBOLO_NET_NO_PLAYER);
  } else {
    /* Name in use  - Just send back a incorrect packet */
    serverNetMakePacketHeader(&(pnp.h), BOLOPACKET_NAMEFAIL);    
  }

  /* Send reply */
  memcpy(info, &pnp, sizeof(pnp));
  serverTransportSendUDPLast(info, sizeof(pnp), TRUE);
}

/*********************************************************
*NAME:          serverNetSendPlayerLeaveGracefulMessage
*AUTHOR:        John Morrison
*CREATION DATE: 19/12/03
*LAST MODIFIED: 19/12/03
*PURPOSE:
* Sends the "is quiting" message to all clients
*
*ARGUMENTS:
*
*********************************************************/
void serverNetSendPlayerLeaveGracefulMessage(BYTE playerNum) {
  BYTE buff[MAX_UDPPACKET_SIZE] = SEVERMSGHEADER;
  BYTE *ptr;
  char str[256];
  
  ptr = buff;
  ptr += BOLOPACKET_REQUEST_TYPEPOS +1;
  playersGetPlayerName(screenGetPlayers(), playerNum, str);
  strcat(str, (char *) " is quitting.");

  /* Send message to clients */
  utilCtoPString(str, ptr);
//FIXME send to WBN  logAddEvent(log_MessageServer, 0, 0, 0, 0, 0, (char *) ptr);
  serverNetSendAllExceptPlayer(playerNum, buff, BOLOPACKET_REQUEST_SIZE +(*ptr)+1);
  winbolonetAddEvent(WINBOLO_NET_EVENT_QUITTING, TRUE, playerNum, WINBOLO_NET_NO_PLAYER);
  logAddEvent(log_PlayerLeaving, playerNum, 0, 0, 0, 0, NULL);
}

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
void serverNetPlayerLeave(BYTE playerNum, bool graceful) {
  PLAYERLEAVE_PACKET plp; /* Packet send to all players saying who left */

  /* Remove it */
  basesRemoveTimer(playerNum);
  playerNum = netPlayersRemovePlayerNum(&np, playerNum);
  if (playerNum != NEUTRAL) {
    threadsWaitForMutex();
    if (graceful == TRUE) {
      serverNetSendPlayerLeaveGracefulMessage(playerNum);
    }
    logAddEvent(log_PlayerQuit, playerNum, 0, 0, 0, 0, NULL);
    /* Tell the other players about it */
    serverNetMakePacketHeader(&(plp.h), BOLOPACKET_PLAYERLEAVE);
    plp.playerNumber = playerNum;
    serverNetSendAllExceptPlayer(playerNum, (BYTE *) &plp, sizeof(plp));
    /* Tell WinBolo.net */
    winboloNetClientLeaveGame(playerNum, playersGetNumPlayers(screenGetPlayers()), serverCoreGetNumNeutralBases(), serverCoreGetNumNeutralPills());

    serverCorePlayerLeave(playerNum);
    threadsReleaseMutex();
  }
  if (playersGetNumPlayers(screenGetPlayers()) == 0 && serverLock == FALSE) {
    /* Unlock the game */
    winboloNetSendLock(FALSE);
  }
}

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
void serverNetPlayerDataReq() {
  BYTE *sendBuff;
  PLAYERDATA_PACKET pdp;  /* The packet to send back */
  BYTE count;             /* Looping variable */

  count = 0;
  memset(&pdp, 0, sizeof(pdp));
  serverNetMakePacketHeader(&(pdp.h), BOLOPACKET_PLAYERDATARESPONSE);
  sendBuff = malloc(2048);

  while (count < MAX_TANKS) {
    if (playersIsInUse(screenGetPlayers(), count) == TRUE) {
      pdp.items[count].inUse = TRUE;
      playersGetPlayerName(screenGetPlayers(), count, sendBuff);
      utilCtoPString(sendBuff, pdp.items[count].playerName);
      pdp.items[count].numAllies = playersMakeNetAlliences(screenGetPlayers(), count, pdp.items[count].allies);
      netPlayersGetPlayerDataReq(&np, count, &(pdp.items[count].addr), &(pdp.items[count].port));
    }
    count++;
  } 
  
  memcpy(sendBuff, &pdp, sizeof(pdp));
  serverTransportSendUDPLast(sendBuff, sizeof(pdp), TRUE);
  free(sendBuff);
}


void serverNetSendPlayer(BYTE playerNum, BYTE *buff, int len) {
  BYTE crcA;
  BYTE crcB;
  udpPackets udp;

  if (netPlayersGetInUse(&np, playerNum) == TRUE) {
    udp = netPlayersGetUdpPackets(&np, playerNum);
    buff[len] = udpPacketsGetNextOutSequenceNumber(&udp);
    CRCCalcBytes(buff, len+1, &crcA, &crcB);

    buff[len+1] = crcA;
    buff[len+2] = crcB;
    udpPacketsSetOutBuff(&udp, buff[len], buff, len+3);
    serverTransportSendUDP(buff, len+3, netPlayersGetAddr(&np, playerNum));
  }
}

void serverNetSendAllExceptPlayer(BYTE playerNum, BYTE *buff, int len) {
  BYTE crcA;
  BYTE crcB;
  BYTE count;
  udpPackets udp;

  count = 0;
  while (count < MAX_TANKS) {
    if (count != playerNum && netPlayersGetInUse(&np, count) == TRUE) {
      /* Send it */
      udp = netPlayersGetUdpPackets(&np, count);

      buff[len] = udpPacketsGetNextOutSequenceNumber(&udp);
      CRCCalcBytes(buff, len+1, &crcA, &crcB);

      buff[len+1] = crcA;
      buff[len+2] = crcB;
      udpPacketsSetOutBuff(&udp, buff[len], buff, len+3);
      serverTransportSendUDP(buff, len+3, netPlayersGetAddr(&np, count));
    }
    count++;
  }
}

void serverNetSendAll(BYTE *buff, int len) {
  BYTE crcA;
  BYTE crcB;
  BYTE count;
  udpPackets udp;

  count = 0;
  while (count < MAX_TANKS) {
    if (netPlayersGetInUse(&np, count) == TRUE) {
      /* Send it */
      udp = netPlayersGetUdpPackets(&np, count);

      buff[len] = udpPacketsGetNextOutSequenceNumber(&udp);
      CRCCalcBytes(buff, len+1, &crcA, &crcB);
      buff[len+1] = crcA;
      buff[len+2] = crcB;
      udpPacketsSetOutBuff(&udp, buff[len], buff, len+3);
      serverTransportSendUDP(buff, len+3, netPlayersGetAddr(&np, count));
    }
    count++;
  }
}

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
*  playerNum - The player number it came from
*  buff    - Buffer that has arrived.
*********************************************************/
void serverNetChangePlayerName(BYTE playerNum, BYTE *buff) {
  PLAYERNAMECHANGE_PACKET *pncp;
  char name[512];

  if (winboloNetIsPlayerParticipant(playerNum) == FALSE) {
    pncp = (PLAYERNAMECHANGE_PACKET*) buff;
    /* Send to everyone */
    serverNetSendAllExceptPlayer(playerNum, buff, sizeof(*pncp));
    /* Process */
    utilPtoCString(pncp->playerName, name);
    playersSetPlayerName(screenGetPlayers(), pncp->playerNum, name  );
  }
}

void serverTransportDoChecks();
void playerNeedUpdateDone();
/*********************************************************
*NAME:          serverNetMakePosPackets
*AUTHOR:        John Morrison
*CREATION DATE: 31/8/99
*LAST MODIFIED: 30/10/99
*PURPOSE:
* Makes and send out the positions of every person in the
* game.
*
*ARGUMENTS:
*
*********************************************************/
void serverNetMakePosPackets(void) {
  static int c = 0;
  static BYTE stale = 0; /* Should we send stale packets */
  char info[MAX_UDPPACKET_SIZE] = POSHEADER; /* Buffer to send */
  BYTE shellBuff[MAX_UDPPACKET_SIZE]; /* Buffer to send */
  BYTE playerBuff[MAX_UDPPACKET_SIZE]; /* Buffer to send */
  BYTE *ptr;
  int packetLen;
  BYTE shellDataLen;
  BYTE playerDataLen;
  BYTE count;
  bool needSend;
  bool sendStale;

  /* Do checks */
  serverTransportDoChecks();

  packetLen = BOLOPACKET_REQUEST_SIZE+1;
  c++;
  if (playersGetNumPlayers(screenGetPlayers()) > 0) {
    if (c < 3) {
      return;
    }
    c = 0;
    stale++;
    sendStale = FALSE;
    if (stale == 75) {
      stale = 0;
      sendStale = TRUE;
    }
    threadsWaitForMutex();
    serverCorePreparePosPackets();
    threadsReleaseMutex();
    count = 0;
    while (count < MAX_TANKS) {
      if (playersIsInUse(screenGetPlayers(), count) == TRUE) {
        /* Send them a packet */
        needSend = FALSE;
        ptr = info;
        ptr += BOLOPACKET_REQUEST_TYPEPOS+1;
        packetLen = BOLOPACKET_REQUEST_SIZE+1;

        threadsWaitForMutex();
        playerDataLen = serverCoreMakePosPackets(playerBuff, count, sendStale);
        threadsReleaseMutex();

        if (playerDataLen > 0) {
          needSend = TRUE;
          *ptr = BOLOPACKET_MAND_DATA;
          ptr++;
          packetLen++;
          *ptr = playerDataLen;
          ptr++;
          packetLen++;
          memcpy(ptr, playerBuff, playerDataLen);
          packetLen += playerDataLen;
          ptr += playerDataLen;
        }
        
        threadsWaitForMutex();      
        shellDataLen = (BYTE) serverCoreMakeShellData(shellBuff, count, FALSE);
        threadsReleaseMutex();

        /* Add shell data */
        if (shellDataLen > 0) {
          *ptr = BOLO_PACKET_SHELLDATA;
          ptr++;
          packetLen++;
          *ptr = shellDataLen;
          ptr++;
          packetLen++;
          memcpy(ptr, shellBuff, shellDataLen);
          ptr += shellDataLen;
          packetLen += shellDataLen;
          needSend = TRUE;
        } 

        /* Send packet */
        if (needSend == TRUE) {
          BYTE crcA, crcB;
          CRCCalcBytes(info, packetLen, &crcA, &crcB);
          info[packetLen] = crcA;
          info[packetLen + 1] = crcB;
          
          serverTransportSendUDP(info, packetLen+2, netPlayersGetAddr(&np, count));
        }
      }
      count++;
    }
    threadsWaitForMutex();      
    shellDataLen = (BYTE) serverCoreMakeShellData(shellBuff, 0xFF, TRUE);
    playerNeedUpdateDone(screenGetPlayers());
    threadsReleaseMutex();
  }
}

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
void severNetProcessClientData(BYTE *buff, int len, BYTE playerNum) {
  BYTE *ptr;        /* Pointer to the Buffer   */
  BYTE sectionLen;  /* Length of the section   */
  BYTE sectionType; /* Type of the section     */
  int pos;          /* Position through packet */
  
  ptr = buff;
  pos = BOLOPACKET_REQUEST_TYPEPOS+1;
  ptr += pos;
  while (pos < len) {
    sectionType = *ptr;
    pos++;
    ptr++;
    sectionLen = *ptr;
    pos++;
    ptr++;
    if (sectionLen > 0) {
      switch (sectionType) {
      case BOLO_PACKET_MAPDATA:
        severCoreExtractClientMapData(ptr, sectionLen, playerNum);
        break;
      case BOLO_PACKET_PNBDATA:
        serverCoreExtractPNBData(ptr, sectionLen);
        break;
      case BOLO_PACKET_TKDATA:
        serverCoreExtractTkExpData(ptr, sectionLen);
        break;
      case BOLO_PACKET_MNTDATA:
        serverCoreExtractNMTData(ptr, sectionLen);
        break;
      default:
      /* BOLO_PACKET_SHELLDATA */
        serverCoreExtractShellData(ptr, sectionLen);
        break;
      }
    }
    ptr += sectionLen;
    pos += sectionLen;
  }
}

/*********************************************************
*NAME:          serverNetMakeData
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
void serverNetMakeData() {
  char info[MAX_UDPPACKET_SIZE] = DATAHEADER; /* Buffer to send */
  BYTE *ptr;
  BYTE *posLen;
  int packetLen;
  bool shouldSend; /* Should we send that packet */
  
  packetLen = BOLOPACKET_REQUEST_SIZE+1;
  shouldSend = FALSE;
  if (playersGetNumPlayers(screenGetPlayers()) > 0) {
    ptr = info;
    ptr += BOLOPACKET_REQUEST_TYPEPOS+1;
    *ptr = BOLO_PACKET_MAPDATA;
    ptr++;
    packetLen++;
    posLen = ptr;
    ptr++;
    threadsWaitForMutex();
    *posLen = serverCoreMakeMapData(ptr);
    threadsReleaseMutex();
    ptr += *posLen;
    packetLen += *posLen;
    if (*posLen > 0) {
      shouldSend = TRUE;
    } else {
      ptr--;
      ptr--;
      packetLen--;
      packetLen--;
    }

    *ptr = BOLO_PACKET_TKDATA;
    ptr++;
    packetLen++;
    posLen = ptr;
    ptr++;
    packetLen++;
    threadsWaitForMutex();
    *posLen = (BYTE) serverCoreMakeTkData(ptr);
    threadsReleaseMutex();
    ptr += *posLen;
    packetLen += *posLen;
    if (*posLen > 0) {
      shouldSend = TRUE;
    } else {
      ptr--;
      ptr--;
      packetLen--;
      packetLen--;
    }


    *ptr = BOLO_PACKET_PNBDATA;
    ptr++;
    packetLen++;
    posLen = ptr;
    ptr++;
    packetLen++;
    threadsWaitForMutex();
    *posLen = (BYTE) netPNBMake(screenGetNetPnb(), ptr);
    threadsReleaseMutex();
    ptr += *posLen;
    packetLen += *posLen;
    if (*posLen > 0) {
      shouldSend = TRUE;
    }else {
      ptr--;
      ptr--;
      packetLen--;
      packetLen--;
    }

   *ptr = BOLO_PACKET_MNTDATA;
    ptr++;
    packetLen++;
    posLen = ptr;
    ptr++;
    packetLen++;
    threadsWaitForMutex();
    *posLen = (BYTE) netMNTMake(screenGetNetMnt(), ptr);
    threadsReleaseMutex();
    ptr += *posLen;
    packetLen += *posLen;
    if (*posLen > 0) {
      shouldSend = TRUE;
    } else {
      ptr--;
      ptr--;
      packetLen--;
      packetLen--;
    }

    if (shouldSend == TRUE) {
      serverNetSendAll(info, packetLen);
    }
  }
}

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
void serverNetSendTrackerUpdate() {
  INFO_PACKET h; /* Packet to send */
  struct in_addr dumb;
  unsigned short port;

  if (netUseTracker == TRUE) {
    serverNetMakeInfoRespsonse(&h);
    serverTransportGetUs(&dumb, &port);
    h.gameid.serverport = port;
    serverTransportSendUdpTracker((char *) &h, sizeof(h));
  }
}




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
void serverNetSetLock(bool lockState) {
  BYTE buff[MAX_UDPPACKET_SIZE] = SEVERMSGHEADER;
  BYTE *ptr;
  
  ptr = buff;
  ptr += BOLOPACKET_REQUEST_TYPEPOS +1;

  if (lockState == TRUE) {
    screenServerConsoleMessage((char *) "This game is now locked to new players (server lock)");
    if (serverLock != lockState) {
      /* Send message to clients */
      utilCtoPString((char *) "This game is now locked to new players (server lock)", ptr);
      logAddEvent(log_MessageServer, 0, 0, 0, 0, 0, (char *) ptr);
      serverNetSendAll(buff, BOLOPACKET_REQUEST_SIZE +(*ptr)+1);
      winboloNetSendLock(TRUE);
    }
    serverLock = TRUE;
  } else {
    /* Unlock */
    screenServerConsoleMessage((char *) "This game is now unlocked to new players (server unlock)");
    if (serverLock != lockState) {
      /* Send message to clients */
      utilCtoPString((char *) "This game is now unlocked to new players (server unlock)", ptr);
      logAddEvent(log_MessageServer, 0, 0, 0, 0, 0, (char *) ptr);
      serverNetSendAll(buff, BOLOPACKET_REQUEST_SIZE +(*ptr)+1);
      winboloNetSendLock(FALSE);
    }
    serverLock = FALSE;
  }
}

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
bool serverNetIsGameLocked() {
  bool returnValue; /* Value to return */

  returnValue = serverLock;
  if (returnValue == FALSE) {
    if (playersGetNumPlayers(screenGetPlayers()) > 0) {
      returnValue = netPlayersIsLocked(&np);
    }
  }

  return returnValue;
}

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
void serverNetSendQuitMessage() {
  BYTE buff[MAX_UDPPACKET_SIZE] = SEVERMSGHEADER;
  BYTE *ptr;
  
  ptr = buff;
  ptr += BOLOPACKET_REQUEST_TYPEPOS +1;
  /* Send message to clients */
  utilCtoPString((char *) "Server received quit message", ptr);
  logAddEvent(log_MessageServer, 0, 0, 0, 0, 0, (char *) ptr);
  serverNetSendAll(buff, BOLOPACKET_REQUEST_SIZE +(*ptr)+1);
  buff[BOLOPACKET_REQUEST_TYPEPOS] = BOLOPACKET_PACKETQUIT;
  serverNetSendAll(buff, BOLOPACKET_REQUEST_SIZE);
}

/*********************************************************
*NAME:          serverNetSendManReturnMessge
*AUTHOR:        John Morrison
*CREATION DATE: 2/12/00
*LAST MODIFIED: 2/12/00
*PURPOSE:
* Sends the man has return packet to playerNum
*
*ARGUMENTS:
*  playerNum - Player number to send to.
*  numTrees  - Amount of trees lgm is carrying
*  numMines  - Amount of mines the lgm is carrying
*  pillNum   - Pillbox being carries
*********************************************************/
void serverNetSendManReturnMessge(BYTE playerNum, BYTE numTrees, BYTE numMines, BYTE pillNum) {
  BYTE sendBuff[sizeof(LGMRETURN_PACKET) + 3];
  LGMRETURN_PACKET buff = LGMRETURNHEADER;
  buff.numMines = numMines;
  buff.numPills = pillNum;
  buff.numTrees = numTrees;

  memcpy(sendBuff, &buff, sizeof(LGMRETURN_PACKET));
  serverNetSendPlayer(playerNum, sendBuff, sizeof(buff));
}

void serverNetSendManWorkingMessage(BYTE playerNum, BYTE mapX, BYTE mapY, BYTE numMines, BYTE numTrees, BYTE pillNum) {
  BYTE sendBuff[sizeof(LGMMOVE_PACKET) + 3];
  LGMMOVE_PACKET buff = LGMWORKINGHEADER;
  buff.mapX = mapX;
  buff.mapY = mapY;
  buff.pillNum = pillNum;
  buff.mines = numMines;
  buff.trees = numTrees;

  memcpy(sendBuff, &buff, sizeof(LGMMOVE_PACKET));
  serverNetSendPlayer(playerNum, sendBuff, sizeof(buff));
}

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
void serverNetCheckRemovePlayers() {
  static time_t lastCheck = 0; /* Time between removing players */
  BYTE count;                  /* Looping Variable */

  if (time(NULL) - lastCheck >= SERVER_NET_REMOVE_CHECKTIME) {
    count = 0;
    while (count < MAX_TANKS) {
      if (netPlayersShouldDisconnectPlayer(&np, count) == TRUE) {
        /* Disconnect them */
        serverNetPlayerLeave(count, FALSE);
      }
      count++;
    }
    lastCheck = time(NULL);
  }
}

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
void serverNetSendServerMessageAllPlayers(char *msg) {
  static char info[MAX_UDPPACKET_SIZE] = GENERICHEADER; /* Buffer to send */
  BYTE *ptr; /* Data Ptr */
  int len; /* Line length */
  info[BOLOPACKET_REQUEST_TYPEPOS] = BOLOSERVERMESSAGE; 
  ptr = info;
  ptr += BOLOPACKET_REQUEST_TYPEPOS+1;


  /* Remove a newline if present at the end of the string */
  len = (int) strlen(msg);
  if (msg[len-1] == '\n') {
    msg[len-1] = EMPTY_CHAR;
  }
  utilCtoPString(msg, ptr);
  logAddEvent(log_MessageServer, 0, 0, 0, 0, 0, (char *) ptr);
  serverNetSendAll(info, BOLOPACKET_REQUEST_SIZE +(*ptr)+1);
}

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
BYTE serverNetGetMaxPlayers() {
  return snMaxPlayers;
}

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
bool serverNetGetIsLocked() {
	bool returnValue; /* Value to return */
  BYTE numPlayers; /* Number of players in this game */

  returnValue = FALSE;
  numPlayers = playersGetNumPlayers(screenGetPlayers());
	if (serverLock == TRUE || (numPlayers > 0 && netPlayersIsLocked(&np) == TRUE)) {
    returnValue = TRUE;
  }

  return returnValue;
}

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
void serverNetGetUs(BYTE *buff, unsigned short *port) {
  struct in_addr dest;
  char *ip;
  char *current;

  

  serverTransportGetUs(&dest, port);
  ip = inet_ntoa(dest);
  current = strtok(ip, ".");
  buff[0] = (BYTE) atoi(current);
  current = strtok(NULL, ".");
  buff[1] = (BYTE) atoi(current);
  current = strtok(NULL, ".");
  buff[2] = (BYTE) atoi(current);
  current = strtok(NULL, ".");
  buff[3] = (BYTE) atoi(current);
}
