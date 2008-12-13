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
*Name:          currentGames
*Filename:      currentGames.c
*Author:        John Morrison
*Creation Date: 00/01/18
*LAST MODIFIED: 00/10/01
*Purpose:
*  Responsable for holding the list of current games
*********************************************************/

#include <pthread.h>
#include <time.h>
#include "irc.h"
#include "global.h"
#include "currentgames.h"
#include "stats.h"

extern long statsGames;

pthread_mutex_t currentGamesMutex;

/*********************************************************
*NAME:          currentGamesCreate
*AUTHOR:        John Morrison
*CREATION DATE: 00/01/18
*LAST MODIFIED: 00/01/18
*PURPOSE:
*  Sets up the currentGames data structure
*
*ARGUMENTS:
*
*********************************************************/
currentGames currentGamesCreate(void) {
	/*currentGamesMutex = PTHREAD_MUTEX_INITIALIZER;*/
	pthread_mutex_init(&currentGamesMutex, NULL);
	return NULL;
}

/*********************************************************
 *NAME:          currentGamesLock
 *AUTHOR:        Andrew Roth
 *CREATION DATE: 03/01/22
 *LAST MODIFIED: 03/01/22
 *PURPOSE:
 *  Waits until a lock is obtained on the currentGames 
 *  structure.  When it is finished with the currentGames
 *  structure, it should call currentGamesUnlock
 *
 *ARGUMENTS:
 **********************************************************/
void currentGamesLock(void) {
	pthread_mutex_lock(&currentGamesMutex);
}

/*********************************************************
 *NAME:          currentGamesUnlock
 *AUTHOR:        Andrew Roth
 *CREATION DATE: 03/01/22
 *LAST MODIFIED: 03/01/22
 *PURPOSE:
 *  unlocks the currentgames structure.  only a thread
 *  which has a lock should do this
 *
 *ARGUMENTS:
 **********************************************************/
void currentGamesUnlock(void) {
	pthread_mutex_unlock(&currentGamesMutex);
}

/*********************************************************
 *NAME:          currentGamesDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE: 00/01/18
 *LAST MODIFIED: 00/01/18
 *PURPOSE:
 *  Destroys the currentGames data structure
 *
 *ARGUMENTS:
 *  value - The structure to destroy
 *********************************************************/
void currentGamesDestroy(currentGames *value) {
	currentGames q;

	printf("Destroying game\n");

	while (NonEmpty(*value)) {
		q = *value;
		*value = CurrentTail(*value);
		Dispose(q);
	}
}

/*********************************************************
 *NAME:          currentGamesUpdate
 *AUTHOR:        John Morrison and Andrew Roth
 *CREATION DATE: 00/01/18
 *LAST MODIFIED: 01/03/22
 *PURPOSE:
 * This function adds a game to the game list. (or updates
 * if it exists)  It does NOT delete expired games now, use
 * currentGamesPurge for that
 *
 *ARGUMENTS:
 *  value      - The currentGames structure
 *  address    - Server address
 *  port       - Server port
 *  mapName    - Name of the map
 *  version    - Game version
 *  numPlayers - Number of players
 *  numBases   - Number of bases
 *  numPills   - Number of pills
 *  mines      - Allow hidden mines
 *  game       - Game type
 *  ai         - Ai type
 *  password   - Has password
 *  starttime  - Time the game started
 *  startdelay - Start delay time
 *  timelimit  - Game time limit
 *  ip         - 4 byte IP for stats purposes
 *********************************************************/
void currentGamesUpdate(currentGames *value, char *address, unsigned short port, char *mapName, char *version, BYTE numPlayers, BYTE numBases, BYTE numPills, bool mines, gameType game, aiType ai, bool password, unsigned long starttime, long startdelay, long timelimit, unsigned char *ip) {
	bool found;          /* Have we found the item */
	currentGames q;      /* Temp pointers */
	time_t currentTime;  /* Current time */
        char str[IRC_MAX_GAME_LEN];

	/* this method needs access to the current games strucutre */
	currentGamesLock();

	found = FALSE;
	q = *value;
	currentTime = time(NULL);
	while (NonEmpty(q) && found == FALSE) {
		/*printf("Searching for game... my starttime %ld, searching %ld\n", starttime, q->starttime);*/
		if (memcmp(ip, q->ip, 4) == 0 && q->port == port && q->starttime == starttime) {
			/* We found it */
			fprintf(stderr, "GAMES UPDATE: found and updated %s:%d\n", q->address, q->port);
			found = TRUE;
			q->numPlayers = numPlayers;
			q->numBases = numBases;
			q->numPills = numPills;
			q->timelimit = timelimit;
			q->lastpacket = currentTime;
			statsSaveGame(q->gameLogOffset, q);
		} else
			q = q->next;
	}

	if (found == FALSE) {
		/* Add it */
		currentGamesAddItem(value, address, port, mapName, version, numPlayers, numBases, numPills, mines, game, ai, password, starttime, startdelay, timelimit, ip);
		fprintf(stderr, "GAMES UPDATE: didn't find %s:%d, adding new game\n", address, port);
		currentGamesCreateIrcString(str, *value); /* want to release the current games lock asap */
		currentGamesUnlock();
		ircSendClientsMessage(str);     /* so send irc msg after unlocking */
	} else {
		currentGamesUnlock();
	}
}



