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


#include <stdio.h>
#include <string.h>

#include "../winbolonet/http.h"

#ifdef _WIN32
  #include <windows.h>
  #include "../gui/resource.h"
#else
  #include <sys/time.h>
  #include <sys/types.h>
  #include <unistd.h>
  #include <signal.h>
  #include <errno.h>
  #include "SDL.h"
//  typedef Uint32 DWORD;
  #define USING_SDL
  #include "../gui/linresource.h"
#endif

#include "../bolo/global.h"
#include "../bolo/backend.h"
#include "servercore.h"
#include "../bolo/gametype.h"
#include "servernet.h"
#include "servertransport.h"
#include "threads.h"
#include "../winbolonet/winbolonet.h"

#ifndef MAX_PATH
	#define MAX_PATH 512
#endif


#include <ctype.h>
#include <time.h>

DWORD oldTick;     /* Number of ticks passed */
bool quitOnWinFlag = FALSE;
bool autoClose = FALSE;
bool isGameOver = FALSE;
bool printGameWinners = FALSE;
bool isQuiet = FALSE;
bool isNoInput = FALSE;
unsigned int serverTimerGameID = 1;
char fileName[MAX_PATH]; /* Log file Name */
bool isLogging = FALSE;
bool dontSendLog = FALSE;

time_t ticks = 0;

/* Game Tick */
#define SERVER_TICK_LENGTH (GAME_TICK_LENGTH*2)

#define WIND_CLASSNAME "WinBoloServ" 
#define WIND_TITLE "WinBoloServ"

/* There are 60 seconds in a minute */
#define NUM_SECONDS 60 

/* The network need read message */
#define WSA_READ     (WM_USER + 1)
#define WSA_CLOSE (WM_USER +2)

typedef enum {
  alarmNone,
  alarmInterrupt,
  alarmLock,
  alarmUnlock
} alarmType;


alarmType alarmRaised;

#ifndef _WIN32
/* Signal handler for linux */
void
catch_alarm (int sig)
{
  switch (sig) {
  case SIGINT:
    alarmRaised = alarmInterrupt;
    break;
  case SIGUSR1:
    alarmRaised = alarmLock;
    break;
  case SIGUSR2:
    alarmRaised = alarmUnlock;
    break;
  }
}

#endif


void strlower(char *s) {
  while(*s) {
    *s = tolower(*s);
    s++;
  }
}

void saveMap(char *line) {
  char *ptr;
  int len;
  ptr = line;
  ptr += 7;

  /* Strip newline */
  len =strlen(line);
  if (line[len-1] == '\n') {
    line[len-1] = '\0';
  }
  
  while (*ptr != EMPTY_CHAR  && (*ptr == '\t' || *ptr == ' ')) {
    ptr++;
  }
  if (*ptr == EMPTY_CHAR) {
    fprintf(stderr, "Sorry, you must enter a filename for this command\n");
  } else {
    len = strlen(ptr);
    if (len < 4) {
      strcat(ptr, ".map");
    } else if (strcmp(ptr+len-4, ".map") != 0) {
      strcat(ptr, ".map");
    }
    if (serverCoreSaveMap(ptr) == FALSE) {
      fprintf(stderr, "Sorry, an error occured saving the map. Is the path correct?\n");
    }
  }
}

bool serverCoreRunning();

void printHelp() {
  fprintf(stderr, "Help:\n Lock - Locks the server and stops new players from joining.\n Unlock - Unlocks the server and allows new players to join.\n savemap <map file> - Save the map file to path and file <map file>\n Say <text> - Sends this message to all players in the game unless they have turned off server messages.\n Quit - Exits the server.\n Info - Provide information about the current game\n");
}


