/**************************************************************
* Class: CSC-415-03 Spring 2023
* Names: Minh Dang, Sabrina Diaz-Erazo, Trinity Godwin
* Student IDs: 921210261, 916931527, 918448783
* GitHub Name: MikeMikey200
* Group Name: Wanna Cry
* Project: Basic File System
*
* File: b_io.c
*
* Description: Basic File System - Key File I/O Operations
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "b_io.h"
#include "fsFunction.h"
#include "parsePath.h"
#include "fsLow.h"
#include "vcb.h"
#include "fat.h"
#include "dir.h"

#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
	{
	/** TODO add al the information you need in the file control block **/
	fileInfo *file;
	char * buffer;		//holds the open file buffer
	int fileOffset;		//holds the current position in the file
	int location;		//location in the LBA
	int locationEnd;	//endlocation in the LBA
	int read;			//permission to read
	int write;			//permission to write
	int unusedBlock;	//amount of block that needed to be released
	int startup;		//the start of the file used for reading
	} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].buffer = NULL; //indicates a free fcbArray
		}
		
	startup = 1;
	}

//Method to get a free FCB element
b_io_fd b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].buffer == NULL)
			{
			return i;		//Not thread safe (But do not worry about it for this assignment)
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open (char * filename, int flags)
	{
	b_io_fd returnFd;

	//*** TODO ***:  Modify to save or set any information needed
	//
	//
		
	if (startup == 0) b_init();  //Initialize our system
	
	// check for error - all used FCB's
	if (returnFd == -1)
		return -1;

	dirEntry * dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);

	char str[MAXPATH];
	strcpy(str, filename);

	int index;

	//find the given filename
	if (str[0] == '\\') {
        index = parsePath(str, rootDir, dir);
    } else {
        index = parsePath(str, currentwd, dir);
    }

	strcpy(str, filename);
    char *saveptr, *tokenPrev;
    char *delim = "\\";
    char *token = strtok_r(str, delim, &saveptr);
    
	//grabbing the last token in the filename if given with \ character
    while (token != NULL) {
        tokenPrev = token;
        token = strtok_r(NULL, delim, &saveptr);
    }

	fileInfo *finfo;

	//index -1 mean not found in all the directories and the current directory
	if (index == -1) {
		//if it was given permission to create and write it will create a new file otherwise determined an error
		if (flags & O_CREAT && (flags & O_WRONLY || flags & O_RDWR)) {
			finfo = FileInit (tokenPrev, filename, dir);
		} else {
			free(dir);
			return -1;
		}
	} else {
		//grab file inodes from the parsepath directory parent
		finfo = GetFileInfo(tokenPrev, filename, dir);

		if (finfo == NULL) {
			free(dir);
			return -1;
		}
	}

	// get our own file descriptor
	returnFd = b_getFCB();	

	//intializing b_fcb structure
	fcbArray[returnFd].file = finfo;

	//release all blocks if truncating and setting filesize back to 0
	if (flags & O_TRUNC && (flags & O_WRONLY || flags & O_RDWR)) {
		fcbArray[returnFd].file->fileSize = 0;
		if (freespaceFindFreeBlock(fcbArray[returnFd].file->location) != 0) {
			freespaceReleaseBlocks(freespaceNextBlock(fcbArray[returnFd].file->location));
		}
	}

	fcbArray[returnFd].buffer = malloc(fsvcb->blockSize * sizeof(char));
	if (fcbArray[returnFd].buffer == NULL) {
		return -1;
	}

	//move the offset to the end of the file otherwise the start of the file
	if (flags & O_APPEND) {
		fcbArray[returnFd].fileOffset = fcbArray[returnFd].file->fileSize;
	} else {
		fcbArray[returnFd].fileOffset = 0;
	}
	fcbArray[returnFd].location = fcbArray[returnFd].file->location;
	fcbArray[returnFd].locationEnd = freespaceEndBlock(fcbArray[returnFd].file->location);

	//initializing the given permissions of the flag for reading and writing
	if (((flags + 1) & (O_RDONLY + 1)) || flags & O_RDWR) {
		fcbArray[returnFd].read = 1;
	} else {
		fcbArray[returnFd].read = 0;
	}
	if (flags & O_WRONLY || flags & O_RDWR) {
		fcbArray[returnFd].write = 1;
	} else {
		fcbArray[returnFd].write = 0;
	}
	fcbArray[returnFd].unusedBlock = 0;
	fcbArray[returnFd].startup = 0;
	
	free(dir);
	return (returnFd); // all set
	}


