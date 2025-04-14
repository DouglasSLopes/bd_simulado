# Avaliação M1 – IPC, Threads e Paralelismo de Sistema Operacionais (SO)

Esse projeto é trabalho de SO de um Sistema de Processamento Paralelo de Requisições a um Banco de Dado.

# Como rodar?

Informação **importante**: o projeto só foi testado no ```GitHub Codespaces```, não foi testando em outros locais, portando pode ocorrer erros se for testada em outros IDEs.

## Siga os seguintes comandos para rodar

>Primeiramente, esteja na pasta ```bd_simulado```, se não estiver escreve o seguinte comando no terminal ```cd_simulado```
>depois, escreve o comando ```make``` no terminal para rodar os codigos que estão nele.
>Por fim crie ```2 terminal```, um para o ```servidor``` e outro para o ```cliente```, no servidor se digita o comando ```./servidor``` e no cliente ```./cliente```.

Pronto, o seu código está funcionando.

>Caso queira testar, escreva um ```INSERT id=1 nome=Joao``` no terminal onde está o ```./cliente```, e para verificar se está funcionando vai para o terminal onde está o ```./servidor``` que irá mostra se o ```requisição foi realizada com sucesso```, caso queira ver todos os dados vai para o arquivo ```banco.txt```.


## Instalações 

**Baixe as seguintes extensões:** <br>
C/C++ Extension Pack <br>
Code Runner <br>
Makefile Tools <br>

Obs: normalmente quando você abre os códigos e acessa os arquivos o VS Code já vai recomendar baixas eles.

## Informações sobre o Make

Caso você escreve ```make``` e apareça a seguinte mensagem ```make: Nothing to be done for 'all'```, já está funcionando, portando só criar os terminais e adicionar os comando do servidor e para o cliente.
