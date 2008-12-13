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


#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stats.h"

int main() {
        FILE *fp;
        statsGameSave statsRead;
	long int fSize, count=0;

	fp = fopen("./games.log","rb+");

	if (!fp) {
		printf("Error opening games.log\n");
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	fSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	while (!feof(fp) && ftell(fp) < fSize) {
                fread(&statsRead, sizeof(statsRead), 1, fp);
/*printf("Size %d\n", sizeof(statsRead));*/
		printf("IP: %d.%d.%d.%d\n", statsRead.ip[0], statsRead.ip[1], statsRead.ip[2], statsRead.ip[3]);
		printf("DNS: %s\n", statsRead.ipdns);
		printf("MAP: %s\n", statsRead.det.mapName);
		printf("FIRST PACKET DATE: %s", asctime(gmtime(&statsRead.date)));
		printf("VERSION: %s\n", statsRead.det.version);
		printf("PORT: %d\n", statsRead.det.port);
		printf("BASES: %d\n", statsRead.det.numBases);
                printf("PILLS: %d\n", statsRead.det.numPills);
		printf("MINES: %d\n", statsRead.det.mines);
		printf("AI: %d\n", statsRead.det.ai);
		printf("PASSWORD: %d\n", statsRead.det.password);
		printf("STARTTIME: %s", asctime(gmtime(&(statsRead.det.starttime))));
		printf("STARTDELAY: %ld\n", statsRead.det.startdelay/50);
		printf("TIMELIMT: %ld\n", statsRead.det.timelimit/50);
		printf("NUM PLAYERS AT FIRST UPDATE: %d\n", statsRead.det.numPlayers);
                printf("NUM PLAYERS AT TIMEOUT: %d\n", statsRead.end.numPlayers);
		printf("MAX PLAYERS: %d\n", statsRead.end.maxPlayers);
		printf("BASES AT LAST UPDATE OR TIMEOUT: %d\n", statsRead.end.freeBases);
                printf("PILLS AT LAST UPDATE OR TIMEOUT: %d\n", statsRead.end.freePills);
		printf("MIN BASES: %d\n", statsRead.end.minBases);
                printf("MIN PILLS: %d\n", statsRead.end.minPills);
		printf("LAST UPDATE TIME, OR TIMEOUT TIME: %s", asctime(gmtime(&statsRead.end.date)));
		printf("TIMEDOUT: %d\n", statsRead.end.timedOut);
		printf("========================================\n");
		count++;
	}
	printf("Processed %ld games\n", count);
	return 0;
}

