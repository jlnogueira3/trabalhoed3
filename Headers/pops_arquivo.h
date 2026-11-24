// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Arquivos de PoPs (trabalho 2): cabeçalho, status, abertura, fechamento e índice primário.

#ifndef TRABALHOED3_POPS_ARQUIVO_H
#define TRABALHOED3_POPS_ARQUIVO_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "pops.h"

// O byte offset é controlado pelas próprias leituras, sem consultar ftell.
// No Windows, long pode ter 4 bytes. O deslocamento é dividido para não truncar o offset.
int posicionar_pops(FILE *f, int64_t pos);

// Lê o cabeçalho do arquivo de PoPs. Retorna 1 se o arquivo estiver consistente e o
// cabeçalho for lido por completo, e 0 caso contrário. Ao retornar 1, o ponteiro fica logo após o
// cabeçalho.
int leitura_cabecalho_pops(FILE *f, cabecalho_pops *cabecalho);

// Os campos são gravados um a um, para não levar o preenchimento da struct ao arquivo.
int escrever_cabecalho_pops(FILE *f, const cabecalho_pops *cabecalho);

// O índice é reservado em blocos fixos, tanto na leitura quanto nas inserções.
int adicionar_indice_pops(indice_pops *indice, int32_t id, int64_t pos);

// Procura o idPoPs no índice em memória. Retorna 1 e a posição da entrada se achar, ou 0 caso
// contrário.
int buscar_indice_pops(const indice_pops *indice, int32_t id, size_t *posicao);

// Abre os arquivos de dados e de índice (rb, ou r+b se 'editar') e carrega cabeçalho e índice em
// memória.
// Retorna 0 se algo estiver inconsistente.
int abrir_arquivos_pops(const char *binario, const char *nome_indice, int editar,
                       FILE **f, FILE **fi, cabecalho_pops *cabecalho, indice_pops *indice);

// Fecha os arquivos abertos (se houver) e libera o índice em memória. Retorna 0 se algum fclose
// falhar.
int fechar_arquivos_pops(FILE *f, FILE *fi, indice_pops *indice);

// Marca os dois arquivos como inconsistentes antes de qualquer alteração. Retorna 0 em caso de
// erro.
int iniciar_alteracoes_pops(FILE *f, FILE *fi);

// Função de comparação para qsort: ordena as entradas do índice por idPoPs crescente.
int comparar_indice_pops(const void *a, const void *b);

// Ordena o índice, regrava o arquivo de índice e marca os dois arquivos como consistentes.
// Retorna 0 em caso de erro.
int finalizar_alteracoes_pops(FILE *f, FILE **fi, const char *nome_indice,
                             cabecalho_pops *cabecalho, indice_pops *indice);

#endif // TRABALHOED3_POPS_ARQUIVO_H
