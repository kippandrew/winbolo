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
*Name:          WinBolo.net
*Filename:      winbolonet.c
*Author:        John Morrison
*Creation Date: 23/09/01
*Last Modified: 10/01/04
*Purpose:
*  Responsable for interacting with WinBolo.net
*********************************************************/

#include <time.h>
#include "winbolonet.h"
#include "http.h"
#include "../server/servercore.h"
#include "../bolo/netpacks.h"
#include "../bolo/util.h"
#include "winbolonetevents.h"
#include "winbolonetthread.h"

bool winboloNetRunning = FALSE;

#define WINBOLONET_BUFFSIZE (1024*8)
BYTE *winboloNetBuff = NULL;
BYTE winboloNetServerKey[WINBOLONET_KEY_LEN];
/* Keys for each player if in use - Always position 0 if we are a client */
BYTE winboloNetPlayerKey[MAX_TANKS][WINBOLONET_KEY_LEN];
time_t winboloNetLastSent;

//FIXME: All the string error catching stuff

/*********************************************************
*NAME:          winbolonetCreateServer
*AUTHOR:        John Morrison
*CREATION DATE: 23/09/01
*LAST MODIFIED: 02/04/02
*PURPOSE:
* Initialises the WinBolo.net module. Returns success.
* Tries to contact server to verify versions.
*
*ARGUMENTS:
* mapName - Name of the map
* port - Port we are running on
* gameType - Game Type
* ai - Is AI allowed
* mines - Mines allowed
* password - Has password
* numBases - Number of bases
* numPills - Number of pills
* freeBases - Free bases
* freePills - Free pills
* numPlayers -  Number of players in the game
* startTime - Game start time
*********************************************************/
bool winbolonetCreateServer(char *mapName, unsigned short port, BYTE gameType, BYTE ai, bool mines, bool password, BYTE numBases, BYTE numPills, BYTE freeBases, BYTE freePills, BYTE numPlayers, long startTime) {
  char buff[FILENAME_MAX]; /* Send buffer */
  int ans;                 /* Function return */
  BYTE count;              /* Looping Variable */

  screenServerConsoleMessage((char *) "WinBolo.net Startup");
  winboloNetRunning = FALSE;
  winbolonetEventsCreate();
  winboloNetServerKey[0] = EMPTY_CHAR;
  count = 0;
  while (count < MAX_TANKS) {
    winboloNetPlayerKey[count][0] = EMPTY_CHAR;
    count++;
  }


  winboloNetRunning = httpCreate();
  if (winboloNetRunning == TRUE) {
    buff[0] = WINBOLO_NET_VERSION_MAJOR;
    buff[1] = WINBOLO_NET_VERSION_MINOR;
    buff[2] = WINBOLO_NET_VERSION_REVISION;
    buff[3] = WINBOLO_NET_MESSAGE_VERSION_REQ;
    winboloNetBuff = malloc(WINBOLONET_BUFFSIZE);
    memset(winboloNetBuff, 0, WINBOLONET_BUFFSIZE);
    ans = httpSendMessage(buff , 4, winboloNetBuff, WINBOLONET_BUFFSIZE);
    
    if (ans > 0) {
      if (winboloNetBuff[0] == 0) {
        /* Error */
        winboloNetBuff[ans] = '\0';
        fprintf(stderr, "Error: %s\n", winboloNetBuff+1);
        winbolonetDestroy();
      } else if (winboloNetBuff[0] == 1) {
        sprintf(buff, "\tWinBolo.net Available: Version: %d.%d.%d", winboloNetBuff[1], winboloNetBuff[2], winboloNetBuff[3]);
        screenServerConsoleMessage((char *) buff);
        if (ans > 4) {
          winboloNetBuff[ans] = '\0';
          sprintf(buff, "\tMOTD: %s", winboloNetBuff+4);
          screenServerConsoleMessage((char *) buff);
          /* Get a server key */
          if (winbolonetRequestServerKey(mapName, port, gameType, ai, mines, password, numBases, numPills, freeBases, freePills, numPlayers, startTime) == FALSE) {
            /* Error can't get a key */
            winbolonetDestroy();
          } else {
            winbolonetThreadCreate();
            winboloNetLastSent = time(NULL);
            winboloNetSendVersion();
          }

        }
      } else {
        /* Unexpected error */
        winboloNetBuff[ans] = '\0';
        screenServerConsoleMessage("Error: WinBolo.net unavailable\n");
        winbolonetDestroy();

      }

    } else {
      screenServerConsoleMessage("Error: No response from WinBolo.net - Winbolo.net disabled\n");
      winbolonetDestroy();
    }
  }
  return winboloNetRunning;
}

