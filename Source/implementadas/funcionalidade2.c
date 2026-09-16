#include <stdio.h>
#include <string.h>
#include "../../Headers/registros.h"
#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

//Percorre o arquivo binário
//e exibe todos os registros válidos, desconsiderando os
//registros removidos.
void funcionalidade2(const char *binario, registro_cabecalho *cabecalho) {

    // Primeiro devemos verificar se o arquivo está consistente
    FILE *f3 = fopen(binario, "rb");

    if (f3 == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    if (fread(&cabecalho->status, sizeof(char), 1, f3) != 1 ||
    fread(&cabecalho->topo_pilha, sizeof(int), 1, f3) != 1 ||
    fread(&cabecalho->proxRRN, sizeof(int), 1, f3) != 1 ||
    fread(&cabecalho->nroRegRem, sizeof(int), 1, f3) != 1 ||
    fread(&cabecalho->nroPares, sizeof(int), 1, f3) != 1) {

        printf("Falha no processamento do arquivo.\n");
        fclose(f3);
        return;
    }

    // A partir disso devemos ler o status do cabecalho do arquivo
    // se der erro vamos imprimir a mensagem de falha

    if (cabecalho->status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(f3);
        return;
    }


    // COmo lemos o cabeçalho podemos seguir

    int encontrou = 0;

    for (int i = 0; i < cabecalho->proxRRN; i++) {

        registro_dados dados = {
            .idPoPs = -1,
            .idPoPsConectado = -1,
            .velocidade = -1,
            .unidadeMedida = '$',
            .removido = '0',
            .encadeamentoPilha = -1
        };

        // Lê o primeiro byte do registro
        if (leitura_dados(&dados, f3) != 1) {
            break;
        }

        // Registro logicamente removido não é mostrado
        if (dados.removido == '1') {
            continue;
        }

        encontrou = 1;

        print_dados(dados);
    }

    if (encontrou == 0) {
        printf("Registro inexistente.\n");
    }

    fclose(f3);
}