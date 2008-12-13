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
*Name:          DNS
*Filename:      dns.c
*Author:        John Morrison
*Creation Date: 07/05/01
*Last Modified: 07/05/01
*Purpose:
*  Looks after the DMS resolutions
*********************************************************/

#include <winsock2.h>
#include "global.h"
#include "dns.h"

/* Module level variable on DNS setting */
bool dnsEnabled = FALSE;

/*********************************************************
*NAME:          dnsCreate
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Sets up the DNS lookup module
*
*ARGUMENTS:
*
*********************************************************/
bool dnsCreate() {
  WSADATA wsaData;         /* Winsock create data */
  int ret;                 /* Function returns */

  ret = WSAStartup(MAKEWORD(2,0), &wsaData);
  if (ret != 0) {
    MessageBox(NULL, "Winsock startup failed", "LogViewer", MB_ICONEXCLAMATION);
    return FALSE;
  }
  dnsEnabled = TRUE;
  return TRUE;
}

/*********************************************************
*NAME:          dnsSetEnabled
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Enables/Disables DNS lookups. Returns success
*
*ARGUMENTS:
* set - Value to set
*********************************************************/
bool dnsSetEnabled(bool set) {
  bool returnValue = TRUE; // Value to set

  if (set != dnsEnabled) {
    if (set == FALSE) {
      dnsShutdown();
    } else {
      returnValue = dnsCreate();
    }

  }

  return returnValue;
}

/*********************************************************
*NAME:          dnsShutdown
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Shuts down DNS module
*
*ARGUMENTS:
* set - Value to set
*********************************************************/
void dnsShutdown() {
  if (dnsEnabled == TRUE) {
    WSACleanup();
    dnsEnabled = FALSE;
  }
}

/*********************************************************
*NAME:          dnsLookup
*AUTHOR:        John Morrison
*CREATION DATE: 07/05/01
*LAST MODIFIED: 07/05/01
*PURPOSE:
*  Looks up a IP address. Stores result in host. If lookup
*  failed or not enabled copies IP to host
*
*ARGUMENTS:
* ip - IP to lookup
* host - Host to hold result
*********************************************************/
void dnsLookup(char *ip, char *host) {
  HOSTENT  *hd;           /* Host Data */
  struct sockaddr_in  addr;
  
  if (dnsEnabled == FALSE) {
    strcpy(host, ip);
  } else {
    addr.sin_addr.s_addr = inet_addr(ip);
    hd = gethostbyaddr((const void*)&addr.sin_addr, sizeof(struct in_addr), AF_INET);
     if (hd == NULL) {
      strcpy(host, ip);
    } else {
      strcpy(host, hd->h_name);
    }
  }
}
