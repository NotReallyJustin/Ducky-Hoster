#include "post.h"
#include <stdio.h>

// Compile with: $gcc .\sample_post.c -o .\sample_post -L./ -lpost -lwininet --> Get object file to link

int main()
{
    puts("Done!");
    send_post_request("127.0.0.1:80", "Hello World!", 12, "ENUM", "HI");
    return 0;
}