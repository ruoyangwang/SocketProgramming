all: server.c deliver.c
	gcc -o server server.c
	gcc -o deliver deliver.c
clean: 
	${RM} *.c~ deliver server *.~