/*********************************************************
*NAME:          winbolonetCreateClient
*AUTHOR:        John Morrison
*CREATION DATE: 31/03/02
*LAST MODIFIED: 31/03/02
*PURPOSE:
* Initialises the WinBolo.net module for a client.
* Returns success.
*
*ARGUMENTS:
* userName  - WinBolo.net account name
* password  - Password for the account
* serverKey - Session key for the server
* errorMsg  - Buffer to hold Error message if required
*********************************************************/
bool winbolonetCreateClient(char *userName, char *password, BYTE *serverKey, char *errorMsg) {
  bool ownServer;          /* Is this client running the server too */
  char buff[FILENAME_MAX]; /* Send buffer */
  int ret;                 /* Function return */

  ownServer = FALSE;
  
  if (winboloNetRunning == FALSE) {
    winboloNetRunning = httpCreate();
    winboloNetServerKey[0] = '\0';
  } else {
    ownServer = TRUE;
  }
  if (winboloNetRunning == TRUE) {
    buff[0] = WINBOLO_NET_VERSION_MAJOR;
    buff[1] = WINBOLO_NET_VERSION_MINOR;
    buff[2] = WINBOLO_NET_VERSION_REVISION;
    buff[3] = WINBOLO_NET_MESSAGE_VERSION_REQ;
    winboloNetBuff = malloc(WINBOLONET_BUFFSIZE);
    memset(winboloNetBuff, 0, WINBOLONET_BUFFSIZE);
    ret = httpSendMessage(buff , 4, winboloNetBuff, WINBOLONET_BUFFSIZE);
    
    if (ret > 0) {
      if (winboloNetBuff[0] == 0) {
        /* Error */
        winboloNetBuff[ret] = '\0';
        strcpy(errorMsg, winboloNetBuff+1);
        winbolonetDestroy();
      } else {
        /* Get a Client key */
        if (winbolonetRequestClientKey(userName, password, serverKey, errorMsg) == FALSE) {
          /* Error can't get a key */
          winbolonetDestroy();
        }
      }
    } else {
      strcpy(errorMsg, "Error: No response from WinBolo.net - Winbolo.net disabled");
      winbolonetDestroy();
    }
  }
  return winboloNetRunning;
}

/*********************************************************
*NAME:          winbolonetDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 23/9/01
*LAST MODIFIED: 23/9/01
*PURPOSE:
* Destroys the winbolonet module.
* Cleans up any open libraries
*
*ARGUMENTS:
* 
*********************************************************/
void winbolonetDestroy() {
  screenServerConsoleMessage((char *) "WinBolo.net Shutdown");
  if (winboloNetRunning == TRUE) {
    winbolonetThreadDestroy();
    /* We need to say goodbye */
    if (threadsGetContext() == TRUE && winboloNetServerKey[0] != EMPTY_CHAR) {
      /* Note only servers have to say goodbye */
      winbolonetGoodbye();
    }
    httpDestroy();
    free(winboloNetBuff);
    winboloNetBuff = NULL;
  }
  winbolonetEventsDestroy();

  winboloNetRunning = FALSE;
}

