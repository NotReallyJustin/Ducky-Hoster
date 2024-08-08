/**
 * Performs a memory concatenation that automatically reallocates the existing destination string and copies source for $size bytes.
 * 🌟Sometimes, this is necessary when you're dealing with strings that contain internal null bytes (likely from reading a binary file)
 * @param dest Pointer to destination string. This will be malloc'd
 * @param injection_idx "string index" to inject the new items
 * @param source Source string
 * @param size Number of bytes (read: chars) to copy. 🚨 THIS ASSUMES YOU ADDED THE NULL TERMINATOR 🚨
 * @param str_len Pointer to string length item. This will get incremented by $size
*/
extern void concat_mem(char** dest, int injection_idx, char* source, int size, int* str_len);

/**
 * Performs a memory concatenation. However, strings will get concatenated as \" instead.
 * It's reccomended to not use this function unless you are 100% sure you might deal with \"
 * @param dest Pointer to destination string. This will be malloc'd
 * @param injection_idx "string index" to inject the new items
 * @param source Source string
 * @param size Number of bytes (read: chars) to copy. 🚨 THIS ASSUMES YOU ADDED THE NULL TERMINATOR 🚨
 * @param str_len Pointer to string length item. This will get incremented by $size
*/
extern void concat_str_mem(char** dest, int injection_idx, char* source, int size, int* str_len);

/**
 * Deallocates a string array (char**)
 * @param str_arr The array to deallocate
 * @param size Size of array
*/
extern void dealloc_str_arr(char** str_arr, int size);

/**
 * Return the size (read: number of chars since 1 char == 1 byte) of a given file.
 * @param file Pointer to the file object
 * @return The number of chars in the file
*/
extern long get_file_size(FILE* file);

/**
 * Converts a character into an 8 bit binary and chucks the result in binary_output
 * @param ascii_char ASCII character
 * @param binary_output {Outbound} String to store the 8 bit binary number in. Allocate 9 bytes so we can put a \0
*/
void char_to_binary(char ascii_char, char* binary_output);

/**
 * Converts a binary string into base 10.
 * @param binary_str The binary string to convert
 * @param size Size of the binary string. This is just a standard for C, but exclude the \0 at the end
 * @param intended_size Intended size of the binary string. This is mainly used for operations such as base64. \n
 * Zero padding would occur at the end of the binary string until $size == $intended_size. If you don't wish to use this, set this to $size. \n
 * Exclude \0
*/
int binary_to_base10(char* binary_str, int size, int intended_size);

/**
 * Prints all contents of a string array (char**)
 * @param str_arr The array to print
 * @param size Size of array
*/
void print_str_arr(char** str_arr, int size);

/**
 * Given the length of a normal binary string, determine the size of its base64 string
 * @param char_str_size The size of the binary string
 * @returns The size of its base64 string
*/
int base64_size(int char_str_size);

/**
 * Finds the base64 of a given string
 * @param str String to find the base64 of
 * @param size Size of the string to convert to base64 - Exclude \0
 * @param base64_str {Outbound} String to store the base64 output. Malloc base64_size() + 1 null byte
*/
void base64(char* str, int size, char* base64_str);

/**
 * Prints a string (including null bytes!) up to $size size
 * @param str String to print
 * @param size Size of String. Must be >= 0
 * @param show_null Binary value (1 or 0). Show null bytes as "\0" instead of concealing it
*/
void print_mem(char* str, int size, int show_null);

/**
 * Checks to see if a file path exists AND is a non-directory/non-device file.
 * @param path Specified path to check.
 */
int is_valid_file(char* file_path);