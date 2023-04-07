#ifndef FAT_H
#define FAT_H

#include "fsLow.h"

/*!
@struct 	fat
@abstract	structure implementation of the File Allocation Table method (FAT)

@field		used 
			indicate 0 is free, 1 is used
@field		next
			indicate next location !0 is location, 0 is EOF
*/
typedef struct fat {
	uint64_t used;
	uint64_t next;
} fat;

// fat *freespace;

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
uint64_t freespaceFindFreeBlock(fat *freespace, uint64_t startLocation) {
	uint64_t i = 0;

	// mean there is an allocated file here already recursively go to freespace.next until reaches -1
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
uint64_t freespaceReleaseBlock(fat *freespace, uint64_t startLocation){
	uint64_t i;

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

// example of how to use freespaceFindFreeBlock(fat *freespace, uint64_t startLocation)

/*
	printf("locationFreespace: %ld\n", fsvcb->locationFreespace);
	printf("locationRootDir: %ld\n", fsvcb->locationRootDir);
	uint64_t num;
	num = freespaceFindFreeBlock(freespace, 0);
	printf("num: %ld\n", num);
	printf("num %ld free.next: %ld\n", num - 1, freespace[num - 1].next);
	num = freespaceFindFreeBlock(freespace, num);
	printf("num: %ld\n", num);
	printf("num %ld free.next: %ld\n", num - 1, freespace[num - 1].next);
	num = freespaceFindFreeBlock(freespace, num);
	printf("num: %ld\n", num);
	printf("num %ld free.next: %ld\n", num - 1, freespace[num - 1].next);

	// example of how to use freespaceReleaseBlock(fat *freespace, uint64_t startLocation)
	freespaceReleaseBlock(freespace, num - 2);
	printf("num %ld free.used: %ld\n", num - 2, freespace[num - 2].used);
	printf("num %ld free.next: %ld\n", num - 2, freespace[num - 2].next);	
	printf("num %ld free.used: %ld\n", num - 1, freespace[num - 1].used);
	printf("num %ld free.next: %ld\n", num - 1, freespace[num - 1].next);
	printf("num %ld free.used: %ld\n", num, freespace[num].used);
	printf("num %ld free.next: %ld\n", num, freespace[num].next);
*/

#endif /* FAT_H */