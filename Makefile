all: start

start: main.o functions.o
	gcc -o start main.o functions.o -I/usr/include/postgresql -lpq

main.o: src/main.c files/functions.h
	gcc -o main.o src/main.c -c -I/usr/include/postgresql -lpq

functions.o: src/functions.c files/functions.h
	gcc -o functions.o src/functions.c -c -I/usr/include/postgresql -lpq

run: start
	./start localhost 5432 postgres postgres 123

clean:
	rm -f *.o start
