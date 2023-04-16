#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dir.h"
#include "fat.h"
#include "vcb.h"
#include "parsePath.h"
#include "mfs.h"

int fs_mkdir(const char *pathname, mode_t mode) {
    

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