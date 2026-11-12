//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#include <stdio.h>
#include <stdlib.h>
#include "../Headers/implementadas.h"
#include "../Headers/auxiliares/auxiliar_2.h"

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
    if(scanf("%99s %99s", opcao, arg1) != 2){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    long int opcao_int;
    opcao_int = strtol(opcao,NULL,10);

    //A listagem do trabalho 1 recebe so o arquivo. As outras opcoes
    //recebem tambem um terceiro argumento, que agora pode ser o nome do indice.
    if(opcao_int != 2 && scanf("%99s", arg2) != 1){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    int quantidade_pops = 0;
    if(opcao_int >= 11 && opcao_int <= 14 && !ler_quantidade_pops(&quantidade_pops)){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }


// O próprio compilador reclamou no scanf do uso direto do "%d"
    // então decidi fazer a conversão usando o strtol para então levar ao
    // switch case


    switch (opcao_int) {
        case 1:{
            /*Devemos chamar a função com os endereços dos argumentos
             utilizados, só lembrando que *&arg1 é equivalente a arg1,
             escrevi para lembrar de como fiz a passagem de argumentos
            */
            funcionalidade1(arg1,arg2,&cabecalho);
            break;
        }
        case 2:{
            /* Diferente da funcionalidade 1, a funcionalidade 2 recebe apenas o argumento
             * arg1 e o registro de cabecalho
             */
            funcionalidade2(arg1,&cabecalho);
            break;
        }
        case 3:{
            funcionalidade3(arg1, &cabecalho, arg2);
            break;
        }
        case 4:{
            int rrn = (int)strtol(arg2, NULL, 10);   //string -> long -> int (cast)
            funcionalidade4(arg1, rrn);
            break;
        }
        case 5:{
            int n = (int)strtol(arg2, NULL, 10);    //mesma coisa do case 4
            funcionalidade5(arg1, n);
            break;
        }
        case 6:{
            int n = (int)strtol(arg2, NULL, 10);
            funcionalidade6(arg1, n);
            break;
        }
        case 7: {
            int n = (int)strtol(arg2, NULL, 10);
            funcionalidade7(arg1, n);
            break;
        }
        case 11:{
            funcionalidade11(arg1, arg2, quantidade_pops);
            break;
        }
        case 12:{
            funcionalidade12(arg1, arg2, quantidade_pops);
            break;
        }
        case 13:{
            funcionalidade13(arg1, arg2, quantidade_pops);
            break;
        }
        case 14:{
            funcionalidade14(arg1, arg2, quantidade_pops);
            break;
        }
    }
}
