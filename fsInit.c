/**************************************************************
* Class: CSC-415-03 Spring 2023
* Names: Minh Dang, Sabrina Diaz-Erazo, Trinity Godwin, Karma Namgyal Ghale
* Student IDs: 921210261, 916931527, 918448783, 921425775
* GitHub Name: MikeMikey200
* Group Name: Wanna Cry
* Project: Basic File System
*
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "fsLow.h"
#include "mfs.h"
#include "vcb.h"
#include "fat.h"
#include "dir.h"
#include "parsePath.h"

/* extern definitions */
vcb *fsvcb;
fat *freespace;
dirEntry *rootDir;
dirEntry *currentwd;

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	unsigned int vcbSize = sizeof(vcb); // size of vcb
	unsigned int vcbBlock = BLOCK(vcbSize, 1, blockSize); // num of blocks of vcb
	
	unsigned int fatSize = sizeof(fat); // size of fat
	unsigned int fatBlock = BLOCK(fatSize, numberOfBlocks, blockSize); // num of blocks of fat

	unsigned int dirEntrySize = sizeof(dirEntry); // size of directory entry
	unsigned int dirEntryBlock = (BLOCK(dirEntrySize, INITENTRIES, blockSize) * blockSize) / dirEntrySize; // num of blocks of directory entries

	unsigned int totalBlock = vcbBlock + fatBlock + dirEntryBlock;

	fsvcb = malloc(vcbBlock * blockSize);
	freespace = malloc(fatBlock * blockSize);
	currentwd = malloc(MAXENTRIES * dirEntrySize);

	// this is to check whether vcb is already init so we don't override disk
	LBAread(fsvcb, vcbBlock, 0);
	if(fsvcb->signature == SIGNATURE){
		// LBAread(freespace, fatBlock, vcbBlock);
		// rootDir = malloc(dirEntryBlock * blockSize);
		// return 0;
	}

	// initializing vcb
	fsvcb->signature = SIGNATURE; // our magic number
	fsvcb->locationFreespace = vcbBlock; // end of vcb is vcbBlock - 1
	fsvcb->locationRootDir = vcbBlock + fatBlock; // end of freespace is vcbBlock + fatBlock - 1
	fsvcb->blockNum = numberOfBlocks; // num of block in LBA
	fsvcb->blockNumFree = numberOfBlocks; // num of block left available
	fsvcb->blockSize = blockSize; // size of each block

	LBAwrite(fsvcb, vcbBlock, 0); // write vcb into disk

	// initializing freespace to 0
	freespaceInit();
	
	// mark vcb on freespace map as used
	freespaceAllocateBlocks(0, vcbBlock);

	// mark freespace on freespace map as used
	freespaceAllocateBlocks(vcbBlock, fatBlock);

	rootDir = dirInit(INITENTRIES, NULL);
	
	LBAread(rootDir, rootDir->size / fsvcb->blockSize, rootDir->location);
	
	// testing for parsePath for dir
	dirEntry *tempDir;

	dirEntry *dir1 = dirInit(INITENTRIES, rootDir);
	dirEntryCopy(rootDir, dir1, dirFindUnusedEntry(rootDir), "dir1");
	dirEntry *dir2 = dirInit(INITENTRIES, rootDir);
	dirEntryCopy(rootDir, dir2, dirFindUnusedEntry(rootDir), "dir2");
	dirEntry *dir3 = dirInit(INITENTRIES, rootDir);
	dirEntryCopy(rootDir, dir3, dirFindUnusedEntry(rootDir), "dir3");

	LBAread(tempDir, rootDir->size / fsvcb->blockSize, rootDir->location);
	
	for(int i = 2; i < 5; i++) {
		printf("%s\n", tempDir[i].name);
	}

	dirEntry *foo1 = dirInit(INITENTRIES, dir3);
	dirEntryCopy(dir3, foo1, dirFindUnusedEntry(dir3), "foo1");
	dirEntry *foo2 = dirInit(INITENTRIES, dir3);
	dirEntryCopy(dir3, foo2, dirFindUnusedEntry(dir3), "foo2");

	LBAread(tempDir, dir3->size / fsvcb->blockSize, dir3->location);

	for(int i = 2; i < 4; i++) {
		printf("%s\n", tempDir[i].name);
	}

	dirEntry *bar1 = dirInit(INITENTRIES, foo2);
	dirEntryCopy(foo2, bar1, dirFindUnusedEntry(foo2), "bar1");
	dirEntry *bar2 = dirInit(INITENTRIES, foo2);
	dirEntryCopy(foo2, bar2, dirFindUnusedEntry(foo2), "bar2");
	dirEntry *bar3 = dirInit(INITENTRIES, foo2);
	dirEntryCopy(foo2, bar3, dirFindUnusedEntry(foo2), "bar3");

	LBAread(tempDir, foo2->size / fsvcb->blockSize, foo2->location);

	for(int i = 2; i < 5; i++) {
		printf("%s\n", tempDir[i].name);
	}

	char pathname[] = "dir3\\foo2\\bar2";
	int index = parsePath(pathname, rootDir, tempDir);
	printf("parsePath: %d\n", index);

	// location check
	printf("%d\n", bar2->location);
	printf("%d\n", tempDir[index].location);

	// testing parsePath for file.name == dir.name
	dirEntry *dir1file1 = dirInit(INITENTRIES, dir1);
	dir1file1->type = 1;
	dirEntryCopy(dir1, dir1file1, dirFindUnusedEntry(dir1), "dir1file1");
	dirEntry *dir1file2 = dirInit(INITENTRIES, dir1);
	dir1file2->type = 1;
	dirEntryCopy(dir1, dir1file2, dirFindUnusedEntry(dir1), "dir1file2");

	dirEntry *dir1dir1 = dirInit(INITENTRIES, dir1);
	dirEntryCopy(dir1, dir1dir1, dirFindUnusedEntry(dir1), "dir1file1");
	dirEntry *dir1dir2 = dirInit(INITENTRIES, dir1);
	dirEntryCopy(dir1, dir1dir2, dirFindUnusedEntry(dir1), "dir1file2");

	LBAread(tempDir, dir1->size / fsvcb->blockSize, dir1->location);

	for(int i = 2; i < 6; i++) {
		printf("%s\n", tempDir[i].name);
	}

	dirEntry *dir1dir1file1 = dirInit(INITENTRIES, dir1dir2);
	dirEntryCopy(dir1dir2, dir1dir1file1, dirFindUnusedEntry(dir1dir2), "dir1dir1file1");

	LBAread(tempDir, dir1dir2->size / fsvcb->blockSize, dir1dir2->location);

	for(int i = 2; i < 3; i++) {
		printf("%s\n", tempDir[i].name);
	}

	char pathname1[] = "dir1\\dir1file2\\dir1dir1file1";
	index = parsePath(pathname1, rootDir, tempDir);
	printf("parsePath: %d\n", index);

	// location check
	printf("%d\n", dir1dir1file1->location);
	printf("%d\n", tempDir[index].location);

	fs_setcwd("dir3\\foo2\\bar3");
	printf("%d\n", currentwd->location);

	char *str;
	str = fs_getcwd(str, 1024);
	printf("%s\n", str);

	fdDir * fd = fs_opendir(pathname);
	struct fs_diriteminfo *dirInfo = fs_readdir(fd);

	return 0;
	}
	
	
void exitFileSystem ()
	{
	// free up resources
	free(fsvcb);
	free(freespace);
	free(rootDir);
	free(currentwd);
	printf ("System exiting\n");
	}
