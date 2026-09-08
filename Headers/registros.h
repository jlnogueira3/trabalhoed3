//
// Created by joao-lucas-nogueira on 08/09/2026.
//
#ifndef TRABALHOED3_REGISTROS_H
#define TRABALHOED3_REGISTROS_H
#include <stdio.h>
struct registro_cabecalho {
    char status;
    int topo_pilha;
    int proxRRN;
    //sempre que for utilizar, inicializar proxRRN com valor 0
    int nroRegRem;
    //sempre que for utilizar, inicializar nroRegRem com valor 0
    int nroPares;
    char removido;
    char encadeamentoPilha;
    //encadeamentoPilha deve ser inicializado com valor -1
};
//Sempre que for inicializar: .proxRRN = 0; .nroRegRem = 0; .encadeamentoPilha = -1;


struct registro_dados {
    int idPoPs;
    int idPoPsConectado;
    // idPoPs e idPoPsConectado não podem assumir valores nulos
    int velocidade;
    char unidadeMedida;
    char removido;
    char encadeamentoPilha;
    //encadeamentoPilha deve ser inicializado com valor -1
};
//Sempre que for inicializar: .encadeamentoPilha = -1;




#endif //TRABALHOED3_REGISTROS_H
