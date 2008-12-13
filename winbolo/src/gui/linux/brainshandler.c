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
*Name:          brainsHandler
*Filename:      brainsHandler.c
*Author:        John Morrison
*Creation Date: 23/11/99
*Last Modified: 29/4/00
*Purpose:
*  Handles front end brain operations. Menu setting etc.
*
*NOTE: This file must be compiled with optimisations 
*      disabled under Visual Studio (VC++) 5 with any
*      or none of the service packs installed due to a
*      a compiler bug causing crashes whenever a brain
*      is loaded/unloaded
*********************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <gmodule.h>
#include <gtk/gtk.h>
#include "../../bolo/global.h"
#include "../../bolo/brain.h"
#include "../../bolo/backend.h"
#include "../linresource.h"
#include "../clientmutex.h"
#include "../lang.h"
#include "messagebox.h"

typedef  GModule* HINSTANCE;
typedef GtkWidget* HWND;
typedef unsigned int UNIT;

#define EMPTY_STRING '\\'
#define BRAINMAIN_PROC "BrainMain"
#define MAX_PATH 1024 

#define SHARED_LIB_EXT ".so"
#define BRAINS_DIR_STRING "brains"

#define FreeLibrary(X) g_module_close(X)
#define LoadLibrary(X) g_module_open(X, 0)
#define GetProcAddress(X, Y) g_module_symbol(X, Y, (gpointer *) &brainsProc)


char brainsDir[MAX_PATH]; /* The Brains Directory */
char brainsLocalDir[MAX_PATH];
int brainsNum; /* Number of brains */
bool brainsRunning; /* Is a brain running */
HINSTANCE brainsInst; /* Brains Instance */
bool bIsFirst;        /* Is this the first time a brain will be called */
bool brainsProcExecuting = FALSE; /* Is the brain proc in execution */
BrainInfo bInfo; /* The Brain Info */
bool brainsLoadedOnce = FALSE; /* We only want this to be displayed once */
short (*brainsProc)(BrainInfo*);

G_MODULE_IMPORT short BrainMain(BrainInfo *bi); 

extern gchar *applicationPath;
extern GtkWidget *brains1_menu;
extern GtkWidget *menu_bar;
extern GtkWidget *manual1;
extern GtkWidget *window;
extern GSList *brainsGroup;
extern bool isInMenu;
GSList *brainsLoadedList = NULL;

void brainsHandlerManual(HWND hWnd); 
bool brainsHandlerStart(HWND hWnd, char *str, char *name);



void brainsHandlerSelect(GtkWidget *widget, gpointer user_data) {
  char str[MAX_PATH];  /* Path to the brain to run */
  int ret;             /* Function return value    */
  FILE *fp;
  gchar *menuStr;

  if (GTK_CHECK_MENU_ITEM(widget)->active == TRUE && isInMenu == FALSE) {
    isInMenu = TRUE;
    gtk_label_get(GTK_LABEL(GTK_BIN(widget)->child), &menuStr); 
    
    /* First shut down the last brain if it is running */
    if (brainsRunning == TRUE) {
      brainsHandlerManual(NULL);
    }
 
    /* Get the brains name */
    strcpy(str, brainsLocalDir);
    strcat(str, menuStr);
    strcat(str, SHARED_LIB_EXT);
    fp = fopen(str, "r");
    if (fp) {
      /* In local directory */
      fclose(fp);
    } else {
      /* Change to global */
     strcpy(str, brainsDir);
     strcat(str, "/");
     strcat(str, menuStr);
     strcat(str, SHARED_LIB_EXT);
   }
   
    /* Try starting the new brain */
    brainsRunning = brainsHandlerStart(NULL, str, menuStr);

    if (brainsRunning == TRUE) {
      bIsFirst = TRUE;
    } else {
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(manual1), TRUE);
    }
    isInMenu = FALSE;
  }

