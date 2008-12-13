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
*Name:          stats
*Filename:      stats.c
*Author:        Andrew Roth
*Creation Date: 00/11/06
*LAST MODIFIED: 00/11/06
*Purpose:
*  Responsable for saving stats to file, and generating
*  a stats summary (which can use html tags)
*  This is currently not in use - JM
*********************************************************/

#define TELNET_GAME_LIST 0
#define WEB_GAME_LIST 1

#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
/* Windows winsock */
#include <winsock2.h>
#include <windows.h>
#include <memory.h>
#define	chdir(X)	_chdir(X)

#else
/* Liunux Winsock plus some useful macros */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define closesocket(X) close(X)
typedef struct hostent  HOSTENT;
#define SD_BOTH 2

#endif
#include "stats.h"
#include "util.h"
#include "currentgames.h"
#include "global.h"

short int end = FALSE;

/*typedef struct in_addr *pack;
*/
/*********************************************************
*NAME:          statsLogConnection
*AUTHOR:        Andrew Roth
*CREATION DATE: 00/11/06
*LAST MODIFIED: 01/04/29
*PURPOSE:
* Logs a connection.  Files consist of a *.* for example
* home.com
*
*ARGUMENTS:
*  
*********************************************************/
void statsLogConnection(int connectionType, BYTE ip[IP_SIZE]) {
	struct sockaddr_in address;
	char ipStr[20], ipRes[STAT_DNS_SIZE], fileName[STAT_LOG_FILE_SIZE];
	char tmp[STAT_LOG_FILE_SIZE];
	int nDot, gotOne, foundIP, i;
	unsigned int e/*, seekDest*/;

	FILE *fp;
	IPstatsStruct connStat, connStatTmp;
	IPconnectStruct oneConnStat;
	long int fSize;
return;

	sprintf(ipStr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	address.sin_addr.s_addr = inet_addr(ipStr);
	printf("Stats: Log connection %d from %d.%d.%d.%d\n", connectionType, ip[0], ip[1], ip[2], ip[3]);
	
	/* if IP resolved */
	if (utilReverseLookup(&(address.sin_addr), ipRes) && strcmp(ipRes, ipStr) != 0) {
/*		printf("Converted to %s\n", ipRes);*/
		strcpy(connStat.ipDNS, ipRes);
		nDot = 0;
		for (e = 0; e < strlen(ipRes); e++)
			if (ipRes[e] == '.')
				nDot++;
		if (nDot == 1)         /* if there is one dot in dns */
			strcpy(fileName, ipRes); /* filename is dns */
		else {                 /* if there is more than one dot */
			/* get second dot from the right */
			gotOne = FALSE;
			for (i = strlen(ipRes)-1; i >= 0; i--) {
				if (ipRes[i] == '.' && !gotOne)
					gotOne = TRUE;
				else if (ipRes[i] == '.' && gotOne)
					break;
			}
			/* filename is everything right of it */
			strcpy(fileName, &ipRes[i+1]);
		}
		printf("\t\tFilename %s\n", fileName);
	} else {
		strcpy(fileName, "unresolved");  /* filename is unresolved */
		sprintf(connStat.ipDNS, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	}
	memcpy(connStat.ip, ip, IP_SIZE);

	/* Log the connection to a "connections.log" file which
	will have ALL the connections */
	fp = fopen("connections.log", "ab+");
	if (fp == NULL) {
		printf("Error opening connections.log\n");
		return;
	}
	memset(&oneConnStat, 0, sizeof(oneConnStat));
	memcpy(&oneConnStat, &connStat, sizeof(oneConnStat));
	oneConnStat.date = time(NULL);
	oneConnStat.stat = connectionType;
	fwrite(&oneConnStat, sizeof(oneConnStat), 1, fp);
	printf("\t\tByte size: %d (should be 4)\n", sizeof(oneConnStat.stat));
	fclose(fp);
	/* done logging to connections.log */

	if (chdir("stats") == -1) {
		printf("\t\tError going to stats dir\n");
		return;
	}
	sprintf(tmp, "%s", fileName);
	strcpy(fileName, tmp);
	printf("\t\tUsing %s\n", fileName);

	fp = fopen(fileName, "ab+");
	if (fp == NULL) {
		printf("\t\tError opening file!\n");
		chdir("..");  fclose(fp);
		return;
	}
	fseek(fp, 0, SEEK_END);
	if (ftell(fp) != 0) { 	/* if filename exists */
		fSize = ftell(fp);
		printf("\t\tExists\n");
		fclose(fp);  fp = fopen(fileName, "rb+");
		fseek(fp, 0, SEEK_SET);
		
		memcpy(&connStatTmp, &connStat, sizeof(connStat));
		
		foundIP = FALSE;
		while (!feof(fp) && ftell(fp) < fSize && !foundIP) { /* loop through file */
			fread(&connStat, sizeof(connStat), 1, fp);
                        /* if currentIP ip is IP */
/*			printf("Comparing %d.%d.%d.%d & %d.%d.%d.%d\n", connStat.ip[0], connStat.ip[1],connStat.ip[2],connStat.ip[3], ip[0],ip[1],ip[2],ip[3]);*/
			if (memcmp(connStat.ip, ip, IP_SIZE) == 0) {
				printf("\t\tFound ip in file\n");
				/* increment the right field (connectionType) */
/*				printf("\t\tLoaded stat as: %ld\n", connStat.stats[connectionType]);*/
				connStat.stats[connectionType]++;
				printf("\t\tBefore, at byte %ld, size %d .. ", ftell(fp), sizeof(connStat));
				
/*				seekDest = ftell(fp)-sizeof(connStat);
				fclose(fp);  fp = fopen(fileName, "rwb");
				fseek(fp, seekDest, SEEK_SET);*/
				fseek(fp, ftell(fp)-sizeof(connStat), SEEK_SET);

				printf("At byte %ld\n", ftell(fp));
				/* save the currentIP */
				printf("\t\tBefore writing stat index %d is: %ld\n", connectionType, connStat.stats[connectionType]);
				fwrite(&connStat, sizeof(connStat), 1, fp);

				/* del me */
/*				fclose(fp);
				fp = fopen(fileName, "rb+");
				fseek(fp, ftell(fp)-sizeof(connStat), SEEK_SET);
				fread(&connStat, sizeof(connStat), 1, fp);
				printf("\t\tMy test gives: %ld\n", connStat.stats[connectionType]);
*/

				/* exit */
				foundIP = TRUE;
			}
		}

		if (!foundIP) {
			/* make a new new stat IP variable -- DONE already with connStatTmp */
			printf("\t\tIP not found, adding new entry in file\n");
	                memset(connStatTmp.stats, 0, sizeof(connStatTmp.stats));
			/* set the correct connectionType field to 1 */
			connStatTmp.stats[connectionType]++;
			/* save the currentSetting */
			fwrite(&connStatTmp, sizeof(connStatTmp), 1, fp);
		}
	} else {
/*		fclose(fp);
		fp = fopen(fileName, "wb");*/
		if (fp == NULL) {
			printf("\t\tError opening file!\n");
			chdir("..");  fclose(fp);
        	        return;
	        }
		printf("\t\tNew File\n");
		/* make a new file named filename */
		/* make a new newIP (variable -- save type as currentIP) */
		memset(connStat.stats, 0, sizeof(connStat.stats));
		/* set the correct connectionType field to 1 */
                connStat.stats[connectionType]++;
		/* save the currentSetting */
		fwrite(&connStat, sizeof(connStat), 1, fp);
		printf("\t\tSize %d\n", sizeof(connStat));
	}
	fclose(fp);
	chdir("..");
}

/*********************************************************
*NAME:          statsSaveGameStart
*AUTHOR:        Andrew Roth
*CREATION DATE: 00/11/06
*LAST MODIFIED: 00/11/06
*PURPOSE:
*  Saves the game info at "value" to the games.log, and returns
*  the file record index where it saved it
*
*ARGUMENTS:
*  gameIndex - The game's index to save
*********************************************************/
unsigned long statsSaveGameStart(currentGames value) {
	statsGameSave statsSave;
	FILE *fp;
	unsigned long int ret;
return 0;
	printf("Stats: Save game start at value %p  Size %d\n", value, sizeof(statsSave));	

	/* make a new currentGameStats -- Done above */
	/* convert currentGame to the statsGameInfo */
	statsSave.date = value->lastpacket;
	strcpy(statsSave.ipdns, value->address);
	memcpy(statsSave.ip, & (value->ip) , IP_SIZE);
/*	printf("\t\t%d.%d.%d.%d\n", statsSave.ip[0], statsSave.ip[1],statsSave.ip[2],statsSave.ip[3]);
	printf("\t\tOrig %d.%d.%d.%d\n", value->ip[0], value->ip[1], value->ip[2], value->ip[3]);*/
	strcpy(statsSave.det.mapName, value->mapName);
	strncpy(statsSave.det.version, value->version, 8);
	statsSave.det.port = value->port;
	statsSave.det.numPlayers = value->numPlayers;
        statsSave.det.numBases = value->numBases;
        statsSave.det.numPills = value->numPills;
	statsSave.det.mines = value->mines;
	statsSave.det.game = value->game;
	statsSave.det.ai = value->ai;
	statsSave.det.password = value->password;
	statsSave.det.starttime = value->starttime;
	statsSave.det.startdelay = value->startdelay;
/*  printf("\t\tstartdelay %ld\n", value->startdelay);*/
	statsSave.det.timelimit = value->timelimit;
/*  printf("\t\ttimelimit %ld\n", value->timelimit);*/
/*	printf("Map %s, %s\n", value->mapName, statsSave.det.version);*/

	memset(&statsSave.end, 0, sizeof(statsSave.end));
	statsSave.end.minPills = value->numPills;
	statsSave.end.minBases = value->numBases;

/*	printf("Size: %d\n", sizeof(statsSave));*/
	/* open games.log as random */
	fp = fopen("games.log","ab+");
	/* seek to end of file since win32 doesn't do it */
	fseek(fp, 0, SEEK_END);

        /* get the next free index -- Done when opening with 'a' */
        /* write the currentGameStats info to it */
	if (fp != NULL) {
		ret = ftell(fp);
		printf("\t\tRandom record file position: %ld\n", ret);
		memcpy(statsSave.newLine, "\r\n", 2);
		fwrite(&statsSave, sizeof(statsSave), 1, fp);
		fclose(fp);
	} else {
		printf("\t\tError opening games.log\n");
		return -1;
	}

	/* update it to get the 'at end' stats in case this is only 
	packet we get */
	statsSaveGame(ret, value);

	/* return the random record index */
	return ret;
}


/*********************************************************
*NAME:          statsSaveGame
*AUTHOR:        Andrew Roth
*CREATION DATE: 00/11/06
*LAST MODIFIED: 00/11/06
*PURPOSE:
*  Saves game info at "value" to the games.log starting at byte
*  origFileIndex
*
*ARGUMENTS:
*  currentGames - The game info address to save
*********************************************************/
void statsSaveGame(unsigned long origFileIndex, currentGames value) {
	statsGameSave statsSave;
	FILE *fp;
return;
	printf("Stats: Update game at %p fileIndex %ld\n", value, origFileIndex);

	/* open the games.log file as random */
	fp = fopen("games.log", "rb+");
	if (fp == NULL) {
		printf("\t\tError opening games.log\n");
		return;
	}
	/* go to the index saved when statsSaveGameStart was called */
	if (fseek(fp, origFileIndex, SEEK_SET) != 0) {
		printf("\t\tError seeking to offset %ld\n", origFileIndex);
		return;
	}

	/* update this entry */
	fread(&statsSave, sizeof(statsSave), 1, fp);
	statsSave.end.numPlayers = value->numPlayers;
	if (value->numPlayers > statsSave.end.maxPlayers)
		statsSave.end.maxPlayers = value->numPlayers;
        statsSave.end.freePills = value->numPills;
        statsSave.end.freeBases = value->numBases;
	if (value->numPills < statsSave.end.minPills)
		statsSave.end.minPills = value->numPills;
        if (value->numBases < statsSave.end.minBases)
                statsSave.end.minBases = value->numBases;
        if (fseek(fp, origFileIndex, SEEK_SET) != 0) {
                printf("\t\tError seeking back to offset %ld\n", origFileIndex);
                return;
        }
	statsSave.end.date = time(NULL);
/*	printf("Set to %s\n", asctime(gmtime(&statsSave.end.date)));*/
	if (end) {
	        statsSave.end.timedOut = TRUE;
			printf("\t\tGame has timed out\n");
	}

/*	printf("\t\tAt %ld\n", ftell(fp)); */

	fwrite(&statsSave, sizeof(statsSave), 1, fp);
        fclose(fp);
}

/*********************************************************
*NAME:          statsSaveGame
*AUTHOR:        Andrew Roth
*CREATION DATE: 01/04/15
*LAST MODIFIED: 01/04/15
*PURPOSE:
*  Saves ending info for the game at "value" to offset offset
*
*ARGUMENTS:
*  value - The game info address to save
*********************************************************/
void statsSaveGameEnd(unsigned long offset, currentGames value) {
	printf("Stats: Game end\n");
	end = TRUE;
	statsSaveGame(offset, value);
	end = FALSE;
return;
	printf("\t\tSTATS: Finished saving end of game\n");
}

