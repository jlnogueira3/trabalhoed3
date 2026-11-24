// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/pops_registro.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "../Headers/registros.h"
#include "../Headers/pops.h"
#include "../Headers/pops_arquivo.h"

// Essa leitura tambem e usada ao percorrer a lista de removidos.
int ler_controle_pops(FILE *f, int64_t pos, int64_t fim, dados_pops *dado) {
    if (pos < CABECALHO_POPS || pos > fim || fim - pos < REGISTRO_POPS_MINIMO ||
       !posicionar_pops(f, pos))
        return 0;
    if (fread(&dado->removido, 1, 1, f) != 1 ||
       fread(&dado->tamanhoRegistro, 4, 1, f) != 1 ||
       fread(&dado->encadeamentoLista, 8, 1, f) != 1)
        return 0;
    return (dado->removido == REGISTRO_ATIVO || dado->removido == REGISTRO_REMOVIDO) &&
           dado->tamanhoRegistro >= REGISTRO_POPS_MINIMO && dado->tamanhoRegistro <= fim - pos;
}

// O limite impede que uma string sem delimitador avance para o registro seguinte.
static char *ler_nome_pops(FILE *f, int32_t *restante) {
    size_t tamanho = 0, capacidade = BLOCO_POPS;
    char *nome = malloc(capacidade);
    if (nome == NULL)
        return NULL;
    while (*restante > 0) {
        int c = fgetc(f);
        (*restante)--;
        if (c == EOF)
            break;
        if (c == '|') {
            nome[tamanho] = '\0';
            return nome;
        }
        if (tamanho + 1 == capacidade) {
            if (capacidade > SIZE_MAX - BLOCO_POPS)
                break;
            char *novo = realloc(nome, capacidade + BLOCO_POPS);
            if (novo == NULL)
                break;
            nome = novo;
            capacidade += BLOCO_POPS;
        }
        nome[tamanho++] = (char)c;
    }
    free(nome);
    return NULL;
}

// Libera os nomes alocados em dado e zera os ponteiros, evitando liberação dupla.
void destroi_dados_pops(dados_pops *dado) {
    free(dado->nomePoPs);
    free(dado->nomePais);
    dado->nomePoPs = NULL;
    dado->nomePais = NULL;
}

// Lê o registro de PoPs que começa em pos. Retorna 1 se o registro foi lido ou pulado (removido),
// e 0 em caso de erro. Os nomes são alocados e devem ser liberados com destroi_dados_pops.
int leitura_dados_pops(FILE *f, int64_t pos, int64_t fim, dados_pops *dado) {
    // Somente os ponteiros sao inicializados, para permitir a liberacao em caso de erro.
    dado->nomePoPs = NULL;
    dado->nomePais = NULL;
    if (!ler_controle_pops(f, pos, fim, dado))
        return 0;
    if (dado->removido == REGISTRO_REMOVIDO)
        return posicionar_pops(f, pos + dado->tamanhoRegistro); // pula o lixo sem ler os dados

    if (fread(&dado->idPoPs, 4, 1, f) != 1 || fread(dado->siglaPais, 1, 2, f) != 2)
        return 0;
    dado->siglaPais[2] = '\0';
    int32_t restante = dado->tamanhoRegistro - TAM_FIXO_ATE_NOMES_POPS;
    dado->nomePoPs = ler_nome_pops(f, &restante);
    if (dado->nomePoPs != NULL)
        dado->nomePais = ler_nome_pops(f, &restante);
    if (dado->nomePoPs == NULL || dado->nomePais == NULL ||
       !posicionar_pops(f, pos + dado->tamanhoRegistro)) {
        destroi_dados_pops(dado);
        return 0;
    }
    return 1;
}

// Calcula o tamanho físico do registro (controle, id, sigla e nomes com delimitadores).
// Retorna 0 se o tamanho não couber em int32_t.
int tamanho_dados_pops(const dados_pops *dado, int32_t *tamanho) {
    size_t nome = strlen(dado->nomePoPs), pais = strlen(dado->nomePais);
    if (nome > INT32_MAX - REGISTRO_POPS_MINIMO ||
       pais > INT32_MAX - REGISTRO_POPS_MINIMO - nome)
        return 0;
    *tamanho = (int32_t)(REGISTRO_POPS_MINIMO + nome + pais);
    return 1;
}

// Grava 'quantidade' bytes de lixo ('$') na posição atual do arquivo.
int escrever_lixo_pops(FILE *f, int32_t quantidade) {
    char lixo = '$';
    for (int32_t i = 0; i < quantidade; i++) {
        if (fwrite(&lixo, 1, 1, f) != 1)
            return 0;
    }
    return 1;
}

// Grava o registro na posição pos, campo a campo, e preenche a sobra do espaço físico com lixo.
// Retorna 0 em caso de erro.
int escrever_dados_pops(FILE *f, int64_t pos, const dados_pops *dado) {
    int32_t usado;
    if (!tamanho_dados_pops(dado, &usado) || usado > dado->tamanhoRegistro ||
        !posicionar_pops(f, pos))
        return 0;
    char delimitador = '|';
    size_t nome = strlen(dado->nomePoPs), pais = strlen(dado->nomePais);
    if (fwrite(&dado->removido, 1, 1, f) != 1 ||
       fwrite(&dado->tamanhoRegistro, 4, 1, f) != 1 ||
       fwrite(&dado->encadeamentoLista, 8, 1, f) != 1 ||
       fwrite(&dado->idPoPs, 4, 1, f) != 1 || fwrite(dado->siglaPais, 1, 2, f) != 2 ||
       fwrite(dado->nomePoPs, 1, nome, f) != nome || fwrite(&delimitador, 1, 1, f) != 1 ||
       fwrite(dado->nomePais, 1, pais, f) != pais || fwrite(&delimitador, 1, 1, f) != 1)
        return 0;
    // Se reutilizamos um espaco maior, o tamanho fisico fica igual e a sobra recebe '$'.
    return escrever_lixo_pops(f, dado->tamanhoRegistro - usado);
}

// Imprime um registro de PoPs. Valores vazios ou nulos aparecem como NULO.
void print_dados_pops(const dados_pops *dado) {
    printf("%d %s %s %s\n", (int)dado->idPoPs,
           dado->nomePoPs[0] == '\0' ? "NULO" : dado->nomePoPs,
           dado->nomePais[0] == '\0' ? "NULO" : dado->nomePais,
           strcmp(dado->siglaPais, "$$") == 0 ? "NULO" : dado->siglaPais);
}

// Verifica se o registro atende a todos os critérios. Retorna 1 se atende, 0 caso contrário.
// Campos desconhecidos nunca batem com o registro.
int atende_criterios_pops(const dados_pops *dado, const criterio_pops *criterios,
                                 int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        const char *chave = criterios[i].chave, *valor = criterios[i].valor;
        if (strcmp(chave, "idPoPs") == 0) {
            if (dado->idPoPs != strtol(valor, NULL, 10))
                return 0;
        }
        else if (strcmp(chave, "nomePoPs") == 0) {
            if (strcmp(dado->nomePoPs, valor) != 0)
                return 0;
        }
        else if (strcmp(chave, "nomePais") == 0) {
            if (strcmp(dado->nomePais, valor) != 0)
                return 0;
        }
        else if (strcmp(chave, "siglaPais") == 0) {
            if (strcmp(dado->siglaPais, valor[0] == '\0' ? "$$" : valor) != 0)
                return 0;
        }
        else
            return 0;
    }
    return 1;
}
