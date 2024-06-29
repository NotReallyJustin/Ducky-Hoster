#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common.h>
#include <windows.h>
#include <Shlwapi.h>

// Define some macros for address and auth key
#ifndef SVL_ADDRESS
    #define SVL_ADDRESS "https://localhost"
#endif

#ifndef SVL_AUTHKEY
    #define SVL_AUTHKEY "NO AUTH KEY"
#endif

// ⭐ Enumerator-Specific configurations

/**
 * Maximum depth to enumerate (for file directories).
 * Basically, how many folders do we want to look into. Even if this number is big, we do need a max size for this because we don't have infinite GBs to transfer files.
 * This is also important as the Node server limits how much memory can be transferred.
 */
#define MAX_DEPTH 2

/**
 * Defines a path to start from when recursively enumerating directories.
 * By default, this is set to the home directory
 */
// #define ROOT_PATH getenv("USERPROFILE")
#define ROOT_PATH "C:/Users/Justi/Desktop/Code/Ducky-Hoster/Server/Executables/EnumeratorRecursive"

/**
 * Windows' max path size. Although this could be edited via the registry, we'll leave it as default and truncate it if people do anything crazy.
 * We'll talk more about this variable later, but essentially this will serve as our max file size.
 */
#define MAX_PATH_SIZE 260

// Ignore implicit function declarations
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

/**
 * Defines a file system element.
 * This is going to be used to map out the Windows directories and files.
 */
struct FS_Element
{
    /**
     * The file system element name.
     * The maximum length of a file name (technically max length of path, but file name < path < max length so we good here) is 260 characters.
     * That's how much space we're going to allocate. While we could malloc() this thing, it's in our best interest to restrict the file name size since
     * we have a limited number of GBs available to transfer JSONs to the express.js server - and we don't want to waste that stuff on file names. Worst case
     * scenario, we can just truncate everything.
     * @note This does mean that you must use memory-safe functions when dealing with the file name. `n = 259` since we need to add the null byte ourselves.
     * @see https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=registry
     */
    char name[MAX_PATH_SIZE];

    /**
     * Whether a file system element is a directory (read: folder)
     */
    BOOLEAN is_directory;

    /**
     * ONLY VALID IF THE FS ELEMENT IS A DIRECTORY.
     * Returns the number of file system elements (files, folders, shortcuts, etc) in the directort
     */
    int dir_size;
    
    /**
     * ONLY VALID IF THE FS ELEMENT IS A DIRECTORY.
     * An array that contains FS_Element* (FS_Element pointers) that represent all the file elements inside the directory.
     * @note You must free() this yourself
     */
    struct FS_Element** dir_elements;
};

/**
 * Recursively frees all the `dir_elements` inside a FS_Element (and the double arrays)
 * Then, it frees itself.
 */
void free_fs_element(struct FS_Element* file_system_element)
{
    if (file_system_element->is_directory == TRUE)
    {
        // If it is a directory, recursively free it
        for (int i = 0; i < file_system_element->dir_size; i++)
        {
            free_fs_element(file_system_element->dir_elements[i]);
        }
    }

    free(file_system_element);
}

/**
 * Returns whether or not a given path is a directory.
 * @param path Path to test whether it's a directory
 * @returns A boolean indicating whether the path is a directory
 */
BOOLEAN is_directory(char* path)
{
    DWORD file_attribute = GetFileAttributes(path);

    // We actually don't get 1 or 0, and instead, we get a bitwise-AND' result. Hence, we need to feed it through
    // an if statement
    if ((file_attribute != INVALID_FILE_ATTRIBUTES) && (file_attribute & FILE_ATTRIBUTE_DIRECTORY))
    {
        return TRUE;
    }
    
    return FALSE;
}

/**
 * Enumerates the file system starting from $ROOT_PATH.
 * It will then enumerate recursively for $MAX_DEPTH number of directories
 * @param depth Starts at 0. Indicates the number of directories we're peering into. This does mean a $MAX_DEPTH of 0 is just $ls_read.exe
 * @param dir_path Directory path to enumerate. When you call this, this should usually always be $ROOT_PATH
 * @note You are responsible for free-ing all the FS_Element structs recursively. We reccomend calling free_fs_element
 * @returns A FS_Element struct representing the current directory. If there is nothing to enumerate (due to us exceeding $MAX_DEPTH usually), this returns NULL
 */
