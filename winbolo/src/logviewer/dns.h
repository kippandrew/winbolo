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
*Filename:      dns.h
*Author:        John Morrison
*Creation Date: 07/05/01
*Last Modified: 07/05/01
*Purpose:
*  Looks after the DMS resolutions
*********************************************************/

#ifndef __DNS_H
#define __DNS_H

#include "global.h"

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
bool dnsCreate();

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
bool dnsSetEnabled(bool set);

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
void dnsShutdown();

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
void dnsLookup(char *ip, char *host);

#endif /* __DNS_H */