/*********************************************************
*NAME:          winbolonetGoodbye
*AUTHOR:        John Morrison
*CREATION DATE: 02/04/02
*LAST MODIFIED: 02/04/02
*PURPOSE:
* Destroys the winbolonet module.
* Cleans up any open libraries
*
*ARGUMENTS:
* 
*********************************************************/
void winbolonetGoodbye() {
  BYTE buff[FILENAME_MAX]; /* Sending buffer */
  int ret;  

  /* Send off final data */
  winbolonetServerUpdate(0,0,0, TRUE);

  /* Shutdown */
  buff[0] = WINBOLO_NET_VERSION_MAJOR;
  buff[1] = WINBOLO_NET_VERSION_MINOR;
  buff[2] = WINBOLO_NET_VERSION_REVISION;
  buff[3] = WINBOLO_NET_MESSAGE_SERVERQUIT_REQ;
  memcpy(buff+4, winboloNetServerKey, WINBOLONET_KEY_LEN);

  ret = httpSendMessage(buff, 4+WINBOLONET_KEY_LEN, winboloNetBuff, WINBOLONET_BUFFSIZE);
  if (ret > 0) {
    if (winboloNetBuff[0] == 0) {
      /* Error */
      winboloNetBuff[ret] = EMPTY_CHAR;
//      fprintf(stderr, "Error: %s\n", winboloNetBuff+1);
    }
  } else {
    fprintf(stderr, "Error: No response from WinBolo.net\n");
  } 
}

/*********************************************************
*NAME:          winbolonetServerSendTeams
*AUTHOR:        John Morrison
*CREATION DATE: 21/02/03
*LAST MODIFIED: 21/02/03
*PURPOSE:
* Sends the list of teams at the end of the game.
*
*ARGUMENTS:
* array    - BYTE array containing team memberships
* length   - Length of the array
* numTeams - Number of teams in the array
*********************************************************/
void winbolonetServerSendTeams(BYTE *array, BYTE length, BYTE numTeams) {
  BYTE *ptr;     /* Our memory pointer */
  BYTE arrayPos; /* Position in the array */
  int pos;       /* Pointer inside the buffer */
  int ret;       /* Function return value */

  ptr = malloc(4 /* Header */ +  (length * (WINBOLONET_KEY_LEN+1)) + WINBOLONET_KEY_LEN);
  ptr[0] = WINBOLO_NET_VERSION_MAJOR;
  ptr[1] = WINBOLO_NET_VERSION_MINOR;
  ptr[2] = WINBOLO_NET_VERSION_REVISION;
  ptr[3] = WINBOLO_NET_MESSAGE_TEAMS;
  /* Is next */
  pos = 4;
  memcpy(ptr+4, winboloNetServerKey, WINBOLONET_KEY_LEN);
  pos += WINBOLONET_KEY_LEN;

  ptr[pos] = numTeams;
  pos++;
  arrayPos = 1;
  while (arrayPos <= length) {
    if (array[arrayPos] == WINBOLO_NET_TEAM_MARKER) {
      /* New Team */
      ptr[pos] = WINBOLO_NET_TEAM_MARKER;
      pos++;
    } else {
      /* Team member */
      if (winboloNetPlayerKey[array[arrayPos]][0] != EMPTY_CHAR) {
        memcpy((ptr+pos), winboloNetPlayerKey[array[arrayPos]], WINBOLONET_KEY_LEN);
	pos += WINBOLONET_KEY_LEN;
      } else {
        ptr[pos] = 0;
        pos++;
      } 
    }
    arrayPos++;
  } 
  
  /* Send it */
  ret = httpSendMessage(ptr, pos, winboloNetBuff, WINBOLONET_BUFFSIZE);
  free(ptr);
}

