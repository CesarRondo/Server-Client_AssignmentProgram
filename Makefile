$(CC) = gcc

server: myServer.c functs.c
		$(CC) myServer.c functs.c -o server
client: myClient.c functs.c
		$(CC) myClient.c functs.c -o client
all: server client
clean:
		rm -f server
		rm -f client