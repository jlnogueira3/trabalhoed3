//
// Created by joao-lucas-nogueira on 15/09/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

void funcionalidade3(const char *binario,registro_cabecalho *cabecalho,char *n) {

    //Primeiro devemos verificar a integridade do arquivo e se está tudo correto
    FILE *f4 = fopen(binario, "rb");
    if (f4 == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Verifica status do arquivo
    fread(&cabecalho->status, sizeof(char), 1, f4);

    if (cabecalho->status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(f4);
        return;
    }

    long int n_int = strtol(n, NULL, 10);


    // Cada iteração representa uma busca
    for (int i = 0; i < n_int; i++) {
        int quantidade_pares;
        scanf("%d", &quantidade_pares);

        struct chave_valor *p =
            calloc(quantidade_pares, sizeof(struct chave_valor));

        if (p == NULL) {
            printf("Falha no processamento do arquivo.\n");
            fclose(f4);
            return;
        }
        //Devemos agora ler os filtros passados utilizando a funcao ScanQuoteString que
        // lê de um jeito mais "clean"

        for (int j = 0; j < quantidade_pares; j++) {
            scanf("%s", p[j].chave);
            ScanQuoteString(p[j].valor);
        }

        //Antes de comecar a percorrer o arquivo devemos pular o cabeçalho
        fseek(f4, 17, SEEK_SET);
        int match = 0;

        while (1) {

            registro_dados dados3 = {
                .removido = '0',
                .encadeamentoPilha = -1,
                .idPoPs = -1,
                .idPoPsConectado = -1,
                .velocidade = -1,
                .unidadeMedida = '$'
            };

            if (leitura_dados(&dados3, f4) != 1) {
                break;
            }

            if (dados3.removido == '1') {
                continue;
            }

            int filtro = 1;
            filtro = filtrar_dados(p, quantidade_pares, dados3, filtro);

            if (filtro == 1) {
                match = 1;
                print_dados(dados3);
            }
        }
        if (match == 0) {
            printf("Registro inexistente.\n");
        }
        free(p);
        printf("\n");
    }
    fclose(f4);
}