/*********************************************************
*NAME:          winbolonetServerUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 04/04/02
*LAST MODIFIED: 16/02/03
*PURPOSE:
* Sends a server winbolo.net update
*
*ARGUMENTS:
* numPlayer    - Number of pleyers in the game
* numFreePills - Number of free bases in the game
* numFreePills - Number of free pills in the game
* sendNow      - If TRUE the data should not be queued
*********************************************************/
void winbolonetServerUpdate(BYTE numPlayers, BYTE numFreeBases, BYTE numFreePills, bool sendNow) {
  static BYTE staticNumPlayers = 0;
  static BYTE staticNumFreeBases = 0;
  static BYTE staticNumFreePills = 0;
  BYTE *ptr; /* Our memory pointer */
  int size;  /* Number of items to send */
  int pos;   /* Pointer inside the buffer */
  int ret;   /* Function return */
  BYTE keyA[WINBOLONET_KEY_LEN];
  BYTE keyB[WINBOLONET_KEY_LEN];
  BYTE val;

  
  if (winboloNetRunning == TRUE ) {
    size = winbolonetEventsGetSize();
    if (size > 0 || staticNumFreePills != numFreePills || numFreeBases != staticNumFreeBases || numPlayers != staticNumPlayers || time(NULL) - winboloNetLastSent > WINBOLO_NET_MAX_NOSEND) {
      pos = 0;
      staticNumFreePills = numFreePills;
      staticNumFreeBases = numFreeBases;
      staticNumPlayers = numPlayers;
      ptr = malloc(7 /* Header */ +  (2 * size * (WINBOLONET_KEY_LEN+1)) + WINBOLONET_KEY_LEN);
      ptr[0] = WINBOLO_NET_VERSION_MAJOR;
      ptr[1] = WINBOLO_NET_VERSION_MINOR;
      ptr[2] = WINBOLO_NET_VERSION_REVISION;
      ptr[3] = WINBOLO_NET_MESSAGE_SERVERUPDATE_REQ;
      ptr[4] = staticNumPlayers;
      ptr[5] = staticNumFreeBases;
      ptr[6] = staticNumFreePills;
      /* Is next */
      pos = 7;
      memcpy(ptr+7, winboloNetServerKey, WINBOLONET_KEY_LEN);
      pos += WINBOLONET_KEY_LEN;
      
      val = winbolonetEventsRemove(keyA, keyB);
      while (val != WINBOLONET_EVENT_NOITEM) {
        *(ptr+pos) = val;
        pos++;
        /* Key A */
        if (keyA[0] != EMPTY_CHAR) {
          memcpy((ptr+pos), keyA, WINBOLONET_KEY_LEN);
          pos += WINBOLONET_KEY_LEN;
        } else {
          *(ptr + pos) = EMPTY_CHAR;
          pos++;
        }
        /* Key B */
        if (keyB[0] != EMPTY_CHAR) {
          memcpy((ptr+pos), keyB, WINBOLONET_KEY_LEN);
          pos += WINBOLONET_KEY_LEN;
        } else {
          *(ptr + pos) = EMPTY_CHAR;
          pos++;
        }
        val = winbolonetEventsRemove(keyA, keyB);
      }

/*    *(ptr + pos) = 0;
  while (*(ptr + pos) != WINBOLONET_EVENT_NOITEM) {
        *(ptr + pos) = winbolonetEventsRemove((ptr+pos+1), (ptr+pos+1+WINBOLONET_KEY_LEN));
        if (*(ptr + pos) != WINBOLONET_EVENT_NOITEM) {
          if (*(ptr + pos) == WINBOLO_NET_EVENT_ALLY_JOIN || *(ptr + pos) == WINBOLO_NET_EVENT_ALLY_LEAVE) {
            /* Both keys are used *
            pos += WINBOLONET_KEY_LEN;
          }
          pos += 1 + WINBOLONET_KEY_LEN;
        }
      } */
      /* Send it off to winbolo.net */
//      fwrite(winboloNetServerKey, WINBOLONET_KEY_LEN, 1, stderr);
//      fputc('\n', stderr);
//      fprintf(stderr, "Sending off %d bytes to winbolo.net: \n", pos);

      if (sendNow == FALSE) {
        winbolonetThreadAddRequest(ptr, pos);
      } else {
        ret = httpSendMessage(ptr, pos, winboloNetBuff, WINBOLONET_BUFFSIZE);
        if (ret > 0) {
          if (winboloNetBuff[0] == 0) {
          /* Error */
            winboloNetBuff[ret] = '\0';
//            fprintf(stderr, "Error: %s\n", winboloNetBuff+1);
          }
        } else {
//          fprintf(stderr, "Error: %s\n", winboloNetBuff+1);
        }
      }

      free(ptr);
      winboloNetLastSent = time(NULL);
    }
  }
}

