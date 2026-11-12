/*
 * João L A Nogueira - N usp: 16986970
 * José Eduardo Constantino Nordemann - N usp: 16877738
 */
#include <stdio.h>
#include <stdlib.h>
#include "../Headers/implementadas.h"
#include "../Headers/auxiliares/auxiliar_2.h"

// Lê a opção da funcionalidade e os argumentos, e chama a funcionalidade correspondente
int main(void) {
    char opcao[100];
    char arg1[100];
    char arg2[100];

    // Lemos a opção e o primeiro argumento. A funcionalidade 2 recebe só o arquivo.
    if (scanf("%99s %99s", opcao, arg1) != 2) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // A opção é lida como string e convertida para inteiro com strtol,
    // porque o scanf com "%d" não é usado aqui
    long int opcao_int = strtol(opcao, NULL, 10);

    // As opções 2 e 10 recebem só o arquivo; as demais recebem um segundo argumento
    if (opcao_int != 2 && opcao_int != 10 && scanf("%99s", arg2) != 1) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // As funcionalidades 11 a 14 recebem a quantidade de solicitações pela entrada padrão
    int quantidade_pops = 0;
    if (opcao_int >= 11 && opcao_int <= 14 && !ler_quantidade_pops(&quantidade_pops)) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    switch (opcao_int) {
        case 1:
            funcionalidade1(arg1, arg2);
            break;
        case 2:
            funcionalidade2(arg1);
            break;
        case 3:
            funcionalidade3(arg1, arg2);
            break;
        case 4: {
            int rrn = (int)strtol(arg2, NULL, 10);
            funcionalidade4(arg1, rrn);
            break;
        }
        case 5: {
            int n = (int)strtol(arg2, NULL, 10);
            funcionalidade5(arg1, n);
            break;
        }
        case 6: {
            int n = (int)strtol(arg2, NULL, 10);
            funcionalidade6(arg1, n);
            break;
        }
        case 7: {
            int n = (int)strtol(arg2, NULL, 10);
            funcionalidade7(arg1, n);
            break;
        }
        case 8:
            funcionalidade8(arg1, arg2);
            break;
        case 9:
            // arg1 é o arquivo de dados PoPs e arg2 é o arquivo de índice a ser criado
            funcionalidade9(arg1, arg2);
            break;
        case 10:
            funcionalidade10(arg1);
            break;
        case 11:
            funcionalidade11(arg1, arg2, quantidade_pops);
            break;
        case 12:
            funcionalidade12(arg1, arg2, quantidade_pops);
            break;
        case 13:
            funcionalidade13(arg1, arg2, quantidade_pops);
            break;
        case 14:
            funcionalidade14(arg1, arg2, quantidade_pops);
            break;
    }
    return 0;
}
