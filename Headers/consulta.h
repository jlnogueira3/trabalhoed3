// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Funcionalidades 2, 3 e 4 (trabalho 1): somente leitura do arquivo.

#ifndef TRABALHOED3_CONSULTA_H
#define TRABALHOED3_CONSULTA_H

// Percorre o arquivo binário e exibe todos os registros válidos,
// desconsiderando os registros logicamente removidos.
void funcionalidade2(const char *binario);

// Realiza buscas sequenciais no arquivo binário usando um ou mais pares campo/valor como critérios.
// São realizadas n buscas em sequência.
void funcionalidade3(const char *binario, const char *n);

// Acessa diretamente um registro pelo seu RRN e o exibe caso ele exista e não esteja removido.
void funcionalidade4(const char *binario, int rrn);

#endif // TRABALHOED3_CONSULTA_H
