//João Lucas Almeida Nogueira ;         NUSP 16986970
//José Eduardo Constantino Nordemann ;  NUSP 16877738

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../../Headers/auxiliares/auxiliar_2.h"

//Controlamos o byte offset pelas leituras, sem precisar consultar ftell.
//No Windows, long pode ter 4 bytes. Dividimos o deslocamento para nao truncar o offset.
int posicionar_pops(FILE *f, int64_t pos){
    if(pos < 0 || fseek(f, 0, SEEK_SET) != 0)
        return 0;
    while(pos > LONG_MAX){
        if(fseek(f, LONG_MAX, SEEK_CUR) != 0)
            return 0;
        pos -= LONG_MAX;
    }
    return fseek(f, (long)pos, SEEK_CUR) == 0;
}

int leitura_cabecalho_pops(FILE *f, cabecalho_pops *cabecalho){
    //Primeiro lemos so o status. Se estiver inconsistente, nao lemos o restante.
    if(!posicionar_pops(f, 0) || fread(&cabecalho->status, 1, 1, f) != 1 ||
       cabecalho->status != '1')
        return 0;
    if(fread(&cabecalho->topoLista, 8, 1, f) != 1 ||
       fread(&cabecalho->proxByteOffset, 8, 1, f) != 1 ||
       fread(&cabecalho->nroRegRem, 4, 1, f) != 1 ||
       fread(&cabecalho->nroPoPs, 4, 1, f) != 1)
        return 0;
    return cabecalho->proxByteOffset >= CABECALHO_POPS && cabecalho->nroRegRem >= 0 &&
           cabecalho->nroPoPs >= 0 &&
           (cabecalho->topoLista == -1 || (cabecalho->topoLista >= CABECALHO_POPS &&
            cabecalho->topoLista < cabecalho->proxByteOffset));
}

//Gravamos campo a campo para nao levar o preenchimento da struct para o arquivo.
int escrever_cabecalho_pops(FILE *f, const cabecalho_pops *cabecalho){
    return posicionar_pops(f, 0) &&
           fwrite(&cabecalho->status, 1, 1, f) == 1 &&
           fwrite(&cabecalho->topoLista, 8, 1, f) == 1 &&
           fwrite(&cabecalho->proxByteOffset, 8, 1, f) == 1 &&
           fwrite(&cabecalho->nroRegRem, 4, 1, f) == 1 &&
           fwrite(&cabecalho->nroPoPs, 4, 1, f) == 1;
}

//Essa leitura tambem e usada ao percorrer a lista de removidos.
static int ler_controle_pops(FILE *f, int64_t pos, int64_t fim, dados_pops *dado){
    if(pos < CABECALHO_POPS || pos > fim || fim - pos < REGISTRO_POPS_MINIMO ||
       !posicionar_pops(f, pos))
        return 0;
    if(fread(&dado->removido, 1, 1, f) != 1 ||
       fread(&dado->tamanhoRegistro, 4, 1, f) != 1 ||
       fread(&dado->encadeamentoLista, 8, 1, f) != 1)
        return 0;
    return (dado->removido == '0' || dado->removido == '1') &&
           dado->tamanhoRegistro >= REGISTRO_POPS_MINIMO && dado->tamanhoRegistro <= fim - pos;
}

//O limite impede que uma string sem delimitador avance para o registro seguinte.
static char *ler_nome_pops(FILE *f, int32_t *restante){
    size_t tamanho = 0, capacidade = BLOCO_POPS;
    char *nome = malloc(capacidade);
    if(nome == NULL)
        return NULL;
    while(*restante > 0){
        int c = fgetc(f);
        (*restante)--;
        if(c == EOF)
            break;
        if(c == '|'){
            nome[tamanho] = '\0';
            return nome;
        }
        if(tamanho + 1 == capacidade){
            if(capacidade > SIZE_MAX - BLOCO_POPS)
                break;
            char *novo = realloc(nome, capacidade + BLOCO_POPS);
            if(novo == NULL)
                break;
            nome = novo;
            capacidade += BLOCO_POPS;
        }
        nome[tamanho++] = (char)c;
    }
    free(nome);
    return NULL;
}

