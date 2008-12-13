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
#include <math.h>
#include "global.h"
#include "screen.h"
#include "base64.h"
#include "cgi-lib.h"
#include "html-lib.h"
#include "string-lib.h"


#define BASE_URL http://www.website.com/url

void mainWriteFile(char *fileName) {
  FILE *fp;
  char c;
  fp = fopen(fileName, "r");
  if (fp) {
    c = fgetc(fp);
    while (!feof(fp)) {
      fputc(c, stdout);
      c = fgetc(fp);
    }
    fclose(fp);
  }
}  


BYTE getSize(char *str) {
  BYTE val;
  if (str == NULL) {
    return 0;
  }
  val = atoi(str);
  if (val > 4) {
    val = 0;
  }

  return val;
}

llist entries;

/* Loads in the error page, cleansup and quits */
void mainPrintError(char *errmsg) {
  html_header();
  screenDestroy();
  mainWriteFile(ERROR_HEAD);
  printf(errmsg);
  mainWriteFile(ERROR_TAIL);
  list_clear(&entries);
  exit(0);
}

void mainPrintToolButton(char *value, char *name, char *imagePath, char *caption, bool checked) {
  if (imagePath != NULL) {
    if (checked == TRUE) {
      printf("<p><input type=\"radio\" name=\"%s\" value=\"%s\" checked><img src=\"%s\"> %s</p>", name, value, imagePath, caption);
    } else {
      printf("<p><input type=\"radio\" name=\"%s\" value=\"%s\"><img src=\"%s\"> %s</p>", name, value, imagePath, caption);
    }
  } else {
     if (checked == TRUE) {
      printf("<p><input type=\"radio\" name=\"%s\" value=\"%s\" checked> %s</p>", name, value, caption);
    } else {
      printf("<p><input type=\"radio\" name=\"%s\" value=\"%s\"> %s</p>", name, value, caption);
    }
  } 
}


void mainPrintTools(BYTE sel) {
  mainPrintToolButton("Building", "TERRAIN", "BASE_URL/images/m50.gif", "Building", (sel == BUILDING ? 1 : 0));
  mainPrintToolButton("River", "TERRAIN", "BASE_URL/images/m101.gif", "River", (sel == RIVER ? 1 : 0));
  mainPrintToolButton("Crater", "TERRAIN", "BASE_URL/images/m3.gif", "Crater", (sel == CRATER ? 1 : 0));
  mainPrintToolButton("Swamp", "TERRAIN", "BASE_URL/images/m2.gif", "Swamp", (sel == SWAMP ? 1 : 0));
  mainPrintToolButton("Road", "TERRAIN", "BASE_URL/images/m28.gif", "Road", (sel == ROAD ? 1 : 0));
  mainPrintToolButton("Forest", "TERRAIN", "BASE_URL/images/m5.gif", "Forest", (sel == FOREST ? 1 : 0));
  mainPrintToolButton("Rubble", "TERRAIN", "BASE_URL/images/m6.gif", "Rubble", (sel == RUBBLE ? 1 : 0));
  mainPrintToolButton("Grass", "TERRAIN", "BASE_URL/images/m7.gif", "Grass", (sel == GRASS ? 1 : 0));
  mainPrintToolButton("Shot Building", "TERRAIN", "BASE_URL/images/m8.gif", "Shot Building", (sel == SHOT_BUILDING ? 1 : 0));
  mainPrintToolButton("Boat", "TERRAIN", "BASE_URL/images/m9.gif", "Boat", (sel == BOAT ? 1 : 0));
  mainPrintToolButton("Deep Sea", "TERRAIN", "BASE_URL/images/m113.gif", "Deep Sea", (sel == DEEP_SEA ? 1 : 0));
  mainPrintToolButton("River", "TERRAIN", "BASE_URL/images/m101.gif", "River", (sel == RIVER ? 1 : 0));
  /* Print greyed out mine */
  printf("<input disabled type=\"checkbox\" name=\"MINE\" value=\"Mine\">Mine</p>");

  mainPrintToolButton("Base", "TERRAIN", "BASE_URL/images/m49.gif", "Base", (sel == BASE_GOOD ? 1 : 0));
  mainPrintToolButton("Pillbox", "TERRAIN", "BASE_URL/images/m48.gif", "Pillbox", (sel == PILL_EVIL ? 1 : 0));
  mainPrintToolButton("Start", "TERRAIN", "BASE_URL/images/m122.gif", "Start", (sel == START ? 1 : 0));
  /* Print rest */
  printf("</td><td align=RIGHT width=\"82%\"><table width=\"100%\" border=\"0\" cellspacing=\"0\"><tr><td>&nbsp;</td><td><div align=\"center\"><input type=\"submit\" name=\"MOVE\" value=\"Up\"></div></td><td>&nbsp;</td></tr><tr><td><div align=\"center\"><input type=\"submit\" name=\"MOVE\" value=\"Left\"></div></td><td>\n");
}

