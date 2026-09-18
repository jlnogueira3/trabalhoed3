#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

// Implementação das funções auxiliares

//Começamos com a função atualizar cabecalho

//Essa função inicializa os campos do cabeçalho com os valores
//necessários para começar a construção de um arquivo binário.
void inicializar_cabecalho(registro_cabecalho *cabecalho) {
    cabecalho->status = '0';
    cabecalho->topo_pilha = -1;
    cabecalho->proxRRN = 0;
    cabecalho->nroRegRem = 0;
    cabecalho->nroPares = 0;

    /*
         * Escrevemos inicialmente o cabeçalho com status '0',
         * pois o arquivo ainda está sendo construído
         */
}

//Essa função escreve todos os campos do cabeçalho no arquivo binário.
//O arquivo deve estar posicionado no local em que o cabeçalho será escrito.
void escrever_cabecalho(const registro_cabecalho *cabecalho,
                        FILE *arquivo) {

    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->topo_pilha, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->proxRRN, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroPares, sizeof(int), 1, arquivo);
}

// Agora partimos para as funções de registros de dados
// é importante que seja implementado sem um loop
// a fim de essas funções consigam rodar dentro e fora
// de um loop, de forma a facilitar a implementação

//Essa função escreve todos os campos de um registro de dados no arquivo binário.
//Os campos são escritos separadamente para manter o tamanho definido no trabalho.
void escrever_dados(registro_dados dados, FILE *arquivo) {
    fwrite(&dados.removido, sizeof(char), 1, arquivo);
    fwrite(&dados.encadeamentoPilha, sizeof(int), 1, arquivo);
    fwrite(&dados.idPoPs, sizeof(int), 1, arquivo);
    fwrite(&dados.idPoPsConectado, sizeof(int), 1, arquivo);
    fwrite(&dados.velocidade, sizeof(int), 1, arquivo);
    fwrite(&dados.unidadeMedida, sizeof(char), 1, arquivo);
}

//Essa função imprime os campos de um registro de dados.
//Quando um campo possui o valor usado para representar nulo, imprime NULO.
void print_dados(registro_dados dados) {
    if (dados.idPoPs == -1) {
        printf("NULO ");
    } else {
        printf("%d ", dados.idPoPs);
    }
    if (dados.idPoPsConectado == -1) {
        printf("NULO ");
    } else {
        printf("%d ", dados.idPoPsConectado);
    }
    if (dados.velocidade == -1) {
        printf("NULO ");
    } else {
        printf("%d ", dados.velocidade);
    }

    if (dados.unidadeMedida == '$') {
        printf("NULO\n");
    } else {
        printf("\"%c\"\n", dados.unidadeMedida);
    }
}

// Agora devemos implementar a leitura dos dados

//Essa função lê do arquivo todos os campos de um registro de dados.
//Retorna 1 quando todos os campos são lidos e 0 quando alguma leitura falha.
int leitura_dados(registro_dados *dados, FILE *arquivo) {
    if (fread(&dados->removido, sizeof(char), 1, arquivo) != 1)
        return 0;

    if (fread(&dados->encadeamentoPilha, sizeof(int), 1, arquivo) != 1)
        return 0;

    if (fread(&dados->idPoPs, sizeof(int), 1, arquivo) != 1)
        return 0;

    if (fread(&dados->idPoPsConectado, sizeof(int), 1, arquivo) != 1)
        return 0;

    if (fread(&dados->velocidade, sizeof(int), 1, arquivo) != 1)
        return 0;

    if (fread(&dados->unidadeMedida, sizeof(char), 1, arquivo) != 1)
        return 0;

    return 1;
}

