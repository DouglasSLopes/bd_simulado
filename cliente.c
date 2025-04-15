#include <stdio.h>     // Biblioteca padrão de entrada/saída (fgets, printf, perror)
#include <stdlib.h>    // Biblioteca padrão para funções utilitárias (não usada diretamente aqui)
#include <string.h>    // Biblioteca para manipulação de strings (strlen)
#include <fcntl.h>     // Para controle de arquivos (open, O_WRONLY)
#include <unistd.h>    // Para chamadas de sistema Unix (write, close)

#define FIFO_PATH "/tmp/fifo_requisicoes" // Caminho absoluto para a FIFO usada para comunicação

int main() {
    char comando[256]; // Buffer onde será armazenado o comando digitado pelo usuário

    while (1) {
        // Exibe mensagem para o usuário inserir uma requisição
        printf("\nDigite a requisição (ex: INSERT id=1 nome=Joao):\n> ");
        
        // Lê a linha digitada do teclado
        if (!fgets(comando, sizeof(comando), stdin)) break;

        // Abre o FIFO para escrita. O_WRONLY significa "somente escrita"
        int fd = open(FIFO_PATH, O_WRONLY);
        if (fd < 0) {
            perror("Erro ao abrir FIFO"); // Mostra erro caso o FIFO não exista ou não possa ser aberto
            continue;
        }

        // Escreve o comando no FIFO. O '+1' é para incluir o caractere nulo '\0' no final da string
        write(fd, comando, strlen(comando) + 1);

        // Fecha o descritor de arquivo (libera o recurso)
        close(fd);
    }

    return 0;
}