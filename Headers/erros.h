// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Mensagens de falha e verificação de abertura de arquivos.

#ifndef TRABALHOED3_ERROS_H
#define TRABALHOED3_ERROS_H

#include <stdio.h>

// Exibe a mensagem padrão de falha e fecha o arquivo (se estiver aberto).
void falha_processamento_arquivo(FILE *f);

// Retorna 1 se o arquivo foi aberto com sucesso.
// Caso contrário, exibe a mensagem padrão de falha e retorna 0.
int verificar_abertura_arquivo(FILE *f);

#endif // TRABALHOED3_ERROS_H
