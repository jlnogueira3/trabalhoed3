//
// Created by joao-lucas-nogueira on 08/09/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include "../Headers/registros.h"
#include "fornecidas.c"
#include "../Headers/implementadas.h"



void funcionalidade1(const char *csv,char *binario, registro_cabecalho *cabecalho){
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
        registro_dados dados;

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


void funcionalidade2(const char *binario,  registro_cabecalho *cabecalho) {
    FILE *f3 = fopen(binario, "rb");
    if (f3 == NULL) {
        printf("Registro inexistente.");
        cabecalho->status = '0';
        fclose(f3);
        return;
    }
    cabecalho->status = '1';
    /* Já que ocorreu tudo certo, podemos iniciar a leitura
     */
    char linha[100];
    fgets(linha,sizeof(linha),f3);
    while (fgets(linha,sizeof(linha),f3) != NULL){
        struct registro_dados dados;

        sscanf(linha,"%d,%d,%d,%c",
    &dados.idPoPs,
    &dados.idPoPsConectado,
    &dados.velocidade,
    &dados.unidadeMedida);

        if (dados.idPoPs == -1) {
            printf("NULO ");
        } else {
            printf("%d ", dados.idPoPs);
        }

        if (dados.idPoPsConectado == -1) {
            printf("NULO ");
        } else {
            printf("%d ", dados.idPoPsConectado);
        }

        if (dados.velocidade == -1) {
            printf("NULO ");
        } else {
            printf("%d ", dados.velocidade);
        }

        if (dados.unidadeMedida == '$') {
            printf("NULO ");
        } else {
            printf("%c ", dados.unidadeMedida);
        }
    }
    fclose(f3);
}

void funcionalidade3(const char *binario,  registro_cabecalho *cabecalho, char *n) {
    FILE *f4 = fopen(binario, "rb");
    if (f4 == NULL) {
        printf("Falha no processamento do arquivo.");
        cabecalho->status = '0';
    }
    // Como está tudo certo com o arquivo vamos começar a manipulação, para isso devemos pensar em
    //como receber os argumentos e parâmetros
    cabecalho->status = '1';


    long int n_int = strtol(n,NULL,10);

    for (int i = 0; i < n_int; i++) {
        int quantidade_pares;
        scanf("%d", &quantidade_pares);

        struct chave_valor *p =
            calloc(quantidade_pares, sizeof(struct chave_valor));
        for (int j = 0; j < quantidade_pares; j++) {
            scanf("%s", p[j].chave);
            scanf("%s", p[j].valor);
        }

        free(p);
        }

    fclose(f4);
}

void funcionalidade4(const char *binario, int rrn){
    FILE* f; 
    f = fopen(binario, "rb");
    if(f == NULL){
        printf("Falha no processamento do arqvuivo");
        return; 
        }
    registro_cabecalho cabecalho;
    
    //LEITURA DO CABECALHO (obs fread retorna a quantidade de elementos lidos)
    if(fread(&cabecalho.status, sizeof(char), 1, f) != 1){
        printf("Falha no processamento do arquivo");
        fclose(f);
        return; 
        }       //leitura do byte 0
    if(fread(&cabecalho.topo_pilha, sizeof(int), 1, f)!= 1){
        printf("Falha no processamento do arquivo");
        fclose(f);
        return; 
        }     //leitura dos bytes 1 a 4
    if(fread(&cabecalho.proxRRN, sizeof(int), 1, f)!= 1){
        printf("Falha no processamento do arquivo");
        fclose(f);
        return; 
        }        //leitura dos bytes 5 a 8
    if(fread(&cabecalho.nroRegRem, sizeof(int), 1, f)!= 1){
        printf("Falha no processamento do arquivo");
        fclose(f);
        return; 
        }      //leitura dos bytes 9 a 12
    if(fread(&cabecalho.nroPares, sizeof(int), 1, f)!= 1){
        printf("Falha no processamento do arquivo");
        fclose(f);
        return; 
        }          //leitura dos bytes 13 a 16

    if(0 > rrn || rrn >= cabecalho.proxRRN){
        printf("Registro inexistente");
        fclose(f);
        return;
    }
    long int pos = 17 + rrn*18;
    if(fseek(f, pos, 0) != 1){
        printf("Falha no processamento do arquivo");
        fclose(f);
        return; 
        }  
    
    
}
