#include "dir.h"
#include "vcb.h"

/* unsure how this function will help us input the field for the dir */
int dirInit(unsigned int initNumEntry, dirEntry *parent) {
    unsigned int dirEntrySize = sizeof(dirEntry);
    unsigned int bytesNeeded = initNumEntry * dirEntrySize;
    unsigned int blocksNeeded = (blocksNeeded + fsvcb->blockSize - 1) / fsvcb->blockSize;
    unsigned int bytesUsed = blocksNeeded * fsvcb->blockSize;
    unsigned int numEntry = bytesUsed / dirEntrySize;
    bytesNeeded = numEntry * dirEntrySize;
    dirEntry *dir = malloc(bytesUsed);
    unsigned int startBlock; //freespaceAllocateBlocks
}