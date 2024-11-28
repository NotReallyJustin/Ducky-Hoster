#include <wininet.h>

/**
 * Frees everything we malloc's inside url_components.
 * Usually, this is done after all fields inside `parse_url()` is processed
 * @param url_components Poiner to url_components struct to free
*/
void free_url_components(URL_COMPONENTS* url_components);

/**
 * Parses a HTTPS URL into its components.
 * It is your job to free all the fields inside url_components when done. We reccomend calling `free_url_components()`
 * @param address HTTPS URL address string to parse
 * @param url_components {Outbound} Pointer to a struct that will contain the URL components. This will be zero'd out.
 * @throw Error if there's an issue parsing URL
 * @returns 0 if successful, -1 otherwise
*/
int parse_url(char* address, URL_COMPONENTS* url_components);

/**
 * Sends a POST request with the given text inside the body. The content type is text/plain.
 * This POST_Request is designed to work with the Ducky Hoster Server
 * @param address The address to send the item to
 * @param text String to send in the POST request
 * @param text_size Size of text to send. This excludes NULL
 * @param exe_type The EXE you're POSTing for (ie. ENUM)
 * @param auth_key Built in authentication key
 * @returns 0 if successful, -1 otherwise
 * @throw Errors if WiniNet returns issues. These go into stderr
*/
extern int send_post_request(char* address, char* text, int text_size, char* exe_type, char* auth_key);