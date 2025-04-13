#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define CMD_FIFO "db_fifo"
#define RESP_FIFO "db_fifo_resp"
#define BUFFER_SIZE 256

//COMANDOS PARA EXECUTAR SERVIDOR TEM QUE SER ABERTO PRIMEIRO
//gcc -pthread -o cliente cliente.c
//./cliente



// Função executada pela thread que lê as respostas do servidor
void *read_responses(void *arg) {
    int resp_fd = *(int *)arg;
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = read(resp_fd, buffer, BUFFER_SIZE);
        if (n > 0) {
            printf("\n---- Resposta do Servidor ----\n%s\n------------------------------\n", buffer);
        }
    }
    pthread_exit(NULL);
}

int main() {
    int cmd_fd, resp_fd;
    char buffer[BUFFER_SIZE];

    // Abre o FIFO de comandos para escrita
    cmd_fd = open(CMD_FIFO, O_WRONLY);
    if (cmd_fd < 0) {
        perror("Erro ao abrir FIFO de comandos para escrita");
        exit(EXIT_FAILURE);
    }

    // Abre o FIFO de respostas para leitura
    resp_fd = open(RESP_FIFO, O_RDONLY);
    if (resp_fd < 0) {
        perror("Erro ao abrir FIFO de respostas para leitura");
        exit(EXIT_FAILURE);
    }

    // Cria uma thread para ler as respostas do servidor
    pthread_t reader_thread;
    if (pthread_create(&reader_thread, NULL, read_responses, (void *)&resp_fd) != 0) {
        perror("Erro ao criar thread de leitura");
        exit(EXIT_FAILURE);
    }

    // Loop para enviar comandos
    while (1) {
        printf("\nDigite um comando (ex: INSERT id=7 nome=Joao, DELETE id=7, SELECT id=7, SHOW) ou 'exit' para sair:\n");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        // Remove o '\n'
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        if (write(cmd_fd, buffer, strlen(buffer) + 1) < 0) {
            perror("Erro ao escrever no FIFO de comandos");
        }
    }

    close(cmd_fd);
    close(resp_fd);
    pthread_cancel(reader_thread); // Encerra a thread de leitura
    pthread_join(reader_thread, NULL);

    return 0;
}
