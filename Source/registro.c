// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/registro.h"

#include <stdio.h>

#include "../Headers/registros.h"

// Calcula o byte offset do registro de RRN informado.
long posicao_registro(int rrn) {
    return (long)TAM_CABECALHO + (long)rrn * TAM_REGISTRO;
}

// Lê um registro na posição atual do arquivo.
// Retorna:
//   LEITURA_OK       se o registro foi lido por completo;
//   LEITURA_REMOVIDO se o registro está removido (seu conteúdo não é lido,
//                    apenas os bytes restantes são pulados);
//   LEITURA_ERRO     em caso de erro de leitura.
int leitura_dados(registro_dados *dados, FILE *arquivo) {
    // O primeiro byte indica se o registro foi removido
    if (fread(&dados->removido, sizeof(char), 1, arquivo) != 1)
        return LEITURA_ERRO;

    // Registro removido: pula os bytes restantes sem ler seu conteúdo
    if (dados->removido == REGISTRO_REMOVIDO) {
        if (fseek(arquivo, TAM_REGISTRO - 1, SEEK_CUR) != 0)
            return LEITURA_ERRO;
        return LEITURA_REMOVIDO;
    }

    // Registro válido: lê os demais campos na ordem do registro
    if (fread(&dados->encadeamentoPilha, sizeof(int), 1, arquivo) != 1)
        return LEITURA_ERRO;
    if (fread(&dados->idPoPs, sizeof(int), 1, arquivo) != 1)
        return LEITURA_ERRO;
    if (fread(&dados->idPoPsConectado, sizeof(int), 1, arquivo) != 1)
        return LEITURA_ERRO;
    if (fread(&dados->velocidade, sizeof(int), 1, arquivo) != 1)
        return LEITURA_ERRO;
    if (fread(&dados->unidadeMedida, sizeof(char), 1, arquivo) != 1)
        return LEITURA_ERRO;

    return LEITURA_OK;
}

// Escreve um registro completo na posição atual do arquivo.
void escrever_dados(registro_dados dados, FILE *arquivo) {
    fwrite(&dados.removido, sizeof(char), 1, arquivo);
    fwrite(&dados.encadeamentoPilha, sizeof(int), 1, arquivo);
    fwrite(&dados.idPoPs, sizeof(int), 1, arquivo);
    fwrite(&dados.idPoPsConectado, sizeof(int), 1, arquivo);
    fwrite(&dados.velocidade, sizeof(int), 1, arquivo);
    fwrite(&dados.unidadeMedida, sizeof(char), 1, arquivo);
}

// Imprime um registro na saída padrão, substituindo os valores nulos por NULO.
void print_dados(registro_dados dados) {
    if (dados.idPoPs == VALOR_NULO_INTEIRO)
        printf("NULO ");
    else
        printf("%d ", dados.idPoPs);

    if (dados.idPoPsConectado == VALOR_NULO_INTEIRO)
        printf("NULO ");
    else
        printf("%d ", dados.idPoPsConectado);

    if (dados.velocidade == VALOR_NULO_INTEIRO)
        printf("NULO ");
    else
        printf("%d ", dados.velocidade);

    // Cada registro termina com quebra de linha
    if (dados.unidadeMedida == VALOR_NULO_CHAR)
        printf("NULO\n");
    else
        printf("\"%c\"\n", dados.unidadeMedida);
}