#ifdef _WIN32
void processKeys(bool isQuiet) {
  char keyBuff[256] = "\0";
  char saveBuff[256] = "\0";
  
  if (isQuiet == TRUE || isNoInput == TRUE) {
    while (serverCoreRunning() == TRUE && isGameOver == FALSE) {
      Sleep(1000);
   }
  } else {
    while (strncmp(keyBuff, "quit", 4) != 0 && isGameOver == FALSE && serverCoreRunning()) {
      if (strncmp(keyBuff, "help", 4) == 0) {
        /* Help */
        printHelp();
      } else if (strncmp(keyBuff, "lock", 4) == 0) {
        serverNetSetLock(TRUE);
      } else if (strncmp(keyBuff, "unlock", 6) == 0) {
        serverNetSetLock(FALSE);
      } else if (strncmp(keyBuff, "info", 4) == 0) {
        serverCoreInformation();
      } else if (strncmp(keyBuff, "savemap", 7) == 0) {
        saveMap(saveBuff);
      } else if (strncmp(keyBuff, "say ", 4) == 0) {
        serverNetSendServerMessageAllPlayers((char *) keyBuff+4);
      } else if (strncmp(keyBuff, "\n", 1) != 0 && strncmp(keyBuff, "\0", 1) != 0) {
        fprintf(stderr, "Unknown command - Type \"help\" for help\n");
      }
      
      fgets(keyBuff, 256, stdin);
      strcpy(saveBuff, keyBuff);
      strlower(keyBuff);
    }
  }
}

#else
/* Linux */
void processKeys() {
  char keyBuff[256] = "\0";
  char saveBuff[256] = "\0";
  fd_set fdmask; /* for our select               */
  struct timeval timer; /*select timer */
  int ret;

  timer.tv_sec = 1;
  timer.tv_usec = 0;
  FD_ZERO(&fdmask);
  FD_SET(STDIN_FILENO, &fdmask);

  if (isQuiet == TRUE || isNoInput == TRUE) {
    while (serverCoreRunning() == TRUE && isGameOver == FALSE) {
    if (alarmRaised == alarmInterrupt) {
       break;
    } else if (alarmRaised == alarmLock) {
      serverNetSetLock(TRUE);	    
      alarmRaised = alarmNone;
    } else if (alarmRaised == alarmUnlock) {
      serverNetSetLock(FALSE);
      alarmRaised = alarmNone;
    }


    sleep(1);
    }
  } else {
    while (strncmp(keyBuff, "quit", 4) != 0 && isGameOver == FALSE && serverCoreRunning()) {
      if (strncmp(keyBuff, "help", 4) == 0) {
        /* Help */
        printHelp();
      } else if (strncmp(keyBuff, "lock", 4) == 0) {
        serverNetSetLock(TRUE);
      } else if (strncmp(keyBuff, "info", 4) == 0) {
        serverCoreInformation();
      } else if (strncmp(keyBuff, "unlock", 6) == 0) {
        serverNetSetLock(FALSE);
      } else if (strncmp(keyBuff, "savemap", 7) == 0) {
        saveMap(saveBuff);
      } else if (strncmp(keyBuff, "say ", 4) == 0) {
        serverNetSendServerMessageAllPlayers((char *) keyBuff+4);
      } else if (strncmp(keyBuff, "\n", 1) != 0 && strncmp(keyBuff, "\0", 1) != 0) {
        fprintf(stderr, "Unknown command - Type \"help\" for help\n");
      }
    
      
      timer.tv_sec = 1;
      timer.tv_usec = 0;
      FD_ZERO(&fdmask);
      FD_SET(STDIN_FILENO, &fdmask);

      ret =  select(STDIN_FILENO + 1, &fdmask, NULL, NULL, &timer);
      if (ret > 0) {
        fgets(keyBuff, 256, stdin);
	      strcpy(saveBuff, keyBuff);
        strlower(keyBuff);
      } else if (ret != -1) {
	      keyBuff[0] = '\0';
      }

     if (alarmRaised == alarmInterrupt) {
       strcpy(keyBuff, "quit"); 
     } else if (alarmRaised == alarmLock) {
       strcpy(keyBuff, "lock");
       alarmRaised = alarmNone;
     } else if (alarmRaised == alarmUnlock) {
       strcpy(keyBuff, "unlock");
       alarmRaised = alarmNone;
     }
    }
  }
}


#endif

