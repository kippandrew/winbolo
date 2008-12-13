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
*Name:          ServerMessages
*Filename:      ServerMessages.c
*Author:        John Morrison
*Creation Date:  10/8/99
*Last Modified:  10/8/99
*Purpose:
*  Responsable for messages.
*********************************************************/

#include <stdio.h>
#include "../bolo/messages.h"


bool isServerQuiet = FALSE;              /* Is the server running in quiet mode */
char serverMessageLogFile[FILENAME_MAX]; /* FileName to save to */
bool serverMessageUseLogFile = FALSE; /* Are we using a log file */

void serverMessageSetQuietMode(bool modeOn) {
  isServerQuiet = modeOn;
}

void serverMessagesSetLogFile(char *logFile) {
  strcpy(serverMessageLogFile, logFile);
  serverMessageUseLogFile = TRUE;
}

/*********************************************************
*NAME:          serverMessageAdd
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  3/1/99
*PURPOSE:
*  Functions call this to display a message. They must
*  pass the message type so that it can be determined
*  whether the header should be printed etc.
*
*ARGUMENTS:
*  msgType - The type of the message
*  top     - The message to print in the top line
*  bottom  - The message to print in the bottom line
*********************************************************/
void serverMessageAdd(messageType msgType, char *top, char *bottom) {
  FILE *fp; /* File to write to */
  if (msgType != assistantMessage && isServerQuiet == FALSE) {
    if (serverMessageUseLogFile == FALSE) {
      fprintf(stdout, "%s\n%s\n", top, bottom);
     } else {
      fp = fopen(serverMessageLogFile, "a");
      if (fp) {
        fprintf(fp, "%s\n%s\n", top, bottom);
        fclose(fp);
      }
    }
  }
}

void serverMessageConsoleMessage(char *msg) {
  FILE *fp; /* File to write to */
  if (isServerQuiet == FALSE) {
    if (serverMessageUseLogFile == FALSE) {
      fprintf(stderr, "%s\n", msg);
    } else {
      fp = fopen(serverMessageLogFile, "a");
      if (fp) {
        fprintf(fp, "%s\n", msg);
        fclose(fp);
      }
    }
  }
}

