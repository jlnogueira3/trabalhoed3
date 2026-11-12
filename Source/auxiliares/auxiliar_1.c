/*
 * João L A Nogueira - N usp: 16986970
 */
//
// Created by joao-lucas-nogueira on 11/09/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

// Funções auxiliares compartilhadas pelas funcionalidades.
// Sempre que uma operação de disco se repetir em mais de uma funcionalidade,
// ela deve ficar aqui, e não copiada nos arquivos de funcionalidade.


// ---------------------------------------------------------------------------
// Cabeçalho
// ---------------------------------------------------------------------------

// Inicializa o cabeçalho com os valores da especificação para um arquivo novo.
// Só deve ser usada na criação do arquivo (funcionalidade 1).
void inicializar_cabecalho(registro_cabecalho *cabecalho) {
    cabecalho->status = STATUS_INCONSISTENTE; // arquivo em construção
    cabecalho->topo_pilha = -1;
    cabecalho->proxRRN = 0;
    cabecalho->nroRegRem = 0;
    cabecalho->nroPares = 0;
}

// Lê o cabeçalho do arquivo.
// Retorna 1 se o cabeçalho foi lido e o arquivo está consistente,
// e 0 em caso de erro de leitura ou de arquivo inconsistente.
int leitura_cabecalho(FILE *f, registro_cabecalho *cabecalho) {
    // O cabeçalho começa sempre no byte 0
    fseek(f, 0, SEEK_SET);

    // Primeiro lemos apenas o status. Se o arquivo estiver inconsistente,
    // os demais campos do cabeçalho não devem ser lidos.
    if (fread(&cabecalho->status, sizeof(char), 1, f) != 1)
        return 0;
    if (cabecalho->status != STATUS_CONSISTENTE)
        return 0;

    // Arquivo consistente: lemos o restante do cabeçalho
    if (fread(&cabecalho->topo_pilha, sizeof(int), 1, f) != 1)
        return 0;
    if (fread(&cabecalho->proxRRN, sizeof(int), 1, f) != 1)
        return 0;
    if (fread(&cabecalho->nroRegRem, sizeof(int), 1, f) != 1)
        return 0;
    if (fread(&cabecalho->nroPares, sizeof(int), 1, f) != 1)
        return 0;

    return 1;
}

// Escreve os campos do cabeçalho na posição atual do arquivo.
// Usada diretamente quando o arquivo já está posicionado no byte 0.
void escrever_cabecalho(const registro_cabecalho *cabecalho, FILE *arquivo) {
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->topo_pilha, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->proxRRN, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroPares, sizeof(int), 1, arquivo);
}

// Regrava o cabeçalho inteiro no byte 0 do arquivo e força a gravação no disco.
void salvar_cabecalho(FILE *f, const registro_cabecalho *cabecalho) {
    fseek(f, 0, SEEK_SET);
    escrever_cabecalho(cabecalho, f);
    fflush(f);
}


// ---------------------------------------------------------------------------
// Registros de dados
// ---------------------------------------------------------------------------

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
    if (dados->removido == '1') {
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
    if (dados.idPoPs == -1)
        printf("NULO ");
    else
        printf("%d ", dados.idPoPs);

    if (dados.idPoPsConectado == -1)
        printf("NULO ");
    else
        printf("%d ", dados.idPoPsConectado);

    if (dados.velocidade == -1)
        printf("NULO ");
    else
        printf("%d ", dados.velocidade);

    // Cada registro termina com quebra de linha
    if (dados.unidadeMedida == '$')
        printf("NULO\n");
    else
        printf("\"%c\"\n", dados.unidadeMedida);
}


// ---------------------------------------------------------------------------
// Critérios de busca e atualização
// ---------------------------------------------------------------------------

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

// Compara o registro com todos os pares de 'p'.
// Retorna 1 se todos os pares batem com o registro, 0 caso contrário.
int filtrar_dados(struct chave_valor *p, int quantidade_pares,
                  registro_dados dados, int filtro) {
    for (int j = 0; j < quantidade_pares; j++) {

        if (strcmp(p[j].chave, "idPoPs") == 0) {
            int valor = (int)strtol(p[j].valor, NULL, 10);
            if (dados.idPoPs != valor) {
                filtro = 0;
                break;
            }
        }
        else if (strcmp(p[j].chave, "idPoPsConectado") == 0) {
            int valor = (int)strtol(p[j].valor, NULL, 10);
            if (dados.idPoPsConectado != valor) {
                filtro = 0;
                break;
            }
        }
        else if (strcmp(p[j].chave, "velocidade") == 0) {
            // valor vazio representa NULO, guardado como -1 no registro
            if (p[j].valor[0] == '\0') {
                if (dados.velocidade != -1) {
                    filtro = 0;
                    break;
                }
            }
            else {
                int valor = (int)strtol(p[j].valor, NULL, 10);
                if (dados.velocidade != valor) {
                    filtro = 0;
                    break;
                }
            }
        }
        else if (strcmp(p[j].chave, "unidadeMedida") == 0) {
            // valor vazio representa NULO, guardado como '$' no registro
            if (p[j].valor[0] == '\0') {
                if (dados.unidadeMedida != '$') {
                    filtro = 0;
                    break;
                }
            }
            else {
                if (dados.unidadeMedida != p[j].valor[0]) {
                    filtro = 0;
                    break;
                }
            }
        }
    }
    return filtro;
}

// Verifica se o registro atende a todos os critérios.
// Retorna 1 se atende a todos, 0 caso contrário.
int atende_criterios(const registro_dados *dado, const struct chave_valor *criterios, int quantidade) {
    int tester = 1;
    for (int j = 0; j < quantidade; j++) {
        if (strcmp(criterios[j].chave, "idPoPs") == 0) {
            long int temp = strtol(criterios[j].valor, NULL, 10);
            if (dado->idPoPs != temp)
                tester = 0;
        }
        else if (strcmp(criterios[j].chave, "idPoPsConectado") == 0) {
            long int temp = strtol(criterios[j].valor, NULL, 10);
            if (dado->idPoPsConectado != temp)
                tester = 0;
        }
        else if (strcmp(criterios[j].chave, "velocidade") == 0) {
            long int temp;
            if (criterios[j].valor[0] == '\0')
                temp = -1; // nulo
            else
                temp = strtol(criterios[j].valor, NULL, 10);
            if (dado->velocidade != temp)
                tester = 0;
        }
        else if (strcmp(criterios[j].chave, "unidadeMedida") == 0) {
            char temp;
            if (criterios[j].valor[0] == '\0')
                temp = '$'; // nulo
            else
                temp = criterios[j].valor[0];
            if (dado->unidadeMedida != temp)
                tester = 0;
        }
        else {
            tester = 0; // nome de campo desconhecido
        }

        // Se um critério falhou, não é preciso verificar os demais
        if (tester == 0)
            break;
    }
    return tester;
}


// ---------------------------------------------------------------------------
// Erros
// ---------------------------------------------------------------------------

// Exibe a mensagem padrão de falha e fecha o arquivo (se estiver aberto).
void falha_processamento_arquivo(FILE *f) {
    printf("Falha no processamento do arquivo.\n");
    if (f != NULL)
        fclose(f);
}

// Retorna 1 se o arquivo foi aberto com sucesso.
// Caso contrário, exibe a mensagem padrão de falha e retorna 0.
int verificar_abertura_arquivo(FILE *f) {
    if (f == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    return 1;
}
