# To use this library, you must link this static lib AND wininet

# # Make this a static library
# # To use `libpost.a`, just include post.h. For all intents and purposes, refer to this file as `post`. You will never need to write `libpost`

gcc -c -m32 post.c -o post.o -I./ -I../Error/
gcc -c -m32 ../Error/winerror.c -o ../Error/winerror.o
ar -rcs libpost.a post.o ../Error/winerror.o

# Uncomment this out if debug
# gcc post.o -o post.exe -L../Error/ -lwinerror -lwininet

rm post.o
rm ../Error/winerror.o