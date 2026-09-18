//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

//Lê os registros de um arquivo CSV e os converte para o
//formato binário, criando também o cabeçalho do arquivo.
void funcionalidade1(const char *csv, char *binario,
                     registro_cabecalho *cabecalho) {
    // Primeiro devemos nos certificar de que o arquivo está completo
    FILE *leitura = fopen(csv, "rb");

    if (leitura == NULL) {
        printf("Falha no processamento do arquivo.");
        return;
    }

    FILE *escrita = fopen(binario, "w+b");

    if (escrita == NULL) {
        printf("Falha no processamento do arquivo.");
        fclose(leitura);
        return;
    }
        //Agora que está tudo correto,
        // devemos inicializar o registro de cabeçalho conforme
        // a especificação do trabalho

        inicializar_cabecalho(cabecalho);
        // As funções auxiliares usadas nesse arquivo estão em
        // Source/auxiliares/auxiliar_1.c


        escrever_cabecalho(cabecalho,escrita);

        // Agora partimos para a leitura em si do arquivo

        char linha[100];

        fgets(linha, sizeof(linha), leitura);

    while (fgets(linha, sizeof(linha), leitura) != NULL) {

        registro_dados dados = {
            .idPoPs = dados.removido = '0',
            .idPoPsConectado = dados.encadeamentoPilha = -1,
            .velocidade = dados.velocidade = -1,
            .unidadeMedida = dados.unidadeMedida = '$',
        };

        // DEVEMOS inicializar conforme especificado no trabalho
        /*
         * Separamos manualmente os campos porque velocidade
         * e unidadeMedida podem estar vazios no CSV
         */
        char *campo1 = linha;
        char *virgula1 = strchr(campo1, ',');
        *virgula1 = '\0';
        char *campo2 = virgula1 + 1;
        char *virgula2 = strchr(campo2, ',');
        *virgula2 = '\0';
        char *campo3 = virgula2 + 1;
        char *virgula3 = strchr(campo3, ',');
        *virgula3 = '\0';
        char *campo4 = virgula3 + 1;

        /*
             * Retira a quebra de linha do último campo
             */
        campo4[strcspn(campo4, "\r\n")] = '\0';
        dados.idPoPs = atoi(campo1);
        dados.idPoPsConectado = atoi(campo2);
        if (campo3[0] != '\0') {
            dados.velocidade = atoi(campo3);
        }
        if (campo4[0] != '\0') {
            dados.unidadeMedida = campo4[0];
        }


        // Agora partimos para a escrita do arquivo csv
        // Um detalhe interessante é que eu (João) fiquei me batendo para
        // chegar em fwrite e dar certo, antes lia apenas lixo de memória
        // usando o fgets() e rodava um monte de 0 no runcodes
        escrever_dados(dados,escrita);

        //Cada novo registro ocupa um novo RRN.
        // Como o enunciado garante que os pares não são repetidos,
        // cada registro também adiciona um novo par.
        //
        cabecalho->proxRRN++;
        cabecalho->nroPares++;
    }
    /*
         * Ao finalizar corretamente a construção do arquivo,
         * atualizamos o status para consistente
         */
        cabecalho->status = '1';

        //
        // Voltamos para o início do arquivo para atualizar
        // os 17 bytes do cabeçalho

        fseek(escrita, 0, SEEK_SET);

        escrever_cabecalho(cabecalho,escrita);
        fclose(escrita);
        fclose(leitura);
        // modifiquei a parte do binário, colocando apenas char *binario ao invés de
        // "const char binario" a fim de adaptar a função a "fornecidas.c"
        BinarioNaTela(binario);
}