/*********************************************************
*NAME:          winbolonetRequestServerKey
*AUTHOR:        John Morrison
*CREATION DATE: 29/03/02
*LAST MODIFIED: 02/04/02
*PURPOSE:
* Tries to get a server key for this session. Returns
* success.
*
*ARGUMENTS:
* mapName - Name of the map
* port - Port we are running on
* gameType - Game Type
* ai - Is AI allowed
* mines - Mines allowed
* password - Has password
* numBases - Number of bases
* numPills - Number of pills
* freeBases - Free bases
* freePills - Free pills
* numPlayers - number of players
* startTime - Game start time
*********************************************************/
bool winbolonetRequestServerKey(char *mapName, unsigned short port, BYTE gameType, BYTE ai, bool mines, bool password, BYTE numBases, BYTE numPills, BYTE freeBases, BYTE freePills, BYTE numPlayers, long startTime) {
  bool returnValue;        /* Value to return */
  BYTE buff[FILENAME_MAX]; /* Sending buffer */
  int buffPos = 0;
  int ret;

  returnValue = FALSE;
  startTime = htonl(startTime);
  port = htons(port);

  buff[0] = WINBOLO_NET_VERSION_MAJOR;
  buff[1] = WINBOLO_NET_VERSION_MINOR;
  buff[2] = WINBOLO_NET_VERSION_REVISION;
  buff[3] = WINBOLO_NET_MESSAGE_SERVERKEY_REQ;
  buff[4] = gameType;
  buff[5] = ai;
  buff[6] = mines;
  buff[7] = password;
  buff[8] = freeBases;
  buff[9] = freePills;
  buff[10] = numPlayers;

  buff[11] = numBases;
  buff[12] = numPills;


  buffPos = 13;
  utilCtoPString(mapName, buff+13);
  buffPos = 14 + buff[13];

  memcpy(buff+buffPos, &port, sizeof(port));
  buffPos += sizeof(port);
  memcpy(buff+buffPos, &startTime, sizeof(startTime));
  buffPos += sizeof(startTime);


  ret = httpSendMessage(buff, buffPos, winboloNetBuff, WINBOLONET_BUFFSIZE);
  if (ret > 0) {
    if (winboloNetBuff[0] == 0) {
      /* Error */
      winboloNetBuff[ret] = EMPTY_CHAR;
//      fprintf(stderr, "Error: %s\n", winboloNetBuff+1);
    } else if (ret == 33) {
      memcpy(winboloNetServerKey, winboloNetBuff+1, WINBOLONET_KEY_LEN);
      returnValue = TRUE;
    }
  } else {
    fprintf(stderr, "Error: No response from WinBolo.net - Winbolo.net disabled\n");
  }

  return returnValue;
}

