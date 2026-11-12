// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/cabecalho.h"

#include <stdio.h>

#include "../Headers/registros.h"

// Inicializa o cabeçalho com os valores da especificação para um arquivo novo.
// Só deve ser usada na criação do arquivo (funcionalidade 1).
void inicializar_cabecalho(registro_cabecalho *cabecalho) {
    cabecalho->status = STATUS_INCONSISTENTE; // arquivo em construção
    cabecalho->topo_pilha = -1;
    cabecalho->proxRRN = 0;
    cabecalho->nroRegRem = 0;
    cabecalho->nroPares = 0;
}

// Lê o cabeçalho do arquivo.
// Retorna 1 se o cabeçalho foi lido e o arquivo está consistente,
// e 0 em caso de erro de leitura ou de arquivo inconsistente.
// Se retornar 1, o ponteiro fica logo após o cabeçalho, no início do primeiro registro.
int leitura_cabecalho(FILE *f, registro_cabecalho *cabecalho) {
    // O cabeçalho começa sempre no byte 0
    fseek(f, 0, SEEK_SET);

    // Primeiro lemos apenas o status. Se o arquivo estiver inconsistente,
    // os demais campos do cabeçalho não devem ser lidos.
    if (fread(&cabecalho->status, sizeof(char), 1, f) != 1)
        return 0;
    if (cabecalho->status != STATUS_CONSISTENTE)
        return 0;

    // Arquivo consistente: lemos o restante do cabeçalho
    if (fread(&cabecalho->topo_pilha, sizeof(int), 1, f) != 1)
        return 0;
    if (fread(&cabecalho->proxRRN, sizeof(int), 1, f) != 1)
        return 0;
    if (fread(&cabecalho->nroRegRem, sizeof(int), 1, f) != 1)
        return 0;
    if (fread(&cabecalho->nroPares, sizeof(int), 1, f) != 1)
        return 0;

    return 1;
}

// Escreve os campos do cabeçalho na posição atual do arquivo.
// Usada diretamente quando o arquivo já está posicionado no byte 0.
void escrever_cabecalho(const registro_cabecalho *cabecalho, FILE *arquivo) {
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->topo_pilha, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->proxRRN, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroPares, sizeof(int), 1, arquivo);
}

// Regrava o cabeçalho inteiro no byte 0 do arquivo e força a gravação no disco.
void salvar_cabecalho(FILE *f, const registro_cabecalho *cabecalho) {
    fseek(f, 0, SEEK_SET);
    escrever_cabecalho(cabecalho, f);
    fflush(f);
}

// Regrava somente o byte de status, no byte 0, e força a gravação no disco.
// Deixa o ponteiro logo após o byte de status: quem chama deve reposicionar o ponteiro antes de ler
// ou gravar registros.
void marcar_status_cabecalho(FILE *f, char status) {
    fseek(f, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, f);
    fflush(f);
}
