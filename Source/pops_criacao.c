// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/pops_criacao.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../Headers/fornecidas.h"
#include "../Headers/registros.h"
#include "../Headers/pops.h"
#include "../Headers/erros.h"
#include "../Headers/pops_arquivo.h"
#include "../Headers/pops_registro.h"

#define TAM_LINHA_CSV 512
#define MAX_COLUNAS_CSV 16

// siglaPais ocupa 2 bytes fixos no arquivo de dados
#define TAM_SIGLA_PAIS 2

// Separa uma linha do CSV em campos, trocando cada vírgula por '\0'.
// Campos vazios ou só com espaços viram strings vazias (valores nulos).
// Retorna a quantidade de campos encontrados.
static int separar_campos(char *linha, char *campos[], int max) {
    int n = 0;
    campos[n++] = linha;
    // Cada vírgula encerra um campo e o próximo começa logo depois dela
    for (char *p = linha; *p != '\0' && n < max; p++) {
        if (*p == ',') {
            *p = '\0';
            campos[n++] = p + 1;
        }
    }
    for (int i = 0; i < n; i++) {
        if (campos[i][strspn(campos[i], " \t")] == '\0')
            campos[i][0] = '\0';
    }
    return n;
}

// Fecha os arquivos e exibe a mensagem de falha.
static void encerrar_com_falha(FILE *leitura, FILE *escrita, indice_pops *indice) {
    free(indice->entradas);
    fclose(leitura);
    falha_processamento_arquivo(escrita);
}

// Lê o CSV de PoPs e grava o arquivo binário de dados.
// A primeira linha do CSV diz o que cada coluna representa, então as colunas
// são localizadas pelo nome, e não pela posição.
void funcionalidade8(const char *csv, const char *binario) {
    // O CSV é texto
    FILE *leitura = fopen(csv, "r");
    if (verificar_abertura_arquivo(leitura) == 0)
        return;

    // O arquivo de dados é criado do zero, só com escrita
    FILE *escrita = fopen(binario, "wb");
    if (verificar_abertura_arquivo(escrita) == 0) {
        fclose(leitura);
        return;
    }

    // Cabeçalho inicial, com status '0' até o fim da escrita
    indice_pops indice = {0};
    cabecalho_pops cabecalho;
    cabecalho.status = STATUS_INCONSISTENTE;
    cabecalho.topoLista = -1;
    cabecalho.proxByteOffset = CABECALHO_POPS;
    cabecalho.nroRegRem = 0;
    cabecalho.nroPoPs = 0;
    if (!escrever_cabecalho_pops(escrita, &cabecalho)) {
        encerrar_com_falha(leitura, escrita, &indice);
        return;
    }

    // Lê a primeira linha e localiza as colunas pelo nome
    char linha[TAM_LINHA_CSV];
    char *campos[MAX_COLUNAS_CSV];
    if (fgets(linha, sizeof(linha), leitura) == NULL) {
        encerrar_com_falha(leitura, escrita, &indice);
        return;
    }
    linha[strcspn(linha, "\r\n")] = '\0';
    int ncolunas = separar_campos(linha, campos, MAX_COLUNAS_CSV);

    int col_id = -1, col_nome = -1, col_pais = -1, col_sigla = -1;
    // Procura a posição de cada coluna exigida, pelo nome do cabeçalho do CSV
    for (int i = 0; i < ncolunas; i++) {
        if (strcmp(campos[i], "idPoPs") == 0)
            col_id = i;
        else if (strcmp(campos[i], "nomePoPs") == 0)
            col_nome = i;
        else if (strcmp(campos[i], "nomePais") == 0)
            col_pais = i;
        else if (strcmp(campos[i], "siglaPais") == 0)
            col_sigla = i;
    }

    // O CSV precisa ter as quatro colunas da especificação
    if (col_id < 0 || col_nome < 0 || col_pais < 0 || col_sigla < 0) {
        encerrar_com_falha(leitura, escrita, &indice);
        return;
    }

    // Cada linha de dados vira um registro. Linhas vazias são ignoradas.
    while (fgets(linha, sizeof(linha), leitura) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0')
            continue;

        ncolunas = separar_campos(linha, campos, MAX_COLUNAS_CSV);

        // A linha precisa ter pelo menos as colunas procuradas
        int maior_coluna = col_id;
        if (col_nome > maior_coluna) maior_coluna = col_nome;
        if (col_pais > maior_coluna) maior_coluna = col_pais;
        if (col_sigla > maior_coluna) maior_coluna = col_sigla;
        if (ncolunas <= maior_coluna) {
            encerrar_com_falha(leitura, escrita, &indice);
            return;
        }

        // Os caracteres '|' são reservados como delimitador dos campos variáveis
        if (strchr(campos[col_nome], '|') != NULL || strchr(campos[col_pais], '|') != NULL) {
            encerrar_com_falha(leitura, escrita, &indice);
            return;
        }

        // siglaPais tem 2 bytes. Vazio vira "$$" (nulo); caractere faltante vira '$'.
        char sigla[TAM_SIGLA_PAIS + 1];
        size_t tamanho_sigla = strlen(campos[col_sigla]);
        for (int i = 0; i < TAM_SIGLA_PAIS; i++)
            sigla[i] = (size_t)i < tamanho_sigla ? campos[col_sigla][i] : '$';
        sigla[TAM_SIGLA_PAIS] = '\0';

        dados_pops dado;
        dado.removido = REGISTRO_ATIVO;
        dado.encadeamentoLista = -1;
        dado.idPoPs = (int32_t)atoi(campos[col_id]);
        memcpy(dado.siglaPais, sigla, TAM_SIGLA_PAIS + 1);
        dado.nomePoPs = campos[col_nome];
        dado.nomePais = campos[col_pais];

        // Guarda os IDs já lidos para rejeitar uma chave repetida antes de gravar o registro.
        size_t entrada;
        if (buscar_indice_pops(&indice, dado.idPoPs, &entrada) ||
            !adicionar_indice_pops(&indice, dado.idPoPs, cabecalho.proxByteOffset)) {
            encerrar_com_falha(leitura, escrita, &indice);
            return;
        }

        // tamanhoRegistro é o tamanho total do registro, incluindo o controle
        if (!tamanho_dados_pops(&dado, &dado.tamanhoRegistro) ||
            !escrever_dados_pops(escrita, cabecalho.proxByteOffset, &dado)) {
            encerrar_com_falha(leitura, escrita, &indice);
            return;
        }

        cabecalho.proxByteOffset += dado.tamanhoRegistro;
        cabecalho.nroPoPs++;
    }

    // Arquivo completo: marca como consistente e regrava o cabeçalho no início
    cabecalho.status = STATUS_CONSISTENTE;
    if (!escrever_cabecalho_pops(escrita, &cabecalho) || fflush(escrita) != 0) {
        encerrar_com_falha(leitura, escrita, &indice);
        return;
    }

    free(indice.entradas);
    fclose(escrita);
    fclose(leitura);

    BinarioNaTela((char *)binario);
}

