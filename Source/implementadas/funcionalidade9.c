/*
 * João L A Nogueira - N usp: 16986970
 * José Eduardo Constantino Nordemann - N usp: 16877738
 *
 * Funcionalidade 9 (trabalho 2): cria o arquivo de índice primário indexaPoPs
 * a partir do arquivo de dados PoPs gerado pela funcionalidade 8.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"
#include "../../Headers/auxiliares/auxiliar_2.h"

// Posição do campo idPoPs dentro do registro de dados, contada a partir do início do registro
#define POS_ID_NO_REGISTRO_POPS 13

// Uma entrada do índice: código do PoPs e byte offset do registro de dados
typedef struct entrada_indice {
    int idPoPs;
    int64_t byteoffset;
} entrada_indice;

// Ordena as entradas do índice pelo idPoPs, em ordem crescente
static int comparar_entradas(const void *a, const void *b) {
    const entrada_indice *ea = a;
    const entrada_indice *eb = b;
    if (ea->idPoPs < eb->idPoPs) return -1;
    if (ea->idPoPs > eb->idPoPs) return 1;
    return 0;
}

// Percorre os registros do arquivo de dados e preenche o vetor de entradas.
// Registros removidos são pulados. Retorna 1 em sucesso, 0 em erro.
static int coletar_entradas(FILE *dados, const cabecalho_pops *cabecalho,
                            entrada_indice **entradas_saida, int *quantidade_saida) {
    int capacidade = cabecalho->nroPoPs > 0 ? cabecalho->nroPoPs : 1;
    int n = 0;
    entrada_indice *entradas = malloc(capacidade * sizeof(entrada_indice));
    if (entradas == NULL)
        return 0;

    // Os registros começam logo após o cabeçalho e terminam em proxByteOffset
    int64_t offset = CABECALHO_POPS;
    while (offset < cabecalho->proxByteOffset) {
        fseek(dados, offset, SEEK_SET);

        // Lê só o removido e o tamanho do registro para saber onde está o próximo
        char removido;
        int tamanho;
        if (fread(&removido, sizeof(char), 1, dados) != 1 ||
            fread(&tamanho, sizeof(int), 1, dados) != 1 ||
            tamanho <= 0) {
            free(entradas);
            return 0;
        }

        // Registro válido: lê o idPoPs e guarda a entrada no índice
        if (removido == '0') {
            int idPoPs;
            fseek(dados, offset + POS_ID_NO_REGISTRO_POPS, SEEK_SET);
            if (fread(&idPoPs, sizeof(int), 1, dados) != 1) {
                free(entradas);
                return 0;
            }

            // Aumenta o vetor se a estimativa inicial (nroPoPs) foi pequena
            if (n == capacidade) {
                capacidade *= 2;
                entrada_indice *novo = realloc(entradas, capacidade * sizeof(entrada_indice));
                if (novo == NULL) {
                    free(entradas);
                    return 0;
                }
                entradas = novo;
            }

            entradas[n].idPoPs = idPoPs;
            entradas[n].byteoffset = offset;
            n++;
        }

        // Avança para o próximo registro usando o tamanho lido
        offset += tamanho;
    }

    // O último registro deve terminar exatamente em proxByteOffset
    if (offset != cabecalho->proxByteOffset) {
        free(entradas);
        return 0;
    }

    *entradas_saida = entradas;
    *quantidade_saida = n;
    return 1;
}

// Cria o arquivo de índice primário a partir do arquivo de dados PoPs.
// O índice só é criado depois que o arquivo de dados foi lido por completo.
void funcionalidade9(const char *binario_dados, const char *binario_indice) {
    // O arquivo de dados só é lido
    FILE *dados = fopen(binario_dados, "rb");
    if (verificar_abertura_arquivo(dados) == 0)
        return;

    // Só usa o arquivo de dados se o cabeçalho estiver consistente
    cabecalho_pops cabecalho;
    if (leitura_cabecalho_pops(dados, &cabecalho) == 0) {
        falha_processamento_arquivo(dados);
        return;
    }

    // Coleta as entradas dos registros válidos
    entrada_indice *entradas = NULL;
    int quantidade = 0;
    if (coletar_entradas(dados, &cabecalho, &entradas, &quantidade) == 0) {
        falha_processamento_arquivo(dados);
        return;
    }
    fclose(dados);

    // O índice precisa estar ordenado por idPoPs
    qsort(entradas, quantidade, sizeof(entrada_indice), comparar_entradas);

    // idPoPs é chave primária: dois registros válidos com o mesmo id tornam o arquivo inválido
    for (int i = 1; i < quantidade; i++) {
        if (entradas[i].idPoPs == entradas[i - 1].idPoPs) {
            free(entradas);
            falha_processamento_arquivo(NULL);
            return;
        }
    }

    // Grava o índice: status '0' durante a escrita, '1' ao final
    FILE *indice = fopen(binario_indice, "wb");
    if (indice == NULL) {
        free(entradas);
        verificar_abertura_arquivo(indice);
        return;
    }

    char status = STATUS_INCONSISTENTE;
    fwrite(&status, sizeof(char), 1, indice);

    for (int i = 0; i < quantidade; i++) {
        fwrite(&entradas[i].idPoPs, sizeof(int), 1, indice);
        fwrite(&entradas[i].byteoffset, sizeof(int64_t), 1, indice);
    }

    status = STATUS_CONSISTENTE;
    fseek(indice, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, indice);

    fclose(indice);
    free(entradas);

    BinarioNaTela((char *)binario_indice);
}
