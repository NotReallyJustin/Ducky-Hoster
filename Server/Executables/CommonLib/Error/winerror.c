/*
    When the Windows API errors, its gets annoying since you can't exactly use traditional errno to deal with this.
    Hence, this C file exists.
*/

#include <stdio.h>
#include <windows.h>

void print_last_error(char* pretext)
{    
    LPSTR messageBuffer = NULL;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, 
        GetLastError(),             // We are local mallocing. Apparently when you do this, Windows wants you to use their own Janky free
        0,
        (LPSTR)&messageBuffer,      // IDK why mingw is giving me a type error here since this should be an address, but we're casting it now
        0, 
        NULL
    );

    fprintf(stderr, "%s: %s", pretext, messageBuffer);

    LocalFree(messageBuffer);
}