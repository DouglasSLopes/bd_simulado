#ifndef BANCO_H
#define BANCO_H

#define MAX_REGISTROS 100     // Define o número máximo de registros que o banco pode armazenar
#define TAM_NOME 50           // Define o tamanho máximo permitido para o nome de cada registro

// Define a estrutura de um registro no banco
typedef struct {
    int id;                   // Identificador único do registro
    char nome[TAM_NOME];      // Nome associado ao ID (array de caracteres)
} Registro;

// Declaração de variáveis globais que serão definidas em um arquivo .c (ex: banco.c)
extern Registro banco_de_dados[MAX_REGISTROS]; // Array de registros (o "banco de dados" em memória)
extern int total_registros;                    // Quantidade atual de registros inseridos

// Declaração das funções que serão implementadas em banco.c
void inicializar_banco();                       // Carrega os dados de um arquivo para memória
int inserir(int id, const char *nome);          // Insere um novo registro
int deletar(int id);                            // Deleta um registro pelo ID
int selecionar(int id, char *out_nome);         // Busca um registro pelo ID e retorna o nome
int atualizar(int id, const char *novo_nome);   // Atualiza o nome de um registro
void salvar_em_arquivo();                       // Salva os dados atuais em um arquivo de texto

#endif
// Fim da diretiva de inclusão condicional