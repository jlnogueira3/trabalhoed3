//
// Created by joao-lucas-nogueira on 09/09/2026.
//

#ifndef TRABALHOED3_IMPLEMENTADAS_H
#define TRABALHOED3_IMPLEMENTADAS_H

struct chave_valor {
    char chave[50];
    char valor[50];
};

void funcionalidade1(
    const char *csv,
    char *binario,
    struct registro_cabecalho *cabecalho);

void funcionalidade2(const char *binario,
    struct registro_cabecalho *cabecalho);

void funcionalidade3(const char *binario,
    struct registro_cabecalho *cabecalho,
    char *n);



#endif //TRABALHOED3_IMPLEMENTADAS_H
