/*
 * João L A Nogueira - N usp: 16986970
 */
//
// Created by joao-lucas-nogueira on 11/09/2026.
//

#ifndef TRABALHOED3_AUXILIAR_1_H
#define TRABALHOED3_AUXILIAR_1_H

#include <stdio.h>
#include "../registros.h"

// Resultados possíveis de leitura_dados
#define LEITURA_ERRO 0       // falha ao ler o registro
#define LEITURA_OK 1         // registro lido por completo
#define LEITURA_REMOVIDO 2   // registro logicamente removido: os bytes restantes foram pulados

// Cabeçalho: inicialização, leitura, escrita e posicionamento
void inicializar_cabecalho(registro_cabecalho *cabecalho);
int leitura_cabecalho(FILE *f, registro_cabecalho *cabecalho);
void escrever_cabecalho(const registro_cabecalho *cabecalho, FILE *arquivo);
void salvar_cabecalho(FILE *f, const registro_cabecalho *cabecalho);

// Registros de dados: posicionamento, leitura, escrita e impressão
long posicao_registro(int rrn);
int leitura_dados(registro_dados *dados, FILE *arquivo);
void escrever_dados(registro_dados dados, FILE *arquivo);
void print_dados(registro_dados dados);

// Critérios de busca e atualização
struct chave_valor *ler_pares(int quantidade);
int filtrar_dados(struct chave_valor *p, int quantidade_pares, registro_dados dados, int filtro);
int atende_criterios(const registro_dados *dado, const struct chave_valor *criterios, int quantidade);

// Mensagens de erro e verificação de abertura
void falha_processamento_arquivo(FILE *f);
int verificar_abertura_arquivo(FILE *f);

#endif //TRABALHOED3_AUXILIAR_1_H
