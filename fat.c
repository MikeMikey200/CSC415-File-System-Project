#include <stdlib.h>

#include "fsLow.h"
#include "fat.h"
#include "vcb.h"

unsigned int freespaceFindFreeBlock() {
	for(unsigned int i = 0; i < fsvcb->blockNum; i++) {
		if (freespace[i].used == 0) {
			return i;
		}
	}

	return 0;
}

int freespaceAllocateBlocks(unsigned int startLocation, unsigned int blockNum) {
	unsigned int num;
	unsigned int i;

	if (startLocation >= fsvcb->blockNum)
		return -1;

	num = freespaceFindFreeBlock(freespace);

	for(i = 0; i < blockNum; i++) {
		startLocation = num;
		freespace[num].used = 1;
		num = freespaceFindFreeBlock(freespace);
		freespace[startLocation].next = num;
	}

	freespace[startLocation].next = 0;
	fsvcb->blockNumFree -= i - 1;

	LBAwrite(fsvcb, fsvcb->locationFreespace, 0);
	LBAwrite(freespace, fsvcb->locationRootDir - fsvcb->locationFreespace, fsvcb->locationFreespace);

	return i - 1;
}

int freespaceReleaseBlocks(unsigned int startLocation) {
	unsigned int num;
	unsigned int i;
	unsigned int total = 0;

	if (startLocation >= fsvcb->blockNum) {
		return -1;
	}

	for(i = startLocation; freespace[i].next != 0;) {
		num = i;
		i = freespace[i].next;
		total++;
		freespace[num].used = 0;
		freespace[num].next = 0;
	}	

	freespace[i].used = 0;
	fsvcb->blockNumFree += total + 1;

	LBAwrite(fsvcb, fsvcb->locationFreespace, 0);
	LBAwrite(freespace, fsvcb->locationRootDir - fsvcb->locationFreespace, fsvcb->locationFreespace);

	return total + 1;
}

void freespaceInit() {
	for (unsigned int i = 0; i < fsvcb->blockNum; i++){
		freespace[i].next = 0;
		freespace[i].used = 0;
	}
}