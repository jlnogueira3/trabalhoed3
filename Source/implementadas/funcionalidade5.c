/*
 * João L A Nogueira - N usp: 16986970
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

// Realiza a remoção lógica dos registros que atendem aos critérios informados,
// atualizando a pilha de removidos e o cabeçalho.
// São realizadas n remoções em sequência.
void funcionalidade5(const char *binario, int n) {
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
        // Quantidade de critérios desta remoção
        int quantidade;
        scanf("%d", &quantidade);
        if (quantidade <= 0) {
            falha_processamento_arquivo(f);
            return;
        }

        struct chave_valor *criterios = ler_pares(quantidade);
        if (criterios == NULL) {
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
                falha_processamento_arquivo(f);
                return;
            }

            // Registros já removidos não são alterados
            if (resultado == LEITURA_REMOVIDO)
                continue;

            if (atende_criterios(&dado, criterios, quantidade) == 0)
                continue;

            // Volta ao início do registro para gravar os campos de controle
            fseek(f, pos, SEEK_SET);

            // Marca o registro como removido e o encadeia no topo da pilha
            char removido = '1';
            fwrite(&removido, sizeof(char), 1, f);
            fwrite(&cabecalho.topo_pilha, sizeof(int), 1, f);

            // Preenche o restante do registro com lixo ('$').
            // São TAM_REGISTRO - 5 bytes, pois removido (1) e encadeamentoPilha (4) já foram gravados.
            char lixo = '$';
            for (int k = 0; k < TAM_REGISTRO - 5; k++)
                fwrite(&lixo, sizeof(char), 1, f);

            // Atualiza a pilha de removidos e o contador
            cabecalho.topo_pilha = rrn;
            cabecalho.nroRegRem++;
        }

        free(criterios);
    }

    // Grava o cabeçalho atualizado e marca o arquivo como consistente
    cabecalho.status = STATUS_CONSISTENTE;
    salvar_cabecalho(f, &cabecalho);

    fclose(f);
    BinarioNaTela((char *)binario);
}
