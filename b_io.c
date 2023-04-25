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
	int bufferOffset;		//holds the current position in the buffer
	int fileOffset;
	int blockNum;
	int location;
	int flag;       //specifies if file is read only (0), write only (1), or read/write (2)
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
	str[strlen(filename)] = '\0';

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
	dirEntry *file;

	if (index == -1) {
		if (flags & O_CREAT) {
			file = FileInit (tokenPrev, dir, finfo);
		} else {
			free(dir);
			return -1;
		}

		if (file == NULL) {
			free(dir);
			free(finfo);
			return -1;
		}
	} else {
		finfo = GetFileInfo(tokenPrev, dir);

		if (finfo == NULL) {
			free(dir);
			return -1;
		}
	}

	// get our own file descriptor
	returnFd = b_getFCB();				

	// different cases of flag options
    // O_RDONLY, 0
    if (flags & O_RDONLY) {
        // check if create and/or truncate flag is listed
        if ((flags & O_CREAT) || (flags & O_TRUNC)) {
            // error
            // exit out of if statement
        }
        // keep O_RDONLY flag inside FCB
        fcbArray[returnFd].flag = 0;

    }

    // O_WRONLY | O_CREAT
    if ((flags & O_WRONLY) && (flags & O_CREAT)) {
        // keep O_WRONLY flag inside FCB
        fcbArray[returnFd].flag = 1;
        // is this flag already covered?

        //if file does not exist, create it

    }

    // O_WRONLY | O_CREAT | O_TRUNC
    if ((flags & O_WRONLY ) && (flags & O_CREAT) && (flags & O_TRUNC)) {
        // keep O_WRONLY flag inside FCB
        fcbArray[returnFd].flag = 1;

        //if file does not exist, create it

        //else truncate file to 0 length

    }

    // O_RDWR
    if (flags & O_RDWR) {
        // keep O_RDWR flag inside FCB
        fcbArray[returnFd].flag = 2;

    }

	fcbArray[returnFd].file = finfo;
	
	fcbArray[returnFd].buffer = malloc(B_CHUNK_SIZE * sizeof(char));
	if (fcbArray[returnFd].buffer == NULL) {
		return -1;
	}
	fcbArray[returnFd].bufferOffset = 0;
	fcbArray[returnFd].fileOffset = 0;
	fcbArray[returnFd].blockNum = (fcbArray[returnFd].file->fileSize + fsvcb->blockSize - 1) / fsvcb->blockSize;
	fcbArray[returnFd].location = fcbArray[returnFd].file->location;

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
		
		
	return (0); //Change this
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

	/*
	count
	count > remainder
	part 1 = remainder
	leftover = count - part 1
	blockCnt = leftover / fsvcb->blockSize
	part 2 = blockCnt * fsvcb->blockSize
	part 3 = leftover - part 2

	part 1 fill up your buffer to the remainder
	part 2 fill directly to the caller's buffer
	part 3 fill up your buffer again with new index
	*/

	if (count + fcbArray[fd].fileOffset > fcbArray[fd].file->fileSize) {
		count = fcbArray[fd].file->fileSize - fcbArray[fd].fileOffset;
	}

	// remainder in the block B_CHUNK_SIZE - current index
	int currentBlock = fcbArray[fd].fileOffset / fsvcb->blockSize;
	int remainder = fcbArray[fd].fileOffset % fsvcb->blockSize;
	int part1, part2, part3, leftover, blockCnt, read, location;
	if (count > remainder) {
		part1 = remainder;
		leftover = count - part1;
		blockCnt = leftover / fsvcb->blockSize;
		part2 = blockCnt * fsvcb->blockSize;
		part3 = leftover - part2;
	} else {
		part1 = count;
		part2 = 0;
		part3 = 0;
	}

	if (part1 > 0) {
		memcpy(buffer, fcbArray[fd].buffer + fcbArray[fd].bufferOffset, part1);
		fcbArray[fd].bufferOffset += part1;
		fcbArray[fd].fileOffset += part1;
	}

	if (part2 > 0) {
		location = freespaceNextBlock(fcbArray[fd].location);
		fcbArray[fd].location = location;
		read = LBAread(buffer + part1, 1, location);
		part2 = read * fsvcb->blockSize;
	}

	if (part3 > 0) {
		read = LBAread(fcbArray[fd].buffer, 1, fcbArray[fd].location);
	}


		
	return (0);	//Change this
	}
	
// Interface to Close the file	
int b_close (b_io_fd fd)
	{
		if (fcbArray[fd].buffer == NULL) {
			return -1;
		}
		/*
		free(fcbArray[fd].buffer);
		fcbArray[fd].buffer = NULL;
		free(fcbArray[fd].file);
		fcbArray[fd].file = NULL;
		*/
		return 0;
	}
