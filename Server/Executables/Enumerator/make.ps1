<#
    Required macros:
    - SVL_ADDRESS           Server Address. Without this, ls_read will default to http://localhost
    - SVL_AUTHKEY           Post request authentication key
#>
param (
    [string]$SVL_ADDRESS,
    [string]$SVL_AUTHKEY
)

# 🚨 Important Note: Link dependencies AFTER the library. So if post.a depends on wininet, link wininet after

<#
    Header file folders necessary for ls_read:
    - ../CommonLib/Common
    - ../CommonLib/Post/

    Linking components for ls_read:
    - wininet   (Should be universal across all Windows systems)
    - ../CommonLib/Common/libcommon.a
    - ../CommonLib/Post/libpost.a
#>

<#
    GCC and Powershell work as well as oil on fire when it comes to passing in quotation marks. What we need to do is:
    1. Use "" so bash recognizes that $SVL_ADDRESS is a variable that must be evaluated when passing into gc
    2. Use \ so GCC recognizes that it's a backslash escape (and to actually turn that macro into a String literal during preprocessing)
    3. Use `" so powershell passes in the quotation instead of being dumb
#>

# gcc -E ls_read.c -o ls_read.i -I../CommonLib/Common/ -I../CommonLib/Post/ -DSVL_ADDRESS="\`"$SVL_ADDRESS\`"" -DSVL_AUTHKEY="\`"$SVL_AUTHKEY\`""
gcc -c ls_read.c -o ls_read.o -I../CommonLib/Common/ -I../CommonLib/Post/ -DSVL_ADDRESS="\`"$SVL_ADDRESS\`"" -DSVL_AUTHKEY="\`"$SVL_AUTHKEY\`""
gcc ls_read.o -o ls_read.exe -L../CommonLib/Common/ -L../CommonLib/Post/ -L../CommonLib/Error/ -lpost -lcommon -lwinerror -lwininet

# Remove extraneous files
rm ls_read.o