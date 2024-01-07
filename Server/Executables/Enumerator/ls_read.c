#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>            // You must import this to interact with Windows API

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

        // fread does read in binary mode, but ASCII values will still be displayed normally
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

    puts("");
}

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
        if (read_file == NULL)
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
        long file_contents_size = get_file_size(read_file) + 1;
        char* file_contents = malloc(file_contents_size);       // We will transfer this over via strcat
        fread(file_contents, file_contents_size, 1, read_file);
        file_contents[file_contents_size - 1] = '\0';                   // C doesn't automatically add null terminators to the end of fread, so we should do it manually ourselves

        // Actually write the string here
        concat_mem(&json_string, json_size - 1, "\"", 2, &json_size);
        concat_str_mem(&json_string, json_size - 1, file_contents, file_contents_size, &json_size);   // File contents size already includes the null terminator
        concat_mem(&json_string, json_size - 1, "\",", 3, &json_size);

        free(file_contents);
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
    print_mem(json_str, json_length, TRUE);

    dealloc_str_arr(dir_names, size);
    free(json_str);

    return 0;
}