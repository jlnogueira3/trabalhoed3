//
// Created by joao-lucas-nogueira on 11/09/2026.
//

#ifndef TRABALHOED3_AUXILIAR_1_H
#define TRABALHOED3_AUXILIAR_1_H
// Criei esse arquivo para ajudar na modularização das funções implementadas
// A fim de facilitar a legibilidade das funções
// Primeiro fazemos a inicialização e a escrita do registro de
// cabeçalho
void inicializar_cabecalho(registro_cabecalho *cabecalho);
void escrever_cabecalho(const registro_cabecalho *cabecalho,FILE *arquivo);


// Agora partimos para a inicialização e a escrita do registro
// de dados
void escrever_dados(registro_dados dados, FILE *arquivo);
void print_dados(registro_dados dados);
int leitura_dados(registro_dados *dados, FILE *arquivo);
int filtrar_dados(struct chave_valor *p, int quantidade_pares, struct registro_dados dados, int filtro);
int leitura_cabecalho(FILE *f, registro_cabecalho *cabecalho);
int atende_criterios(const registro_dados *dado, const struct chave_valor *criterios, int quantidade);
void falha_processamento_arquivo(FILE *f);
int verificar_abertura_arquivo(FILE *f);

#endif //TRABALHOED3_AUXILIAR_1_H
