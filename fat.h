#ifndef FAT_H
#define FAT_H

#include "fsLow.h"

// contains FAT implementation
typedef struct fat {
	uint64_t used; // indicate 0 is free, 1 is used
	uint64_t next; // indicate next location > 0 is location, 0 is EOF
} fat;

fat *freespace;

// this is an allocation function for freespace using FAT method
// startLocation indicate if not in freespace 0, if exist in freespace > 0
// return an index, otherwise 0 indicate an error
uint64_t freespaceFindFreeBlock(fat *freespace, uint64_t numberOfBlocks, uint64_t startLocation) {
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

/*
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
*/

#endif /* FAT_H */