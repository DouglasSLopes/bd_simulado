#include "banco.h"         // Cabeçalho com operações do "banco de dados" (insert, select, etc.)
#include <stdio.h>         // Entrada/saída padrão
#include <stdlib.h>        // Funções utilitárias (malloc, free, etc.)
#include <string.h>        // Manipulação de strings
#include <pthread.h>       // Threads POSIX
#include <fcntl.h>         // Controle de arquivos (open)
#include <unistd.h>        // Funções POSIX (read, write, close, etc.)
#include <sys/stat.h>      // Permissões e tipos de arquivos (para mkfifo)

#define FIFO_PATH "/tmp/fifo_requisicoes"  // Caminho do FIFO nomeado
#define MAX_REQ 256                        // Tamanho máximo de uma requisição

// Mutex para evitar condições de corrida durante acesso ao "banco"
pthread_mutex_t mutex_banco;

// Função que será executada por cada thread criada para tratar uma requisição
void* tratar_requisicao(void *arg) {
    char *requisicao = (char *)arg;     // Converte argumento de void* para char*
    char resposta[100] = "";            // Buffer da resposta para exibir no terminal
    int id;
    char nome[50];

    // Seção crítica protegida por mutex (somente uma thread por vez acessa o banco)
    pthread_mutex_lock(&mutex_banco);

    // Verifica e trata cada tipo de requisição
    if (sscanf(requisicao, "INSERT id=%d nome=%s", &id, nome) == 2) {
        if (inserir(id, nome) == 0) {
            salvar_em_arquivo();  // Salva alterações no arquivo
            sprintf(resposta, "INSERT: OK\n");
        } else
            sprintf(resposta, "INSERT: ERRO (ID duplicado ou limite)\n");
    } else if (sscanf(requisicao, "DELETE id=%d", &id) == 1) {
        if (deletar(id) == 0) {
            salvar_em_arquivo();
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
            salvar_em_arquivo();
            sprintf(resposta, "UPDATE: OK\n");
        } else
            sprintf(resposta, "UPDATE: ERRO (não encontrado)\n");
    }

    // Libera o mutex após terminar a operação
    pthread_mutex_unlock(&mutex_banco);

    // Exibe no terminal do servidor o que foi recebido e a resposta
    printf("Requisição: %sResposta: %s", requisicao, resposta);

    // Libera memória alocada com strdup()
    free(requisicao);
    pthread_exit(NULL); // Finaliza a thread
}

int main() {
    inicializar_banco(); // Carrega registros previamente salvos (de banco.txt)
    pthread_mutex_init(&mutex_banco, NULL); // Inicializa mutex

    mkfifo(FIFO_PATH, 0666); // Cria FIFO nomeado (modo leitura/escrita para todos)

    printf("Servidor pronto. Aguardando requisições...\n");

    while (1) {
        char buffer[MAX_REQ];  // Buffer para ler requisição

        // Abre FIFO para leitura. Bloqueia até que um processo abra para escrita.
        int fd = open(FIFO_PATH, O_RDONLY);
        if (fd < 0) {
            perror("Erro ao abrir FIFO"); // Exibe erro se falhar
            continue;
        }

        ssize_t bytes_lidos;
        // Lê dados do FIFO. Loop interno permite múltiplas requisições seguidas.
        while ((bytes_lidos = read(fd, buffer, sizeof(buffer)-1)) > 0) {
            buffer[bytes_lidos] = '\0'; // Termina string com '\0'
            char *requisicao = strdup(buffer); // Duplica string para evitar sobrescrever
            pthread_t tid;
            // Cria uma thread para tratar essa requisição
            pthread_create(&tid, NULL, tratar_requisicao, requisicao);
            pthread_detach(tid); // Libera recursos automaticamente após terminar
        }

        close(fd); // Fecha FIFO após ler
    }

    pthread_mutex_destroy(&mutex_banco); // Destroi mutex (nunca será chamado nesse loop infinito)
    unlink(FIFO_PATH); // Remove FIFO do sistema (opcional, também nunca será alcançado)
    return 0;
}