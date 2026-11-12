// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/pops_manipulacao.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "../Headers/fornecidas.h"
#include "../Headers/pops.h"
#include "../Headers/pops_arquivo.h"
#include "../Headers/pops_registro.h"
#include "../Headers/pops_operacoes.h"
#include "../Headers/pops_entrada.h"

// Remove logicamente os registros encontrados, colocando os espacos na lista
// de removidos e retirando as chaves correspondentes do indice.
void funcionalidade12(const char *binario, const char *nome_indice, int n) {
    FILE *f, *fi;
    cabecalho_pops cabecalho;
    indice_pops indice;
    int ok = abrir_arquivos_pops(binario, nome_indice, 1, &f, &fi, &cabecalho, &indice);
    if (ok)
        ok = iniciar_alteracoes_pops(f, fi); // Os dois status ficam '0' durante a edição.

    // Cada solicitação de remoção tem seus próprios critérios.
    for (int i = 0; ok && i < n; i++) {
        criterio_pops *criterios;
        int quantidade;
        if (!ler_criterios_pops(&criterios, &quantidade)) {
            ok = 0;
            break;
        }
        int64_t *posicoes;
        size_t encontrados;
        ok = buscar_registros_pops(f, &cabecalho, &indice, criterios, quantidade,
                                   &posicoes, &encontrados);
        destroi_criterios_pops(criterios, quantidade);

        // A busca ja desconsiderou os removidos. Cada registro encontrado entra uma vez na lista.
        for (size_t j = 0; ok && j < encontrados; j++) {
            dados_pops dado;
            ok = leitura_dados_pops(f, posicoes[j], cabecalho.proxByteOffset, &dado);
            if (ok) {
                ok = remover_registro_pops(f, &cabecalho, &indice, posicoes[j], &dado);
                destroi_dados_pops(&dado);
            }
        }
        free(posicoes);
        // Se nao encontrou nenhum, seguimos para a proxima solicitacao sem imprimir mensagem.
    }

    if (ok)
        ok = finalizar_alteracoes_pops(f, &fi, nome_indice, &cabecalho, &indice);
    if (!fechar_arquivos_pops(f, fi, &indice))
        ok = 0;
    if (!ok)
        printf("Falha no processamento do arquivo.\n");
    else {
        // Os arquivos precisam estar fechados antes de chamar a funcao fornecida.
        BinarioNaTela((char *)binario);
        BinarioNaTela((char *)nome_indice);
    }
}

// Insere os registros usando o maior espaco removido que comportar os dados.
// Quando nenhum servir, escreve no final do arquivo e atualiza o proximo offset.
void funcionalidade13(const char *binario, const char *nome_indice, int n) {
    FILE *f, *fi;
    cabecalho_pops cabecalho;
    indice_pops indice;
    int ok = abrir_arquivos_pops(binario, nome_indice, 1, &f, &fi, &cabecalho, &indice);
    if (ok)
        ok = iniciar_alteracoes_pops(f, fi);

    // Cada inserção lê seu registro e o grava logo em seguida.
    for (int i = 0; ok && i < n; i++) {
        dados_pops dado;
        // Aqui os campos de controle são inicializados, pois o registro é novo.
        // A leitura recebe idPoPs, nomePoPs, nomePais e siglaPais, nessa ordem.
        if (!ler_insercao_pops(&dado)) {
            ok = 0;
            break;
        }
        ok = inserir_registro_pops(f, &cabecalho, &indice, &dado);
        destroi_dados_pops(&dado);
    }

    // O indice fica atualizado em memoria a cada insercao e e ordenado ao gravar.
    if (ok)
        ok = finalizar_alteracoes_pops(f, &fi, nome_indice, &cabecalho, &indice);
    if (!fechar_arquivos_pops(f, fi, &indice))
        ok = 0;
    if (!ok)
        printf("Falha no processamento do arquivo.\n");
    else {
        BinarioNaTela((char *)binario);
        BinarioNaTela((char *)nome_indice);
    }
}

