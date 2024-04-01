# Add multiple -I flags to include more header file locations
gcc -c ./post.c -o post.o -I../Libcurl/ -I./

ar -rcs ./libpost.a ./post.o

rm ./post.o