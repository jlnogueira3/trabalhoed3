// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Entrada de dados de PoPs (trabalho 2): leitura de valores, critérios e registros da
// entrada padrão.

#ifndef TRABALHOED3_POPS_ENTRADA_H
#define TRABALHOED3_POPS_ENTRADA_H

#include "pops.h"

// Lê um inteiro não negativo da entrada padrão (quantidade de solicitações ou de critérios).
// Retorna 1 se o valor for válido, e 0 caso contrário.
int ler_quantidade_pops(int *quantidade);

// Libera o vetor de critérios e as strings de chave e valor de cada um.
void destroi_criterios_pops(criterio_pops *criterios, int quantidade);

// Essa leitura e a mesma nas buscas, remocoes e atualizacoes.
int ler_criterios_pops(criterio_pops **criterios, int *quantidade);

// Lê os quatro campos de um novo PoPs (idPoPs, nomePoPs, nomePais, siglaPais), valida e prepara o
// registro.
// Retorna 1 se estiver tudo certo. Em caso de erro, libera a memória já alocada.
int ler_insercao_pops(dados_pops *dado);

// Aplica os campos informados ao registro em memória. Retorna 0 se faltar memória ou se o valor for
// inválido.
int atualizar_campos_pops(dados_pops *dado, const criterio_pops *campos, int quantidade);

#endif // TRABALHOED3_POPS_ENTRADA_H
