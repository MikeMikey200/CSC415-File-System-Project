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
/*!
@function 	freespaceInit
@param		freespace
			reference to the freespace map
@return 	nothing
*/
void freespaceInit(fat *freespace);

/*!
@function 	freespaceAllocateBlocks
@param		freespace
			reference to the freespace map
@param		startLocation
			the start of filelocation
@param		blockNum
			specify the amount of block the file need
@return		amount of block allocated, -1 for error

*/
int freespaceAllocateBlocks(fat *freespace, unsigned int startLocation, unsigned int blockNum);

/*!
@function	freespaceReleaseBlocks
@abstract	clear from startLocation to EOF
@param		freespace
			reference to the freespace map
@param		startLocation
			given file location
@return		amount of block released, -1 for error
*/
int freespaceReleaseBlocks(fat *freespace, unsigned int startLocation);

#endif /* FAT_H */