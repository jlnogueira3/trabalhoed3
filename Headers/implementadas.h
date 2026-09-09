//
// Created by joao-lucas-nogueira on 09/09/2026.
//

#ifndef TRABALHOED3_IMPLEMENTADAS_H
#define TRABALHOED3_IMPLEMENTADAS_H

#include "registros.h"

struct chave_valor {
    char chave[50];
    char valor[50];
};

void funcionalidade1(
    const char *csv,
    char *binario,
     registro_cabecalho *cabecalho);

void funcionalidade2(const char *binario,
     registro_cabecalho *cabecalho);

void funcionalidade3(const char *binario,
     registro_cabecalho *cabecalho,
    char *n);

void funcionalidade4(const char *binario, int rrn);

#endif //TRABALHOED3_IMPLEMENTADAS_H
