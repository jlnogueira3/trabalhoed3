// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Critérios de busca e atualização do trabalho 1 (pares campo/valor).

#ifndef TRABALHOED3_CRITERIOS_H
#define TRABALHOED3_CRITERIOS_H

#include "registros.h"

// Lê 'quantidade' pares campo/valor da entrada padrão.
// Retorna um vetor alocado com calloc (quem chama deve dar free),
// ou NULL se a quantidade for inválida ou se a alocação falhar.
// Valores NULO são guardados como string vazia.
struct chave_valor *ler_pares(int quantidade);

// Verifica se o registro atende a todos os critérios.
// Retorna 1 se atende a todos, 0 caso contrário.
// Campos desconhecidos nunca batem com o registro.
int atende_criterios(const registro_dados *dado, const struct chave_valor *criterios,
                     int quantidade);

#endif // TRABALHOED3_CRITERIOS_H
