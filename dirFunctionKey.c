#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dir.h"
#include "fat.h"
#include "vcb.h"
#include "parsePath.h"
#include "mfs.h"
#include "fsLow.h"

//mode_t will be implimented if permission are require
//pathname refers to the directory name, currentwd is used as the parent when called
int fs_mkdir(const char *pathname, mode_t mode) {
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    if (dir == NULL) {
        return -1;
    }

    dirEntry *item;
    char str[MAXPATH];
    strcpy(str, pathname);
    str[strlen(pathname)] = '\0';
    int index;

    if (str[0] == '\\') {
        index = parsePath(str, rootDir, dir);
    } else {
        index = parsePath(str, currentwd, dir);
    }

    if (index != -1) {
        free(dir);
        dir = NULL;
        return -1;
    }

    index = dirFindUnusedEntry(dir);
    if (index == -1) {
        free(dir);
        dir = NULL;
        return -1;
    }

    item = dirInit(INITENTRIES, dir);

    char *saveptr, *tokenPrev;
    char *delim = "\\";
    char *token = strtok_r(str, delim, &saveptr);
    
    while (token != NULL) {
        tokenPrev = token;
        token = strtok_r(NULL, delim, &saveptr);
    }
    
    dirEntryCopy(dir, item, index, tokenPrev);
    dirEntryLoad(currentwd, currentwd);

    free(dir);
    free(item);
    dir = NULL;
    item = NULL;
    return 0;
}

int fs_rmdir(const char *pathname) {
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    if (dir == NULL) {
        return -1;
    }

    char str[MAXPATH];
    strcpy(str, pathname);
    str[strlen(pathname)] = '\0';
    int index;

    if (str[0] == '\\') {
        index = parsePath(str, rootDir, dir);
    } else {
        index = parsePath(str, currentwd, dir);
    }

    if (index == -1) {
        free(dir);
        dir = NULL;
        return -1;
    }

    dirEntryLoadIndex(dir, dir, index);

    int size = dir->size / fsvcb->blockSize;
    for (int i = 2; i < size; i++) {
        if (dir[i].name[0] != '\0') {
            free(dir);
            dir = NULL;
            return -1;
        }
    }

    dirEntryLoadIndex(dir, dir, 1);

    strcpy(dir[index].name, "\0");
    LBAwrite(dir, dir->size / fsvcb->blockSize, dir->location);
    dirEntryLoad(currentwd, currentwd);

    freespaceReleaseBlocks(dir[index].location);

    free(dir);
    dir = NULL;
    return 0;
}