//  if (id != brainsRunningId || brainsRunning == FALSE) {
//  FIXME
 

}


/*********************************************************
*NAME:          brainsHandlerLoadBrainMenuItems
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 24/11/99
*PURPOSE:
*  Loads the brain file name into the menu.
*  Returns success
*
*ARGUMENTS:
*  brainsList - The list of brains to add on to.
*********************************************************/
bool brainsHandlerLoadBrainMenuItems(GtkWidget *menu) {
  bool returnValue;         /* Value to return */
  char checkPath[MAX_PATH]; /* Path to check */
  DIR *dir;
  struct dirent *ent;
  struct stat statBuff;
  gchar *fileNamePath;
  gchar *ext;
  GtkWidget *item;
  char fileName[FILENAME_MAX];
  
  returnValue = TRUE;
  strcpy(checkPath, brainsLocalDir);
  
 
  if (brainsLoadedList != NULL) {
    g_slist_free(brainsLoadedList);
  }
  brainsLoadedList = NULL;
  dir = opendir(checkPath);
  if (dir) {
    ent =  (struct dirent *) readdir(dir);
    while (ent) {
      fileNamePath = g_strdup_printf("%s/%s", checkPath, ent->d_name);
      if (!stat(fileNamePath, &statBuff) && S_ISREG(statBuff.st_mode) && (ext = strrchr(ent->d_name, '.')) != NULL) {
        if (!strcmp(ext, SHARED_LIB_EXT)) {
          strcpy(fileName, ent->d_name);
          fileName[strlen(fileName)-3] = '\0';
          item = gtk_radio_menu_item_new_with_label(brainsGroup, fileName);
	  brainsGroup = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM(item));
	  gtk_widget_ref(item);
          gtk_object_set_data_full (GTK_OBJECT (window), fileName, item, (GtkDestroyNotify) gtk_widget_unref);
          gtk_widget_show (item);
          gtk_container_add (GTK_CONTAINER (brains1_menu), item);
	  gtk_signal_connect(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(brainsHandlerSelect), 0);
          brainsNum++;
	  brainsLoadedList = g_slist_append(brainsLoadedList, item);
	}
      }
      g_free(fileNamePath);
      ent = (struct dirent *) readdir(dir);
    }
    closedir(dir);
  }
  
  /* Global Path */
  strcpy(checkPath, brainsDir);
  dir = opendir(checkPath);
  if (dir) {
    ent =  (struct dirent *) readdir(dir);
    while (ent) {
      fileNamePath = g_strdup_printf("%s/%s", checkPath, ent->d_name);
      if (!stat(fileNamePath, &statBuff) && S_ISREG(statBuff.st_mode) && (ext = strrchr(ent->d_name, '.')) != NULL) {
        if (!strcmp(ext, SHARED_LIB_EXT)) {
          strcpy(fileName, ent->d_name);
          fileName[strlen(fileName)-3] = '\0';
          item = gtk_radio_menu_item_new_with_label(brainsGroup, fileName);
	  brainsGroup = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM(item));
	  gtk_widget_ref(item);
          gtk_object_set_data_full (GTK_OBJECT (window), fileName, item, (GtkDestroyNotify) gtk_widget_unref);
          gtk_widget_show (item);
          gtk_container_add (GTK_CONTAINER (brains1_menu), item);
	  gtk_signal_connect(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(brainsHandlerSelect), 0);
          brainsNum++;
	  brainsLoadedList = g_slist_append(brainsLoadedList, item);
	}
      }
      g_free(fileNamePath);
      ent = (struct dirent *) readdir(dir);
    }
    closedir(dir);
  }
  return returnValue;
}

bool winUtilWBSubDirExist(char *path) {
  bool returnValue;
  DIR *dir;

  dir = opendir(path);
  returnValue = FALSE;
  if (dir) {
    returnValue = TRUE;
    closedir(dir);
  }

  return returnValue;
}
  


