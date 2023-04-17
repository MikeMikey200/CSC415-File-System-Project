#include <stdlib.h>
#include <string.h>

#include "mfs.h"
#include "dir.h"
#include "vcb.h"
#include "parsePath.h"

fdDir * fs_opendir(const char *pathname) {
    dirEntry *dir = malloc(MAXENTRIES * sizeof(dirEntry));
    
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
        return NULL;
    }

    fdDir *fd = malloc(sizeof(fdDir));
    fd->dirEntryPosition = 0;
    fd->directoryStartLocation = dir[index].location;
    fd->d_reclen = sizeof(fdDir);

    free(dir);
    return fd;
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp) {
    int size = currentwd->size / sizeof(dirEntry);
    
    for(int i = dirp->dirEntryPosition; i < size; i++) {
        if (strcmp(currentwd[i].name, "\0") != 0) {
            struct fs_diriteminfo *dirInfo = malloc(sizeof(struct fs_diriteminfo));
            dirInfo->d_reclen = sizeof(struct fs_diriteminfo);

            if(currentwd[i].type == 0) {
                dirInfo->fileType = DT_DIR;
            } else {
                dirInfo->fileType = DT_REG;
            }
            strcpy(dirInfo->d_name, currentwd[i].name);

            dirp->dirEntryPosition = i + 1;
            
            return dirInfo;
        }
    }

    return NULL;
}

int fs_closedir(fdDir *dirp) {
    if (dirp == NULL) {
        return -1;
    }

    free(dirp);
    dirp = NULL;
    return 0;
}