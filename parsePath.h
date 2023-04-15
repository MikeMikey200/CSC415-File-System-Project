#ifndef PARSEPATH_H
#define PARSEPATH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vcb.h"
#include "dir.h"

#define MAXENTRIES 100

/*
@function 	parsePath
@abstract 	take in a $pathname and parse it until last tokenized string if it exist or not in our directory

@param		pathname
			$pathname "foo\bar"
@param		dir
			rootDir or starting dirEntry

@return		the index of the directory
*/
int parsePath(char *pathname, dirEntry *dir) {
	dirEntry *entryDir = malloc(sizeof(dirEntry) * MAXENTRIES);

	char *saveptr, *token, *tokenPrev;
	char *delim = "\\";

	// determine if file name and directory name convention are the same
	int flag = 0;

	// skipping "." and ".." entry on directory
	int index = 2;

	// load the initial dir
	LBAread(entryDir, dir->size / fsvcb->blockSize, dir->location);

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
			LBAread(entryDir, entryDir[index].size / fsvcb->blockSize, entryDir[index].location);
			index = 2;
			flag = 0;
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

/*!
@function 	locateEntry
@abstract locate the entry using index to traverse the directory to find name

@param 		name
			indicate pass by reference of tokenized string name
@param 		dir
			indicate pass by reference of dirEntry object
@param		index
			indicate the index of the directory

@return		if found return index of the entry,
			if not return -1
*/
int locateEntry(char *name, dirEntry *dir, int index) {
	int size = dir->size / sizeof(dirEntry);

	for (int i = index; i < size; i++){
		if (entryDir[i].name != '\0' && strcmp(entryDir[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}

#endif /* PARSEPATH_H */