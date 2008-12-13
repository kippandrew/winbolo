/*
 * $Id$
 *
 * Copyright (c) 2000-2008 John Morrison.
 * Copyright (c) 2000-2008 Andrew Roth.
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
*Name:          Util
*Filename:      util.c
*Author:        John Morrison
*Creation Date: 00/10/01
*Last Modified: 00/10/01
*Purpose:
*  Provides misc functions
*********************************************************/

/* Inludes */

#ifdef _WIN32
/* Windows winsock */
#include <winsock2.h>
#include <windows.h>
#include <memory.h>

#else
/* Liunux Winsock plus some useful macros */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(X) close(X)
typedef struct hostent  HOSTENT;
#define SD_BOTH 2

#endif


#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "util.h"

/* Used to stop socket blocking */
#define NO_BLOCK_SOCK 1

/*********************************************************
*NAME:          utilMakeBoundSocket
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Makes a bound socket and returns it. Returns 
* SOCKET_ERROR on error
*
*ARGUMENTS:
*  tcp       - TRUE if a TCP socket, else false
*  listening - TRUE if the socket is to listen for 
*              connections
*  blocking  - TRUE if this should be a blocking socket
*  port      - Port to bind to
*********************************************************/
SOCKET utilMakeBoundSocket(bool tcp, bool listening, bool blocking, unsigned short port) {
  int sock;                /* Socket to return */
  struct sockaddr_in addr; /* Socket structure */
  
  
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  /* Create Sockets */
  if (tcp == TRUE) {    
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  } else {
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  }
  
  /* Binding */
  if (sock > 0) {
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      shutdown(sock, SD_BOTH);
      closesocket(sock);
      sock = SOCKET_ERROR;
    }
  }

  /* Listening */
  if (listening == TRUE && tcp == TRUE) {
    if (listen(sock, MAX_CONNECTIONS) != 0) {
      shutdown(sock, SD_BOTH);
      closesocket(sock);
      sock = SOCKET_ERROR;
    }
  }


  /* Blocking */
  if (blocking == FALSE) {
#ifdef _WIN32
    long noBlock = NO_BLOCK_SOCK;
    ioctlsocket(sock, FIONBIO, &noBlock);
#else 
    fcntl(sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL));
#endif
  }
  return sock;
}


/*********************************************************
*NAME:          utilReverseLookup
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Performs a reverse lookup on an IP
*
*ARGUMENTS:
*  pack - Source ip packet
*  dest - Destination space
*********************************************************/
bool utilReverseLookup(struct in_addr *pack, char *dest) {
  HOSTENT *hent;
  
  dest[0] = 0;
  if (strcmp(inet_ntoa(*pack), "207.88.53.107") == 0) {
		strcpy(dest, "paniq.winbolo.us");
  return 1;
  }

  fprintf(stderr, "\t\tREVDNS: lookup on %s:\n", inet_ntoa(*pack));
  hent = gethostbyaddr( (char *) &((*pack).s_addr) , sizeof((*pack).s_addr), AF_INET);
  fprintf(stderr, "\t\tREVDNS: DONE\n");

  if (hent != NULL) {
	  strcpy(dest, hent->h_name);
	  if (strcmp("fuckbigpond.com", dest) == 0) {
		  strcpy(dest, "west.winbolo.com");
	  } else if (strcmp("ool-4351bf05.dyn.optonline.net", dest) == 0) {
		  strcpy(dest, "sheeps.winbolo.us");
	  } else if (strcmp("CPE0002446032e4-CM000039fbb861.cpe.net.cable.rogers.com", dest) == 0) {
		  strcpy(dest, "winbolo.dhs.org");
	  } else if (strcmp("pool-141-154-115-77.bos.east.verizon.net", dest) == 0) {
		  strcpy(dest, "bluebot.winbolo.us");
	 } else if (strcmp("dsl092-218-111.sfo2.dsl.speakeasy.net", dest) == 0) {
		 strcpy(dest, "renbot.winbolo.us");
	 } else if (strcmp("12.39.4.70", dest) == 0) {
		 strcpy(dest, "pawnyBot.winbolo.us");
	}

	  fprintf(stderr, "\t\tREVDNS: resolved to %s\n", dest);

	  return 1;
  } else {
	  strcpy(dest, inet_ntoa(*pack));
	    fprintf(stderr, "\t\tREVDNS: resolved to %s\n", dest);

	  return 0;
  }
}

/*********************************************************
*NAME:          utilPtoCString
*AUTHOR:        John Morrison
*CREATION DATE: 00/10/01
*LAST MODIFIED: 00/10/01
*PURPOSE:
* Convert Bolo's network pascal string to C strings
*
*ARGUMENTS:
*  src  - Source string
*  dest - Destination string 
*********************************************************/
void utilPtoCString(char *src, char *dest) {
  int count; /* Looping variable */
  int len;   /* Length of the string */
  len = min(src[0],35);
  for (count=0; count<len; count++) {
    dest[count] = src[count+1];
	}
  dest[count] = '\0';
}