// Aplica uma atualizacao e decide se o registro cabe no espaco que ja ocupava.
static int atualizar_registro_pops(FILE *f, cabecalho_pops *cabecalho, indice_pops *indice,
                                  int64_t pos, dados_pops *dado,
                                  const criterio_pops *atualizacoes, int quantidade) {
    int32_t id_antigo = dado->idPoPs;
    size_t entrada, repetido;
    if (!buscar_indice_pops(indice, id_antigo, &entrada) ||
        indice->entradas[entrada].byteoffset != pos ||
       !atualizar_campos_pops(dado, atualizacoes, quantidade))
        return 0;

    // Nao podemos trocar o id por um que pertence a outro registro ativo.
    if (buscar_indice_pops(indice, dado->idPoPs, &repetido) && repetido != entrada)
        return 0;
    int32_t necessario;
    if (!tamanho_dados_pops(dado, &necessario))
        return 0;
    if (necessario <= dado->tamanhoRegistro) {
        // O tamanho físico é mantido, incluindo a sobra que já existia antes.
        // A escrita preenche o restante com '$', sem mudar o byte offset.
        if (!escrever_dados_pops(f, pos, dado))
            return 0;
        indice->entradas[entrada].idPoPs = dado->idPoPs;
        return 1;
    }

    // O registro cresceu. A chave antiga é removida e os novos dados são inseridos
    // por Worst Fit, que também registra o novo id e offset no índice.
    int32_t id_novo = dado->idPoPs;
    dado->idPoPs = id_antigo;
    int ok = remover_registro_pops(f, cabecalho, indice, pos, dado);
    dado->idPoPs = id_novo;
    if (!ok)
        return 0;
    return inserir_registro_pops(f, cabecalho, indice, dado);
}

// Atualiza os PoPs que atendem aos critérios. Se o registro crescer, ele é removido e reinserido.
// O índice é atualizado ao final, em finalizar_alteracoes_pops.
void funcionalidade14(const char *binario, const char *nome_indice, int n) {
    FILE *f, *fi;
    cabecalho_pops cabecalho;
    indice_pops indice;
    int ok = abrir_arquivos_pops(binario, nome_indice, 1, &f, &fi, &cabecalho, &indice);
    if (ok)
        ok = iniciar_alteracoes_pops(f, fi);

    // Cada solicitação de atualização tem seus próprios critérios e campos.
    for (int i = 0; ok && i < n; i++) {
        criterio_pops *criterios, *atualizacoes;
        int quantidade_criterios, quantidade_atualizacoes;
        if (!ler_criterios_pops(&criterios, &quantidade_criterios)) {
            ok = 0;
            break;
        }
        if (!ler_criterios_pops(&atualizacoes, &quantidade_atualizacoes)) {
            destroi_criterios_pops(criterios, quantidade_criterios);
            ok = 0;
            break;
        }

        // A busca é feita antes de alterar, para não visitar de novo um registro realocado.
        // Na proxima solicitacao, a busca usa os dados e o indice ja atualizados.
        int64_t *posicoes;
        size_t encontrados;
        ok = buscar_registros_pops(f, &cabecalho, &indice, criterios, quantidade_criterios,
                                   &posicoes, &encontrados);
        destroi_criterios_pops(criterios, quantidade_criterios);
        // Cada registro encontrado é lido e atualizado; a posição vem da busca.
        for (size_t j = 0; ok && j < encontrados; j++) {
            dados_pops dado;
            ok = leitura_dados_pops(f, posicoes[j], cabecalho.proxByteOffset, &dado);
            if (ok) {
                ok = atualizar_registro_pops(f, &cabecalho, &indice, posicoes[j], &dado,
                                             atualizacoes, quantidade_atualizacoes);
                destroi_dados_pops(&dado);
            }
        }
        free(posicoes);
        destroi_criterios_pops(atualizacoes, quantidade_atualizacoes);
    }

    if (ok)
        ok = finalizar_alteracoes_pops(f, &fi, nome_indice, &cabecalho, &indice);
    if (!fechar_arquivos_pops(f, fi, &indice))
        ok = 0;
    if (!ok)
        printf("Falha no processamento do arquivo.\n");
    else {
        BinarioNaTela((char *)binario);
        BinarioNaTela((char *)nome_indice);
    }
}
