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

    if (index == -1) {
        free(dir);
        dir = NULL;
        return -1;
    }

    dirEntryLoadIndex(currentwd, dir, index);

    free(dir);
    dir = NULL;
    return 0;
}

char *fs_getcwd(char *pathname, size_t size) {
    if(size == 0) {
        return NULL;
    }

    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    if (dir == NULL) {
        return 0;
    }

    dirEntryLoad(dir, currentwd);
    
    pathname = malloc(size * sizeof(char));
    if (pathname == NULL) {
        free(dir);
        dir = NULL;
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
            dir = NULL;
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
    dir = NULL;
    return pathname;
}

int fs_isDir(char * pathname) {
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    if (dir == NULL) {
        return 0;
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

    if (dir[index].type == 0) {
        free(dir);
        dir = NULL;
        return 1;
    }
    else {
        free(dir);
        dir = NULL;
        return 0;
    }
}

int fs_isFile(char * filename) {
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    if (dir == NULL) {
        return 0;
    }

    char str[MAXPATH];
    strcpy(str, filename);
    str[strlen(filename)] = '\0';
    int index;

    if (str[0] == '\\') {
        index = parsePath(str, rootDir, dir);
    } else {
        index = parsePath(str, currentwd, dir);
    }
    
    if (index == -1 || dir[index].type == 0) {
        free(dir);
        dir = NULL;
        return 0;
    }

    if (dir[index].type != 0) {
        free(dir);
        dir = NULL;
        return 1;
    }

    free(dir);
    dir = NULL;
    return 0;
}

int fs_delete(char* filename) {
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    if (dir == NULL) {
        return -1;
    }

    char str[MAXPATH];
    strcpy(str, filename);
    str[strlen(filename)] = '\0';
    int index;

    if (str[0] == '\\') {
        index = parsePath(str, rootDir, dir);
    } else {
        index = parsePath(str, currentwd, dir);
    }

    if (index == -1 || dir[index].type == 0) {
        free(dir);
        return -1;
    }

    strcpy(dir[index].name, "\0");
    LBAwrite(dir, dir->size / fsvcb->blockSize, dir->location);
    dirEntryLoad(currentwd, currentwd);

    freespaceReleaseBlocks(dir[index].location);

    free(dir);
    dir = NULL;
    return 0;
}

int fs_stat(const char *path, struct fs_stat *buf) {
    dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
    if (dir == NULL) {
        return -1;
    }

    char str[MAXPATH];
    strcpy(str, path);
    str[strlen(path)] = '\0';

    int index;
    if (str[0] == '\\') {
        index = parsePath(str, rootDir, dir);
    } else {
        index = parsePath(str, currentwd, dir);
    }

    buf->st_size = dir[index].size;
    buf->st_blksize = fsvcb->blockSize;
    buf->st_blocks = dir[index].size / fsvcb->blockSize;
    buf->st_createtime = mktime(&dir[index].timeCreate);
    buf->st_modtime = mktime(&dir[index].timeModify);
    buf->st_accesstime = mktime(&dir[index].timeAccess);

    free(dir);
    dir = NULL;
    return 0;
}