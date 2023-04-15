#include "fat.h"
#include "vcb.h"

/*!
@function 	freespaceFindFreeBlock
@abstract	find the next free block in freespace map using FAT method
@param 		freespace
			indicate pass by reference of freespace map
@param 		location
			indicate the starting location of the file so that the reference of next is pointing to the new 
			allocated file 0 indicate it doesn't belong to any file, !0 indicate it belong to a file
@return		the index of the next available unused block,
			0 if none are available return
*/
unsigned int freespaceFindFreeBlock(fat *freespace, unsigned int startLocation) {
	unsigned int i = 0;

	// mean there is an allocated file here already recursively go to freespace.next until reaches 0
	if (startLocation > 0){
		while (freespace[startLocation].next != 0) {
			startLocation = freespace[startLocation].next;
			// error value
			if (startLocation == freespace[startLocation].next) {
				return 0;
			}
		}
		i = startLocation;
	}
	
	for(; i < fsvcb->blockNum; i++) {
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

/*!
@function 	freespaceReleaseBlock
@abstract 	chain release the block from the given location of the file
@param 		freespace
			indicates pass by reference of freespace map
@param 		startLocation
			indicate the starting location of the file so that the reference of next is pointing to the 
			new allocated file must pass in a value !0
			it dictate where it will release the file from a start location
			it doesn't have to be the start of the file and can be starting from the middle or near the end of the file
@return		1 indicate successfully released, 
			0 error input value for startLocation
*/
unsigned int freespaceReleaseBlock(fat *freespace, unsigned int startLocation){
	unsigned int i;

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

void freespaceAllocateBlocks(unsigned int startLocation, unsigned int blockNum) {
	unsigned int fatBlock = (fsvcb->blockNum * sizeof(fat) + fsvcb->blockSize - 1) / fsvcb->blockSize;
	fat * freespace = malloc(fatBlock * fsvcb->blockSize);
	LBAread(freespace, fatBlock, fsvcb->locationFreespace);

	for(unsigned int i = 0; i < blockNum; i++) {
		startLocation = freespaceFindFreeBlock(freespace, startLocation);
	}

	LBAwrite(freespace, fatBlock, fsvcb->locationFreespace);
	free(freespace);
}