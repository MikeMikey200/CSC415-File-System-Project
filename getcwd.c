#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "mfs.h"
#include "fsLow.h"

/*
    - this function copies an absolute path name of the current
    working directory to pathname, which is of length size
    - it returns the absolute path name, which is the current
    working directory of the calling process
*/
char * fs_getcwd(char *pathname, size_t size) {
    pathname[size];

    // copy absolute path name of cwd to pathname
    // testing with ".."
    strcpy(pathname, "..\0");
    printf("Printing path name: %s\n", pathname);

    if (strlen("..\0") > size) {
        // ERANGE represents a range error
        errno = ERANGE;
        return NULL;
    }

    return pathname;
}