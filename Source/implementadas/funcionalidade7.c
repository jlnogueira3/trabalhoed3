//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

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
        criterios = cria_criterios(quantidade_criterios);

        if(criterios == NULL){
            falha_processamento_arquivo(f);
            return;
        }

        adiciona_criterios(criterios, quantidade_criterios);

        //quantidade de campos que serao atualizados
        int quantidade_atualizacoes;
        scanf("%d", &quantidade_atualizacoes);
        if(quantidade_atualizacoes <= 0){
            destroi_criterios(criterios);
            falha_processamento_arquivo(f);
            return;
        }

        struct chave_valor *atualizacoes;
        atualizacoes = cria_criterios(quantidade_atualizacoes);

        if(atualizacoes == NULL){
            destroi_criterios(criterios);
            falha_processamento_arquivo(f);
            return;
        }

        adiciona_criterios(atualizacoes, quantidade_atualizacoes);

        for(int rrn = 0; rrn < cabecalho.proxRRN; rrn++){
            long int pos = 17L + (long)rrn*18L;
            fseek(f, pos, SEEK_SET);

            if(leitura_dados(&dado, f) != 1){
                destroi_criterios(criterios);
                destroi_criterios(atualizacoes);
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

        destroi_criterios(criterios);
        destroi_criterios(atualizacoes);
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