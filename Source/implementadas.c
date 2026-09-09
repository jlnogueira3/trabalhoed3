//
// Created by joao-lucas-nogueira on 08/09/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include "../Headers/registros.h"



void funcionalidade1(const char *csv,const char *binario,struct registro_cabecalho *cabecalho){
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

    // Já que ocorreu tudo normal podemos iniciar a leitura do arquivo

    char linha[100];
    fgets(linha,sizeof(linha),f2);


    while (fgets(linha,sizeof(linha),f2) != NULL) {
        struct registro_dados dados;

        dados.removido = '0';
        dados.encadeamentoPilha = -1;

        /*Antes de começar a ler o registro devemos verificar se o registro
         está registrado como removido

        Preciso perguntar ao Renan para saber como que vou saber isso
         */


        sscanf(linha,"%d,%d,%d,%c",
            &dados.idPoPs,
            &dados.idPoPsConectado,
            &dados.velocidade,
            &dados.unidadeMedida);

        // Leitura do arquivo csv no formato indicado do registro de dados
        fwrite(&dados.removido, sizeof(char), 1, f1);
        fwrite(&dados.encadeamentoPilha, sizeof(int), 1, f1);
        fwrite(&dados.idPoPs, sizeof(int), 1, f1);
        fwrite(&dados.idPoPsConectado, sizeof(int), 1, f1);
        fwrite(&dados.velocidade, sizeof(int), 1, f1);
        fwrite(&dados.unidadeMedida, sizeof(char), 1, f1);
        // Escrita do arquivo .csv no binário

    }


    fclose(f1);
    fclose(f2);
}
