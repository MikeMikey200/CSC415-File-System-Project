#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dir.h"
#include "vcb.h"
#include "parsePath.h"
#include "mfs.h"
#include "fsLow.h"

int fs_setcwd(char *pathname) { 
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    if (dir == NULL) {
        return -1;
    }

    char *str = malloc((strlen(pathname) + 1) * sizeof(char));
    strcpy(str, pathname);
    str[strlen(pathname)] = '\0';

    int index = parsePath(str, rootDir, dir);
    if (index == -1) {
        free(dir);
        return -1;
    }

    dirEntryLoadIndex(currentwd, dir, index);

    free(dir);
    free(str);
    return 0;
}

char *fs_getcwd(char *pathname, size_t size) {
    if(size == 0) {
        return NULL;
    }

    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);

    dirEntryLoad(dir, currentwd);
    
    pathname = malloc(size * sizeof(char));
    if (pathname == NULL) {
        free(dir);
        return NULL;
    }
    pathname[0] = '\0';

    char str[size];
    str[0] = '\0'; 

    char *name;
    unsigned int location;

    int index = 0;
    while(dir[0].location != rootDir[0].location) {
        location = dir[0].location;
        LBAread(dir, dir[1].size / fsvcb->blockSize, dir[1].location);
        name = dirFindName(dir, location);
        
        index++;
        index += strlen(name);
        if (index >= size) {
            free(dir);
            return NULL;
        }

        strcat(pathname, "\\");
        strcat(pathname, name);
        strcat(pathname, str);

        strcpy(str, pathname);
        strcpy(pathname, "");
        
    }
    strcpy(pathname, str);

    free(dir);
    return pathname;
}

int fs_isDir(char * pathname) {
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    if (dir == NULL) {
        return -1;
    }

    // check if pathname is a directory
    int index = parsePath(pathname, rootDir, dir);
    if (index == -1 ) {
        free(dir);
        return -1;
    }

    if (dir[index].type == 0) {
        free(dir);
        return 1;
    }
    else {
        free(dir);
        return 0;
    }
}

int fs_isFile(char * filename) {
    for(int i = 0; i < currentwd->size / fsvcb->blockSize; i++) {
        if (currentwd[i].type != 0 && strcmp(currentwd[i].name, filename) == 0) {
            return 1;
        }
    }

    return 0;
}