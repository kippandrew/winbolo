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


/*
Portions of this file based upon zlib unzip sample code 
You'll know it when you see it :)

*/

/*********************************************************
*Name:          Skins
*Filename:      skins.c
*Author:        John Morrison
*Creation Date: 27/09/03
*Last Modified: 27/09/03
*Purpose:
*  Looks after loading/unloading skins
*  Code from zlib version 1.1.4
*********************************************************/

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <direct.h>

#include "../bolo/global.h"
#include "../zlib/unzip.h"
#include "skins.h"
#include "winutil.h"

/* Unzip functions */
#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)

bool skinsInUse = FALSE;
char skinsDirectory[512]; /* Path for skins to go */
char skinFileName[512]; /* Skin filename */

/*********************************************************
*NAME:          skinsSetup
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Sets up the skins initially. Returns Success
*
*ARGUMENTS:
*
*********************************************************/
bool skinsSetup(void) {
  skinsInUse = FALSE;
  return TRUE;
}

/*********************************************************
*NAME:          skinsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Cleans up after the skins. Remove temp files.
*
*ARGUMENTS:
*
*********************************************************/
void skinsDestroy(void) {
  if (skinsInUse == TRUE) {
    skinsRemoveFiles();
    skinsInUse = FALSE;
    /* Delete files and directory */
  }
}

/*********************************************************
*NAME:          skinsRemoveFiles
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Removes the skin files and temp directory
*
*ARGUMENTS:
*
*********************************************************/
void skinsRemoveFiles(void) {
  BOOL ret;                 /* Function return */
  WIN32_FIND_DATA fd;       /* Find data */
  HANDLE findHandle;        /* The find handle */
  char checkPath[MAX_PATH]; /* Path to check */
  char temp[MAX_PATH];

  if (skinsInUse == TRUE) {
    if (skinsDirectory[strlen(skinsDirectory)-1] != '\\') {
        strcat(skinsDirectory, "\\");
    }
    strcpy(checkPath, skinsDirectory);
    strcat(checkPath, "*.*");
    findHandle = FindFirstFile(checkPath, &fd);
    if (findHandle != INVALID_HANDLE_VALUE) {
      /* Atleast One file exist */
      if (fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
        /* Not a directory */
        sprintf(temp, "%s%s",skinsDirectory, fd.cFileName);
        DeleteFile(temp);
      }
      ret = FindNextFile(findHandle, &fd);
      while (ret != 0) {
        /* Process remaining */
        if (fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
          /* Not a directory */
          sprintf(temp, "%s%s",skinsDirectory, fd.cFileName);
          DeleteFile(temp);
        }
        ret = FindNextFile(findHandle, &fd);
      }
    }
    FindClose(findHandle);
    rmdir(skinsDirectory);
  }
}


/* change_file_date : change the date/time of a file
    filename : the filename of the file where date/time must be modified
    dosdate : the new date at the MSDos format (4 bytes)
    tmu_date : the SAME new date at the tm_unz format */
void change_file_date(filename,dosdate,tmu_date)
	const char *filename;
	uLong dosdate;
	tm_unz tmu_date;
{
#ifdef WIN32
  HANDLE hFile;
  FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;

  hFile = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,
                      0,NULL,OPEN_EXISTING,0,NULL);
  GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
  DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
  LocalFileTimeToFileTime(&ftLocal,&ftm);
  SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
  CloseHandle(hFile);
#else
#ifdef unix
  struct utimbuf ut;
  struct tm newdate;
  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min=tmu_date.tm_min;
  newdate.tm_hour=tmu_date.tm_hour;
  newdate.tm_mday=tmu_date.tm_mday;
  newdate.tm_mon=tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
      newdate.tm_year=tmu_date.tm_year - 1900;
  else
      newdate.tm_year=tmu_date.tm_year ;
  newdate.tm_isdst=-1;

  ut.actime=ut.modtime=mktime(&newdate);
  utime(filename,&ut);
#endif
#endif
}


/* mymkdir and change_file_date are not 100 % portable
   As I don't know well Unix, I wait feedback for the unix portion */

