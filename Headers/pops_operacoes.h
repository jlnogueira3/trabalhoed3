// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Operações de PoPs (trabalho 2): busca, remoção e inserção com Worst Fit.

#ifndef TRABALHOED3_POPS_OPERACOES_H
#define TRABALHOED3_POPS_OPERACOES_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "pops.h"

// Procura os registros que atendem aos critérios e guarda suas posições em *posicoes (liberar com
// free).
// Usa o índice quando há idPoPs entre os critérios; caso contrário, percorre o arquivo. Retorna 0
// em caso de erro.
int buscar_registros_pops(FILE *f, const cabecalho_pops *cabecalho, const indice_pops *indice,
                         const criterio_pops *criterios, int quantidade,
                         int64_t **posicoes, size_t *encontrados);

// Remove logicamente o registro em pos: ele entra no início da lista de removidos e sua chave sai
// do índice.
// Retorna 0 em caso de erro.
int remover_registro_pops(FILE *f, cabecalho_pops *cabecalho, indice_pops *indice,
                         int64_t pos, const dados_pops *dado);

// Insere um registro novo: reaproveita o maior removido que couber (Worst Fit) ou grava no fim do
// arquivo.
// Atualiza índice e contadores. Retorna 0 em caso de erro.
int inserir_registro_pops(FILE *f, cabecalho_pops *cabecalho, indice_pops *indice,
                          dados_pops *dado);

#endif // TRABALHOED3_POPS_OPERACOES_H
