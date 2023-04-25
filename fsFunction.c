#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "fsFunction.h"
#include "dir.h"
#include "vcb.h"
#include "fat.h"
#include "fsLow.h"

fileInfo * GetFileInfo (char * fname, dirEntry * parent) {
    fileInfo *finfo = malloc(sizeof(fileInfo));

    int size = parent->size / fsvcb->blockSize;
    for (int i = 2; i < size; i++) {
        if (strcmp(parent[i].name, fname) == 0) {
            strcpy(finfo->fileName, fname);
            finfo->fileSize = parent[i].size;
            int location = freespaceNextBlock(parent[i].location);
            location = freespaceNextBlock(location);
            finfo->location = location;
            return finfo;
        }
    }

    free(finfo);
    return NULL;
}

dirEntry *FileInit (char *fname, dirEntry *parent, fileInfo *finfo) {
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
    
    finfo = malloc(sizeof(fileInfo));

    strcpy(finfo->fileName, fname);
    finfo->fileSize = parent[i].size;
    freespaceAllocateBlocks(parent[i].location, 1);
    int location = freespaceNextBlock(parent[i].location);
    location = freespaceNextBlock(location);
    finfo->location = location;
    return fileEntry;
}