int mymkdir(dirname)
	const char* dirname;
{
    int ret=0;
#ifdef WIN32
	ret = mkdir(dirname);
#else
#ifdef unix
	ret = mkdir (dirname,0775);
#endif
#endif
	return ret;
}

int makedir (newdir)
    char *newdir;
{
  char *buffer ;
  char *p;
  int  len = strlen(newdir);  

  if (len <= 0) 
    return 0;

  buffer = (char*)malloc(len+1);
  strcpy(buffer,newdir);
  
  if (buffer[len-1] == '/') {
    buffer[len-1] = '\0';
  }
  if (mymkdir(buffer) == 0)
    {
      free(buffer);
      return 1;
    }

  p = buffer+1;
  while (1)
    {
      char hold;

      while(*p && *p != '\\' && *p != '/')
        p++;
      hold = *p;
      *p = 0;
      if ((mymkdir(buffer) == -1) && (errno == ENOENT))
        {
          printf("couldn't create directory %s\n",buffer);
          free(buffer);
          return 0;
        }
      if (hold == 0)
        break;
      *p++ = hold;
    }
  free(buffer);
  return 1;
}


int doExtractCurrentFile(uf,popt_extract_without_path,popt_overwrite)
	unzFile uf;
	const int* popt_extract_without_path;
    int* popt_overwrite;
{
	char filename_inzip[256];
	char* filename_withoutpath;
	char* p;
    int err=UNZ_OK;
    FILE *fout=NULL;
    void* buf;
    uInt size_buf;
	
	unz_file_info file_info;
	uLong ratio=0;
	err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);

	if (err!=UNZ_OK)
	{
		printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
		return err;
	}

    size_buf = WRITEBUFFERSIZE;
    buf = (void*)malloc(size_buf);
    if (buf==NULL)
    {
        printf("Error allocating memory\n");
        return UNZ_INTERNALERROR;
    }

	p = filename_withoutpath = filename_inzip;
	while ((*p) != '\0')
	{
		if (((*p)=='/') || ((*p)=='\\'))
			filename_withoutpath = p+1;
		p++;
	}

	if ((*filename_withoutpath)=='\0')
	{
		if ((*popt_extract_without_path)==0)
		{
			mymkdir(filename_inzip);
		}
	}
	else
	{
		char* write_filename;
		int skip=0;

		if ((*popt_extract_without_path)==0)
			write_filename = filename_inzip;
		else
			write_filename = filename_withoutpath;

		err = unzOpenCurrentFile(uf);
		if (err!=UNZ_OK)
		{
			printf("error %d with zipfile in unzOpenCurrentFile\n",err);
		}

    skip = 0;

		if ((skip==0) && (err==UNZ_OK))
		{
      char temp[512];

      sprintf(temp, "%s%s", skinsDirectory, write_filename);
      strcpy((char *) write_filename, temp);
			fout=fopen(write_filename,"wb");
      if (fout==NULL) {
				printf("error opening %s\n",write_filename);
      }
		}

		if (fout!=NULL)
		{
			printf(" extracting: %s\n",write_filename);

			do
			{
				err = unzReadCurrentFile(uf,buf,size_buf);
				if (err<0)	
				{
					printf("error %d with zipfile in unzReadCurrentFile\n",err);
					break;
				}
				if (err>0)
					if (fwrite(buf,err,1,fout)!=1)
					{
						printf("error in writing extracted file\n");
                        err=UNZ_ERRNO;
						break;
					}
			}
			while (err>0);
			fclose(fout);
			if (err==0) 
				change_file_date(write_filename,file_info.dosDate,
					             file_info.tmu_date);
		}

        if (err==UNZ_OK)
        {
		    err = unzCloseCurrentFile (uf);
		    if (err!=UNZ_OK)
		    {
			    printf("error %d with zipfile in unzCloseCurrentFile\n",err);
		    }
        }
        else
            unzCloseCurrentFile(uf); /* don't lose the error */       
	}

    free(buf);    
    return err;
}



