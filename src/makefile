
CC = cc -Wall -W -Wextra -pedantic



clean:
	rm -f *.o *.out



# executables
server: server.o messagequeuehelper.o session.o
	$(CC) -o ./server.out server.o messagequeuehelper.o session.o

client: client.o messagequeuehelper.o
	$(CC) -o ./client.out client.o messagequeuehelper.o



# main modules
server.o: server.c
	$(CC) -c server.c

client.o: client.c
	$(CC) -c client.c



# common helper modules
messagequeuehelper.o: messagequeuehelper.c
	$(CC) -c messagequeuehelper.c



# server helper modules
session.o: session.c
	$(CC) -c session.c
