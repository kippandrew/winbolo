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
*Filename:      util.h
*Author:        John Morrison
*Creation Date: 00/10/01
*Last Modified: 00/10/01
*Purpose:
*  Provides misc functions
*********************************************************/

#ifndef _UTIL_H
#define _UTIL_H

#ifdef _WIN32
#define	sleep(X)	sleep(X)
#else
#define	sleep(X)	usleep(X)
#endif

/* misc defines */
#define max(A,B)        (A > B) ? (A) : (B)
#define min(A,B)	(A < B) ? (A) : (B)

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
SOCKET utilMakeBoundSocket(bool tcp, bool listening, bool blocking, unsigned short port);

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
bool utilReverseLookup(struct in_addr *pack, char *dest);

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
void utilPtoCString(char *src, char *dest);

#endif /* _UTIL_H */
