gcc -O3 -Wall -Wextra -FPIC -c src/ush.c -o build/ush.o
ar rcs build/libush.a build/ush.o

gcc -O3 -Lbuild -Wall -Wextra src/main.c -o build/ush -lush