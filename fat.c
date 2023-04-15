#include "fat.h"
#include "vcb.h"

/*!
@function 	freespaceFindFreeBlock
@abstract	freespaceAllocateBlocks helper function
			find the next free block in the freespace map
@param 		freespace
			reference to the freespace map
@return		the index of the next available unused block,
			0 if none are available return
*/
unsigned int freespaceFindFreeBlock(fat *freespace) {
	for(unsigned int i = 0; i < fsvcb->blockNum; i++) {
		if (freespace[i].used == 0) {
			return i;
		}
	}

	return 0;
}

int freespaceAllocateBlocks(fat *freespace, unsigned int startLocation, unsigned int blockNum) {
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

	return i - 1;
}

int freespaceReleaseBlocks(fat *freespace, unsigned int startLocation) {
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

	return total + 1;
}

void freespaceInit(fat *freespace) {
	for (unsigned int i = 0; i < fsvcb->blockNum; i++){
		freespace[i].next = 0;
		freespace[i].used = 0;
	}
}