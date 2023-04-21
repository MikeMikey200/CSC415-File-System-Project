/**************************************************************
* Class: CSC-415-03 Spring 2023
* Names: Minh Dang, Sabrina Diaz-Erazo, Trinity Godwin
* Student IDs: 921210261, 916931527, 918448783
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
#include "parsePath.h"

/* extern definitions */
vcb *fsvcb;
fat *freespace;
dirEntry *rootDir;
dirEntry *currentwd;

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	unsigned int vcbSize = sizeof(vcb); // size of vcb
	unsigned int vcbBlock = BLOCK(vcbSize, 1, blockSize); // num of blocks of vcb
	
	unsigned int fatSize = sizeof(fat); // size of fat
	unsigned int fatBlock = BLOCK(fatSize, numberOfBlocks, blockSize); // num of blocks of fat

	unsigned int dirEntrySize = sizeof(dirEntry); // size of directory entry
	unsigned int dirEntryBlock = (BLOCK(dirEntrySize, INITENTRIES, blockSize) * blockSize) / dirEntrySize; // num of blocks of directory entries

	unsigned int totalBlock = vcbBlock + fatBlock + dirEntryBlock;

	fsvcb = malloc(vcbBlock * blockSize);
	freespace = malloc(fatBlock * blockSize);
	currentwd = malloc(BLOCK(dirEntrySize, MAXENTRIES, blockSize) * blockSize);

	// this is to check whether vcb is already init so we don't override disk
	LBAread(fsvcb, vcbBlock, 0);
	if(fsvcb->signature == SIGNATURE){
		LBAread(freespace, fatBlock, vcbBlock);
		rootDir = malloc(dirEntryBlock * blockSize);
		LBAread(rootDir, dirEntryBlock, vcbBlock + fatBlock);

		fs_setcwd("\\");
		return 0;
	}

	// initializing vcb
	fsvcb->signature = SIGNATURE; // our magic number
	fsvcb->locationFreespace = vcbBlock; // end of vcb is vcbBlock - 1
	fsvcb->locationRootDir = vcbBlock + fatBlock; // end of freespace is vcbBlock + fatBlock - 1
	fsvcb->blockNum = numberOfBlocks; // num of block in LBA
	fsvcb->blockNumFree = numberOfBlocks; // num of block left available
	fsvcb->blockSize = blockSize; // size of each block

	LBAwrite(fsvcb, vcbBlock, 0); // write vcb into disk

	// initializing freespace to 0
	freespaceInit();
	
	// mark vcb on freespace map as used
	freespaceAllocateBlocks(0, vcbBlock);

	// mark freespace on freespace map as used
	freespaceAllocateBlocks(vcbBlock, fatBlock);

	rootDir = dirInit(INITENTRIES, NULL);
	
	fs_setcwd("\\");
	return 0;
	}
	
	
void exitFileSystem ()
	{
	// free up resources
	free(fsvcb);
	free(freespace);
	free(rootDir);
	free(currentwd);
	printf ("System exiting\n");
	}
