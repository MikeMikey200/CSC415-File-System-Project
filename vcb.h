#ifndef VCB_H
#define VCB_H

#include "fsLow.h"

#define SIGNATURE 2023

/*!
@struct		vcb
@abstract	contain file's volume information, it does not contain the volume location
			in the LBA as it is not necessary since it at block 0

@field		signature
			our magic num to prevent override the disk
@field 		locationFreespace
			location of block # for free space  
@field		locationRootDir
			location of block # for root dir  
@field		blockNum
			num of block in the volume
@field		blockNumFree
			num of free blocks in the volume
@field		blockSize
			size of each blocks typically 512
*/
typedef struct vcb {
	int signature;
	uint64_t locationFreespace;
	uint64_t locationRootDir;    
	uint64_t blockNum;
	uint64_t blockNumFree;
	uint64_t blockSize;
} vcb;

// making this a global variable so all can reference it
extern vcb *fsvcb;

#endif /* VCB_H */