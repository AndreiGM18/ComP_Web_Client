CC=gcc
CFLAGS=-g -Wall -Wextra -Werror -pedantic

client: client.c requests.c helpers.c buffer.c parson.c
	$(CC) -o client client.c requests.c snd_recv.c helpers.c buffer.c parson.c utils.c -I.

run: client
	./client

clean:
	rm -f *.o client
