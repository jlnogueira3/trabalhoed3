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


void funcionalidade4(const char *binario, int rrn){
    FILE* f;
    f = fopen(binario, "rb");

    // primeiro devemos verificar, assim como nas funcionalidades anteriores, a consistência do
    // arquivo
    if(f == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    registro_cabecalho cabecalho;

    if(leitura_cabecalho(f, &cabecalho) == 0){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return;
    }

    if(cabecalho.status != '1'){       //arquivo inconsistente nao pode ser consultado
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return;
    }

    if(0 > rrn || rrn >= cabecalho.proxRRN){    // 0 <= rrn < proxRRN
        printf("Registro inexistente.\n");
        fclose(f);
        return;
    }
    long int pos = 17L + (long)rrn*18L;
    if(fseek(f, pos, SEEK_SET) != 0){      //fseek retorna 0 quando funciona
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return;
    }

    //LEITURA DO DADO
    registro_dados dado;
    if(leitura_dados(&dado, f) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return;
    }

    //print do dado
    if(dado.removido == '1')
        printf("Registro inexistente.\n");
    else{
        print_dados(dado);
    }
    fclose(f);
}
