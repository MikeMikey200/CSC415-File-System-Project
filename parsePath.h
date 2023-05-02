/**************************************************************
* Class: CSC-415-03 Spring 2023
* Names: Minh Dang, Sabrina Diaz-Erazo, Trinity Godwin
* Student IDs: 921210261, 916931527, 918448783
* GitHub Name: MikeMikey200
* Group Name: Wanna Cry
* Project: Basic File System
*
* File: parsePath.h
*
* Description: This file contains the function prototypes for
*			   locateEntry and parsePath.
*
**************************************************************/

#ifndef PARSEPATH_H
#define PARSEPATH_H

#include "dir.h"

#define MAXPATH 4096

// prototypes
/*!
@function 	locateEntry
@abstract 	locate the entry using index to traverse the directory to find name

@param 		name
			indicate pass by reference of tokenized string name
@param 		dir
			indicate pass by reference of dirEntry object
@param		index
			indicate the index of the directory

@return		if found return index of the entry,
			if not return -1
*/
int locateEntry(char *name, dirEntry *dir, int index);

/*
@function 	parsePath
@abstract 	take in a $pathname and parse it until last tokenized string if it exist or not in our directory

@param		pathname
			$pathname "foo\\bar"
@param		dir
			rootDir (absolute path) or starting dirEntry (relative path)
@param		catch
			the holder of found directory
			make sure to have the catch at MAX size

@return		the index of the directory
*/
int parsePath(char *pathname, dirEntry *dir, dirEntry *catch);

#endif /* PARSEPATH_H */