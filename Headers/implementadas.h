/*
 * João L A Nogueira - N usp: 16986970
 * José Eduardo Constantino Nordemann - N usp: 16877738
 */
#ifndef TRABALHOED3_IMPLEMENTADAS_H
#define TRABALHOED3_IMPLEMENTADAS_H

#include "registros.h"

// Cada funcionalidade declara o próprio cabeçalho quando precisa dele.
// O cabeçalho não é mais passado a partir do main.

void funcionalidade1(const char *csv, const char *binario);
void funcionalidade2(const char *binario);
void funcionalidade3(const char *binario, const char *n);
void funcionalidade4(const char *binario, int rrn);
void funcionalidade5(const char *binario, int n);
void funcionalidade6(const char *binario, int n);
void funcionalidade7(const char *binario, int n);

// Trabalho 2: arquivo de dados PoPs
void funcionalidade8(const char *csv, const char *binario);
void funcionalidade9(const char *binario_dados, const char *binario_indice);
void funcionalidade11(const char *binario, const char *nome_indice, int n);
void funcionalidade12(const char *binario, const char *nome_indice, int n);
void funcionalidade13(const char *binario, const char *nome_indice, int n);
void funcionalidade14(const char *binario, const char *nome_indice, int n);

#endif
