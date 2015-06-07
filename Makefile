client:
	$(CC) simple_client.c -o simpleClient

server:
	$(CC) simple_server.c -o simpleServer

all:    server client

clean:
	rm -f simpleServer
	rm -f simpleClient
