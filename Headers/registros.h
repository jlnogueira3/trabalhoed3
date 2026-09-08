//
// Created by joao-lucas-nogueira on 08/09/2026.
//
#include <stdio.h>
#ifndef TRABALHOED3_REGISTROS_H
#define TRABALHOED3_REGISTROS_H

struct registro_cabecalho {
    char status;
    int topo_pilha;
    int proxRRN;
    //sempre que for utilizar, inicializar proxRRN com valor 0
    int nroRegRem;
    //sempre que for utilizar, inicializar nroRegRem com valor 0
    int nroPares;
};

struct registro_dados {
    int idPoPs;
    int idPoPsConectado;
    // idPoPs e idPoPsConectado não podem assumir valores nulos
    int velocidade;
    char unidadeMedida;
};





#endif //TRABALHOED3_REGISTROS_H
