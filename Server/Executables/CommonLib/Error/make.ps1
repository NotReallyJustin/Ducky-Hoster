gcc -c ./winerror.c -o ./winerror.o -I./

# Make this a static library
ar -rcs libwinerror.a winerror.o

rm winerror.o