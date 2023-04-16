#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dir.h"
#include "vcb.h"
#include "parsePath.h"
#include "mfs.h"
#include "fsLow.h"

int fs_setcwd(char *pathname) { 
    dirEntry *dir = malloc(MAXENTRIES * sizeof(dirEntry));
    if (dir == NULL) {
        return -1;
    }

    char *str = malloc((strlen(pathname) + 1) * sizeof(char));
    strcpy(str, pathname);
    str[strlen(pathname)] = '\0';

    if (parsePath(str, rootDir, dir) == -1) {
        free(dir);
        return -1;
    }

    if (currentwd != NULL) {
        free(currentwd);
    }
    currentwd = dir;

    free(str);
    return 0;
}

char *fs_getcwd(char *pathname, size_t size) {
    if(size == 0) {
        return NULL;
    }

    dirEntry *dir = malloc(MAXENTRIES * sizeof(dirEntry));
    if (dir == NULL) {
        return NULL;
    }
    dirEntryLoad(dir, currentwd);

    pathname = malloc(size * sizeof(char));
    if (pathname == NULL) {
        free(dir);
        return NULL;
    }
    pathname[0] = '\0';

    char *str, *name;
    unsigned int location;

    while(dir->location != rootDir->location) {
        location = dir->location;
        LBAread(dir, dir[1].size / fsvcb->blockSize, dir[1].location);
        name = dirFindName(dir, location);

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