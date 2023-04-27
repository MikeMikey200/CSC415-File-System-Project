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
	int fileOffset;		//holds the current position in the buffer
	int location;
	int locationEnd;
	int read;
	int write;
	int unusedBlock;
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

	if (str[0] == '\\') {
        index = parsePath(str, rootDir, dir);
    } else {
        index = parsePath(str, currentwd, dir);
    }

	strcpy(str, filename);
    char *saveptr, *tokenPrev;
    char *delim = "\\";
    char *token = strtok_r(str, delim, &saveptr);
    
    while (token != NULL) {
        tokenPrev = token;
        token = strtok_r(NULL, delim, &saveptr);
    }

	fileInfo *finfo;

	if (index == -1) {
		if (flags & O_CREAT && (flags & O_WRONLY || flags & O_RDWR)) {
			finfo = FileInit (tokenPrev, filename, dir);
		} else {
			free(dir);
			return -1;
		}
	} else {
		finfo = GetFileInfo(tokenPrev, filename, dir);

		if (finfo == NULL) {
			free(dir);
			return -1;
		}
	}

	// get our own file descriptor
	returnFd = b_getFCB();	

	fcbArray[returnFd].file = finfo;
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
	if (flags & O_APPEND) {
		fcbArray[returnFd].fileOffset = fcbArray[returnFd].file->fileSize;
	} else {
		fcbArray[returnFd].fileOffset = 0;
	}
	fcbArray[returnFd].location = fcbArray[returnFd].file->location;
	fcbArray[returnFd].locationEnd = freespaceEndBlock(fcbArray[returnFd].file->location);
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

	printf("b_open name %s\n", filename);
	printf("b_open locationEnd location = %d %d\n", fcbArray[returnFd].locationEnd, fcbArray[returnFd].location);
	printf("b_open file->location = %d\n", fcbArray[returnFd].file->location);
	
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

	// temporarily ignore this function professor cmds is not using this, we'll be using this somewhere
	// changing the offset
	// reload your buffer
		
	return (0); //Change this
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
			memcpy(fcbArray[fd].buffer, buffer + leftover + blockCnt * fsvcb->blockSize, last);
		}

		fcbArray[fd].file->fileSize += remainder + blockCnt * fsvcb->blockSize + last;

		return remainder + blockCnt * fsvcb->blockSize + last;
	} else {
		memcpy(fcbArray[fd].buffer + fcbArray[fd].file->fileSize % fsvcb->blockSize, buffer, count);
		fcbArray[fd].file->fileSize += count;
		return count;
	}
	

	/*
	if (count + fcbArray[fd].fileOffset > fcbArray[fd].file->fileSize)
		count = fcbArray[fd].file->fileSize - fcbArray[fd].fileOffset;

	int currentBlock = fcbArray[fd].fileOffset / fsvcb->blockSize;
	int remainder = (fsvcb->blockSize - fcbArray[fd].fileOffset) % fsvcb->blockSize;
	int part1 = 0, part2 = 0, part3 = 0;
	int leftOver, blockCount, write;
	
	if(count > remainder)
	{
		part1 = remainder;
		leftOver = count - part1;
		blockCount = leftOver / fsvcb->blockSize;
		part2 = blockCount * fsvcb->blockSize;
		part3 = leftOver - part2;
	} else {
		part1 = count;
	}

	if (part1 > 0)
	{
		memcpy(fcbArray[fd].buffer + fcbArray[fd].fileOffset % fsvcb->blockSize, buffer, part1);
		fcbArray[fd].fileOffset += part1;
	}

	if(part2 > 0)
	{
		int temp = 0;
		for (int i = 0; i < blockCount; i++) {
			fcbArray[fd].location = freespaceNextBlock(fcbArray[fd].location);
			write = LBAwrite(buffer + part1 + temp, 1, fcbArray[fd].location);
			temp += write * fsvcb->blockSize;
		}
		part2 = temp;
		fcbArray[fd].fileOffset += part2;
	}
	
	if (part3 > 0) 
	{
		write = LBAwrite(fcbArray[fd].buffer, 1, fcbArray[fd].location);
		write = write * fsvcb->blockSize;

		if (write < part3)
			part3 = write;

		if (part3 > 0) 
		{
			memcpy(fcbArray[fd].buffer + fcbArray[fd].fileOffset % fsvcb->blockSize, buffer + part1 + part2, part3);
			fcbArray[fd].fileOffset += part3;
		}
	}

	if (freespaceFindFreeBlock(fcbArray[fd].location) != 0)
			freespaceReleaseBlocks(freespaceNextBlock(fcbArray[fd].location));

	fcbArray[fd].locationEnd = freespaceEndBlock(fcbArray[fd].location);
	fcbArray[fd].totalAllocated = freespaceTotalAllocated(freespaceNextBlock(fcbArray[fd].location));

	return part1 + part2 + part3;
	*/
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

	if (part1 > 0) {
		if (fcbArray[fd].fileOffset == 0) {
			location = freespaceNextBlock(fcbArray[fd].location);
			fcbArray[fd].location = location;
			read = LBAread(fcbArray[fd].buffer, 1, location);
			if (read != 1) {
				return 0;
			}
		}
		memcpy(buffer, fcbArray[fd].buffer + fcbArray[fd].fileOffset % fsvcb->blockSize, part1);
		fcbArray[fd].fileOffset += part1;
	}

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

		/*
		writing last block
		flush buffer?
		*/

		if (fcbArray[fd].write == 1) {
			LBAwrite(fcbArray[fd].buffer, 1, freespaceNextBlock(fcbArray[fd].locationEnd));
			fcbArray[fd].unusedBlock -= 1;
			int begin = fcbArray[fd].file->location;
			int total = freespaceTotalAllocated(begin) - fcbArray[fd].unusedBlock;
			printf("b_close total %d\n", total);
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
		
		free(fcbArray[fd].buffer);
		fcbArray[fd].buffer = NULL;
		fcbArray[fd].file = NULL;
		
		return 0;
	}
