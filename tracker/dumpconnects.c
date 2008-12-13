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

/* This code is not part of the tracker today. Utility code to parse the stats database generated. Andrew would have more details on this. */



#include <string.h>
#include <stdio.h>
#include "stats.h"

char connToStr[10][30];

int main(int argc, char *arg[]) {
	FILE *fp;
	unsigned long int fSize, nP;
	IPconnectStruct connStat;
	char fileName[100];

	strcpy(connToStr[STATS_LOG_NEW_GAME], "New Game");
        strcpy(connToStr[STATS_LOG_GAME_UPDATE], "Game Update");
        strcpy(connToStr[STATS_LOG_TELNET_LIST], "Telnet List");

	if (argc == 1) {
                strcpy(fileName, "connections.log");
		printf("Using %s\n", fileName);
	} else
		strcpy(fileName, arg[1]);
	fp = fopen(fileName, "rb");
	if (fp == NULL) {
		printf("Error opening %s\n", fileName);
		return;
	}
	fseek(fp, 0, SEEK_END);
	fSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
printf("sdfs\n");
	nP = 0;
	while (!feof(fp) && ftell(fp) < fSize) { /* loop through file */
		fread(&connStat, sizeof(connStat), 1, fp);
		printf("%d.%d.%d.%d\n", connStat.ip[0],connStat.ip[1],connStat.ip[2],connStat.ip[3]);
		printf("%s\n", connStat.ipDNS);
		printf("%s", asctime(gmtime(&connStat.date)));
		printf("%s\n", connToStr[connStat.stat]);
                printf("========================================\n");
		nP++;
        }
	printf("%ld entries\n", nP);
}
