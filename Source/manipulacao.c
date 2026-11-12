// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/manipulacao.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Headers/fornecidas.h"
#include "../Headers/registros.h"
#include "../Headers/cabecalho.h"
#include "../Headers/registro.h"
#include "../Headers/criterios.h"
#include "../Headers/erros.h"

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
    marcar_status_cabecalho(f, cabecalho.status);

        // Cada iteração executa uma remoção, com seus próprios critérios.
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
            char removido = REGISTRO_REMOVIDO;
            fwrite(&removido, sizeof(char), 1, f);
            fwrite(&cabecalho.topo_pilha, sizeof(int), 1, f);

            // Preenche o restante do registro com lixo ('$') em uma única gravação.
            // São TAM_REGISTRO - TAM_CONTROLE_REMOCAO bytes, pois removido (1) e
            // encadeamentoPilha (4) já foram gravados.
            char lixo[TAM_REGISTRO];
            memset(lixo, VALOR_NULO_CHAR, sizeof(lixo));
            fwrite(lixo, sizeof(char), TAM_REGISTRO - TAM_CONTROLE_REMOCAO, f);

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

// Insere novos registros no arquivo binário, reutilizando posições de registros removidos
// quando possível: a reutilização segue a pilha de removidos, sempre pelo topo.
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
    marcar_status_cabecalho(f, cabecalho.status);

        // Cada iteração insere um registro, reaproveitando o topo da pilha quando ela não está
        // vazia.
    for (int i = 0; i < n; i++) {
        // Valores padrão: campos nulos são guardados como -1 (inteiros) ou '$' (unidadeMedida)
        registro_dados dado;
        dado.removido = REGISTRO_ATIVO;
        dado.encadeamentoPilha = -1;
        dado.velocidade = VALOR_NULO_INTEIRO;
        dado.unidadeMedida = VALOR_NULO_CHAR;

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
            // O byte 'removido' (1 byte) é pulado, e só esse campo é lido.
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

        // Cada inserção acrescenta um par (idPoPs, idPoPsConectado) novo ao arquivo
        cabecalho.nroPares++;
    }

    // Grava o cabeçalho atualizado e marca o arquivo como consistente
    cabecalho.status = STATUS_CONSISTENTE;
    salvar_cabecalho(f, &cabecalho);

    fclose(f);
    BinarioNaTela((char *)binario);
}

// Converte o valor de uma atualização para inteiro. Valor vazio representa NULO.
static int converter_campo_inteiro(const char *valor) {
    if (valor[0] == '\0')
        return VALOR_NULO_INTEIRO;
    return (int)strtol(valor, NULL, 10);
}

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
    marcar_status_cabecalho(f, cabecalho.status);

        // Cada iteração executa uma atualização, com seus próprios critérios e campos.
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
                const char *chave = atualizacoes[j].chave;
                const char *valor = atualizacoes[j].valor;

                if (strcmp(chave, "idPoPs") == 0)
                    dado.idPoPs = converter_campo_inteiro(valor);
                else if (strcmp(chave, "idPoPsConectado") == 0)
                    dado.idPoPsConectado = converter_campo_inteiro(valor);
                else if (strcmp(chave, "velocidade") == 0)
                    dado.velocidade = converter_campo_inteiro(valor);
                else if (strcmp(chave, "unidadeMedida") == 0)
                    dado.unidadeMedida = valor[0] == '\0' ? VALOR_NULO_CHAR : valor[0];
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
