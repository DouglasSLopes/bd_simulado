#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_PATH "/tmp/fifo_requisicoes"

int main() {
    char comando[256];

    while (1) {
        printf("\nDigite a requisição (ex: INSERT id=1 nome=Joao):\n> ");
        if (!fgets(comando, sizeof(comando), stdin)) break;

        int fd = open(FIFO_PATH, O_WRONLY);
        if (fd < 0) {
            perror("Erro ao abrir FIFO");
            continue;
        }

        write(fd, comando, strlen(comando) + 1); // Inclui '\0'
        close(fd);
    }

    return 0;
}
