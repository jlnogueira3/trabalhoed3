// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Cabeçalho do arquivo de registros: leitura, escrita e status (trabalho 1).

#ifndef TRABALHOED3_CABECALHO_H
#define TRABALHOED3_CABECALHO_H

#include <stdio.h>

#include "registros.h"

// Inicializa o cabeçalho com os valores da especificação para um arquivo novo.
// Só deve ser usada na criação do arquivo (funcionalidade 1).
void inicializar_cabecalho(registro_cabecalho *cabecalho);

// Lê o cabeçalho do arquivo.
// Retorna 1 se o cabeçalho foi lido e o arquivo está consistente,
// e 0 em caso de erro de leitura ou de arquivo inconsistente.
// Se retornar 1, o ponteiro fica logo após o cabeçalho, no início do primeiro registro.
int leitura_cabecalho(FILE *f, registro_cabecalho *cabecalho);

// Escreve os campos do cabeçalho na posição atual do arquivo.
// Usada diretamente quando o arquivo já está posicionado no byte 0.
void escrever_cabecalho(const registro_cabecalho *cabecalho, FILE *arquivo);

// Regrava o cabeçalho inteiro no byte 0 do arquivo e força a gravação no disco.
void salvar_cabecalho(FILE *f, const registro_cabecalho *cabecalho);

// Regrava somente o byte de status, no byte 0, e força a gravação no disco.
// Deixa o ponteiro logo após o byte de status: quem chama deve reposicionar o ponteiro antes de ler
// ou gravar registros.
void marcar_status_cabecalho(FILE *f, char status);

#endif // TRABALHOED3_CABECALHO_H