void destroi_dados_pops(dados_pops *dado){
    free(dado->nomePoPs);
    free(dado->nomePais);
    dado->nomePoPs = NULL;
    dado->nomePais = NULL;
}

int leitura_dados_pops(FILE *f, int64_t pos, int64_t fim, dados_pops *dado){
    //Somente os ponteiros sao inicializados, para permitir a liberacao em caso de erro.
    dado->nomePoPs = NULL;
    dado->nomePais = NULL;
    if(!ler_controle_pops(f, pos, fim, dado))
        return 0;
    if(dado->removido == '1')
        return posicionar_pops(f, pos + dado->tamanhoRegistro); //pula o lixo sem ler os dados

    if(fread(&dado->idPoPs, 4, 1, f) != 1 || fread(dado->siglaPais, 1, 2, f) != 2)
        return 0;
    dado->siglaPais[2] = '\0';
    int32_t restante = dado->tamanhoRegistro - 19;
    dado->nomePoPs = ler_nome_pops(f, &restante);
    if(dado->nomePoPs != NULL)
        dado->nomePais = ler_nome_pops(f, &restante);
    if(dado->nomePoPs == NULL || dado->nomePais == NULL ||
       !posicionar_pops(f, pos + dado->tamanhoRegistro)){
        destroi_dados_pops(dado);
        return 0;
    }
    return 1;
}

int tamanho_dados_pops(const dados_pops *dado, int32_t *tamanho){
    size_t nome = strlen(dado->nomePoPs), pais = strlen(dado->nomePais);
    if(nome > INT32_MAX - REGISTRO_POPS_MINIMO ||
       pais > INT32_MAX - REGISTRO_POPS_MINIMO - nome)
        return 0;
    *tamanho = (int32_t)(REGISTRO_POPS_MINIMO + nome + pais);
    return 1;
}

static int escrever_lixo_pops(FILE *f, int32_t quantidade){
    char lixo = '$';
    for(int32_t i = 0; i < quantidade; i++){
        if(fwrite(&lixo, 1, 1, f) != 1)
            return 0;
    }
    return 1;
}

int escrever_dados_pops(FILE *f, int64_t pos, const dados_pops *dado){
    int32_t usado;
    if(!tamanho_dados_pops(dado, &usado) || usado > dado->tamanhoRegistro || !posicionar_pops(f, pos))
        return 0;
    char delimitador = '|';
    size_t nome = strlen(dado->nomePoPs), pais = strlen(dado->nomePais);
    if(fwrite(&dado->removido, 1, 1, f) != 1 ||
       fwrite(&dado->tamanhoRegistro, 4, 1, f) != 1 ||
       fwrite(&dado->encadeamentoLista, 8, 1, f) != 1 ||
       fwrite(&dado->idPoPs, 4, 1, f) != 1 || fwrite(dado->siglaPais, 1, 2, f) != 2 ||
       fwrite(dado->nomePoPs, 1, nome, f) != nome || fwrite(&delimitador, 1, 1, f) != 1 ||
       fwrite(dado->nomePais, 1, pais, f) != pais || fwrite(&delimitador, 1, 1, f) != 1)
        return 0;
    //Se reutilizamos um espaco maior, o tamanho fisico fica igual e a sobra recebe '$'.
    return escrever_lixo_pops(f, dado->tamanhoRegistro - usado);
}

void print_dados_pops(const dados_pops *dado){
    printf("%d %s %s %s\n", (int)dado->idPoPs,
           dado->nomePoPs[0] == '\0' ? "NULO" : dado->nomePoPs,
           dado->nomePais[0] == '\0' ? "NULO" : dado->nomePais,
           strcmp(dado->siglaPais, "$$") == 0 ? "NULO" : dado->siglaPais);
}

