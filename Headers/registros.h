//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#ifndef TRABALHOED3_REGISTROS_H
#define TRABALHOED3_REGISTROS_H
typedef struct registro_cabecalho {
    char status;
    int topo_pilha;
    int proxRRN;
    //sempre que for utilizar, inicializar proxRRN com valor 0
    int nroRegRem;
    //sempre que for utilizar, inicializar nroRegRem com valor 0
    int nroPares;
} registro_cabecalho; 
//Sempre que for inicializar: .proxRRN = 0; .nroRegRem = 0; .encadeamentoPilha = -1;

typedef struct chave_valor {
    char chave[50];
    char valor[50];
} chave_valor;


typedef struct registro_dados {
    int idPoPs;
    int idPoPsConectado;
    // idPoPs e idPoPsConectado não podem assumir valores nulos
    int velocidade;
    char unidadeMedida;
    char removido;
    int encadeamentoPilha;
    //encadeamentoPilha deve ser inicializado com valor -1
} registro_dados;
//Sempre que for inicializar: .encadeamentoPilha = -1;




#endif //TRABALHOED3_REGISTROS_H
