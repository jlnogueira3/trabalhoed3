// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Registros de PoPs (trabalho 2): leitura, escrita, impressão e critérios sobre um registro.

#ifndef TRABALHOED3_POPS_REGISTRO_H
#define TRABALHOED3_POPS_REGISTRO_H

#include <stdio.h>
#include <stdint.h>

#include "pops.h"

// Essa leitura tambem e usada ao percorrer a lista de removidos.
int ler_controle_pops(FILE *f, int64_t pos, int64_t fim, dados_pops *dado);

// Libera os nomes alocados em dado e zera os ponteiros, evitando liberação dupla.
void destroi_dados_pops(dados_pops *dado);

// Lê o registro de PoPs que começa em pos. Retorna 1 se o registro foi lido ou pulado (removido),
// e 0 em caso de erro. Os nomes são alocados e devem ser liberados com destroi_dados_pops.
int leitura_dados_pops(FILE *f, int64_t pos, int64_t fim, dados_pops *dado);

// Calcula o tamanho físico do registro (controle, id, sigla e nomes com delimitadores).
// Retorna 0 se o tamanho não couber em int32_t.
int tamanho_dados_pops(const dados_pops *dado, int32_t *tamanho);

// Grava 'quantidade' bytes de lixo ('$') na posição atual do arquivo.
int escrever_lixo_pops(FILE *f, int32_t quantidade);

// Grava o registro na posição pos, campo a campo, e preenche a sobra do espaço físico com lixo.
// Retorna 0 em caso de erro.
int escrever_dados_pops(FILE *f, int64_t pos, const dados_pops *dado);

// Imprime um registro de PoPs. Valores vazios ou nulos aparecem como NULO.
void print_dados_pops(const dados_pops *dado);

// Verifica se o registro atende a todos os critérios. Retorna 1 se atende, 0 caso contrário.
// Campos desconhecidos nunca batem com o registro.
int atende_criterios_pops(const dados_pops *dado, const criterio_pops *criterios,
                                 int quantidade);

#endif // TRABALHOED3_POPS_REGISTRO_H