//Reservamos o indice em blocos fixos, tanto na leitura quanto nas insercoes.
static int adicionar_indice_pops(indice_pops *indice, int32_t id, int64_t pos){
    if(indice->quantidade == indice->capacidade){
        if(indice->capacidade > SIZE_MAX / sizeof(entrada_indice_pops) - BLOCO_POPS)
            return 0;
        size_t capacidade = indice->capacidade + BLOCO_POPS;
        entrada_indice_pops *novo = realloc(indice->entradas, capacidade * sizeof(entrada_indice_pops));
        if(novo == NULL)
            return 0;
        indice->entradas = novo;
        indice->capacidade = capacidade;
    }
    indice->entradas[indice->quantidade].idPoPs = id;
    indice->entradas[indice->quantidade].byteoffset = pos;
    indice->quantidade++;
    return 1;
}

int buscar_indice_pops(const indice_pops *indice, int32_t id, size_t *posicao){
    //A busca e linear no indice em memoria, sem implementar busca binaria.
    for(size_t i = 0; i < indice->quantidade; i++){
        if(indice->entradas[i].idPoPs == id){
            *posicao = i;
            return 1;
        }
    }
    return 0;
}

int abrir_arquivos_pops(const char *binario, const char *nome_indice, int editar,
                       FILE **f, FILE **fi, cabecalho_pops *cabecalho, indice_pops *indice){
    *f = NULL;
    *fi = NULL;
    indice->entradas = NULL;
    indice->quantidade = 0;
    indice->capacidade = 0;
    if(strcmp(binario, nome_indice) == 0)
        return 0;
    *f = fopen(binario, editar ? "r+b" : "rb");
    if(*f == NULL || !leitura_cabecalho_pops(*f, cabecalho))
        return 0;
    *fi = fopen(nome_indice, editar ? "r+b" : "rb");
    char status;
    if(*fi == NULL || fread(&status, 1, 1, *fi) != 1 || status != '1')
        return 0;

    //O arquivo de indice nao tem contador no cabecalho. Lemos ate o fim,
    //distinguindo fim normal de uma entrada incompleta ou erro de leitura.
    while(1){
        entrada_indice_pops entrada;
        size_t lidos = fread(&entrada.idPoPs, 1, 4, *fi);
        if(lidos == 0 && feof(*fi) && !ferror(*fi))
            break;
        if(lidos != 4 || fread(&entrada.byteoffset, 8, 1, *fi) != 1 || entrada.idPoPs == -1 ||
           entrada.byteoffset < CABECALHO_POPS || entrada.byteoffset >= cabecalho->proxByteOffset)
            return 0;
        if(indice->quantidade > 0 && entrada.idPoPs <= indice->entradas[indice->quantidade - 1].idPoPs)
            return 0;
        if(!adicionar_indice_pops(indice, entrada.idPoPs, entrada.byteoffset))
            return 0;
    }
    return indice->quantidade == (size_t)cabecalho->nroPoPs;
}

int fechar_arquivos_pops(FILE *f, FILE *fi, indice_pops *indice){
    int ok = 1;
    if(f != NULL && fclose(f) != 0)
        ok = 0;
    if(fi != NULL && fclose(fi) != 0)
        ok = 0;
    free(indice->entradas);
    return ok;
}

static int escrever_status_pops(FILE *f, char status){
    return posicionar_pops(f, 0) && fwrite(&status, 1, 1, f) == 1 && fflush(f) == 0;
}

int iniciar_alteracoes_pops(FILE *f, FILE *fi){
    //Os dois arquivos ficam inconsistentes antes da primeira alteracao.
    return escrever_status_pops(f, '0') && escrever_status_pops(fi, '0');
}

static int comparar_indice_pops(const void *a, const void *b){
    const entrada_indice_pops *primeiro = a, *segundo = b;
    return (primeiro->idPoPs > segundo->idPoPs) - (primeiro->idPoPs < segundo->idPoPs);
}

