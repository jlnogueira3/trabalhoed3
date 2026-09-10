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
        registro_dados dados;

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

void funcionalidade3(const char *binario,registro_cabecalho *cabecalho,char *n) {

    //Primeiro devemos verificar a integridade do arquivo e se está tudo correto
    FILE *f4 = fopen(binario, "rb");
    if (f4 == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Verifica status do arquivo
    fread(&cabecalho->status, sizeof(char), 1, f4);

    if (cabecalho->status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(f4);
        return;
    }

    long int n_int = strtol(n, NULL, 10);


    // Cada iteração representa uma busca
    for (int i = 0; i < n_int; i++) {
        int quantidade_pares;
        scanf("%d", &quantidade_pares);

        struct chave_valor *p =
            calloc(quantidade_pares, sizeof(struct chave_valor));

        if (p == NULL) {
            printf("Falha no processamento do arquivo.\n");
            fclose(f4);
            return;
        }


        //Devemos agora ler os filtros passados utilizando a funcao ScanQuoteString que
        // lê de um jeito mais "clean"

        for (int j = 0; j < quantidade_pares; j++) {
            scanf("%s", p[j].chave);
            ScanQuoteString(p[j].valor);
        }

        //Antes de comecar a percorrer o arquivo devemos pular o cabeçalho
        fseek(f4, 17, SEEK_SET);
        int match = 0;

        while (1) {
            registro_dados dados3;
            if (fread(&dados3.removido,sizeof(char), 1, f4) != 1) {
                break;
            }
            fread(&dados3.encadeamentoPilha,sizeof(int), 1, f4);
            fread(&dados3.idPoPs,sizeof(int), 1, f4);
            fread(&dados3.idPoPsConectado,sizeof(int), 1, f4);
            fread(&dados3.velocidade,sizeof(int), 1, f4);
            fread(&dados3.unidadeMedida,sizeof(char), 1, f4);


            // Ignora registro removido
            if (dados3.removido == '1') {
                continue;
            }

            /*
             * Começamos supondo que o registro
             * satisfaz todos os critérios.
             */
            int filtro = 1;


         //Agora que o arquivo está integro devemos passar campo a campo verificando da mesma maneira
         // que fizemos na funcionalidade 2 conferindo se estava vazio e comparando com os valores passados
         // Para isso devemos passar campo a campo fazendo  o cara-crachá da função
            for (int j = 0; j < quantidade_pares; j++) {
                if (strcmp(p[j].chave, "idPoPs") == 0) {
                    int valor = (int) strtol(p[j].valor, NULL, 10);
                    if (dados3.idPoPs != valor) {
                        filtro = 0;
                        break;
                    }

                }
                else if (strcmp(p[j].chave,"idPoPsConectado") == 0) {
                    int valor = (int) strtol(p[j].valor, NULL, 10);
                    if (dados3.idPoPsConectado != valor) {
                        filtro = 0;
                        break;
                    }
                }

                else if (strcmp(p[j].chave,"velocidade") == 0) {
                    if (p[j].valor[0] == '\0') {
                        if (dados3.velocidade != -1) {
                        filtro = 0;
                        break;
                        }
                    }
                    else {
                        int valor = (int) strtol(p[j].valor,NULL, 10);
                        if (dados3.velocidade != valor) {
                        filtro = 0;
                        break;
                        }
                    }
                }

                else if (strcmp(p[j].chave, "unidadeMedida") == 0) {
                    if (p[j].valor[0] == '\0') {
                        if (dados3.unidadeMedida != '$') {
                        filtro = 0;
                        break;
                        }
                    }

                    else {
                        if (dados3.unidadeMedida != p[j].valor[0]) {
                        filtro = 0;
                        break;
                        }
                    }
                }
            }


    // Conforme inicializamos o valor "filtro" anteriormente na nossa função
    // Agora vamos utilizar como parâmetro para printar

            if (filtro == 1) {
                match = 1;
                // Aqui já sabemos que o idPoPs não pode ser nulo por isso seguimos diretamente
                printf("%d ", dados3.idPoPs);
                printf("%d ", dados3.idPoPsConectado);

                if (dados3.velocidade == -1) {
                    printf("NULO ");
                }
                else {
                    printf("%d ", dados3.velocidade);
                }
                if (dados3.unidadeMedida == '$') {
                    printf("NULO\n");
                }
                else {
                    printf("\"%c\"\n",
                           dados3.unidadeMedida);
                }
            }
        }


        if (match == 0) {
            printf("Registro inexistente.\n");
        }

        free(p);
    }


    fclose(f4);
}

void funcionalidade4(const char *binario, int rrn){
    FILE* f; 
    f = fopen(binario, "rb");
    if(f == NULL){
        printf("Falha no processamento do arquivo");
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
