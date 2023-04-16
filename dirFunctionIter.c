#include <stdlib.h>
#include <string.h>

#include "mfs.h"
#include "dir.h"
#include "parsePath.h"

fdDir * fs_opendir(const char *pathname) {
    dirEntry *dir = malloc(MAXENTRIES * sizeof(dirEntry));
    
    char *str = malloc((strlen(pathname) + 1) * sizeof(dirEntry));
    strncpy(str, pathname, strlen(pathname));
    str[strlen(pathname)] = '\0';

    int index = parsePath(str, rootDir, dir);
    if (index == -1) {
        free(dir);
        return NULL;
    }

    fdDir *fd = malloc(sizeof(fdDir));
    fd->dirEntryPosition = 0;
    fd->directoryStartLocation = dir[index].location;
    fd->d_reclen = sizeof(fdDir);

    return fd;
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp) {
    struct fs_diriteminfo *dirInfo = malloc(sizeof(struct fs_diriteminfo));
    dirInfo->d_reclen = sizeof(struct fs_diriteminfo);
    if(currentwd[dirp->dirEntryPosition].type == 0) {
        dirInfo->fileType = DT_DIR;
    } else {
        dirInfo->fileType = DT_REG;
    }
    
    strcpy(dirInfo->d_name, currentwd[dirp->dirEntryPosition].name);

    while(currentwd[++dirp->dirEntryPosition].name[0] == '\0') {
        if (dirp->dirEntryPosition >= currentwd->size / sizeof(dirEntry)) {
            return NULL;
        }
    }

    return dirInfo;
}

int fs_closedir(fdDir *dirp) {
    free(dirp);
}