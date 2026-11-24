// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/consulta.h"

#include <stdio.h>
#include <stdlib.h>

#include "../Headers/registros.h"
#include "../Headers/cabecalho.h"
#include "../Headers/registro.h"
#include "../Headers/criterios.h"
#include "../Headers/erros.h"

// Percorre o arquivo binário e exibe todos os registros válidos,
// desconsiderando os registros logicamente removidos.
void funcionalidade2(const char *binario) {
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

    // Após o cabeçalho, a leitura começa no primeiro RRN.
    // leitura_cabecalho deixa o arquivo posicionado logo após o cabeçalho.
    int encontrou = 0;
    for (int rrn = 0; rrn < cabecalho.proxRRN; rrn++) {
        registro_dados dados;
        int resultado = leitura_dados(&dados, f);

        if (resultado == LEITURA_ERRO) {
            falha_processamento_arquivo(f);
            return;
        }

        // Registro removido: leitura_dados já pulou seus bytes
        if (resultado == LEITURA_REMOVIDO)
            continue;

        encontrou = 1;
        print_dados(dados);
    }

    if (encontrou == 0)
        printf("Registro inexistente.\n");

    fclose(f);
}

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

            if (atende_criterios(&dados, p, quantidade_pares)) {
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

// Acessa diretamente um registro pelo seu RRN e o exibe caso ele exista e não esteja removido.
void funcionalidade4(const char *binario, int rrn) {
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

    // RRN válido: 0 <= rrn < proxRRN
    if (rrn < 0 || rrn >= cabecalho.proxRRN) {
        printf("Registro inexistente.\n");
        fclose(f);
        return;
    }

    // Posiciona o arquivo no início do registro desejado
    fseek(f, posicao_registro(rrn), SEEK_SET);

    registro_dados dado;
    int resultado = leitura_dados(&dado, f);

    if (resultado == LEITURA_ERRO) {
        falha_processamento_arquivo(f);
        return;
    }

    if (resultado == LEITURA_REMOVIDO)
        printf("Registro inexistente.\n");
    else
        print_dados(dado);

    fclose(f);
}
