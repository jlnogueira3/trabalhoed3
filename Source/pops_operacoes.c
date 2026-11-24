// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/pops_operacoes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "../Headers/registros.h"
#include "../Headers/pops.h"
#include "../Headers/pops_arquivo.h"
#include "../Headers/pops_registro.h"

// Procura os registros que atendem aos critérios e guarda suas posições em *posicoes (liberar com
// free).
// Usa o índice quando há idPoPs entre os critérios; caso contrário, percorre o arquivo. Retorna 0
// em caso de erro.
int buscar_registros_pops(FILE *f, const cabecalho_pops *cabecalho, const indice_pops *indice,
                         const criterio_pops *criterios, int quantidade,
                         int64_t **posicoes, size_t *encontrados) {
    *posicoes = NULL;
    *encontrados = 0;
    size_t capacidade = 0;
    int campo_id = -1;
        // Pré-tratamento: o idPoPs é procurado entre TODOS os critérios antes de ler registros.
    for (int i = 0; i < quantidade; i++) {
        if (strcmp(criterios[i].chave, "idPoPs") == 0) {
            campo_id = i;
            break;
        }
    }
    int64_t pos = CABECALHO_POPS;
    if (campo_id != -1) {
        size_t entrada;
        int32_t id = (int32_t)strtol(criterios[campo_id].valor, NULL, 10);
        if (!buscar_indice_pops(indice, id, &entrada))
                        return 1; // a chave não existe; não é preciso percorrer os dados
        pos = indice->entradas[entrada].byteoffset;
    }

        // Somente os offsets encontrados são guardados. Isso evita atualizar duas vezes
    // um registro que foi movido para uma posicao mais adiante durante a funcionalidade 14.
    while (pos < cabecalho->proxByteOffset) {
        dados_pops dado;
        if (!leitura_dados_pops(f, pos, cabecalho->proxByteOffset, &dado))
            goto falha;
        int64_t proximo = pos + dado.tamanhoRegistro;
        int atende = dado.removido == REGISTRO_ATIVO &&
                     atende_criterios_pops(&dado, criterios, quantidade);
        destroi_dados_pops(&dado);
        if (atende) {
            if (*encontrados == capacidade) {
                if (capacidade > SIZE_MAX / sizeof(int64_t) - BLOCO_POPS)
                    goto falha;
                int64_t *novo = realloc(*posicoes, (capacidade + BLOCO_POPS) * sizeof(int64_t));
                if (novo == NULL)
                    goto falha;
                *posicoes = novo;
                capacidade += BLOCO_POPS;
            }
            (*posicoes)[(*encontrados)++] = pos;
        }
        if (campo_id != -1)
                        break; // o id é único, mas os demais critérios ainda são verificados
        pos = proximo;
    }
    return 1;

falha:
    free(*posicoes);
    *posicoes = NULL;
    *encontrados = 0;
    return 0;
}

// Remove logicamente o registro em pos: ele entra no início da lista de removidos e sua chave sai
// do índice.
// Retorna 0 em caso de erro.
int remover_registro_pops(FILE *f, cabecalho_pops *cabecalho, indice_pops *indice,
                         int64_t pos, const dados_pops *dado) {
    size_t entrada;
    if (dado->removido != REGISTRO_ATIVO || cabecalho->nroRegRem == INT32_MAX ||
        cabecalho->nroPoPs <= 0 ||
       !buscar_indice_pops(indice, dado->idPoPs, &entrada) ||
       indice->entradas[entrada].byteoffset != pos)
        return 0;

        // O novo removido entra no início da lista, sem ordenar por tamanho.
    // A escolha do maior espaco sera feita percorrendo a lista na insercao.
    int64_t atual = cabecalho->topoLista;
    char marca = REGISTRO_REMOVIDO;
    if (!posicionar_pops(f, pos) || fwrite(&marca, 1, 1, f) != 1 ||
       fwrite(&dado->tamanhoRegistro, 4, 1, f) != 1 || fwrite(&atual, 8, 1, f) != 1 ||
       !escrever_lixo_pops(f, dado->tamanhoRegistro - TAM_CONTROLE_REMOCAO_POPS))
        return 0;
    cabecalho->topoLista = pos;
    cabecalho->nroRegRem++;
    cabecalho->nroPoPs--;
    memmove(&indice->entradas[entrada], &indice->entradas[entrada + 1],
            (indice->quantidade - entrada - 1) * sizeof(entrada_indice_pops));
    indice->quantidade--;
    return 1;
}

// Insere um registro novo: reaproveita o maior removido que couber (Worst Fit) ou grava no fim do
// arquivo.
// Atualiza índice e contadores. Retorna 0 em caso de erro.
int inserir_registro_pops(FILE *f, cabecalho_pops *cabecalho, indice_pops *indice,
                          dados_pops *dado) {
    size_t entrada;
    int32_t necessario;
    if (dado->idPoPs == -1 || cabecalho->nroPoPs == INT32_MAX ||
       buscar_indice_pops(indice, dado->idPoPs, &entrada) || !tamanho_dados_pops(dado, &necessario))
        return 0;
    int64_t pos = cabecalho->proxByteOffset;
    dado->tamanhoRegistro = necessario;
    dado->removido = REGISTRO_ATIVO;
    dado->encadeamentoLista = -1;

        // Worst Fit: todos os removidos são percorridos, e o maior que serve é guardado.
        // Em empate, fica o primeiro encontrado na lista (convenção adotada).
    int64_t atual = cabecalho->topoLista, anterior = -1;
    int64_t maior = -1, anterior_maior = -1, proximo_maior = -1;
    int32_t tamanho_maior = 0, visitados = 0;
    while (atual != -1) {
        dados_pops removido;
        if (visitados++ >= cabecalho->nroRegRem ||
           !ler_controle_pops(f, atual, cabecalho->proxByteOffset, &removido) ||
           removido.removido != REGISTRO_REMOVIDO)
            return 0;
        if (removido.tamanhoRegistro >= necessario && removido.tamanhoRegistro > tamanho_maior) {
            maior = atual;
            anterior_maior = anterior;
            proximo_maior = removido.encadeamentoLista;
            tamanho_maior = removido.tamanhoRegistro;
        }
        anterior = atual;
        atual = removido.encadeamentoLista;
    }
    if (visitados != cabecalho->nroRegRem)
        return 0;
    if (maior != -1) {
                // O escolhido é retirado da lista, preservando o encadeamento dos outros.
        pos = maior;
        dado->tamanhoRegistro = tamanho_maior;
        if (anterior_maior == -1)
            cabecalho->topoLista = proximo_maior;
        else {
            if (!posicionar_pops(f, anterior_maior + OFFSET_ENCADEAMENTO_POPS) ||
                fwrite(&proximo_maior, 8, 1, f) != 1)
                return 0;
        }
        cabecalho->nroRegRem--;
    }
    if (pos == cabecalho->proxByteOffset) {
        if (cabecalho->proxByteOffset > INT64_MAX - necessario)
            return 0;
        cabecalho->proxByteOffset += necessario;
    }
    if (!adicionar_indice_pops(indice, dado->idPoPs, pos) || !escrever_dados_pops(f, pos, dado))
        return 0;
    cabecalho->nroPoPs++;
    return 1;
}
