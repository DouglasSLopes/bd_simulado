#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "banco.h"

#define CMD_FIFO "db_fifo"
#define RESP_FIFO "db_fifo_resp"
#define BUFFER_SIZE 256

//COMANDOS PARA EXECUTAR SERVIDOR TEM QUE SER ABERTO PRIMEIRO
//gcc -pthread -o servidor servidor.c
//./servidor


// Banco de dados simulado (vetor de registros) e contador
Registro banco[MAX_REGISTROS];
int count_registros = 0;

// Mutex para proteger o acesso ao banco
pthread_mutex_t banco_mutex;
// Mutex para proteger escrita no pipe de resposta
pthread_mutex_t resp_mutex;

// File descriptor global para o FIFO de resposta
int resp_fd;

// Estrutura para passar argumentos à thread
typedef struct {
    char comando[BUFFER_SIZE];
} ThreadArg;

// Função para enviar mensagem ao pipe de resposta de forma thread-safe
void send_response(const char *msg) {
    pthread_mutex_lock(&resp_mutex);
    write(resp_fd, msg, strlen(msg) + 1);
    pthread_mutex_unlock(&resp_mutex);
}

// Função executada por cada thread para processar uma requisição
void *process_request(void *arg) {
    ThreadArg *targ = (ThreadArg *) arg;
    char cmd_copy[BUFFER_SIZE];
    
    // Copia o comando para uma variável local
    strcpy(cmd_copy, targ->comando);
    free(targ);

    char msg[1024] = {0};
    // Tokeniza o comando
    char *token = strtok(cmd_copy, " ");
    if (token == NULL) {
        pthread_exit(NULL);
    }
    
    if (strcmp(token, "INSERT") == 0) {
        // Espera-se: "INSERT id=VAL nome=VALUE"
        char *id_token = strtok(NULL, " ");
        char *nome_token = strtok(NULL, " ");
        if(id_token && nome_token) {
            int id;
            char nome[50];
            if (sscanf(id_token, "id=%d", &id) == 1 && sscanf(nome_token, "nome=%49[^\n]", nome) == 1) {
                pthread_mutex_lock(&banco_mutex);
                if (count_registros < MAX_REGISTROS) {
                    banco[count_registros].id = id;
                    strncpy(banco[count_registros].nome, nome, 50);
                    count_registros++;
                    sprintf(msg, "INSERT: Registro inserido: id=%d, nome=%s\n", id, nome);
                } else {
                    sprintf(msg, "INSERT: Banco cheio!\n");
                }
                pthread_mutex_unlock(&banco_mutex);
            } else {
                sprintf(msg, "INSERT: Comando mal formatado\n");
            }
        }
    } else if (strcmp(token, "DELETE") == 0) {
        // Espera-se: "DELETE id=VAL"
        char *id_token = strtok(NULL, " ");
        if(id_token) {
            int id;
            if (sscanf(id_token, "id=%d", &id) == 1) {
                pthread_mutex_lock(&banco_mutex);
                int found = 0;
                for (int i = 0; i < count_registros; i++) {
                    if (banco[i].id == id) {
                        // Remove o registro deslocando os demais
                        for (int j = i; j < count_registros - 1; j++) {
                            banco[j] = banco[j+1];
                        }
                        count_registros--;
                        found = 1;
                        sprintf(msg, "DELETE: Registro com id=%d removido\n", id);
                        break;
                    }
                }
                if (!found) {
                    sprintf(msg, "DELETE: Registro com id=%d nao encontrado\n", id);
                }
                pthread_mutex_unlock(&banco_mutex);
            } else {
                sprintf(msg, "DELETE: Comando mal formatado\n");
            }
        }
    } else if (strcmp(token, "SELECT") == 0) {
        // Operação extra: "SELECT id=VAL"
        char *id_token = strtok(NULL, " ");
        if (id_token) {
            int id;
            if (sscanf(id_token, "id=%d", &id) == 1) {
                pthread_mutex_lock(&banco_mutex);
                int found = 0;
                for (int i = 0; i < count_registros; i++) {
                    if (banco[i].id == id) {
                        sprintf(msg, "SELECT: Registro encontrado: id=%d, nome=%s\n", banco[i].id, banco[i].nome);
                        found = 1;
                        break;
                    }
                }
                if(!found) {
                    sprintf(msg, "SELECT: Registro com id=%d nao encontrado\n", id);
                }
                pthread_mutex_unlock(&banco_mutex);
            }
        }
    } else if (strcmp(token, "SHOW") == 0) {
        // Novo comando: exibe todos os registros atuais
        pthread_mutex_lock(&banco_mutex);
        sprintf(msg, "SHOW: Total de registros: %d\n", count_registros);
        for (int i = 0; i < count_registros; i++) {
            char linha[100];
            sprintf(linha, "id=%d, nome=%s\n", banco[i].id, banco[i].nome);
            strcat(msg, linha);
        }
        pthread_mutex_unlock(&banco_mutex);
    } else {
        sprintf(msg, "Comando desconhecido: %s\n", token);
    }

    // Envia a resposta de volta ao cliente
    send_response(msg);

    pthread_exit(NULL);
}

int main() {
    // Inicializa os mutexes
    pthread_mutex_init(&banco_mutex, NULL);
    pthread_mutex_init(&resp_mutex, NULL);

    // Cria os FIFOs, se não existirem
    if (mkfifo(CMD_FIFO, 0666) == -1) {
        // Se já existe, continua
    }
    if (mkfifo(RESP_FIFO, 0666) == -1) {
        // Se já existe, continua
    }

    // Abre o FIFO de comandos para leitura
    int cmd_fd = open(CMD_FIFO, O_RDONLY);
    if (cmd_fd < 0) {
        perror("Erro ao abrir FIFO de comandos");
        exit(EXIT_FAILURE);
    }

    // Abre o FIFO de respostas para escrita – 
    // IMPORTANTE: certifique-se que o cliente já está lendo para evitar bloqueios
    resp_fd = open(RESP_FIFO, O_WRONLY);
    if (resp_fd < 0) {
        perror("Erro ao abrir FIFO de respostas");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];

    printf("Servidor iniciado, aguardando requisicoes...\n");

    // Loop infinito para ler requisições
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = read(cmd_fd, buffer, BUFFER_SIZE);
        if (n > 0) {
            // Cria estrutura para argumentos da thread
            ThreadArg *targ = malloc(sizeof(ThreadArg));
            if(targ == NULL) {
                perror("malloc");
                continue;
            }
            strncpy(targ->comando, buffer, BUFFER_SIZE);
            pthread_t tid;
            // Cria thread para processar o comando
            if (pthread_create(&tid, NULL, process_request, (void *)targ) != 0) {
                perror("pthread_create");
                free(targ);
            }
            pthread_detach(tid);
        }
    }

    // Nunca atingido, mas boa prática:
    close(cmd_fd);
    close(resp_fd);
    pthread_mutex_destroy(&banco_mutex);
    pthread_mutex_destroy(&resp_mutex);

    return 0;
}
