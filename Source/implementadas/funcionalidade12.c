//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#include <stdio.h>
#include <stdlib.h>

#include "../../Headers/implementadas.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_2.h"

//Remove logicamente os registros encontrados, colocando os espacos na lista
//de removidos e retirando as chaves correspondentes do indice.
void funcionalidade12(const char *binario, const char *nome_indice, int n){
    FILE *f, *fi;
    cabecalho_pops cabecalho;
    indice_pops indice;
    int ok = abrir_arquivos_pops(binario, nome_indice, 1, &f, &fi, &cabecalho, &indice);
    if(ok)
        ok = iniciar_alteracoes_pops(f, fi); //0 nos dois status pq vamos editar

    for(int i = 0; ok && i < n; i++){
        criterio_pops *criterios;
        int quantidade;
        if(!ler_criterios_pops(&criterios, &quantidade)){
            ok = 0;
            break;
        }
        int64_t *posicoes;
        size_t encontrados;
        ok = buscar_registros_pops(f, &cabecalho, &indice, criterios, quantidade, &posicoes, &encontrados);
        destroi_criterios_pops(criterios, quantidade);

        //A busca ja desconsiderou os removidos. Cada registro encontrado entra uma vez na lista.
        for(size_t j = 0; ok && j < encontrados; j++){
            dados_pops dado;
            ok = leitura_dados_pops(f, posicoes[j], cabecalho.proxByteOffset, &dado);
            if(ok){
                ok = remover_registro_pops(f, &cabecalho, &indice, posicoes[j], &dado);
                destroi_dados_pops(&dado);
            }
        }
        free(posicoes);
        //Se nao encontrou nenhum, seguimos para a proxima solicitacao sem imprimir mensagem.
    }

    if(ok)
        ok = finalizar_alteracoes_pops(f, &fi, nome_indice, &cabecalho, &indice);
    if(!fechar_arquivos_pops(f, fi, &indice))
        ok = 0;
    if(!ok)
        printf("Falha no processamento do arquivo.\n");
    else{
        //Os arquivos precisam estar fechados antes de chamar a funcao fornecida.
        BinarioNaTela((char*)binario);
        BinarioNaTela((char*)nome_indice);
    }
}
