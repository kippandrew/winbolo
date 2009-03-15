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
*Name:          http
*Filename:      http.h
*Author:        John Morrison
*Creation Date: 16/9/01
*Last Modified: 23/9/01
*Purpose:
*  Responsable for sending/receiving http messages
*********************************************************/

#ifndef __HTTP_H
#define __HTTP_H

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
  #include <winsock2.h>
  #include <windows.h>
  #include <memory.h>
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #define INVALID_SOCKET -1
  #define SD_BOTH 2
  #define SOCKET_ERROR -1
  typedef int SOCKET;
#endif
#include "../bolo/global.h"


#define HTTP_SERVER_PORT 80 /* HTTP runs on port 80 */
#define HTTP_SERVER_ADDRESS "wbn.winbolo.net"
#define HTTP_SEND_HEADER "GET /wbn.php?data="
#define HTTP_POST_HEADER "POST /log.php?key="
#define HTTP_HEXTOINT(val) ((val >= '0' && val <='9') ? val : (val-30))

/*********************************************************
*NAME:          httpCreate
*AUTHOR:        John Morrison
*CREATION DATE: 16/9/01
*LAST MODIFIED: 16/9/01
*PURPOSE:
* Initialises the http module. Initialises winsock etc.
* Returns success.
*
*ARGUMENTS:
* 
*********************************************************/
bool httpCreate();

/*********************************************************
*NAME:          httpDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 16/9/01
*LAST MODIFIED: 16/9/01
*PURPOSE:
* Destroys the http module. Cleans up any open libraries
*
*ARGUMENTS:
* 
*********************************************************/
void httpDestroy();

/*********************************************************
*NAME:          httpDecodeData
*AUTHOR:        John Morrison
*CREATION DATE: 16/9/01
*LAST MODIFIED: 16/9/01
*PURPOSE:
* Decodes the byte buffer and returns its decoded length
* (-1 for error)
*
*ARGUMENTS:
* buff - The source byte buffer
* len  - The source byte buffer length
*********************************************************/
int httpDecodeData(BYTE *buff, int len);

/*********************************************************
*NAME:          httpEncodeData
*AUTHOR:        John Morrison
*CREATION DATE: 16/9/01
*LAST MODIFIED: 16/9/01
*PURPOSE:
* Encodes a byte buffer and returns its encoded length
* (-1 for error)
*
*ARGUMENTS:
* src    - The source byte buffer
* len    - The source byte buffer length
* dest   - The destination byte buffer
* maxLen - Maximum length of the byte buffer
*********************************************************/
int httpEncodeData(BYTE *src, int len, BYTE *dest, int maxLen);

/*********************************************************
*NAME:          httpSendData
*AUTHOR:        John Morrison
*CREATION DATE: 19/9/01
*LAST MODIFIED: 19/9/01
*PURPOSE:
* Sends the data to the server, including http headers
* returns the length of the resulting message or -1 if
* an error occurs
*
*ARGUMENTS:
* sock    - Socket used to send.
* message - The message to send to the WinBolo.net server
* len     - Length of the data
*********************************************************/
int httpSendData(SOCKET sock, BYTE *message, int len);

/*********************************************************
*NAME:          httpRecvData
*AUTHOR:        John Morrison
*CREATION DATE: 23/9/01
*LAST MODIFIED: 23/9/01
*PURPOSE:
* Revieves the data from the server, Checks headers
* returns the length of the resulting data or -1 if
* an error occurs
*
*ARGUMENTS:
* sock    - Data Socket
* buff    - Data buffer to write to
* maxSize - The maximum size of the response
*********************************************************/
int httpRecvData(SOCKET sock, BYTE *buff, int maxSize);

/*********************************************************
*NAME:          httpSendMessage
*AUTHOR:        John Morrison
*CREATION DATE: 16/9/01
*LAST MODIFIED: 19/9/01
*PURPOSE:
* Sends a message to the WinBolo.net server via http and
* returns the length of the resulting message or -1 if
* an error occurs
*
*ARGUMENTS:
* message - The message to send to the WinBolo.net server
* len     - Length of the data
* respose - A buffer to hold the response
* maxSize - The maximum size of the response
*********************************************************/
int httpSendMessage(BYTE *message, int len, BYTE *response, int maxSize);

void setAltIpAddress(char *iptoset);

#endif /* __HTTP_H */
