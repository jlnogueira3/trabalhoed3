#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

//Realiza a remoção lógica dos registros que atendem aos
//critérios informados, atualizando a pilha de removidos e o cabeçalho.
void funcionalidade5(const char *binario, int n){
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

    if(cabecalho.status != '1'){       //arquivo inconsistente nao pode ser consultado
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
        int quantidade;
        scanf("%d", &quantidade);
        if(quantidade <= 0){
            falha_processamento_arquivo(f);
            return;
        }

        struct chave_valor *criterios;
        criterios = calloc(quantidade, sizeof(struct chave_valor));

        for(int j =0; j < quantidade; j++){
            scanf("%49s", criterios[j].chave);

            if(strcmp(criterios[j].chave, "unidadeMedida") == 0)
                ScanQuoteString(criterios[j].valor);    //funcao dada
            else{
                scanf("%49s", criterios[j].valor);
                if(strcmp(criterios[j].valor, "NULO") == 0)
                    criterios[j].valor[0] = '\0';
            }
        }
        
        for(int rrn = 0; rrn < cabecalho.proxRRN; rrn++){
            long int pos = 17L +(long)rrn*18L;
            fseek(f, pos, SEEK_SET);
            if(leitura_dados(&dado, f) != 1){
                free(criterios);
                falha_processamento_arquivo(f);
                return;
            }
            if(dado.removido == '1')    //se tiver sido removido, vai pro proximo
                continue;
            int tester = atende_criterios(&dado, criterios, quantidade);
            if (tester == 0)
                continue; // vai pro proximo registro
            dado.removido = '1';
            dado.encadeamentoPilha = cabecalho.topo_pilha;

            //volta pro inicio do registro e grava os campos de controle
            fseek(f, pos, SEEK_SET);
            fwrite(&dado.removido, sizeof(char), 1, f);
            fwrite(&dado.encadeamentoPilha, sizeof(int), 1, f);

            //preenche os campos de dados com lixo
            char lixo = '$';
            for(int k = 0; k < 13; k++){
                fwrite(&lixo, sizeof(char), 1, f);
            }

            //atualiza a pilha e os contadores
            cabecalho.topo_pilha = rrn;
            cabecalho.nroRegRem++;
        }
        free(criterios);
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