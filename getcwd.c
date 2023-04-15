#include <unistd.h>
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

    //copy absolute path name of cwd to pathname
    //how do i do this?


}