/*********************************************************
*NAME:          brainsHandlerLoadBrains
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 24/12/99
*PURPOSE:
*  Loads the brain entries into the brains menu.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
*  hWnd  - Main Window Handle.
*********************************************************/
bool brainsHandlerLoadBrains(HWND hWnd) {
  bool returnValue; /* Value to return */

  /* Set up the constants in the bInfo structure */
  bInfo.BoloVersion = 0x0108;
  bInfo.InfoVersion = CURRENT_BRAININFO_VERSION;
  bInfo.PrefsVRefNum = 0;
  bInfo.PrefsFileName = NULL;


  strcpy(brainsDir, applicationPath);
  strcat(brainsDir, "/");
  strcat(brainsDir, BRAINS_DIR_STRING);
  strcpy(brainsLocalDir, g_get_home_dir());
  strcat(brainsLocalDir, "/.linbolo/brains/");
  
  brainsNum = 0;
  brainsInst = NULL;
  brainsRunning = FALSE;
  returnValue = TRUE;

  returnValue = g_module_supported();
  if (returnValue == FALSE) {
    MessageBox("Sorry, dynamic loading of modules isn't supported\n on this platform", DIALOG_BOX_TITLE);
  }

  /* Look for the 'Brains' directories */
  if (returnValue == TRUE) {
    if (winUtilWBSubDirExist(brainsDir) == FALSE && winUtilWBSubDirExist(brainsLocalDir) == FALSE && brainsLoadedOnce == FALSE) {
      returnValue = FALSE;
      MessageBox(langGetText(STR_BRAINERR_BRAINDIR), DIALOG_BOX_TITLE);
      brainsLoadedOnce = TRUE;
    }
  }
  /* Load the brains */
  if (returnValue == TRUE) {
    returnValue = brainsHandlerLoadBrainMenuItems(NULL);
  }

  return returnValue;
}


/*********************************************************
*NAME:          brainsHandlerSet
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED:  3/12/99
*PURPOSE:
*  Sets each item in the menu to be disabled.
*
*ARGUMENTS:
*  hWnd    - Handle to the main window
*  enabled - TRUE if we should enable the items else
*            disable them
*********************************************************/
void brainsHandlerSet(HWND hWnd, bool enabled) {
  int    nIndex;
  GSList *node;
  GtkWidget *w;
  for (nIndex = 0; node = g_slist_nth (brainsLoadedList, nIndex); nIndex++) {
    w = (GtkWidget *) node->data;
    gtk_widget_set_sensitive(w, enabled);
  }
}

/*********************************************************
*NAME:          brainsHandlerManual
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 12/12/99
*PURPOSE:
*  The manual brain button has been pressed
*
*ARGUMENTS:
*  hWnd - Window Handle
*********************************************************/
void brainsHandlerManual(HWND hWnd) {

  if (brainsRunning == TRUE) {
    /* Shut that brain down! */
    while(brainsProcExecuting) {} /* Loop until it isn't running */
    /* Prepare brain */
    clientMutexWaitFor();
    brainsProcExecuting = TRUE;
    brainsRunning = FALSE;
    bInfo.operation = BRAIN_CLOSE;
    bInfo.userdata = menu_bar;
    brainsProc(&bInfo);
    brainsProcExecuting = FALSE;
    FreeLibrary(brainsInst);
    brainsInst = NULL;
    clientMutexRelease();
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(manual1), TRUE);
  }
} 

