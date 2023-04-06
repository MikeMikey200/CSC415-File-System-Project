#ifndef VCB_H
#define VCB_H

#include "fsLow.h"

#define SIGNATURE 2023

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

#endif /* VCB_H */