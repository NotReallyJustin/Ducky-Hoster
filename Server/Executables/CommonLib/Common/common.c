// A common C header file to be used in all executables
// Think of this as Justin's standard lib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <math.h>
#include "common.h"

// Tell C to ignore the gibberish about implicit function declarations because I know what I'm doing
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

void concat_mem(char** dest, int injection_idx, char* source, int size, int* str_len)
{
    // Allocate new memory.
    // We're subtracting 1 from this, since copying the null terminator doesn't change our string size (since we overwrote a null terminator)
    *str_len = *str_len + size - 1;
    *dest = realloc(*dest, *str_len);

    // Copy $size bytes
    memcpy(*dest + injection_idx, source, size);
}

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

        // Return the fseek back to the start of file to not mess with `f_pos` (basically, low level I/O for where you are in the file)
        fseek(file, 0, SEEK_SET);

        return byte_size;
    }
    else
    {
        return 0;
    }
}

void char_to_binary(char ascii_char, char* binary_output)
{
    int BYTE_SIZE = 8;
    int curr_str_idx = 0;           // Tracker to see what binary_output string index we are at right now

    // We'll perform a bit shift here to isolate each char and put them in the binary_output.
    // ie.          10010101 (ASCII char)
    //            & 10000000 (1 << 7)
    //         ==   10000000 ( Now we will >> 7)                --> We start from BYTE_SIZE - 1 to 0
    //              1        ( Put this in binary_output )

    for (int i = BYTE_SIZE - 1; i >= 0; i--)
    {
        // Grab the isolated binary digit
        int isolated_bin_digit = (ascii_char & (1 << i)) >> i;

        // Chuck it as a number
        binary_output[curr_str_idx] = isolated_bin_digit == 1 ? '1' : '0';

        // Increment curr_str_idx
        curr_str_idx++;
    }

    // Add null byte
    binary_output[curr_str_idx] = '\0';
}

int binary_to_base10(char* binary_str, int size, int intended_size)
{
    // Allocate a string up to $intended_size + 1 null byte
    char* intended_bin_str = malloc(intended_size + 1);

    // Populate intended_bin_str. It'll copy $size characters from the address in $binary_str, and then pad the rest with zeroes
    for (int i = 0; i < intended_size; i++)
    {
        if (i < size)
        {
            intended_bin_str[i] = binary_str[i];
        }
        else
        {
            intended_bin_str[i] = '0';
        }
    }

    // Add a null byte at the end.
    intended_bin_str[intended_size] = '\0';

    // Now, convert the binary string into base 10
    // We'll use bit shifting for this. Take for example:
    // ie.  1010101         (intended_bin_str)
    //      Curr bit = 1
    //      Binary value = 1000000         ( Curr Bit << [intended_bin_str_size - 1] )
    //      Add that to $value

    int value = 0;  
    for (int i = intended_size - 1, str_idx = 0; str_idx < intended_size; i--, str_idx++)
    {
        // Bits can either be 1 or 0
        int curr_bit = intended_bin_str[str_idx] == '1' ? 1 : 0;

        value += curr_bit << i;
    }

    // Garbage collection go brrrr
    free(intended_bin_str);

    return value;
}

void print_mem(char* str, int size, int show_null)
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

void print_str_arr(char** str_arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        puts(str_arr[i]);
    }
}

int base64_size(int char_str_size)
{
    // Base64 takes in multiples of 3 bytes * 8 (24) bits in each byte. If there's less than that, it'll pad it with zeroes
    // Each multiple (group) will then be converted to 4 base64 chars
    
    // Hence, the formula is ceil(n * 8 / 24) * 4
    // The *8 exists because chars are in bytes
    return ceil((char_str_size * 8) / 24.0) * 4;
}

