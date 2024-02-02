/*
    This file handles all of the potential POST requests that will be sent out by our executables.

    While I originally wrote a version of this using wininet, I ultimately scratched that and decided to use libcurl for the following
    reasons:

    1. Wininet is no longer supported by Windows (and hence, using it is extremely bad security practice)
    2. Ducky Hoster is still under development. When this deploys, I intend to use HTTPS - and libcurl supports that
    3. Wininet is bad at error handling

    I also thought about writing the HTTPS post request from scratch, but that requires fiddling with the OpenSSL library
    --> Which isn't really writing anything from scratch anyways
*/

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>          // Compile this with libcurl as a static library

int libcurl_initialized = 0;

/**
 * Sends a POST request with the given text inside the body. The content type is text/plain.
 * This POST_Request is designed to work with the Ducky Hoster Server
 * @param text String to send in the POST request
 * @param exe_type The EXE you're POSTing for (ie. ENUM)
 * @param auth_key Built in authentication key
*/
void send_post_request(char* text, char* exe_type, char* auth_key)
{
    // Before we even do anything,we must invoke global init if it's not already invoked
    if (libcurl_initialized == 0)
    {
        // Initialize all sub modules because we're on Windows and might want HTTPS down the line
        curl_global_init(CURL_GLOBAL_ALL);
    }

    // Init a single thread $CURL "command instance" - or at least I like to think of it as a command instance because that's how it is on Kali
    CURL* curl_instance = curl_easy_init(); 

    // Set URL - address is a macro that is passed in by get_req.js at compile time
    curl_easy_setopt(curl_instance, CURLOPT_URL, address);

    // Set headers
    struct curl_slist* headers;
    curl_slist_append(headers, strcat("exe: ", exe_type));
    curl_slist_append(headers, "Content-Type: text/plain");
    // Auth

    // Set data to send
    curl_easy_setopt(curl_instance, CURLOPT_POSTFIELDS, text);
    
    // Send the POST request
    curl_easy_perform(curl_instance);

    // Cleanup everything (and the POST request)
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl_instance);
    curl_global_cleanup();
}