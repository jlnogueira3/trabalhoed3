/*
 * João L A Nogueira - N usp: 16986970
 * José Eduardo Constantino Nordemann - N usp: 16877738
 */
//
// Created by joao-lucas-nogueira on 08/09/2026.
//
#ifndef TRABALHOED3_REGISTROS_H
#define TRABALHOED3_REGISTROS_H

// Tamanhos fixos do arquivo binário de dados (em bytes)
#define TAM_CABECALHO 17   // status(1) + topo_pilha(4) + proxRRN(4) + nroRegRem(4) + nroPares(4)
// removido(1) + encadeamentoPilha(4) + idPoPs(4) + idPoPsConectado(4)
// + velocidade(4) + unidadeMedida(1)
#define TAM_REGISTRO 18

// Valores possíveis do campo status do cabeçalho
#define STATUS_CONSISTENTE '1'
#define STATUS_INCONSISTENTE '0'

// Valores possíveis do campo removido de um registro
#define REGISTRO_ATIVO '0'
#define REGISTRO_REMOVIDO '1'

// Valores nulos guardados no registro: inteiros usam -1 e unidadeMedida usa '$'
#define VALOR_NULO_INTEIRO -1
#define VALOR_NULO_CHAR '$'

// Bytes de controle de um registro removido, fora o lixo: removido (1) e encadeamentoPilha (4)
#define TAM_CONTROLE_REMOCAO 5

typedef struct registro_cabecalho {
    char status;
    int topo_pilha;
    int proxRRN;
    // sempre que for utilizar, inicializar proxRRN com valor 0
    int nroRegRem;
    // sempre que for utilizar, inicializar nroRegRem com valor 0
    int nroPares;
} registro_cabecalho;
// Sempre que for inicializar: .proxRRN = 0; .nroRegRem = 0; .encadeamentoPilha = -1;

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
    // encadeamentoPilha deve ser inicializado com valor -1
} registro_dados;
// Sempre que for inicializar: .encadeamentoPilha = -1;

#endif // TRABALHOED3_REGISTROS_H