int finalizar_alteracoes_pops(FILE *f, FILE **fi, const char *nome_indice,
                             cabecalho_pops *cabecalho, indice_pops *indice){
    cabecalho->status = '0';
    if(!escrever_cabecalho_pops(f, cabecalho) || fflush(f) != 0)
        return 0;

    //Ordenamos tudo uma vez com a funcao da biblioteca. O wb tambem elimina
    //as entradas antigas que sobrariam no fim do arquivo apos uma remocao.
    if(indice->quantidade > 1)
        qsort(indice->entradas, indice->quantidade, sizeof(entrada_indice_pops), comparar_indice_pops);
    FILE *novo = freopen(nome_indice, "wb", *fi);
    *fi = novo;
    if(novo == NULL || !escrever_status_pops(*fi, '0'))
        return 0;
    for(size_t i = 0; i < indice->quantidade; i++){
        if(fwrite(&indice->entradas[i].idPoPs, 4, 1, *fi) != 1 ||
           fwrite(&indice->entradas[i].byteoffset, 8, 1, *fi) != 1)
            return 0;
    }
    if(fflush(*fi) != 0 || !escrever_status_pops(*fi, '1') || !escrever_status_pops(f, '1'))
        return 0;
    cabecalho->status = '1';
    return 1;
}

static int atende_criterios_pops(const dados_pops *dado, const criterio_pops *criterios, int quantidade){
    for(int i = 0; i < quantidade; i++){
        const char *chave = criterios[i].chave, *valor = criterios[i].valor;
        if(strcmp(chave, "idPoPs") == 0){
            if(dado->idPoPs != strtol(valor, NULL, 10))
                return 0;
        }
        else if(strcmp(chave, "nomePoPs") == 0){
            if(strcmp(dado->nomePoPs, valor) != 0)
                return 0;
        }
        else if(strcmp(chave, "nomePais") == 0){
            if(strcmp(dado->nomePais, valor) != 0)
                return 0;
        }
        else if(strcmp(chave, "siglaPais") == 0){
            if(strcmp(dado->siglaPais, valor[0] == '\0' ? "$$" : valor) != 0)
                return 0;
        }
        else
            return 0;
    }
    return 1;
}

int buscar_registros_pops(FILE *f, const cabecalho_pops *cabecalho, const indice_pops *indice,
                         const criterio_pops *criterios, int quantidade,
                         int64_t **posicoes, size_t *encontrados){
    *posicoes = NULL;
    *encontrados = 0;
    size_t capacidade = 0;
    int campo_id = -1;
    //Pre tratamento: procuramos idPoPs entre TODOS os criterios antes de ler registros.
    for(int i = 0; i < quantidade; i++){
        if(strcmp(criterios[i].chave, "idPoPs") == 0){
            campo_id = i;
            break;
        }
    }
    int64_t pos = CABECALHO_POPS;
    if(campo_id != -1){
        size_t entrada;
        int32_t id = (int32_t)strtol(criterios[campo_id].valor, NULL, 10);
        if(!buscar_indice_pops(indice, id, &entrada))
            return 1; //nao existe essa chave; nao precisamos percorrer os dados
        pos = indice->entradas[entrada].byteoffset;
    }

    //Guardamos somente os offsets encontrados. Isso evita atualizar duas vezes
    //um registro que foi movido para uma posicao mais adiante durante a funcionalidade 14.
    while(pos < cabecalho->proxByteOffset){
        dados_pops dado;
        if(!leitura_dados_pops(f, pos, cabecalho->proxByteOffset, &dado))
            goto falha;
        int64_t proximo = pos + dado.tamanhoRegistro;
        int atende = dado.removido == '0' && atende_criterios_pops(&dado, criterios, quantidade);
        destroi_dados_pops(&dado);
        if(atende){
            if(*encontrados == capacidade){
                if(capacidade > SIZE_MAX / sizeof(int64_t) - BLOCO_POPS)
                    goto falha;
                int64_t *novo = realloc(*posicoes, (capacidade + BLOCO_POPS) * sizeof(int64_t));
                if(novo == NULL)
                    goto falha;
                *posicoes = novo;
                capacidade += BLOCO_POPS;
            }
            (*posicoes)[(*encontrados)++] = pos;
        }
        if(campo_id != -1)
            break; //o id e unico, mas ainda verificamos os outros criterios
        pos = proximo;
    }
    return 1;

falha:
    free(*posicoes);
    *posicoes = NULL;
    *encontrados = 0;
    return 0;
}

