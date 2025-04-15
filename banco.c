#include "banco.h"          
#include <stdio.h>          
#include <string.h>         

// Banco de dados em memória: um array de registros
Registro banco[MAX_REGISTROS];
int total_registros = 0;    // Quantidade atual de registros no banco

// Função para carregar registros do arquivo "banco.txt" para a memória
void inicializar_banco() {
    FILE *fp = fopen("banco.txt", "r");  // Abre o arquivo para leitura
    if (fp) {
        // Lê linha por linha: um ID e um nome
        while (fscanf(fp, "%d %s", &banco[total_registros].id, banco[total_registros].nome) == 2) {
            total_registros++;  // Atualiza o contador a cada linha lida
        }
        fclose(fp);  // Fecha o arquivo após a leitura
    }
}

// Insere um novo registro no banco
int inserir(int id, const char *nome) {
    // Verifica se já existe um registro com o mesmo ID
    for (int i = 0; i < total_registros; i++) {
        if (banco[i].id == id)
            return -1; // Falha: ID duplicado
    }

    // Se ainda houver espaço, adiciona o novo registro
    if (total_registros < MAX_REGISTROS) {
        banco[total_registros].id = id;
        strncpy(banco[total_registros].nome, nome, 49); // Copia o nome (máximo 49 caracteres)
        total_registros++;  // Atualiza o contador
        return 0; // Sucesso
    }

    return -1; // Falha: banco cheio
}

// Remove um registro pelo ID
int deletar(int id) {
    for (int i = 0; i < total_registros; i++) {
        if (banco[i].id == id) {
            // Substitui o registro atual pelo último registro do array
            banco[i] = banco[total_registros - 1];
            total_registros--; // Decrementa o contador
            return 0; // Sucesso
        }
    }
    return -1; // Falha: ID não encontrado
}

// Busca um registro pelo ID e copia o nome correspondente para nome_out
int selecionar(int id, char *nome_out) {
    for (int i = 0; i < total_registros; i++) {
        if (banco[i].id == id) {
            strcpy(nome_out, banco[i].nome); // Copia o nome para o ponteiro de saída
            return 0; // Sucesso
        }
    }
    return -1; // Falha: ID não encontrado
}

// Atualiza o nome de um registro identificado pelo ID
int atualizar(int id, const char *novo_nome) {
    for (int i = 0; i < total_registros; i++) {
        if (banco[i].id == id) {
            strncpy(banco[i].nome, novo_nome, 49); // Atualiza o nome
            return 0; // Sucesso
        }
    }
    return -1; // Falha: ID não encontrado
}

// Salva todos os registros atuais no arquivo "banco.txt"
void salvar_em_arquivo() {
    FILE *fp = fopen("banco.txt", "w"); // Abre o arquivo para escrita (sobrescreve o existente)
    if (fp) {
        for (int i = 0; i < total_registros; i++) {
            // Escreve ID e nome de cada registro no arquivo
            fprintf(fp, "%d %s\n", banco[i].id, banco[i].nome);
        }
        fclose(fp); // Fecha o arquivo após salvar
    }
}