/*********************************************************
*NAME:          winbolonetRequestClientKey
*AUTHOR:        John Morrison
*CREATION DATE: 31/03/02
*LAST MODIFIED: 31/03/02
*PURPOSE:
* Attempts to get a client session key from WinBolo.net
* Returns success.
*
*ARGUMENTS:
* userName  - WinBolo.net account name
* password  - Password for the account
* serverKey - Session key for the server
* errorMsg  - Buffer to hold Error message if required
*********************************************************/
bool winbolonetRequestClientKey(char *userName, char *password, BYTE *serverKey, char *errorMsg) {
  bool returnValue;        /* Value to return */
  BYTE buff[FILENAME_MAX]; /* Sending buffer */
  int buffPos = 0;
  int ret;

  returnValue = FALSE;

  buff[0] = WINBOLO_NET_VERSION_MAJOR;
  buff[1] = WINBOLO_NET_VERSION_MINOR;
  buff[2] = WINBOLO_NET_VERSION_REVISION;
  buff[3] = WINBOLO_NET_MESSAGE_CLIENTKEY_REQ;
  memcpy(buff+4, serverKey, WINBOLONET_KEY_LEN);
  buffPos = 4 + WINBOLONET_KEY_LEN;
  utilCtoPString(userName, buff+buffPos);
  buffPos = buffPos + buff[buffPos] + 1;
  utilCtoPString(password, buff+buffPos);
  buffPos = buffPos + buff[buffPos] + 1;
  ret = httpSendMessage(buff, buffPos, winboloNetBuff, WINBOLONET_BUFFSIZE);
  if (ret > 0) {
    if (winboloNetBuff[0] == 0) {
      /* Error */
      winboloNetBuff[ret] = EMPTY_CHAR;
      strcpy(errorMsg, winboloNetBuff+1);
    } else if (ret == 33) {
      memcpy(winboloNetServerKey, serverKey, WINBOLONET_KEY_LEN);
      memcpy(winboloNetPlayerKey[0], winboloNetBuff+1, WINBOLONET_KEY_LEN);
      returnValue = TRUE;
    } else {
      strcpy(errorMsg, "Error: No response from WinBolo.net - Winbolo.net disabled");
    }

  } else {
    strcpy(errorMsg, "Error: No response from WinBolo.net - Winbolo.net disabled");
  }

  return returnValue;
}

/*********************************************************
*NAME:          winboloNetGetServerKey
*AUTHOR:        John Morrison
*CREATION DATE: 01/04/02
*LAST MODIFIED: 01/04/02
*PURPOSE:
* Copies the server key into keyBuff. Will be NULL if
* not participating in WinBolo.net
*
*ARGUMENTS:
* keyBuff - Buffer to hold key
*********************************************************/
void winboloNetGetServerKey(BYTE *keyBuff) {
  memcpy(keyBuff, winboloNetServerKey, WINBOLONET_KEY_LEN);
}

/*********************************************************
*NAME:          winboloNetGetMyClientKey
*AUTHOR:        John Morrison
*CREATION DATE: 01/04/02
*LAST MODIFIED: 01/04/02
*PURPOSE:
* Copies this clients key into keyBuff. Will be NULL if
* not set or not participating in WinBolo.net. Expected
* to be called by clients
*
*ARGUMENTS:
* keyBuff - Buffer to hold key
*********************************************************/
void winboloNetGetMyClientKey(BYTE *keyBuff) {
  memcpy(keyBuff, winboloNetPlayerKey[0], WINBOLONET_KEY_LEN);
}

/*********************************************************
*NAME:          winboloNetIsPlayerParticipant
*AUTHOR:        John Morrison
*CREATION DATE: 07/09/02
*LAST MODIFIED: 07/09/02
*PURPOSE:
* Returns if this player number is a winbolo.net
* particpant or not
*
*ARGUMENTS:
* playerNum - Player number to check
*********************************************************/
bool winboloNetIsPlayerParticipant(BYTE playerNum) {
  bool returnValue; /* Value to return */

  returnValue = winboloNetRunning;
  if (returnValue == TRUE) {
    if (winboloNetPlayerKey[playerNum][0] != EMPTY_CHAR) {
      returnValue = TRUE;
    }
  }
  return returnValue;
}

