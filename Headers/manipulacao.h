// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

// Funcionalidades 5, 6 e 7 (trabalho 1): remoção, inserção e atualização com pilha de removidos.

#ifndef TRABALHOED3_MANIPULACAO_H
#define TRABALHOED3_MANIPULACAO_H

// Realiza a remoção lógica dos registros que atendem aos critérios informados,
// atualizando a pilha de removidos e o cabeçalho.
// São realizadas n remoções em sequência.
void funcionalidade5(const char *binario, int n);

// Insere novos registros no arquivo binário, reutilizando posições de registros removidos
// quando possível: a reutilização segue a pilha de removidos, sempre pelo topo.
// São realizadas n inserções em sequência.
void funcionalidade6(const char *binario, int n);

// Atualiza os registros que atendem aos critérios de busca, alterando os campos informados.
// São realizadas n atualizações em sequência.
void funcionalidade7(const char *binario, int n);

#endif // TRABALHOED3_MANIPULACAO_H