int remover_registro_pops(FILE *f, cabecalho_pops *cabecalho, indice_pops *indice,
                         int64_t pos, const dados_pops *dado){
    size_t entrada;
    if(dado->removido != '0' || cabecalho->nroRegRem == INT32_MAX || cabecalho->nroPoPs <= 0 ||
       !buscar_indice_pops(indice, dado->idPoPs, &entrada) || indice->entradas[entrada].byteoffset != pos)
        return 0;

    //Colocamos o novo removido no inicio da lista, sem ordenar por tamanho.
    //A escolha do maior espaco sera feita percorrendo a lista na insercao.
    int64_t atual = cabecalho->topoLista;
    char marca = '1';
    if(!posicionar_pops(f, pos) || fwrite(&marca, 1, 1, f) != 1 ||
       fwrite(&dado->tamanhoRegistro, 4, 1, f) != 1 || fwrite(&atual, 8, 1, f) != 1 ||
       !escrever_lixo_pops(f, dado->tamanhoRegistro - 13))
        return 0;
    cabecalho->topoLista = pos;
    cabecalho->nroRegRem++;
    cabecalho->nroPoPs--;
    memmove(&indice->entradas[entrada], &indice->entradas[entrada + 1],
            (indice->quantidade - entrada - 1) * sizeof(entrada_indice_pops));
    indice->quantidade--;
    return 1;
}

int inserir_registro_pops(FILE *f, cabecalho_pops *cabecalho, indice_pops *indice, dados_pops *dado){
    size_t entrada;
    int32_t necessario;
    if(dado->idPoPs == -1 || cabecalho->nroPoPs == INT32_MAX ||
       buscar_indice_pops(indice, dado->idPoPs, &entrada) || !tamanho_dados_pops(dado, &necessario))
        return 0;
    int64_t pos = cabecalho->proxByteOffset;
    dado->tamanhoRegistro = necessario;
    dado->removido = '0';
    dado->encadeamentoLista = -1;

    //Worst Fit: percorremos TODOS os removidos e guardamos o maior que serve.
    //Em empate, mantemos o primeiro encontrado na lista (convencao adotada).
    int64_t atual = cabecalho->topoLista, anterior = -1;
    int64_t maior = -1, anterior_maior = -1, proximo_maior = -1;
    int32_t tamanho_maior = 0, visitados = 0;
    while(atual != -1){
        dados_pops removido;
        if(visitados++ >= cabecalho->nroRegRem ||
           !ler_controle_pops(f, atual, cabecalho->proxByteOffset, &removido) ||
           removido.removido != '1')
            return 0;
        if(removido.tamanhoRegistro >= necessario && removido.tamanhoRegistro > tamanho_maior){
            maior = atual;
            anterior_maior = anterior;
            proximo_maior = removido.encadeamentoLista;
            tamanho_maior = removido.tamanhoRegistro;
        }
        anterior = atual;
        atual = removido.encadeamentoLista;
    }
    if(visitados != cabecalho->nroRegRem)
        return 0;
    if(maior != -1){
        //Retiramos o escolhido da lista, preservando o encadeamento dos outros.
        pos = maior;
        dado->tamanhoRegistro = tamanho_maior;
        if(anterior_maior == -1)
            cabecalho->topoLista = proximo_maior;
        else{
            if(!posicionar_pops(f, anterior_maior + 5) || fwrite(&proximo_maior, 8, 1, f) != 1)
                return 0;
        }
        cabecalho->nroRegRem--;
    }
    if(pos == cabecalho->proxByteOffset){
        if(cabecalho->proxByteOffset > INT64_MAX - necessario)
            return 0;
        cabecalho->proxByteOffset += necessario;
    }
    if(!adicionar_indice_pops(indice, dado->idPoPs, pos) || !escrever_dados_pops(f, pos, dado))
        return 0;
    cabecalho->nroPoPs++;
    return 1;
}
