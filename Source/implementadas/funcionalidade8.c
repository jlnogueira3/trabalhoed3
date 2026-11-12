/*
 * João L A Nogueira - N usp: 16986970
 * José Eduardo Constantino Nordemann - N usp: 16877738
 *
 * Funcionalidade 8 (trabalho 2): lê registros de um CSV de PoPs e grava
 * o arquivo de dados PoPs, usando as funções de auxiliar_2 do arquivo de dados.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"
#include "../../Headers/auxiliares/auxiliar_2.h"

#define TAM_LINHA_CSV 512
#define MAX_COLUNAS_CSV 16

// siglaPais ocupa 2 bytes fixos no arquivo de dados
#define TAM_SIGLA_PAIS 2

// Separa uma linha do CSV em campos, trocando cada vírgula por '\0'.
// Campos vazios são mantidos como strings vazias (valores nulos).
// Retorna a quantidade de campos encontrados.
static int separar_campos(char *linha, char *campos[], int max) {
    int n = 0;
    campos[n++] = linha;
    for (char *p = linha; *p != '\0' && n < max; p++) {
        if (*p == ',') {
            *p = '\0';
            campos[n++] = p + 1;
        }
    }
    return n;
}

// Fecha os arquivos e exibe a mensagem de falha.
static void encerrar_com_falha(FILE *leitura, FILE *escrita) {
    fclose(leitura);
    falha_processamento_arquivo(escrita);
}

// Lê o CSV de PoPs e grava o arquivo binário de dados.
// A primeira linha do CSV diz o que cada coluna representa, então as colunas
// são localizadas pelo nome, e não pela posição.
void funcionalidade8(const char *csv, const char *binario) {
    // O CSV é texto
    FILE *leitura = fopen(csv, "r");
    if (verificar_abertura_arquivo(leitura) == 0)
        return;

    // O arquivo de dados é criado do zero, só com escrita
    FILE *escrita = fopen(binario, "wb");
    if (verificar_abertura_arquivo(escrita) == 0) {
        fclose(leitura);
        return;
    }

    // Cabeçalho inicial, com status '0' até o fim da escrita
    cabecalho_pops cabecalho;
    cabecalho.status = '0';
    cabecalho.topoLista = -1;
    cabecalho.proxByteOffset = CABECALHO_POPS;
    cabecalho.nroRegRem = 0;
    cabecalho.nroPoPs = 0;
    if (!escrever_cabecalho_pops(escrita, &cabecalho)) {
        encerrar_com_falha(leitura, escrita);
        return;
    }

    // Lê a primeira linha e localiza as colunas pelo nome
    char linha[TAM_LINHA_CSV];
    char *campos[MAX_COLUNAS_CSV];
    if (fgets(linha, sizeof(linha), leitura) == NULL) {
        encerrar_com_falha(leitura, escrita);
        return;
    }
    linha[strcspn(linha, "\r\n")] = '\0';
    int ncolunas = separar_campos(linha, campos, MAX_COLUNAS_CSV);

    int col_id = -1, col_nome = -1, col_pais = -1, col_sigla = -1;
    for (int i = 0; i < ncolunas; i++) {
        if (strcmp(campos[i], "idPoPs") == 0)
            col_id = i;
        else if (strcmp(campos[i], "nomePoPs") == 0)
            col_nome = i;
        else if (strcmp(campos[i], "nomePais") == 0)
            col_pais = i;
        else if (strcmp(campos[i], "siglaPais") == 0)
            col_sigla = i;
    }

    // O CSV precisa ter as quatro colunas da especificação
    if (col_id < 0 || col_nome < 0 || col_pais < 0 || col_sigla < 0) {
        encerrar_com_falha(leitura, escrita);
        return;
    }

    // Cada linha de dados vira um registro. Linhas vazias são ignoradas.
    while (fgets(linha, sizeof(linha), leitura) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0')
            continue;

        ncolunas = separar_campos(linha, campos, MAX_COLUNAS_CSV);

        // A linha precisa ter pelo menos as colunas procuradas
        int maior_coluna = col_id;
        if (col_nome > maior_coluna) maior_coluna = col_nome;
        if (col_pais > maior_coluna) maior_coluna = col_pais;
        if (col_sigla > maior_coluna) maior_coluna = col_sigla;
        if (ncolunas <= maior_coluna) {
            encerrar_com_falha(leitura, escrita);
            return;
        }

        // Os caracteres '|' são reservados como delimitador dos campos variáveis
        if (strchr(campos[col_nome], '|') != NULL || strchr(campos[col_pais], '|') != NULL) {
            encerrar_com_falha(leitura, escrita);
            return;
        }

        // siglaPais tem 2 bytes. Vazio vira "$$" (nulo); caractere faltante vira '$'.
        char sigla[TAM_SIGLA_PAIS + 1];
        for (int i = 0; i < TAM_SIGLA_PAIS; i++)
            sigla[i] = campos[col_sigla][i] != '\0' ? campos[col_sigla][i] : '$';
        sigla[TAM_SIGLA_PAIS] = '\0';

        dados_pops dado;
        dado.removido = '0';
        dado.encadeamentoLista = -1;
        dado.idPoPs = (int32_t)atoi(campos[col_id]);
        memcpy(dado.siglaPais, sigla, TAM_SIGLA_PAIS + 1);
        dado.nomePoPs = campos[col_nome];
        dado.nomePais = campos[col_pais];

        // tamanhoRegistro é o tamanho total do registro, incluindo o controle
        if (!tamanho_dados_pops(&dado, &dado.tamanhoRegistro) ||
            !escrever_dados_pops(escrita, cabecalho.proxByteOffset, &dado)) {
            encerrar_com_falha(leitura, escrita);
            return;
        }

        cabecalho.proxByteOffset += dado.tamanhoRegistro;
        cabecalho.nroPoPs++;
    }

    // Arquivo completo: marca como consistente e regrava o cabeçalho no início
    cabecalho.status = STATUS_CONSISTENTE;
    if (!escrever_cabecalho_pops(escrita, &cabecalho) || fflush(escrita) != 0) {
        encerrar_com_falha(leitura, escrita);
        return;
    }

    fclose(escrita);
    fclose(leitura);

    BinarioNaTela((char *)binario);
}
