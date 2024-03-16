/**
 * Sends a POST request with the given text inside the body. The content type is text/plain.
 * This POST_Request is designed to work with the Ducky Hoster Server
 * @param address The address to send the item to
 * @param text String to send in the POST request
 * @param text_size Size of text to send
 * @param exe_type The EXE you're POSTing for (ie. ENUM)
 * @param auth_key Built in authentication key
*/
extern void send_post_request(char* address, char* text, int text_size, char* exe_type, char* auth_key);