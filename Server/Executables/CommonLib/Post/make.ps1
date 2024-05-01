# gcc -c ./post.c -o ./post.o -I./

# # Make this a static library
# # To use `libpost.a`, just include post.h. For all intents and purposes, refer to this file as `post`. You will never need to write `libpost`
# ar -rcs libpost.a post.o

# rm post.o

gcc -c post.c -o post.o -I./
gcc post.o -o post.exe -L../Error/ -lwinerror -lwininet

rm post.o