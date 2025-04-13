// servidor.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "banco.h"

#define FIFO_PATH "/tmp/fifo_requisicoes"
#define MAX_REQ 256

pthread_mutex_t mutex_banco;

void* tratar_requisicao(void *arg) {
    char *requisicao = (char *)arg;
    char resposta[100] = "";
    int id;
    char nome[50];

    pthread_mutex_lock(&mutex_banco); // Protege acesso ao banco

    if (sscanf(requisicao, "INSERT id=%d nome=%s", &id, nome) == 2) {
        if (inserir(id, nome) == 0) {
            salvar_em_arquivo();  // <-- só aqui
            sprintf(resposta, "INSERT: OK\n");
        } else
            sprintf(resposta, "INSERT: ERRO (ID duplicado ou limite)\n");
    } else if (sscanf(requisicao, "DELETE id=%d", &id) == 1) {
        if (deletar(id) == 0) {
            salvar_em_arquivo();  // <-- aqui também
            sprintf(resposta, "DELETE: OK\n");
        } else
            sprintf(resposta, "DELETE: ERRO (não encontrado)\n");
    } else if (sscanf(requisicao, "SELECT id=%d", &id) == 1) {
        if (selecionar(id, nome) == 0)
            sprintf(resposta, "SELECT: nome=%s\n", nome);
        else
            sprintf(resposta, "SELECT: ERRO (não encontrado)\n");
    } else if (sscanf(requisicao, "UPDATE id=%d nome=%s", &id, nome) == 2) {
        if (atualizar(id, nome) == 0) {
            salvar_em_arquivo();  // <-- também aqui
            sprintf(resposta, "UPDATE: OK\n");
        } else
            sprintf(resposta, "UPDATE: ERRO (não encontrado)\n");
    }    

    salvar_em_arquivo();
    pthread_mutex_unlock(&mutex_banco); // Libera acesso

    printf("Requisição: %sResposta: %s", requisicao, resposta);
    free(requisicao);
    pthread_exit(NULL);
}

int main() {
    inicializar_banco();
    pthread_mutex_init(&mutex_banco, NULL);

    mkfifo(FIFO_PATH, 0666); // Cria FIFO

    printf("Servidor pronto. Aguardando requisições...\n");

    while (1) {
        char buffer[MAX_REQ];
        int fd = open(FIFO_PATH, O_RDONLY);
        if (fd < 0) {
            perror("Erro ao abrir FIFO");
            continue;
        }

        ssize_t bytes_lidos;
        while ((bytes_lidos = read(fd, buffer, sizeof(buffer)-1)) > 0) {
            buffer[bytes_lidos] = '\0'; // Garante string terminada
            char *requisicao = strdup(buffer); // Cópia segura
            pthread_t tid;
            pthread_create(&tid, NULL, tratar_requisicao, requisicao);
            pthread_detach(tid);
        }  

        close(fd);
    }

    pthread_mutex_destroy(&mutex_banco);
    unlink(FIFO_PATH); // Remove FIFO ao final (opcional)
    return 0;
}
