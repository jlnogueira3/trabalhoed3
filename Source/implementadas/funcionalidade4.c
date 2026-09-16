#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Headers/registros.h"
#include "../../Headers/fornecidas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"

//Acessa diretamente um registro por meio de seu RRN e o exibe caso ele
//exista e não esteja removido.

void funcionalidade4(const char *binario, int rrn){
    FILE *f;
    f = fopen(binario, "rb");

    // primeiro devemos verificar, assim como nas funcionalidades anteriores, a consistência do
    // arquivo
    if(verificar_abertura_arquivo(f) == 0)
        return;

    registro_cabecalho cabecalho;

    if(leitura_cabecalho(f, &cabecalho) == 0){
        falha_processamento_arquivo(f);
        return;
    }

    if(cabecalho.status != '1'){       //arquivo inconsistente nao pode ser consultado
        falha_processamento_arquivo(f);
        return;
    }

    if(0 > rrn || rrn >= cabecalho.proxRRN){    // 0 <= rrn < proxRRN
        printf("Registro inexistente.\n");
        fclose(f);
        return;
    }
    long int pos = 17L + (long)rrn*18L;
    fseek(f, pos, SEEK_SET);      //fseek retorna 0 quando funciona

    //LEITURA DO DADO
    registro_dados dado;
    if(leitura_dados(&dado, f) != 1){
        falha_processamento_arquivo(f);
        return;
    }

    //print do dado
    if(dado.removido == '1')
        printf("Registro inexistente.\n");
    else{
        print_dados(dado);
    }
    fclose(f);
}