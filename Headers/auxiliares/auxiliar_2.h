//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#ifndef TRABALHOED3_AUXILIAR_2_H
#define TRABALHOED3_AUXILIAR_2_H

#include <stdio.h>
#include "../pops.h"

int posicionar_pops(FILE *f, int64_t pos);
int leitura_cabecalho_pops(FILE *f, cabecalho_pops *cabecalho);
int escrever_cabecalho_pops(FILE *f, const cabecalho_pops *cabecalho);
int leitura_dados_pops(FILE *f, int64_t pos, int64_t fim, dados_pops *dado);
int escrever_dados_pops(FILE *f, int64_t pos, const dados_pops *dado);
void destroi_dados_pops(dados_pops *dado);
void print_dados_pops(const dados_pops *dado);
int tamanho_dados_pops(const dados_pops *dado, int32_t *tamanho);

int ler_quantidade_pops(int *quantidade);
int ler_insercao_pops(dados_pops *dado);
int ler_criterios_pops(criterio_pops **criterios, int *quantidade);
void destroi_criterios_pops(criterio_pops *criterios, int quantidade);
int atualizar_campos_pops(dados_pops *dado, const criterio_pops *campos, int quantidade);

int abrir_arquivos_pops(const char *binario, const char *nome_indice, int editar,
                       FILE **f, FILE **fi, cabecalho_pops *cabecalho, indice_pops *indice);
int fechar_arquivos_pops(FILE *f, FILE *fi, indice_pops *indice);
int iniciar_alteracoes_pops(FILE *f, FILE *fi);
int finalizar_alteracoes_pops(FILE *f, FILE **fi, const char *nome_indice,
                             cabecalho_pops *cabecalho, indice_pops *indice);
int buscar_indice_pops(const indice_pops *indice, int32_t id, size_t *posicao);
int buscar_registros_pops(FILE *f, const cabecalho_pops *cabecalho, const indice_pops *indice,
                         const criterio_pops *criterios, int quantidade,
                         int64_t **posicoes, size_t *encontrados);
int remover_registro_pops(FILE *f, cabecalho_pops *cabecalho, indice_pops *indice,
                         int64_t pos, const dados_pops *dado);
int inserir_registro_pops(FILE *f, cabecalho_pops *cabecalho, indice_pops *indice,
                         dados_pops *dado);

#endif
