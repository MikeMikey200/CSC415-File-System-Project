/**************************************************************
* Class: CSC-415-03 Spring 2023
* Names: Minh Dang, Sabrina Diaz-Erazo, Trinity Godwin
* Student IDs: 921210261, 916931527, 918448783
* GitHub Name: MikeMikey200
* Group Name: Wanna Cry
* Project: Basic File System
*
* File: fsFunction.h
*
* Description: This file contains the fileInfo struct and 
*			   the function prototypes for GetFileInfo and
*			   FileInit.
*
**************************************************************/

#include "dir.h"
#include "parsePath.h"

#define WRITEBLOCK 50

typedef struct fileInfo {
	char pathname[MAXPATH];	//pathname
	char fileName[64];		//filename
	int fileSize;			//file size in bytes
	int location;			//starting lba (block number) for the file data
} fileInfo;

fileInfo * GetFileInfo (char * fname, char * path, dirEntry * parent);
fileInfo *FileInit(char *fname, char * path, dirEntry *parent);