#include "post.h"
#include <stdio.h>

// Compile with: $. --> Get object file to link

int main()
{
    puts("Done!");
    send_post_request("http://localhost", "Hello World!", 12, "ENUM", "HI");
    return 0;
}