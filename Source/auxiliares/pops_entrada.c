//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include "../../Headers/auxiliares/auxiliar_2.h"

//Lemos as strings por partes pq nomePoPs e nomePais nao possuem tamanho fixo.
//O realloc aumenta sempre um bloco de 64 bytes, sem cortar o nome recebido.
static char *ler_valor_pops(void){
    int c;
    do{
        c = getchar();
    }while(c != EOF && isspace((unsigned char)c));
    if(c == EOF)
        return NULL;

    int aspas = c == '"';
    if(aspas)
        c = getchar();
    size_t tamanho = 0, capacidade = BLOCO_POPS;
    char *valor = malloc(capacidade);
    if(valor == NULL)
        return NULL;

    //Com aspas, os espacos fazem parte do campo. Sem aspas, terminam o campo.
    while(c != EOF && (aspas ? c != '"' : !isspace((unsigned char)c))){
        if(tamanho + 1 == capacidade){
            if(capacidade > SIZE_MAX - BLOCO_POPS){
                free(valor);
                return NULL;
            }
            char *novo = realloc(valor, capacidade + BLOCO_POPS);
            if(novo == NULL){
                free(valor);
                return NULL;
            }
            valor = novo;
            capacidade += BLOCO_POPS;
        }
        valor[tamanho++] = (char)c;
        c = getchar();
    }
    if(aspas && c != '"'){
        free(valor);
        return NULL;
    }
    valor[tamanho] = '\0';
    if(!aspas && strcmp(valor, "NULO") == 0)
        valor[0] = '\0';
    return valor;
}

//A conversao precisa consumir o valor inteiro, senao "12abc" seria aceito como 12.
static int converter_inteiro_pops(const char *valor, int32_t *numero){
    char *fim;
    errno = 0;
    long temp = strtol(valor, &fim, 10);
    if(valor[0] == '\0' || *fim != '\0' || errno == ERANGE ||
       temp < INT32_MIN || temp > INT32_MAX)
        return 0;
    *numero = (int32_t)temp;
    return 1;
}

int ler_quantidade_pops(int *quantidade){
    char *valor = ler_valor_pops();
    if(valor == NULL)
        return 0;
    int32_t numero;
    int ok = converter_inteiro_pops(valor, &numero);
    free(valor);
    if(!ok || numero < 0 || numero > INT_MAX)
        return 0;
    *quantidade = (int)numero;
    return 1;
}

//Confere o nome e o tipo antes de usar um campo como filtro ou atualizacao.
static int validar_campo_pops(const char *chave, const char *valor){
    if(strcmp(chave, "idPoPs") == 0){
        int32_t id;
        return converter_inteiro_pops(valor, &id) && id != -1;
    }
    if(strcmp(chave, "siglaPais") == 0)
        return valor[0] == '\0' || strlen(valor) == 2;
    if(strcmp(chave, "nomePoPs") == 0 || strcmp(chave, "nomePais") == 0)
        return strchr(valor, '|') == NULL;
    return 0;
}

void destroi_criterios_pops(criterio_pops *criterios, int quantidade){
    if(criterios == NULL)
        return;
    for(int i = 0; i < quantidade; i++){
        free(criterios[i].chave);
        free(criterios[i].valor);
    }
    free(criterios);
}

//Essa leitura e a mesma nas buscas, remocoes e atualizacoes.
int ler_criterios_pops(criterio_pops **criterios, int *quantidade){
    *criterios = NULL;
    if(!ler_quantidade_pops(quantidade) || *quantidade <= 0 ||
       (size_t)*quantidade > SIZE_MAX / sizeof(criterio_pops))
        return 0;
    *criterios = calloc((size_t)*quantidade, sizeof(criterio_pops));
    if(*criterios == NULL)
        return 0;

    //O calloc deixa os ponteiros vazios para podermos liberar mesmo se a leitura falhar.
    for(int i = 0; i < *quantidade; i++){
        (*criterios)[i].chave = ler_valor_pops();
        (*criterios)[i].valor = ler_valor_pops();
        if((*criterios)[i].chave == NULL || (*criterios)[i].valor == NULL ||
           !validar_campo_pops((*criterios)[i].chave, (*criterios)[i].valor)){
            destroi_criterios_pops(*criterios, *quantidade);
            *criterios = NULL;
            return 0;
        }
    }
    return 1;
}

//A sigla nula ocupa dois '$' no disco, sem gravar o terminador da string.
static void copiar_sigla_pops(char sigla[3], const char *valor){
    if(valor[0] == '\0'){
        sigla[0] = '$';
        sigla[1] = '$';
    }
    else{
        sigla[0] = valor[0];
        sigla[1] = valor[1];
    }
    sigla[2] = '\0';
}

int ler_insercao_pops(dados_pops *dado){
    char *id = ler_valor_pops();
    dado->nomePoPs = ler_valor_pops();
    dado->nomePais = ler_valor_pops();
    char *sigla = ler_valor_pops();
    int ok = id != NULL && dado->nomePoPs != NULL && dado->nomePais != NULL && sigla != NULL;
    if(ok){
        ok = validar_campo_pops("idPoPs", id) &&
             validar_campo_pops("nomePoPs", dado->nomePoPs) &&
             validar_campo_pops("nomePais", dado->nomePais) &&
             validar_campo_pops("siglaPais", sigla);
    }
    if(ok){
        converter_inteiro_pops(id, &dado->idPoPs);
        copiar_sigla_pops(dado->siglaPais, sigla);
        dado->removido = '0';
        dado->encadeamentoLista = -1;
        ok = tamanho_dados_pops(dado, &dado->tamanhoRegistro);
    }
    free(id);
    free(sigla);
    if(!ok)
        destroi_dados_pops(dado);
    return ok;
}

//Substituimos a string so depois de conseguir reservar a memoria da nova.
static int trocar_nome_pops(char **nome, const char *valor){
    char *novo = malloc(strlen(valor) + 1);
    if(novo == NULL)
        return 0;
    strcpy(novo, valor);
    free(*nome);
    *nome = novo;
    return 1;
}

int atualizar_campos_pops(dados_pops *dado, const criterio_pops *campos, int quantidade){
    for(int i = 0; i < quantidade; i++){
        if(strcmp(campos[i].chave, "idPoPs") == 0){
            if(!converter_inteiro_pops(campos[i].valor, &dado->idPoPs))
                return 0;
        }
        else if(strcmp(campos[i].chave, "nomePoPs") == 0){
            if(!trocar_nome_pops(&dado->nomePoPs, campos[i].valor))
                return 0;
        }
        else if(strcmp(campos[i].chave, "nomePais") == 0){
            if(!trocar_nome_pops(&dado->nomePais, campos[i].valor))
                return 0;
        }
        else if(strcmp(campos[i].chave, "siglaPais") == 0)
            copiar_sigla_pops(dado->siglaPais, campos[i].valor);
    }
    return 1;
}
