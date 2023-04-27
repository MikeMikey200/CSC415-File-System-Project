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
    int i = dirFindUnusedEntry(parent);
    if (i == -1) {
        return NULL;
    }

    dirEntry * fileEntry = dirInit(0, parent);
    fileEntry->type = 1;
    dirEntryCopy(parent, fileEntry, i, fname);
    if (parent->location != currentwd->location) {
        dirEntryLoad(parent, parent);
    } else {
        dirEntryLoad(currentwd, currentwd);
    }
    free(fileEntry);
    
    fileInfo *finfo = malloc(sizeof(fileInfo));

    strcpy(finfo->pathname, path);
    strcpy(finfo->fileName, fname);
    finfo->fileSize = parent[i].size;
    finfo->location = freespaceNextBlock(parent[i].location);
    return finfo;
}