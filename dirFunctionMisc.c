#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dir.h"
#include "vcb.h"
#include "fat.h"
#include "parsePath.h"
#include "mfs.h"
#include "fsLow.h"

int fs_setcwd(char *pathname) { 
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

    printf("%d\n", index);
    if (index == -1) {
        free(dir);
        return -1;
    }

    dirEntryLoadIndex(currentwd, dir, index);

    free(dir);
    return 0;
}

char *fs_getcwd(char *pathname, size_t size) {
    if(size == 0) {
        return NULL;
    }

    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);

    dirEntryLoad(dir, currentwd);
    
    pathname = malloc(size * sizeof(char));
    if (pathname == NULL) {
        free(dir);
        return NULL;
    }
    pathname[0] = '\0';

    char str[size];
    str[0] = '\0'; 

    char *name;
    unsigned int location;

    int index = 0;
    while(dir[0].location != rootDir[0].location) {
        location = dir[0].location;
        LBAread(dir, dir[1].size / fsvcb->blockSize, dir[1].location);
        name = dirFindName(dir, location);
        
        index++;
        index += strlen(name);
        if (index >= size) {
            free(dir);
            return NULL;
        }

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

int fs_isDir(char * pathname) {
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    if (dir == NULL) {
        return -1;
    }

    // check if pathname is a directory
    int index = parsePath(pathname, rootDir, dir);
    if (index == -1 ) {
        free(dir);
        return -1;
    }

    if (dir[index].type == 0) {
        free(dir);
        return 1;
    }
    else {
        free(dir);
        return 0;
    }
}

int fs_isFile(char * filename) {
    for(int i = 0; i < currentwd->size / fsvcb->blockSize; i++) {
        if (currentwd[i].type != 0 && strcmp(currentwd[i].name, filename) == 0) {
            return 1;
        }
    }

    return 0;
}

int fs_delete(char* filename) {
    int size = currentwd->size / fsvcb->blockSize;
    for (int i = 2; i < size; i++) {
        if (strcmp(currentwd[i].name, filename) == 0 && currentwd[i].type != 0) {
            freespaceReleaseBlocks(currentwd[i].location);
            return 0;
        }
    }

    return -1;
}

int fs_stat(const char *path, struct fs_stat *buf) {
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    char str[MAXPATH];
    strcpy(str, path);
    str[strlen(path)] = '\0';

    int index = parsePath(str, rootDir, dir);

    buf->st_size = dir[index].size;
    buf->st_blksize = fsvcb->blockSize;
    buf->st_blocks = dir[index].size / fsvcb->blockSize;
    buf->st_createtime = mktime(&dir[index].timeCreate);
    buf->st_modtime = mktime(&dir[index].timeModify);
    buf->st_accesstime = mktime(&dir[index].timeAccess);

    free(dir);
    return 0;
}