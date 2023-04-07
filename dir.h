#ifndef DIR_H
#define DIR_H

#include <time.h>
#include "fsLow.h"

// number of directory entries
#define INITENTRIES 50 

/*!
@struct 	dirEntry
@discussion contains directory entry details

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
	char name[128];
	int idOwner;
	int idGroup;
	int type;
	uint64_t size;
	uint64_t location;
	time_t time; 
} dirEntry;

// making this a global variable so all can reference it
dirEntry *rootDir;

#endif /* DIR_H */