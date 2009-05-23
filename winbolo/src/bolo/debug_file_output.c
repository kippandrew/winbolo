

#include "debug_file_output.h"

FILE *debugFileStream;
char fileName[2048];
int writeToDebugFileStream;

void getFileName(char *file) {
	if (writeToDebugFileStream == 1) {
		strncpy(file,fileName,strlen(fileName));
	} else {
		strncpy(file,'\0',strlen('\0'));
	}
}

void setFileName(char *newFileName) {
	if (writeToDebugFileStream == 1) {
		strncpy(fileName,newFileName,strlen(newFileName));
	}
}

int openDebugFile() {
	if (writeToDebugFileStream == 1) {
		debugFileStream = fopen(fileName,"w");
		if (debugFileStream != NULL) {
			return 0;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

int writeDebugFile(char *text) {
	if (writeToDebugFileStream == 1) {
		if (debugFileStream != NULL) {
			fputs(text, debugFileStream);
			/* fputs("\n",debugFileStream); */
			return 0;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

int closeDebugFile() {
	if (writeToDebugFileStream == 1) {
		if (debugFileStream != NULL) {
			fclose(debugFileStream);
			return 0;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

int getWriteToDebugFileStream() {
	return writeToDebugFileStream;
}

void setWriteToDebugFileStream(int write) {
	writeToDebugFileStream = write;
}
