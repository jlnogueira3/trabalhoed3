// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Funcionalidades 10 e 11 (trabalho 2): listagem e busca de PoPs.

#ifndef TRABALHOED3_POPS_CONSULTA_H
#define TRABALHOED3_POPS_CONSULTA_H

// Percorre todos os registros do arquivo, do primeiro ao último byte válido.
void funcionalidade10(const char *binario);

// Busca os PoPs que atendem aos critérios. Se houver idPoPs entre os campos, o índice é usado;
// caso contrário, o arquivo é percorrido sequencialmente.
void funcionalidade11(const char *binario, const char *nome_indice, int n);

#endif // TRABALHOED3_POPS_CONSULTA_H
