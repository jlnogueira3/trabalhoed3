// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Funcionalidades 12, 13 e 14 (trabalho 2): remoção, inserção e atualização de PoPs.

#ifndef TRABALHOED3_POPS_MANIPULACAO_H
#define TRABALHOED3_POPS_MANIPULACAO_H

// Remove logicamente os registros encontrados, colocando os espacos na lista
// de removidos e retirando as chaves correspondentes do indice.
void funcionalidade12(const char *binario, const char *nome_indice, int n);

// Insere os registros usando o maior espaco removido que comportar os dados.
// Quando nenhum servir, escreve no final do arquivo e atualiza o proximo offset.
void funcionalidade13(const char *binario, const char *nome_indice, int n);

// Atualiza os PoPs que atendem aos critérios. Se o registro crescer, ele é removido e reinserido.
// O índice é atualizado ao final, em finalizar_alteracoes_pops.
void funcionalidade14(const char *binario, const char *nome_indice, int n);

#endif // TRABALHOED3_POPS_MANIPULACAO_H
