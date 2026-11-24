// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/pops_arquivo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>

#include "../Headers/registros.h"
#include "../Headers/pops.h"

// O byte offset é controlado pelas próprias leituras, sem consultar ftell.
// No Windows, long pode ter 4 bytes. O deslocamento é dividido para não truncar o offset.
int posicionar_pops(FILE *f, int64_t pos) {
    if (pos < 0 || fseek(f, 0, SEEK_SET) != 0)
        return 0;
    while (pos > LONG_MAX) {
        if (fseek(f, LONG_MAX, SEEK_CUR) != 0)
            return 0;
        pos -= LONG_MAX;
    }
    return fseek(f, (long)pos, SEEK_CUR) == 0;
}

// Lê o cabeçalho do arquivo de PoPs. Retorna 1 se o arquivo estiver consistente e o
// cabeçalho for lido por completo, e 0 caso contrário. Ao retornar 1, o ponteiro fica logo após o
// cabeçalho.
int leitura_cabecalho_pops(FILE *f, cabecalho_pops *cabecalho) {
    // Primeiro lemos so o status. Se estiver inconsistente, nao lemos o restante.
    if (!posicionar_pops(f, 0) || fread(&cabecalho->status, 1, 1, f) != 1 ||
       cabecalho->status != STATUS_CONSISTENTE)
        return 0;
    if (fread(&cabecalho->topoLista, 8, 1, f) != 1 ||
       fread(&cabecalho->proxByteOffset, 8, 1, f) != 1 ||
       fread(&cabecalho->nroRegRem, 4, 1, f) != 1 ||
       fread(&cabecalho->nroPoPs, 4, 1, f) != 1)
        return 0;
    return cabecalho->proxByteOffset >= CABECALHO_POPS && cabecalho->nroRegRem >= 0 &&
           cabecalho->nroPoPs >= 0 &&
           (cabecalho->topoLista == -1 || (cabecalho->topoLista >= CABECALHO_POPS &&
            cabecalho->topoLista < cabecalho->proxByteOffset));
}

// Os campos são gravados um a um, para não levar o preenchimento da struct ao arquivo.
int escrever_cabecalho_pops(FILE *f, const cabecalho_pops *cabecalho) {
    return posicionar_pops(f, 0) &&
           fwrite(&cabecalho->status, 1, 1, f) == 1 &&
           fwrite(&cabecalho->topoLista, 8, 1, f) == 1 &&
           fwrite(&cabecalho->proxByteOffset, 8, 1, f) == 1 &&
           fwrite(&cabecalho->nroRegRem, 4, 1, f) == 1 &&
           fwrite(&cabecalho->nroPoPs, 4, 1, f) == 1;
}

// O índice é reservado em blocos fixos, tanto na leitura quanto nas inserções.
int adicionar_indice_pops(indice_pops *indice, int32_t id, int64_t pos) {
    if (indice->quantidade == indice->capacidade) {
        if (indice->capacidade > SIZE_MAX / sizeof(entrada_indice_pops) - BLOCO_POPS)
            return 0;
        size_t capacidade = indice->capacidade + BLOCO_POPS;
        entrada_indice_pops *novo =
            realloc(indice->entradas, capacidade * sizeof(entrada_indice_pops));
        if (novo == NULL)
            return 0;
        indice->entradas = novo;
        indice->capacidade = capacidade;
    }
    indice->entradas[indice->quantidade].idPoPs = id;
    indice->entradas[indice->quantidade].byteoffset = pos;
    indice->quantidade++;
    return 1;
}

// Procura o idPoPs no índice em memória. Retorna 1 e a posição da entrada se achar, ou 0 caso
// contrário.
int buscar_indice_pops(const indice_pops *indice, int32_t id, size_t *posicao) {
    // A busca e linear no indice em memoria, sem implementar busca binaria.
    for (size_t i = 0; i < indice->quantidade; i++) {
        if (indice->entradas[i].idPoPs == id) {
            *posicao = i;
            return 1;
        }
    }
    return 0;
}

