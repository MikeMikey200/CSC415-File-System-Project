#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dir.h"
#include "fat.h"
#include "vcb.h"
#include "parsePath.h"
#include "mfs.h"

//mode_t will be implimented if permission are require
//pathname refers to the directory name, currentwd is used as the parent when called
int fs_mkdir(const char *pathname, mode_t mode) {
    //checking if the name is already in use in the directory
    //starts at zero to prevent user adding . or .. to directory
    for(int i = 0; i < currentwd->size; i++)
    {
        if (currentwd[i].name[0] != '\0' && strcmp(currentwd[i].name, pathname) == 0) {
			return -1;
		}
	}
    dirEntry *dir = dirInit(MAXENTRIES, currentwd);
    dirEntryCopy(currentwd, dir, dirFindUnusedEntry(currentwd), pathname);
    return 0;
}

int fs_rmdir(const char *pathname) {
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    char str[MAXPATH];
    strcpy(str, pathname);
    str[strlen(pathname)] = '\0';
    int index = parsePath(str, rootDir, dir);
    if (index == -1) {
        free(dir);
        return -1;
    }



    return 0;
}

/*
dirEntryLoadIndex(currentwd, currentwd, i);
                int sizeDir = currentwd->size / fsvcb->blockSize; 
                for (int j = 2; j < sizeDir; j++) {
                    fs_delete(currentwd[j].name);
                }
                dirEntryLoadIndex(currentwd, currentwd, 1);
                freespaceReleaseBlocks(currentwd[i].location);
*/