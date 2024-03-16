gcc -c ./common.c -o ./common.o -I./

# Make this a static library
# To use `libcommon.a`, just include common.h. For all intents and purposes, refer to this file as `common`. You will never need to write `libcommon`
ar -rcs libcommon.a common.o

rm common.o