//Essa função compara um registro com todos os pares de campo e valor recebidos.
//Retorna 1 quando o registro atende a todos os filtros e 0 quando algum não bate.
int filtrar_dados(struct chave_valor *p,
            int quantidade_pares,
            struct registro_dados dados,
            int filtro) {
    //Devemos passar campo a campo verificando
    //campo a campo fazendo  o cara-crachá da função
    for (int j = 0; j < quantidade_pares; j++) {

        if (strcmp(p[j].chave, "idPoPs") == 0) {

            int valor = (int) strtol(p[j].valor, NULL, 10);

            if (dados.idPoPs != valor) {
                filtro = 0;
                break;
            }
        }
        else if (strcmp(p[j].chave, "idPoPsConectado") == 0) {

            int valor = (int) strtol(p[j].valor, NULL, 10);

            if (dados.idPoPsConectado != valor) {
                filtro = 0;
                break;
            }
        }
        else if (strcmp(p[j].chave, "velocidade") == 0) {

            if (p[j].valor[0] == '\0') {

                if (dados.velocidade != -1) {
                    filtro = 0;
                    break;
                }
            }
            else {

                int valor = (int) strtol(p[j].valor, NULL, 10);

                if (dados.velocidade != valor) {
                    filtro = 0;
                    break;
                }
            }
        }

        else if (strcmp(p[j].chave, "unidadeMedida") == 0) {

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

//Essa função lê todos os campos do cabeçalho e guarda os valores na estrutura.
//Retorna 1 quando a leitura funciona e 0 quando algum campo não pode ser lido.
int leitura_cabecalho(FILE *f, registro_cabecalho *cabecalho) {
    if (fread(&cabecalho->status, sizeof(char), 1, f) != 1)
        return 0;

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

//Essa função verifica se um registro atende a todos os critérios informados.
//Ela também trata os valores nulos e retorna 0 quando encontra um campo desconhecido.
int atende_criterios(const registro_dados *dado, const struct chave_valor *criterios, int quantidade){
    int tester = 1;
    for(int j = 0; j < quantidade; j++){
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

        else if(strcmp(criterios[j].chave, "velocidade") == 0) {
            long int temp;
            if (criterios[j].valor[0] == '\0')
                temp = -1; // nulo
            else
                temp = strtol(criterios[j].valor, NULL, 10);
            if (dado->velocidade != temp)
                tester = 0;
        }
        else if(strcmp(criterios[j].chave, "unidadeMedida") == 0) {
            char temp;
            if(criterios[j].valor[0] == '\0')
                temp = '$'; // nulo
            else
                temp = criterios[j].valor[0];
            if(dado->unidadeMedida != temp)
                tester = 0;
        }
        else{
            tester = 0; // nome de campo desconhecido
        }
        if (tester == 0)
            break; // algum criterio deu pau, nn precisa verificar os outros
    }
    return tester;
}

//Essa função mostra a mensagem de falha no processamento e fecha o arquivo.
//Ela deve ser usada somente quando o arquivo já foi aberto.
void falha_processamento_arquivo(FILE *f){
    printf("Falha no processamento do arquivo.\n");
    fclose(f);
}

//Essa função verifica se o arquivo foi aberto corretamente.
//Retorna 1 quando o arquivo existe e 0 quando o ponteiro recebido é nulo.
int verificar_abertura_arquivo(FILE *f){
    if(f == NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    return 1;
}

//Essa função cria um vetor de critérios com a quantidade informada.
//Retorna o endereço do vetor criado ou NULL caso a alocação não funcione.
struct chave_valor *cria_criterios(int quantidade){
    struct chave_valor *criterios;
    criterios = calloc(quantidade, sizeof(struct chave_valor));

    return criterios;
}

//Essa função lê do teclado os campos e valores de todos os critérios.
//Também transforma o valor NULO em uma string vazia para facilitar as comparações.
void adiciona_criterios(struct chave_valor *criterios, int quantidade){
    for(int j = 0; j < quantidade; j++){
        scanf("%49s", criterios[j].chave);

        if(strcmp(criterios[j].chave, "unidadeMedida") == 0)
            ScanQuoteString(criterios[j].valor);
        else{
            scanf("%49s", criterios[j].valor);
            if(strcmp(criterios[j].valor, "NULO") == 0)
                criterios[j].valor[0] = '\0';
        }
    }
}

//Essa função libera a memória usada pelo vetor de critérios.
void destroi_criterios(struct chave_valor *criterios){
    free(criterios);
}