/*********************************************************
*NAME:          winboloNetVerifyClientKey
*AUTHOR:        John Morrison
*CREATION DATE: 01/04/02
*LAST MODIFIED: 01/04/02
*PURPOSE:
* Verifies a client key by sending it to the server for
* authentication. Returns if its a valid key for this
* session
*
*ARGUMENTS:
* keyBuff   - Buffer to hold key
* userName  - Username of the player
* playerNum - Player position Number
*********************************************************/
bool winboloNetVerifyClientKey(BYTE *keyBuff, char *userName, BYTE playerNum) {
  bool returnValue; /* Value to return */
  BYTE buff[FILENAME_MAX]; /* Sending buffer */
  int buffPos = 0;
  int ret;

  returnValue = FALSE;
  if (winboloNetPlayerKey[playerNum][0] != EMPTY_CHAR || winboloNetRunning == FALSE) {
    returnValue = FALSE;
  } else {
    buff[0] = WINBOLO_NET_VERSION_MAJOR;
    buff[1] = WINBOLO_NET_VERSION_MINOR;
    buff[2] = WINBOLO_NET_VERSION_REVISION;
    buff[3] = WINBOLO_NET_MESSAGE_VERIFYCLIENTKEY_REQ;
    memcpy(buff+4, winboloNetServerKey, WINBOLONET_KEY_LEN);
    buffPos = 4 + WINBOLONET_KEY_LEN;
    utilCtoPString(userName, buff+buffPos);
    buffPos = buffPos + buff[buffPos] + 1;
    memcpy(buff+buffPos, keyBuff, WINBOLONET_KEY_LEN);


    ret = httpSendMessage(buff, buffPos + WINBOLONET_KEY_LEN, winboloNetBuff, WINBOLONET_BUFFSIZE);
    if (ret > 0) {
      if (winboloNetBuff[0] == 0) {
        /* Error */
        winboloNetBuff[ret] = EMPTY_CHAR;
//        fprintf(stderr, winboloNetBuff+1);
      } else if (ret == 2 && winboloNetBuff[1] == 1) {
        returnValue = TRUE;
        /* Copy it */
        memcpy(winboloNetPlayerKey[playerNum], keyBuff, WINBOLONET_KEY_LEN);
      } else {
        fprintf(stderr, "Error: No response from WinBolo.net");
      }

    } else {
      fprintf(stderr, "Error: No response from WinBolo.net");
    }

  }
  return returnValue;
}

/*********************************************************
*NAME:          winboloNetClientLeaveGame
*AUTHOR:        John Morrison
*CREATION DATE: 01/04/02
*LAST MODIFIED: 01/04/02
*PURPOSE:
* Called when a player leaves the game. Tells the server
* so.
*
*ARGUMENTS:
* playerNum  - Player position Number
* numPlayers - Number of players now in the game
* freeBases  - Number of free bases
* freePills  - Number of free pills
*********************************************************/
void winboloNetClientLeaveGame(BYTE playerNum, BYTE numPlayers, BYTE freeBases, BYTE freePills) {
  BYTE buff[FILENAME_MAX]; /* Sending buffer */
  int ret;

  winbolonetAddEvent(WINBOLO_NET_EVENT_PLAYER_LEAVE, TRUE, playerNum, WINBOLO_NET_NO_PLAYER);
  if (winboloNetPlayerKey[playerNum][0] != EMPTY_CHAR && winboloNetRunning == TRUE) {
    /* Send an update first to flush buffers */
    winbolonetServerUpdate(numPlayers, freeBases, freePills, TRUE);
    /* Send actual quit item */
    buff[0] = WINBOLO_NET_VERSION_MAJOR;
    buff[1] = WINBOLO_NET_VERSION_MINOR;
    buff[2] = WINBOLO_NET_VERSION_REVISION;
    buff[3] = WINBOLO_NET_MESSAGE_CLIENTLEAVE_REQ;
    buff[4] = numPlayers;
    buff[5] = freeBases;
    buff[6] = freePills;
    memcpy(buff+7, winboloNetServerKey, WINBOLONET_KEY_LEN);
    memcpy(buff+7+WINBOLONET_KEY_LEN, winboloNetPlayerKey[playerNum], WINBOLONET_KEY_LEN);


    ret = httpSendMessage(buff, 7+WINBOLONET_KEY_LEN + WINBOLONET_KEY_LEN, winboloNetBuff, WINBOLONET_BUFFSIZE);
    if (ret > 0) {
      if (winboloNetBuff[0] == 0) {
        /* Error */
//        fprintf(stderr, winboloNetBuff+1);
      } else if (ret == 2 && winboloNetBuff[1] == 1) {
        /* Copy it */
//        fprintf(stderr, "Successfully ditched client\n");
      } else {
        fprintf(stderr, "Error: No response from WinBolo.net");
      }

    } else {
      fprintf(stderr, "Error: No response from WinBolo.net");
    }
    winboloNetBuff[ret] = EMPTY_CHAR;
    winboloNetPlayerKey[playerNum][0] = EMPTY_CHAR;
  }
}

