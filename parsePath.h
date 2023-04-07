#ifndef PARSEPATH_H
#define PARSEPATH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "vcb.h"
#include "dir.h"

// testing out parsePath
int parsePath(char *pathname){
	uint64_t dirEntrySize = sizeof(dirEntry);
	dirEntry *entryDir = malloc(dirEntrySize);

	char *saveptr, *token;
	char *delim = "/";

	token = strtok_r(pathname, delim, &saveptr);

    // skipping "." and ".." folder on root
	for(int i = 2; i < rootDir->size / dirEntrySize; i++){
		if (strcmp(rootDir[i].name, token) == 0) {

		}
	}

	while(token != NULL){
		
	}

	free(entryDir);
	free(rootDir);
	return 0;
}

#endif /* PARSEPATH_H */