#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

//Realiza a atualização dos registros no arquivo binário que atendem aos
//critérios de busca, alterando os campos informados pelo usuário.

void funcionalidade7(const char *binario, int n){
    FILE *f;
    f = fopen(binario, "r+b");

    if(verificar_abertura_arquivo(f) == 0)
        return;

    //leitura do cabecalho
    registro_cabecalho cabecalho;
    if(leitura_cabecalho(f, &cabecalho) != 1){
        falha_processamento_arquivo(f);
        return;
    }

    if(cabecalho.status != '1'){       //arquivo inconsistente nao pode ser alterado
        falha_processamento_arquivo(f);
        return;
    }

    //0 no status do cabecalho pq estamos editando
    cabecalho.status = '0';
    fseek(f, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, f);

    registro_dados dado;    //variavel auxiliar
    for(int i = 0; i<n; i++){

        //quantidade de criterios desta solicitacao
        int quantidade_criterios;
        scanf("%d", &quantidade_criterios);
        if(quantidade_criterios <= 0){
            falha_processamento_arquivo(f);
            return;
        }

        struct chave_valor *criterios;
        criterios = calloc(quantidade_criterios, sizeof(struct chave_valor));

        for(int j = 0; j < quantidade_criterios; j++){
            scanf("%49s", criterios[j].chave);

            if(strcmp(criterios[j].chave, "unidadeMedida") == 0)
                ScanQuoteString(criterios[j].valor);
            else{
                scanf("%49s", criterios[j].valor);
                if(strcmp(criterios[j].valor, "NULO") == 0)
                    criterios[j].valor[0] = '\0';
            }
        }

        //quantidade de campos que serao atualizados
        int quantidade_atualizacoes;
        scanf("%d", &quantidade_atualizacoes);
        if(quantidade_atualizacoes <= 0){
            free(criterios);
            falha_processamento_arquivo(f);
            return;
        }

        struct chave_valor *atualizacoes;
        atualizacoes = calloc(quantidade_atualizacoes, sizeof(struct chave_valor));

        for(int j = 0; j < quantidade_atualizacoes; j++){
            scanf("%49s", atualizacoes[j].chave);

            if(strcmp(atualizacoes[j].chave, "unidadeMedida") == 0)
                ScanQuoteString(atualizacoes[j].valor);
            else{
                scanf("%49s", atualizacoes[j].valor);
                if(strcmp(atualizacoes[j].valor, "NULO") == 0)
                    atualizacoes[j].valor[0] = '\0';
            }
        }

        for(int rrn = 0; rrn < cabecalho.proxRRN; rrn++){
            long int pos = 17L + (long)rrn*18L;
            fseek(f, pos, SEEK_SET);

            if(leitura_dados(&dado, f) != 1){
                free(criterios);
                free(atualizacoes);
                falha_processamento_arquivo(f);
                return;
            }

            if(dado.removido == '1')    //se tiver sido removido, vai pro proximo
                continue;

            int tester = atende_criterios(&dado, criterios, quantidade_criterios);
            if(tester == 0)
                continue; // vai pro proximo registro

            //atualiza os campos informados
            for(int j = 0; j < quantidade_atualizacoes; j++){
                if(strcmp(atualizacoes[j].chave, "idPoPs") == 0){
                    if(atualizacoes[j].valor[0] == '\0')
                        dado.idPoPs = -1;
                    else
                        dado.idPoPs = (int)strtol(atualizacoes[j].valor, NULL, 10);
                }
                else if(strcmp(atualizacoes[j].chave, "idPoPsConectado") == 0){
                    if(atualizacoes[j].valor[0] == '\0')
                        dado.idPoPsConectado = -1;
                    else
                        dado.idPoPsConectado = (int)strtol(atualizacoes[j].valor, NULL, 10);
                }
                else if(strcmp(atualizacoes[j].chave, "velocidade") == 0){
                    if(atualizacoes[j].valor[0] == '\0')
                        dado.velocidade = -1;
                    else
                        dado.velocidade = (int)strtol(atualizacoes[j].valor, NULL, 10);
                }
                else if(strcmp(atualizacoes[j].chave, "unidadeMedida") == 0){
                    if(atualizacoes[j].valor[0] == '\0')
                        dado.unidadeMedida = '$';
                    else
                        dado.unidadeMedida = atualizacoes[j].valor[0];
                }
            }

            //volta pro inicio do registro e grava os dados atualizados
            fseek(f, pos, SEEK_SET);
            escrever_dados(dado, f);
        }

        free(criterios);
        free(atualizacoes);
    }

    //grava o cabecalho atualizado ainda com status 0
    fseek(f, 0, SEEK_SET);
    escrever_cabecalho(&cabecalho, f);
    fflush(f);

    //marca como consistente depois de gravar as alteracoes
    cabecalho.status = '1';
    fseek(f, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, f);
    fflush(f);

    fclose(f);
    BinarioNaTela((char*)binario);
}