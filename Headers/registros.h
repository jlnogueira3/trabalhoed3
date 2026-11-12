/*
 * João L A Nogueira - N usp: 16986970
 */
//
// Created by joao-lucas-nogueira on 08/09/2026.
//
#ifndef TRABALHOED3_REGISTROS_H
#define TRABALHOED3_REGISTROS_H

// Tamanhos fixos do arquivo binário de dados (em bytes)
#define TAM_CABECALHO 17   // status(1) + topo_pilha(4) + proxRRN(4) + nroRegRem(4) + nroPares(4)
#define TAM_REGISTRO 18    // removido(1) + encadeamentoPilha(4) + idPoPs(4) + idPoPsConectado(4) + velocidade(4) + unidadeMedida(1)

// Valores possíveis do campo status do cabeçalho
#define STATUS_CONSISTENTE '1'
#define STATUS_INCONSISTENTE '0'

typedef struct registro_cabecalho {
    char status;
    int topo_pilha;
    int proxRRN;
    //sempre que for utilizar, inicializar proxRRN com valor 0
    int nroRegRem;
    //sempre que for utilizar, inicializar nroRegRem com valor 0
    int nroPares;
} registro_cabecalho;
//Sempre que for inicializar: .proxRRN = 0; .nroRegRem = 0; .encadeamentoPilha = -1;

typedef struct chave_valor {
    char chave[50];
    char valor[50];
} chave_valor;


typedef struct registro_dados {
    int idPoPs;
    int idPoPsConectado;
    // idPoPs e idPoPsConectado não podem assumir valores nulos
    int velocidade;
    char unidadeMedida;
    char removido;
    int encadeamentoPilha;
    //encadeamentoPilha deve ser inicializado com valor -1
} registro_dados;
//Sempre que for inicializar: .encadeamentoPilha = -1;

#endif //TRABALHOED3_REGISTROS_H