/*********************************************************
*NAME:          serverGameTimer
*AUTHOR:        John Morrison
*CREATION DATE: 24/11/98
*LAST MODIFIED: 20/3/99
*PURPOSE:
* The Game Timer. If there are no events to prcess this 
* routine is called. If the elapsed
*
*ARGUMENTS:
*
*********************************************************/
#ifdef _WIN32
void CALLBACK serverGameTimer(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
  DWORD tick;     /* Number of ticks passed */
  static int trackerTime = 5500;   /* When we should update the tracker */
  static int wbnTime = 0;

  tick = winboloTimer();
#else
  Uint32 serverGameTimer (Uint32 interval) {
  DWORD tick;     /* Number of ticks passed */
  static int wbnTime = 0;
  static int trackerTime = 5500;   /* When we should update the tracker */
  tick = winboloTimer();
#endif

  if ((tick - oldTick) > SERVER_TICK_LENGTH) {
    /* Get the keyboard state */
    while ((tick - oldTick) > SERVER_TICK_LENGTH) {
      trackerTime++;
      wbnTime++;
      
      threadsWaitForMutex();
      serverCoreGameTick();
      threadsReleaseMutex();
      ticks++;
      oldTick += SERVER_TICK_LENGTH;
    }
    if (quitOnWinFlag == TRUE || autoClose == TRUE || (winbolonetIsRunning() == TRUE && serverCoreGetActualGameType() != gameOpen)) {
      threadsWaitForMutex();
      if (quitOnWinFlag == TRUE && isGameOver == FALSE){
		isGameOver = serverCoreCheckGameWin(printGameWinners);
	  }
      if (autoClose == TRUE && isGameOver == FALSE) {
        isGameOver = serverCoreCheckAutoClose();
      }
      threadsReleaseMutex();
      #ifdef _WIN32  
      if (isQuiet == FALSE && isGameOver == TRUE) {
        serverNetSendQuitMessage();
        timeKillEvent(serverTimerGameID);



        if (isLogging == TRUE && winbolonetIsRunning() == TRUE && dontSendLog == FALSE) {
          winboloNetGetServerKey(key);
        } else {
          key[0] = EMPTY_CHAR;
        }

        winbolonetDestroy();
        serverNetDestroy();
        threadsDestroy();
        serverCoreStopLog();

        if (isLogging == TRUE && key[0] != EMPTY_CHAR && dontSendLog == FALSE) {
          screenServerConsoleMessage((char *)"Uploading log file to winbolo.net");  
          httpCreate();
          httpSendLogFile(fileName, key, FALSE);
          httpDestroy();
        }
        serverCoreDestroy();
        WSACleanup();
        exit(0);
      }
      #endif
    }
  } 
  
  serverNetCheckRemovePlayers();
  serverNetMakePosPackets();
  serverNetMakeData();
  
  
  if (wbnTime > 100) {
    threadsWaitForMutex();
    winbolonetServerUpdate(serverCoreGetNumPlayers(), serverCoreGetNumNeutralBases(), serverCoreGetNumNeutralPills(), FALSE);
    threadsReleaseMutex();
    wbnTime = 0;
  }


  if (trackerTime >= (6000)) {
    serverNetSendTrackerUpdate();
    trackerTime = 0;
  }
#ifdef USING_SDL 
  return interval;
#endif
}

void printArgs() {
#ifdef _WIN32
  fprintf(stderr, "Usage:\nWinBoloDS -map <Filename> -port <Port> -gametype <GameType> -mines <Mines> -ai <AiType> -delay <Delay> -limit <Limit> -tracker <Tracker> -password <Password>\n\n");
#else 
  fprintf(stderr, "Usage:\nLinBoloDS -map <Filename> -port <Port> -gametype <GameType> -mines <Mines> -ai <AiType> -delay <Delay> -limit <Limit> -tracker <Tracker> -password <Password>\n\n");
#endif
  fprintf(stderr, "<Filename>    - Path and file name of the map file to open (-inbuilt can be used\n");
  fprintf(stderr, "                instead of -map to enable inbuilt map Everard Island)\n");
  fprintf(stderr, "<Port>        - Port to run the server on\n");
  fprintf(stderr, "<GameType>    - Specifies the game type: \"Open\" or \"Tournament\" or \"Strict\"\n");
  fprintf(stderr, "\nOptional\n");
  fprintf(stderr, "<Mines>       - Specifies allowing hidden mines: \"yes\" for allow,\n");
  fprintf(stderr, "                \"no\" for disallow (on if not specified)\n" );
  fprintf(stderr, "<AiType>      - Specifies allowing brains. Valid values are \"no\" for\n");
  fprintf(stderr, "                disallowing, \"yes\" for allowing and \"yesAdv\" for giving\n");
  fprintf(stderr, "                them an advantage. (disallowed if not specified)\n");
  fprintf(stderr, "<Delay>       - Specifies the start delay (in seconds) (none if not specified)\n");
  fprintf(stderr, "<Limit>       - Specifies the game time limit (in minutes)\n");
  fprintf(stderr, "                \"-1\" for no time limit (none if not specified)\n");
  fprintf(stderr, "<Password>    - Game Password (none if not specified)\n");
  fprintf(stderr, "<tracker>     - address:port of the internet tracker to notify\n\n");
  fprintf(stderr, "-quiet        - No screen input or output (silent mode)\n");
  fprintf(stderr, "-noinput      - No keyboard input\n");
  fprintf(stderr, "-addr         - Specify a different address to use if avaliable\n");
  fprintf(stderr, "-autoclose    - Automatically quit the server when all players have left\n");
  fprintf(stderr, "                the game\n");
  fprintf(stderr, "-nowinbolonet - Do not participate in winbolo.net game tracking\n");
  fprintf(stderr, "-logfile      - Write all output to file instead of console.\n");
  fprintf(stderr, "-maxplayers   - Specifies the maximum number of players that can be on this\n");
  fprintf(stderr, "                server.\n");
  fprintf(stderr, "-log          - Create game log file (filename optional)\n");
  fprintf(stderr, "-dontsendlog  - Don't upload game log to winbolo.net\n");
}


void processTrackerArg(char *argItem, char *trackerAddr, unsigned short *trackerPort) {
  char *tmp;
  tmp = strtok(argItem, ":");
  strcpy(trackerAddr, tmp);
  tmp = strtok(NULL, ":");
  *trackerPort = atoi(tmp);
}

#define ARG_NOT_FOUND -1 

int findArg(int numArgs, char **argv, const char *argname) {
  int returnValue; /* Value to return */
  char temp[255];
  int count;

  sprintf(temp, "-%s", argname);
  strlower(temp);
  returnValue = ARG_NOT_FOUND;
  count = 0;
  while (returnValue == -1 && count < numArgs) {
    if (strcmp((char *) argv[count], temp) == 0) {
      returnValue = count+1;
    }
    count++;
  }

  /* Make sure we don't fall off the end of the arguments */
  if (returnValue == numArgs) {
    returnValue = ARG_NOT_FOUND;
  }

  return returnValue;
}


bool argExist(int numArgs, char **argv, char *argname) {
  bool returnValue; /* Value to return */
  char temp[255];
  int count;

  sprintf(temp, "-%s", argname);
  strlower(temp);
  returnValue = FALSE;
  count = 0;
  while (returnValue == FALSE && count < numArgs) {
    strlower((char *) argv[count]);
    if (strcmp((char *) argv[count], temp) == 0) {
      returnValue = TRUE;
    }
    count++;
  }


  return returnValue;
}

/** Generates a log file name based on current time and map name and copies to fileName */
void makeLogFileName(char *fileName) {
  time_t t;
  struct tm *tmt;
  int count = 0;
  int len;
  char mapName[256];

  serverCoreGetMapName(mapName);
  time(&t);
  tmt = localtime(&t);
  
  sprintf(fileName, "%04d%02d%02dt%02d%02d%02d_%s", (1900 + tmt->tm_year), (1 + tmt->tm_mon), tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, mapName);
  len = strlen(fileName);
  /* Replace spaces with underscores */
  while (count < len){
    if (fileName[count] == ' ') {
      fileName[count] = '_';
    }
    count++;
  }
}

bool processArgs(int numArgs, char **argv, char *mapName, unsigned short *port, gameType *game, bool *hiddenMines, aiType *ai, int *srtDelay, long *gmeLen, char *trackerAddr, unsigned short *trackerPort, bool *trackerUse, char *password) {
  bool returnValue; /* Value to return */
  int argNum;
  char temp[255];

  returnValue = TRUE;
  
  /* Map */
  argNum = findArg(numArgs, argv, "map");
  if (argNum != ARG_NOT_FOUND) {
    strcpy(mapName, (char *) argv[argNum]);
  } else if (argExist(numArgs, argv, "inbuilt") == TRUE) {
    strcpy(mapName, "-inbuilt");
  } else {
    fprintf(stderr, "Missing map file\n");
    returnValue = FALSE;
  }
  
  /* Port */
  argNum = findArg(numArgs, argv, "port");
  if (argNum != ARG_NOT_FOUND) {
    *port = atoi((char *) argv[argNum]);
  } else {
    fprintf(stderr, "Missing port Number\n");
    returnValue = FALSE;
  }


  /* Game type */
  argNum = findArg(numArgs, argv, "gametype");
  if (argNum != ARG_NOT_FOUND) {
    strlower((char *) argv[argNum]);
    strcpy (temp, (char *) argv[argNum]);
    if (strcmp((char *) argv[argNum], "open") == 0) {
      *game = gameOpen;
    } else if (strcmp((char *) argv[argNum], "tournament") == 0) {
      *game = gameTournament;
    } else if (strcmp((char *) argv[argNum], "strict") == 0) {
      *game = gameStrictTournament;
    } else {
      returnValue = FALSE;
      fprintf(stderr, "Error in game type parameter\n");
    }
  } else {
    fprintf(stderr, "Missing game type parameter\n");
    returnValue = FALSE;
  }

  /* Option Arguments */
  
  /* Tracker */
  *trackerUse = FALSE;
  argNum = findArg(numArgs, argv, "tracker");
  if (argNum != ARG_NOT_FOUND) {
    *trackerUse = TRUE;
    processTrackerArg((char *) argv[argNum], trackerAddr, trackerPort);
  }

  /* Mines */
  argNum = findArg(numArgs, argv, "mines");
  if (argNum != ARG_NOT_FOUND) {
    strlower((char *) argv[argNum]);
    if (strcmp((char *) argv[argNum], "yes") == 0) {
      *hiddenMines = TRUE;
    } else if (strcmp((char *) argv[argNum], "no") == 0) {
      *hiddenMines = FALSE;
    } else {
      returnValue = FALSE;
      fprintf(stderr, "Error in hidden mines parameter\n");
    }
  } else {
    *hiddenMines = TRUE;
  }

  /* Allow AI */
  argNum = findArg(numArgs, argv, "ai");
  if (argNum != ARG_NOT_FOUND) {
    strlower((char *) argv[argNum]);
    if (strcmp((char *) argv[argNum], "no") == 0) {
      *ai = aiNone;
    } else if (strcmp((char *) argv[argNum], "yes") == 0) {
      *ai = aiYes;
    } else if (strcmp((char *) argv[argNum], "yesadv") == 0) {
      *ai = aiYesAdvantage;
    } else {
      returnValue = FALSE;
      fprintf(stderr, "Error in ai parameter\n");
    }
  } else {
    *ai = aiNone;
  }

  /* Start delay */
  argNum = findArg(numArgs, argv, "delay");
  if (argNum != ARG_NOT_FOUND) {
    *srtDelay = atoi((char *) argv[argNum]);
    if (*srtDelay < 0) {
      *srtDelay = 0;
    } else if (*srtDelay > 0) {
      (*srtDelay) *= GAME_NUMGAMETICKS_SEC;
    }
  } else {
    *srtDelay = 0;
  }

  /* Time limit */
  argNum = findArg(numArgs, argv, "limit");
  if (argNum != ARG_NOT_FOUND) {
    *gmeLen = atoi((char *) argv[argNum]);
    if (*gmeLen <= 0) {
      *gmeLen = -1;
    } else if (*gmeLen > 0) {
      (*gmeLen) *= GAME_NUMGAMETICKS_SEC;
      (*gmeLen) *= NUM_SECONDS;
    }
  } else {
    *gmeLen = -1;
  }


  /* Password */
  argNum = findArg(numArgs, argv, "password");
  if (argNum != ARG_NOT_FOUND) {
    strcpy(password, (char *) argv[argNum]);
  } else {
    password[0] = '\0';
  }


  return returnValue;
}

#include <time.h>

int main(int argc, char **argv) {
  gameType game;
  unsigned short port;
  bool hiddenMines;
  int srtDelay;
  long gmeLen;
  char pass[FILENAME_MAX]; /* Password */
  char mapName[2048];
  aiType ai; /* Should we allow ai */
  /* Tracker stuff */
  char trackerAddr[FILENAME_MAX];
  unsigned short trackerPort;
  bool trackerUse;
  char *useAddr;
  int maxPlayers;
  BYTE key[32]; /* WBN Key */

  threadsSetContext(TRUE);
  serverCoreSetQuietMode(FALSE);
  isQuiet = FALSE;
  isNoInput = FALSE;
  maxPlayers = 0;


  alarmRaised = alarmNone;
  initWinboloTimer();
#ifndef _WIN32 
  /* Set up signal handler */
  signal(SIGINT, catch_alarm);
  signal(SIGUSR1, catch_alarm);
  signal(SIGUSR2, catch_alarm);
#endif
    
  if (processArgs(argc, argv, mapName, &port, &game, &hiddenMines, &ai, &srtDelay, &gmeLen, trackerAddr, &trackerPort, &trackerUse, pass) == FALSE) {
    fprintf(stderr, "Error in command line parameters\n");
    printArgs();
    exit(0);
  }
  if (argExist(argc, argv, "quiet") == TRUE) {
    serverCoreSetQuietMode(TRUE);
    isQuiet = TRUE;
  }
  isNoInput = argExist(argc, argv, "noinput");
  if (findArg(argc, argv, "logfile") != ARG_NOT_FOUND) {
    serverCoreSetServerLogFile((char *) argv[findArg(argc, argv, "logfile")]);
  }

  if (argExist(argc, argv, "maxplayers") == TRUE) {
    maxPlayers = atoi((char *) argv[findArg(argc, argv, "maxplayers")]);
    if (maxPlayers < 0 || maxPlayers > MAX_TANKS) {
      maxPlayers = 0;
    }
  }
  
#ifdef USING_SDL 
  if (SDL_Init(SDL_INIT_TIMER) < 0) {
    fprintf(stderr, "Error starting SDL - %s\n", SDL_GetError());
    exit(0);
  }
#endif
  if (strcmp(mapName, "-inbuilt") == 0) {
    /* Use the inbuilt map */
#ifdef _WIN32
    HGLOBAL hGlobal;  /* Resource handle */
    BYTE *buff;       /* Byte buffer     */
    HRSRC res;        /* FindResource return */

    res = FindResource(NULL, MAKEINTRESOURCE(IDR_EVERARD), "MAPS");
    if (res != NULL) {
      hGlobal = LoadResource(NULL, res);
      if (hGlobal != NULL) {
        buff = LockResource(hGlobal);
        if (buff != NULL) {
          serverCoreCreateCompressed(buff, 5097, "Everard Island", game, hiddenMines, srtDelay, gmeLen);
        }
      }
    }
#else
  BYTE emap[6000] = E_MAP;
  serverCoreCreateCompressed(emap, 5097, "Everard Island", game, hiddenMines, srtDelay, gmeLen);

#endif
  } else {
    if (serverCoreCreate(mapName, game, hiddenMines, srtDelay, gmeLen) == FALSE) {
      fprintf(stderr, "Error starting Core Simulation\n");
#ifdef USING_SDL
      SDL_Quit();
#endif
      return 0;
    }
  }
  useAddr = NULL;
  if (argExist(argc, argv, "addr") == TRUE) {
    useAddr = (char *) argv[findArg(argc, argv, "addr")];
  }

  quitOnWinFlag = argExist(argc,argv, "quitonwin");
  autoClose = argExist(argc, argv, "autoclose");
  printGameWinners = argExist(argc, argv, "printwinners");

  if (serverNetCreate(port, pass, ai, trackerAddr, trackerPort, trackerUse, useAddr, (BYTE) maxPlayers) == FALSE) {
    fprintf(stderr, "Error starting Network\n");
    serverNetDestroy();
    serverCoreDestroy();
#ifdef USING_SDL
    SDL_Quit();
#endif
return 0;
  }

  if (argExist(argc, argv, "nowinbolonet") == FALSE) {
    char mapName[255];
    serverCoreGetMapName(mapName);
    winbolonetCreateServer(mapName, port, (BYTE) game, (BYTE) ai, serverCoreGetAllowHiddenMines(), (BYTE) (pass[0] == 0 ? FALSE : TRUE), screenNumBases(), screenNumPills(), serverCoreGetNumNeutralBases(), serverCoreGetNumNeutralPills(), serverCoreGetNumPlayers(), serverCoreGetTimeGameCreated());
  }
  dontSendLog = argExist(argc, argv, "dontsendlog");

  /* Log file generation */
  if (argExist(argc, argv, "log") == TRUE) {
    char logFileName[512];
    int arg = findArg(argc, argv, "log");

    if (arg == ARG_NOT_FOUND) {
      makeLogFileName(logFileName);
    } else if ((char) argv[arg][0] == '-') {
      makeLogFileName(logFileName);
    } else {
      strcpy(logFileName, (char *) argv[arg]);
    }
    if (strlen(logFileName) > 4) {
      if (strcmp(logFileName + (strlen(logFileName)-4), ".wbv") != 0) {
        strcat(logFileName, ".wbv");
      }
    } else {
      strcat(logFileName, ".wbv");
    }
    strcpy(fileName, logFileName);
    isLogging = serverCoreStartLog(logFileName, ai, maxPlayers, (BYTE) (pass[0] == 0 ? FALSE : TRUE));
  }


  if (threadsCreate(TRUE) == FALSE) {
    fprintf(stderr, "Error starting Thread Manger\n");
    threadsDestroy();
    serverNetDestroy();
    serverCoreDestroy();
    #ifdef USING_SDL
          SDL_Quit();
    #endif
	  return 0;
  } 
  screenServerConsoleMessage("Type \"help\" for help, \"quit\" to exit.");
#ifdef _WIN32
  oldTick = winboloTimer();
  serverTimerGameID = timeSetEvent(SERVER_TICK_LENGTH, 10, serverGameTimer, 0, TIME_PERIODIC);
#else
  oldTick = winboloTimer();
  serverTimerGameID = SDL_SetTimer(SERVER_TICK_LENGTH, (SDL_TimerCallback) serverGameTimer);

#endif

  // Comment out to quit straight away
  processKeys(isQuiet);
  serverNetSendQuitMessage();
#ifdef _WIN32  
  timeKillEvent(serverTimerGameID);
#else
  SDL_SetTimer(0, NULL);
#endif
  serverNetDestroy();
  threadsDestroy();
  serverCoreStopLog();

  if (isLogging == TRUE && winbolonetIsRunning() == TRUE && argExist(argc, argv, "dontsendlog") == FALSE) {
    winboloNetGetServerKey(key);
  } else {
    key[0] = EMPTY_CHAR;
  }

  winbolonetDestroy();

  if (isLogging == TRUE && key[0] != EMPTY_CHAR && argExist(argc, argv, "dontsendlog") == FALSE) {
    screenServerConsoleMessage((char *)"Uploading log file to winbolo.net");  
    httpCreate();
    httpSendLogFile(fileName, key, FALSE);
    httpDestroy();
  }
  endWinboloTimer();
  serverCoreDestroy();
#ifdef _WIN32
  WSACleanup();
#endif
  return 0;
}

time_t serverMainGetTicks() {
  return ticks;
}
