// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Funcionalidades 8 e 9 (trabalho 2): criação do arquivo de dados e do índice primário.

#ifndef TRABALHOED3_POPS_CRIACAO_H
#define TRABALHOED3_POPS_CRIACAO_H

// Lê o CSV de PoPs e grava o arquivo binário de dados.
// A primeira linha do CSV diz o que cada coluna representa, então as colunas
// são localizadas pelo nome, e não pela posição.
void funcionalidade8(const char *csv, const char *binario);

// Cria o arquivo de índice primário a partir do arquivo de dados PoPs.
// O índice só é criado depois que o arquivo de dados foi lido por completo.
void funcionalidade9(const char *binario_dados, const char *binario_indice);

#endif // TRABALHOED3_POPS_CRIACAO_H
