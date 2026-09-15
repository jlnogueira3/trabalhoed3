//
// Created by joao-lucas-nogueira on 08/09/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Headers/registros.h"
#include "../Headers/fornecidas.h"
#include "../Headers/implementadas.h"
#include "../Headers/auxiliares/auxiliar_1.h"

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

            registro_dados dados3 = {
                .removido = '0',
                .encadeamentoPilha = -1,
                .idPoPs = -1,
                .idPoPsConectado = -1,
                .velocidade = -1,
                .unidadeMedida = '$'
            };

            if (leitura_dados(&dados3, f4) != 1) {
                break;
            }

            if (dados3.removido == '1') {
                continue;
            }

            int filtro = 1;
            filtro = filtrar_dados(p, quantidade_pares, dados3, filtro);

            if (filtro == 1) {
                match = 1;
                print_dados(dados3);
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

void funcionalidade5(const char*binario, int n){
    FILE *f;
    f = fopen(binario, "r+b");
    if(f == NULL){
        printf("Falha no processamento do arquivo.\n");
        return; 
        }

        //leitura do cabecalho
    registro_cabecalho cabecalho;
    if(leitura_cabecalho(f, &cabecalho) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return; 
    }

    if(cabecalho.status != '1'){       //arquivo inconsistente nao pode ser consultado
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return;
    }

    //0 no status do cabecalho pq estamos editando
    cabecalho.status = '0';
    if(fseek(f, 0, SEEK_SET) != 0 || fwrite(&cabecalho.status, sizeof(char), 1, f) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(f);
        return;
    }

    registro_dados dado;    //variavel auxiliar
    for(int i = 0; i<n; i++){

        //quantidade de criterios desta solicitacao
        int quantidade;
        scanf("%d", &quantidade);
        if(quantidade <= 0){  
            printf("Falha no processamento do arquivo.\n");
            fclose(f);
            return;
        }
        struct chave_valor *criterios;
        criterios = calloc(quantidade, sizeof(struct chave_valor));
        if(criterios == NULL){
            printf("Falha no processamento do arquivo.\n");
            fclose(f);
            return; //falha na alocacao
        }
        /*struct chave_valor {
        char chave[50];
        char valor[50];
        };*/

        for(int j =0; j < quantidade; j++){
            scanf("%49s", criterios[j].chave);
            ScanQuoteString(criterios[j].valor);    //funcao dada
        }

        for(int rrn = 0; rrn < cabecalho.proxRRN; rrn++){
            long int pos = 17L +(long)rrn*18L;
            if(fseek(f, pos, SEEK_SET)!=0 || leitura_dados(&dado, f) != 1){
                free(criterios);
                fclose(f);
                printf("Falha no processamento do arquivo.\n");
                return;
            }
            if(dado.removido == '1')    //se tiver sido removido, vai pro proximo
                continue;
            int tester = atende_criterios(&dado, criterios, quantidade);
            if (tester == 0)
                continue; // vai pro proximo registro
            dado.removido = '1';
            dado.encadeamentoPilha = cabecalho.topo_pilha;

            //volta pro inicio do registro e grava os campos de controle
            if(fseek(f, pos, SEEK_SET) != 0 ||
            fwrite(&dado.removido, sizeof(char), 1, f) != 1 ||
            fwrite(&dado.encadeamentoPilha, sizeof(int), 1, f) != 1){
                free(criterios);
                fclose(f);
                printf("Falha no processamento do arquivo.\n");
                return;
            }

            //preenche os campos de dados com lixo
            char lixo = '$';
            for(int k = 0; k < 13; k++){
                if(fwrite(&lixo, sizeof(char), 1, f) != 1){
                    free(criterios);
                    fclose(f);
                    printf("Falha no processamento do arquivo.\n");
                    return;
                }
            }

            //atualiza a pilha e os contadores
            cabecalho.topo_pilha = rrn;
            cabecalho.nroRegRem++;
        }
        free(criterios);
    }

    //grava o cabecalho atualizado ainda com status '0'
    if(fseek(f, 0, SEEK_SET) != 0 ||
       fwrite(&cabecalho.status, sizeof(char), 1, f) != 1 ||
       fwrite(&cabecalho.topo_pilha, sizeof(int), 1, f) != 1 ||
       fwrite(&cabecalho.proxRRN, sizeof(int), 1, f) != 1 ||
       fwrite(&cabecalho.nroRegRem, sizeof(int), 1, f) != 1 ||
       fwrite(&cabecalho.nroPares, sizeof(int), 1, f) != 1 ||
       fflush(f) != 0){
        fclose(f);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //marca como consistente depois de gravar as alteracoes
    cabecalho.status = '1';
    if(fseek(f, 0, SEEK_SET) != 0 ||
       fwrite(&cabecalho.status, sizeof(char), 1, f) != 1 ||
       fflush(f) != 0){
        fclose(f);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    if(fclose(f) != 0){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    BinarioNaTela((char *)binario);
}
