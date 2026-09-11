//
// Created by joao-lucas-nogueira on 11/09/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

// Implementação das funções auxiliares

//Começamos com a função atualizar cabecalho
void inicializar_cabecalho(registro_cabecalho *cabecalho) {
    cabecalho->status = '0';
    cabecalho->topo_pilha = -1;
    cabecalho->proxRRN = 0;
    cabecalho->nroRegRem = 0;
    cabecalho->nroPares = 0;

    /*
         * Escrevemos inicialmente o cabeçalho com status '0',
         * pois o arquivo ainda está sendo construído
         */
}

void escrever_cabecalho(const registro_cabecalho *cabecalho,
                        FILE *arquivo) {

    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->topo_pilha, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->proxRRN, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroPares, sizeof(int), 1, arquivo);
}

// Agora partimos para as funções de registros de dados
// é importante que seja implementado sem um loop
// a fim de essas funções consigam rodar dentro e fora
// de um loop, de forma a facilitar a implementação

void escrever_dados(registro_dados dados, FILE *arquivo) {
    fwrite(&dados.removido, sizeof(char), 1, arquivo);
    fwrite(&dados.encadeamentoPilha, sizeof(int), 1, arquivo);
    fwrite(&dados.idPoPs, sizeof(int), 1, arquivo);
    fwrite(&dados.idPoPsConectado, sizeof(int), 1, arquivo);
    fwrite(&dados.velocidade, sizeof(int), 1, arquivo);
    fwrite(&dados.unidadeMedida, sizeof(char), 1, arquivo);
}

void print_dados(registro_dados dados) {
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
        printf("\"%c\"\n", dados.unidadeMedida);
    }
}

// Agora devemos implementar a leitura dos dados

int leitura_dados(registro_dados *dados, FILE *arquivo) {
    if (fread(&dados->removido, sizeof(char), 1, arquivo) != 1)
        return 0;

    if (fread(&dados->encadeamentoPilha, sizeof(int), 1, arquivo) != 1)
        return 0;

    if (fread(&dados->idPoPs, sizeof(int), 1, arquivo) != 1)
        return 0;

    if (fread(&dados->idPoPsConectado, sizeof(int), 1, arquivo) != 1)
        return 0;

    if (fread(&dados->velocidade, sizeof(int), 1, arquivo) != 1)
        return 0;

    if (fread(&dados->unidadeMedida, sizeof(char), 1, arquivo) != 1)
        return 0;

    return 1;
}

int filtrar_dados(struct chave_valor *p,
            int quantidade_pares,
            struct registro_dados dados,
            int filtro) {
    //Devemos passar campo a campo verificando
    //campo a campo fazendo  o cara-crachá da função
    for (int j = 0; j < quantidade_pares; j++) {

        if (strcmp(p[j].chave, "idPoPs") == 0) {

            int valor = (int) strtol(p[j].valor, NULL, 10);

            if (dados.idPoPs != valor) {
                filtro = 0;
                break;
            }
        }
        else if (strcmp(p[j].chave, "idPoPsConectado") == 0) {

            int valor = (int) strtol(p[j].valor, NULL, 10);

            if (dados.idPoPsConectado != valor) {
                filtro = 0;
                break;
            }
        }
        else if (strcmp(p[j].chave, "velocidade") == 0) {

            if (p[j].valor[0] == '\0') {

                if (dados.velocidade != -1) {
                    filtro = 0;
                    break;
                }
            }
            else {

                int valor = (int) strtol(p[j].valor, NULL, 10);

                if (dados.velocidade != valor) {
                    filtro = 0;
                    break;
                }
            }
        }

        else if (strcmp(p[j].chave, "unidadeMedida") == 0) {

            if (p[j].valor[0] == '\0') {

                if (dados.unidadeMedida != '$') {
                    filtro = 0;
                    break;
                }
            }

            else {

                if (dados.unidadeMedida != p[j].valor[0]) {
                    filtro = 0;
                    break;
                }
            }
        }
    }
    return filtro;
}

int leitura_cabecalho(FILE *f, registro_cabecalho *cabecalho) {
    if (fread(&cabecalho->status, sizeof(char), 1, f) != 1)
        return 0;

    if (fread(&cabecalho->topo_pilha, sizeof(int), 1, f) != 1)
        return 0;

    if (fread(&cabecalho->proxRRN, sizeof(int), 1, f) != 1)
        return 0;

    if (fread(&cabecalho->nroRegRem, sizeof(int), 1, f) != 1)
        return 0;

    if (fread(&cabecalho->nroPares, sizeof(int), 1, f) != 1)
        return 0;

    return 1;
}

int atende_criterios(const registro_dados *dado, const struct chave_valor *criterios, int quantidade){
    int tester = 1;
    for(int j = 0; j < quantidade; j++){
        if (strcmp(criterios[j].chave, "idPoPs") == 0) {
            long int temp = strtol(criterios[j].valor, NULL, 10);
            if (dado->idPoPs != temp)
                tester = 0;
        }

        else if (strcmp(criterios[j].chave, "idPoPsConectado") == 0) {
            long int temp = strtol(criterios[j].valor, NULL, 10);
            if (dado->idPoPsConectado != temp)
                tester = 0;
        }

        else if(strcmp(criterios[j].chave, "velocidade") == 0) {
            long int temp;
            if (criterios[j].valor[0] == '\0')
                temp = -1; // nulo
            else
                temp = strtol(criterios[j].valor, NULL, 10);
            if (dado->velocidade != temp)
                tester = 0;
        }
        else if(strcmp(criterios[j].chave, "unidadeMedida") == 0) {
            char temp;
            if(criterios[j].valor[0] == '\0')
                temp = '$'; // nulo
            else
                temp = criterios[j].valor[0];
            if(dado->unidadeMedida != temp)
                tester = 0;
        }
        else{
            tester = 0; // nome de campo desconhecido
        }
        if (tester == 0)
            break; // algum criterio deu pau, nn precisa verificar os outros
    }
    return tester;
}
