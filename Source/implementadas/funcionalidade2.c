/*
 * João L A Nogueira - N usp: 16986970
 */
#include <stdio.h>
#include "../../Headers/registros.h"
#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

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
