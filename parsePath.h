#ifndef PARSEPATH_H
#define PARSEPATH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "vcb.h"
#include "dir.h"

#define MAXENTRIES 100

// exist or not exist
int parsePath(char *pathname) {
	dirEntry *entryDir = malloc(sizeof(dirEntry) * MAXENTRIES);

	char *saveptr, *token, *tokenPrev;
	char *delim = "/";
	int flag = 0;

	// skipping "." and ".." folder on directory
	int index = 2;

	// load the rootDir
	LBAread(entryDir, rootDir->size / MINBLOCKSIZE, rootDir->location);

	token = strtok_r(pathname, delim, &saveptr);
	while(token != NULL){
		// ignoring if the naming of the file == directory for now
		index = locateEntry(token, entryDir, index);
		if (index == -1) {
			// not exist
			free(entryDir);
			return -1;
		}
		if (!flag) {
			tokenPrev = token;
			token = strtok_r(NULL, delim, &saveptr);
		}
		if (entryDir[index].type == 0) {
			// a directory
			// is this the item?
			if (token == NULL){
				if (strcmp(entryDir[index].name, tokenPrev) == 0){
					// exist
					free(entryDir);
					return index;
				}
				// not exist
				free(entryDir);
				return -1;
			}
			// load directory
			LBAread(entryDir, entryDir[index].size / MINBLOCKSIZE, entryDir[index].location);
			index = 2;
		} else {
			// not a directory
			// is this the item?
			if (token == NULL){
				if (strcmp(entryDir[index].name, tokenPrev) == 0){
					// exist
					free(entryDir);
					return index;
				}
				// not exist
				free(entryDir);
				return -1;
			}

			// if file name and directory name are the same, skip the current file
			if (strcmp(entryDir[index].name, tokenPrev) == 0) {
				index++;
				flag = 1;
			} else {
				flag = 0;
			}
		}
	}

	// not exist
	free(entryDir);
	return -1;
}

// return index of the entry if found, -1 if not
int locateEntry(char *name, dirEntry *dir, int index) {
	int size = dir->size / sizeof(dirEntry);

	for (int i = index; i < size; i++){
		if (strcmp(entryDir[i].name, token) == 0) {
			return i;
		}
	}
	return -1;
}

#endif /* PARSEPATH_H */