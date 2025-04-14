#ifndef BANCO_H
#define BANCO_H

#define MAX_REGISTROS 100
#define TAM_NOME 50

typedef struct
{
    int id;
    char nome[TAM_NOME];
} Registro;

extern Registro banco_de_dados[MAX_REGISTROS];
extern int total_registros;

void inicializar_banco();
int inserir(int id, const char *nome);
int deletar(int id);
int selecionar(int id, char *out_nome);
int atualizar(int id, const char *novo_nome);
void salvar_em_arquivo();
void carregar_do_arquivo();

#endif
