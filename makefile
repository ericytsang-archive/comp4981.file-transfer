
CC = cc -Wall -W -Wextra -pedantic

example: example.o
	$(CC) -o ./example example.o

server: server.o
	$(CC) -o ./server server.o

clean:
	rm -f *.o core.* ./server ./example

server.o: server.c
	$(CC) -c server.c

example.o: example.c
	$(CC) -c example.c
