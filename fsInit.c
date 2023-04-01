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

#define SIGNATURE 1234

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

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	int vcbSize = sizeof(vcb); // size of vcb
	int vcbBlock = (vcbSize + MINBLOCKSIZE - 1) / MINBLOCKSIZE; // location of vcb
	vcb *fsvcb = malloc(vcbSize);
	LBAread(fsvcb, 1, 1);
	if(fsvcb->signature == SIGNATURE)
		return 0;

	// initializing
	// based off of lecture, partition/boot record is at block 0, and VCB is at block 1
	fsvcb->signature = SIGNATURE; // please change to a value that we want
	fsvcb->locationFreespace = vcbBlock + 1; // temporary value
	fsvcb->locationRootDir = vcbBlock + 2; // temporary value
	fsvcb->blockNum = numberOfBlocks; // num of block in LBA
	fsvcb->blockNumFree = numberOfBlocks - (vcbBlock + 2); // temporary value
	fsvcb->blockSize = blockSize; // size of each block
	LBAwrite((void *)fsvcb, 1, 1); // size of vcb usually should be smaller MINBLOCKSIZE to be fit in a single block 
			               // so we don't have to worry about vcb overflowing to multiple blocks

	free(fsvcb);
	return 0;
	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}
