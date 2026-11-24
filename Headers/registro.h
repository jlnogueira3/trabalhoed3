// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Registros de dados do trabalho 1: posicionamento, leitura, escrita e impressão.

#ifndef TRABALHOED3_REGISTRO_H
#define TRABALHOED3_REGISTRO_H

#include <stdio.h>

#include "registros.h"

#define LEITURA_ERRO 0       // falha ao ler o registro
#define LEITURA_OK 1         // registro lido por completo
#define LEITURA_REMOVIDO 2   // registro logicamente removido: os bytes restantes foram pulados

// Calcula o byte offset do registro de RRN informado.
long posicao_registro(int rrn);

// Lê um registro na posição atual do arquivo.
// Retorna:
//   LEITURA_OK       se o registro foi lido por completo;
//   LEITURA_REMOVIDO se o registro está removido (seu conteúdo não é lido,
//                    apenas os bytes restantes são pulados);
//   LEITURA_ERRO     em caso de erro de leitura.
int leitura_dados(registro_dados *dados, FILE *arquivo);

// Escreve um registro completo na posição atual do arquivo.
void escrever_dados(registro_dados dados, FILE *arquivo);

// Imprime um registro na saída padrão, substituindo os valores nulos por NULO.
void print_dados(registro_dados dados);

#endif // TRABALHOED3_REGISTRO_H
