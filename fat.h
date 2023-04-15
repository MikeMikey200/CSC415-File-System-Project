#ifndef FAT_H
#define FAT_H

/*!
@struct 	fat
@abstract	structure implementation of the File Allocation Table method (FAT)

@field		used 
			indicate 0 is free, 1 is used
@field		next
			indicate next location !0 is location, 0 is EOF
*/
typedef struct fat {
	unsigned int used;
	unsigned int next;
} fat;

// prototypes
void freespaceAllocateBlocks(unsigned int startLocation, unsigned int blockNum);

#endif /* FAT_H */