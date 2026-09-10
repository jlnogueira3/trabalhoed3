#include <stdio.h>
#include <stdlib.h>
#include "../Headers/implementadas.h"



int main(void) {
    // Precisamos ler a opcão que vamos utilizar
    char opcao[100];
    char arg1[100];
    char arg2[100];
    registro_cabecalho cabecalho={
        cabecalho.status = '0',
        cabecalho.topo_pilha = -1,
        cabecalho.proxRRN = 0,
        cabecalho.nroRegRem = 0,
        cabecalho.nroPares = 0,
    };
    scanf("%99s %99s %99s", opcao,arg1,arg2);
    long int opcao_int;
    opcao_int = strtol(opcao,NULL,10);


// O próprio compilador reclamou no scanf do uso direto do "%d"
    // então decidi fazer a conversão usando o strtol para então levar ao
    // switch case


    switch (opcao_int) {
        case 1:
            /*Devemos chamar a função com os endereços dos argumentos
             utilizados, só lembrando que *&arg1 é equivalente a arg1,
             escrevi para lembrar de como fiz a passagem de argumentos
            */
            funcionalidade1(arg1,arg2,&cabecalho);
            break;
            return 0;
        case 2:
            /* Diferente da funcionalidade 1, a funcionalidade 2 recebe apenas o argumento
             * arg1 e o registro de cabecalho
             */
            funcionalidade2(arg1,&cabecalho);
            break;
            return 0;
        case 3:
            funcionalidade3(arg1, &cabecalho, arg2);
            break;
        case 4:
            //funcionalidade1(arg1,arg2);
        case 5:
            //funcionalidade1(arg1,arg2)
        case 6:
            //funcionalidade1(arg1,arg2)
        case 7:
            //funcionalidade1(arg1,arg2)

    }

}