/*********************************************************
*NAME:          windowUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 31/10/98
*LAST MODIFIED: 31/10/98
*PURPOSE:
*  Updates the Window View
*
*ARGUMENTS:
*  value  - Pointer to the sceen structure
*********************************************************/
void Ex_OnPaint(screen *value, screenMines *mineView) {
  bool done;
  int count; /* Looping Variable */
  BYTE x; /* X and Y co-ordinates */
  BYTE y;
  BYTE pos; /* Current position */
  BYTE xPos;
  BYTE yPos;

  xPos = screenGetOffsetX();
  yPos = screenGetOffsetY();
  printf("<CENTER><BR>View Location - (%d, %d)<P><table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">", xPos, yPos);
  count = 0;
  x = 0;
  y = 0;
  done = FALSE;
    printf("<tr>");
  while (done == FALSE) {

    pos = screenGetPos(value,x,y);
    printf("<TD><input type=\"image\" name=\"C%x%x\" src=\"%sm%d.gif\"></TD>", (xPos+x), (yPos+y), TILES, pos); 
//    printf("<TD><IMG SRC=%sm%d.gif><BR></TD>", TILES, pos);
    /* Mine check */

    x++;
    if (x == SCREEN_SIZE_X) {
      y++;
      printf("</tr>");

      x = 0;
      if (y == SCREEN_SIZE_Y) {
        done = TRUE;
      } else {
        printf("<tr>");
      }
    }
  }
  printf("</table></CENTER>");
  
}


void mainLoadEverard() {
  BYTE map[1965]; /* Size of emap */
  FILE *fp = fopen(E_MAP_PATH, "rb");
  fread(map, 1965, 1, fp);
  fclose(fp);
  screenLoadMap(map, 1965);
}

void mainPrintSizeTable() {
  BYTE val;
  printf("<table border=1 cellspacing=0 width=\"95%\"><tbody><tr><td height=45><table border=0 cellpadding=1 cellspacing=10 width=\"100%\"><tbody><tr><td colspan=3><p><font color=\"#ffff0\" face=\"Verdana, Arial, Helvetica, sans-serif\"><b>Block Size</b></font></p></td></tr><tr><td>");
  
  val = getSize(cgi_val(entries, "BLOCK"));
  mainPrintToolButton("0", "BLOCK", NULL, "1x1", (val == 0 ? 1 : 0));
  printf("</td><td>");
  mainPrintToolButton("1", "BLOCK", NULL, "2x2", (val == 1 ? 1 : 0));
  printf("</td><td>");
  mainPrintToolButton("2", "BLOCK", NULL, "4x4", (val == 2 ? 1 : 0)); 
  printf("</td><td>");
  mainPrintToolButton("3", "BLOCK", NULL, "8x8", (val == 3 ? 1 : 0));
  printf("</td><td>");
  mainPrintToolButton("4", "BLOCK", NULL, "16x16", (val == 4 ? 1 : 0));
  printf("</td></tr></tbody></table></tr></tbody></table><p>&nbsp;</p>");
}


void mainPrintInfoTable() {
  printf("<table border=1 cellspacing=0 width=\"95%\"><tbody><tr><td height=45><table border=0 cellpadding=1 cellspacing=10 width=\"100%\"><tbody><tr><td colspan=3><p><font color=\"#ffff0\" face=\"Verdana, Arial, Helvetica, sans-serif\"><b>Map Information</b></font></p></td></tr><tr> <td><div align=center><p> Number of Pillboxes: %d</p></div></td><td><div align=center><p>Number of Bases: %d</p></div></td><td><div align=center><p>Number of Starts: %d</p></div></td></tr></tbody></table></tr></tbody></table>", screenGetNumPills(), screenGetNumBases(), screenGetNumStarts());
}

void mainCloseTable() {
  printf("</td><td><div align=\"center\"><input type=\"submit\" name=\"MOVE\" value=\"Right\"></div></td></tr><tr><td>&nbsp;</td><td><BR><div align=\"center\"><input type=\"submit\" name=\"MOVE\" value=\"Down\"><P></div></td><td>&nbsp;</td></tr></table></td></tr></table></td></tr></table></td></tr></table><p>&nbsp;</p>");
}

void mainLoadMap(char *ptr) {
  long uuLen;
  long mapLen;
  BYTE *memMap;

  uuLen =strlen(ptr);
  memMap = malloc(64 * 1024);
  mapLen = base64_decode(ptr, memMap, (64 * 1024));
  if (mapLen == -1 || screenLoadMap(memMap, mapLen) == FALSE) {
    mainPrintError("Map Load Failed");
  }
  free(memMap);
}


BYTE getSelectedTerrain(char *terrain) {
  if (terrain == NULL) {
    return DEEP_SEA;
  }
  if (strcmp(terrain, "Deep Sea") == 0) {
    return  DEEP_SEA;
  } else if (strcmp(terrain, "Boat") == 0) {
    return BOAT;
  } else if (strcmp(terrain, "Shot Building") == 0) {
    return SHOT_BUILDING;
  } else if (strcmp(terrain, "Grass") == 0) {
    return GRASS;
  } else if (strcmp(terrain, "Rubble") == 0) {
    return RUBBLE;
  } else if (strcmp(terrain, "Forest") == 0) {
    return FOREST;
  } else if (strcmp(terrain, "Road") == 0) {
    return ROAD;
  } else if (strcmp(terrain, "Swamp") == 0) {
    return SWAMP;
  } else if (strcmp(terrain, "Crater") == 0) {
    return CRATER;
  } else if (strcmp(terrain, "River") == 0) {
    return RIVER;
  } else if (strcmp(terrain, "Building") == 0) {
    return BUILDING;
  } else if (strcmp(terrain, "Base") == 0) {
    return BASE_GOOD;
  } else if (strcmp(terrain, "Pillbox") == 0) {
    return PILL_EVIL;
  } else if (strcmp(terrain, "Start") == 0) {
    return START;
  } else {
    return DEEP_SEA;
  }
} 


  

void processSquare(long a, long b, char *terrain, char *mines) {
  BYTE x, y;
  BYTE mined = 0;
  BYTE t;
  BYTE count1, count2;
  BYTE maxx, maxy;
  if (mines != NULL) {
    mined = MINE_SUBTRACT;
  }
  x = (BYTE) a;
  y = (BYTE) b;
  t = getSelectedTerrain(terrain);
  
   
  if (t == START) {
    if (screenSetStart(x, y) == FALSE) {
      mainPrintError("There are too many starts. Please remove one first\n");
    }
    return;
  }
  if (t == PILL_EVIL) {
    if (screenSetPill(x, y) == FALSE) {
      mainPrintError("There are too many pillboxes. Please remove one first\n");
    }
    return;
  }
  if (t == BASE_GOOD) {
    if (screenSetBase(x, y) == FALSE) {
      mainPrintError("There are too many bases. Please remove one first\n");
    }
     return;
  }

  maxx = getSize(cgi_val(entries, "BLOCK"));
  maxx = (BYTE) pow(2, maxx);
  maxy = maxx;
  if (x + maxx > 255) {
    maxx = 255;
  } else {
    maxx = x +maxx;
  }

  if (y +maxy > 255) {
    maxy = 255;
  } else {
    maxy = y +maxy;
  }

  
  for (count1 = x;count1<maxx;count1++) {
    for (count2 =y;count2<maxy;count2++) {
      if (t >= CRATER && t <= 7) {
        screenSetPos(count1, count2, t+mined);
      } else {
        screenSetPos(count1, count2, t);
      }
    }
  }
}
 
void process() {
  char *ptr;
  node *e;

  /* Set location */
  ptr = cgi_val(entries, "X");
  if (ptr != NULL && cgi_val(entries, "NEW") == NULL) {
    screenSetOffset((BYTE) (atoi(ptr)), screenGetOffsetY());
  }
  ptr = cgi_val(entries, "Y");
  if (ptr != NULL && cgi_val(entries, "NEW") == NULL) {
    screenSetOffset(screenGetOffsetX(), (BYTE) (atoi(ptr)));
  }

  if (cgi_val(entries, "GO") != NULL) {
    /* Set X location */
    ptr = cgi_val(entries, "WANT_X");
    if (ptr != NULL) {
      screenSetOffset((BYTE) (atoi(ptr)), screenGetOffsetY());
    } /* Set Y location */
    ptr = cgi_val(entries, "WANT_Y");
    if (ptr != NULL) {
      screenSetOffset(screenGetOffsetX(), (BYTE) (atoi(ptr)));
    }
  }		  
  /* Check squres */
  e = entries.head;
  while (e != NULL) {
    if ((*e).entry.name[0] == 'C') {
      char str[256];
      unsigned int x;
      unsigned int y;
      strncpy(str, (*e).entry.name, 255);
      str[255] = '\0';
      sscanf(str+3, "%x", &y);
      str[3] = '\0';
      sscanf(str+1, "%x", &x);
      processSquare(x, y, cgi_val(entries, "TERRAIN"), cgi_val(entries, "MINES"));
      return;
    }
    e = e->next;
  }

  /* Move */
  ptr = cgi_val(entries, "MOVE");
  if (ptr != NULL) {
    if (strcmp(ptr, "Up") == 0) {
      screenSetOffset(screenGetOffsetX(), (BYTE) (screenGetOffsetY() - 16));
    } else if (strcmp(ptr, "Down") == 0) {
      screenSetOffset(screenGetOffsetX(), (BYTE) (screenGetOffsetY() + 16));
    } else if (strcmp(ptr, "Left") == 0) {
      screenSetOffset((BYTE) (screenGetOffsetX() - 16), screenGetOffsetY());
    } else if (strcmp(ptr, "Right") == 0) {
      screenSetOffset((BYTE) (screenGetOffsetX() + 16), screenGetOffsetY());
    }
  }

}


unsigned char *mapUp;
long mapUpLen;


int main(int argc, char *argv[]) {
  char *ptr;
  BYTE *memMap;
  BYTE *memUU;
  long mapLen;
  long uuLen; 
  /* parse the form data and add it to the list */

  mapLen = 0;
  memMap = malloc(64 * 1024);
  mapUpLen = 0;
  mapUp = memMap;
//  pragma("No-cache");
  if (read_cgi_input(&entries) == -1) {
    mainPrintError("Error reading in values");
  }

  screenSetup();
  if (mapUpLen != 0) {
    FILE *fp;
	/* FIXME: Harcoded upload file log list path. */
    fp = fopen("/usr/lib/cgi-bin/Members/up.txt", "a");
    fprintf(fp, "opened %d\n", mapUpLen);
    if (mapUpLen == -1) {
      fprintf(fp, "error uploading\n");
      fclose(fp);
      mainPrintError("Error Uplaoding Map\n");
    }
    if (screenLoadMap(memMap, mapUpLen) == FALSE) {
      fprintf(fp, "error processing\n");
      fclose(fp);
      mainPrintError("Error Processing Map");
    }
      fclose(fp);
  }
  
  if (cgi_val(entries, "LOAD") != NULL) {
    show_html_page(UPLOAD_FILE);
    list_clear(&entries);
    exit(0);
  }

  
  ptr = cgi_val(entries, "MAP");
  if (ptr != NULL && cgi_val(entries, "NEW") == NULL  && cgi_val(entries, "LOAD_E") == NULL && mapLen == 0) {
    mainLoadMap(ptr);
  } else if (mapLen != 0) {
    if (screenLoadMap(memMap, mapLen) == FALSE) {
      mainPrintError("Error Processing Map");
    }
  } else if (cgi_val(entries, "NEW") == NULL && mapUpLen == 0) {
    mainLoadEverard();
  }
	  

  if (cgi_val(entries, "SAVE") != NULL) {
    /* Save the map */
    printf("Content-type: application/octet-stream\n");
    printf("Content-Disposition: filename=\"Wome.map\"\n\n");
    screenWriteMap(memMap, &mapLen);
    fwrite(memMap, mapLen, 1, stdout);
    free(memMap);
    list_clear(&entries);  
    screenDestroy();
    exit(0);
  }
  
  //html_header();           /* print HTML MIME header */
  //print_entries(entries);
  process(&entries);
  html_header();
  mainWriteFile(FILE_HEAD); 
  mainPrintTools(getSelectedTerrain(cgi_val(entries, "TERRAIN")));
  screenUpdate(redraw);
  memUU = malloc(64 * 1024);
  screenWriteMap(memMap, &mapLen);
  uuLen = base64_encode(memMap, mapLen, memUU,(64 * 1024));
  printf("<input type=\"hidden\" name=\"MAP\" value=\"");
  fwrite(memUU, uuLen, 1, stdout);
  printf("\">");
  printf("<input type=\"hidden\" name=\"X\" value=\"%d\"><input type=\"hidden\" name=\"Y\" value=\"%d\">", screenGetOffsetX(), screenGetOffsetY());
  free(memMap);
  free(memUU);
  mainCloseTable();
  mainPrintSizeTable();
  mainPrintInfoTable();
  mainWriteFile(FILE_TAIL);
  screenDestroy();
  list_clear(&entries);
  
}

