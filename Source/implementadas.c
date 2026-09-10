//
// Created by joao-lucas-nogueira on 08/09/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Headers/registros.h"
#include "../Headers/fornecidas.h"
#include "../Headers/implementadas.h"


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

        cabecalho->status = '0';
        cabecalho->topo_pilha = -1;
        cabecalho->proxRRN = 0;
        cabecalho->nroRegRem = 0;
        cabecalho->nroPares = 0;

        /*
         * Escrevemos inicialmente o cabeçalho com status '0',
         * pois o arquivo ainda está sendo construído
         */
        fwrite(&cabecalho->status, sizeof(char), 1, escrita);
        fwrite(&cabecalho->topo_pilha, sizeof(int), 1, escrita);
        fwrite(&cabecalho->proxRRN, sizeof(int), 1, escrita);
        fwrite(&cabecalho->nroRegRem, sizeof(int), 1, escrita);
        fwrite(&cabecalho->nroPares, sizeof(int), 1, escrita);

        // Agora partimos para a leitura em si do arquivo

        char linha[100];

        fgets(linha, sizeof(linha), leitura);

        while (fgets(linha, sizeof(linha), leitura) != NULL) {

            registro_dados dados;

            // DEVEMOS inicializar conforme especificado no trabalho
            dados.removido = '0';
            dados.encadeamentoPilha = -1;
            dados.velocidade = -1;
            dados.unidadeMedida = '$';

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
            fwrite(&dados.removido, sizeof(char), 1, escrita);
            fwrite(&dados.encadeamentoPilha, sizeof(int), 1, escrita);
            fwrite(&dados.idPoPs, sizeof(int), 1, escrita);
            fwrite(&dados.idPoPsConectado, sizeof(int), 1, escrita);
            fwrite(&dados.velocidade, sizeof(int), 1, escrita);
            fwrite(&dados.unidadeMedida, sizeof(char), 1, escrita);
            // Escrita do arquivo .csv no binário
            /*
             * Cada novo registro ocupa um novo RRN.
             * Como o enunciado garante que os pares não são repetidos,
             * cada registro também adiciona um novo par.
             */
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
        fwrite(&cabecalho->status, sizeof(char), 1, escrita);
        fwrite(&cabecalho->topo_pilha, sizeof(int), 1, escrita);
        fwrite(&cabecalho->proxRRN, sizeof(int), 1, escrita);
        fwrite(&cabecalho->nroRegRem, sizeof(int), 1, escrita);
        fwrite(&cabecalho->nroPares, sizeof(int), 1, escrita);
        fclose(escrita);
        fclose(leitura);
        // modifiquei a parte do binário, colocando apenas char *binario ao invés de
        // "const char binario" a fim de adaptar a função a "fornecidas.c"
        BinarioNaTela(binario);
}


void funcionalidade2(const char *binario, registro_cabecalho *cabecalho) {

    // Primeiro devemos verificar se o arquivo está consistente
    FILE *f3 = fopen(binario, "rb");

    if (f3 == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // A partir disso devemos ler o status do cabecalho do arquivo
    // se der erro vamos imprimir a mensagem de falha
    if (fread(&cabecalho->status, sizeof(char), 1, f3) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(f3);
        return;
    }

    if (cabecalho->status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(f3);
        return;
    }

    fread(&cabecalho->topo_pilha, sizeof(int), 1, f3);
    fread(&cabecalho->proxRRN, sizeof(int), 1, f3);
    fread(&cabecalho->nroRegRem, sizeof(int), 1, f3);
    fread(&cabecalho->nroPares, sizeof(int), 1, f3);

    // COmo lemos o cabeçalho podemos seguir

    int encontrou = 0;

    for (int i = 0; i < cabecalho->proxRRN; i++) {

        registro_dados dados;

        // Primeiro campo de cada registro
        if (fread(&dados.removido, sizeof(char), 1, f3) != 1)
            break;

        fread(&dados.encadeamentoPilha, sizeof(int), 1, f3);
        fread(&dados.idPoPs, sizeof(int), 1, f3);
        fread(&dados.idPoPsConectado, sizeof(int), 1, f3);
        fread(&dados.velocidade, sizeof(int), 1, f3);
        fread(&dados.unidadeMedida, sizeof(char), 1, f3);

        // Ignora registros logicamente removidos
        if (dados.removido == '1')
            continue;

        encontrou = 1;
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
            printf("NULO");
        } else {
            printf("\"%c\"", dados.unidadeMedida);
        }

        printf("\n");
    }

    if (encontrou == 0) {
        printf("Registro inexistente.\n");
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
        printf("\n");
    }


    fclose(f4);
}

void funcionalidade4(const char *binario, int rrn){
    FILE* f; 
    f = fopen(binario, "rb");

    // Primeiro devemos verificar, assim como nas funcionalidades anteriores, a consistência do
    // arquivo
    if(f == NULL){
        printf("Falha no processamento do arquivo.\n");
        return; 
        }
    registro_cabecalho cabecalho;
    
    //Agora partimos para a  (obs fread retorna a quantidade de elementos lidos)
    if(fread(&cabecalho.status, sizeof(char), 1, f) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        }       //leitura do byte 0
    if(fread(&cabecalho.topo_pilha, sizeof(int), 1, f)!= 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        }     //leitura dos bytes 1 a 4
    if(fread(&cabecalho.proxRRN, sizeof(int), 1, f)!= 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        }        //leitura dos bytes 5 a 8
    if(fread(&cabecalho.nroRegRem, sizeof(int), 1, f)!= 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        }      //leitura dos bytes 9 a 12
    if(fread(&cabecalho.nroPares, sizeof(int), 1, f)!= 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        }          //leitura dos bytes 13 a 16

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
    if(fseek(f, pos, 0) != 0){      //fseek retorna 0 quando funciona
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        } 
    
    //LEITURA DO DADO
    registro_dados dado;
    if(fread(&dado.removido, sizeof(char), 1, f) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        }  
    if(fread(&dado.encadeamentoPilha, sizeof(int), 1, f) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        }  
    if(fread(&dado.idPoPs, sizeof(int), 1, f) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        }  
    if(fread(&dado.idPoPsConectado, sizeof(int), 1, f) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        }  
    if(fread(&dado.velocidade, sizeof(int), 1, f) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
        }  
    if(fread(&dado.unidadeMedida, sizeof(char), 1, f) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return;
    }

    //print do dado
    if(dado.removido == '1')
        printf("Registro inexistente.");    // nao precisa de \n pq ja tem antes do fim da funcao
    else{
        if(dado.velocidade == -1 && dado.unidadeMedida == '$'){
            printf("%d %d NULO NULO", dado.idPoPs, dado.idPoPsConectado);
        }
        else if(dado.velocidade == -1){
            printf("%d %d NULO \"%c\"", dado.idPoPs, dado.idPoPsConectado, dado.unidadeMedida);
        }
        else if(dado.unidadeMedida == '$'){
            printf("%d %d %d NULO", dado.idPoPs, dado.idPoPsConectado, dado.velocidade);
        }
        else
            printf("%d %d %d \"%c\"", dado.idPoPs, dado.idPoPsConectado, dado.velocidade, dado.unidadeMedida);
    }
    fclose(f);
    printf("\n");
}
