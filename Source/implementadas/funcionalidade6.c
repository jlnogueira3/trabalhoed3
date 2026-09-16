#include <stdio.h>
#include <stdlib.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"


//Realiza a inserção de novos registros no arquivo binário, reutilizando
//posições de registros removidos quando possível.

void funcionalidade6(const char*binario, int n){
    FILE *f;
    f = fopen(binario, "r+b");

    if(verificar_abertura_arquivo(f) == 0)
        return;

    registro_cabecalho cabecalho;

    if(leitura_cabecalho(f, &cabecalho) != 1){
        falha_processamento_arquivo(f);
        return;
    }

    //se o status for diferente de 1, informa falha e fecha o arquivo
    if(cabecalho.status != '1'){
        falha_processamento_arquivo(f);
        return;
    }

    cabecalho.status = '0';     //0 no status do cabecalho pq estamos editando

    // volta ao byte zero e grava status 0
    fseek(f, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, f);
    fflush(f);

    for(int i = 0; i < n; i++){
        registro_dados dado;
        dado.removido = '0';
        dado.encadeamentoPilha = -1;
        dado.velocidade = -1;
        dado.unidadeMedida = '$';

        char velocidade[50];
        char unidadeMedida[50];

        //leitura dos dados que vao ser inseridos
        scanf("%d", &dado.idPoPs);
        scanf("%d", &dado.idPoPsConectado);
        ScanQuoteString(velocidade);
        ScanQuoteString(unidadeMedida);

        if(velocidade[0] != '\0')
            dado.velocidade = (int)strtol(velocidade, NULL, 10);

        if(unidadeMedida[0] != '\0')
            dado.unidadeMedida = unidadeMedida[0];

        if(cabecalho.topo_pilha != -1){
            //reutiliza o registro que esta no topo da pilha
            int rrn = cabecalho.topo_pilha;
            long int pos = 17L + (long)rrn*18L;
            registro_dados removido;

            fseek(f, pos, SEEK_SET);
            if(leitura_dados(&removido, f) != 1){
                falha_processamento_arquivo(f);
                return;
            }

            //atualiza o topo da pilha para o proximo registro removido
            cabecalho.topo_pilha = removido.encadeamentoPilha;
            cabecalho.nroRegRem--;

            fseek(f, pos, SEEK_SET);
            escrever_dados(dado, f);
        }
        else{
            //se a pilha estiver vazia, escreve no proximo RRN disponivel
            long int pos = 17L + (long)cabecalho.proxRRN*18L;
            fseek(f, pos, SEEK_SET);
            escrever_dados(dado, f);
            cabecalho.proxRRN++;
        }

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
    BinarioNaTela((char *)binario);
}