/**************************************************************
* Class: CSC-415-03 Spring 2023
* Names: Minh Dang, Sabrina Diaz-Erazo, Trinity Godwin
* Student IDs: 921210261, 916931527, 918448783
* GitHub Name: MikeMikey200
* Group Name: Wanna Cry
* Project: Basic File System
*
* File: fsFunction.c
*
* Description: This file contains the functions GetFileInfo
*              and FileInit, which helps b_open, the interface
*              to open a buffered file.
*
**************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "fsFunction.h"
#include "dir.h"
#include "vcb.h"
#include "fat.h"
#include "fsLow.h"

fileInfo * GetFileInfo (char * fname, char * path, dirEntry * parent) {
    fileInfo *finfo = malloc(sizeof(fileInfo));

    //find the file in the directories
    int size = parent->size / sizeof(dirEntry);
    for (int i = 2; i < size; i++) {
        if (strcmp(parent[i].name, fname) == 0) {
            strcpy(finfo->pathname, path);
            strcpy(finfo->fileName, fname);
            finfo->fileSize = parent[i].size;
            finfo->location = freespaceNextBlock(parent[i].location);
            return finfo;
        }
    }

    free(finfo);
    return NULL;
}

fileInfo *FileInit(char *fname, char * path, dirEntry *parent) {
    //check for unused entry in the directory
    int i = dirFindUnusedEntry(parent);
    if (i == -1) {
        return NULL;
    }

    dirEntry * fileEntry = dirInit(0, parent);
    fileEntry->type = 1;

    //writing new information in the parent directory and updating the current/parent directory
    dirEntryCopy(parent, fileEntry, i, fname);
    if (parent->location != currentwd->location) {
        dirEntryLoad(parent, parent);
    } else {
        dirEntryLoad(currentwd, currentwd);
    }
    free(fileEntry);
    
    fileInfo *finfo = malloc(sizeof(fileInfo));

    //putting in the inodes informations
    strcpy(finfo->pathname, path);
    strcpy(finfo->fileName, fname);
    finfo->fileSize = parent[i].size;
    finfo->location = freespaceNextBlock(parent[i].location);
    
    return finfo;
}