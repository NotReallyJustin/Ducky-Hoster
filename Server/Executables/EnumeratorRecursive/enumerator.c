#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>            // You must import this to interact with Windows API because Windows doesn't have <dirent.h> (fml)
#include <common.h>

// Define some macros for address and auth key
#ifndef SVL_ADDRESS
    #define SVL_ADDRESS "https://localhost"
#endif

#ifndef SVL_AUTHKEY
    #define SVL_AUTHKEY "NO AUTH KEY"
#endif

// ⭐ Enumerator-Specific configurations

/**
 * Maximum depth to enumerate (for file directories).
 * Basically, how many folders do we want to look into. Even if this number is big, we do need a max size for this because we don't have infinite GBs to transfer files.
 * This is also important as the Node server limits how much memory can be transferred.
 */
#define MAX_DEPTH 3

/**
 * Defines a path to start from when recursively enumerating directories.
 * By default, this is set to the home directory
 */
#define ROOT_PATH getenv("USERPROFILE")

// Ignore implicit function declarations
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

int main(int argc, char** argv)
{
    puts(ROOT_PATH);
    return 0;
}