int doExtract(unzFile uf,int optExtractWithoutPath,int optOverwrite) {
	uLong i;
	unz_global_info gi;
	int err;
	FILE* fout=NULL;

	err = unzGetGlobalInfo (uf,&gi);
  if (err!=UNZ_OK) {
		printf("error %d with zipfile in unzGetGlobalInfo \n",err);
  }

  for (i=0;i<gi.number_entry;i++) {
    if (doExtractCurrentFile(uf, &optExtractWithoutPath, &optOverwrite) != UNZ_OK) {
      /* Errlr */
    }

    if ((i+1)<gi.number_entry) {
			err = unzGoToNextFile(uf);
			if (err!=UNZ_OK) {
				printf("error %d with zipfile in unzGoToNextFile\n",err);
				break;
			}
		}
	}

	return 0;
}

/*********************************************************
*NAME:          skinsLoadSkin
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Attempts to load a skin set.
*
*ARGUMENTS:
*  fileName - Filename of skin to load.
*********************************************************/
bool skinsLoadSkin(char *fileName) {
  unzFile uf = NULL;      /* Unzip structure */
  char fileNameTry[512];  /* Unzip file to try */
  char path[512];         /* Path to prepend to load filename */

  skinsRemoveFiles();
  skinsInUse = FALSE;
  skinFileName[0] = EMPTY_CHAR;



  /* Create a temporary directory for it */
  GetTempPath(512, skinsDirectory);
  if (skinsDirectory[strlen(skinsDirectory)-1] != '\\') {
      strcat(skinsDirectory, "\\");
  }
  
  strcat(skinsDirectory, "WinBoloSkin");
  makedir(skinsDirectory);

  strcpy(path, SKIN_DIR_STRING);
  if (fileName[0] != EMPTY_CHAR && winUtilWBSubDirExist(path) == TRUE) {
    if (path[strlen(path)-1] != '\\') {
        strcat(path, "\\");
    }
    sprintf(fileNameTry, "%s%s", path, fileName);
    /* Try loading the zip file */
    uf = unzOpen((const char*) fileNameTry);
    if (uf == NULL) {
      strcat(fileNameTry,".wsf");
      uf = unzOpen((const char*) fileNameTry);
      if (uf != NULL) {
        strcpy(skinFileName, fileName);
      }
    } else {
      strcpy(skinFileName, fileName);
    }

	  if (uf == NULL) {
      sprintf(fileNameTry, "%s%s", path, fileName);
	    strcat(fileNameTry,".zip");
	    uf = unzOpen((const char*) fileNameTry);
      if (uf != NULL) {
        strcpy(skinFileName, fileName);
      }
		}
  }

  if (uf != NULL) {
    if (skinsDirectory[strlen(skinsDirectory)-1] != '\\') {
        strcat(skinsDirectory, "\\");
    }
    doExtract(uf, 0, 0);
    unzCloseCurrentFile(uf);
    skinsInUse = TRUE;
  } else {
    /* Remove directory */
    skinsRemoveFiles();
  }

  return skinsInUse;
}


/*********************************************************
*NAME:          skinsIsLoaded
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Returns if a skin set is loaded
*
*ARGUMENTS:
*
*********************************************************/
bool skinsIsLoaded() {
  return skinsInUse;
}

/*********************************************************
*NAME:          skinsIsLoaded
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Returns if a skin set is loaded
*
*ARGUMENTS:
* value - Char pointer to hold directory
*********************************************************/
void skinsGetSkinDirectory(char *value) {
  value[0] = EMPTY_CHAR;
  if (skinsInUse == TRUE) {
    strcpy(value, skinsDirectory);
  }
}

/*********************************************************
*NAME:          skinsGetFileName
*AUTHOR:        John Morrison
*CREATION DATE: 27/09/03
*LAST MODIFIED: 27/09/03
*PURPOSE:
*  Gets the skin file name of the current skin (excluding
*  extension) If no skin is loaded it returns NULL.
*
*ARGUMENTS:
* value - Char pointer to hold skin name
*********************************************************/
void skinsGetFileName(char *value) {
  strcpy(value, skinFileName);
}
