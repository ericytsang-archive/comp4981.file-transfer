
CC = cc -Wall -W -Wextra -pedantic

example: example.o
	$(CC) -o ./example.out example.o

server: server.o
	$(CC) -o ./server.out server.o

clean:
	rm -f *.o core.* ./server.out ./example.out

server.o: server.c
	$(CC) -c server.c

example.o: example.c
	$(CC) -c example.c
