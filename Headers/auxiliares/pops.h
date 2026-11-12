/*
 * João L A Nogueira - N usp: 16986970
 *
 * Funções do arquivo de dados PoPs (trabalho 2): cabeçalho de 25 bytes e
 * registros de tamanho variável com delimitador '|'.
 */

#ifndef TRABALHOED3_POPS_H
#define TRABALHOED3_POPS_H

#include <stdio.h>
#include <stdint.h>

// Tamanho fixo do cabeçalho do arquivo PoPs (em bytes)
#define TAM_CABECALHO_POPS 25

// Tamanho fixo de cada campo de tamanho fixo do registro PoPs (em bytes)
#define TAM_SIGLA_PAIS 2

// Delimitador dos campos de tamanho variável
#define DELIMITADOR_POPS '|'

// Caractere usado para preencher campos nulos do tipo string (e o lixo)
#define LIXO_POPS '$'

// Cabeçalho do arquivo PoPs. Os campos são gravados um a um, na ordem da especificação.
typedef struct cabecalho_pops {
    char status;               // '0' em escrita, '1' consistente
    int64_t topoLista;         // byte offset do registro removido no topo da pilha, ou -1
    int64_t proxByteOffset;    // próximo byte offset disponível para novo registro
    int nroRegRem;             // quantidade de registros logicamente removidos
    int nroPoPs;               // quantidade de PoPs diferentes armazenados
} cabecalho_pops;

void inicializar_cabecalho_pops(cabecalho_pops *cabecalho);
int leitura_cabecalho_pops(FILE *f, cabecalho_pops *cabecalho);
void escrever_cabecalho_pops(const cabecalho_pops *cabecalho, FILE *arquivo);
void salvar_cabecalho_pops(FILE *f, const cabecalho_pops *cabecalho);

// Retorna o tamanho total do registro, em bytes (valor do campo tamanhoRegistro)
int tamanho_registro_pops(const char *nomePoPs, const char *nomePais);
void escrever_registro_pops(FILE *f, int idPoPs, const char *siglaPais,
                            const char *nomePoPs, const char *nomePais);

#endif //TRABALHOED3_POPS_H