/*********************************************************
 *NAME:          currentGamesAddItem
 *AUTHOR:        John Morrison
 *CREATION DATE: 00/01/18
 *LAST MODIFIED: 00/10/01
 *PURPOSE:
 *  Adds an item to the currentGames data structure.
 *  This method does NOT lock the currentGames strucutre,
 *  it should be done by the method calling it.
 *
 *  (currently only currentGamesUpdate calls this method,
 *  and currentGamesUpdate does lock the games list
 *
 *ARGUMENTS:
 *  value      - The currentGames structure
 *  address    - Server address
 *  port       - Server port
 *  mapName    - Name of the map
 *  version    - Game version
 *  numPlayers - Number of players
 *  numBases   - Number of bases
 *  numPills   - Number of pills
 *  mines      - Allow hidden mines
 *  game       - Game type
 *  ai         - Ai type
 *  password   - Has password
 *  starttime  - Time the game started
 *  startdelay - Start delay time
 *  timelimit  - Game time limit
 *  ip         - 4 byte IP for stats purposes
 *********************************************************/
void currentGamesAddItem(currentGames *value, char *address, unsigned short port, char *mapName, char *version, BYTE numPlayers, BYTE numBases, BYTE numPills, bool mines, gameType game, aiType ai, bool password, unsigned long starttime, long startdelay, long timelimit, unsigned char *ip) {
	currentGames q;

	/*fprintf(stderr, "GAMES: Adding new game\n");*/

	New(q);
	strcpy(q->address, address);
	strcpy(q->mapName, mapName);
	strcpy(q->version, version);
	memcpy(q->ip, ip, IP_SIZE);
	q->port = port;
	q->numPlayers = numPlayers;
	q->numBases = numBases;
	q->numPills = numPills;
	q->mines = mines;
	q->game = game;
	q->ai = ai;
	q->password = password;
	q->starttime = starttime;
	q->startdelay = startdelay;
	q->timelimit = timelimit;
	q->lastpacket = time(NULL);
	q->next = *value;
	*value = q;

	statsLogConnection(STATS_LOG_NEW_GAME, ip);
	statsGames++;
	q->gameLogOffset = statsSaveGameStart(*value);
}

/*********************************************************
 *NAME:          currentGamesCreateIrcString
 *AUTHOR:        Andrew Roth
 *CREATION DATE: 03/03/29 YMD
 *LAST MODIFIED: 03/03/29
 *PURPOSE:
 *  Creates the irc string into str
 *
 *  Note the currentGames structure is not locked in this
 *  method but it should be locked by a calling method
 *
 *ARGUMENTS:
 *  str - a pointer to the irc string
 *  q - a pointer to the game to make the irc string from
 *********************************************************/
void currentGamesCreateIrcString(char str[IRC_MAX_GAME_LEN], currentGames q) {
	char gameStr[50], passwordStr[50];

	switch (q->game) {
		case gameOpen:
			strcpy(gameStr, "Open");
			break;
		case gameTournament:
			strcpy(gameStr, "Tournament");
			break;
		default:
			strcpy(gameStr, "Strict");
	}
	if (q->password) {
		sprintf(passwordStr, " - Password");
	} else {
		sprintf(passwordStr, " ");
	}
	sprintf(str, "/say  %c%s%c @ winbolo://%s:%d - %s - %s%s", (unsigned char)2,
			q->mapName, (unsigned char)2, q->address, 
			q->port, q->version, gameStr, passwordStr);
}

/*********************************************************
 *NAME:          currentGamesItemCount
 *AUTHOR:        John Morrison
 *CREATION DATE: 00/01/18
 *LAST MODIFIED: 00/10/01
 *PURPOSE:
 *  Returns the number of elements in the structure. Also
 *  purges empty games
 *
 *ARGUMENTS:
 *  value - The currentGames structure
 *********************************************************/
int currentGamesItemCount(currentGames *value) {
	currentGames q;      /* Temp pointers */
	int returnValue;     /* Value to return */
	time_t currentTime;  /* Current time */

	returnValue = 0;
	q = *value;
	currentTime = time(NULL);
	while (NonEmpty(q)) {
		q = CurrentTail(q);
		returnValue++;
	}
	return returnValue;
}

