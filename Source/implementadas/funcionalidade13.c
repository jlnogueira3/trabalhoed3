//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#include <stdio.h>

#include "../../Headers/implementadas.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_2.h"

//Insere os registros usando o maior espaco removido que comportar os dados.
//Quando nenhum servir, escreve no final do arquivo e atualiza o proximo offset.
void funcionalidade13(const char *binario, const char *nome_indice, int n){
    FILE *f, *fi;
    cabecalho_pops cabecalho;
    indice_pops indice;
    int ok = abrir_arquivos_pops(binario, nome_indice, 1, &f, &fi, &cabecalho, &indice);
    if(ok)
        ok = iniciar_alteracoes_pops(f, fi);

    for(int i = 0; ok && i < n; i++){
        dados_pops dado;
        //Aqui sim inicializamos os campos de controle, pois o registro e novo.
        //A leitura recebe idPoPs, nomePoPs, nomePais e siglaPais, nessa ordem.
        if(!ler_insercao_pops(&dado)){
            ok = 0;
            break;
        }
        ok = inserir_registro_pops(f, &cabecalho, &indice, &dado);
        destroi_dados_pops(&dado);
    }

    //O indice fica atualizado em memoria a cada insercao e e ordenado ao gravar.
    if(ok)
        ok = finalizar_alteracoes_pops(f, &fi, nome_indice, &cabecalho, &indice);
    if(!fechar_arquivos_pops(f, fi, &indice))
        ok = 0;
    if(!ok)
        printf("Falha no processamento do arquivo.\n");
    else{
        BinarioNaTela((char*)binario);
        BinarioNaTela((char*)nome_indice);
    }
}
