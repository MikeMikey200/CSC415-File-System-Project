#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parsePath.h"
#include "vcb.h"
#include "fsLow.h"

int parsePath(char *pathname, dirEntry *dir, dirEntry *catch) {
	if (strlen(pathname) == 1 && pathname[0] == '\\') {
		dirEntryLoad(catch, rootDir);
		return 0;
	}

	char str[MAXPATH];
    strcpy(str, pathname);
    str[strlen(pathname)] = '\0';

	dirEntry *entryDir = malloc(sizeof(dirEntry) * MAXENTRIES);

	char *saveptr, *token, *tokenPrev;
	char *delim = "\\";

	// determine if file name and directory name convention are the same
	int flag = 0;

	int index = 0;
	
	// load the initial dir
    LBAread(entryDir, dir->size / fsvcb->blockSize, dir->location);

	token = strtok_r(str, delim, &saveptr);
	while(token != NULL){
		// ignoring if the naming of the file == directory for now
        if (!flag) {
		    index = locateEntry(token, entryDir, index);
			tokenPrev = token;
			token = strtok_r(NULL, delim, &saveptr);
        }
        else {
            index = locateEntry(tokenPrev, entryDir, index);
        }

		if (index == -1) {
			// not exist
			dirEntryLoad(catch, entryDir);
			free(entryDir);
			return -1;
		}

		// is this the item?
		if (token == NULL){
			dirEntryLoad(catch, entryDir);
			if (strcmp(entryDir[index].name, tokenPrev) == 0){
				// exist
				free(entryDir);
				return index;
			}
			// not exist
			free(entryDir);
			return -1;
		}

		if (strcmp(entryDir[index].name, tokenPrev) == 0) {
			if (entryDir[index].type == 0) {
				// a directory
				// load directory
				LBAread(entryDir, entryDir[index].size / fsvcb->blockSize, entryDir[index].location);
				index = 0;
				flag = 0;
			} else {
				// not a directory
				// if file name and directory name are the same, skip the current file
				index++;
				flag = 1;
			}
		}
	}

	// not exist
	catch = NULL;
	free(entryDir);
	return -1;
}

int locateEntry(char *name, dirEntry *dir, int index) {
	int size = dir->size / sizeof(dirEntry);

	for (int i = index; i < size; i++){
		if (dir[i].name[0] != '\0' && strcmp(dir[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}