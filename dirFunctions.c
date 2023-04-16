#include <stdlib.h>

#include "dir.h"
#include "parsePath.h"
#include "mfs.h"

/* extern definitions */
dirEntry *currentwd;

int fs_setcwd(char *pathname) { 
    dirEntry *dir = malloc(MAXENTRIES * sizeof(dirEntry));
    parsePath(pathname, rootDir, dir);
    if (dir == NULL) {
        return -1;
    }

    if (currentwd != NULL) {
        free(currentwd);
    }
    currentwd = dir;

    return 0;
}

char *fs_getcwd(char *pathname, size_t size) {
    
}