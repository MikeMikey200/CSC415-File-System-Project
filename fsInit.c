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

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	uint64_t vcbSize = sizeof(vcb); // size of vcb
	uint64_t vcbBlock = (vcbSize + blockSize - 1) / blockSize; // num of blocks of vcb
	
	uint64_t fatSize = sizeof(fat); // size of fat
	uint64_t fatBlock = (fatSize * numberOfBlocks + blockSize - 1) / blockSize; // num of blocks of fat

	uint64_t dirEntrySize = sizeof(dirEntry); // size of directory entry
	uint64_t dirEntryBlock = (dirEntrySize * INITENTRIES + blockSize - 1) / blockSize; // num of blocks of directory entries

	fsvcb = malloc(vcbBlock * blockSize);
	rootDir = malloc(dirEntryBlock * blockSize);

	LBAread(fsvcb, vcbBlock, 0);

	// this is to check whether vcb is already init so we don't override disk
	if(fsvcb->signature == SIGNATURE){
		printf("%d\n", fsvcb->signature);
		// load the rootDir as a global var
		LBAread(rootDir, dirEntryBlock, vcbBlock + fatBlock);
		//return 0;
	}

	// debug
	// printf("Size of DE: %ld\n", dirEntrySize);
	// debug
	// printf("Number of DE blocks: %ld\n", dirEntryBlock);
	
	// initialize each directory entry structure to be in a known free state
	for (int i = 0; i < INITENTRIES + 1; i++) {
		rootDir[i].name[0] = '\0'; // \0 means a directory entry is unused
	}

	// initialize "."
	time_t timer;
    rootDir[0].name[0] = '.';
    rootDir[0].name[1] = '\0';
    rootDir[0].size = sizeof(dirEntry) * (INITENTRIES + 1);
    rootDir[0].location = fsvcb->locationRootDir;
    rootDir[0].idOwner = 0;
    rootDir[0].idGroup = 0;
    timer = time(NULL);
    localtime_r(&timer, &rootDir[0].time);
    rootDir[0].type = 0; // 0 = directory

	// initialize ".."
    rootDir[1].name[0] = '.';
    rootDir[1].name[1] = '.';
    rootDir[1].name[2] = '\0';
    rootDir[1].size = sizeof(dirEntry) * (INITENTRIES + 1);
    rootDir[1].location = fsvcb->locationRootDir;
    rootDir[1].idOwner = 0;
    rootDir[1].idGroup = 0;
    localtime_r(&timer, &rootDir[1].time);
    rootDir[1].type = 0; // 0 = directory 

	fat *freespace = malloc(fatBlock * blockSize);

	uint64_t totalBlock = vcbBlock + fatBlock + dirEntryBlock;

	// mark vcb on freespace map as used
	for(uint64_t i = 0; i < vcbBlock; i++){
		freespace[i].used = 1;
		freespace[i].next = i + 1;
	}

	freespace[vcbBlock - 1].next = 0;

	// mark freespace on freespace map as used
	for(uint64_t i = vcbBlock; i < vcbBlock + fatBlock; i++){
		freespace[i].used = 1;
		freespace[i].next = i + 1;
	}

	freespace[vcbBlock + fatBlock - 1].next = 0;

	// mark rootDir on freespace map as used
	for(uint64_t i = vcbBlock + fatBlock; i < totalBlock; i++){
		freespace[i].used = 1;
		freespace[i].next = i + 1;
	}

	freespace[totalBlock - 1].next = 0;
	
	// initializing the rest of freespace
	for(uint64_t i = totalBlock; i < numberOfBlocks; i++){
		freespace[i].used = 0;
		freespace[i].next = 0;
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
	LBAwrite((void *)freespace, fatBlock, vcbBlock); // write freespace into disk
	LBAwrite((void *)rootDir, dirEntryBlock, vcbBlock + fatBlock); // write root directory into disk
	
	// free up resources
	free(freespace);
	return 0;
	}
	
	
void exitFileSystem ()
	{
	// free up resources here?
	free(fsvcb);
	free(rootDir);
	printf ("System exiting\n");
	}
