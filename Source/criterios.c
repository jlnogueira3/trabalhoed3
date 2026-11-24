// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/criterios.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Headers/fornecidas.h"
#include "../Headers/registros.h"

// Lê 'quantidade' pares campo/valor da entrada padrão.
// Retorna um vetor alocado com calloc (quem chama deve dar free),
// ou NULL se a quantidade for inválida ou se a alocação falhar.
// Valores NULO são guardados como string vazia.
struct chave_valor *ler_pares(int quantidade) {
    if (quantidade <= 0)
        return NULL;

    struct chave_valor *pares = calloc(quantidade, sizeof(struct chave_valor));
    if (pares == NULL)
        return NULL;

    for (int j = 0; j < quantidade; j++) {
        // Cada iteração lê uma chave e o valor correspondente
        scanf("%49s", pares[j].chave);

        // unidadeMedida é o único campo do tipo string: vem entre aspas
        if (strcmp(pares[j].chave, "unidadeMedida") == 0) {
            ScanQuoteString(pares[j].valor);
        }
        // Os demais campos são inteiros
        else {
            scanf("%49s", pares[j].valor);
            if (strcmp(pares[j].valor, "NULO") == 0)
                pares[j].valor[0] = '\0';
        }
    }
    return pares;
}

// Converte o valor de um critério inteiro. Valor vazio representa NULO.
static long valor_criterio_inteiro(const char *valor) {
    if (valor[0] == '\0')
        return VALOR_NULO_INTEIRO;
    return strtol(valor, NULL, 10);
}

// Verifica se o registro atende a todos os critérios.
// Retorna 1 se atende a todos, 0 caso contrário.
// Campos desconhecidos nunca batem com o registro.
int atende_criterios(const registro_dados *dado, const struct chave_valor *criterios,
                     int quantidade) {
    for (int j = 0; j < quantidade; j++) {
        const char *chave = criterios[j].chave;
        const char *valor = criterios[j].valor;
        int atende;

        if (strcmp(chave, "idPoPs") == 0)
            atende = dado->idPoPs == strtol(valor, NULL, 10);
        else if (strcmp(chave, "idPoPsConectado") == 0)
            atende = dado->idPoPsConectado == valor_criterio_inteiro(valor);
        else if (strcmp(chave, "velocidade") == 0)
            atende = dado->velocidade == valor_criterio_inteiro(valor);
        else if (strcmp(chave, "unidadeMedida") == 0)
            atende = dado->unidadeMedida == (valor[0] == '\0' ? VALOR_NULO_CHAR : valor[0]);
        else
            atende = 0;

        // Se um critério falhou, não é preciso verificar os demais
        if (!atende)
            return 0;
    }
    return 1;
}
