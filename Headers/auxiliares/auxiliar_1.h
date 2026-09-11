//
// Created by joao-lucas-nogueira on 11/09/2026.
//

#ifndef TRABALHOED3_AUXILIAR_1_H
#define TRABALHOED3_AUXILIAR_1_H
// Criei esse arquivo para ajudar na modularização das funções implementadas
// A fim de facilitar a legibilidade das funções
// Primeiro fazemos a inicialização e a escrita do registro de
// cabeçalho
void inicializar_cabeçalho(registro_cabecalho *cabecalho);
void escrever_cabeçalho(const registro_cabecalho *cabecalho,FILE *arquivo);


// Agora partimos para a inicialização e a escrita do registro
// de dados
void escrever_dados(registro_dados dados, FILE *arquivo);
void print_dados(registro_dados dados);
void leitura_dados(registro_dados *dados, FILE *arquivo);
int filtrar_dados(struct chave_valor *p, int quantidade_pares, struct registro_dados dados, int filtro);


#endif //TRABALHOED3_AUXILIAR_1_H
