#include "dir.h"
#include "parsePath.h"

#define WRITEBLOCK 50

typedef struct fileInfo {
	char pathname[MAXPATH];
	char fileName[64];		//filename
	int fileSize;			//file size in bytes
	int location;			//starting lba (block number) for the file data
} fileInfo;

fileInfo * GetFileInfo (char * fname, char * path, dirEntry * parent);
fileInfo *FileInit(char *fname, char * path, dirEntry *parent);