/*
 * João L A Nogueira - N usp: 16986970
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

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
