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

    int index = parsePath(str, rootDir, dir);
    if (index == -1) {
        free(dir);
        return -1;
    }

    if (currentwd != NULL) {
        free(currentwd);
    }
    dirEntryLoadIndex(currentwd, dir, index);

    free(dir);
    free(str);
    return 0;
}

char *fs_getcwd(char *pathname, size_t size) {
    if(size == 0) {
        return NULL;
    }
    printf("set here\n");
    dirEntry *dir = malloc(MAXENTRIES * sizeof(dirEntry));
    printf("set here\n");
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

    char *str = malloc(sizeof(pathname));
    str[0] = '\0';

    char *name;
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
    free(str);
    return pathname;
}

int fs_isDir(char * pathname) {
    dirEntry *dir = malloc(MAXENTRIES * sizeof(dirEntry));
    if (dir == NULL) {
        return -1;
    }

    // check if pathname is a directory
    if (parsePath(pathname, rootDir, dir) != -1 ) {
        return 1;
    }

    else {
        return 0;
    }

} //return 1 if directory, 0 otherwise

int fs_isFile(char * filename) {
    if (!fs_isDir(pathname)) {
        return 1;
    }

    else {
        return 0;
    }

}	//return 1 if file, 0 otherwise