void base64(char* str, int size, char* base64_str)
{
    int BYTE_SIZE = 8;

    // ⭐ Step 1: Convert each character of the string into 1 byte ASCII binary number

    // Each character in str is going to be an 8 bit binary. Hence, malloc $size * 8 bytes.
    // However, when we malloc, we'll add 1 extra byte for the null byte at the end
    int binary_str_size = BYTE_SIZE * size;
    char* binary_str = malloc(binary_str_size + 1);
    
    // For each char in $str, convert it to a binary and chuck it inside $binary_str
    // Technically, char_to_binary takes in 9 bytes. However, we're going to overwrite the null byte either way so we can multiply by $BYTE_SIZE
    for (int i = 0; i < size; i++)
    {
        char_to_binary(str[i], binary_str + i * BYTE_SIZE);
    }
    binary_str[binary_str_size] = '\0';

    // ⭐ Step 2: Split the binary string into chunks of 6 bits. Then, it would convert to them to a base64 string.
    char* base_64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int base64_str_idx = 0;             // Track where on the base64 string we are at

    for (int i = 0; i < binary_str_size; i += 6)
    {
        // Calculate the current chunk size.
        // If everything fits from base_64_str[0] to base_64_str[5], our chunk size is 6
        // If it doesn't fit, then all we're dealing with is the extra binary characters from size

        // ie. Let's say i = 30 and bin_str_size = 31.
        // base_64_str[30] to base_64_str[35] is not possible. So, the current chunk size is not 6
        // Hence, we take the max binary string size (31). 31 % 6 = 1. And it turns out we do only have 1 extra bit.
        // And that 1 isolated bit is going to be our current chunk size
        
        int curr_chunk_size = (i + 5 < binary_str_size) ? 6 : (binary_str_size % 6 + 1);

        // Grab the value of the chunk of 6 bits. To make indexing easier, this is in base 10
        int binary_num = binary_to_base10(binary_str + i, curr_chunk_size, 6);

        // Go on the base64 table and grab the corresponding base64 character. Add that to base_64_str
        base64_str[base64_str_idx] = base_64_table[binary_num];
        base64_str_idx++;
    }

    // If we can't make it to a 3 byte sequence (basically, binary_str_size is not divisible by 3), add = at the end
    for (int i = 0; i < (binary_str_size % 3); i++)
    {
        base64_str[base64_str_idx] = '=';
        base64_str_idx++;
    }

    // Add 1 null byte at the end
    base64_str[base64_str_idx] = '\0';

    // ⭐ Garbage collection
    free(binary_str);
}

int is_valid_file(char* file_path) 
{
    DWORD root_file_attributes = GetFileAttributes(file_path);
    return (root_file_attributes != INVALID_FILE_ATTRIBUTES) && (!(root_file_attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)));
}

int is_admin() 
{
    /* Generate a "well-known" SID for accounts that can start processes in admin mode
        - Top level identifier authority: SECURITY_NT_AUTHORITY (S-1-5)  --> From what I understand, this is needed to craft SIDs specific to a windows installation/domain
        - RID: DOMAIN_ALIAS_RID_ADMINS (local admin group on a domain)
        - RID: SECURITY_BUILTIN_DOMAIN_RID (the domain is the local, built-in system on a PC)
    */
    SID_IDENTIFIER_AUTHORITY top_level_auth = SECURITY_NT_AUTHORITY;
    PSID created_wk_sid;

    if (AllocateAndInitializeSid(&top_level_auth, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &created_wk_sid)) 
    {
        int running_with_admin_perms;

        // Compare impersonation tokens. While we do that, handle errors
        if (CheckTokenMembership(NULL, created_wk_sid, &running_with_admin_perms) == 0) 
        {
            running_with_admin_perms = -1;
            print_last_error("is_admin: Unable to check token membership");
        }

        FreeSid(created_wk_sid);

        return running_with_admin_perms;
    }

    // If we reach here, there's an error
    print_last_error("is_admin: Unable to allocate well known SID to verify token");
    return -1;
}