// Percorre os registros do arquivo de dados e preenche o vetor de entradas.
// Registros removidos são pulados. Retorna 1 em sucesso, 0 em erro.
static int coletar_entradas(FILE *dados, const cabecalho_pops *cabecalho,
                            entrada_indice_pops **entradas_saida, int *quantidade_saida) {
    int capacidade = cabecalho->nroPoPs > 0 ? cabecalho->nroPoPs : 1;
    int n = 0;
    entrada_indice_pops *entradas = malloc(capacidade * sizeof(entrada_indice_pops));
    if (entradas == NULL)
        return 0;

    // Os registros começam logo após o cabeçalho e terminam em proxByteOffset
    int64_t offset = CABECALHO_POPS;
    while (offset < cabecalho->proxByteOffset) {
        // Valida os campos de controle e deixa o ponteiro logo antes do idPoPs.
        dados_pops dado;
        if (!ler_controle_pops(dados, offset, cabecalho->proxByteOffset, &dado)) {
            free(entradas);
            return 0;
        }

        // Registro válido: lê o idPoPs e guarda a entrada no índice
        if (dado.removido == REGISTRO_ATIVO) {
            int32_t idPoPs;
            if (fread(&idPoPs, sizeof(idPoPs), 1, dados) != 1) {
                free(entradas);
                return 0;
            }

            // Aumenta o vetor se a estimativa inicial (nroPoPs) foi pequena
            if (n == capacidade) {
                capacidade *= 2;
                entrada_indice_pops *novo =
                    realloc(entradas, capacidade * sizeof(entrada_indice_pops));
                if (novo == NULL) {
                    free(entradas);
                    return 0;
                }
                entradas = novo;
            }

            entradas[n].idPoPs = idPoPs;
            entradas[n].byteoffset = offset;
            n++;
        }

        // Avança para o próximo registro usando o tamanho lido
        offset += dado.tamanhoRegistro;
    }

    // O último registro deve terminar exatamente em proxByteOffset
    if (offset != cabecalho->proxByteOffset) {
        free(entradas);
        return 0;
    }

    *entradas_saida = entradas;
    *quantidade_saida = n;
    return 1;
}

// Cria o arquivo de índice primário a partir do arquivo de dados PoPs.
// O índice só é criado depois que o arquivo de dados foi lido por completo.
void funcionalidade9(const char *binario_dados, const char *binario_indice) {
    // O arquivo de dados só é lido
    FILE *dados = fopen(binario_dados, "rb");
    if (verificar_abertura_arquivo(dados) == 0)
        return;

    // Só usa o arquivo de dados se o cabeçalho estiver consistente
    cabecalho_pops cabecalho;
    if (leitura_cabecalho_pops(dados, &cabecalho) == 0) {
        falha_processamento_arquivo(dados);
        return;
    }

    // Coleta as entradas dos registros válidos
    entrada_indice_pops *entradas = NULL;
    int quantidade = 0;
    if (coletar_entradas(dados, &cabecalho, &entradas, &quantidade) == 0) {
        falha_processamento_arquivo(dados);
        return;
    }
    fclose(dados);

    // O índice precisa estar ordenado por idPoPs
    qsort(entradas, quantidade, sizeof(entrada_indice_pops), comparar_indice_pops);

    // idPoPs é chave primária: dois registros válidos com o mesmo id tornam o arquivo inválido
    for (int i = 1; i < quantidade; i++) {
        if (entradas[i].idPoPs == entradas[i - 1].idPoPs) {
            free(entradas);
            falha_processamento_arquivo(NULL);
            return;
        }
    }

    // Grava o índice: status '0' durante a escrita, '1' ao final
    FILE *indice = fopen(binario_indice, "wb");
    if (indice == NULL) {
        free(entradas);
        verificar_abertura_arquivo(indice);
        return;
    }

    char status = STATUS_INCONSISTENTE;
    fwrite(&status, sizeof(char), 1, indice);

    for (int i = 0; i < quantidade; i++) {
        fwrite(&entradas[i].idPoPs, sizeof(int), 1, indice);
        fwrite(&entradas[i].byteoffset, sizeof(int64_t), 1, indice);
    }

    status = STATUS_CONSISTENTE;
    fseek(indice, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, indice);

    fclose(indice);
    free(entradas);

    BinarioNaTela((char *)binario_indice);
}