struct FS_Element* enumerate(int depth, char* dir_path)
{
    // 🚨 Apparently, FindFirstFileA is a bozo. This means it won't match everything in dir_path and instead, we need to provide it a "regex" like query
    // This just concatenates a wildcard at the end so that we query through everything in our directory
    // What's crazy is that fullpath doesn't automatically add / so we need to do it ourselves
    char curr_query[MAX_PATH_SIZE];
    strncpy(curr_query, dir_path, MAX_PATH_SIZE);
    strncat(curr_query, "/*", MAX_PATH_SIZE - strlen(curr_query));
    curr_query[strlen(curr_query)] = '\0';

    // Check to see that we don't exceed $MAX_DEPTH
    if (depth <= MAX_DEPTH && depth >= 0)
    {
        // Create a current file system element for this directory
        struct FS_Element* curr_element = malloc(sizeof(struct FS_Element));

        // Get the current directory name and append it to the struct. This windows call automatically handles input size for us
        GetCurrentDirectory(MAX_PATH_SIZE - 1, curr_element->name);
        curr_element->name[MAX_PATH_SIZE] = '\0';

        // This is a directory, so let's initialize some stuff off that:
        curr_element->dir_size = 0;
        curr_element->is_directory = TRUE;

        // ⭐ Enumerate all the files in the directory. The following code will look like LS_READ

        // Variable for... well... file data. This will be overwritten for each iteration of FindFirstFile();
        WIN32_FIND_DATAA file_data; 

        // Windows handle (sorry I meant iterator) to search through all the items. Loop through the current directory.
        // Also, if you can't find any files, just return NULL since this directory is useless
        HANDLE search_handle = FindFirstFileA(curr_query, &file_data);
        if (search_handle == INVALID_HANDLE_VALUE)
        {
            return NULL;
        }

        // If there are files to enumerate, append the current data. Then continue looping until there are no more files left.
        do
        {
            // Literally ignore . and .. because infinite recursion isn't good lol
            if (strcmp(file_data.cFileName, ".") != 0 && strcmp(file_data.cFileName, "..") != 0)
            {
                // We're going to be dynamically malloc'ing the $dir_elements array
                // If we exhaust all 10 items in the array, create another 10
                if (curr_element->dir_size % 10 == 0 && curr_element->dir_size > 0)
                {
                    curr_element->dir_elements = realloc(curr_element->dir_elements, (curr_element->dir_size + 10) * sizeof(struct FS_Element*));
                }
                else if (curr_element->dir_size == 0)
                {
                    // If we're malloc'ing for the first time
                    // We will use this malloc b/c if we're in this loop, that means we do have an element
                    curr_element->dir_elements = malloc(10 * sizeof(struct FS_Element*));
                }

                // Check to see if we have a directory. If we do, start recursion here
                char full_path[MAX_PATH_SIZE];
                PathCombine(full_path, dir_path, file_data.cFileName);

                if (is_directory(full_path))
                {
                    struct FS_Element* dir_data = enumerate(depth + 1, full_path);

                    // If the dir actually has stuff, add it to our elements path
                    if (dir_data != NULL)
                    {
                        printf("%s : directory\n", file_data.cFileName);
                        curr_element->dir_elements[curr_element->dir_size] = dir_data;
                        curr_element->dir_size += 1;
                    }
                }
                else
                {
                    // If it's not a directory, we're just going to create a new FS_Element struct for it and add it to the current file system element's array
                    struct FS_Element* file_element = malloc(sizeof(struct FS_Element));
                    file_element->is_directory = FALSE;

                    // Add the name
                    strncpy(file_element->name, file_data.cFileName, MAX_PATH_SIZE - 1);
                    file_element->name[strlen(file_data.cFileName)] = '\0';

                    // Add the file element to the current element's array
                    curr_element->dir_elements[curr_element->dir_size] = file_element;
                    curr_element->dir_size += 1;

                    printf("%s : file\n", file_data.cFileName);
                }
            }

        } while (FindNextFileA(search_handle, &file_data) != 0);     // FindNextFile returns 0 when it fails

        // And we're done! Return the current element
        return curr_element;
    }

    return NULL;
}

int main(int argc, char** argv)
{
    // Error check our default params to make sure they're correctly defined
    if (MAX_DEPTH < 0)
    {
        fprintf(stderr, "Error: $MAX_DEPTH should be > 0.");
        return 1;
    }

    // Check for valid directory
    DWORD root_file_attributes = GetFileAttributes(ROOT_PATH);

    if (root_file_attributes == INVALID_FILE_ATTRIBUTES)
    {
        fprintf(stderr, "Error: $ROOT_PATH is not a valid path.");
        return 1;
    }
    if (!(root_file_attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        fprintf(stderr, "Error: $ROOT_PATH is not a directory.");
        return 1;
    }

    // Run enumerator
    struct FS_Element* enumerated_results = enumerate(0, ROOT_PATH);
    free_fs_element(enumerated_results);

    return 0;
}