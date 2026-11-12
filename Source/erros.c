// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/erros.h"

#include <stdio.h>
#include <stdlib.h>

// Exibe a mensagem padrão de falha e fecha o arquivo (se estiver aberto).
void falha_processamento_arquivo(FILE *f) {
    printf("Falha no processamento do arquivo.\n");
    if (f != NULL)
        fclose(f);
}

// Retorna 1 se o arquivo foi aberto com sucesso.
// Caso contrário, exibe a mensagem padrão de falha e retorna 0.
int verificar_abertura_arquivo(FILE *f) {
    if (f == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    return 1;
}
