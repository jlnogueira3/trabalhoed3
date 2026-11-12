//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#include <stdio.h>
#include <stdlib.h>

#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_2.h"

//Busca os PoPs que atendem aos criterios. Se tiver idPoPs entre os campos,
//usamos o indice; caso contrario, percorremos o arquivo sequencialmente.
void funcionalidade11(const char *binario, const char *nome_indice, int n){
    FILE *f, *fi;
    cabecalho_pops cabecalho;
    indice_pops indice;
    int ok = abrir_arquivos_pops(binario, nome_indice, 0, &f, &fi, &cabecalho, &indice);

    //Cada solicitacao tem seus proprios criterios, mesmo quando a anterior nao encontrou nada.
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

        //Uma linha em branco separa o resultado de cada solicacao da anterior.
        if(ok && i > 0)
            printf("\n");
        if(ok && encontrados == 0)
            printf("Registro inexistente.\n");
        for(size_t j = 0; ok && j < encontrados; j++){
            dados_pops dado;
            //A posicao ja veio da busca. Aqui so lemos o registro para imprimir seus campos.
            ok = leitura_dados_pops(f, posicoes[j], cabecalho.proxByteOffset, &dado);
            if(ok){
                print_dados_pops(&dado);
                destroi_dados_pops(&dado);
            }
        }
        free(posicoes);
    }
    if(!fechar_arquivos_pops(f, fi, &indice))
        ok = 0;
    if(!ok)
        printf("Falha no processamento do arquivo.\n");
}
