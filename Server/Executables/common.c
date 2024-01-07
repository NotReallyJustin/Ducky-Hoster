// A common C header file to be used in all executables
// Think of this as Justin reusing code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tell C to ignore the gibberish about implicit function declarations because I know what I'm doing
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

/**
 * Performs a memory concatenation that automatically reallocates the existing destination string and copies source for $size bytes.
 * 🌟Sometimes, this is necessary when you're dealing with strings that contain internal null bytes (likely from reading a binary file)
 * @param dest Pointer to destination string. This will be malloc'd
 * @param injection_idx "string index" to inject the new items
 * @param source Source string
 * @param size Number of bytes (read: chars) to copy. 🚨 THIS ASSUMES YOU ADDED THE NULL TERMINATOR 🚨
 * @param str_len Pointer to string length item. This will get incremented by $size
*/
void concat_mem(char** dest, int injection_idx, char* source, int size, int* str_len)
{
    // Allocate new memory.
    // We're subtracting 1 from this, since copying the null terminator doesn't change our string size (since we overwrote a null terminator)
    *str_len = *str_len + size - 1;
    *dest = realloc(*dest, *str_len);

    // Copy $size bytes
    memcpy(*dest + injection_idx, source, size);
}

/**
 * Performs a memory concatenation. However, strings will get concatenated as \" instead.
 * It's reccomended to not use this function unless you are 100% sure you might deal with \"
 * @param dest Pointer to destination string. This will be malloc'd
 * @param injection_idx "string index" to inject the new items
 * @param source Source string
 * @param size Number of bytes (read: chars) to copy. 🚨 THIS ASSUMES YOU ADDED THE NULL TERMINATOR 🚨
 * @param str_len Pointer to string length item. This will get incremented by $size
*/
void concat_str_mem(char** dest, int injection_idx, char* source, int size, int* str_len)
{
    // Allocate new memory FOR NOW.
    // This will get realloc'd if we see a \"
    *str_len = *str_len + size - 1;
    *dest = realloc(*dest, *str_len);
    
    // We will have 2 loop control variables here: i to loop through source, and curr_idx to loop and update the destination
    for (int i = 0, curr_idx = injection_idx; i < size; i++, curr_idx++)
    {
        if (source[i] == '"')
        {
            // If we find a ", we need to replace it with \". This means we need to:
            // Incremenmt str_len to account for \, reallocate dest with new str_len to account for \, and add \ into the destination

            *str_len = *str_len + 1;
            *dest = realloc(*dest, *str_len);

            (*dest)[curr_idx] = '\\';       // This is just \, but C chars are acting weird

            curr_idx++;
            (*dest)[curr_idx] = '"';
        }
        else
        {
            (*dest)[curr_idx] = source[i];
        }
    }
}

/**
 * Deallocates a string array (char**)
 * @param str_arr The array to deallocate
 * @param size Size of array
*/
void dealloc_str_arr(char** str_arr, int size)
{
    // Cautionary tale: sizeof on a pointer returns the size of its first element
    // sizeof(char**) == sizeof(char*[0])
    for (int i = 0; i < size; i++)
    {
        free(str_arr[i]);
    }

    free(str_arr);
}

/**
 * Return the size (read: number of chars since 1 char == 1 byte) of a given file.
 * @param file Pointer to the file object
 * @return The number of chars in the file
*/
long get_file_size(FILE* file)
{
    // If we have nullptr, file size basically does not exist
    if (file == NULL)
    {
        return 0;
    }

    // Now, we use fseek to jump the pointer to the end of the file, and then ftell will tell us how many bytes we moved
    // fseek returns 0 if successful
    if (fseek(file, 0, SEEK_END) == 0)
    {
        long byte_size = ftell(file);

        // Return the fseek back to the start of file to not mess with fgets
        fseek(file, 0, SEEK_SET);

        return byte_size;
    }
    else
    {
        return 0;
    }
}


/**
 * Prints all contents of a string array (char**)
 * @param str_arr The array to print
 * @param size Size of array
*/
void print_str_arr(char** str_arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        puts(str_arr[i]);
    }
}