// Interface to seek function	
int b_seek (b_io_fd fd, off_t offset, int whence)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}

	/* from the lseek flags 
	SEEK_SET - sets the file offset to the absolute value of the offset parameter.
	SEEK_CUR - adds the value of offset to the current file offset.
	SEEK_END - sets the file offset to the size of the file plus offset.
	and if no valid flag return error
	*/
	if ((whence + 1) & (SEEK_SET + 1)) {
		fcbArray[fd].fileOffset = offset;
	} else if (whence & SEEK_CUR) {
		fcbArray[fd].fileOffset += offset;
	} else if (whence & SEEK_END) {
		fcbArray[fd].fileOffset = fcbArray[fd].file->fileSize + offset;
	} else {
		return -1;
	}
		
	return fcbArray[fd].fileOffset;
	}



// Interface to write function	
int b_write (b_io_fd fd, char * buffer, int count)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	if (fcbArray[fd].write != 1) {
		return -1;
	}

	//initially give the writer 50 blocks and as it uses up the blocks to below 10, it will give another 50 blocks.
	int block = fcbArray[fd].unusedBlock;
	if (block < 10) {
		block = freespaceAllocateBlocks(fcbArray[fd].locationEnd, WRITEBLOCK);
		if (block == 0)
			return 0;
		fcbArray[fd].locationEnd = freespaceEndBlock(fcbArray[fd].location);
		fcbArray[fd].unusedBlock += block;
	}

	int remainder, blockCnt, leftover, last, location;
	remainder = fsvcb->blockSize - fcbArray[fd].file->fileSize % fsvcb->blockSize;

	/*
	when count is higher than remainder do a split similar to b_read
	part 1 fill up current buffer and write it
	part 2 fill up as many blocks given from count amount caller's buffer directly to write
	part 3 refill the buffer with 0 offset with the remaining

	otherwise just fill the buffer with count
	*/
	if (remainder < count) {
		leftover = count - remainder;
		blockCnt = leftover / fsvcb->blockSize;
		last = leftover % fsvcb->blockSize;

		memcpy(fcbArray[fd].buffer + fcbArray[fd].file->fileSize % fsvcb->blockSize, buffer, remainder);
		fcbArray[fd].location = freespaceNextBlock(fcbArray[fd].location);
		LBAwrite(fcbArray[fd].buffer, 1, fcbArray[fd].location);

		fcbArray[fd].unusedBlock -= 1;

		for(int i = 0; i < blockCnt; i++) {
			fcbArray[fd].location = freespaceNextBlock(fcbArray[fd].location);
			LBAwrite(buffer + i * fsvcb->blockSize, 1, fcbArray[fd].location);
		}


		fcbArray[fd].unusedBlock -= blockCnt;

		if (last > 0) {
			memcpy(fcbArray[fd].buffer, buffer + remainder + blockCnt * fsvcb->blockSize, last);
		}

		fcbArray[fd].file->fileSize += remainder + blockCnt * fsvcb->blockSize + last;

		return remainder + blockCnt * fsvcb->blockSize + last;
	} else {
		memcpy(fcbArray[fd].buffer + fcbArray[fd].file->fileSize % fsvcb->blockSize, buffer, count);
		fcbArray[fd].file->fileSize += count;
		return count;
	}
	}



// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill 
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
int b_read (b_io_fd fd, char * buffer, int count)
	{

	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}

	if (fcbArray[fd].read != 1) {
		return -1;
	}

	if (count + fcbArray[fd].fileOffset > fcbArray[fd].file->fileSize) {
		count = fcbArray[fd].file->fileSize - fcbArray[fd].fileOffset;
	}

	// remainder in the block B_CHUNK_SIZE - current index
	int remainder = fsvcb->blockSize - fcbArray[fd].fileOffset % fsvcb->blockSize;
	int part1, part2, part3, leftover, blockCnt, read, location;
	
	if (count > remainder) {
		part1 = remainder;
		leftover = count - part1;
		blockCnt = leftover / fsvcb->blockSize;
		part2 = blockCnt * fsvcb->blockSize;
		part3 = leftover % fsvcb->blockSize;
	} else {
		part1 = count;
		part2 = 0;
		part3 = 0;
	}

	//remainder to fill up to
	if (part1 > 0) {
		//filling our buffer initially cause file just started
		if (fcbArray[fd].startup == 0) {
			if (fcbArray[fd].fileOffset == 0) {
				fcbArray[fd].location = freespaceNextBlock(fcbArray[fd].location);
				read = LBAread(fcbArray[fd].buffer, 1, fcbArray[fd].location);
			} else {
				read = LBAread(fcbArray[fd].buffer, 1, fcbArray[fd].location);
			}
			if (read != 1) {
				return 0;
			}
			fcbArray[fd].startup = 1;
		}
		memcpy(buffer, fcbArray[fd].buffer + fcbArray[fd].fileOffset % fsvcb->blockSize, part1);
		fcbArray[fd].fileOffset += part1;
	}

	//leftover blocks
	if (part2 > 0) {
		int temp = 0;
		for (int i = 0; i < blockCnt; i++) {
			location = freespaceNextBlock(fcbArray[fd].location);
			fcbArray[fd].location = location;
			read = LBAread(buffer + part1 + temp, 1, location);
			temp += read * fsvcb->blockSize;
		}
		part2 = temp;
		fcbArray[fd].fileOffset += part2;
	}

	//remaining to refill
	if (part3 > 0) {
		location = freespaceNextBlock(fcbArray[fd].location);
		fcbArray[fd].location = location;
		read = LBAread(fcbArray[fd].buffer, 1, location);
		read = read * fsvcb->blockSize;
		if (read < part3) {
			part3 = read;
		}

		if (part3 > 0) {
			memcpy(buffer + part1 + part2, fcbArray[fd].buffer + fcbArray[fd].fileOffset % fsvcb->blockSize, part3);
			fcbArray[fd].fileOffset += part3;
		}
	}

	return part1 + part2 + part3;
	}
	
// Interface to Close the file	
int b_close (b_io_fd fd)
	{
		if (fcbArray[fd].buffer == NULL) {
			return -1;
		}

		//write the last block and free up the unused blocks
		if (fcbArray[fd].write == 1) {
			LBAwrite(fcbArray[fd].buffer, 1, freespaceNextBlock(fcbArray[fd].location));
			fcbArray[fd].unusedBlock -= 1;
			int begin = fcbArray[fd].file->location;
			int total = freespaceTotalAllocated(begin) - fcbArray[fd].unusedBlock;
			while (total != 0) {
				begin = freespaceNextBlock(begin);
				total -= 1;
			}
			freespaceReleaseBlocks(begin);
			dirEntry *dir = malloc(BLOCK(sizeof(dirEntry), MAXENTRIES, fsvcb->blockSize) * fsvcb->blockSize);
			int index = parsePath(fcbArray[fd].file->pathname, currentwd, dir);
			dir[index].size = fcbArray[fd].file->fileSize;
			LBAwrite(dir, (dir[0].size + fsvcb->blockSize - 1) / fsvcb->blockSize, dir[0].location);
			dirEntryLoad(currentwd, currentwd);
			free(dir);
		}
		
		//free the fd in the fcbArray
		free(fcbArray[fd].buffer);
		fcbArray[fd].buffer = NULL;
		fcbArray[fd].file = NULL;
		
		return 0;
	}
