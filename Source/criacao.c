// José Eduardo Constantino Nordemann - N usp: 16877738
/*
 * João L A Nogueira - N usp: 16986970
 */

#include "../Headers/criacao.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Headers/fornecidas.h"
#include "../Headers/registros.h"
#include "../Headers/cabecalho.h"
#include "../Headers/registro.h"
#include "../Headers/erros.h"

// Lê os registros de um arquivo CSV e grava no arquivo binário,
// criando também o cabeçalho do arquivo.
void funcionalidade1(const char *csv, const char *binario) {
    // O CSV é texto, então é aberto em modo texto
    FILE *leitura = fopen(csv, "r");
    if (verificar_abertura_arquivo(leitura) == 0)
        return;

    // O arquivo binário é criado do zero e só recebe escrita
    FILE *escrita = fopen(binario, "wb");
    if (verificar_abertura_arquivo(escrita) == 0) {
        fclose(leitura);
        return;
    }

    // Cabeçalho do arquivo novo. O status fica '0' enquanto o arquivo
    // está sendo construído.
    registro_cabecalho cabecalho;
    inicializar_cabecalho(&cabecalho);
    escrever_cabecalho(&cabecalho, escrita);

    // A primeira linha do CSV contém apenas os nomes das colunas
    char linha[100];
    fgets(linha, sizeof(linha), leitura);

    // Cada linha do CSV gera um registro de dados
    while (fgets(linha, sizeof(linha), leitura) != NULL) {
        registro_dados dados;

        // Valores padrão para os campos de controle e para os campos
        // que podem estar vazios no CSV (velocidade e unidadeMedida)
        dados.removido = REGISTRO_ATIVO;
        dados.encadeamentoPilha = -1;
        dados.velocidade = VALOR_NULO_INTEIRO;
        dados.unidadeMedida = VALOR_NULO_CHAR;

        // Separa os campos trocando cada vírgula por '\0'.
        // Isso é feito manualmente porque velocidade e unidadeMedida podem estar vazios.
        char *campo1 = linha;
        char *virgula1 = strchr(campo1, ',');
        *virgula1 = '\0';
        char *campo2 = virgula1 + 1;
        char *virgula2 = strchr(campo2, ',');
        *virgula2 = '\0';
        char *campo3 = virgula2 + 1;
        char *virgula3 = strchr(campo3, ',');
        *virgula3 = '\0';
        char *campo4 = virgula3 + 1;

        // Remove a quebra de linha do último campo
        campo4[strcspn(campo4, "\r\n")] = '\0';

        dados.idPoPs = atoi(campo1);
        dados.idPoPsConectado = atoi(campo2);
        if (campo3[0] != '\0')
            dados.velocidade = atoi(campo3);
        if (campo4[0] != '\0')
            dados.unidadeMedida = campo4[0];

        // Grava o registro na posição atual do arquivo (campo a campo)
        escrever_dados(dados, escrita);

        // Cada registro ocupa um novo RRN.
        // O enunciado garante que os pares não se repetem: cada registro adiciona um par novo.
        cabecalho.proxRRN++;
        cabecalho.nroPares++;
    }

    // Arquivo completo: marca como consistente e regrava o cabeçalho no início
    cabecalho.status = STATUS_CONSISTENTE;
    salvar_cabecalho(escrita, &cabecalho);

    fclose(escrita);
    fclose(leitura);

    BinarioNaTela((char *)binario);
}
