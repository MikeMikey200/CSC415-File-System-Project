/**************************************************************
* Class: CSC-415-03 Spring 2023
* Names: Minh Dang, Sabrina Diaz-Erazo, Trinity Godwin, Sungmu Cho
* Student IDs: 921210261, 916931527, 918448783, 921791166
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

#define SIGNATURE 1234 // please change to a value that we want

// contains volume details
typedef struct vcb {
	// no need location for vcb itself, it is at 0 in the LBA
	int signature; // an integer of a magic number or a string like ELF 
	unsigned int locationFreespace; // location of block # for free space  
	unsigned int locationRootDir; // location of block # for root dir       
	unsigned int blockNum; // num of block in the volume
	unsigned int blockNumFree; // num of free blocks in the freespace
	unsigned int blockSize; // size of each blocks typically 512
} vcb;

typedef struct fat {
	unsigned int used; // indicate 0 is free, 1 is used
	unsigned int next; // indicate next location greater than -1 is location, -1 is EOF
} fat;

// returning block location if there is free block, otherwise -1
int freespaceFindFreeBlock(fat *freespace, uint64_t numberOfBlocks) {
	for(unsigned int i = 0; i < numberOfBlocks; i++) {
		if(freespace[i].used == 0) {
			freespace[i].used = 1;
			return i;
		}
	}
	return -1;
}

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	int vcbSize = sizeof(vcb); // size of vcb
	int vcbBlock = (vcbSize + blockSize - 1) / blockSize; // num of blocks of vcb
	vcb *fsvcb = malloc(vcbSize);
	//LBAread(fsvcb, vcbBlock, 0);
	if(fsvcb->signature == SIGNATURE){
		//return 0; uncomment this whenever rootDir init is finished
	}

	int fatSize = sizeof(fat); // size of fat
	int fatBlock = (fatSize * numberOfBlocks + blockSize - 1) / blockSize; // num of blocks of fat
	fat *freespace = malloc(fatSize * numberOfBlocks);

	// initializing freespace
	for(unsigned int i = 0; i < numberOfBlocks; i++){
		freespace[i].used = 0;
		freespace[i].next = -1;
	}

	int rootBlock = 1; // please fill this one with rootDir block size

	/* TODO:
	fill in intialization of rootDir and making of rootDir here
	*/

	int totalBlock = vcbBlock + fatBlock + rootBlock;

	// mark vcb on freespace map as used
	for(unsigned int i = 0; i < vcbBlock; i++){
		freespace[i].used = 1;
		freespace[i].next = i + 1;
	}

	freespace[vcbBlock - 1].next = -1;

	// mark freespace on freespace map as used
	for(unsigned int i = vcbBlock; i < vcbBlock + fatBlock; i++){
		freespace[i].used = 1;
		freespace[i].next = i + 1;
	}

	freespace[vcbBlock + fatBlock - 1].next = -1;

	// mark rootDir on freespace map as used
	for(unsigned int i = vcbBlock + fatBlock; i < totalBlock; i++){
		freespace[i].used = 1;
		freespace[i].next = i + 1;
	}

	freespace[totalBlock - 1].next = -1;

	
	
	// initializing vcb
	fsvcb->signature = SIGNATURE; // refer to #define above
	fsvcb->locationFreespace = vcbBlock; // end of vcb is vcbBlock - 1
	fsvcb->locationRootDir = vcbBlock + fatBlock; // end of rootDir is vcbBlock + fatBlock - 1
	fsvcb->blockNum = numberOfBlocks; // num of block in LBA
	fsvcb->blockNumFree = numberOfBlocks - totalBlock; 
	fsvcb->blockSize = blockSize; // size of each block

	LBAwrite((void *)fsvcb, vcbBlock, 0); // size of vcb usually should be smaller blockSize to be fit in a single block 
			       	              // so we don't have to worry about vcb overflowing to multiple blocks
	LBAwrite((void *)freespace, fatBlock, vcbBlock); // start from vcbBlock
	//LBAwrite((void *)rootDir, rootBlock, vcbBlock + fatBlock); uncomment this when rootDir is finished
	
	free(fsvcb);
	free(freespace);
	
	return 0;
	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}
