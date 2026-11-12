//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#include <stdio.h>
#include <stdlib.h>

#include "../../Headers/implementadas.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_2.h"

//Aplica uma atualizacao e decide se o registro cabe no espaco que ja ocupava.
static int atualizar_registro_pops(FILE *f, cabecalho_pops *cabecalho, indice_pops *indice,
                                  int64_t pos, dados_pops *dado,
                                  const criterio_pops *atualizacoes, int quantidade){
    int32_t id_antigo = dado->idPoPs;
    size_t entrada, repetido;
    if(!buscar_indice_pops(indice, id_antigo, &entrada) || indice->entradas[entrada].byteoffset != pos ||
       !atualizar_campos_pops(dado, atualizacoes, quantidade))
        return 0;

    //Nao podemos trocar o id por um que pertence a outro registro ativo.
    if(buscar_indice_pops(indice, dado->idPoPs, &repetido) && repetido != entrada)
        return 0;
    int32_t necessario;
    if(!tamanho_dados_pops(dado, &necessario))
        return 0;
    if(necessario <= dado->tamanhoRegistro){
        //Mantemos o tamanho fisico, incluindo a sobra que ja existia antes.
        //A escrita preenche o restante com '$', sem mudar o byte offset.
        if(!escrever_dados_pops(f, pos, dado))
            return 0;
        indice->entradas[entrada].idPoPs = dado->idPoPs;
        return 1;
    }

    //O registro cresceu. Removemos a chave antiga e depois inserimos os novos
    //dados por Worst Fit, que tambem registra o novo id e offset no indice.
    int32_t id_novo = dado->idPoPs;
    dado->idPoPs = id_antigo;
    int ok = remover_registro_pops(f, cabecalho, indice, pos, dado);
    dado->idPoPs = id_novo;
    if(!ok)
        return 0;
    return inserir_registro_pops(f, cabecalho, indice, dado);
}

void funcionalidade14(const char *binario, const char *nome_indice, int n){
    FILE *f, *fi;
    cabecalho_pops cabecalho;
    indice_pops indice;
    int ok = abrir_arquivos_pops(binario, nome_indice, 1, &f, &fi, &cabecalho, &indice);
    if(ok)
        ok = iniciar_alteracoes_pops(f, fi);

    for(int i = 0; ok && i < n; i++){
        criterio_pops *criterios, *atualizacoes;
        int quantidade_criterios, quantidade_atualizacoes;
        if(!ler_criterios_pops(&criterios, &quantidade_criterios)){
            ok = 0;
            break;
        }
        if(!ler_criterios_pops(&atualizacoes, &quantidade_atualizacoes)){
            destroi_criterios_pops(criterios, quantidade_criterios);
            ok = 0;
            break;
        }

        //Buscamos antes de alterar para nao visitar novamente um registro realocado.
        //Na proxima solicitacao, a busca usa os dados e o indice ja atualizados.
        int64_t *posicoes;
        size_t encontrados;
        ok = buscar_registros_pops(f, &cabecalho, &indice, criterios, quantidade_criterios,
                                   &posicoes, &encontrados);
        destroi_criterios_pops(criterios, quantidade_criterios);
        for(size_t j = 0; ok && j < encontrados; j++){
            dados_pops dado;
            ok = leitura_dados_pops(f, posicoes[j], cabecalho.proxByteOffset, &dado);
            if(ok){
                ok = atualizar_registro_pops(f, &cabecalho, &indice, posicoes[j], &dado,
                                             atualizacoes, quantidade_atualizacoes);
                destroi_dados_pops(&dado);
            }
        }
        free(posicoes);
        destroi_criterios_pops(atualizacoes, quantidade_atualizacoes);
    }

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
