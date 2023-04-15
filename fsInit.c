/**************************************************************
* Class: CSC-415-03 Spring 2023
* Names: Minh Dang, Sabrina Diaz-Erazo, Trinity Godwin, Karma Namgyal Ghale
* Student IDs: 921210261, 916931527, 918448783, 921425775
* GitHub Name: MikeMikey200
* Group Name: Wanna Cry
* Project: Basic File System
*
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "fsLow.h"
#include "mfs.h"
#include "vcb.h"
#include "fat.h"
#include "dir.h"

/* extern definitions */
vcb * fsvcb;

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	unsigned int vcbSize = sizeof(vcb); // size of vcb
	unsigned int vcbBlock = (vcbSize + blockSize - 1) / blockSize; // num of blocks of vcb
	
	unsigned int fatSize = sizeof(fat); // size of fat
	unsigned int fatBlock = (fatSize * numberOfBlocks + blockSize - 1) / blockSize; // num of blocks of fat

	unsigned int dirEntrySize = sizeof(dirEntry); // size of directory entry
	unsigned int dirEntryBlock = (dirEntrySize * INITENTRIES + blockSize - 1) / blockSize; // num of blocks of directory entries

	unsigned int totalBlock = vcbBlock + fatBlock + dirEntryBlock;

	fsvcb = malloc(vcbBlock * blockSize);

	// this is to check whether vcb is already init so we don't override disk
	LBAread(fsvcb, vcbBlock, 0);
	if(fsvcb->signature == SIGNATURE){
		// return 0;
	}

	// initializing vcb
	fsvcb->signature = SIGNATURE; // our magic number
	fsvcb->locationFreespace = vcbBlock; // end of vcb is vcbBlock - 1
	fsvcb->locationRootDir = vcbBlock + fatBlock; // end of freespace is vcbBlock + fatBlock - 1
	fsvcb->blockNum = numberOfBlocks; // num of block in LBA
	fsvcb->blockNumFree = numberOfBlocks - totalBlock; // num of block left available
	fsvcb->blockSize = blockSize; // size of each block

	// writing to disk
	LBAwrite((void *)fsvcb, vcbBlock, 0); // write vcb into disk
	
	// mark vcb on freespace map as used
	freespaceAllocateBlocks(0, vcbBlock);

	// mark freespace on freespace map as used
	freespaceAllocateBlocks(vcbBlock, vcbBlock + fatBlock);

	// mark rootDir on freespace map as used
	freespaceAllocateBlocks(vcbBlock + fatBlock, totalBlock);
	
	// initializing the rest of freespace
	freespaceAllocateBlocks(totalBlock, numberOfBlocks - totalBlock);

	dirEntry * rootDir = malloc(dirEntryBlock * blockSize);
	
	// initialize each directory entry structure to be in a known free state
	int totalEntry = INITENTRIES + ((dirEntryBlock * blockSize - dirEntrySize * INITENTRIES) / dirEntrySize);
	for (int i = 0; i < totalEntry; i++) {
		rootDir[i].name[0] = '\0'; // \0 means a directory entry is unused
	}

	// initialize "."
	time_t timer;
    strcpy(rootDir[0].name, ".");
    rootDir[0].size = dirEntrySize * totalEntry;
    rootDir[0].location = vcbBlock + fatBlock;
    rootDir[0].idOwner = 0;
    rootDir[0].idGroup = 0;
    timer = time(NULL);
    localtime_r(&timer, &rootDir[0].time);
    rootDir[0].type = 0; // 0 = directory

	// initialize ".."
    strcpy(rootDir[1].name, "..");
    rootDir[1].size = dirEntrySize * totalEntry;
    rootDir[1].location = vcbBlock + fatBlock;
    rootDir[1].idOwner = 0;
    rootDir[1].idGroup = 0;
    localtime_r(&timer, &rootDir[1].time);
    rootDir[1].type = 0; // 0 = directory 

	// write root directory into disk
	LBAwrite((void *)rootDir, dirEntryBlock, vcbBlock + fatBlock); 
	
	// free up resources
	free(rootDir);
	return 0;
	}
	
	
void exitFileSystem ()
	{
	// free up resources here?
	free(fsvcb);
	printf ("System exiting\n");
	}
