#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "fsLow.h"
#include "dir.h"
#include "vcb.h"
#include "fat.h"

dirEntry * dirInit(unsigned int initNumEntry, dirEntry *parent) {
    unsigned int dirEntrySize = sizeof(dirEntry);
    unsigned int bytesNeeded = initNumEntry * dirEntrySize;
    unsigned int blocksNeeded = (bytesNeeded + fsvcb->blockSize - 1) / fsvcb->blockSize;
    unsigned int bytesUsed = blocksNeeded * fsvcb->blockSize;
    unsigned int numEntry = bytesUsed / dirEntrySize;
    bytesNeeded = numEntry * dirEntrySize;
    dirEntry *dir = malloc(bytesUsed);
    unsigned int startBlock = freespaceFindFreeBlock();
    freespaceAllocateBlocks(startBlock, blocksNeeded);
    
    // init each dir to unused
    for(int i = 2; i < numEntry; i++){
        dir[i].name[0] = '\0';
    }

    // initialize "."
    strcpy(dir[0].name, ".");
    time_t timer;
    timer = time(NULL);
    localtime_r(&timer, &dir[0].time);
    dir[0].location = startBlock;
    dir[0].size = bytesNeeded;
    dir[0].type = 0;

    // initialize ".."
    // it is a rootDir
    if (parent == NULL){
        strcpy(dir[1].name, "..");
        dir[1].size = dir[0].size;
        dir[1].location = dir[0].location;
	    dir[1].time = dir[0].time;
        dir[1].type = dir[0].type;
    } else {
        // not a rootDir
        strcpy(dir[1].name, parent->name);
        dir[1].size = parent->size;
        dir[1].location = parent->location;
        dir[1].time = parent->time;
        dir[1].type = parent->type;
    }

    LBAwrite(dir, blocksNeeded, startBlock);
    return dir;
}

int dirFindUnusedEntry(dirEntry *dir) {
    unsigned int dirEntryNum = dir->size / sizeof(dirEntry);
    for(int i = 2; i < dirEntryNum; i++){
        if (dir[i].name[0] == '\0') {
            return i;
        }
    }
    return -1;
}

void dirEntryCopy (dirEntry *destination, dirEntry *source, unsigned int index, char *name) {
    strcpy(destination[index].name, name);
    destination[index].size = source->size;
    destination[index].location = source->location;
    destination[index].time = source->time;
    destination[index].type = source->type;
    LBAwrite(destination, destination->size / fsvcb->blockSize, destination->location);
}

void dirEntryLoadIndex(dirEntry *destination, dirEntry *source, unsigned int index) {
    LBAread(destination, source[index].size / fsvcb->blockSize, source[index].location);
}

void dirEntryLoad(dirEntry *destination, dirEntry *source) {
    LBAread(destination, source->size / fsvcb->blockSize, source->location);
}

char *dirFindName(dirEntry *dir, unsigned int location) {
    int size = dir->size / fsvcb->blockSize;

    for(int i = 0; i < size; i++) {
        if(dir[i].location == location) {
            return dir[i].name;
        }
    }

    return NULL;
}