/*********************************************************
 *NAME:          currentGamesGetItem
 *AUTHOR:        John Morrison
 *CREATION DATE: 00/01/18
 *LAST MODIFIED: 19/1/00
 *PURPOSE:
 *  Gets the item for an itemNum
 *
 *ARGUMENTS:
 *  value      - The currentGames structure
 *  itemNum    - Item number to get record for
 *  address    - Server address
 *  port       - Server port
 *  mapName    - Name of the map
 *  version    - Game version
 *  numPlayers - Number of players
 *  numBases   - Number of bases
 *  numPills   - Number of pills
 *  mines      - Allow hidden mines
 *  game       - Game type
 *  ai         - Ai type
 *  password   - Has password
 *  starttime  - Time the game started
 *  startdelay - Start delay time
 *  timelimit  - Game time limit
 *********************************************************/
void currentGamesGetItem(currentGames *value, int itemNum, char *address, unsigned short *port, char *mapName, char *version, BYTE *numPlayers, BYTE *numBases, BYTE *numPills, bool *mines, gameType *game, aiType *ai, bool *password, unsigned long *starttime, long *startdelay, long *timelimit) {
	currentGames q;
	int count; /* Looping variable */

	if (itemNum <= currentGamesItemCount(value)) { /* FIXME: Optimal? I don't think so */
		count = 1;
		q = *value;
		while (count < itemNum) {
			q = CurrentTail(q);
			count++;
		}
		strcpy(address, q->address);
		strcpy(mapName, q->mapName);
		strcpy(version, q->version);
		/*    printf("GET ITEM: gameItem %d, Version is %s\n", itemNum, q->version);*/
		*port = q->port;
		*numPlayers = q->numPlayers;
		*numBases = q->numBases;
		*numPills = q->numPills;
		*mines = q->mines;
		*game = q->game;
		*ai = q->ai;
		*password = q->password;
		*starttime = q->starttime;
		*startdelay = q->startdelay;
		*timelimit = q->timelimit;
	}
}

/*********************************************************
 *NAME:          currentGamesGetServerName
 *AUTHOR:        John Morrison
 *CREATION DATE: 00/01/18
 *LAST MODIFIED: 00/01/18
 *PURPOSE:
 *  Gets the server name for an itemNum
 *
 *ARGUMENTS:
 *  value   - The currentGames structure
 *  itemNum - Item number to get record for
 *  address - Server address
 *********************************************************/
void currentGamesGetServerName(currentGames *value, int itemNum, char *address) {
	currentGames q;
	int count; /* Looping variable */

	if (itemNum <= currentGamesItemCount(value)) {
		count = 1;
		q = *value;
		while (count < itemNum) {
			q = CurrentTail(q);
			count++;
		}
		strcpy(address, q->address);
	}
}


/*********************************************************
 *NAME:          currentGamesItemCount
 *AUTHOR:        Andrew Roth
 *CREATION DATE: 01/05/01
 *LAST MODIFIED: 01/05/01
 *PURPOSE:
 *  purges timed out games
 *
 *ARGUMENTS:
 *  value - The currentGames structure
 *********************************************************/
void currentGamesPurge(currentGames *value) {
	currentGames q, prev; /* Temp pointers */
	time_t currentTime;  /* Current time */
	int timeOut;         /* Min purge time different */

	/* this method goes through all the games -- it should
	 * have it's own lock on the currentGames structure */
	currentGamesLock();
	
	q = *value;
	prev = q;
	currentTime = time(NULL);
	while (NonEmpty(q)) {
		/* Check for expired games */
		/* printf("GAMES PURGE: Version of this game is %s\n", q->version); */
		if (q->version[0] == '1' && q->version[1] == '.' && q->version[2] == '0' && q->version[3] <= '8') {
			timeOut = CURRENT_GAME_TRACKER_EXPIRE_OLD;
		} else {
			timeOut = CURRENT_GAME_TRACKER_EXPIRE_NEW;
		}
		if (currentTime - q->lastpacket > timeOut) {
			/* Expire */
			statsSaveGameEnd(q->gameLogOffset, q);
			if (q == *value) {
				fprintf(stderr, "GAMES PURGE: removing first \n");
				/* Removing first item */
				*value = q->next;
				prev = q->next;
				Dispose(q);
				q = prev;
			} else {
				fprintf(stderr, "GAMES PURGE: removing other \n");
				/* Removing other entries */
				prev->next = q->next;
				Dispose(q);
				q = prev->next;
			}
			fprintf(stderr, "GAMES PURGE: done Dispose\n");
		} else {
			prev = q;
			q = CurrentTail(q); 
		}
	}
	
	currentGamesUnlock();

	fprintf(stderr, "GAMES PURGE: finished purging\n");
}




