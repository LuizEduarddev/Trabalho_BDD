CC = gcc
CFLAGS = -I/usr/include/postgresql
LDFLAGS = -lpq

all: start

start: main.o functions.o
	$(CC) -o start main.o functions.o $(LDFLAGS) 

main.o: src/main.c files/functions.h
	$(CC) $(CFLAGS) src/main.c -c 

functions.o: src/functions.c files/functions.h
	$(CC) $(CFLAGS) src/functions.c -c 

run: start
	./start localhost 5432 postgres postgres 1234 

clean:
	rm start
	rm -f functions.o
	rm -f main.o