/*********************************************************
*NAME:          winbolonetAddEvent
*AUTHOR:        John Morrison
*CREATION DATE: 04/04/02
*LAST MODIFIED: 04/04/02
*PURPOSE:
* Adds a WinBolo.net Event for sending to the server
*
*ARGUMENTS:
*  eventType - Type of event this is
*  isServer  - Are we the server for this and not a client
*  playerA   - Player A player Number
*  playerB   - Player B player Number
*********************************************************/
void winbolonetAddEvent(BYTE eventType, bool isServer, BYTE playerA, BYTE playerB) {
  BYTE *keyA;
  BYTE *keyB;
  BYTE emptyKey[WINBOLONET_KEY_LEN];
 
 if (winboloNetRunning == TRUE && isServer == TRUE) {  
    emptyKey[0] = EMPTY_CHAR;
    keyA = winboloNetPlayerKey[playerA];
    if (playerB == WINBOLO_NET_NO_PLAYER) {
      keyB = emptyKey;
    } else {
      keyB = winboloNetPlayerKey[playerB];
    }
    /* Add it */
    winbolonetEventsAddItem(eventType, keyA, keyB);
  }
}

/*********************************************************
*NAME:          winbolonetIsRunning
*AUTHOR:        John Morrison
*CREATION DATE: 14/04/02
*LAST MODIFIED: 14/04/02
*PURPOSE:
* Returns if the winbolonet module is running or not
*
*ARGUMENTS:
*
*********************************************************/
bool winbolonetIsRunning() {
  return winboloNetRunning;
}

/*********************************************************
*NAME:          winbolonetIsRunning
*AUTHOR:        John Morrison
*CREATION DATE: 10/01/04
*LAST MODIFIED: 10/01/04
*PURPOSE:
* Sends the game version to winbolo.net
*
*ARGUMENTS:
*
*********************************************************/
void winboloNetSendVersion() {
  BYTE buff[FILENAME_MAX]; /* Sending buffer */  

  if (winboloNetRunning == TRUE) {
    buff[0] = WINBOLO_NET_VERSION_MAJOR;
    buff[1] = WINBOLO_NET_VERSION_MINOR;
    buff[2] = WINBOLO_NET_VERSION_REVISION;
    buff[3] = WINBOLO_NET_VERSION;
    memcpy(buff+4, winboloNetServerKey, WINBOLONET_KEY_LEN);
    buff[4+WINBOLONET_KEY_LEN] = BOLO_VERSION_MINOR;
    buff[5+WINBOLONET_KEY_LEN] = BOLO_VERSION_REVISION;
    httpSendMessage(buff, 6+WINBOLONET_KEY_LEN, winboloNetBuff, WINBOLONET_BUFFSIZE);
  }
}

/*********************************************************
*NAME:          winbolonetSendLock
*AUTHOR:        John Morrison
*CREATION DATE: 10/01/04
*LAST MODIFIED: 10/01/04
*PURPOSE:
* Sends whether the game is locked or not to winbolo.net
*
*ARGUMENTS:
* isLocked - Is this game locked or not
*********************************************************/
void winboloNetSendLock(bool isLocked) {
  BYTE buff[FILENAME_MAX]; /* Sending buffer */  

  if (winboloNetRunning == TRUE) {
    buff[0] = WINBOLO_NET_VERSION_MAJOR;
    buff[1] = WINBOLO_NET_VERSION_MINOR;
    buff[2] = WINBOLO_NET_VERSION_REVISION;
    buff[3] = WINBOLO_NET_LOCK;
    memcpy(buff+4, winboloNetServerKey, WINBOLONET_KEY_LEN);
    buff[4+WINBOLONET_KEY_LEN] = isLocked;
    httpSendMessage(buff, 5+WINBOLONET_KEY_LEN, winboloNetBuff, WINBOLONET_BUFFSIZE);
  }
}
