/**************************************************************
* Class: CSC-415-03 Spring 2023
* Names: Minh Dang, Sabrina Diaz-Erazo, Trinity Godwin
* Student IDs: 921210261, 916931527, 918448783
* GitHub Name: MikeMikey200
* Group Name: Wanna Cry
* Project: Basic File System
*
* File: dirFunctionKey.c
*
* Description: This file contains the following key directory
*              functions: fs_mkdir and fs_rmdir.
*
**************************************************************/

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
    //parsepath doesn't take const so a copy is made and passed
    dirEntry *item;
    char str[MAXPATH];
    strcpy(str, pathname);
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

    int size = dir->size / sizeof(dirEntry);
    for (int i = 2; i < size; i++) {
        if (dir[i].name[0] != '\0') {
            free(dir);
            dir = NULL;
            return -1;
        }
    }

    dirEntryLoadIndex(dir, dir, 1);

    strcpy(dir[index].name, "\0");
    LBAwrite(dir, (dir->size + fsvcb->blockSize - 1) / fsvcb->blockSize, dir->location);
    dirEntryLoad(currentwd, currentwd);

    freespaceReleaseBlocks(dir[index].location);

    free(dir);
    dir = NULL;
    return 0;
}