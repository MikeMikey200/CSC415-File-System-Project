#ifndef DIR_H
#define DIR_H

#include <time.h>
#include "fsLow.h"

// number of directory entries
#define INITENTRIES 50 

// contains directory entry details
typedef struct dirEntry {
	char name[128]; // max char for name
	int idOwner; // unique owner id
	int idGroup; // unique group id
	int type; // like 1 = .txt, 2 = .pdf, 3 = .img, whatever
	uint64_t size; // size of file in bytes
	uint64_t location; // location of the file
	time_t time; // from 1900 using localtime_s
} dirEntry;

// making this a global variable so all can reference it
dirEntry *rootDir;

#endif /* DIR_H */