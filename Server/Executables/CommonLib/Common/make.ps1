# To compile common lib, link liberror as a static library
gcc -m32 -c ./common.c -o ./common.o -I./
gcc -c -m32 ../Error/winerror.c -o ../Error/winerror.o

# Make this a static library
# To use `libcommon.a`, just include common.h. For all intents and purposes, refer to this file as `common`. You will never need to write `libcommon`
ar -rcs libcommon.a common.o ../Error/winerror.o

rm common.o
rm ../Error/winerror.o