/*********************************************************
*NAME:          brainsHandlerStart
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  Try to launch a new brain. Returns success.
*
*ARGUMENTS:
*  hWnd - Window Handle
*  str  - Filename and path of brain to load
*  name - Name of the brain
*********************************************************/
bool brainsHandlerStart(HWND hWnd, char *str, char *name) {
  bool returnValue; /* Value to return */
  short ret;

  clientMutexWaitFor();
  returnValue = TRUE;
  brainsInst = LoadLibrary(str);
  if (brainsInst == NULL) {
    MessageBox(langGetText(STR_BRAINERR_LAUNCH), DIALOG_BOX_TITLE);
    returnValue = FALSE;
  }

  if (returnValue == TRUE) {
    ret = GetProcAddress(brainsInst, BRAINMAIN_PROC);
    if (ret == FALSE) {
      MessageBox(langGetText(STR_BRAINERR_LAUNCHMAIN), DIALOG_BOX_TITLE);
      returnValue = FALSE;
    }
  }
  /* Try to send the initial launch message */
  if (returnValue == TRUE) {
    screenMakeBrainInfo(&bInfo, TRUE);
    bInfo.userdata = menu_bar;
    while(brainsProcExecuting) {} /* Loop until it isn't running */
    brainsProcExecuting = TRUE;
    bInfo.operation = BRAIN_OPEN;
    ret = brainsProc(&bInfo);
    brainsProcExecuting = FALSE;
    if (ret != 0) {
      /* Shut it down */
      FreeLibrary(brainsInst);
      brainsInst = NULL;
      returnValue = FALSE;
      MessageBox(langGetText(STR_BRAINERR_INIT), DIALOG_BOX_TITLE);
    }
  }
  clientMutexRelease();
  return returnValue; 
}

/*********************************************************
*NAME:          brainsHandlerGetNum
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 23/11/99
*PURPOSE:
*  Returns the number of brains loaded.
*
*ARGUMENTS:
*
*********************************************************/
int brainsHandlerGetNum() {
  return brainsNum;
}

/*********************************************************
*NAME:          brainsHandlerShutdown
*AUTHOR:        John Morrison
*CREATION DATE: 23/11/99
*LAST MODIFIED: 24/11/99
*PURPOSE:
*  Shuts down the brains subsystem.
*
*ARGUMENTS:
*  hWnd - Pointer to the main window
*********************************************************/
void brainsHandlerShutdown(HWND hWnd) {
  if (brainsRunning == TRUE) {
    /* Shut it down */
    brainsHandlerManual(hWnd);
  }
}

/*********************************************************
*NAME:          brainHandlerIsBrainRunning
*AUTHOR:        John Morrison
*CREATION DATE: 25/11/99
*LAST MODIFIED: 25/11/99
*PURPOSE:
*  Returns whether a brain is running or not
*
*ARGUMENTS:
*
*********************************************************/
bool brainHandlerIsBrainRunning() {
  return brainsRunning;
}

/*********************************************************
*NAME:          brainHandlerRun
*AUTHOR:        John Morrison
*CREATION DATE: 26/11/99
*LAST MODIFIED: 26/11/99
*PURPOSE:
*  Called to execute the brains "THINK" call if it is
*  running
*
*ARGUMENTS:
*  hWnd - Main Window handler
*********************************************************/
void brainHandlerRun(HWND hWnd) {
  unsigned short ret; /* Brain return value */
  clientMutexWaitFor();
  if (brainsRunning == TRUE && brainsProcExecuting == FALSE) {
    /* Prepare brain */
    brainsProcExecuting = TRUE;
    if (bIsFirst == TRUE) {
      screenMakeBrainInfo(&bInfo, TRUE);
      bIsFirst = FALSE;
    } else {
      screenMakeBrainInfo(&bInfo, FALSE);
    }
    /* Call brain */

    bInfo.operation = BRAIN_THINK;
    if (brainsRunning == TRUE) {
      ret = brainsProc(&bInfo);
    } else {
      ret = -1;
    }
    if (ret != 0) {
      /* Operation failed */
      brainsHandlerManual(hWnd);
    } else {
      /* Operation succeeded */
      screenExtractBrainInfo(&bInfo);
    }
    brainsProcExecuting = FALSE;
  } else if (brainsRunning == TRUE) {
  }
  clientMutexRelease(); 
}
