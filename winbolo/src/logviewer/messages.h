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
*Name:          Messages
*Filename:      messages.h
*Author:        John Morrison
*Creation Date: 3/1/99
*Last Modified: 1/6/00
*Purpose:
*  Responsable for scrolling messages.
*********************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include "global.h"

#define IsEmpty(list) ((list) ==NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define MessageTail(list) ((list)->next);
#define MessageHeadTop(list) ((list)->topLine);
#define MessageHeadBottom(list) ((list)->bottomLine);

/* Message String Macros */
#define MESSAGE_QUOTES "\""

/* Width of the screen */
#define MESSAGE_WIDTH 68
/* End of string marker */
#define END_OF_STRING '\0'
/* Blank space marker */
#define MESSAGE_BLANK ' '

/* An Empty Message */
#define MESSAGE_EMPTY " \0"

/* Time between screen updates */
#define MESSAGE_SCROLL_TIME 4 /* Was 5 prior to 1.09 */

/* Type structure */

typedef struct messageObj *message;
struct messageObj {
  message next;
  char topLine;
  char bottomLine;
};

/* Offset to a player message */
#define PLAYER_MESSAGE_OFFSET 5

typedef enum {
  newsWireMessage, /* Differnt Message types */
  assistantMessage,
  AIMessage,
  networkMessage,
  networkStatus,
  player0Message,  /* Player Number messages */
  player1Message,
  player2Message,
  player3Message,
  player4Message,
  player5Message,
  player6Message,
  player7Message,
  player8Message,
  player9Message,
  player10Message,
  player11Message,
  player12Message,
  player13Message,
  player14Message,
  player15Message,
  globalMessage     /* Message must be printed */
} messageType;

/* Prototypes */

/*********************************************************
*NAME:          messageCreate
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  3/1/99
*PURPOSE:
*  Sets up the messages data structure
*
*ARGUMENTS:
*
*********************************************************/
void messageCreate(void);

/*********************************************************
*NAME:          messageDestroy
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  3/1/99
*PURPOSE:
*  Destroys and frees memory for the message data 
*  structure
*
*ARGUMENTS:
*
*********************************************************/
void messageDestroy(void);

/*********************************************************
*NAME:          clientMessageAdd
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  4/7/00
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
void clientMessageAdd(messageType msgType, char *top, char *bottom);

/*********************************************************
*NAME:          serverMessageAdd
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  4/7/00
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
void serverMessageAdd(messageType msgType, char *top, char *bottom);

/*********************************************************
*NAME:          messageAdd
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
void messageAdd(messageType msgType, char *top, char *bottom);

/*********************************************************
*NAME:          messageAddItem
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  3/1/99
*PURPOSE:
*  Adds an item to the message data structure. 
*
*ARGUMENTS:
*  top    - The message to print in the top line
*  bottom - The message to print in the bottom line
*********************************************************/
void messageAddItem(char *top, char *bottom);

/*********************************************************
*NAME:          messageUpdate
*AUTHOR:        John Morrison
*CREATION DATE:  3/1/99
*LAST MODIFIED:  3/1/99
*PURPOSE:
*  Updates the scrolling message
*
*ARGUMENTS:
*
*********************************************************/
void messageUpdate(void);

/*********************************************************
*NAME:          messageGetMessage
*AUTHOR:        John Morrison
*CREATION DATE: 1/1/98
*LAST MODIFIED: 1/1/98
*PURPOSE:
*  Copys the messages on screen into the given variables
*
*ARGUMENTS:
*  top    - The message to print in the top line
*  bottom - The message to print in the bottom line
*********************************************************/
void messageGetMessage(char *top, char *bottom);

/*********************************************************
*NAME:          messageSetNewswire
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/98
*LAST MODIFIED: 8/1/98
*PURPOSE:
*  Sets the state of newswire messages
*
*ARGUMENTS:
*  isShown - Is this type of message shown
*********************************************************/
void messageSetNewswire(bool isShown);

/*********************************************************
*NAME:          messageSetAssistant
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/98
*LAST MODIFIED: 8/1/98
*PURPOSE:
*  Sets the state of assistant messages
*
*ARGUMENTS:
*  isShown - Is this type of message shown
*********************************************************/
void messageSetAssistant(bool isShown);

/*********************************************************
*NAME:          messageSetAI
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/98
*LAST MODIFIED: 8/1/98
*PURPOSE:
*  Sets the state of AI messages
*
*ARGUMENTS:
*  isShown - Is this type of message shown
*********************************************************/
void messageSetAI(bool isShown);

/*********************************************************
*NAME:          messageSetNetwork
*AUTHOR:        John Morrison
*CREATION DATE: 8/1/98
*LAST MODIFIED: 8/1/98
*PURPOSE:
*  Sets the state of network messages
*
*ARGUMENTS:
*  isShown - Is this type of message shown
*********************************************************/
void messageSetNetwork(bool isShown);

/*********************************************************
*NAME:          messageSetNetStatus
*AUTHOR:        John Morrison
*CREATION DATE: 1/6/00
*LAST MODIFIED: 1/6/00
*PURPOSE:
*  Sets the state of network status messages
*
*ARGUMENTS:
*  isShown - Is this type of message shown
*********************************************************/
void messageSetNetStatus(bool isShown);

/*********************************************************
*NAME:          messageIsNewMessage
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Returns whether a new message has arrived or not.
*
*ARGUMENTS:
*
*********************************************************/
bool messageIsNewMessage();

/*********************************************************
*NAME:          messageGetNewMessage
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Gets the new message. Returns where the message 
*  originated from. NOTE: PlayerBitmap presently unused
*
*ARGUMENTS:
*  dest         - Destination for the message
*  playerBitmap - Bitmap of players that recieved it
*********************************************************/
BYTE messageGetNewMessage(char *dest, unsigned long **playerBitmap);

#endif /* MESSAGE_H */
