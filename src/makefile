
CC = cc -Wall -W -Wextra -pedantic

clean:
	rm -f *.o core.* ./server.out ./example.out



# executables
example: example.o
	$(CC) -o ./example.out example.o

server: server.o messagequeuehelper.o
	$(CC) -o ./server.out server.o messagequeuehelper.o

client: client.o messagequeuehelper.o
	$(CC) -o ./client.out client.o messagequeuehelper.o



# main modules
example.o: example.c
	$(CC) -c example.c

server.o: server.c
	$(CC) -c server.c

client.o: client.c
	$(CC) -c client.c



# helper modules
messagequeuehelper.o: messagequeuehelper.c
	$(CC) -c messagequeuehelper.c