// Abre os arquivos de dados e de índice (rb, ou r+b se 'editar') e carrega cabeçalho e índice em
// memória.
// Retorna 0 se algo estiver inconsistente.
int abrir_arquivos_pops(const char *binario, const char *nome_indice, int editar,
                       FILE **f, FILE **fi, cabecalho_pops *cabecalho, indice_pops *indice) {
    *f = NULL;
    *fi = NULL;
    indice->entradas = NULL;
    indice->quantidade = 0;
    indice->capacidade = 0;
    if (strcmp(binario, nome_indice) == 0)
        return 0;
    *f = fopen(binario, editar ? "r+b" : "rb");
    if (*f == NULL || !leitura_cabecalho_pops(*f, cabecalho))
        return 0;
    *fi = fopen(nome_indice, editar ? "r+b" : "rb");
    char status;
    if (*fi == NULL || fread(&status, 1, 1, *fi) != 1 || status != STATUS_CONSISTENTE)
        return 0;

        // O arquivo de índice não tem contador no cabeçalho. A leitura vai até o fim,
    // distinguindo fim normal de uma entrada incompleta ou erro de leitura.
    while (1) {
        entrada_indice_pops entrada;
        size_t lidos = fread(&entrada.idPoPs, 1, 4, *fi);
        if (lidos == 0 && feof(*fi) && !ferror(*fi))
            break;
        if (lidos != 4 || fread(&entrada.byteoffset, 8, 1, *fi) != 1 || entrada.idPoPs == -1 ||
           entrada.byteoffset < CABECALHO_POPS || entrada.byteoffset >= cabecalho->proxByteOffset)
            return 0;
        if (indice->quantidade > 0 &&
            entrada.idPoPs <= indice->entradas[indice->quantidade - 1].idPoPs)
            return 0;
        if (!adicionar_indice_pops(indice, entrada.idPoPs, entrada.byteoffset))
            return 0;
    }
    return indice->quantidade == (size_t)cabecalho->nroPoPs;
}

// Fecha os arquivos abertos (se houver) e libera o índice em memória. Retorna 0 se algum fclose
// falhar.
int fechar_arquivos_pops(FILE *f, FILE *fi, indice_pops *indice) {
    int ok = 1;
    if (f != NULL && fclose(f) != 0)
        ok = 0;
    if (fi != NULL && fclose(fi) != 0)
        ok = 0;
    free(indice->entradas);
    return ok;
}

// Grava somente o byte de status no início do arquivo e faz fflush.
static int escrever_status_pops(FILE *f, char status) {
    return posicionar_pops(f, 0) && fwrite(&status, 1, 1, f) == 1 && fflush(f) == 0;
}

// Marca os dois arquivos como inconsistentes antes de qualquer alteração. Retorna 0 em caso de
// erro.
int iniciar_alteracoes_pops(FILE *f, FILE *fi) {
    // Os dois arquivos ficam inconsistentes antes da primeira alteracao.
    return escrever_status_pops(f, STATUS_INCONSISTENTE) &&
           escrever_status_pops(fi, STATUS_INCONSISTENTE);
}

// Função de comparação para qsort: ordena as entradas do índice por idPoPs crescente.
int comparar_indice_pops(const void *a, const void *b) {
    const entrada_indice_pops *primeiro = a, *segundo = b;
    return (primeiro->idPoPs > segundo->idPoPs) - (primeiro->idPoPs < segundo->idPoPs);
}

// Ordena o índice, regrava o arquivo de índice e marca os dois arquivos como consistentes.
// Retorna 0 em caso de erro.
int finalizar_alteracoes_pops(FILE *f, FILE **fi, const char *nome_indice,
                             cabecalho_pops *cabecalho, indice_pops *indice) {
    cabecalho->status = STATUS_INCONSISTENTE;
    if (!escrever_cabecalho_pops(f, cabecalho) || fflush(f) != 0)
        return 0;

        // Tudo é ordenado uma vez com a função da biblioteca. O modo wb também elimina
    // as entradas antigas que sobrariam no fim do arquivo apos uma remocao.
    if (indice->quantidade > 1)
        qsort(indice->entradas, indice->quantidade, sizeof(entrada_indice_pops),
              comparar_indice_pops);
    FILE *novo = freopen(nome_indice, "wb", *fi);
    *fi = novo;
    if (novo == NULL || !escrever_status_pops(*fi, STATUS_INCONSISTENTE))
        return 0;
    for (size_t i = 0; i < indice->quantidade; i++) {
        if (fwrite(&indice->entradas[i].idPoPs, 4, 1, *fi) != 1 ||
           fwrite(&indice->entradas[i].byteoffset, 8, 1, *fi) != 1)
            return 0;
    }
    if (fflush(*fi) != 0 || !escrever_status_pops(*fi, STATUS_CONSISTENTE) ||
        !escrever_status_pops(f, STATUS_CONSISTENTE))
        return 0;
    cabecalho->status = STATUS_CONSISTENTE;
    return 1;
}
