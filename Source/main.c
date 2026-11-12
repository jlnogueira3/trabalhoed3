/*
 * João L A Nogueira - N usp: 16986970
 */
#include <stdio.h>
#include <stdlib.h>
#include "../Headers/implementadas.h"

// Lê a opção da funcionalidade e os argumentos, e chama a funcionalidade correspondente
int main(void) {
    char opcao[100];
    char arg1[100];
    char arg2[100];

    // Lemos a opção e os dois primeiros argumentos da entrada padrão
    scanf("%99s %99s %99s", opcao, arg1, arg2);

    // A opção é lida como string e convertida para inteiro com strtol,
    // porque o scanf com "%d" não é usado aqui
    long int opcao_int = strtol(opcao, NULL, 10);

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
    }
    return 0;
}
