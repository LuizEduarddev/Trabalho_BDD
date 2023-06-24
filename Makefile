main: src/main.c
	gcc -o  main src/main.c src/functions.c -I/usr/include/postgresql -lpq

run: main
	./main

clean:
	rm -f main