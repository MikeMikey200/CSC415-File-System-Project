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
	uint64_t locationFreespace; // location of block # for free space  
	uint64_t locationRootDir; // location of block # for root dir       
	uint64_t blockNum; // num of block in the volume
	uint64_t blockNumFree; // num of free blocks in the freespace
	uint64_t blockSize; // size of each blocks typically 512
} vcb;

// contains FAT implementation
typedef struct fat {
	uint64_t used; // indicate 0 is free, 1 is used
	uint64_t next; // indicate next location > 0 is location, 0 is EOF
} fat;

// this is an allocation function for freespace using FAT method
// startLocation indicate if not in freespace 0, if exist in freespace > 0
// return an index, otherwise 0 indicate an error
uint64_t freespaceFindFreeBlock(fat *freespace, uint64_t numberOfBlocks, uint64_t startLocation) {
	uint64_t i = 0;

	// mean there is an allocated file here already recursively go to freespace.next until reaches -1
	if (startLocation > 0){
		while (freespace[startLocation].next != 0) {
			startLocation = freespace[startLocation].next;
			if (startLocation == freespace[startLocation].next) {
				return 0;
			}
		}
		i = startLocation;
	}
	
	for(; i < numberOfBlocks; i++) {
		if (freespace[i].used == 0) {
			if (startLocation != 0){
				freespace[startLocation].next = i;
			}
			freespace[i].used = 1;
			return i;
		}
	}

	return 0;
}

// this is a release function for freespace using FAT method
// startLocation indicate starting block# to be free, has to be > -1
// return 1 indicate good, return 0 indicate error for startLocation
uint64_t freespaceReleaseBlock(fat *freespace, uint64_t numberOfBlocks, uint64_t startLocation){
	uint64_t i;
	uint64_t start;

	// must have a start location
	if (startLocation == 0) {
		return 0;
	}

	while(freespace[startLocation].next != 0){
		i = startLocation;
		freespace[startLocation].used = 0;
		startLocation = freespace[startLocation].next;
		freespace[i].next = 0;
	}

	freespace[startLocation].used = 0;

	return 1;
}

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	uint64_t vcbSize = sizeof(vcb); // size of vcb
	uint64_t vcbBlock = (vcbSize + blockSize - 1) / blockSize; // num of blocks of vcb
	vcb *fsvcb = malloc(vcbSize);

	LBAread(fsvcb, vcbBlock, 0);

	// this is to check whether vcb is already init so we don't override disk
	if(fsvcb->signature == SIGNATURE){
		printf("%d\n", fsvcb->signature);
		//return 0; uncomment this whenever rootDir init is finished
	}

	uint64_t fatSize = sizeof(fat); // size of fat
	uint64_t fatBlock = (fatSize * numberOfBlocks + blockSize - 1) / blockSize; // num of blocks of fat
	fat *freespace = malloc(fatSize * numberOfBlocks);

	// initializing freespace
	for(uint64_t i = 0; i < numberOfBlocks; i++){
		freespace[i].used = 0;
		freespace[i].next = 0;
	}

	uint64_t rootBlock = 1; // please fill this one with rootDir block size

	/* TODO:
	fill in intialization of rootDir and making of rootDir here
	*/

	uint64_t totalBlock = vcbBlock + fatBlock + rootBlock;

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
	
	// initializing vcb
	fsvcb->signature = SIGNATURE; // refer to #define above
	fsvcb->locationFreespace = vcbBlock; // end of vcb is vcbBlock - 1
	fsvcb->locationRootDir = vcbBlock + fatBlock; // end of freespace is vcbBlock + fatBlock - 1
	fsvcb->blockNum = numberOfBlocks; // num of block in LBA
	fsvcb->blockNumFree = numberOfBlocks - totalBlock; 
	fsvcb->blockSize = blockSize; // size of each block

	// writing to disk
	LBAwrite((void *)fsvcb, vcbBlock, 0); // size of vcb usually should be smaller blockSize to be fit in a single block 
			       	              // so we don't have to worry about vcb overflowing to multiple blocks
	LBAwrite((void *)freespace, fatBlock, vcbBlock); // start from vcbBlock
	//LBAwrite((void *)rootDir, rootBlock, vcbBlock + fatBlock); uncomment this when rootDir is finished

	// example of how to use freespaceFindFreeBlock(fat *freespace, uint64_t numberOfBlocks, uint64_t startLocation)
	printf("locationFreespace: %ld\n", fsvcb->locationFreespace);
	printf("locationRootDir: %ld\n", fsvcb->locationRootDir);
	uint64_t num;
	num = freespaceFindFreeBlock(freespace, numberOfBlocks, 0);
	printf("num: %ld\n", num);
	printf("num %ld free.next: %ld\n", num - 1, freespace[num - 1].next);
	num = freespaceFindFreeBlock(freespace, numberOfBlocks, num);
	printf("num: %ld\n", num);
	printf("num %ld free.next: %ld\n", num - 1, freespace[num - 1].next);
	num = freespaceFindFreeBlock(freespace, numberOfBlocks, num);
	printf("num: %ld\n", num);
	printf("num %ld free.next: %ld\n", num - 1, freespace[num - 1].next);

	// example of how to use freespaceReleaseBlock(fat *freespace, uint64_t numberOfBlocks, uint64_t startLocation)
	freespaceReleaseBlock(freespace, numberOfBlocks, num - 2);
	printf("num %ld free.used: %ld\n", num - 2, freespace[num - 2].used);
	printf("num %ld free.next: %ld\n", num - 2, freespace[num - 2].next);	
	printf("num %ld free.used: %ld\n", num - 1, freespace[num - 1].used);
	printf("num %ld free.next: %ld\n", num - 1, freespace[num - 1].next);
	printf("num %ld free.used: %ld\n", num, freespace[num].used);
	printf("num %ld free.next: %ld\n", num, freespace[num].next);
	
	// free up resources
	free(fsvcb);
	free(freespace);
	return 0;
	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}
