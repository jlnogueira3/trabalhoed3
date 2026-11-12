/*
 * João L A Nogueira - N usp: 16986970
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

// Atualiza os registros que atendem aos critérios de busca, alterando os campos informados.
// São realizadas n atualizações em sequência.
void funcionalidade7(const char *binario, int n) {
    // Leitura e escrita
    FILE *f = fopen(binario, "rb+");
    if (verificar_abertura_arquivo(f) == 0)
        return;

    // Lê o cabeçalho. Retorna 0 se o arquivo estiver inconsistente.
    registro_cabecalho cabecalho;
    if (leitura_cabecalho(f, &cabecalho) == 0) {
        falha_processamento_arquivo(f);
        return;
    }

    // Marca o arquivo como inconsistente enquanto ele é alterado
    cabecalho.status = STATUS_INCONSISTENTE;
    salvar_cabecalho(f, &cabecalho);

    for (int i = 0; i < n; i++) {
        // Critérios que definem quais registros serão atualizados
        int quantidade_criterios;
        scanf("%d", &quantidade_criterios);
        if (quantidade_criterios <= 0) {
            falha_processamento_arquivo(f);
            return;
        }

        struct chave_valor *criterios = ler_pares(quantidade_criterios);
        if (criterios == NULL) {
            falha_processamento_arquivo(f);
            return;
        }

        // Campos e novos valores que serão gravados nos registros encontrados
        int quantidade_atualizacoes;
        scanf("%d", &quantidade_atualizacoes);
        if (quantidade_atualizacoes <= 0) {
            free(criterios);
            falha_processamento_arquivo(f);
            return;
        }

        struct chave_valor *atualizacoes = ler_pares(quantidade_atualizacoes);
        if (atualizacoes == NULL) {
            free(criterios);
            falha_processamento_arquivo(f);
            return;
        }

        // Percorre todos os RRNs do arquivo procurando registros que atendem aos critérios
        for (int rrn = 0; rrn < cabecalho.proxRRN; rrn++) {
            long int pos = posicao_registro(rrn);
            fseek(f, pos, SEEK_SET);

            registro_dados dado;
            int resultado = leitura_dados(&dado, f);

            if (resultado == LEITURA_ERRO) {
                free(criterios);
                free(atualizacoes);
                falha_processamento_arquivo(f);
                return;
            }

            // Registros removidos não são atualizados
            if (resultado == LEITURA_REMOVIDO)
                continue;

            if (atende_criterios(&dado, criterios, quantidade_criterios) == 0)
                continue;

            // Aplica cada atualização ao registro. Valor vazio representa NULO.
            for (int j = 0; j < quantidade_atualizacoes; j++) {
                if (strcmp(atualizacoes[j].chave, "idPoPs") == 0) {
                    if (atualizacoes[j].valor[0] == '\0')
                        dado.idPoPs = -1;
                    else
                        dado.idPoPs = (int)strtol(atualizacoes[j].valor, NULL, 10);
                }
                else if (strcmp(atualizacoes[j].chave, "idPoPsConectado") == 0) {
                    if (atualizacoes[j].valor[0] == '\0')
                        dado.idPoPsConectado = -1;
                    else
                        dado.idPoPsConectado = (int)strtol(atualizacoes[j].valor, NULL, 10);
                }
                else if (strcmp(atualizacoes[j].chave, "velocidade") == 0) {
                    if (atualizacoes[j].valor[0] == '\0')
                        dado.velocidade = -1;
                    else
                        dado.velocidade = (int)strtol(atualizacoes[j].valor, NULL, 10);
                }
                else if (strcmp(atualizacoes[j].chave, "unidadeMedida") == 0) {
                    if (atualizacoes[j].valor[0] == '\0')
                        dado.unidadeMedida = '$';
                    else
                        dado.unidadeMedida = atualizacoes[j].valor[0];
                }
            }

            // Volta ao início do registro e grava os dados atualizados
            fseek(f, pos, SEEK_SET);
            escrever_dados(dado, f);
        }

        free(criterios);
        free(atualizacoes);
    }

    // Grava o cabeçalho e marca o arquivo como consistente
    cabecalho.status = STATUS_CONSISTENTE;
    salvar_cabecalho(f, &cabecalho);

    fclose(f);
    BinarioNaTela((char *)binario);
}
