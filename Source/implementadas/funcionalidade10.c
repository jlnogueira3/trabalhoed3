/*
 * João L A Nogueira - N usp: 16986970
 * José Eduardo Constantino Nordemann - N usp: 16877738
 *
 * Funcionalidade 10 (trabalho 2): lista todos os PoPs do arquivo de dados,
 * percorrendo o arquivo sequencialmente e pulando os registros removidos.
 */

#include <stdio.h>

#include "../../Headers/implementadas.h"
#include "../../Headers/auxiliares/auxiliar_1.h"
#include "../../Headers/auxiliares/auxiliar_2.h"

//Percorre todos os registros do arquivo, do primeiro ao último byte válido.
void funcionalidade10(const char *binario){
    FILE *f = fopen(binario, "rb");
    if(!verificar_abertura_arquivo(f))
        return;

    cabecalho_pops cabecalho;
    if(!leitura_cabecalho_pops(f, &cabecalho)){
        falha_processamento_arquivo(f);
        return;
    }

    int encontrados = 0;
    int ok = 1;
    int64_t pos = CABECALHO_POPS;
    while(ok && pos < cabecalho.proxByteOffset){
        dados_pops dado;
        if(!leitura_dados_pops(f, pos, cabecalho.proxByteOffset, &dado)){
            ok = 0;
            break;
        }
        //Registros removidos não são exibidos, mas o tamanho dele ainda avança a leitura.
        if(dado.removido == '0'){
            print_dados_pops(&dado);
            encontrados++;
        }
        pos += dado.tamanhoRegistro;
        destroi_dados_pops(&dado);
    }

    if(ok && encontrados == 0)
        printf("Registro inexistente.\n");
    if(!ok)
        printf("Falha no processamento do arquivo.\n");
    fclose(f);
}
