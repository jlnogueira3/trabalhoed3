//
// Created by joao-lucas-nogueira on 08/09/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include "../Headers/registros.h"
#include "fornecidas.c"



void funcionalidade1(const char *csv,char *binario,struct registro_cabecalho *cabecalho){
    // Primeiro devemos nos certificar de que o arquivo está completo
    FILE *f2 = fopen(csv, "rb");
    if (f2 == NULL) {
        printf("Falha no processamento do arquivo.");
        cabecalho->status = '0';
        return;
    }

    FILE *f1 = fopen(binario, "w+b");
    if (f1 == NULL) {
        printf("Falha no processamento do arquivo.");
        cabecalho->status = '0';
        fclose(f2);
        return;
    }
    cabecalho->status = '1';

    /* Já que ocorreu tudo normal podemos iniciar a leitura do arquivo,
     Para isso vamos ler conforme o csv padrão indicado pelo csv padrão

    */

    char linha[100];
    fgets(linha,sizeof(linha),f2);


    while (fgets(linha,sizeof(linha),f2) != NULL) {
        struct registro_dados dados;



        sscanf(linha,"%d,%d,%d,%c",
            &dados.idPoPs,
            &dados.idPoPsConectado,
            &dados.velocidade,
            &dados.unidadeMedida
            );

        //Como a documentação do trabalho
        // garante que o .csv não tem campos vazios
        // não verificaremos se foram lidos os 4 campos




        // Leitura do arquivo csv no formato indicado do registro de dados
        fwrite(&dados.idPoPs, sizeof(int), 1, f1);
        fwrite(&dados.idPoPsConectado, sizeof(int), 1, f1);
        fwrite(&dados.velocidade, sizeof(int), 1, f1);
        fwrite(&dados.unidadeMedida, sizeof(char), 1, f1);
        // Escrita do arquivo .csv no binário

    }
    fclose(f1);
    fclose(f2);
// modifiquei a parte do binário, colocando apenas char *binario ao invés de
    // "const char binario" a fim de adaptar a função a "fornecidas.c"
    BinarioNaTela(binario);
}
