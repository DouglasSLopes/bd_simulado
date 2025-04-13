all: servidor cliente

servidor: servidor.c banco.c
	gcc -o servidor servidor.c banco.c -lpthread

cliente: cliente.c
	gcc -o cliente cliente.c
