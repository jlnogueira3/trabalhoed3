// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/pops_consulta.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "../Headers/registros.h"
#include "../Headers/pops.h"
#include "../Headers/erros.h"
#include "../Headers/pops_arquivo.h"
#include "../Headers/pops_registro.h"
#include "../Headers/pops_operacoes.h"
#include "../Headers/pops_entrada.h"

// Percorre todos os registros do arquivo, do primeiro ao último byte válido.
void funcionalidade10(const char *binario) {
    FILE *f = fopen(binario, "rb");
    if (!verificar_abertura_arquivo(f))
        return;

    cabecalho_pops cabecalho;
    if (!leitura_cabecalho_pops(f, &cabecalho)) {
        falha_processamento_arquivo(f);
        return;
    }

    int encontrados = 0;
    int ok = 1;
    int64_t pos = CABECALHO_POPS;
    // Percorre o arquivo registro por registro, enquanto o ponteiro estiver dentro da área de
    // dados.
    while (ok && pos < cabecalho.proxByteOffset) {
        dados_pops dado;
        if (!leitura_dados_pops(f, pos, cabecalho.proxByteOffset, &dado)) {
            ok = 0;
            break;
        }
        // Registros removidos não são exibidos, mas o tamanho dele ainda avança a leitura.
        if (dado.removido == REGISTRO_ATIVO) {
            print_dados_pops(&dado);
            encontrados++;
        }
        pos += dado.tamanhoRegistro;
        destroi_dados_pops(&dado);
    }

    if (ok && encontrados == 0)
        printf("Registro inexistente.\n");
    if (!ok)
        printf("Falha no processamento do arquivo.\n");
    fclose(f);
}

// Busca os PoPs que atendem aos critérios. Se houver idPoPs entre os campos, o índice é usado;
// caso contrário, o arquivo é percorrido sequencialmente.
void funcionalidade11(const char *binario, const char *nome_indice, int n) {
    FILE *f, *fi;
    cabecalho_pops cabecalho;
    indice_pops indice;
    int ok = abrir_arquivos_pops(binario, nome_indice, 0, &f, &fi, &cabecalho, &indice);

    // Cada solicitacao tem seus proprios criterios, mesmo quando a anterior nao encontrou nada.
    // Cada solicitação é atendida com seus próprios critérios.
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

        // Uma linha em branco separa o resultado de cada solicacao da anterior.
        if (ok && i > 0)
            printf("\n");
        if (ok && encontrados == 0)
            printf("Registro inexistente.\n");
        for (size_t j = 0; ok && j < encontrados; j++) {
            dados_pops dado;
            // A posição já veio da busca. Aqui só o registro é lido para imprimir seus campos.
            ok = leitura_dados_pops(f, posicoes[j], cabecalho.proxByteOffset, &dado);
            if (ok) {
                print_dados_pops(&dado);
                destroi_dados_pops(&dado);
            }
        }
        free(posicoes);
    }
    // Ao final de uma execucao bem-sucedida, uma linha em branco finaliza a saida.
    if (ok)
        printf("\n");
    if (!fechar_arquivos_pops(f, fi, &indice))
        ok = 0;
    if (!ok)
        printf("Falha no processamento do arquivo.\n");
}
