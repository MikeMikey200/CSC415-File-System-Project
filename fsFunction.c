#include <string.h>
#include <stdlib.h>

#include "fsFunction.h"
#include "dir.h"
#include "vcb.h"
#include "fsLow.h"

fileInfo * GetFileInfo (char * fname) {
    fileInfo *file = malloc(sizeof(fileInfo));

    int size = currentwd->size / fsvcb->blockSize;
    for (int i = 2; i < size; i++) {
        if (strcmp(currentwd[i].name, fname) == 0) {
            strcpy(file->fileName, fname);
            file->fileSize = currentwd[i].size;
            file->location = currentwd[i].location;
            free(file);
            return file;
        }
    }

    int i = dirFindUnusedEntry(currentwd);
    if (i == -1)
        return NULL;

    dirEntry * fileEntry = dirInit(INITFILESIZE, currentwd);
    fileEntry->type = 1;

    dirEntryCopy(currentwd, fileEntry, dirFindUnusedEntry(currentwd), fname);
    dirEntryLoad(currentwd, currentwd);

    strcpy(file->fileName, fname);
    file->fileSize = currentwd[i].size;
    file->location = currentwd[i].location;
    free(fileEntry);
    return file;
}