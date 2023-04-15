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

extern fat *freespace;

// prototypes
/*!
@function 	freespaceInit
@param		freespace
			reference to the freespace map
@return 	nothing
*/
void freespaceInit();

/*!
@function 	freespaceFindFreeBlock
@abstract	freespaceAllocateBlocks helper function
			find the next free block in the freespace map
@param 		freespace
			reference to the freespace map
@return		the index of the next available unused block,
			0 if none are available return
*/
unsigned int freespaceFindFreeBlock();

/*!
@function 	freespaceAllocateBlocks
@param		freespace
			reference to the freespace map
@param		startLocation
			the start of file location
@param		blockNum
			specify the amount of block the file need
@return		amount of block allocated, -1 for error

*/
int freespaceAllocateBlocks(unsigned int startLocation, unsigned int blockNum);

/*!
@function	freespaceReleaseBlocks
@abstract	clear from startLocation to EOF
@param		freespace
			reference to the freespace map
@param		startLocation
			given file location
@return		amount of block released, -1 for error
*/
int freespaceReleaseBlocks(unsigned int startLocation);

#endif /* FAT_H */