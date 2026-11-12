/*
 * João L A Nogueira - N usp: 16986970
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

// Realiza buscas sequenciais no arquivo binário usando um ou mais pares campo/valor como critérios.
// São realizadas n buscas em sequência.
void funcionalidade3(const char *binario, const char *n) {
    // Somente leitura
    FILE *f = fopen(binario, "rb");
    if (verificar_abertura_arquivo(f) == 0)
        return;

    // Lê o cabeçalho. Retorna 0 se o arquivo estiver inconsistente.
    registro_cabecalho cabecalho;
    if (leitura_cabecalho(f, &cabecalho) == 0) {
        falha_processamento_arquivo(f);
        return;
    }

    long int n_int = strtol(n, NULL, 10);

    // Cada iteração representa uma busca
    for (int i = 0; i < n_int; i++) {
        int quantidade_pares;
        scanf("%d", &quantidade_pares);

        struct chave_valor *p = ler_pares(quantidade_pares);
        if (p == NULL) {
            falha_processamento_arquivo(f);
            return;
        }

        // Antes de cada busca, volta para o primeiro registro (logo após o cabeçalho)
        fseek(f, TAM_CABECALHO, SEEK_SET);

        // Percorre todos os RRNs do arquivo
        int match = 0;
        for (int rrn = 0; rrn < cabecalho.proxRRN; rrn++) {
            registro_dados dados;
            int resultado = leitura_dados(&dados, f);

            if (resultado == LEITURA_ERRO) {
                free(p);
                falha_processamento_arquivo(f);
                return;
            }

            // Registros removidos não entram na busca
            if (resultado == LEITURA_REMOVIDO)
                continue;

            if (filtrar_dados(p, quantidade_pares, dados, 1) == 1) {
                match = 1;
                print_dados(dados);
            }
        }

        if (match == 0)
            printf("Registro inexistente.\n");

        free(p);
        printf("\n");
    }

    fclose(f);
}
