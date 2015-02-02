all: server_simple.c deliver_simple.c
	gcc -o server server_simple.c
	gcc -o deliver deliver_simple.c
