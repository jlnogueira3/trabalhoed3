//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#ifndef TRABALHOED3_POPS_H
#define TRABALHOED3_POPS_H

#include <stdint.h>
#include <stddef.h>

//Os tamanhos abaixo sao do arquivo, nao do sizeof das structs.
//Convencao adotada: tamanhoRegistro conta o registro inteiro, incluindo o controle.
#define CABECALHO_POPS 25
#define REGISTRO_POPS_MINIMO 21
#define BLOCO_POPS 64

typedef struct cabecalho_pops {
    char status;
    int64_t topoLista;
    int64_t proxByteOffset;
    int32_t nroRegRem;
    int32_t nroPoPs;
} cabecalho_pops;

typedef struct dados_pops {
    char removido;
    int32_t tamanhoRegistro;
    int64_t encadeamentoLista;
    int32_t idPoPs;
    char siglaPais[3];      //o '\0' existe so na memoria; no disco sao 2 bytes
    char *nomePoPs;
    char *nomePais;
} dados_pops;

typedef struct entrada_indice_pops {
    int32_t idPoPs;
    int64_t byteoffset;
} entrada_indice_pops;

typedef struct indice_pops {
    entrada_indice_pops *entradas;
    size_t quantidade;
    size_t capacidade;
} indice_pops;

typedef struct criterio_pops {
    char *chave;
    char *valor;
} criterio_pops;

#endif
