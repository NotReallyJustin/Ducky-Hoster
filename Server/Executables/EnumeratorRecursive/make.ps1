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
    Header file folders necessary for ls_read:
    - ../CommonLib/Common
    - ../CommonLib/Post/

    Linking components for ls_read:
    - wininet   (Should be universal across all Windows systems)
    - shlwapi   (Should also be across all Windows systems)             --> Windows Pathing
    - ../CommonLib/Common/libcommon.a
    - ../CommonLib/Post/libpost.a
    - ../CommonLib/Error/libwinerror.a
#>

gcc -c -m32 enumerator.c -o enumerator.o -I../CommonLib/Common/ -I../CommonLib/Post/ -DSVL_ADDRESS="\`"$SVL_ADDRESS\`"" -DSVL_AUTHKEY="\`"$SVL_AUTHKEY\`""
gcc enumerator.o -o enumerator.exe -L../CommonLib/Common/ -L../CommonLib/Post/ -L../CommonLib/Error/ -lpost -lcommon -lwinerror -lwininet -lshlwapi

# Remove extraneous files
rm enumerator.o