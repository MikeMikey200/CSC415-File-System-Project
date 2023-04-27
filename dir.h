/**************************************************************
* Class: CSC-415-03 Spring 2023
* Names: Minh Dang, Sabrina Diaz-Erazo, Trinity Godwin
* Student IDs: 921210261, 916931527, 918448783
* GitHub Name: MikeMikey200
* Group Name: Wanna Cry
* Project: Basic File System
*
* File: dir.h
*
* Description: This file contains the directory entry 
*			   structure and function prototypes for the 
*			   directory entry helper functions.
*
**************************************************************/

#ifndef DIR_H
#define DIR_H

#include <time.h>

// number of directory entries
#define INITENTRIES 50 
#define MAXENTRIES 100

/*!
@struct 	dirEntry
@abstract contains directory entry details

@field		name
			file name 128 max char
@field 		idOwner
			unique owner id
@field		idGroup
			unique group id
@field 		type
			an arbitrary number standing for an extension
			1 = .txt, 2 = .pdf, 3 = .img, and so on
@field		size
			size of the file in bytes
@field		location
			block location of the file
@field		time
			a struct that can be use to dictate time using localtime_s
*/
typedef struct dirEntry {
	char name[256];
	int idOwner;
	int idGroup;
	int type;
	unsigned int size;
	unsigned int location;
	struct tm timeAccess;
	struct tm timeModify;
	struct tm timeCreate; 
} dirEntry;

extern dirEntry *rootDir;
extern dirEntry *currentwd;

// prototypes
/*!
@function	dirInit
@param		initNumEntry
			initial amount of directory entries
@param		parent
			the ".." or parent of the entry, NULL if it's root
@return		the initialized directory of "." and ".."
*/
dirEntry * dirInit(unsigned int initNumEntry, dirEntry *parent);

/*!
@function	dirFindUnusedEntry
@param		dir
			find the first instance of the available directory
@return		the index of the available directory or -1 if none exist
*/
int dirFindUnusedEntry(dirEntry *dir);

/*!
@function 	dirEntryCopy
@abstract 	the destination will be overriden by the source and write into disk
@param		destination
			the destination to be copied into
@param		source
			the source to be copied from
@param 		index
			the index of the source
@param		name
			name of the directory
*/
void dirEntryCopy (dirEntry *destination, dirEntry *source, unsigned int index, char *name);

/*!
@function 	dirEntryLoad
@abstract	the destination will become the source
@param		destination
			the destination to be load into
@param		source
			the source to be load from
@param 		index
			the index of the source
*/
void dirEntryLoadIndex(dirEntry *destination, dirEntry *source, unsigned int index);

void dirEntryLoad(dirEntry *destination, dirEntry *source);

char *dirFindName(dirEntry *dir, unsigned int location);

#endif /* DIR_H */