<#
    Required macros:
    - SVL_ADDRESS           Server Address. Without this, ls_read will default to http://localhost
    - SVL_AUTHKEY           Post request authentication key
#>
param (
    [string]$SVL_ADDRESS,
    [string]$SVL_AUTHKEY
)

<#
    Header file directories necessary for ls_read:
    - ../CommonLib/Common/
    - ../CommonLib/Post/

    Linking components for ls_read:
    - wininet   (Should be universal across all Windows systems)
    - ../CommonLib/Common/libcommon.a
    - ../CommonLib/Post/libpost.a
    - ../CommonLib/Error/libwinerror.a
#>

gcc -m32 -c regexport.c -o regexport.o -I../CommonLib/Common/ -I../CommonLib/Post/ -DSVL_ADDRESS="\`"$SVL_ADDRESS\`"" -DSVL_AUTHKEY="\`"$SVL_AUTHKEY\`""
gcc regexport.o -o regexport.exe -L../CommonLib/Common/ -L../CommonLib/Post/ -L../CommonLib/Error/ -lpost -lcommon -lwinerror -lwininet

rm regexport.o