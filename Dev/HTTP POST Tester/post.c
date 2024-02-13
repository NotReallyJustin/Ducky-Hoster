/*
    This file handles all of the potential POST requests that will be sent out by our executables.

    While I originally wrote a version of this using wininet, I ultimately scratched that and decided to use libcurl for the following
    reasons:

    1. Wininet is no longer supported by Windows (and hence, using it is extremely bad security practice)
    2. Ducky Hoster is still under development. When this deploys, I intend to use HTTPS - and libcurl supports that
    3. Wininet is bad at error handling

    I also thought about writing the HTTPS post request from scratch, but that requires fiddling with the OpenSSL library
    --> Which isn't really writing anything from scratch anyways

    gcc ./post.c -o path.exe -lcurl -L "/Program Files/curl-8.6.0_1-win32-mingw/lib" -I "/Program Files/curl-8.6.0_1-win32-mingw/include/"
    You need to link both the header file and the -a
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
//#include "libcurl.a"
#include "curl/curl.h"          // Compile this with libcurl as a static library

/**
 * Sends a POST request with the given text inside the body. The content type is text/plain.
 * This POST_Request is designed to work with the Ducky Hoster Server
 * @param address The address to send the item to
 * @param text String to send in the POST request
 * @param text_size Size of text to send
 * @param exe_type The EXE you're POSTing for (ie. ENUM)
 * @param auth_key Built in authentication key
*/
void send_post_request(char* address, char* text, int text_size, char* exe_type, char* auth_key)
{
    // Initialize all sub modules because we're on Windows and might want HTTPS down the line
    curl_global_init(CURL_GLOBAL_ALL);

    // Init a single thread $CURL "command instance" - or at least I like to think of it as a command instance because that's how it is on Kali
    CURL* curl_instance = curl_easy_init(); 
    puts("init");
    // Set URL - address usually is a macro that is passed in by get_req.js at compile time
    curl_easy_setopt(curl_instance, CURLOPT_URL, address);
    puts("URL set");

    // Set headers
    struct curl_slist* headers = NULL;

    char* to_send = malloc(5 + strlen(exe_type) + 1);       // EXE type  - Add 1 for null byte
    strcpy(to_send, "exe: ");
    strcat(to_send, exe_type);
    headers = curl_slist_append(headers, to_send);

    headers = curl_slist_append(headers, "Content-Type: text/plain");         // Content Type
    
    // Auth
    char* auth_send = malloc(15 + strlen(auth_key) + 1);
    strcpy(auth_send, "authorization: ");
    strcat(auth_send, auth_key);
    headers = curl_slist_append(headers, auth_send);

    // Set headers
    curl_easy_setopt(curl_instance, CURLOPT_HTTPHEADER, headers);

    // Set data to send
    curl_easy_setopt(curl_instance, CURLOPT_POSTFIELDS, text);
    curl_easy_setopt(curl_instance, CURLOPT_POSTFIELDSIZE, text_size);
    puts("ok data load");
    
    // Send the POST request
    curl_easy_perform(curl_instance);
    puts("ok data send");

    // Cleanup everything (and the POST request)
    free(to_send);
    free(auth_send);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl_instance);
    curl_global_cleanup();

    puts("ok cleanup");
}

int main()
{
    send_post_request("127.0.0.1:8081", "Hello there", 11, "ENUMERATE", "AUTH_KEY_HERE");
    puts("POST Request sent.");
    return 0;
}