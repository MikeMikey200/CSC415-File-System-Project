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
		//LBAread(rootDir, dirEntryBlock, vcbBlock + fatBlock);
		//return 0; uncomment this whenever rootDir init is finished
	}

	// initialize each directory entry structure to be in a known free state
	for (int i = 0; i < INITENTRIES; i++) {
		rootDir[i].name[0] = 0; // 0 means a directory entry is unused
	}

	// TODO: initialize "." and ".." in rootDir[]

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
	fsvcb->signature = SIGNATURE; // refer to #define above
	fsvcb->locationFreespace = vcbBlock; // end of vcb is vcbBlock - 1
	fsvcb->locationRootDir = vcbBlock + fatBlock; // end of freespace is vcbBlock + fatBlock - 1
	fsvcb->blockNum = numberOfBlocks; // num of block in LBA
	fsvcb->blockNumFree = numberOfBlocks - totalBlock; 
	fsvcb->blockSize = blockSize; // size of each block

	// writing to disk
	LBAwrite((void *)fsvcb, vcbBlock, 0); // start from 0
	LBAwrite((void *)freespace, fatBlock, vcbBlock); // start from vcbBlock
	LBAwrite((void *)rootDir, dirEntryBlock, vcbBlock + fatBlock); // start from vcbBlock + fatBlock
	
	// free up resources
	free(fsvcb);
	free(freespace);
	free(rootDir);
	return 0;
	}
	
	
void exitFileSystem ()
	{
	// free up resources here?
	printf ("System exiting\n");
	}
