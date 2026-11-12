/*
 * João L A Nogueira - N usp: 16986970
 *
 * Funções do arquivo de dados PoPs (trabalho 2).
 * Todos os campos são gravados um a um, com o tamanho exato da especificação.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../../Headers/registros.h"
#include "../../Headers/auxiliares/pops.h"

// Inicializa o cabeçalho de um arquivo PoPs novo, conforme a especificação.
// O status fica '0' até o fim da escrita.
void inicializar_cabecalho_pops(cabecalho_pops *cabecalho) {
    cabecalho->status = STATUS_INCONSISTENTE;
    cabecalho->topoLista = -1;
    // Byte offsets são absolutos no arquivo. O primeiro registro começa logo
    // depois do cabeçalho, então o próximo byte disponível é o byte 25.
    cabecalho->proxByteOffset = TAM_CABECALHO_POPS;
    cabecalho->nroRegRem = 0;
    cabecalho->nroPoPs = 0;
}

// Lê o cabeçalho do arquivo PoPs. Retorna 1 se lido com sucesso e o arquivo
// está consistente, ou 0 se houver erro de leitura ou status diferente de '1'.
// O status é lido antes dos demais campos, e o ponteiro fica logo após o cabeçalho.
int leitura_cabecalho_pops(FILE *f, cabecalho_pops *cabecalho) {
    fseek(f, 0, SEEK_SET);

    if (fread(&cabecalho->status, sizeof(char), 1, f) != 1)
        return 0;
    if (cabecalho->status != STATUS_CONSISTENTE)
        return 0;

    if (fread(&cabecalho->topoLista, sizeof(int64_t), 1, f) != 1)
        return 0;
    if (fread(&cabecalho->proxByteOffset, sizeof(int64_t), 1, f) != 1)
        return 0;
    if (fread(&cabecalho->nroRegRem, sizeof(int), 1, f) != 1)
        return 0;
    if (fread(&cabecalho->nroPoPs, sizeof(int), 1, f) != 1)
        return 0;

    return 1;
}

// Escreve os 25 bytes do cabeçalho na posição atual do arquivo, campo a campo.
// Ordem: status(1), topoLista(8), proxByteOffset(8), nroRegRem(4), nroPoPs(4).
void escrever_cabecalho_pops(const cabecalho_pops *cabecalho, FILE *arquivo) {
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->topoLista, sizeof(int64_t), 1, arquivo);
    fwrite(&cabecalho->proxByteOffset, sizeof(int64_t), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroPoPs, sizeof(int), 1, arquivo);
}

// Regrava o cabeçalho inteiro no byte 0 e força a gravação no disco.
void salvar_cabecalho_pops(FILE *f, const cabecalho_pops *cabecalho) {
    fseek(f, 0, SEEK_SET);
    escrever_cabecalho_pops(cabecalho, f);
    fflush(f);
}

// Calcula o campo tamanhoRegistro: número de bytes do registro inteiro.
// Conta: removido (1) + tamanhoRegistro (4) + encadeamentoLista (8) + idPoPs (4)
//        + siglaPais (2) + nomePoPs e nomePais, cada um com seu delimitador '|'.
// Campos nulos de tamanho variável ocupam só o delimitador.
// Confirmado pelo checksum do run.codes: o valor esperado é o tamanho total do registro.
int tamanho_registro_pops(const char *nomePoPs, const char *nomePais) {
    return 1 + 4 + 8 + 4 + TAM_SIGLA_PAIS
         + (int)strlen(nomePoPs) + 1
         + (int)strlen(nomePais) + 1;
}

// Escreve um registro PoPs na posição atual do arquivo, campo a campo.
// Valores vazios ("") representam nulo:
//   - idPoPs nunca é nulo;
//   - siglaPais nulo vira "$$";
//   - nomePoPs e nomePais nulos ficam só com o delimitador '|'.
void escrever_registro_pops(FILE *f, int idPoPs, const char *siglaPais,
                            const char *nomePoPs, const char *nomePais) {
    // removido: registro válido
    char removido = '0';
    fwrite(&removido, sizeof(char), 1, f);

    // tamanhoRegistro: número de bytes do registro inteiro
    int tamanho = tamanho_registro_pops(nomePoPs, nomePais);
    fwrite(&tamanho, sizeof(int), 1, f);

    // encadeamentoLista: registro não está na pilha, então -1
    int64_t encadeamento = -1;
    fwrite(&encadeamento, sizeof(int64_t), 1, f);

    // idPoPs: campo de tamanho fixo, sem nulos
    fwrite(&idPoPs, sizeof(int), 1, f);

    // siglaPais: 2 bytes fixos. Caracteres faltantes são preenchidos com lixo '$'.
    for (int i = 0; i < TAM_SIGLA_PAIS; i++) {
        char c = (i < (int)strlen(siglaPais)) ? siglaPais[i] : LIXO_POPS;
        fwrite(&c, sizeof(char), 1, f);
    }

    char delimitador = DELIMITADOR_POPS;

    // nomePoPs: tamanho variável, terminado pelo delimitador
    if (nomePoPs[0] != '\0')
        fwrite(nomePoPs, sizeof(char), strlen(nomePoPs), f);
    fwrite(&delimitador, sizeof(char), 1, f);

    // nomePais: tamanho variável, terminado pelo delimitador
    if (nomePais[0] != '\0')
        fwrite(nomePais, sizeof(char), strlen(nomePais), f);
    fwrite(&delimitador, sizeof(char), 1, f);
}
