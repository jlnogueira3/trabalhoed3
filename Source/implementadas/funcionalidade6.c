/*
 * João L A Nogueira - N usp: 16986970
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

// Insere novos registros no arquivo binário, reutilizando posições de registros removidos
// quando possível (abordagem Worst Fit: sempre o topo da pilha).
// São realizadas n inserções em sequência.
void funcionalidade6(const char *binario, int n) {
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
        // Valores padrão: campos nulos são guardados como -1 (inteiros) ou '$' (unidadeMedida)
        registro_dados dado;
        dado.removido = '0';
        dado.encadeamentoPilha = -1;
        dado.velocidade = -1;
        dado.unidadeMedida = '$';

        // Lê os valores informados para o novo registro
        char velocidade[50];
        char unidadeMedida[50];

        scanf("%d", &dado.idPoPs);
        scanf("%d", &dado.idPoPsConectado);
        scanf("%49s", velocidade);
        ScanQuoteString(unidadeMedida);

        if (strcmp(velocidade, "NULO") != 0)
            dado.velocidade = (int)strtol(velocidade, NULL, 10);
        if (unidadeMedida[0] != '\0')
            dado.unidadeMedida = unidadeMedida[0];

        if (cabecalho.topo_pilha != -1) {
            // Há registro removido: reutiliza o que está no topo da pilha
            int rrn = cabecalho.topo_pilha;
            long int pos = posicao_registro(rrn);

            // Só o campo encadeamentoPilha do registro removido é necessário,
            // pois ele aponta para o próximo registro da pilha.
            // Pulamos o byte 'removido' (1 byte) e lemos apenas esse campo.
            int proximo;
            fseek(f, pos + sizeof(char), SEEK_SET);
            if (fread(&proximo, sizeof(int), 1, f) != 1) {
                falha_processamento_arquivo(f);
                return;
            }

            // O topo da pilha passa a ser o próximo registro removido
            cabecalho.topo_pilha = proximo;
            cabecalho.nroRegRem--;

            // Sobrescreve o registro removido com os novos dados
            fseek(f, pos, SEEK_SET);
            escrever_dados(dado, f);
        }
        else {
            // Pilha vazia: escreve no próximo RRN disponível, no fim do arquivo
            long int pos = posicao_registro(cabecalho.proxRRN);
            fseek(f, pos, SEEK_SET);
            escrever_dados(dado, f);
            cabecalho.proxRRN++;
        }
    }

    // Grava o cabeçalho atualizado e marca o arquivo como consistente
    cabecalho.status = STATUS_CONSISTENTE;
    salvar_cabecalho(f, &cabecalho);

    fclose(f);
    BinarioNaTela((char *)binario);
}
