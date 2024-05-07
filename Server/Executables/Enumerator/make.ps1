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
gcc -c ls_read.c -o ls_read.o -I../CommonLib/Common/ -I../CommonLib/Post/ -D$SVL_ADDRESS -D$SVL_AUTHKEY
gcc ls_read.o -o ls_read.exe -L../CommonLib/Common/ -L../CommonLib/Post/ -L../CommonLib/Error/ -lpost -lcommon -lwinerror -lwininet

# Remove extraneous files
# rm ls_read.i
# rm ls_read.o