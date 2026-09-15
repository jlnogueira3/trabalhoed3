//
// Created by joao-lucas-nogueira on 15/09/2026.
//
#include <stdio.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/auxiliares/auxiliar_1.h"


void funcionalidade6(const char*binario, int n){
    FILE *f;
    f = fopen(binario, "r+b");
    if(f == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    registro_cabecalho cabecalho;

    if(leitura_cabecalho(f, &cabecalho) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return;
    }

    //se o status for diferente de 1, informa falha e fecha o arquivo
    if(cabecalho.status != '1'){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return;
    }

    cabecalho.status = '0';     //0 no status do cabecalho pq estamos editando

    // volta ao byte zero e grava status 0
    if(fseek(f, 0, SEEK_SET) != 0 || fwrite(&cabecalho.status, sizeof(char), 1, f) != 1 || fflush(f) != 0){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return;
    }

}